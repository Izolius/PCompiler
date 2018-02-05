#include "PCompiler.h"
#include "TypeUtility.h"


void PCompiler::init()
{
	m_ErrList.reserve(ERR_MAX);
	m_KeyWords=map<string,EOperator>(
		{{"if", ifsy},
		{"do", dosy},
		//{ofsy, "of"},
		{"or", orsy},
		//{insy, "in"},
		//{tosy, "to"},
		{"end", endsy},
		{"var", varsy},
		{"div", divsy},
		{"and", andsy},
		{"not", notsy},
		//{forsy,"for"},
		{"mod", modsy},
		//{nilsy,"nil"},
		//{setsy,"set"},
		{"then", thensy},
		{"else", elsesy},
		//{casesy,"case"},
		//{filesy,"file"},
		//{gotosy,"goto"},
		{"type", typesy},
		//{withsy,"with"},
		{"begin", beginsy},
		{"while", whilesy},
		{"array", arraysy},
		{"const", constsy},
		//{labelsy,"label"},
		//{untilsy,"until"},
		//{downtosy,"downto"},
		//{packedsy,"packed"},
		//{recordsy,"record"},
		//{repeatsy,"repeat"}},
		{"program", propgramsy},
		{"function", funcsy},
		{"procedure", procsy}
	});
	m_KeyCacheMap = map<char, EOperator>({
		{'*',star},
		{'/',slash},
		{'=',EOperator::equal},
		{',',comma},
		{';',semicolon},
		{'^',arrow},
		{')',rightpar},
		{'[',lbracket},
		{']',rbracket},
		{'+',EOperator::plus},
		{'-',EOperator::minus}
	});
	m_toStop = false;
	m_ch = '\0';
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
	m_stream = stringstream(Code);
	nextLiter();
	rule_program();
	//while (!m_toStop) {
	//	nextToken();
	//	cout << m_token.ToString() << '\n';
	//}
}


void PCompiler::error(CError Error)
{
	m_ErrList.push_back(Error);
}

void PCompiler::nextLiter()
{
	if (m_curpos.m_pos == m_line.size() - 1 || !m_line.size()) {
		ReadNextLine();
		m_curpos.m_line++;
		m_curpos.m_pos = 0;
	}
	else {
		m_curpos.m_pos++;
	}
	m_ch = m_line[m_curpos.m_pos];
	if (m_line == "")
		nextLiter();
}

void PCompiler::nextToken()
{
	if (m_toStop) {
		m_token.m_str = "";
		return;
	}
	while (m_ch == ' ' || m_ch == '\t') nextLiter();
	m_tokenpos = m_curpos;
	
	
	switch (m_ch)
	{
	case '\'':
		scanString();
		break;
	case '<':
		nextLiter();
		if (m_ch == '=') {
			m_token.Change(laterequal);
			nextLiter();
		}
		else if (m_ch == '>') {
			m_token.Change(latergrater);
			nextLiter();
		}
		else
			m_token.Change(later);
		break;
	case '>':
		nextLiter();
		if (m_ch == '=') {
			m_token.Change(greaterequal);
			nextLiter();
		}
		else
			m_token.Change(greater);
		break;
	case ':':
		nextLiter();
		if (m_ch == '=') {
			m_token.Change(assign);
			nextLiter();
		}
		else
			m_token.Change(colon);
		break;
	case '.':
		nextLiter();
		if (m_ch == '.') {
			m_token.Change(twopoints);
			nextLiter();
		}
		else
			m_token.Change(point);
		break;
	case '(':
		nextLiter();
		if (m_ch == '*') {
			nextLiter();
			removeComments(true);
			nextToken(); // ASK: Надо ли?
		}
		else
			m_token.Change(leftpar);
		break;
	case '{':
		removeComments(false);
		nextToken(); // ASK: Надо ли?
		break;
	default:
		if (m_ch >= 'a' && m_ch <= 'z' || m_ch >= 'A' && m_ch <= 'Z')
			scanIdentKeyWord();
		else if (m_ch >= '0' && m_ch <= '9')
			scanUIntFloatC();
		else {
			auto iter = m_KeyCacheMap.find(m_ch);
			if (iter != m_KeyCacheMap.end()) {
				m_token.Change(iter->second);
				nextLiter();
			}
			else
			{
				error(CError(m_curpos, ERR_ERROR/*TODO*/));
				nextLiter();
			}
		}
		
		break;
	}
}

