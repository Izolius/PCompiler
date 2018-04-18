#pragma once

#include "Ident.h"
#include <map>
#include <algorithm>

class CContext
{
	multimap<string, CIdent*> m_idents;
	CIntTypeIdent *m_IntIdent;
	CCharTypeIdent *m_CharIdent;
	CEnumTypeIdent *m_BooleanIdent;
	CErrorTypeIdent *m_ErrorIdent;
	CRealTypeIdent *m_RealIdent;
	CStringTypeIdent *m_StringIdent;
	CContext *m_parent;
	size_t m_NamesCounter;

public:
	CContext(CContext *parent = nullptr);
	~CContext();
	void add(CIdent *ident);
	void add(CVarIdent *var);
	const CTypeIdent *findT(const string &type, bool brec = true) const;
	CVarIdent *findV(const string &var, bool brec = true) const;
	const CEnumConstIdent *findEC(const string &ident, bool brec = true) const;
	CIdent *find(const string &ident, bool brec = true) const;
	size_t deep(const string &ident) const;
	CContext *parent() const;
	set<CVarIdent*> getLocalVars() const;
	string getNewName();

	const CIntTypeIdent *getInteger() const;
	const CRealTypeIdent *getReal() const;
	const CEnumTypeIdent *getBoolean() const;
	const CCharTypeIdent *getChar() const;
	const CErrorTypeIdent *getError() const;
	const CStringTypeIdent* getString() const;

};

