#pragma once
#include <string>
using namespace std;

enum EVarType {
	vtInt = 0x1,
	vtReal = 0x2,
	vtString = 0x4
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
	string ToString() override;
};

class CRealVariant : public CVariant {
public:
	float m_val;
	CRealVariant(float val);
	string ToString() override;
};

class CStringVariant : public CVariant {
public:
	string m_val;
	CStringVariant(string val);
	string ToString() override;
};

