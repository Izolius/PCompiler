#include "PCompiler.h"
#include "TypeUtility.h"


void CCompiler::init()
{
	m_ErrList.reserve(ERR_MAX);
	m_Context = nullptr;
}

CCompiler::CCompiler()
{
	init();
}

CCompiler::~CCompiler()
{
}

void CCompiler::Compile(const string &Code)
{
	//m_Code = Code;
	m_ErrorManager.reset(new CErrorManager());
	setlocale(LC_ALL, "rus");
	m_Lexer.setCode(Code);
	m_Lexer.UpdateErrorManager(m_ErrorManager.get());
	try {
		nextToken();
		rule_program();
	}
	catch (exception ex) {
		cout << "Compile Error : " << ex.what() << '\n';
	}
	catch (...) {
		cout << "Compile Error." << endl;
	}
	if (m_ErrorManager->getErrors().size()) {
		cout << "Error list : " << endl;
		for (CError *err : m_ErrorManager->getErrors()) {
			cout << err->m_pos.m_line << ' ' << err->m_pos.m_pos << ' ' << err->ToString() << '\n';
		}
	}
	else {
		m_gen.buildFile("123.asm");
	}
	
}

void CCompiler::error(CError *Error)
{
	m_ErrorManager->addError(Error);
}

void CCompiler::nextToken()
{
	m_token = m_Lexer.nextToken();
	m_tokenpos = m_Lexer.getTokenPos();
}

void CCompiler::accept(EOperator expected)
{
	if (m_token->is(expected)) {
		nextToken();
	}
	else {
		CError *err = new CExpectedError(m_tokenpos, expected);
		error(err);
		throw new exception();
	}
}

void CCompiler::accept(initializer_list<EOperator> expected)
{
	if (m_token->is(expected)) {
		nextToken();
	}
	else {
		error(new CError(m_tokenpos));
	}
}

void CCompiler::accept(initializer_list<EOperator> expected, EOperator & res)
{
	if (m_token->is(expected, res)) {
		nextToken();
	}
	else {
		error(new CError(m_tokenpos));
	}
}

void CCompiler::acceptIdent()
{
	if (m_token->isIdent()) {
		nextToken();
	}
	else {
		error(new CError(m_tokenpos, ecIdentExpected));
	}
}

void CCompiler::accept(EVarType expected)
{
	if (m_token->is(expected)) {
		nextToken();
	}
	else {
		error(new CError(m_tokenpos));
	}
}

void CCompiler::rule_start(void(CCompiler::* rule)(), initializer_list<EOperator> stopwords)
{
	try
	{
		(this->*rule)();
	}
	catch (exception *ex) {
		while (!m_token->is(stopwords)) {
			nextToken();
		}
	}
}

void CCompiler::rule_program()
{
	openContext();
	openContext();
	if (m_token->is(programsy)) {
		nextToken();
		acceptIdent();
		accept(semicolon);
	}
	m_curfunc.push("main");
	m_gen.startProc(m_curfunc.top());
	rule_block();
	freeContextVarsMem(vector<CVarIdent*>());
	m_gen.endProc();
	closeContext();
	closeContext();
	accept(point);
}

void CCompiler::rule_block()
{
	rule_labelpart();
	rule_constpart();
	rule_start(&CCompiler::rule_typepart, { varsy,procsy,funcsy,beginsy });
	rule_start(&CCompiler::rule_varpart, { procsy,funcsy,beginsy });
	rule_procFuncPart();
	rule_statementPart();

}

void CCompiler::rule_labelpart()
{
	if (m_token->is(labelsy)) {
		rule_label();
		while (m_token->is(comma)) {
			nextToken();
			rule_label();
		}
	}
}

void CCompiler::rule_label()
{
	accept(vtInt);
}

void CCompiler::rule_constpart()
{
	if (m_token->is(constsy)) {
		do{
			rule_constdecl();
			accept(semicolon);
		} while (m_token->isIdent());
	}
}

void CCompiler::rule_constdecl()
{
	acceptIdent();
	accept(EOperator::equal);
	bool bNeg = false;
	if (m_token->is(EOperator::minus)) {
		bNeg = true;
		nextToken();
	}
	else if (m_token->is(EOperator::plus)) {
		nextToken();
	}
	accept((EVarType)(vtInt | vtReal));
}

