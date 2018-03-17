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
	case ecTypeExpected:
		return "Ожидается тип";
	case ecUnknownName:
		return "Неизвестный идентификатор";
	default:
		break;
	}
	return "";
}