void PCompiler::scanIdentKeyWord()
{
	string name = "";
	size_t line = m_curpos.m_line;
	while ((m_ch >= 'a' && m_ch <= 'z' ||
		m_ch >= 'A' && m_ch <= 'Z' ||
		m_ch >= '0' && m_ch <= '9') && line == m_curpos.m_line)
	{
		name += m_ch;
		nextLiter();
	}
	EOperator kw;
	if (IsKW(name, kw)) {
		m_token.Change(kw);
	}
	else {
		m_token.Change(name);
	}
	//m_token.Change(name);
}

bool PCompiler::IsKW(const string &ident, EOperator &kw) const
{
	auto iter = m_KeyWords.find(ident);
	if (iter != m_KeyWords.cend()) {
		kw = iter->second;
		return true;
	}
	return false;
}

void PCompiler::scanUIntFloatC(bool isNeg/* = false*/)
{
	scanUInt();
	CIntVariant *val = dynamic_cast<CIntVariant*>(m_token.m_val);
	if (isNeg) {
		val->m_val = -val->m_val;
	}
	if (m_ch != '.') {
		return;
	}
	nextLiter();
	int maxlen = numeric_limits<float>::digits10;
	int curlen(0);
	float nmb_float((float)val->m_val), pow(0.1f);
	while (m_ch >= '0' && m_ch <= '9'/* && curlen < maxlen*/) {
		int digit = m_ch - '0';
		nmb_float += digit*pow;
		pow /= 10;
		curlen++;
		nextLiter();
	}
	m_token.Change(new CRealVariant(nmb_float));
}

void PCompiler::scanUInt()
{
	int nmb_int = 0;
	while (m_ch >= '0' && m_ch <= '9') {
		int digit = m_ch - '0';
		if (nmb_int < INT_MAX / 10 || nmb_int < INT_MAX / 10 && digit <= INT_MAX % 10)
			nmb_int = 10 * nmb_int + digit;
		else {
			error(CError(m_curpos, 203/*TODO: enum кодов ошибок*/));
			nmb_int = 0;
		}
		nextLiter();
	}
	m_token.Change(new CIntVariant(nmb_int));
}

void PCompiler::scanString()
{
	nextLiter();
	string str("");
	while (m_ch != '\'') {
		str += m_ch;
	}
	m_token.Change(str);
	nextLiter();
}

void PCompiler::removeComments(bool fromPar)
{
	if (fromPar) {
		enum { trash, star, rightpar } state(trash);
		while (state != rightpar) {
			if (state == trash && m_ch == '*')
				state = star;
			else if (state == star && m_ch == ')')
				state = rightpar;
			else
				state = trash;
			nextLiter();
		}
	}
	else {
		while (m_ch != '}') {
			nextLiter();
		}
		nextLiter();
	}
}

void PCompiler::accept(EOperator expected)
{
	if (m_token.is(expected)) {
		nextToken();
	}
}

void PCompiler::accept(initializer_list<EOperator> expected)
{
	if (m_token.is(expected)) {
		nextToken();
	}
}

void PCompiler::accept(initializer_list<EOperator> expected, EOperator & res)
{
	if (m_token.is(expected, res)) {
		nextToken();
	}
}

void PCompiler::acceptIdent()
{
	if (m_token.isIdent()) {
		nextToken();
	}
}

void PCompiler::accept(EVarType expected)
{
	if (m_token.is(expected)) {
		nextToken();
	}
}

void PCompiler::ReadNextLine()
{
	if (m_stream.eof())
		m_toStop = true;
	getline(m_stream, m_line);
}

