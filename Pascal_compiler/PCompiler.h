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
		size_t m_line; //����� ������
		size_t m_pos; //����� �������
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
		//������ ������ �������
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
	char m_ch; // ������� ������
	CToken m_token; //������� ������
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
	void rule_labelpart();//<������ �����>
	void rule_label();//<�����>
	void rule_constpart();//<������ ��������>
	void rule_constdecl();//<����������� ���������>
	void rule_typepart();//<������ �����>
	void rule_typedecl();//<����������� ����>
	void rule_varpart();//<������ ����������>
	void rule_varDeclaration();//<�������� ���������� ����������>
	void rule_procFuncPart();//<������ �������� � �������>
	void rule_statementPart();//<������ ����������>
	void rule_compStatement();//<��������� ��������>
	void rule_unlabeledStatement();//<����������� ��������>
	void rule_statement();//<��������>
	void rule_simplStatement();//<������� ��������>
	void rule_complexStatement();//<������� ��������>
	void rule_type();//<���>
	void rule_ifStatement();//<�������� ��������>
	void rule_expression();//<���������>
};

#endif // !PCOMPILER_DEF

