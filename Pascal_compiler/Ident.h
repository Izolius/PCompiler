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
	ttProc = 0x200,
	ttFunc = 0x400,

	ttScalar = ttInt | ttChar | ttReal | ttBoolean
};

class CVarIdent;
class CTypeIdent;
class CEnumConstIdent;
class CEnumTypeIdent;

template<class Tto, class Tfrom>
Tto type_cast(Tfrom type) {
	return dynamic_cast<Tto>(type->type());
}

class CIdent
{
public:
	EIdentType m_type;
	string m_name;
	CIdent(const string &name);
	CIdent(const string &name, EIdentType type);
	virtual const CTypeIdent *type() const = 0;
	virtual const string &name() const;
	bool isT(EIdentType type) const;
	~CIdent();
};

class CTypeIdent :public CIdent
{
protected:
	ETypeType m_T;
public:
	const ETypeType &T;
	bool isT(ETypeType type) const;
	bool isT(initializer_list<ETypeType> types) const;
	virtual bool isEqual(const CTypeIdent *type) const;
	virtual bool isOrdered() const { return false; }
	virtual bool contain(const CTypeIdent *ptype) const;
	virtual const CTypeIdent *type() const override;
	virtual const bool isSimple() const;
	virtual size_t size() const = 0;
	virtual int left() const { return 0; }
	virtual size_t len() const = 0;
protected:
	CTypeIdent(const string &name, ETypeType type);
};

class CIntTypeIdent: public CTypeIdent
{
public:
	CIntTypeIdent() :CTypeIdent("integer", ttInt) {}
	virtual bool isOrdered() const override { return true; }
	virtual const bool isSimple() const override { return true; }
	virtual size_t size() const override { return 4; }
	virtual size_t len() const override { return numeric_limits<size_t>::max(); }
	virtual int left() const { return numeric_limits<int>::min(); }
};

class CCharTypeIdent : public CTypeIdent
{
public:
	CCharTypeIdent() :CTypeIdent("char", ttChar) {}
	virtual bool isOrdered() const override { return true; }
	virtual const bool isSimple() const override { return true; }
	virtual size_t size() const override { return 4; }
	virtual size_t len() const override { return numeric_limits<unsigned char>::max(); }
};

class CRealTypeIdent : public CTypeIdent
{
public:
	CRealTypeIdent() :CTypeIdent("real", ttReal) {}
	virtual bool contain(const CTypeIdent *ptype) const override;
	virtual const bool isSimple() const override { return true; }
	virtual size_t size() const override { return 4; }
	virtual size_t len() const override { return numeric_limits<size_t>::max(); }
	virtual int left() const { return numeric_limits<size_t>::min(); }
};

class CStringTypeIdent : public CTypeIdent
{
public:
	const size_t m_maxlen;
	CStringTypeIdent(size_t maxlen = 255);
	virtual size_t size() const override { return m_maxlen; }
	virtual size_t len() const override { return numeric_limits<size_t>::max(); }

};

class CErrorTypeIdent : public CTypeIdent
{
public:
	CErrorTypeIdent() :CTypeIdent("", ttError) {}
	virtual size_t size() const override { return -1; }
	virtual size_t len() const override { return numeric_limits<size_t>::max(); }
};

class CEnumTypeIdent : public CTypeIdent
{
protected:
	const vector<CEnumConstIdent*> m_vals;
public:
	CEnumTypeIdent(const vector<CEnumConstIdent*> &Enum);
	CEnumTypeIdent(const string &name, const vector<CEnumConstIdent*> &Enum, ETypeType type = ttEnum);
	virtual bool isOrdered() const override { return true; }
	virtual bool contain(const CTypeIdent *ptype) const override;
	virtual const bool isSimple() const override { return true; }
	virtual size_t size() const override { return 4; }
	virtual size_t len() const override { return m_vals.size(); }
	const vector<CEnumConstIdent*> &Enum() const;
	int pos(const CEnumConstIdent *ident) const;
};

class CBooleanTypeIdent : public CEnumTypeIdent
{
public:
	CBooleanTypeIdent();
	virtual const bool isSimple() const override { return true; }
	virtual size_t size() const override { return 1; }
	virtual size_t len() const override { return 2; }
};
//
//class CBooleanTypeIdent : public CEnumTypeIdent
//{
//	CBooleanTypeIdent();
//};

class CBasedTypeIdent
{
protected:
	const CTypeIdent *m_base;
public:
	CBasedTypeIdent(const CTypeIdent *base);
	const CTypeIdent *base() const;
};

