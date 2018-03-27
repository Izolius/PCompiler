#include "Context.h"


CContext::CContext(CContext *parent):
	m_parent(parent)
{
	if (!parent) {
		CEnumConstIdent *True = new CEnumConstIdent("true");
		CEnumConstIdent *False = new CEnumConstIdent("false");
		add(True);
		add(False);
		add(m_CharIdent = new CCharTypeIdent());
		add(m_IntIdent = new CIntTypeIdent());
		add(m_RealIdent = new CRealTypeIdent());
		add(m_BooleanIdent = new CEnumTypeIdent("boolean", { True, False }, ttBoolean));
		add(m_ErrorIdent = new CErrorTypeIdent());
		auto procType = new CProcTypeIdent({ m_IntIdent });
		add(procType);
		add(new CProcIdent("printi", procType));
		
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

void CContext::add(CVarIdent * var)
{
	m_idents.insert({ var->name(), var });
	//var->setOffset(m_stackSize);
	//m_stackSize += var->type()->size();
}

const CTypeIdent * CContext::findT(const string &type, bool brec) const
{
	return dynamic_cast<CTypeIdent*>(find(type, brec));
}

CVarIdent * CContext::findV(const string &var, bool brec) const
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

size_t CContext::deep(const string &ident) const
{
	const CContext *cont = this;
	size_t deep = 0;
	while (!cont->find(ident, false)) {
		deep++;
		cont = cont->m_parent;
	}
	return deep;
}

CContext * CContext::parent() const
{
	return m_parent;
}

set<CVarIdent*> CContext::getLocalVars() const
{
	set<CVarIdent*> res;
	for (const pair<string, CIdent*> &pair : m_idents) {
		if (CVarIdent* var=dynamic_cast<CVarIdent*>(pair.second))
			res.insert(var);
	}
	return res;
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

const CEnumTypeIdent * CContext::getBoolean() const
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
