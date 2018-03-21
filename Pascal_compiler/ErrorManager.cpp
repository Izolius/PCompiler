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
	m_str = "������ ���� ������ '" + CToken::to_string(op) + "'";
}

CExpectedError::CExpectedError(CTextPosition pos, EVarType vt):
	CError(pos)
{
	m_str = "������ ���� ��������� ";
	switch (vt)
	{
	case vtInt:
		m_str += "��������������";
		break;
	case vtReal:
		m_str += "�������������";
		break;
	case vtString:
		m_str += "����������";
		break;
	case vtChar:
		m_str += "�����������";
		break;
	}
	m_str += " ����";
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
		return "������ ���� ���";
	case ecUnknownLiter:
		return "����������� ������";
	case ecUnknownName:
		return "����������� �������������";
	case ecWrongRealType:
		return "��� �� ������ ���� REAL";
	case ecWrongLimitedType:
		return "������������ ������������ ���";
	case ecMissedFuncResType:
		return "� �������� ������� �������� ��� ����������";
	case ecWrongParamsCount:
		return "����� ��������� �� ����������� � ���������";
	case ecBooleanOperandType:
		return "��� �������� ������ ���� BOOLEAN";
	case ecVarIsNotArray:
		return "���������� �� ���� ������";
	case ecWrongIndexType:
		return "��� ������� �� ������������� ��������";
	case ecWrongForIteratorType:
		return "������������ ��� ��������� �����";
	case ecWrongExpressionType:
		return "������������ ��� ���������";
	case ecTypesConflict:
		return "�������� �����";
	case ecIncompatableTypes:
		return "���� �� ���������";
	case ecTypeExpected:
		return "��������� ���";
	case ecWrongParamType:
		return "�������� ����� ����������";
	case ecWrongIndexesCount:
		return "���������� �������� �� ����������� � ���������";
	case ecTooMuchInteger:
		return "����� ��������� ��������� ������";
	case ecWrongAndNotOrOperands:
		return "�������� AND, NOT, OR ������ ���� ��������";
	case ecWrongPlusMinusOperandsTypes:
		return "������������ ���� ��������� �������� + ��� -";
	case ecWrongDivModOperandsTypes:
		return "�������� DIV � MOD ������ ���� ������";
	case ecWrongMultOperandsTypes:
		return "������������ ���� ��������� �������� *";
	case ecWrongDivOperandsTypes:
		return "������������ ���� ��������� �������� /";
	case ecWrondIdentType:
		return "�������� ��� ��������������";
	default:
		return "����������� ������";
		break;
	}
	return "";
}
