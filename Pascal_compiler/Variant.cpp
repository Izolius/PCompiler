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

CVariant * CIntVariant::Clone() const
{
	return new CIntVariant(m_val);
}

CRealVariant::CRealVariant(float val):
	CVariant(vtReal), m_val(val)
{
}

string CRealVariant::ToString()
{
	string res = to_string(m_val);
	size_t offset = res.find(',');
	if (offset != string::npos) {
		res.replace(offset, 1, ".");
	}

	return res;
}

CVariant * CRealVariant::Clone() const
{
	return new CRealVariant(m_val);
}

CStringVariant::CStringVariant(string val):
	CVariant(vtString), m_val(val)
{
}

string CStringVariant::ToString()
{
	return m_val;
}

CVariant * CStringVariant::Clone() const
{
	return new CStringVariant(m_val);
}

CCharVariant::CCharVariant(unsigned char val):
	CVariant(vtChar), m_val(val)
{
}

string CCharVariant::ToString()
{
	return string(1, m_val);
}

CVariant * CCharVariant::Clone() const
{
	return new CCharVariant(m_val);
}
