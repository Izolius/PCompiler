#pragma once
#ifndef PCOMPILER_DEF
#define PCOMPILER_DEF

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <sstream>
#include <map>
#include <set>
#include "Tokens.h"
#include <algorithm>
using namespace std;

class PCompiler
{
	

	static const size_t ERR_MAX = 20;
	static const char ERR_LITER = '&';
	static const ETokenCode ERR_TOKEN = ERROR;
	static const size_t ERR_ERROR = 0;
	
	struct CTextPosition
	{
		size_t m_line; //номер строки
		size_t m_pos; //номер позиции
		CTextPosition(size_t line, size_t pos) :m_line(line), m_pos(pos) {}
		CTextPosition() :CTextPosition(0, 0) {}
	};

	struct CError
	{
		CTextPosition m_pos;
		size_t m_code;
		CError(CTextPosition pos, size_t code) :m_pos(pos), m_code(code) {}
		CError() : m_pos(0, 0), m_code(PCompiler::ERR_ERROR) {}
	};

	class CToken
	{
	public:
		//первая литера символа
		CTextPosition m_pos;
		ETokenCode m_code;
		const string *m_name;
		union {
			int nmb_int;
			float nmb_float;
			char one_symbol;
		};
		CToken() :m_pos(0, 0), m_code(PCompiler::ERR_TOKEN) {}
	};

	struct KeyWord 
	{
		ETokenCode code;
		string name;
	};
	vector<CError> m_ErrList;
	vector<vector<KeyWord>> m_KeyWords;
	map<char, ETokenCode> m_KeyCacheMap;
	CTextPosition m_curpos; 
	char m_ch; // тикущая литера
	CToken m_token; //текущий символ
	istream &m_stream;
	string m_line;
	bool m_toStop;

private:
	void init();
	
public:
	PCompiler(istream &stream);
	~PCompiler();

	void Compile();
private:
	void error(CError Error);
	//IO
	void nextLiter();
	void ReadNextLine();
	//lecsical
	void nextToken();
	void scanIdentKeyWord();
	ETokenCode getCode(const string &name,const string *&pname);
	void scanUIntFloatC(bool isNeg = false);
	void scanUInt();
	void scanCharC();
	void removeComments(bool fromPar);
	//syntactic
	void accept(ETokenCode expected);
	void accept(set<ETokenCode> expected);
	set<ETokenCode> start(ETokenCode tokenCode);
	set<ETokenCode> follow(ETokenCode tokenCode);
	bool istoken(ETokenCode expected) const;
	void rule_program();
	void rule_block();
	void rule_labelpart();//<раздел меток>
	void rule_label();//<метка>
	void rule_constpart();//<раздел констант>
	void rule_constdecl();//<определение константы>
	void rule_typepart();//<раздел типов>
	void rule_typedecl();//<определение типа>
	void rule_varpart();//<раздел переменных>
	void rule_varDeclaration();//<описание однотипных переменных>
	void rule_procFuncPart();//<раздел процедур и функций>
	void rule_statementPart();//<раздел операторов>
	void rule_compStatement();//<составной оператор>
	void rule_unlabeledStatement();//<непомеченый оператор>
	void rule_statement();//<оператор>
	void rule_simplStatement();//<простой оператор>
	void rule_complexStatement();//<сложный оператор>
	void rule_type();//<тип>
	void rule_ifStatement();//<условный оператор>
	void rule_expression();//<выражение>
};

#endif // !PCOMPILER_DEF

