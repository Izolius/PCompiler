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
		EOperator code;
		string name;
	};
	vector<CError> m_ErrList;
	map<string, EOperator> m_KeyWords;
	map<char, EOperator> m_KeyCacheMap;
	CTextPosition m_curpos; 
	char m_ch; // ������� ������
	CToken m_token; //������� ������
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
	void WriteCurLine(bool bWithErrors = true);
	//lecsical
	void nextToken();
	void scanIdentKeyWord();
	bool IsKW(const string &ident, EOperator &kw) const;
	void scanUIntFloatC(bool isNeg = false);
	void scanUInt();
	void scanString();
	void removeComments(bool fromPar);
	//syntactic

	void accept(EOperator expected);
	void accept(initializer_list<EOperator> expected);
	void accept(initializer_list<EOperator> expected, EOperator &res);
	void acceptIdent();
	void accept(EVarType expected);

	void rule_program();
	void rule_block();
	void rule_labelpart();//<������ �����>
	void rule_label();//<�����>
	void rule_constpart();//<������ ��������>
	void rule_constdecl();//<����������� ���������>
	void rule_typepart();//<������ �����>
	void rule_varpart();//<������ ����������>
	vector<CVarIdent*> rule_varDeclaration();//<�������� ���������� ����������>
	void rule_procFuncPart();//<������ �������� � �������>
	void rule_funcDecl();
	void rule_funcHeader();
	void rule_statementPart();//<������ ����������>
	void rule_statement();//<��������>
	const CTypeIdent *rule_type();//<���>
	const CTypeIdent *rule_simpleType();//<������� ���>
	const CTypeIdent *rule_expression();//<���������>
	const CTypeIdent *rule_simpleExpression();//<������� ���������>
	const CTypeIdent *rule_term();//<���������>
	const CTypeIdent *rule_factor();//���������
	const CTypeIdent *rule_arrayVar(const CTypeIdent *vartype);
	void rule_Paramed(const CParamedTypeIdent *type);
	//semantic
	void openContext();
	void closeContext();
};

#endif // !PCOMPILER_DEF

