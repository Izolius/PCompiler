#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include "CLib.h"
#include "Token.h"
#include "ErrorManager.h"

using namespace std;
class CLexer
{
	stringstream m_stream;
	string m_line;
	char m_ch; // тикущая литера
	CTextPosition m_curpos;
	CToken m_token;
	CTextPosition m_tokenpos;
	bool m_toStop;
	map<string, EOperator> m_KeyWords;
	map<char, EOperator> m_KeyCacheMap;
	CErrorManager *m_ErrorManager;
public:
	CLexer();
	~CLexer();
private:
	//IO
	void nextLiter();
	void ReadNextLine();
	void WriteCurLine(bool bWithErrors = true);
	//lecsical
	void scanIdentKeyWord();
	bool IsKW(const string &ident, EOperator &kw) const;
	void scanUIntFloatC(bool isNeg = false);
	void scanUInt();
	void scanString();
	void removeComments(bool fromPar);
	//errors
	void error(CError *error);
public:
	void setCode(const string &code);
	CToken *nextToken();
	CTextPosition getTokenPos() const;
	void UpdateErrorManager(CErrorManager *mngr);
};

