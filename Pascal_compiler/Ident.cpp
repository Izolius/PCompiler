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

bool CIdent::isT(EIdentType type) const
{
	return m_type == type;
}

CIdent::~CIdent()
{
}

CVarIdent::CVarIdent(const string &name, const CTypeIdent *type):
	CTypedIdent(name, type, itVar)
{
}

void CVarIdent::setOffset(int offset)
{
	m_offset = offset;
}

int CVarIdent::getOffset() const
{
	return m_offset;
}

bool CTypeIdent::contain(const CTypeIdent * ptype) const
{
	if (isEqual(ptype))
		return true;
	//if (auto type = type_cast<const CBasedTypeIdent*>(ptype))
	//	if (type->base()->isEqual(this))
	//		return true;
	return false;
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

int CEnumTypeIdent::pos(const CEnumConstIdent * ident) const
{
	auto iter = find(m_vals.begin(), m_vals.end(), ident);
	if (iter == m_vals.end())
		return -1;
	else
		return distance(m_vals.begin(), iter);
}

CEnumConstIdent::CEnumConstIdent(const string & name, CEnumTypeIdent *type) :
	CTypedIdent(name, type, itEnumConst)
{
}

void CEnumConstIdent::setType(const CEnumTypeIdent * type)
{
	m_type = type;
}

CLimitedTypeIdent::CLimitedTypeIdent(const CTypeIdent * base):
	CTypeIdent("",ttLimited), CBasedTypeIdent(base)
{
}

int CLimitedTypeIdent::pos(const CEnumConstIdent * val) const
{
	return -1;
}

int CLimitedTypeIdent::pos(int val) const
{
	return -1;
}

int CLimitedTypeIdent::pos(unsigned char val) const
{
	return -1;
}

bool CLimitedTypeIdent::contain(const CTypeIdent * ptype) const
{
	return false;
}

CIntLimitedTypeIdent::CIntLimitedTypeIdent(int from, int to, const CTypeIdent * base) :
	CLimitedTypeIdent(base), m_from(from), m_to(to)
{
}

int CIntLimitedTypeIdent::pos(int val) const
{
	if (m_from <= val && val <= m_to)
		return val - m_from;
	else
		return -1;
}

int CIntLimitedTypeIdent::left() const
{
	return m_from;
}

bool CIntLimitedTypeIdent::contain(const CTypeIdent * ptype) const
{
	if (auto type = dynamic_cast<decltype(this)>(ptype)) {
		int from(-1), to(-1);
		from = pos(type->m_from);
		to = pos(type->m_to);
		return from >= 0 && from <= to;
	}
	return false;
}


CCharLimitedTypeIdent::CCharLimitedTypeIdent(unsigned char from, unsigned char to, const CTypeIdent * base) :
	CLimitedTypeIdent(base), m_from(from), m_to(to)
{
}

int CCharLimitedTypeIdent::pos(unsigned char val) const
{
	if (m_from <= val && val <= m_to)
		return val - m_from;
	else
		return -1;
}

int CCharLimitedTypeIdent::left() const
{
	return m_from;
}

bool CCharLimitedTypeIdent::contain(const CTypeIdent * ptype) const
{
	if (auto type = dynamic_cast<decltype(this)>(ptype)) {
		int from(-1), to(-1);
		from = pos(type->m_from);
		to = pos(type->m_to);
		return from >= 0 && from <= to;
	}
	if (auto type = dynamic_cast<const CCharTypeIdent*>(ptype)) {
		if (!m_base->isEqual(type))
			return false;
		return m_from == 0x0 && m_to == 0xff;
	}
	return false;
}


CEnumLimitedTypeIdent::CEnumLimitedTypeIdent(const CEnumConstIdent * from, const CEnumConstIdent * to, const CTypeIdent * base):
	CLimitedTypeIdent(base), m_from(from), m_to(to)
{

}

size_t CEnumLimitedTypeIdent::len() const
{
	if (const CEnumTypeIdent *type = dynamic_cast<const CEnumTypeIdent *>(m_base)) {
		return type->pos(m_to) - type->pos(m_from) + 1;
	}
}

int CEnumLimitedTypeIdent::pos(const CEnumConstIdent * val) const
{
	if (auto btype = dynamic_cast<const CEnumTypeIdent *>(m_base))
		if (auto vtype = dynamic_cast<const CEnumTypeIdent *>(val->type()))
			if (btype->isEqual(vtype))
				return vtype->pos(val) - btype->pos(m_from);
	return -1;
}

int CEnumLimitedTypeIdent::left() const
{
	if (auto btype = dynamic_cast<const CEnumTypeIdent *>(m_base))
		return btype->pos(m_from);
	return 0;
}

bool CEnumLimitedTypeIdent::contain(const CTypeIdent * ptype) const
{
	if (auto type = dynamic_cast<decltype(this)>(ptype)) {
		int from(-1), to(-1);
		from = pos(type->m_from);
		to = pos(type->m_to);
		return from >= 0 && from <= to;
	}
	if (auto type = dynamic_cast<const CEnumTypeIdent*>(ptype)) {
		if (!m_base->isEqual(type))
			return false;
		return m_from == type->Enum().front() && m_to == type->Enum().back();
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

size_t CArrayTypeIdent::size() const
{
	return /*len()**/m_base->size();
}

size_t CArrayTypeIdent::len() const
{
	size_t res(1);
	for (const CTypeIdent *index : indexes()) {
		res *= index->len();
	}
	return res;
}

CBasedTypeIdent::CBasedTypeIdent(const CTypeIdent * base):
	m_base(base)
{
}

const CTypeIdent * CBasedTypeIdent::base() const
{
	return m_base->type();
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

size_t CNamedTypeIdent::size() const
{
	return type()->size();
}

size_t CNamedTypeIdent::len() const
{
	return type()->len();
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
