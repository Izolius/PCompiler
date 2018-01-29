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

enum EOperator : long {
	star = 0x1, // *
	//sharp, // #
	slash = 0x2, // /
	equal = 0x4, // =
	comma = 0x8, // ,
	semicolon = 0x10, // ;
	colon = 0x20, // :
	point = 0x40, // .
	arrow = 0x80, // ^
	leftpar = 0x100, // (
	rightpar = 0x200, // )
	lbracket = 0x400, // [
	rbracket = 0x800, // ]
	flpar = 0x1000, // {
	frpar = 0x2000, // }
	later = 0x4000, // <
	greater = 0x8000, // >
	laterequal = 0x10000, // <>
	greaterequal = 0x20000, // >=
	latergrater = 0x40000, // <=
	plus = 0x80000, // +
	minus = 0x100000, // -
	lcomment = 0x200000, // (*
	rcomment = 0x400000, // *)
	assign = 0x800000, // :=
	twopoints = 0x1000000// ..
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
