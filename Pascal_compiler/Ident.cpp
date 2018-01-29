#include "Ident.h"

CIdent::CIdent(const string &name):
	CIdent(name, itVar)
{
}

CIdent::CIdent(const string & name, EIdentType type):
	m_name(name), m_type(type)
{
}

CIdent::~CIdent()
{
}

CVarIdent::CVarIdent(const string &name):
	CIdent(name, itVar), m_type(nullptr)
{
}

CTypeIdent::CTypeIdent(const string &name, ETypeType type) :
	CIdent(name, itType),
	m_T(type), T(m_T)
{
}

CSimpleTypeIdent::CSimpleTypeIdent(const string &name, ETypeType type):
	CTypeIdent(name, type)
{
}

bool CTypeIdent::isT(ETypeType type)
{
	return (m_T & type) != 0;
}

CStringTypeIdent::CStringTypeIdent(size_t maxlen) :
	CTypeIdent("string", ttString), m_maxlen(maxlen)
{
}

CEnumTypeIdent::CEnumTypeIdent(const string &name, const vector<CEnumConstIdent*>& Enum):
	m_vals(Enum), CSimpleTypeIdent(name, ttEnum)
{
	for (CEnumConstIdent *item : m_vals) {
		item->m_owner = this;
	}
}

CEnumConstIdent::CEnumConstIdent(const string & name, CEnumTypeIdent *owner) :
	CIdent(name, itEnumConst), m_owner(owner)
{
}

CProcIdent::CProcIdent(const string & name, const vector<CVarIdent*>& params):
	CIdent(name,itProc), m_params(params)
{
}

CFuncIdent::CFuncIdent(const string & name, const vector<CVarIdent*>& params, CTypeIdent * restype):
	CIdent(name, itFunc), m_params(params), m_restype(restype)
{
}

CLimitedTypeIdent::CLimitedTypeIdent(const string & name, CEnumConstIdent * from, CEnumConstIdent * to):
	CTypeIdent(name, ttLimited), m_efrom(from), m_eto(to), m_type(ctEnum)
{
}

CLimitedTypeIdent::CLimitedTypeIdent(const string & name, int from, int to):
	CTypeIdent(name, ttLimited), m_ifrom(from), m_ito(to), m_type(ctInt)
{
}

CLimitedTypeIdent::CLimitedTypeIdent(const string & name, char from, char to):
	CTypeIdent(name, ttLimited), m_cfrom(from), m_cto(to), m_type(ctChar)
{
}

bool CLimitedTypeIdent::contains(CEnumConstIdent * val)
{
	if (m_type != ctEnum)
		return false;
	CEnumTypeIdent *Enum = m_efrom->m_owner;
	auto vals = Enum->m_vals;
	auto iter = find(vals.cbegin(), vals.cend(), val);
	if (iter == vals.cend())
		return false;
	return val >= m_efrom && val <= m_eto;
}

bool CLimitedTypeIdent::contains(int val)
{
	if (m_type != ctInt)
		return false;
	return val >= m_ifrom && val <= m_ito;
}

bool CLimitedTypeIdent::contains(char val)
{
	if (m_type != ctChar)
		return false;
	return val >= m_cfrom && val <= m_cto;
}