void PCompiler::rule_program()
{
	openContext();
	nextToken();
	if (m_token.is(propgramsy)) {
		nextToken();
		acceptIdent();
		accept(semicolon);
	}
	rule_block();
	closeContext();
	accept(point);
	for (const CError &err : m_ErrList) {
		cout << err.m_pos.m_line << ' ' << err.m_pos.m_pos << endl;
	}
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
	if (m_token.is(labelsy)) {
		rule_label();
		while (m_token.is(comma)) {
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
	if (m_token.is(constsy)) {
		do{
			rule_constdecl();
			accept(semicolon);
		} while (m_token.isIdent());
	}
}

void PCompiler::rule_constdecl()
{
	acceptIdent();
	accept(EOperator::equal);
	bool bNeg = false;
	if (m_token.is(EOperator::minus)) {
		bNeg = true;
		nextToken();
	}
	else if (m_token.is(EOperator::plus)) {
		nextToken();
	}
	accept((EVarType)(vtInt | vtReal));
}

void PCompiler::rule_typepart()
{
	if (m_token.is(typesy)) {
		error(CError(m_curpos, ERR_ERROR));
	}
}

void PCompiler::rule_varpart()
{
	if (m_token.is(varsy)) {
		nextToken();
		do {
			rule_varDeclaration();
			accept(semicolon);
		} while (m_token.isIdent());
	}
}

void PCompiler::rule_varDeclaration()
{
	list<CVarIdent*> vars;
	auto addvar = [&]() {
		if (m_token.isIdent()) {
			CVarIdent *var = m_Context->findV(m_token.ToString());
			if (var) {
				error(CError(m_curpos, ERR_ERROR));
			}
			else {
				var = new CVarIdent(m_token.ToString());
				m_Context->add(var);
				vars.push_back(var);
			}
		}
	};
	addvar();
	acceptIdent();
	while (m_token.is(comma)) {
		nextToken();
		addvar();
		acceptIdent();
	}
	accept(colon);
	CTypeIdent *type = rule_type();
	accept(semicolon);
	if (type) {
		for (CVarIdent *var : vars) {
			var->m_type = type;
		}
	}
}

void PCompiler::rule_procFuncPart()
{
	if (m_token.is(funcsy)) {
		rule_funcDecl();
	}
	else if (m_token.is(procsy)) {
		//rule_procDecl();
	}
}

void PCompiler::rule_funcDecl()
{
	rule_funcHeader();
	rule_block();
}

void PCompiler::rule_funcHeader()
{
	accept(funcsy);
	if (m_token.isIdent()) {
		nextToken();
	}
	else {
		error(CError(m_curpos, ERR_ERROR));
	}
	if (m_token.is(colon)) {
		nextToken();
	}
	else if (m_token.is(leftpar)) {
		//TODO
	}
	acceptIdent();
}

void PCompiler::rule_statementPart()
{
	accept(beginsy);
	rule_statement();
	while (m_token.is(semicolon)) {
		nextToken();
		rule_statement();
	}
	accept(endsy);
}

void PCompiler::rule_statement()
{
	if (m_token.isIdent()) {
		CIdent *ident = m_Context->find(m_token.ToString());
		if (ident == nullptr) {
			error(CError(m_curpos, ERR_ERROR));
		}
		if (ident->m_type == itVar) {
			nextToken();
			accept(assign);
			rule_expression();
			accept(semicolon);
			return;
		}
		if (ident->m_type == itProc) {
			nextToken();
			if (m_token.is(leftpar)) {
				nextToken();
				rule_expression();
				while (m_token.is(comma)) {
					rule_expression();
				}
				accept(rightpar);
			}
			return;
		}
		error(CError(m_curpos, ERR_ERROR));
		return;
	}
	if (m_token.is(beginsy)) {
		nextToken();
		rule_statement();
		while (m_token.is(semicolon)) {
			nextToken();
			rule_statement();
		}
		accept(endsy);
		return;
	}
	if (m_token.is(ifsy)) {
		nextToken();
		rule_expression();
		accept(thensy);
		rule_statement();
		if (m_token.is(elsesy)) {
			nextToken();
			rule_statement();
		}
		return;
	}
	if (m_token.is(whilesy)) {
		nextToken();
		rule_expression();
		accept(dosy);
		rule_statement();
		return;
	}
}

CTypeIdent *PCompiler::rule_type()
{
	CTypeIdent *res = nullptr;
	if (m_token.isIdent()) {
		if (!(res = static_cast<CTypeIdent*>(m_Context->find(m_token.ToString())))) {
			error(CError(m_curpos, ERR_ERROR));
		}
	}
	acceptIdent();
	return res;
}

void PCompiler::rule_ifStatement()
{
	accept(ifsy);
	rule_expression();
	accept(thensy);
	rule_statement();
	if (m_token.is(elsesy)) {
		rule_statement();
	}
}

CTypeIdent *PCompiler::rule_expression()
{
	CTypeIdent *left, *right;
	EOperator op;
	left = rule_simpleExpression();
	if (m_token.is({ EOperator::equal, later, greater,
		latergrater, laterequal, greaterequal }, op)) {
		nextToken();
		right = rule_simpleExpression();
		return (CTypeUtility::Compatable(left, right) ? m_Context->findT("boolean") : nullptr);
	}
	return left;
}

CTypeIdent *PCompiler::rule_simpleExpression()
{
	if (m_token.is(EOperator::plus)) {
		nextToken();
	}
	else if (m_token.is(EOperator::minus)) {
		nextToken();
	}
	CTypeIdent *left, *right;
	EOperator oper;
	left = rule_term();
	bool op;
	do {
		op = false;
		if (m_token.is({ EOperator::plus, EOperator::minus, orsy }, oper)) {
			nextToken();
			right = rule_term();
			op = true;
			left = CTypeUtility::Result(left, oper, right);
			if (left->isT(ttError))
				error(CError(m_curpos, ERR_ERROR));
		}
	} while (op);
	return left;
}

CTypeIdent *PCompiler::rule_term()
{
	CTypeIdent *left, *right;
	left = rule_factor();
	bool op;
	EOperator oper;
	do {
		op = false;
		if (m_token.is({ star, slash, divsy, modsy, andsy }, oper)) {
			nextToken();
			right = rule_factor();
			left = CTypeUtility::Result(left, oper, right, m_Context);
			if (left->isT(ttError)) {
				error(CError(m_curpos, ERR_ERROR));
			}
			op = true;
		}
	} while (op);
	return left;
}

CTypeIdent *PCompiler::rule_factor()
{
	CTypeIdent *left(m_Context->findT("")), *right;
	if (m_token.is({ vtInt , vtReal })) {
		if (m_token.is(vtInt))
			left = m_Context->findT("int");
		else
			left = m_Context->findT("real");
		nextToken();
		return left;
	}
	if (m_token.is(leftpar)) {
		nextToken();
		left = rule_expression();
		accept(rightpar);
		return left;
	}
	if (m_token.is(notsy)) {
		nextToken();
		left = rule_factor();
		left = CTypeUtility::Result(left, notsy);
		if (left->isT(ttError)) {
			error(CError(m_curpos, ERR_ERROR));
		}
		return;
	}
	if (m_token.isIdent()) {
		CIdent *ident = m_Context->find(m_token.ToString());
		if (!ident) {
			error(CError(m_curpos, ERR_ERROR));
			return;
		}
		if (ident->m_type == itVar) {
			if (auto var = dynamic_cast<CVarIdent*>(ident)) {
				left = var->m_type;
			}
			nextToken();
			return left;
		}
		if (ident->m_type == itFunc) {
			if (auto func = dynamic_cast<CFuncIdent*>(ident)) {
				left = func->m_restype;
			}
			nextToken();
			if (m_token.is(leftpar)) {
				rule_expression();
				while (m_token.is(comma)) {
					rule_expression();
				}
				accept(rightpar);
			}
			return left;
		}
		error(CError(m_curpos, ERR_ERROR));
		return nullptr;
	}
	error(CError(m_curpos, ERR_ERROR));
	return nullptr;
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