void CCompiler::rule_typepart()
{
	if (m_token->is(typessy)) {
		nextToken();
		while (m_token->isIdent()) {
			string name = m_token->ToString();
			nextToken();
			accept(EOperator::equal);
			const CTypeIdent *type = rule_start<const CTypeIdent*>(&CCompiler::rule_type, { semicolon }, m_Context->getError());
			CTypeIdent *namedType = new CNamedTypeIdent(name, type);
			m_Context->add(namedType);
			accept(semicolon);
		}
	}
}

void CCompiler::rule_varpart()
{
	if (m_token->is(varsy)) {
		nextToken();
		do {
			for (CVarIdent *var : rule_start(&CCompiler::rule_varDeclaration, { semicolon }, vector<CVarIdent*>())) {
				m_Context->add(var);
				m_gen.pushToStack(var);
			}
			accept(semicolon);
		} while (m_token->isIdent());
	}
}

vector<CVarIdent*> CCompiler::rule_varDeclaration()
{
	vector<CVarIdent*> vars;
	auto addvar = [&]() {
		if (m_token->isIdent()) {
			CVarIdent *var = m_Context->findV(m_token->ToString(), false);
			if (var) {
				error(new CError(m_tokenpos, ecSecondlyDescribedIdent));
			}
			else {
				var = new CVarIdent(m_token->ToString(), nullptr);
				//m_Context->add(var);
				vars.push_back(var);
			}
		}
	};
	addvar();
	acceptIdent();
	while (m_token->is(comma)) {
		nextToken();
		addvar();
		acceptIdent();
	}
	accept(colon);
	const CTypeIdent *type = rule_type();
	if (type) {
		for (CVarIdent *var : vars) {
			var->m_type = type;
		}
	}
	return vars;
}

void CCompiler::rule_procFuncPart()
{
	EOperator op;
	while (m_token->is({ funcsy, procsy }, op)) {
		vector<CVarIdent*> fictivParams = rule_start(&CCompiler::rule_procFuncDecl, { semicolon }, vector<CVarIdent*>());
		openContext();
		for (CVarIdent *var : fictivParams) {
			m_Context->add(var);
		}
		size_t offset = m_gen.getStackSize();
		for (auto iter = fictivParams.rbegin(); iter != fictivParams.rend(); iter++) {
			(*iter)->setOffset(offset);
			offset -= (*iter)->type()->size();
		}
		accept(semicolon);
		rule_block();
		accept(semicolon);

		freeContextVarsMem(fictivParams);
		closeContext();

		m_gen.endProc();
		m_curfunc.pop();
		if (m_curfunc.size())
			m_gen.continueProc(m_curfunc.top());
	}
}

vector<CVarIdent*> CCompiler::rule_procFuncDecl()
{
	EOperator op = m_token->m_op;
	nextToken();
	if (CIdent *ident = m_Context->find(m_token->ToString(), false)) {
		error(new CError(m_tokenpos, ecSecondlyDescribedIdent));
	}
	string name = m_token->ToString();
	m_curfunc.push(name);
	m_gen.startProc(name);
	vector<const CTypeIdent*> params;
	vector<CVarIdent*> fictivParams;
	CTypeIdent *collableType;
	CTypedIdent *collable;
	nextToken();
	if (m_token->is(leftpar)) {
		do {
			nextToken();
			vector<CVarIdent*> curparams = rule_start(&CCompiler::rule_varDeclaration, {semicolon}, vector<CVarIdent*>());
			fictivParams.insert(fictivParams.end(), curparams.begin(), curparams.end());
			params.reserve(params.capacity() + curparams.size());
			for (CVarIdent * param : curparams) {
				params.push_back(param->type());
			}
		} while (m_token->is(semicolon));
		accept(rightpar);
	}
	if (op == funcsy) {
		const CTypeIdent* resType(nullptr);
		accept(colon);
		if (m_token->isIdent()) {
			resType = m_Context->findT(m_token->ToString());
			if (!resType) {
				error(new CError(m_tokenpos, ecUnknownName));
				resType = m_Context->getError();
			}
		}
		if (!resType) {
			error(new CError(m_tokenpos, ecTypeExpected));
			resType = m_Context->getError();
		}
		collableType = new CFuncTypeIdent(params, resType);
		collable = new CFuncIdent(name, collableType);
		nextToken();
	}
	else {
		collableType = new CProcTypeIdent(params);
		collable = new CProcIdent(name, collableType);
	}
	m_Context->add(collable);
	m_Context->add(collableType);
	return fictivParams;
}

