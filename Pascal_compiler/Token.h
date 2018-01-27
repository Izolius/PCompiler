#pragma once

#include "CLib.h"
#include "Variant.h"
#include <string>
using namespace std;

enum ETokenType {
	Oper,
	Ident,
	Value,
	KeyWord
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
	graterequal, // >=
	latergrater, // <=
	plus, // +
	minus, // -
	lcomment, // (*
	rcomment, // *)
	assign, // :=
	twopoints // ..
};

enum EKeyWord {
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
		EKeyWord m_kw;
		CVariant *m_val;
	};
	string m_str;
	CToken() {}
	~CToken();
	void Change(EOperator op);
	void Change(EKeyWord kw);
	void Change(string ident);
	void Change(CVariant *val);
	string ToString() const;

	bool is(EOperator expected) const;
	bool is(EKeyWord expected) const;
	bool isIdent() const;
	bool isValue() const;
	bool is(EVarType type) const;

	static string to_string(EOperator oper);
private:
	void ClearVal();
};
