#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include "Variant.h"
#include "Token.h"
using namespace std;

enum EIdentType {
	itProgram, 
	itType,
	itConst,
	itEnumConst,
	itVar,
	itProc,
	itFunc
};

enum ETypeType {
	ttInt = 0x1,
	ttChar = 0x2,
	ttReal = 0x4,
	ttBoolean = 0x8,
	ttString = 0x10,
	ttArray = 0x20,
	ttEnum = 0x40,
	ttLimited = 0x80,
	ttError = 0x100,

	ttScalar = ttInt | ttChar | ttReal | ttBoolean
};

class CVarIdent;
class CEnumConstIdent;
class CEnumTypeIdent;

class CIdent
{
	virtual void func();
public:
	string m_name;
	EIdentType m_type;
	CIdent(const string &name);
	CIdent(const string &name, EIdentType type);
	~CIdent();
};

class CTypeIdent :public CIdent
{
	ETypeType m_T;
public:
	const ETypeType &T;
	bool isT(ETypeType type) const;
	bool isT(initializer_list<ETypeType> types) const;
	bool isEqual(const CTypeIdent *type);
	virtual bool isOrdered() const { return false; }
	virtual bool contain(CTypeIdent *ptype) const { return false; }
protected:
	CTypeIdent(const string &name, ETypeType type);
};

class CSimpleTypeIdent: public CTypeIdent
{
protected:
	CSimpleTypeIdent(const string &name, ETypeType type);
};

class CIntTypeIdent: public CSimpleTypeIdent
{
public:
	CIntTypeIdent() :CSimpleTypeIdent("integer", ttInt) {}
	virtual bool isOrdered() const override { return true; }
};

class CCharTypeIdent : public CSimpleTypeIdent
{
public:
	CCharTypeIdent() :CSimpleTypeIdent("char", ttChar) {}
	virtual bool isOrdered() const override { return true; }
};

class CRealTypeIdent : public CSimpleTypeIdent
{
public:
	CRealTypeIdent() :CSimpleTypeIdent("real", ttReal) {}
	virtual bool contain(CTypeIdent *ptype) const override;
};

class CStringTypeIdent : public CTypeIdent
{
public:
	const size_t m_maxlen;
	CStringTypeIdent(size_t maxlen = 255);
};

class CErrorTypeIdent : public CTypeIdent
{
public:
	CErrorTypeIdent() :CTypeIdent("", ttError) {}
};

class CEnumTypeIdent : public CSimpleTypeIdent
{
public:
	const vector<CEnumConstIdent*> m_vals;
	CEnumTypeIdent(const string &name, const vector<CEnumConstIdent*> &Enum);
	virtual bool isOrdered() const override { return true; }
	virtual bool contain(CTypeIdent *ptype) const override;
};

class CLimitedTypeIdent : public CTypeIdent
{
	enum EConstType {
		ctEnum,
		ctInt,
		ctChar
	} m_type;
	union 
	{
		const CEnumConstIdent *m_efrom, *m_eto;
		const int m_ifrom, m_ito;
		const unsigned char m_cfrom, m_cto;
	};
	CTypeIdent *m_base;
public:
	CLimitedTypeIdent(const string &name, CEnumConstIdent *from, CEnumConstIdent *to, CTypeIdent *base);
	CLimitedTypeIdent(const string &name, int from, int to, CTypeIdent *base);
	CLimitedTypeIdent(const string &name, unsigned char from, unsigned char to, CTypeIdent *base);
	int pos(const CEnumConstIdent *val) const;
	int pos(int val) const;
	int pos(unsigned char val) const;
	CTypeIdent *base() const;
	virtual bool isOrdered() const override { return true; }
	virtual bool contain(CTypeIdent *ptype) const override;
};

class CVarIdent : public CIdent
{
public:
	CTypeIdent *m_type;
	CVarIdent(const string &name);
};

class CEnumConstIdent : public CIdent
{
public:
	CEnumTypeIdent *m_owner;
	CEnumConstIdent(const string &name, CEnumTypeIdent *owner = nullptr);
};

class CProcIdent : public CIdent
{
public:
	const vector<CVarIdent*> m_params;
	CProcIdent(const string &name, const vector<CVarIdent*> &params);
};

class CFuncIdent : public CIdent
{
public:
	const vector<CVarIdent*> m_params;
	CTypeIdent *m_restype;
	CFuncIdent(const string &name, const vector<CVarIdent*> &params, CTypeIdent *restype);
};