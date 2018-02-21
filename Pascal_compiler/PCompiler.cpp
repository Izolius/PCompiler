#include "PCompiler.h"
#include "TypeUtility.h"


void PCompiler::init()
{
	m_ErrList.reserve(ERR_MAX);
	m_Context = nullptr;
}

PCompiler::PCompiler()
{
	init();
}

PCompiler::~PCompiler()
{
}

void PCompiler::Compile(const string &Code)
{
	//m_Code = Code;
	m_ErrorManager.reset(new CErrorManager());
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
	
	cout << "Error list : " << endl;
	for (CError *err : m_ErrorManager->getErrors()) {
		cout << err->m_pos.m_line << ' ' << err->m_pos.m_pos << endl;
	}
}

void PCompiler::error(CError *Error)
{
	m_ErrorManager->addError(Error);
}

void PCompiler::nextToken()
{
	m_token = m_Lexer.nextToken();
	m_tokenpos = m_Lexer.getTokenPos();
}

void PCompiler::accept(EOperator expected)
{
	if (m_token->is(expected)) {
		nextToken();
	}
	else {
		error(new CError(m_tokenpos));
	}
}

void PCompiler::accept(initializer_list<EOperator> expected)
{
	if (m_token->is(expected)) {
		nextToken();
	}
	else {
		error(new CError(m_tokenpos));
	}
}

void PCompiler::accept(initializer_list<EOperator> expected, EOperator & res)
{
	if (m_token->is(expected, res)) {
		nextToken();
	}
	else {
		error(new CError(m_tokenpos));
	}
}

void PCompiler::acceptIdent()
{
	if (m_token->isIdent()) {
		nextToken();
	}
	else {
		error(new CError(m_tokenpos));
	}
}

void PCompiler::accept(EVarType expected)
{
	if (m_token->is(expected)) {
		nextToken();
	}
	else {
		error(new CError(m_tokenpos));
	}
}

void PCompiler::rule_program()
{
	openContext();
	if (m_token->is(propgramsy)) {
		nextToken();
		acceptIdent();
		accept(semicolon);
	}
	rule_block();
	closeContext();
	accept(point);
}

void PCompiler::rule_block()
{
	openContext();
	rule_labelpart();
	rule_constpart();
	rule_typepart();
	rule_varpart();
	rule_procFuncPart();
	rule_statementPart();
	closeContext();
}

void PCompiler::rule_labelpart()
{
	if (m_token->is(labelsy)) {
		rule_label();
		while (m_token->is(comma)) {
			nextToken();
			rule_label();
		}
	}
}

void PCompiler::rule_label()
{
	accept(vtInt);
}

void PCompiler::rule_constpart()
{
	if (m_token->is(constsy)) {
		do{
			rule_constdecl();
			accept(semicolon);
		} while (m_token->isIdent());
	}
}

void PCompiler::rule_constdecl()
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

void PCompiler::rule_typepart()
{
	if (m_token->is(typesy)) {
		nextToken();
		while (m_token->isIdent()) {
			string name = m_token->ToString();
			nextToken();
			accept(EOperator::equal);
			const CTypeIdent *type = rule_type();
			CTypeIdent *namedType = new CNamedTypeIdent(name, type);
			m_Context->add(namedType);
			accept(semicolon);
		}
	}
}

void PCompiler::rule_varpart()
{
	if (m_token->is(varsy)) {
		nextToken();
		do {
			for (CVarIdent *var : rule_varDeclaration()) {
				m_Context->add(var);
			}
			accept(semicolon);
		} while (m_token->isIdent());
	}
}

