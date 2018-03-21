#include "ErrorManager.h"



CErrorManager::CErrorManager()
{
}


CErrorManager::~CErrorManager()
{
	for (CError *error : m_Errors)
		delete error;
}

void CErrorManager::addError(CError * error)
{
	m_Errors.push_back(error);
}

const vector<CError*>& CErrorManager::getErrors() const
{
	return m_Errors;
}

CExpectedError::CExpectedError(CTextPosition pos, EOperator op):
	CError(pos)
{
	m_str = "должен идти символ '" + CToken::to_string(op) + "'";
}

CExpectedError::CExpectedError(CTextPosition pos, EVarType vt):
	CError(pos)
{
	m_str = "должна идти константа ";
	switch (vt)
	{
	case vtInt:
		m_str += "целочисленного";
		break;
	case vtReal:
		m_str += "вещественного";
		break;
	case vtString:
		m_str += "строкового";
		break;
	case vtChar:
		m_str += "символьного";
		break;
	}
	m_str += " типа";
}

string CExpectedError::ToString() const
{
	return m_str;
}

string CError::ToString() const
{
	switch (m_code)
	{
	case ecIdentExpected:
		return "Должно идти имя";
	case ecUnknownLiter:
		return "Запрещённый символ";
	case ecUnknownName:
		return "Неизвестный идентификатор";
	case ecWrongRealType:
		return "Тип не должен быть REAL";
	case ecWrongLimitedType:
		return "Недопустимый ограниченный тип";
	case ecMissedFuncResType:
		return "В описании функции пропущен тип результата";
	case ecWrongParamsCount:
		return "Число парамеров не согласуется с описанием";
	case ecBooleanOperandType:
		return "Тип операнда должен быть BOOLEAN";
	case ecVarIsNotArray:
		return "Переменная не есть массив";
	case ecWrongIndexType:
		return "Тип индекса не соответствует описанию";
	case ecWrongForIteratorType:
		return "Недопустимый тип параметра цикла";
	case ecWrongExpressionType:
		return "Недопустимый тип выражения";
	case ecTypesConflict:
		return "Конфликт типов";
	case ecIncompatableTypes:
		return "Типы не совместны";
	case ecTypeExpected:
		return "Ожидается тип";
	case ecWrongParamType:
		return "Конфликт типов параметров";
	case ecWrongIndexesCount:
		return "Количество индексов не согласуется с описанием";
	case ecTooMuchInteger:
		return "Целая константа превышает предел";
	case ecWrongAndNotOrOperands:
		return "Операнды AND, NOT, OR должны быть булевыми";
	case ecWrongPlusMinusOperandsTypes:
		return "Недопустимые типа операндов операции + или -";
	case ecWrongDivModOperandsTypes:
		return "Операнды DIV и MOD должны быть целыми";
	case ecWrongMultOperandsTypes:
		return "Недопустимые типы операндов операции *";
	case ecWrongDivOperandsTypes:
		return "Недопустимые типы операндов операции /";
	case ecWrondIdentType:
		return "Неверный тип идентификатора";
	default:
		return "Неизвестная ошибка";
		break;
	}
	return "";
}