void CCompiler::rule_statementPart()
{
	accept(beginsy);
	rule_start(&CCompiler::rule_statement, { semicolon, endsy });
	while (m_token->is(semicolon)) {
		nextToken();
		rule_start(&CCompiler::rule_statement, { semicolon, endsy });
	} 
	accept(endsy);
}

void CCompiler::rule_statement()
{
	if (m_token->isIdent()) {
		CIdent *ident = m_Context->find(m_token->ToString());
		if (ident == nullptr) {
			error(new CError(m_tokenpos, ecUnknownName));
			ident = new CVarIdent(m_token->ToString(), m_Context->getError());
			m_Context->add(ident);
		}
		if (ident->m_type == itVar) {
			const CTypeIdent *left, *right;
			CVarIdent* var = dynamic_cast<CVarIdent*>(ident);
			left = rule_start<const CTypeIdent*>(&CCompiler::rule_variable, { assign }, m_Context->getError(), var);
			accept(assign);
			right = rule_start<const CTypeIdent*>(&CCompiler::rule_expression, { semicolon }, m_Context->getError());
			if (!CTypeUtility::CompatableAssign(left, right))
			{
				error(new CError(m_tokenpos, ecIncompatableTypes));
			}
			CRegister reg = m_gen.popFromStack(right->size());
			CRegister varAddr = m_gen.popFromStack(m_gen.getAddrSize());

			m_gen.writeToStack(reg, varAddr);
			m_gen.freeReg(reg);
			m_gen.freeReg(varAddr);
			return;
		}
		if (ident->m_type == itProc) {
			const CParamedTypeIdent *type = type_cast<const CParamedTypeIdent *>(ident);
			rule_start(&CCompiler::rule_Paramed, { semicolon }, type);
			m_gen.EvalProc(ident->name());
			return;
		}
		error(new CError(m_tokenpos, ecWrondIdentType));
		return;
	}
	if (m_token->is(beginsy)) {
		nextToken();
		do{
			rule_statement();
		} while (m_token->is(semicolon));
		accept(endsy);
		return;
	}
	if (m_token->is(ifsy)) {
		accept(ifsy);
		const CTypeIdent *cond(nullptr);
		cond = rule_start<const CTypeIdent*>(&CCompiler::rule_expression, { thensy }, m_Context->getError());
		if (!cond->isT(ttBoolean)) {
			error(new CError(m_tokenpos, ecWrongExpressionType));
		}
		accept(thensy);
		rule_start(&CCompiler::rule_statement, { semicolon, elsesy, endsy });
		if (m_token->is(elsesy)) {
			rule_start(&CCompiler::rule_statement, { semicolon, endsy });
		}
		return;
	}
	if (m_token->is(whilesy)) {
		nextToken();
		const CTypeIdent *cond(nullptr);
		cond = rule_start<const CTypeIdent*>(&CCompiler::rule_expression, { dosy }, m_Context->getError());
		if (!cond->isT(ttBoolean)) {
			error(new CError(m_tokenpos, ecWrongExpressionType));
		}
		accept(dosy);
		rule_start(&CCompiler::rule_statement, { semicolon, endsy });
		return;
	}
	if (m_token->is(forsy)) {
		nextToken();
		if (!m_token->isIdent())
		{
			error(new CError(m_tokenpos, ecIdentExpected));
		}
		CIdent *ident = m_Context->find(m_token->ToString());
		string identName;
		if (!ident || !ident->type()->isOrdered()) {
			error(new CError(m_tokenpos, ecWrongForIteratorType));
			identName = m_token->ToString();
		}
		nextToken();
		accept(assign);
		const CTypeIdent *from, *to;
		from = rule_expression();
		accept({ downtosy,tosy });
		to = rule_expression();
		if (!ident) {
			ident = new CVarIdent(identName, from);
			m_Context->add(ident);
		}
		if (!CTypeUtility::CompatableAssign(ident->type(),from) 
			|| !CTypeUtility::CompatableAssign(ident->type(), to)) {
			error(new CError(m_tokenpos, ecIncompatableTypes));
		}
		accept(dosy);
		rule_start(&CCompiler::rule_statement, { semicolon, endsy });
	}
}

