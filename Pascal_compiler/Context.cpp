#include "Context.h"


CContext::CContext(CContext *parent):
	m_parent(parent)
{
	if (!parent) {
		m_idents.insert({
			new CCharTypeIdent(),
			new CIntTypeIdent(),
			new CRealTypeIdent()
		});
	}
}


CContext::~CContext()
{
	for (CVarIdent *var : m_vars) {
		delete var;
	}
	for (CTypeIdent *type : m_types) {
		delete type;
	}
}

void CContext::add(CIdent * ident)
{
	m_idents.insert(ident);
}

CTypeIdent * CContext::findT(string type, bool brec) const
{
	auto iter = find_if(m_types.begin(), m_types.end(),
		[&type](const CTypeIdent *ptype) {return ptype->m_name == type; });
	if (iter != m_types.end()) {
		return *iter;
	}
	else if (brec && m_parent) {
		return m_parent->findT(type, brec);
	}
	return nullptr;
}

CVarIdent * CContext::findV(string var, bool brec) const
{
	auto iter = find_if(m_vars.begin(), m_vars.end(),
		[&var](const CVarIdent *pvar) {return pvar->m_name == var; });
	if (iter != m_vars.end()) {
		return *iter;
	}
	else if (brec && m_parent) {
		return m_parent->findV(var, brec);
	}
	return nullptr;
}

CIdent * CContext::find(const string & ident, bool brec) const
{
	auto iter = find_if(m_idents.begin(), m_idents.end(),
		[&ident](const CIdent *pident) {return pident->m_name == ident; });
	if (iter != m_idents.end()) {
		return *iter;
	}
	else if (brec && m_parent) {
		return m_parent->find(ident, brec);
	}
	return nullptr;
}

CContext * CContext::parent() const
{
	return m_parent;
}
