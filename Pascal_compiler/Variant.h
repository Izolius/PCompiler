#pragma once
#include <string>
using namespace std;

enum EVarType {
	vtInt = 0x1,
	vtReal = 0x2,
	vtString = 0x4,
	vtChar = 0x8
};
class CVariant
{
private:
	EVarType m_T;
public:
	const EVarType &T;
	virtual string ToString() = 0;
protected:
	CVariant(EVarType type);
};

class CIntVariant: public CVariant {
public:
	int m_val;
	CIntVariant(int val);
	virtual string ToString() override;
};

class CRealVariant : public CVariant {
public:
	float m_val;
	CRealVariant(float val);
	virtual string ToString() override;
};

class CStringVariant : public CVariant {
public:
	string m_val;
	CStringVariant(string val);
	virtual string ToString() override;
};

class CCharVariant : public CVariant {
public:
	unsigned char m_val;
	CCharVariant(unsigned char val);
	virtual string ToString() override;
};

