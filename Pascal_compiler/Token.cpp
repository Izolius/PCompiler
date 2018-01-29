#include "Token.h"

CToken::~CToken()
{
	ClearVal();
}

void CToken::Change(EOperator op)
{
	ClearVal();
	m_type = Oper;
	m_op = op;
	m_str = to_string(op);
}

void CToken::Change(EKeyWord kw)
{
	ClearVal();
	m_type = KeyWord;
	m_kw = kw;
}

void CToken::Change(string ident)
{
	ClearVal();
	m_type = Ident;
	m_str = ident;
}

void CToken::Change(CVariant *val)
{
	ClearVal();
	if (val) {
		m_type = Value;
		m_val = val;
		m_str = val->ToString();
	}
}

string CToken::ToString() const
{
	return m_str;
}

bool CToken::is(EOperator expected) const
{
	return m_type == Oper && (m_op & expected) != 0;
}

bool CToken::is(EKeyWord expected) const
{
	return m_type == KeyWord && m_kw == expected;
}

bool CToken::isIdent() const
{
	return m_type == Ident;
}

bool CToken::isValue() const
{
	return m_type == Value;
}

bool CToken::is(EVarType type) const
{
	return isValue() && (m_val->T & type) != 0;
}

string CToken::to_string(EOperator oper)
{
	switch (oper)
	{
	case star:
		return "*";
	case slash:
		return "/";
	case EOperator::equal:
		return "=";
	case comma:
		return ",";
	case semicolon:
		return ";";
	case colon:
		return ":";
	case point:
		return ".";
	case arrow:
		return "^";
	case leftpar:
		return "(";
	case rightpar:
		return ")";
	case lbracket:
		return "[";
	case rbracket:
		return "]";
	case flpar:
		return "{";
	case frpar:
		return "}";
	case later:
		return "<";
	case greater:
		return ">";
	case laterequal:
		return "<>";
	case greaterequal:
		return ">=";
	case latergrater:
		return "<=";
	case EOperator::plus:
		return "+";
	case EOperator::minus:
		return "-";
	case lcomment:
		return "(*";
	case rcomment:
		return "*)";
	case assign:
		return ":=";
	case twopoints:
		return "..";
	default:
		break;
	}
	return "";
}

void CToken::ClearVal()
{
	if (m_type == Value) {
		delete m_val;
		m_val = nullptr;
	}
	m_str = "";
}