const CTypeIdent *CCompiler::rule_type()
{
	const CTypeIdent *res = rule_simpleType();
	if (!res->isT(ttError))
		return res;
	CTypeIdent *resType;
	if (m_token->is(arraysy)) {
		vector<const CTypeIdent *> types;
		nextToken();
		accept(lbracket);
		do {
			const CTypeIdent *type = rule_simpleType();
			types.push_back(type);
			//nextToken();
		} while (m_token->is(comma));
		accept(rbracket);
		accept(ofsy);
		const CTypeIdent *base = rule_start<const CTypeIdent*>(&CCompiler::rule_type, { semicolon }, m_Context->getError());
		resType = new CArrayTypeIdent(types, base);
		m_Context->add(resType);
		return resType;
	}
	//acceptIdent();
	error(new CError(m_tokenpos, ecTypeExpected));
	return m_Context->getError();
}

const CTypeIdent * CCompiler::rule_simpleType()
{
	const CTypeIdent *res(m_Context->getError());
	CTypeIdent *resType(nullptr);
	if (m_token->isIdent()) {
		if (const CIdent * ident = m_Context->find(m_token->ToString())) {
			if (ident->m_type == itType) {
				nextToken();
				return type_cast<const CTypeIdent*>(ident);
			}
				
			if (ident->m_type == itEnumConst) {
				const CEnumConstIdent *from = dynamic_cast<const CEnumConstIdent *>(ident);
				const CEnumConstIdent *to(nullptr);
				nextToken();
				accept(twopoints);
				if (m_token->isIdent()) {
					if (to = m_Context->findEC(m_token->ToString())) {
						resType = new CEnumLimitedTypeIdent(from, to, from->type());
						m_Context->add(resType);
						nextToken();
						return resType;
					}
				}
			}
		}
		res = m_Context->findT(m_token->ToString());
		if (res == nullptr) {
			error(new CError(m_tokenpos, ecUnknownName));
			res = m_Context->getError();
		}
		nextToken();
		return res;
	}
	if (m_token->is({vtChar, vtInt})) {
		CVariant *var = m_token->m_val;
		switch (var->T)
		{
		case vtChar: {
			unsigned char from, to;
			CCharVariant *val = dynamic_cast<CCharVariant *>(var);
			from = val->m_val;
			nextToken();
			accept(twopoints);
			if (!m_token->is(vtChar))
				error(new CExpectedError(m_tokenpos, vtChar));
			val = dynamic_cast<CCharVariant *>(m_token->m_val);
			if (val) {
				to = val->m_val;
				resType = new CCharLimitedTypeIdent(from, to, m_Context->getChar());
				m_Context->add(resType);
				nextToken();
				return resType;
			}
			return m_Context->getError();
		}
		case vtInt: {
			int from, to;
			CIntVariant *val = dynamic_cast<CIntVariant *>(var);
			from = val->m_val;
			nextToken();
			accept(twopoints);
			if (!m_token->is(vtInt))
				error(new CExpectedError(m_tokenpos, vtInt));
			val = dynamic_cast<CIntVariant *>(m_token->m_val);
			if (val) {
				to = val->m_val;
				resType = new CIntLimitedTypeIdent(from, to, m_Context->getInteger());
				nextToken();
				return resType;
			}
			return m_Context->getError();
		}
		default:
			break;
		}
	}
	if (m_token->is(leftpar)) {
		vector<CEnumConstIdent *> idents;
		do {
			nextToken();
			if (m_token->isIdent()) {
				idents.push_back(new CEnumConstIdent(m_token->ToString()));
			}
			else {
				error(new CError(m_tokenpos, ecIdentExpected));
			}
			nextToken();
		} while (m_token->is(comma));
		resType = new CEnumTypeIdent(idents);
		for (CEnumConstIdent *ident : idents) {
			m_Context->add(ident);
		}
		m_Context->add(resType);
		accept(rightpar);
		return resType;
	}
	return m_Context->getError();
}

