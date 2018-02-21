#pragma once
#include <vector>
#include "CLib.h"
using namespace std;
#define __ERROR__ -1

struct CError
{
	CTextPosition m_pos;
	size_t m_code;
	CError(CTextPosition pos, size_t code) :m_pos(pos), m_code(code) {}
	CError(CTextPosition pos) :CError(pos, __ERROR__) {}
	CError() : m_pos(0, 0), m_code(__ERROR__) {}
};

class CErrorManager
{
	vector<CError*> m_Errors;
public:
	CErrorManager();
	~CErrorManager();

	void addError(CError *error);
	const vector<CError*> &getErrors()const;
};

