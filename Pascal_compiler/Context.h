#pragma once

#include "Ident.h"
#include <set>
#include <algorithm>

class CContext
{
	set<CTypeIdent*> m_types;
	set<CVarIdent*> m_vars;
	set<CIdent*> m_idents;
	CContext *m_parent;

public:
	CContext(CContext *parent = nullptr);
	~CContext();
	void add(CIdent *ident);
	CTypeIdent *findT(string type, bool brec = true) const;
	CVarIdent *findV(string var, bool brec = true) const;
	CIdent *find(const string &ident, bool brec = true) const;
	CContext *parent() const;
};

