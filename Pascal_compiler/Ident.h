#pragma once

#include <string>
#include <vector>
#include "Variant.h"
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

	ttScalar = ttInt | ttChar | ttReal | ttBoolean
};

class CVarIdent;
class CEnumConstIdent;
class CEnumTypeIdent;

class CIdent
{
public:
	string m_name;
	EIdentType m_type;
	CIdent(const string &name);
	CIdent(const string &name, EIdentType type);
	~CIdent();
};

class CTypeIdent : public CIdent
{
	ETypeType m_T;
public:
	const ETypeType &T;
	bool isT(ETypeType type);
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
};

class CCharTypeIdent : public CSimpleTypeIdent
{
public:
	CCharTypeIdent() :CSimpleTypeIdent("char", ttChar) {}
};

class CRealTypeIdent : public CSimpleTypeIdent
{
public:
	CRealTypeIdent() :CSimpleTypeIdent("real", ttReal) {}
};

class CStringTypeIdent : public CTypeIdent
{
public:
	const size_t m_maxlen;
	CStringTypeIdent(size_t maxlen = 255);
};

class CEnumTypeIdent : public CSimpleTypeIdent
{
public:
	const vector<CEnumConstIdent*> m_vals;
	CEnumTypeIdent(const string &name, const vector<CEnumConstIdent*> &Enum);
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
		const char m_cfrom, m_cto;
	};
public:
	CLimitedTypeIdent(const string &name, CEnumConstIdent *from, CEnumConstIdent *to);
	CLimitedTypeIdent(const string &name, int from, int to);
	CLimitedTypeIdent(const string &name, char from, char to);
	bool contains(CEnumConstIdent *val);
	bool contains(int val);
	bool contains(char val);
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
	const CTypeIdent *m_restype;
	CFuncIdent(const string &name, const vector<CVarIdent*> &params, CTypeIdent *restype);
};