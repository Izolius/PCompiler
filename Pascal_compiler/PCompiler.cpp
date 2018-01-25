#include "PCompiler.h"


void PCompiler::init()
{
	m_ErrList.reserve(ERR_MAX);
	m_KeyWords.assign({
		{{ifsy, "if"},
		{dosy,"do"},
		{ofsy, "of"},
		{orsy, "or"},
		{insy, "in"},
		{tosy, "to"}},
		{{endsy,"end" },
		{ varsy,"var" },
		{ divsy,"div" },
		{andsy,"and"},
		{notsy,"not"},
		{forsy,"for"},
		{modsy,"mod"},
		{nilsy,"nil"},
		{setsy,"set"}},
		{{thensy,"then"},
		{elsesy,"else"},
		{casesy,"case"},
		{filesy,"file"},
		{fotosy,"foto"},
		{typesy,"type"},
		{withsy,"with"}},
		{{beginsy,"begin"},
		{whilesy,"while"},
		{arraysy,"array"},
		{constsy,"const"},
		{labelsy,"label"},
		{untilsy,"until"}},
		{{downtosy,"downto"},
		{packedsy,"packed"},
		{recordsy,"record"},
		{repeatsy,"repeat"}},
		{{propgramsy,"propgram"}},
		{{funcsy,"function"}},
		{{procsy,"procedure"}}
	});
	m_KeyCacheMap = map<char, ETokenCode>({
		{'*',star},
		{'/',slash},
		{'=',ETokenCode::equal},
		{',',comma},
		{';',semicolon},
		{'^',arrow},
		{')',rightpar},
		{'[',lbracket},
		{']',rbracket},
		{'+',ETokenCode::plus},
		{'-',ETokenCode::minus}
	});
	m_toStop = false;
}

PCompiler::PCompiler(istream &stream):
	m_stream(stream)
{
	init();
}

PCompiler::~PCompiler()
{
}