vector<CVarIdent*> PCompiler::rule_varDeclaration()
{
	vector<CVarIdent*> vars;
	auto addvar = [&]() {
		if (m_token->isIdent()) {
			CVarIdent *var = m_Context->findV(m_token->ToString());
			if (var) {
				error(new CError(m_tokenpos));
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

void PCompiler::rule_procFuncPart()
{
	EOperator op;
	while (m_token->is({ funcsy, procsy }, op)) {
		nextToken();
		if (CIdent *ident = m_Context->find(m_token->ToString(), false)) {
			error(new CError(m_tokenpos));
		}
		string name = m_token->ToString();
		vector<const CTypeIdent*> params;
		vector<CVarIdent*> fictivParams;
		const CTypeIdent* resType(nullptr);
		CTypeIdent *collableType;
		CTypedIdent *collable;
		nextToken();
		if (m_token->is(leftpar)) {
			do {
				nextToken();
				vector<CVarIdent*> curparams = rule_varDeclaration();
				fictivParams.insert(fictivParams.end(), curparams.begin(), curparams.end());
				params.reserve(params.capacity() + curparams.size());
				for (CVarIdent * param: curparams) {
					params.push_back(param->type());
				}
			} while (m_token->is(semicolon));
			accept(rightpar);
		}
		if (op == funcsy) {
			accept(colon);
			if (m_token->isIdent()) {
				resType = m_Context->findT(m_token->ToString());
			}
			if (!resType) {
				error(new CError(m_tokenpos));
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
		openContext();
		for (CVarIdent *var : fictivParams) {
			m_Context->add(var);
		}
		accept(semicolon);
		rule_block();
		accept(semicolon);
		closeContext();
	}
}

void PCompiler::rule_statementPart()
{
	accept(beginsy);
	rule_statement();
	while (m_token->is(semicolon)) {
		nextToken();
		rule_statement();
	}
	accept(endsy);
}

void PCompiler::rule_statement()
{
	if (m_token->isIdent()) {
		CIdent *ident = m_Context->find(m_token->ToString());
		if (ident == nullptr) {
			error(new CError(m_tokenpos));
			ident = new CVarIdent(m_token->ToString(), m_Context->getError());
		}
		if (ident->m_type == itVar) {
			const CTypeIdent *left, *right;
			left = rule_variable(dynamic_cast<CVarIdent*>(ident));
			accept(assign);
			right = rule_expression();
			if (!CTypeUtility::CompatableAssign(left, right))
			{
				error(new CError(m_tokenpos));
			}
			return;
		}
		if (ident->m_type == itProc) {
			const CParamedTypeIdent *type = type_cast<const CParamedTypeIdent *>(ident);
			rule_Paramed(type);
			return;
		}
		error(new CError(m_tokenpos));
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
		cond = rule_expression();
		if (!cond->isT(ttBoolean)) {
			error(new CError(m_tokenpos));
		}
		accept(thensy);
		rule_statement();
		if (m_token->is(elsesy)) {
			rule_statement();
		}
		return;
	}
	if (m_token->is(whilesy)) {
		nextToken();
		rule_expression();
		accept(dosy);
		rule_statement();
		return;
	}
	if (m_token->is(forsy)) {
		nextToken();
		if (!m_token->isIdent())
		{
			error(new CError(m_tokenpos));
		}
		CIdent *ident = m_Context->find(m_token->ToString());
		string identName;
		if (!ident || !ident->type()->isOrdered()) {
			error(new CError(m_tokenpos));
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
			error(new CError(m_tokenpos));
		}
		accept(dosy);
		rule_statement();
	}
}

const CTypeIdent *PCompiler::rule_type()
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
		const CTypeIdent *base = rule_type();
		resType = new CArrayTypeIdent(types, base);
		m_Context->add(resType);
		return resType;
	}
	//acceptIdent();
	return m_Context->getError();
}

const CTypeIdent * PCompiler::rule_simpleType()
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
						resType = new CLimitedTypeIdent(from, to, from->type());
						m_Context->add(resType);
						nextToken();
						return resType;
					}
				}
			}
			return m_Context->getError();
		}
		return m_Context->getError();
		res = m_Context->findT(m_token->ToString());
		if (res == nullptr) {
			error(new CError(m_tokenpos));
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
				error(new CError(m_tokenpos));
			val = dynamic_cast<CCharVariant *>(m_token->m_val);
			if (val) {
				to = val->m_val;
				resType = new CLimitedTypeIdent(from, to, m_Context->getChar());
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
				error(new CError(m_tokenpos));
			val = dynamic_cast<CIntVariant *>(m_token->m_val);
			if (val) {
				to = val->m_val;
				resType = new CLimitedTypeIdent(from, to, m_Context->getInteger());
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
				error(new CError(m_tokenpos));
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

const CTypeIdent *PCompiler::rule_expression()
{
	const CTypeIdent *left, *right;
	EOperator op;
	left = rule_simpleExpression();
	if (m_token->is({ EOperator::equal, later, greater,
		latergrater, laterequal, greaterequal }, op)) {
		nextToken();
		right = rule_simpleExpression();
		return CTypeUtility::Result(left, op, right, m_Context);
	}
	return left;
}

const CTypeIdent *PCompiler::rule_simpleExpression()
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
			if (left->isT(ttError))
				error(new CError(m_tokenpos));
		}
	} while (op);
	return left;
}

