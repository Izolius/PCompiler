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
#include <list>
#include "Variant.h"
#include "Token.h"
#include "Context.h"
#include "CLib.h"
#include <algorithm>
using namespace std;

class PCompiler
{
	

	static const size_t ERR_MAX = 20;
	static const size_t ERR_ERROR = 0;

	struct CError
	{
		CTextPosition m_pos;
		size_t m_code;
		CError(CTextPosition pos, size_t code) :m_pos(pos), m_code(code) {}
		CError() : m_pos(0, 0), m_code(PCompiler::ERR_ERROR) {}
	};

	struct KeyWord 
	{
		EKeyWord code;
		string name;
	};
	vector<CError> m_ErrList;
	map<string, EKeyWord> m_KeyWords;
	map<char, EOperator> m_KeyCacheMap;
	CTextPosition m_curpos; 
	char m_ch; // тикущая литера
	CToken m_token; //текущий символ
	CTextPosition m_tokenpos;
	string m_line;
	string m_Code;
	stringstream m_stream;
	bool m_toStop;
	CContext *m_Context;

private:
	void init();
	
public:
	PCompiler();
	~PCompiler();

	void Compile(const string &Code);
private:
	void error(CError Error);
	//IO
	void nextLiter();
	void ReadNextLine();
	//lecsical
	void nextToken();
	void scanIdentKeyWord();
	bool IsKW(const string &ident, EKeyWord &kw) const;
	void scanUIntFloatC(bool isNeg = false);
	void scanUInt();
	void scanString();
	void removeComments(bool fromPar);
	//syntactic

	void accept(EKeyWord expected);
	void accept(EOperator expected);
	void acceptIdent();
	void accept(EVarType expected);

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
	void rule_funcDecl();
	void rule_funcHeader();
	void rule_statementPart();//<раздел операторов>
	void rule_compStatement();//<составной оператор>
	void rule_unlabeledStatement();//<непомеченый оператор>
	void rule_statement();//<оператор>
	void rule_simplStatement();//<простой оператор>
	void rule_complexStatement();//<сложный оператор>
	CTypeIdent *rule_type();//<тип>
	void rule_ifStatement();//<условный оператор>
	void rule_expression();//<выражение>
	void rule_simpleExpression();//<простое выражение>
	void rule_term();//<слагаемое>
	void rule_factor();//множитель
	//semantic
	void openContext();
	void closeContext();
};

#endif // !PCOMPILER_DEF