const CTypeIdent *CCompiler::rule_expression()
{
	const CTypeIdent *left, *right;
	EOperator op;
	left = rule_simpleExpression();
	if (m_token->is({ EOperator::equal, later, EOperator::greater,
		latergrater, laterequal, greaterequal }, op)) {
		CTextPosition oppos = m_tokenpos;
		nextToken();
		right = rule_simpleExpression();
		const CTypeIdent *res = CTypeUtility::Result(left, op, right, m_Context);
		if (res->isT(ttError)) {
			error(new CError(oppos, ecIncompatableTypes));
		}
	}
	return left;
}

const CTypeIdent *CCompiler::rule_simpleExpression()
{
	if (m_token->is(EOperator::plus)) {
		nextToken();
	}
	else if (m_token->is(EOperator::minus)) {
		nextToken();
	}
	const CTypeIdent *left, *right;
	EOperator oper;
	left = rule_term();
	bool op;
	do {
		op = false;
		if (m_token->is({ EOperator::plus, EOperator::minus, orsy }, oper)) {
			nextToken();
			right = rule_term();
			op = true;
			left = CTypeUtility::Result(left, oper, right, m_Context);
			if (left->isT(ttError)) {
				EErrorCode code;
				if (oper == orsy)
					code = ecWrongAndNotOrOperands;
				else
					code = ecWrongPlusMinusOperandsTypes;
				error(new CError(m_tokenpos, code));
			}
			CRegister rightreg = m_gen.popFromStack(right->size());
			CRegister leftreg = m_gen.popFromStack(left->size());
			m_gen.Eval(leftreg, rightreg, oper);
			m_gen.pushToStack(leftreg);
			m_gen.freeReg(leftreg);
			m_gen.freeReg(rightreg);
		}
	} while (op);
	return left;
}

const CTypeIdent *CCompiler::rule_term()
{
	const CTypeIdent *left, *right;
	left = rule_factor();
	bool op;
	EOperator oper;
	do {
		op = false;
		if (m_token->is({ star, slash, divsy, modsy, andsy }, oper)) {
			nextToken();
			right = rule_factor();
			left = CTypeUtility::Result(left, oper, right, m_Context);
			if (left->isT(ttError)) {
				EErrorCode code;
				switch (oper)
				{
				case star:
					code = ecWrongMultOperandsTypes;
					break;
				case slash:
					code = ecWrongDivOperandsTypes;
					break;
				case divsy:
				case modsy:
					code = ecWrongDivModOperandsTypes;
					break;
				default://andsy
					code = ecWrongAndNotOrOperands;
					break;
				}
				error(new CError(m_tokenpos, code));
			}
			CRegister rightreg = m_gen.popFromStack(right->size());
			CRegister leftreg = m_gen.popFromStack(left->size());
			m_gen.Eval(leftreg, rightreg, oper);
			m_gen.pushToStack(leftreg);
			m_gen.freeReg(leftreg);
			m_gen.freeReg(rightreg);
			op = true;
		}
	} while (op);
	return left;
}

const CTypeIdent *CCompiler::rule_factor()
{
	const CTypeIdent *left(m_Context->getError()), *right(nullptr);
	EVarType vartype;
	if (m_token->is({ vtInt , vtReal, vtChar }, vartype)) {
		switch (vartype)
		{
		case vtInt:
			left = m_Context->getInteger();
			m_gen.pushIntToStack(m_token->m_val->ToString());
			break;
		case vtReal:
			left = m_Context->getReal();
			m_gen.pushFloatToStack(m_token->m_val->ToString());
			break;
		case vtChar:
			left = m_Context->getChar();
		default:
			break;
		}
		nextToken();
		return left;
	}
	if (m_token->is(leftpar)) {
		nextToken();
		left = rule_expression();
		accept(rightpar);
		return left;
	}
	if (m_token->is(notsy)) {
		nextToken();
		left = rule_factor();
		left = CTypeUtility::Result(left, notsy, nullptr, m_Context);
		if (left->isT(ttError)) {
			error(new CError(m_tokenpos, ecWrongAndNotOrOperands));
		}
		return left;
	}
	if (m_token->isIdent()) {
		CIdent *ident = m_Context->find(m_token->ToString());
		if (!ident) {
			error(new CError(m_tokenpos, ecUnknownName));
			nextToken();
			return m_Context->getError();
		}
		if (ident->m_type == itVar) {
			CVarIdent* var = dynamic_cast<CVarIdent*>(ident);
			const CTypeIdent *res = rule_start<const CTypeIdent*>(&CCompiler::rule_variable, { rightpar,semicolon,star, slash, divsy, modsy, andsy ,
				EOperator::plus, EOperator::minus, orsy ,EOperator::equal, later, EOperator::greater,
				latergrater, laterequal, greaterequal }, m_Context->getError(), var);
			CRegister reg = m_gen.popFromStack(m_gen.getAddrSize());
			reg.setByVal(true);
			m_gen.pushToStack(reg);
			m_gen.freeReg(reg);
			return res;
		}
		if (ident->m_type == itConst || ident->m_type == itEnumConst) {
			left = ident->type();
			nextToken();
			return left;
		}
		if (ident->m_type == itFunc) {
			const CFuncTypeIdent *func = type_cast<const CFuncTypeIdent*>(ident);
			left = func->resType();
			rule_Paramed(func);
			m_gen.EvalProc(ident->name());
			return left;
		}
		error(new CError(m_tokenpos, ecWrongNameUsing));
		return m_Context->getError();
	}
	error(new CError(m_tokenpos));
	return m_Context->getError();
}