void PCompiler::Compile()
{
	nextLiter();
	rule_program();
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
	if (m_toStop)
		return;
	while (m_ch == ' ' || m_ch == '\t') nextLiter();
	m_token.m_pos = m_curpos;
	
	
	switch (m_ch)
	{
	case '\'':
		scanCharC();
		break;
	case '<':
		nextLiter();
		if (m_ch == '=') {
			m_token.m_code = laterequal;
			nextLiter();
		}
		else if (m_ch == '>') {
			m_token.m_code = latergrater;
			nextLiter();
		}
		else
			m_token.m_code = later;
		break;
	case '>':
		nextLiter();
		if (m_ch == '=') {
			m_token.m_code = graterequal;
			nextLiter();
		}
		else
			m_token.m_code = greater;
		break;
	case ':':
		nextLiter();
		if (m_ch == '=') {
			m_token.m_code = assign;
			nextLiter();
		}
		else
			m_token.m_code = colon;
		break;
	case '.':
		nextLiter();
		if (m_ch == '.') {
			m_token.m_code = twopoints;
			nextLiter();
		}
		else
			m_token.m_code = point;
		break;
	case '(':
		nextLiter();
		if (m_ch == '*') {
			nextLiter();
			removeComments(true);
			nextToken(); // ASK: Надо ли?
		}
		else
			m_token.m_code = leftpar;
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
				m_token.m_code = iter->second;
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
	m_token.m_code = getCode(name, m_token.m_name);

}

ETokenCode PCompiler::getCode(const string & name,const string *&pname)
{
	if (name.size() == 1)
		return ident;
	auto &iter = find_if(
		m_KeyWords[name.size() - 2].cbegin(),
		m_KeyWords[name.size() - 2].cend(),
		[&name](const KeyWord &keyword) {return keyword.name == name; });
	if (iter != m_KeyWords[name.size() - 2].cend()) {
		pname = &iter->name;
		return iter->code;
	}
	else
		return ident;
}

void PCompiler::scanUIntFloatC(bool isNeg/* = false*/)
{
	scanUInt();
	if (isNeg)
		m_token.nmb_int = -m_token.nmb_int;
	if (m_ch != '.') {
		return;
	}
	nextLiter();
	int maxlen = numeric_limits<float>::digits10;
	int curlen(0);
	float nmb_float((float)m_token.nmb_int), pow(0.1f);
	while (m_ch >= '0' && m_ch <= '9'/* && curlen < maxlen*/) {
		int digit = m_ch - '0';
		nmb_float += digit*pow;
		pow /= 10;
		curlen++;
		nextLiter();
	}
	m_token.m_code = floatc;
	m_token.nmb_float = nmb_float;
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
	m_token.m_code = intc;
	m_token.nmb_int = nmb_int;
}

void PCompiler::scanCharC()
{
	nextLiter();
	if (m_ch == '\'') {

	}
	m_token.m_code = charc;
	m_token.one_symbol = m_ch;
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

void PCompiler::ReadNextLine()
{
	if (m_stream.eof())
		m_toStop = true;
	getline(m_stream, m_line);
}

void PCompiler::accept(ETokenCode expected)
{
	if (m_token.m_code == expected)
		nextToken();
	else
		error(CError(m_curpos, ERR_ERROR/*TODO*/));
}

void PCompiler::accept(set<ETokenCode> expected)
{
	if (expected.count(m_token.m_code) > 0)
		nextToken();
	else
		error(CError(m_curpos, ERR_ERROR/*TODO*/));
}

set<ETokenCode> PCompiler::start(ETokenCode tokenCode)
{
	switch (tokenCode)
	{
	case star:
		break;
	case slash:
		break;
	case ETokenCode::equal:
		break;
	case comma:
		break;
	case semicolon:
		break;
	case colon:
		break;
	case point:
		break;
	case arrow:
		break;
	case leftpar:
		break;
	case rightpar:
		break;
	case lbracket:
		break;
	case rbracket:
		break;
	case flpar:
		break;
	case frpar:
		break;
	case later:
		break;
	case greater:
		break;
	case laterequal:
		break;
	case graterequal:
		break;
	case latergrater:
		break;
	case ETokenCode::plus:
		break;
	case ETokenCode::minus:
		break;
	case lcomment:
		break;
	case rcomment:
		break;
	case assign:
		break;
	case twopoints:
		break;
	case ident:
		break;
	case floatc:
		break;
	case intc:
		break;
	case charc:
		break;
	case ifsy:
		break;
	case dosy:
		break;
	case ofsy:
		break;
	case orsy:
		break;
	case insy:
		break;
	case tosy:
		break;
	case endsy:
		break;
	case varsy:
		break;
	case divsy:
		break;
	case andsy:
		break;
	case notsy:
		break;
	case forsy:
		break;
	case modsy:
		break;
	case nilsy:
		break;
	case setsy:
		break;
	case thensy:
		break;
	case elsesy:
		break;
	case casesy:
		break;
	case filesy:
		break;
	case fotosy:
		break;
	case typesy:
		break;
	case withsy:
		break;
	case beginsy:
		break;
	case whilesy:
		break;
	case arraysy:
		break;
	case constsy:
		break;
	case labelsy:
		break;
	case untilsy:
		break;
	case downtosy:
		break;
	case packedsy:
		break;
	case recordsy:
		break;
	case repeatsy:
		break;
	case propgramsy:
		break;
	case funcsy:
		break;
	case procsy:
		break;
	case ERROR:
		break;
	default:
		break;
	}
}

set<ETokenCode> PCompiler::follow(ETokenCode tokenCode)
{
	switch (tokenCode)
	{
	case star:
		break;
	case slash:
		break;
	case ETokenCode::equal:
		break;
	case comma:
		break;
	case semicolon:
		break;
	case colon:
		break;
	case point:
		break;
	case arrow:
		break;
	case leftpar:
		break;
	case rightpar:
		break;
	case lbracket:
		break;
	case rbracket:
		break;
	case flpar:
		break;
	case frpar:
		break;
	case later:
		break;
	case greater:
		break;
	case laterequal:
		break;
	case graterequal:
		break;
	case latergrater:
		break;
	case ETokenCode::plus:
		break;
	case ETokenCode::minus:
		break;
	case lcomment:
		break;
	case rcomment:
		break;
	case assign:
		break;
	case twopoints:
		break;
	case ident:
		break;
	case floatc:
		break;
	case intc:
		break;
	case charc:
		break;
	case ifsy:
		break;
	case dosy:
		break;
	case ofsy:
		break;
	case orsy:
		break;
	case insy:
		break;
	case tosy:
		break;
	case endsy:
		break;
	case varsy:
		return set<ETokenCode>({ procsy, funcsy, beginsy });
	case divsy:
		break;
	case andsy:
		break;
	case notsy:
		break;
	case forsy:
		break;
	case modsy:
		break;
	case nilsy:
		break;
	case setsy:
		break;
	case thensy:
		break;
	case elsesy:
		break;
	case casesy:
		break;
	case filesy:
		break;
	case fotosy:
		break;
	case typesy:
		return set<ETokenCode>({ varsy,procsy,funcsy, beginsy });
	case withsy:
		break;
	case beginsy:
		break;
	case whilesy:
		break;
	case arraysy:
		break;
	case constsy:
		return set<ETokenCode>({ varsy,procsy,funcsy, beginsy, typesy });
		break;
	case labelsy:
		return set<ETokenCode>({ varsy,procsy,funcsy, beginsy, typesy, constsy });
		break;
	case untilsy:
		break;
	case downtosy:
		break;
	case packedsy:
		break;
	case recordsy:
		break;
	case repeatsy:
		break;
	case propgramsy:
		break;
	case funcsy:
		break;
	case procsy:
		break;
	case ERROR:
		break;
	default:
		break;
	}
	return set<ETokenCode>();
}

bool PCompiler::istoken(ETokenCode expected) const
{
	return m_token.m_code == expected;
}

void PCompiler::rule_program()
{
	accept(propgramsy);
	accept(ident);
	accept(semicolon);
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
	if (istoken(labelsy)) {
		rule_label();
		while (istoken(comma)) {
			nextToken();
			rule_label();
		}
	}
}

void PCompiler::rule_label()
{
	accept(intc);
}

void PCompiler::rule_constpart()
{
	if (istoken(constsy)) {
		do{
			rule_constdecl();
			accept(semicolon);
		} while (istoken(ident));
	}
}

void PCompiler::rule_constdecl()
{
	accept(ident);
	accept(ETokenCode::equal);
	bool bNeg = false;
	if (istoken(ETokenCode::minus)) {
		bNeg = true;
		nextToken();
	}
	else if (istoken(ETokenCode::plus)) {
		nextToken();
	}
	accept({ intc, floatc });
}

void PCompiler::rule_typepart()
{
	if (istoken(typesy)) {
		error(CError(m_curpos, ERR_ERROR));
	}
}

void PCompiler::rule_varpart()
{
	if (istoken(varsy)) {
		nextToken();
		do {
			rule_varDeclaration();
			accept(semicolon);
		} while (m_token.m_code == ident);
	}
}

void PCompiler::rule_varDeclaration()
{
	accept(ident);
	while (istoken(comma)) {
		nextToken();
		accept(ident);
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
	while (istoken(semicolon)) {
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

void PCompiler::rule_ifStatement()
{
	accept(ifsy);
	rule_expression();
	accept(thensy);
	rule_statement();
	if (istoken(elsesy)) {
		rule_statement();
	}
}
