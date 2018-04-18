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
#include <fstream>
#include "Variant.h"
#include "Token.h"
#include "Context.h"
#include "CLib.h"
#include "Lexer.h"
#include "ErrorManager.h"
#include "Generator.h"
#include <algorithm>
#include <memory>
#include <functional>
using namespace std;

class CGenerator;

class CCompiler
{
	

	static const size_t ERR_MAX = 20;
	static const size_t ERR_ERROR = 0;

	struct KeyWord 
	{
		EOperator code;
		string name;
	};
	vector<CError> m_ErrList;
	CToken *m_token; //������� ������
	CTextPosition m_tokenpos;
	CContext *m_Context;
	CLexer m_Lexer;
	unique_ptr<CErrorManager> m_ErrorManager;
	CGenerator m_gen;
	stack<string> m_curfunc;
private:
	void init();
	
public:
	CCompiler();
	~CCompiler();

	bool Compile(const string &source, const string &dest);
private:
	void error(CError *Error);
	//lecsical
	void nextToken();
	//syntactic

	void accept(EOperator expected);
	void accept(initializer_list<EOperator> expected);
	void accept(initializer_list<EOperator> expected, EOperator &res);
	void acceptIdent();
	void accept(EVarType expected);

	void rule_start(void (CCompiler::*rule)(), initializer_list<EOperator> stopwords);
	template<class T>
	T rule_start(T(CCompiler::*rule)(), initializer_list<EOperator> stopwords, T defaultResult)
	{
		T res;
		try
		{
			res = (this->*rule)();
		}
		catch (exception *ex) {
			res = defaultResult;
			while (!m_token->is(stopwords) && !m_token->is(eofsy)) {
				nextToken();
			}
		}
		return res;
	}
	template<class T, class R>
	T rule_start(T(CCompiler::*rule)(R), initializer_list<EOperator> stopwords, T defaultResult, R arg)
	{
		T res;
		try
		{
			res = (this->*rule)(arg);
		}
		catch (exception *ex) {
			res = defaultResult;
			while (!m_token->is(stopwords) && !m_token->is(eofsy)) {
				nextToken();
			}
		}
		return res;
	}
	template<class R>
	void rule_start(void(CCompiler::*rule)(R), initializer_list<EOperator> stopwords, R arg)
	{
		try
		{
			(this->*rule)(arg);
		}
		catch (exception *ex) {
			while (!m_token->is(stopwords) && !m_token->is(eofsy)) {
				nextToken();
			}
		}
	}

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
	vector<CVarIdent*> rule_procFuncDecl();
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
	const CTypeIdent *rule_variable(CVarIdent *variable);
	void rule_Paramed(const CParamedTypeIdent *type);
	//semantic
	void openContext();
	void closeContext();
	//
	void freeContextVarsMem(const vector<CVarIdent*> &ContextParams);
	void freeContextVarsMem(const CParamedTypeIdent *type);
};

#endif // !PCOMPILER_DEF

