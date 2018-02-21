#include "Ident.h"

CIdent::CIdent(const string &name):
	CIdent(name, itVar)
{
}

CIdent::CIdent(const string & name, EIdentType type):
	m_name(name), m_type(type)
{
}

const string & CIdent::name() const
{
	return m_name;
}

CIdent::~CIdent()
{
}

CVarIdent::CVarIdent(const string &name, const CTypeIdent *type):
	CTypedIdent(name, type, itVar)
{
}

bool CTypeIdent::contain(const CTypeIdent * ptype) const
{
	return isEqual(ptype);
}

const CTypeIdent * CTypeIdent::type() const
{
	return this;
}

const bool CTypeIdent::isSimple() const
{
	return false;
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

bool CTypeIdent::isEqual(const CTypeIdent * type) const
{
	return this->type() == type->type();
}

bool CTypeIdent::isT(ETypeType type) const
{
	return (m_T & type) != 0;
}

CStringTypeIdent::CStringTypeIdent(size_t maxlen) :
	CTypeIdent("string", ttString), m_maxlen(maxlen)
{
}

CEnumTypeIdent::CEnumTypeIdent(const string & name, const vector<CEnumConstIdent*>& Enum, ETypeType type/* = ttEnum*/):
	m_vals(Enum), CTypeIdent(name, type)
{
	for (CEnumConstIdent *item : m_vals) {
		item->setType(this);
	}
}

CEnumTypeIdent::CEnumTypeIdent(const vector<CEnumConstIdent*>& Enum):
	CEnumTypeIdent("",Enum)
{
}

bool CEnumTypeIdent::contain(const CTypeIdent * ptype) const
{
	if (auto type = dynamic_cast<const CLimitedTypeIdent*>(ptype)) {
		return type->base()->isEqual(this);
	}
	return false;
}

const vector<CEnumConstIdent*>& CEnumTypeIdent::Enum() const
{
	return m_vals;
}

CEnumConstIdent::CEnumConstIdent(const string & name, CEnumTypeIdent *type) :
	CTypedIdent(name, type, itEnumConst)
{
}

void CEnumConstIdent::setType(const CEnumTypeIdent * type)
{
	m_type = type;
}

CLimitedTypeIdent::CLimitedTypeIdent(const CEnumConstIdent * from, const CEnumConstIdent * to, const CTypeIdent *base):
	CTypeIdent("", ttLimited), m_efrom(from), m_eto(to), m_type(ctEnum), CBasedTypeIdent(base)
{
}

CLimitedTypeIdent::CLimitedTypeIdent(int from, int to, const CTypeIdent *base):
	CTypeIdent("", ttLimited), m_ifrom(from), m_ito(to), m_type(ctInt), CBasedTypeIdent(base)
{
}

CLimitedTypeIdent::CLimitedTypeIdent(unsigned char from, unsigned char to, const CTypeIdent *base):
	CTypeIdent("", ttLimited), m_cfrom(from), m_cto(to), m_type(ctChar), CBasedTypeIdent(base)
{
}

int CLimitedTypeIdent::pos(const CEnumConstIdent * val) const
{
	if (m_type != ctEnum)
		return -1;
	const CEnumTypeIdent *Enum = dynamic_cast<const CEnumTypeIdent *>(m_efrom->type());
	auto vals = Enum->Enum();
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

bool CLimitedTypeIdent::contain(const CTypeIdent * ptype) const
{
	if (auto type = dynamic_cast<const CLimitedTypeIdent*>(ptype)) {
		if (!m_base->isEqual(type->m_base))
			return false;
		int from(-1), to(-1);
		switch (m_type)
		{
		case CLimitedTypeIdent::ctEnum: {
			int from = pos(type->m_efrom);
			int to = pos(type->m_eto);
			break;
		}
		case CLimitedTypeIdent::ctInt: {
			int from = pos(type->m_ifrom);
			int to = pos(type->m_ito);
			break;
		}
		case CLimitedTypeIdent::ctChar: {
			int from = pos(type->m_cfrom);
			int to = pos(type->m_cto);
			break;
		}
		}
		return from >= 0 && from <= to;
	}
	if (auto type = dynamic_cast<const CEnumTypeIdent*>(ptype)) {
		if (!m_base->isEqual(type) || m_type != ctEnum)
			return false;
		return m_efrom == type->Enum().front() && m_eto == type->Enum().back();
	}
	if (auto type = dynamic_cast<const CCharTypeIdent*>(ptype)) {
		if (!m_base->isEqual(type) || m_type != ctChar)
			return false;
		return m_cfrom == 0x0 && m_cto == 0xff;
	}
	return false;
}

bool CRealTypeIdent::contain(const CTypeIdent * ptype) const
{
	if (ptype->isT(ttInt))
		return true;
	return false;
}

CConstIdent::CConstIdent(const string & name, CTypeIdent * type):
	CTypedIdent(name, type, itConst)
{
}

CArrayTypeIdent::CArrayTypeIdent(const vector<const CTypeIdent*> &indexes, const CTypeIdent * base) :
	CTypeIdent("", ttArray), m_indexes(indexes), CBasedTypeIdent(base)
{
}

const vector<const CTypeIdent*>& CArrayTypeIdent::indexes() const
{
	return m_indexes;
}

CBasedTypeIdent::CBasedTypeIdent(const CTypeIdent * base):
	m_base(base)
{
}

const CTypeIdent * CBasedTypeIdent::base() const
{
	return m_base;
}

CNamedTypeIdent::CNamedTypeIdent(const string & name, const CTypeIdent * type) :
	CTypeIdent(name, type->T), m_type(type)
{
}

const CTypeIdent * CNamedTypeIdent::type() const
{
	return m_type->type();
}

bool CNamedTypeIdent::isOrdered() const
{
	return type()->isOrdered();
}

const bool CNamedTypeIdent::isSimple() const
{
	return type()->isSimple();
}

CProcTypeIdent::CProcTypeIdent(const vector<const CTypeIdent*> params):
	CTypeIdent("",ttProc), CParamedTypeIdent(params)
{
}

bool CProcTypeIdent::isEqual(const CTypeIdent * type) const
{
	if (const CProcTypeIdent *proc = dynamic_cast<const CProcTypeIdent *>(type)) {
		return CParamedTypeIdent::isEqual(proc);
	}
	return false;
}

CFuncTypeIdent::CFuncTypeIdent(const vector<const CTypeIdent*> params, const CTypeIdent *resType):
	CTypeIdent("", ttProc), CParamedTypeIdent(params), m_resType(resType)
{
}

const CTypeIdent * CFuncTypeIdent::resType() const
{
	return m_resType;
}

bool CFuncTypeIdent::isEqual(const CTypeIdent * type) const
{
	if (const CFuncTypeIdent *func = dynamic_cast<const CFuncTypeIdent *>(type)) {
		if (!m_resType->isEqual(func->resType()))
			return false;
		return CParamedTypeIdent::isEqual(func);
	}
	return false;
}

CProcIdent::CProcIdent(const string & name, const CTypeIdent * type):
	CTypedIdent(name, type, itProc)
{
}


CTypedIdent::CTypedIdent(const string & name, const CTypeIdent * ttype, EIdentType itype):
	CIdent(name,itype), m_type(ttype)
{
}

const CTypeIdent * CTypedIdent::type() const
{
	return m_type;
}

CFuncIdent::CFuncIdent(const string & name, const CTypeIdent * type) :
	CTypedIdent(name, type, itFunc)
{
}

CParamedTypeIdent::CParamedTypeIdent(const vector<const CTypeIdent*> params):
	m_params(params)
{
}

const vector<const CTypeIdent*>& CParamedTypeIdent::params() const
{
	return m_params;
}

bool CParamedTypeIdent::isEqual(const CParamedTypeIdent * type) const
{
	if (m_params.size() != type->m_params.size())
		return false;
	for (size_t i = 0, cnt(m_params.size()); i < cnt; i++) {
		if (!m_params[i]->isEqual(type->m_params[i])) {
			return false;
		}
	}
	return true;
}

CBooleanTypeIdent::CBooleanTypeIdent() :
	CEnumTypeIdent("boolean", { new CEnumConstIdent("true", this), new CEnumConstIdent("false", this) }, ttBoolean)
{
}
