#include "Token.h"

CToken::CToken()
{
	m_str = "";
	m_val = nullptr;
}

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
	return m_type == Oper && m_op == expected;
}

bool CToken::is(initializer_list<EOperator> expected) const
{
	if (m_type != Oper)
		return false;
	for (EOperator op : expected)
		if (op == m_op)
			return true;
	return false;
}

bool CToken::is(initializer_list<EOperator> expected, EOperator & res) const
{
	if (m_type != Oper)
		return false;
	for (EOperator op : expected)
		if (op == m_op) {
			res = op;
			return true;
		}
	return false;
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
	return isValue() && m_val->T == type;
}

bool CToken::is(initializer_list<EVarType> type) const
{
	if (!isValue())
		return false;
	for (EVarType tp : type)
		if (tp == m_val->T)
			return true;
	return false;
}

bool CToken::is(initializer_list<EVarType> type, EVarType & res) const
{
	if (!isValue())
		return false;
	for (EVarType tp : type)
		if (tp == m_val->T)
		{
			res = tp;
			return true;
		}
	return false;
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

CToken &CToken::operator=(const CToken & obj)
{
	m_type = obj.m_type;
	switch (m_type)
	{
	case Oper:
		Change(obj.m_op);
		break;
	case Ident:
		Change(obj.m_str);
		break;
	case Value:
		m_val = obj.m_val->Clone();
		m_str = m_val->ToString();
		break;
	default:
		break;
	}
	return *this;
}

void CToken::ClearVal()
{
	if (m_type == Value) {
		if (m_val)
			delete m_val;
		m_val = nullptr;
	}
	m_str = "";
}
