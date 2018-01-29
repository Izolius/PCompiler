#include "Variant.h"


CVariant::CVariant(EVarType type) :
	m_T(type), T(m_T)
{
}

CIntVariant::CIntVariant(int val):
	CVariant(vtInt), m_val(val)
{
}

string CIntVariant::ToString()
{
	return to_string(m_val);
}

CRealVariant::CRealVariant(float val):
	CVariant(vtReal), m_val(val)
{
}

string CRealVariant::ToString()
{
	return to_string(m_val);
}

CStringVariant::CStringVariant(string val):
	CVariant(vtString), m_val(val)
{
}

string CStringVariant::ToString()
{
	return m_val;
}
