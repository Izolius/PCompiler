#include "Generator.h"
#include <fstream>
#include <iostream>


CGenerator::CGenerator():
	m_generateCode(true)
{
	m_freeRegs = {
		{ 1,{al,bl,cl,dl}},
		{ 2,{ax,bx,cx,dx}},
		{ 4,{eax,ebx,ecx,edx}}
	};
}


CGenerator::~CGenerator()
{
}

const string & CGenerator::getCode() const
{
	return m_code;
}

void CGenerator::add(const string & row)
{
	m_code.append("\t" + row + "\n");
}

void CGenerator::pushToStack(CVarIdent * var)
{
	add("repeat " + to_string(var->type()->size()));
	add("\tdec esp");
	add("end repeat");
}

void CGenerator::pushIntToStack(string val)
{
	add("push dword " + val);
}

void CGenerator::pushFloatToStack(string val)
{
	add("push dword " + val);
}

void CGenerator::pushToStack(CRegister reg)
{
	add("push " + reg.toString());
}

CRegister CGenerator::popFromStack(size_t bytes)
{
	if (m_freeRegs.count(bytes)) {
		ERegister reg = *m_freeRegs[bytes].begin();
		m_freeRegs[bytes].erase(reg);
		CRegister res(reg, bytes);
		add("pop " + res.toString());
		return res;
	}
}

void CGenerator::writeToStack(CRegister reg, int offset)
{
	if (reg.getSize() == 4) {
		offset = offset - (int)reg.getSize();
		string sign = offset > 0 ? "+" : "";
		add("mov [ebp" + sign + to_string(offset) + "], " + reg.toString());
	}
}

CRegister CGenerator::readFromStack(int offset, size_t bytes)
{
	CRegister res = allocReg(bytes);
	offset = offset - (int)bytes;
	string sign = offset > 0 ? "+" : "";
	add("mov " + res.toString() + ", [ebp" + sign + to_string(offset) + "]");
	return res;
}

CRegister CGenerator::allocReg(size_t bytes)
{
	if (m_freeRegs.count(bytes)) {
		ERegister reg = *m_freeRegs[bytes].begin();
		m_freeRegs[bytes].erase(reg);
		CRegister res(reg, bytes);
		return res;
	}
}

void CGenerator::freeReg(CRegister reg)
{
	m_freeRegs[reg.getSize()].insert(reg.getReg());
}

void CGenerator::startProc(const string & procName)
{
	m_curProc = procName;
	add(procName + ":");
	if (procName == "main") {
		add("mov ebp, esp");
	}
}

void CGenerator::endProc()
{
	if (m_curProc != "main")
		add("ret");
	else
		add("call [ExitProcess]");
}

void CGenerator::Eval(CRegister leftop, CRegister rightop, EOperator op)
{
	switch (op)
	{
	case EOperator::plus:
		add("add " + leftop.toString() + ", " + rightop.toString());
		break;
	case EOperator::star:
		add("imul " + leftop.toString() + ", " + rightop.toString());
		break;
	default:
		break;
	}
}

void CGenerator::buildFile(const string & filename) const
{
	ofstream ofs;
	ofs.open(filename);
	ofs << "format PE console" << endl <<
		"entry main" << endl <<
		"include 'win32a.inc'" << endl <<
		"section '.text' code executable" << endl << endl;
	ofs << m_code << endl << endl;
	ofs << "section '.idata' data readable import" << endl <<
		"library kernel32, 'kernel32.dll',\\" << endl <<
		"msvcrt, 'msvcrt.dll'" << endl <<
		"import kernel32, ExitProcess, 'ExitProcess'" << endl <<
		"import msvcrt, printf, 'printf'";
	ofs.close();
}

void CGenerator::StopGenerating()
{
	m_generateCode = false;
}

CRegister::CRegister(ERegister reg, size_t size):
	m_reg(reg), m_size(size)
{
}

ERegister CRegister::getReg() const
{
	return m_reg;
}

size_t CRegister::getSize() const
{
	return m_size;
}

string CRegister::toString() const
{
	switch (m_reg)
	{
	case al:
		break;
	case cl:
		break;
	case dl:
		break;
	case bl:
		break;
	case ah:
		break;
	case ch:
		break;
	case dh:
		break;
	case bh:
		break;
	case ax:
		break;
	case cx:
		break;
	case dx:
		break;
	case bx:
		break;
	case sp:
		break;
	case bp:
		break;
	case si:
		break;
	case di:
		break;
	case eax:
		return "eax";
	case ecx:
		return "ecx";
	case edx:
		return "edx";
	case ebx:
		break;
	case esp:
		break;
	case esi:
		break;
	case edi:
		break;
	case ebp:
		break;
	default:
		break;
	}
}
