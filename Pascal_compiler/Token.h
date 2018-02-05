#pragma once

#include "CLib.h"
#include "Variant.h"
#include <string>
#include <set>
using namespace std;

enum ETokenType {
	Oper,
	Ident,
	Value
};

enum EOperator {
	star, // *
	//sharp, // #
	slash, // /
	equal, // =
	comma, // ,
	semicolon, // ;
	colon, // :
	point, // .
	arrow, // ^
	leftpar, // (
	rightpar, // )
	lbracket, // [
	rbracket, // ]
	flpar, // {
	frpar, // }
	later, // <
	greater, // >
	laterequal, // <>
	greaterequal, // >=
	latergrater, // <=
	plus, // +
	minus, // -
	lcomment, // (*
	rcomment, // *)
	assign, // :=
	twopoints, // ..

	ifsy,
	dosy,
	ofsy,
	orsy,
	insy,
	tosy,
	endsy,
	varsy,
	divsy,
	andsy,
	notsy,
	forsy,
	modsy,
	nilsy,
	setsy,
	thensy,
	elsesy,
	casesy,
	filesy,
	gotosy,
	typesy,
	withsy,
	beginsy,
	whilesy,
	arraysy,
	constsy,
	labelsy,
	untilsy,
	downtosy,
	packedsy,
	recordsy,
	repeatsy,
	propgramsy,
	funcsy,
	procsy,
};

class CToken
{
public:
	ETokenType m_type;
	union {
		EOperator m_op;
		CVariant *m_val;
	};
	string m_str;
	CToken() {}
	~CToken();
	void Change(EOperator op);
	void Change(string ident);
	void Change(CVariant *val);
	string ToString() const;

	bool is(EOperator expected) const;
	bool is(initializer_list<EOperator> expected) const;
	bool is(initializer_list<EOperator> expected, EOperator &res) const;
	bool isIdent() const;
	bool isValue() const;
	bool is(EVarType type) const;
	bool is(initializer_list<EVarType> type) const;

	static string to_string(EOperator oper);
private:
	void ClearVal();
};
