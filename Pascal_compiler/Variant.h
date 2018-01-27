#pragma once
#include <string>
using namespace std;

enum EVarType {
	intval = 0x1,
	floatval = 0x2,
	stringval = 0x4
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

class CFloatVariant : public CVariant {
public:
	float m_val;
	CFloatVariant(float val);
	string ToString() override;
};

class CStringVariant : public CVariant {
public:
	string m_val;
	CStringVariant(string val);
	string ToString() override;
};

