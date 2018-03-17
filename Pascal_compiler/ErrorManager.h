#pragma once
#include <vector>
#include "CLib.h"
#include "Token.h"
using namespace std;
#define __ERROR__ -1

enum EErrorCode
{
	ecUnknownLiter=6,
	ecIdentExpected=2,
	ecSecondlyDescribedIdent=101,
	ecWrongNameUsing=100,
	ecLowerBoundMoreThanBigger=102,
	ecUnknownName=104,
	ecWrongRealType=109,
	ecWrongLimitedType=112,
	ecMissedFuncResType=123,
	ecWrongParamsCount=126,
	ecBooleanOperandType=135,
	ecVarIsNotArray=138,
	ecWrongIndexType=139,
	ecWrongForIteratorType=143,
	ecWrongExpressionType=144,
	ecTypesConflict=145,
	ecIncompatableTypes=182,
	ecTypeExpected,//my
	ecWrongParamType=189,
	ecWrongIndexesCount=193,//194
	ecTooMuchInteger=203,
	ecWrongAndNotOrOperands=210,
	ecWrongPlusMinusOperandsTypes=211,
	ecWrongDivModOperandsTypes=212,
	ecWrongMultOperandsTypes=213,
	ecWrongDivOperandsTypes=214,
	//305??
	ecWrondIdentType,//my



	ecERROR
};

struct CError
{
	CTextPosition m_pos;
	EErrorCode m_code;
	CError(CTextPosition pos, EErrorCode code) :m_pos(pos), m_code(code) {}
	CError(CTextPosition pos) :CError(pos, ecERROR) {}
	CError() : m_pos(0, 0), m_code(ecERROR) {}
	
	virtual string ToString() const;
};

struct CExpectedError : public CError
{
private:
	string m_str;
public:
	CExpectedError(CTextPosition pos, EOperator op);
	CExpectedError(CTextPosition pos, EVarType vt);
	virtual string ToString() const override;
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

