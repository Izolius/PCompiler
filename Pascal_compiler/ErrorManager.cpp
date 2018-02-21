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
