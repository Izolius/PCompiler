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
	m_procs[m_curProc].append("\t" + row + "\n");
}

size_t CGenerator::getAddrSize() const
{
	return 4;
}

void CGenerator::pushToStack(CVarIdent * var)
{
	add(";add to stack " + var->name());
	add("sub esp, " + to_string(var->type()->size()));
	m_stackSize.top() += var->type()->size();
	var->setOffset(m_stackSize.top());
}

void CGenerator::freeStackSpace(size_t bytes)
{
	if (bytes) {
		add("; free local variables");
		add("add esp, " + to_string(bytes));
		m_stackSize.top() -= bytes;
	}
}

void CGenerator::pushIntToStack(string val)
{
	add(";push const to stack");
	add("push dword " + val);
	m_stackSize.top() += 4;
}

void CGenerator::pushFloatToStack(string val)
{
	add(";push const to stack");
	add("push dword " + val);
	m_stackSize.top() += 4;
}

void CGenerator::pushToStack(CRegister reg)
{
	add("push " + reg.toString());
	m_stackSize.top() += reg.getSize();
}

CRegister CGenerator::popFromStack(size_t bytes)
{
	if (m_freeRegs.count(bytes)) {
		ERegister reg = *m_freeRegs[bytes].begin();
		m_freeRegs[bytes].erase(reg);
		CRegister res(reg, bytes);
		add("pop " + res.toString());
		m_stackSize.top() -= res.getSize();
		return res;
	}
}

void CGenerator::writeToStack(CRegister reg, int offset)
{
	if (reg.getSize() == 4) {
		offset = -(int)reg.getSize();
		string sign = offset >= 0 ? "+" : "";
		add("mov [ebp" + sign + to_string(offset) + "], " + reg.toString());
	}
}

void CGenerator::writeToStack(CRegister what, CRegister addr)
{
	add("mov [" + addr.toString() + "], " + what.toString());
}

CRegister CGenerator::readFromStack(int offset, size_t bytes)
{
	CRegister res = allocReg(bytes);
	offset = -(int)bytes;
	string sign = offset >= 0 ? "+" : "";
	add("mov " + res.toString() + ", [ebp" + sign + to_string(offset) + "]");
	return res;
}

CRegister CGenerator::readFromStack(CVarIdent * var, size_t deep, bool indexed)
{
	if (!indexed) {
		add(";read " + var->name() + " from stack");
		CRegister res = allocReg(var->type()->size());
		add("mov " + res.toString() + ", ebp");
		while (deep) {
			add("mov " + res.toString() + ", [" + res.toString() + "-" + to_string(getAddrSize()) + "]");
			deep--;
		}
		int offset = -var->getOffset();
		string sign = offset >= 0 ? "+" : "";
		add("mov " + res.toString() + ", [" + res.toString() + sign + to_string(offset) + "]");
		return res;
	}
}

CRegister CGenerator::readAddr(CVarIdent * var, size_t deep, bool indexed)
{
	add(";read addr of " + var->name() + "{");
	CRegister res = allocReg(var->type()->size());
	add("mov " + res.toString() + ", ebp");
	while (deep) {
		add("mov " + res.toString() + ", [" + res.toString() + "-" + to_string(getAddrSize()) + "]");
		deep--;
	}
	int offset = -var->getOffset();
	bool pos = offset > 0;
	offset = abs(offset);
	if (pos)
		add("add " + res.toString() + ", " + to_string(offset));
	else
		add("sub " + res.toString() + ", " + to_string(offset));
	add(";}");
	return res;
}

size_t CGenerator::getStackSize() const
{
	return m_stackSize.top();
}

void CGenerator::restoreStackStart()
{
	add("pop ebp");
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
	add("push ebp");
	add("mov ebp, esp");
	m_stackSize.push(0);
	//m_stackSize = getAddrSize();
}

void CGenerator::continueProc(const string & procName)
{
	m_curProc = procName;
}

void CGenerator::endProc()
{
	if (m_curProc != "main")
		add("ret");
	else
		add("call [ExitProcess]");
	m_stackSize.pop();
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

void CGenerator::EvalProc(const string & procName)
{
	if (procName == "printi") {
		add("; call printi{");
		add("push __imsg__");
		add("call [printf]");
		add("add esp, 4");
		add(";}");
		return;
	}
	if (procName == "printf") {
		add("; call printi{");
		add("push __fmsg__");
		add("call [printf]");
		add("add esp, 4");
		add(";}");
		return;
	}
	add("call " + procName);
}

void CGenerator::printf(CVarIdent * var, size_t deep)
{
	if (auto ivar = dynamic_cast<const CIntTypeIdent*>(var->type())) {
		CRegister reg = readFromStack(var, deep);
		add("push " + reg.toString());
		add("push __msg__");
		add("call [printf]");
	}
}

void CGenerator::buildFile(const string & filename)
{
	LoadProcompiled("buildin\\printi.asm");
	LoadProcompiled("buildin\\rdata.asm");
	ofstream ofs;
	ofs.open(filename);
	ofs << "format PE console" << endl <<
		"entry main" << endl <<
		"include 'D:\\Projects\\Visual_Studio\\Pascal_compiler\\FASM\\win32a.inc'" << endl <<
		"section '.text' code executable" << endl << endl;
	
	for (const pair<string,string> &proc : m_procs) {
		ofs << proc.second << endl;
	}
	for (const string &code : m_precompiled) {
		ofs << code << endl;
	}
	ofs <<"section '.idata' data readable import" << endl <<
		"\tlibrary kernel32, 'kernel32.dll',\\" << endl <<
		"\t\tmsvcrt, 'msvcrt.dll'" << endl <<
		"\timport kernel32, ExitProcess, 'ExitProcess'" << endl <<
		"\timport msvcrt, printf, 'printf'";
	ofs.close();
}

void CGenerator::LoadProcompiled(const string & filename)
{
	ifstream ifs(filename);
	std::string func((std::istreambuf_iterator<char>(ifs)),
		std::istreambuf_iterator<char>());
	m_precompiled.insert(func);
	ifs.close();
}

void CGenerator::StopGenerating()
{
	m_generateCode = false;
}

CRegister::CRegister(ERegister reg, size_t size):
	m_reg(reg), m_size(size), m_byVal(false)
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

void CRegister::setByVal(bool byval)
{
	m_byVal = byval;
}

string CRegister::toString() const
{
	string res = "";
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
		res = "eax";
		break;
	case ecx:
		res = "ecx";
		break;
	case edx:
		res = "edx";
		break;
	case ebx:
		res = "ebx";
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
	if (m_byVal) {
		res = "[" + res + "]";
		switch (getSize())
		{
		case 4:
			res = "dword" + res;
			break;
		default:
			break;
		}
	}
	return res;
}
