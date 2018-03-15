#include "Lexer.h"



CLexer::CLexer()
{
	m_KeyWords = map<string, EOperator>(
	{ { "if", ifsy },
	{ "do", dosy },
	{ "of",ofsy },
	{ "or", orsy },
		//{insy, "in"},
	{ "to", tosy },
	{ "end", endsy },
	{ "var", varsy },
	{ "div", divsy },
	{ "and", andsy },
	{ "not", notsy },
	{ "for", forsy },
	{ "mod", modsy },
		//{nilsy,"nil"},
		//{setsy,"set"},
	{ "then", thensy },
	{ "else", elsesy },
		//{casesy,"case"},
		//{filesy,"file"},
		//{gotosy,"goto"},
	{ "type", typessy },
		//{withsy,"with"},
	{ "begin", beginsy },
	{ "while", whilesy },
	{ "array", arraysy },
	{ "const", constsy },
	{ "label", labelsy},
		//{untilsy,"until"},
	{"downto",downtosy },
		//{packedsy,"packed"},
		//{recordsy,"record"},
		//{repeatsy,"repeat"}},
	{ "program", programsy },
	{ "function", funcsy },
	{ "procedure", procsy }
	});
	m_KeyCacheMap = map<char, EOperator>({
		{ '*',star },
		{ '/',slash },
		{ '=',EOperator::equal },
		{ ',',comma },
		{ ';',semicolon },
		{ '^',arrow },
		{ ')',rightpar },
		{ '[',lbracket },
		{ ']',rbracket },
		{ '+',EOperator::plus },
		{ '-',EOperator::minus }
	});
}


CLexer::~CLexer()
{
}


void CLexer::nextLiter()
{
	if (m_curpos.m_pos == m_line.size() - 1 || !m_line.size()) {
		WriteCurLine();
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

CToken *CLexer::nextToken()
{
	if (m_toStop) {
		m_token.m_str = "";
		return &m_token;
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
				error(new CError(m_curpos, ecUnknownLiter));
				nextLiter();
			}
		}

		break;
	}
	return &m_token;
}

CTextPosition CLexer::getTokenPos() const
{
	return m_tokenpos;
}

void CLexer::UpdateErrorManager(CErrorManager * mngr)
{
	m_ErrorManager = mngr;
}

void CLexer::scanIdentKeyWord()
{
	string name = "";
	size_t line = m_curpos.m_line;
	while ((m_ch >= 'a' && m_ch <= 'z' ||
		m_ch >= 'A' && m_ch <= 'Z' ||
		m_ch >= '0' && m_ch <= '9') && line == m_curpos.m_line)
	{
		name += (char)tolower(m_ch);
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

bool CLexer::IsKW(const string &ident, EOperator &kw) const
{
	auto iter = m_KeyWords.find(ident);
	if (iter != m_KeyWords.cend()) {
		kw = iter->second;
		return true;
	}
	return false;
}

void CLexer::scanUIntFloatC(bool isNeg/* = false*/)
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

void CLexer::scanUInt()
{
	int nmb_int = 0;
	while (m_ch >= '0' && m_ch <= '9') {
		int digit = m_ch - '0';
		if (nmb_int < INT_MAX / 10 || nmb_int < INT_MAX / 10 && digit <= INT_MAX % 10)
			nmb_int = 10 * nmb_int + digit;
		else {
			error(new CError(m_curpos));
			nmb_int = 0;
		}
		nextLiter();
	}
	m_token.Change(new CIntVariant(nmb_int));
}

void CLexer::scanString()
{
	nextLiter();
	string str("");
	while (m_ch != '\'') {
		str += m_ch;
		nextLiter();
	}
	if (str.size() == 1)
		m_token.Change(new CCharVariant(str[0]));
	else
		m_token.Change(new CStringVariant(str));
	nextLiter();
}

void CLexer::removeComments(bool fromPar)
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

void CLexer::error(CError * error)
{
	m_ErrorManager->addError(error);
}

void CLexer::ReadNextLine()
{
	if (m_stream.eof())
		m_toStop = true;
	getline(m_stream, m_line);
}

void CLexer::WriteCurLine(bool bWithErrors)
{
	//cout << m_line << endl;
	//size_t printed(0);
	//for (const CError &error : m_ErrList) {
	//	if (error.m_pos.m_line == m_curpos.m_line) {
	//		cout << string(error.m_pos.m_pos - printed, ' ');
	//		cout << '^';
	//		printed += error.m_pos.m_pos - printed + 1;
	//	}
	//}
}

void CLexer::setCode(const string &code)
{
	m_stream = stringstream(code);
	m_toStop = false;
	m_ch = '\0';
	nextLiter();
}
