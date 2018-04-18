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
	ecTypeDeclError=10,//ошибка в описании типа
	ecSecondlyDescribedIdent=101,//Имя описано повторно
	ecWrongNameUsing=100,//Исопльзование имени не соответствует описанию
	ecLowerBoundMoreThanBigger=102,//нижняяграница превосходит верхнюю
	ecUnknownName=104,//имя не описано
	ecWrongRealType=109,// тип не должен быть REAL
	ecWrongLimitedType=112,//недопустимый ограниченный тип
	ecMissedFuncResType=123,//в описании функции пропущен тип результата
	ecWrongParamsCount=126,//число парамеров не согласуется с описанием
	ecBooleanOperandType=135,//тип операнда должен быть BOOLEAN
	ecVarIsNotArray=138,//переменная не есть массив
	ecWrongIndexType=139,//Тип индекса не соответствует описанию
	ecWrongForIteratorType=143,//недопустимый тип параметра цикла
	ecWrongExpressionType=144,//недопустимый тип выражения
	ecTypesConflict=145,//конфликт типов
	ecIncompatableTypes=182,//типы не совместны
	ecTypeExpected,//my
	ecWrongParamType=189,//конфликт типов параметров
	ecWrongIndexesCount=193,//количество индексов не согласуется с описанием
	ecTooMuchInteger=203,//целая константа превышает предел
	ecWrongAndNotOrOperands=210,//операнды AND, NOT, OR должны быть булевыми
	ecWrongPlusMinusOperandsTypes=211,//недопустимые типа операндов операции + или -
	ecWrongDivModOperandsTypes=212,//операнды DIV и MOD должны быть целыми
	ecWrongMultOperandsTypes=213,//недопустимые типы операндов операции *
	ecWrongDivOperandsTypes=214,//недопустимые типы операндов операции /
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