class CLimitedTypeIdent : public CTypeIdent, public CBasedTypeIdent
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
protected:
	CLimitedTypeIdent(const CTypeIdent *base);
public:
	int pos(const CEnumConstIdent *val) const;
	int pos(int val) const;
	int pos(unsigned char val) const;
	virtual int left() const = 0;
	virtual bool isOrdered() const override { return true; }
	virtual const bool isSimple() const override { return true; }
	virtual size_t size() const override { return m_base->size(); }
	virtual bool contain(const CTypeIdent *ptype) const override;
};

class CIntLimitedTypeIdent : public CLimitedTypeIdent
{
	const int m_from, m_to;
public:
	CIntLimitedTypeIdent(int from, int to, const CTypeIdent *base);
	virtual size_t len() const override { return m_to - m_from + 1; }
	int pos(int val) const;
	virtual int left() const;
	virtual bool contain(const CTypeIdent *ptype) const override;
};

class CCharLimitedTypeIdent : public CLimitedTypeIdent
{
	const unsigned char m_from, m_to;
public:
	CCharLimitedTypeIdent(unsigned char from, unsigned char to, const CTypeIdent *base);
	virtual size_t len() const override { return m_to - m_from + 1; }
	int pos(unsigned char val) const;
	virtual int left() const;
	virtual bool contain(const CTypeIdent *ptype) const override;
};

class CEnumLimitedTypeIdent : public CLimitedTypeIdent
{
	const CEnumConstIdent *m_from, *m_to;
public:
	CEnumLimitedTypeIdent(const CEnumConstIdent * from, const CEnumConstIdent * to, const CTypeIdent *base);
	virtual size_t len() const override;
	int pos(const CEnumConstIdent *val) const;
	virtual int left() const;
	virtual bool contain(const CTypeIdent *ptype) const override;
};

class CArrayTypeIdent : public CTypeIdent, public CBasedTypeIdent
{
	const vector<const CTypeIdent *> m_indexes;
public:
	CArrayTypeIdent(const vector<const CTypeIdent *> &indexes, const CTypeIdent *base);
	const vector<const CTypeIdent *> &indexes() const;
	virtual size_t size() const override;
	virtual size_t len() const override;

};

class CNamedTypeIdent : public CTypeIdent
{
	const CTypeIdent *m_type;
public:
	CNamedTypeIdent(const string &name, const CTypeIdent *type);
	const CTypeIdent *type() const override;
	virtual bool isOrdered() const override;
	virtual const bool isSimple() const override;
	virtual size_t size() const override;
	virtual size_t len() const override;
};

class CParamedTypeIdent
{
protected:
	const vector<const CTypeIdent*> m_params;
public:
	CParamedTypeIdent(const vector<const CTypeIdent*> params);
	const vector<const CTypeIdent*> &params() const;
	bool isEqual(const CParamedTypeIdent *type) const;
};

class CProcTypeIdent : public CTypeIdent, public CParamedTypeIdent
{
public:
	CProcTypeIdent(const vector<const CTypeIdent*> params);
	virtual bool isEqual(const CTypeIdent *type) const override;
	virtual size_t size() const override { return 4; }
	virtual size_t len() const override { return -1; }
};

class CFuncTypeIdent : public CTypeIdent, public CParamedTypeIdent
{
	const CTypeIdent *m_resType;
public:
	CFuncTypeIdent(const vector<const CTypeIdent*> params, const CTypeIdent *resType);
	const CTypeIdent *resType() const;
	virtual bool isEqual(const CTypeIdent *type) const override;
	virtual size_t size() const override { return 4; }
	virtual size_t len() const override { return -1; }
};

class CTypedIdent : public CIdent
{
public:
	const CTypeIdent *m_type;
	CTypedIdent(const string &name, const CTypeIdent *ttype, EIdentType itype);
	virtual const CTypeIdent *type() const override;
};

class CVarIdent : public CTypedIdent
{
	size_t m_offset;
public:
	CVarIdent(const string &name, const CTypeIdent *type);
	void setOffset(int offset);
	int getOffset() const;
};

class CConstIdent : public CTypedIdent
{
public:
	CConstIdent(const string &name, CTypeIdent *type);
};

class CEnumConstIdent : public CTypedIdent
{
public:
	CEnumConstIdent(const string &name, CEnumTypeIdent *type = nullptr);
	void setType(const CEnumTypeIdent *type);
};

class CProcIdent : public CTypedIdent
{
public:
	CProcIdent(const string &name, const CTypeIdent *type);
};

class CFuncIdent : public CTypedIdent
{
public:
	CFuncIdent(const string &name, const CTypeIdent *type);
};