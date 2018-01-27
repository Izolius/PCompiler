#include "PCompiler.h"


void PCompiler::init()
{
	m_ErrList.reserve(ERR_MAX);
	m_KeyWords=map<string,EKeyWord>(
		{{"if", ifsy},
		{"do", dosy},
		//{ofsy, "of"},
		//{orsy, "or"},
		//{insy, "in"},
		//{tosy, "to"},
		{"end", endsy},
		{"var", varsy},
		//{ divsy,"div" },
		//{andsy,"and"},
		//{notsy,"not"},
		//{forsy,"for"},
		//{modsy,"mod"},
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
		//{whilesy,"while"},
		//{arraysy,"array"},
		//{constsy,"const"},
		//{labelsy,"label"},
		//{untilsy,"until"},
		//{downtosy,"downto"},
		//{packedsy,"packed"},
		//{recordsy,"record"},
		//{repeatsy,"repeat"}},
		{"program", propgramsy}
		//{funcsy,"function"},
		//{procsy,"procedure"}
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
	//rule_program();
	while (!m_toStop) {
		nextToken();
		cout << m_token.ToString() << '\n';
	}
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
			m_token.Change(graterequal);
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
	while (m_ch >= 'a' && m_ch <= 'z' ||
		m_ch >= 'A' && m_ch <= 'Z' ||
		m_ch >= '0' && m_ch <= '9')
	{
		name += m_ch;
		nextLiter();
	}
	//EKeyWord kw;
	//if (IsKW(name, kw)) {
	//	m_token.Change(kw);
	//}
	//else {
	//	m_token.Change(name);
	//}
	m_token.Change(name);
}

bool PCompiler::IsKW(const string &ident, EKeyWord &kw) const
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
	m_token.Change(new CFloatVariant(nmb_float));
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

void PCompiler::accept(EKeyWord expected)
{
	if (m_token.is(expected)) {
		nextToken();
	}
}

void PCompiler::accept(EOperator expected)
{
	if (m_token.is(expected)) {
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
	if (m_token.is(propgramsy)) {
		nextToken();
		acceptIdent();
		accept(semicolon);
	}
	rule_block();
	accept(point);
}

void PCompiler::rule_block()
{
	rule_labelpart();
	rule_constpart();
	rule_typepart();
	rule_varpart();
	//rule_procfuncpart();
	rule_statementPart();
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
	accept(intval);
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
	accept((EVarType)(intval | floatval));
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
	acceptIdent();
	while (m_token.is(comma)) {
		nextToken();
		acceptIdent();
	}
	accept(colon);
	rule_type();
}

void PCompiler::rule_statementPart()
{
	rule_compStatement();
}

void PCompiler::rule_compStatement()
{
	accept(beginsy);
	rule_unlabeledStatement();
	while (m_token.is(semicolon)) {
		accept(semicolon);
		rule_unlabeledStatement();
	}
}

void PCompiler::rule_unlabeledStatement()
{
	rule_statement();
}

void PCompiler::rule_statement()
{

}

void PCompiler::rule_type()
{
	acceptIdent();
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

void PCompiler::rule_expression()
{
}
