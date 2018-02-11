#include "Context.h"


CContext::CContext(CContext *parent):
	m_parent(parent)
{
	if (!parent) {
		add(m_CharIdent = new CCharTypeIdent());
		add(m_IntIdent = new CIntTypeIdent());
		add(m_RealIdent = new CRealTypeIdent());
		add(m_BooleanIdent = new CBooleanTypeIdent());
		add(m_ErrorIdent = new CErrorTypeIdent());
		add(new CConstIdent("true", m_BooleanIdent));
		add(new CConstIdent("false", m_BooleanIdent));
		
		m_NamesCounter = 0;
	}
}


CContext::~CContext()
{
	for (pair<string, CIdent *>ident : m_idents)
		delete ident.second;
}

void CContext::add(CIdent * ident)
{
	m_idents.insert({ ident->name(), ident });
}

const CTypeIdent * CContext::findT(string type, bool brec) const
{
	return dynamic_cast<CTypeIdent*>(find(type, brec));
}

CVarIdent * CContext::findV(string var, bool brec) const
{
	return nullptr;
}

const CEnumConstIdent * CContext::findEC(const string & ident, bool brec) const
{
	return dynamic_cast<CEnumConstIdent *>(find(ident, brec));
}

CIdent * CContext::find(const string & ident, bool brec) const
{
	auto iter = m_idents.find(ident);
	if (iter != m_idents.end()) {
		return iter->second;
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

string CContext::getNewName()
{
	if (!parent()) {
		//return "ident_______" + to_string(m_NamesCounter++);
		return "";
	}
	return parent()->getNewName();
}

const CIntTypeIdent * CContext::getInteger() const
{
	if (parent())
		return parent()->getInteger();
	return m_IntIdent;
}

const CRealTypeIdent * CContext::getReal() const
{
	if (parent())
		return parent()->getReal();
	return m_RealIdent;
}

const CBooleanTypeIdent * CContext::getBoolean() const
{
	if (parent())
		return parent()->getBoolean();
	return m_BooleanIdent;
}

const CCharTypeIdent * CContext::getChar() const
{
	if (parent())
		return parent()->getChar();
	return m_CharIdent;
}

const CErrorTypeIdent * CContext::getError() const
{
	if (parent())
		return parent()->getError();
	return m_ErrorIdent;
}