const CTypeIdent *PCompiler::rule_term()
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
				error(new CError(m_tokenpos));
			}
			op = true;
		}
	} while (op);
	return left;
}

const CTypeIdent *PCompiler::rule_factor()
{
	const CTypeIdent *left(m_Context->getError()), *right(nullptr);
	EVarType vartype;
	if (m_token->is({ vtInt , vtReal, vtChar }, vartype)) {
		switch (vartype)
		{
		case vtInt:
			left = m_Context->getInteger();
			break;
		case vtReal:
			left = m_Context->getReal();
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
			error(new CError(m_tokenpos));
		}
		return left;
	}
	if (m_token->isIdent()) {
		CIdent *ident = m_Context->find(m_token->ToString());
		if (!ident) {
			error(new CError(m_tokenpos));
			nextToken();
			return m_Context->getError();
		}
		if (ident->m_type == itVar) {
			return rule_variable(dynamic_cast<CVarIdent*>(ident));
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
			return left;
		}
		error(new CError(m_tokenpos));
		return m_Context->getError();
	}
	error(new CError(m_tokenpos));
	return m_Context->getError();
}

const CTypeIdent * PCompiler::rule_arrayVar(const CTypeIdent *vartype)
{
	accept(lbracket);
	size_t i = 0;
	const CArrayTypeIdent *array = type_cast<const CArrayTypeIdent *>(vartype);
	if (!array) {
		CArrayTypeIdent *tarray(nullptr);
		tarray = new CArrayTypeIdent({}, m_Context->getError());
		error(new CError(m_tokenpos));
		m_Context->add(tarray);
		array = tarray;
	}
	const vector<const CTypeIdent *> &indexes = array->indexes();
	do {
		const CTypeIdent *type = rule_expression();
		if (i < indexes.size() && !CTypeUtility::Compatable(type, indexes[i])) {
			error(new CError(m_tokenpos));
		}
		i++;
	} while (m_token->is(comma));
	if (i != indexes.size()) {
		error(new CError(m_tokenpos));
	}
	accept(rbracket);
	return array->base();
}

const CTypeIdent * PCompiler::rule_variable(CVarIdent *variable)
{
	if (!variable) {
		error(new CError(m_tokenpos));
		return m_Context->getError();
	}
	const CTypeIdent *type = variable->type();
	nextToken();
	while (m_token->is(lbracket)) {
		type = rule_arrayVar(type);
	}
	return type;
}

void PCompiler::rule_Paramed(const CParamedTypeIdent *type)
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
				error(new CError(m_tokenpos));
			}
			i++;
		} while (m_token->is(comma));
		if (i != params.size()) {
			error(new CError(m_tokenpos));
		}
		accept(rightpar);
	}
}

void PCompiler::openContext()
{
	if (m_Context) {
		CContext *cur = new CContext(m_Context);
		m_Context = cur;
	}
	else {
		m_Context = new CContext();
	}
}

void PCompiler::closeContext()
{
	CContext *parent = m_Context->parent();
	delete m_Context;
	m_Context = parent;
}
