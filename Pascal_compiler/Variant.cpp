#include "Variant.h"


CVariant::CVariant(EVarType type) :
	m_T(type), T(m_T)
{
}

CIntVariant::CIntVariant(int val):
	CVariant(intval), m_val(val)
{
}

string CIntVariant::ToString()
{
	return to_string(m_val);
}

CFloatVariant::CFloatVariant(float val):
	CVariant(floatval), m_val(val)
{
}

string CFloatVariant::ToString()
{
	return to_string(m_val);
}

CStringVariant::CStringVariant(string val):
	CVariant(stringval), m_val(val)
{
}

string CStringVariant::ToString()
{
	return m_val;
}
