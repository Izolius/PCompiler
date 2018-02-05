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

bool CTypeIdent::isT(initializer_list<ETypeType> types) const
{
	for (ETypeType type : types) {
		if (type == m_T)
			return true;
	}
	return false;
}

bool CTypeIdent::isEqual(const CTypeIdent * type)
{
	return this == type;
}

CSimpleTypeIdent::CSimpleTypeIdent(const string &name, ETypeType type):
	CTypeIdent(name, type)
{
}

bool CTypeIdent::isT(ETypeType type) const
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

bool CEnumTypeIdent::contain(CTypeIdent * ptype) const
{
	if (auto type = dynamic_cast<CLimitedTypeIdent*>(ptype)) {
		return type->base()->isEqual(this);
	}
	return false;
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

CLimitedTypeIdent::CLimitedTypeIdent(const string & name, CEnumConstIdent * from, CEnumConstIdent * to, CTypeIdent *base):
	CTypeIdent(name, ttLimited), m_efrom(from), m_eto(to), m_type(ctEnum), m_base(base)
{
}

CLimitedTypeIdent::CLimitedTypeIdent(const string & name, int from, int to, CTypeIdent *base):
	CTypeIdent(name, ttLimited), m_ifrom(from), m_ito(to), m_type(ctInt), m_base(base)
{
}

CLimitedTypeIdent::CLimitedTypeIdent(const string & name, unsigned char from, unsigned char to, CTypeIdent *base):
	CTypeIdent(name, ttLimited), m_cfrom(from), m_cto(to), m_type(ctChar), m_base(base)
{
}

int CLimitedTypeIdent::pos(const CEnumConstIdent * val) const
{
	if (m_type != ctEnum)
		return -1;
	CEnumTypeIdent *Enum = m_efrom->m_owner;
	auto vals = Enum->m_vals;
	auto iter = find(vals.cbegin(), vals.cend(), val);
	if (iter == vals.cend())
		return -1;
	return distance(iter, vals.cbegin());
}

int CLimitedTypeIdent::pos(int val) const
{
	if (m_type != ctInt)
		return -1;
	if (val >= m_ifrom && val <= m_ito)
		return val - m_ifrom;
	return -1;
}

int CLimitedTypeIdent::pos(unsigned char val) const
{
	if (m_type != ctChar)
		return false;
	if (val >= m_cfrom && val <= m_cto)
		return val - m_cfrom;
	return -1;
}

CTypeIdent *CLimitedTypeIdent::base() const
{
	return m_base;
}

bool CLimitedTypeIdent::contain(CTypeIdent * ptype) const
{
	if (auto type = dynamic_cast<CLimitedTypeIdent*>(ptype)) {
		if (!m_base->isEqual(type->m_base))
			return false;
		int from(-1), to(-1);
		switch (m_type)
		{
		case CLimitedTypeIdent::ctEnum:
			int from = pos(type->m_efrom);
			int to = pos(type->m_eto);
			break;
		case CLimitedTypeIdent::ctInt:
			int from = pos(type->m_ifrom);
			int to = pos(type->m_ito);
			break;
		case CLimitedTypeIdent::ctChar:
			int from = pos(type->m_cfrom);
			int to = pos(type->m_cto);
			break;
		}
		return from >= 0 && from <= to;
	}
	if (auto type = dynamic_cast<CEnumTypeIdent*>(ptype)) {
		if (!m_base->isEqual(type) || m_type != ctEnum)
			return false;
		return m_efrom == type->m_vals.front() && m_eto == type->m_vals.back();
	}
	if (auto type = dynamic_cast<CCharTypeIdent*>(ptype)) {
		if (!m_base->isEqual(type) || m_type != ctChar)
			return false;
		return m_cfrom == 0x0 && m_cto == 0xff;
	}
}

bool CRealTypeIdent::contain(CTypeIdent * ptype) const
{
	if (ptype->isT(ttInt))
		return true;
	return false;
}