const CTypeIdent * CCompiler::rule_arrayVar(const CTypeIdent *vartype)
{
	size_t i = 0;
	const CArrayTypeIdent *array = type_cast<const CArrayTypeIdent *>(vartype);
	if (!array) {
		CArrayTypeIdent *tarray(nullptr);
		tarray = new CArrayTypeIdent({}, m_Context->getError());
		error(new CError(m_tokenpos, ecVarIsNotArray));
		m_Context->add(tarray);
		array = tarray;
	}
	const vector<const CTypeIdent *> &indexes = array->indexes();
	do {
		const CTypeIdent *type = rule_expression();
		if (i < indexes.size() && !CTypeUtility::Compatable(type, indexes[i])) {
			error(new CError(m_tokenpos, ecWrongIndexType));
		}
		i++;
	} while (m_token->is(comma));
	if (i != indexes.size()) {
		error(new CError(m_tokenpos, ecWrongIndexesCount));
	}
	return array->base();
}

const CTypeIdent * CCompiler::rule_variable(CVarIdent *variable)
{
	if (!variable) {
		error(new CError(m_tokenpos));
		return m_Context->getError();
	}
	const CTypeIdent *type = variable->type();
	nextToken();
	while (m_token->is(lbracket)) {
		nextToken();
		type = rule_arrayVar(type);
		accept(rbracket);
	}
	//положим в стек адрес, куда где лежит переменная
	if (auto arrtype = type_cast<const CArrayTypeIdent*>(type)) {

	}
	else {
		CRegister addr = m_gen.readAddr(variable, m_Context->deep(variable->name()));
		m_gen.pushToStack(addr);
		m_gen.freeReg(addr);
	}
	return type;
}

void CCompiler::rule_Paramed(const CParamedTypeIdent *type)
{
	const vector<const CTypeIdent*> &params = type->params();
	const CTypeIdent* curtype(nullptr);
	size_t i = 0;
	nextToken();
	if (m_token->is(leftpar)) {
		do {
			nextToken();
			curtype = rule_expression();
			if (i < params.size() && !CTypeUtility::CompatableAssign(params[i], curtype)) {
				error(new CError(m_tokenpos, ecWrongParamType));
			}
			i++;
		} while (m_token->is(comma));
		if (i != params.size()) {
			error(new CError(m_tokenpos, ecWrongParamsCount));
		}
		accept(rightpar);
	}
}

void CCompiler::openContext()
{
	if (m_Context) {
		CContext *cur = new CContext(m_Context);
		m_Context = cur;
	}
	else {
		m_Context = new CContext();
	}
}

void CCompiler::closeContext()
{
	CContext *parent = m_Context->parent();
	delete m_Context;
	m_Context = parent;
}

void CCompiler::freeContextVarsMem(const vector<CVarIdent*> &ContextParams)
{
	size_t bytes(0);
	for (CVarIdent *var : m_Context->getLocalVars()) {
		bytes += var->type()->size();
	}
	size_t paramsSize(0);
	for (CVarIdent *var : ContextParams) {
		paramsSize += var->type()->size();
	}
	bytes -= paramsSize;
	m_gen.freeStackSpace(bytes);
	m_gen.restoreStackStart();
	m_gen.freeStackSpace(paramsSize);
}
