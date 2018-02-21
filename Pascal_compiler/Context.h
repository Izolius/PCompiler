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
	CContext *m_parent;
	size_t m_NamesCounter;

public:
	CContext(CContext *parent = nullptr);
	~CContext();
	void add(CIdent *ident);
	const CTypeIdent *findT(const string &type, bool brec = true) const;
	CVarIdent *findV(const string &var, bool brec = true) const;
	const CEnumConstIdent *findEC(const string &ident, bool brec = true) const;
	CIdent *find(const string &ident, bool brec = true) const;
	CContext *parent() const;
	string getNewName();

	const CIntTypeIdent *getInteger() const;
	const CRealTypeIdent *getReal() const;
	const CEnumTypeIdent *getBoolean() const;
	const CCharTypeIdent *getChar() const;
	const CErrorTypeIdent *getError() const;
};

