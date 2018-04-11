#include "Generator.h"
#include <fstream>
#include <iostream>


CGenerator::CGenerator() :
	m_generateCode(true),
	m_LabelNum(0),
	m_offset(1)
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
	m_procs[m_curProc].append(string(m_offset, '\t') + row + "\n");
}

void CGenerator::add(const string & cmd, const string & param1)
{
	m_procs[m_curProc].append(string(m_offset, '\t') + cmd + " " + param1 + '\n');
}

void CGenerator::add(const string & cmd, const string & param1, const string & param2)
{
	m_procs[m_curProc].append(string(m_offset, '\t') + cmd + " " + param1 + ", " + param2 + '\n');
}

size_t CGenerator::getAddrSize() const
{
	return 4;
}

void CGenerator::pushToStack(CVarIdent * var)
{
	add(";add to stack " + var->name());
	const CTypeIdent *type = var->type();
	size_t size = type->size();
	if (type->isT(ttArray))
		size *= type->len();
	add("sub", "esp", to_string(size));
	m_stackSize.top() += size;
	var->setOffset(m_stackSize.top());
}

void CGenerator::freeStackSpace(size_t bytes)
{
	if (bytes) {
		add("; free local variables");
		add("add", "esp", to_string(bytes));
		m_stackSize.top() -= bytes;
	}
}

void CGenerator::saveVarAddr(CVarIdent * var, size_t deep)
{
	CRegister addr = readVarAddr(var, deep);
	pushToStack(addr);
	freeReg(addr);
}

void CGenerator::saveIndexedVarAddr(CRegister & varAddr, const CArrayTypeIdent* vartype)
{
	add(";eval indexed var addr{");
	{
		OffsetIncrementer incer(m_offset);
		CRegister resOffset = allocIntReg(0);
		auto indexes = vartype->indexes();
		size_t offset = 1;
		for (size_t i = 0; i < indexes.size(); i++) {
			auto indexType = indexes[i];
			CRegister index = popFromStack(4);
			if (indexType->left())
				add("sub", index.toString(), to_string(indexType->left()));
			if (offset>1)
				add("imul", index.toString(), to_string(offset));
			if (vartype->base()->isT(ttArray))
				add("imul", index.toString(), to_string(vartype->size()*vartype->base()->len()));
			else
				add("imul", index.toString(), to_string(vartype->size()));
			add("add", resOffset.toString(), index.toString());
			offset *= indexType->len();
			freeReg(index);
		}
		add("add", varAddr.toString(), resOffset.toString());
		freeReg(resOffset);
	}
	add(";}");
}

void CGenerator::pushIntToStack(string val)
{
	add(";push const to stack");
	add("push", "dword " + val);
	m_stackSize.top() += 4;
}

void CGenerator::pushFloatToStack(string val)
{
	add(";push const to stack");
	add("push", "dword " + val);
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
		add("pop", res.toString());
		m_stackSize.top() -= res.getSize();
		return res;
	}
}

void CGenerator::writeToStack(CRegister reg, int offset)
{
	if (reg.getSize() == 4) {
		offset = -(int)reg.getSize();
		string sign = offset >= 0 ? "+" : "";
		add("mov", "[ebp" + sign + to_string(offset) + "]", reg.toString());
	}
}

void CGenerator::writeToStack(CRegister what, CRegister addr)
{
	add("mov", "[" + addr.toString() + "]", what.toString());
}

CRegister CGenerator::readFromStack(int offset, size_t bytes)
{
	CRegister res = allocReg(bytes);
	offset = -(int)bytes;
	string sign = offset >= 0 ? "+" : "";
	add("mov", res.toString(), "[ebp" + sign + to_string(offset) + "]");
	return res;
}

CRegister CGenerator::readFromStack(CVarIdent * var, size_t deep, bool indexed)
{
	if (!indexed) {
		add(";read " + var->name() + " from stack {");
		CRegister res = allocReg(var->type()->size());
		{
			OffsetIncrementer inc(m_offset);
			add("mov", res.toString(), "ebp");
			while (deep) {
				add("mov", res.toString(), "[" + res.toString() + "-" + to_string(getAddrSize()) + "]");
				deep--;
			}
			int offset = -var->getOffset();
			bool pos = offset > 0;
			offset = abs(offset);
			if (pos)
				add("add", res.toString(), to_string(offset));
			else
				add("sub", res.toString(), to_string(offset));
		}
		
		add(";}");
		return res;
	}
}

CRegister CGenerator::readVarAddr(CVarIdent * var, size_t deep, bool indexed)
{
	add(";read addr of " + var->name() + "{");
	CRegister res = allocReg(var->type()->size());
	{
		OffsetIncrementer inc(m_offset);
		add("mov", res.toString() + ", ebp");
		while (deep) {
			add("mov", res.toString(), "[" + res.toString() + "-" + to_string(getAddrSize()) + "]");
			deep--;
		}
		int offset = -var->getOffset();
		bool pos = offset > 0;
		offset = abs(offset);
		if (pos)
			add("add", res.toString(), to_string(offset));
		else
			add("sub", res.toString(), to_string(offset));
	}
	
	add(";}");
	return res;
}

size_t CGenerator::getStackSize() const
{
	return m_stackSize.top();
}

void CGenerator::restoreStackStart()
{
	add("pop", "ebp");
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

CRegister CGenerator::allocIntReg(int val)
{
	CRegister res = allocReg(4);
	add("mov", res.toString(), to_string(val));
	return res;
}

void CGenerator::freeReg(CRegister reg)
{
	m_freeRegs[reg.getSize()].insert(reg.getReg());
}

void CGenerator::startProc(const string & procName)
{
	m_curProc = procName;
	add(procName + ":");
	add("push", "ebp");
	add("mov", "ebp", "esp");
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
		add("call", "[ExitProcess]");
	m_stackSize.pop();
}

void CGenerator::Eval(const CTypeIdent *lefttype, const CTypeIdent *righttype, EOperator op)
{
	CRegister rightop = popFromStack(righttype->size());
	CRegister leftop = popFromStack(lefttype->size());

	if (lefttype->isEqual(righttype) && lefttype->isT(ttInt)) {
		auto compartion = [](EOperator op)->string {
			switch (op)
			{
			case greater: return "jle";
			case later:return "jge";
			case laterequal: return "jg";
			case greaterequal: return "jl";
			case EOperator::equal:return "jne";
			case latergrater: return "je";
			default:
				break;
			}
		};
		switch (op)
		{
		case EOperator::plus:
			add("add", leftop.toString(), rightop.toString());
			break;
		case EOperator::star:
			add("imul", leftop.toString(), rightop.toString());
			break;
		case EOperator::minus:
			add("sub", leftop.toString(), rightop.toString());
			break;
		case greater:
		case later:
		case greaterequal:
		case laterequal:
		case EOperator::equal:
		case latergrater:
			add("cmp", leftop.toString(), rightop.toString());
			add(compartion(op), "__setfalse" + to_string(++m_LabelNum));
			add("mov", leftop.toString(), "1");
			add("jmp", "__toNext" + to_string(m_LabelNum));
			add("__setfalse" + to_string(m_LabelNum) + ":");
			add("mov", leftop.toString(), "0");
			add("__toNext" + to_string(m_LabelNum) + ":");
			break;
		default:
			break;
		}
	}
	else if (lefttype->isEqual(righttype) && lefttype->isT(ttReal)) {
		add("fld", leftop.toString());
		switch (op)
		{
		case EOperator::plus:
			add("fadd", rightop.toString());
			break;
		case EOperator::star:
			add("fmul", rightop.toString());
			break;
		case EOperator::minus:
			add("fsub", rightop.toString());
			break;
		case EOperator::slash:
			add("fdiv", rightop.toString());
			break;
		default:
			break;
		}
		add("fst", leftop.toString());
	}
	pushToStack(leftop);
	freeReg(leftop);
	freeReg(rightop);
}

void CGenerator::EvalAssgn(const CTypeIdent * lefttype, const CTypeIdent * righttype)
{
	add("; assign {");
	{
		OffsetIncrementer incer(m_offset);
		CRegister reg = popFromStack(righttype->size());
		CRegister varAddr = popFromStack(getAddrSize());

		writeToStack(reg, varAddr);
		freeReg(reg);
		freeReg(varAddr);
	}
	add(";}");
}

void CGenerator::EvalProc(const string & procName)
{
	//if (procName == "printi") {
	//	add("; call printi{");
	//	{
	//		OffsetIncrementer inc(m_offset);
	//		add("push __imsg__");
	//		add("call [printf]");
	//		add("add esp, 4");
	//	}
	//	add(";}");
	//	return;
	//}
	//if (procName == "printr") {
	//	add("; call printr{");
	//	{
	//		OffsetIncrementer inc(m_offset);
	//		add("push", "__fmsg__");
	//		add("call", "[printf]");
	//		add("add", "esp", "4");
	//	}
	//	add(";}");
	//	return;
	//}
	add("call", procName);
}

void CGenerator::For_Start()
{
	m_offset++;
	m_LabelNum++;
	m_LabelStack.push(m_LabelNum);
}

void CGenerator::For_Check(CVarIdent *var, size_t deep, bool inc)
{
	add("__forstart" + to_string(m_LabelStack.top()) + ":");
	CRegister iter = readFromStack(var, deep);
	iter.setByVal(true);
	CRegister right = popFromStack(var->type()->size());

	add("clc");
	add("cmp", iter.toString(), right.toString());

	pushToStack(right);
	if (inc) {
		add("jg", "__forend" + to_string(m_LabelStack.top()));
	}
	else {
		add("jl", "__forend" + to_string(m_LabelStack.top()));
	}
	freeReg(right);
	freeReg(iter);
}

void CGenerator::For_Next(CVarIdent * var, size_t deep, bool inc)
{
	CRegister iterAddr = readVarAddr(var, deep);
	iterAddr.setByVal(true);
	if (inc)
		add("inc", iterAddr.toString());
	else
		add("dec",iterAddr.toString());
	add("jmp",  "__forstart" + to_string(m_LabelStack.top()));
	freeReg(iterAddr);
}

void CGenerator::For_End()
{
	add("__forend" + to_string(m_LabelStack.top())+":");
	//add("add", "esp", "4");
	m_LabelStack.pop();
	m_offset--;
}

void CGenerator::If_Start()
{
	m_LabelNum++;
	m_LabelStack.push(m_LabelNum);
}

void CGenerator::If_Check()
{
	CRegister expr = popFromStack(4);
	add("test", expr.toString(), expr.toString());
	add("je", "__labelelse" + to_string(m_LabelStack.top()));

	freeReg(expr);
}

void CGenerator::If_Else()
{
	add("jmp", "__labelend" + to_string(m_LabelStack.top()));
	add("__labelelse" + to_string(m_LabelStack.top()) + ":");
}

void CGenerator::If_End()
{
	add("__labelend" + to_string(m_LabelStack.top()) + ":");
	m_LabelStack.pop();
}

void CGenerator::printf(CVarIdent * var, size_t deep)
{
	if (auto ivar = dynamic_cast<const CIntTypeIdent*>(var->type())) {
		CRegister reg = readFromStack(var, deep);
		add("push", reg.toString());
		add("push", "__msg__");
		add("call", "[printf]");
	}
}

void CGenerator::buildFile(const string & filename)
{
	LoadProcompiled("buildin\\printi.asm");
	LoadProcompiled("buildin\\printr.asm");
	LoadProcompiled("buildin\\printc.asm");
	LoadProcompiled("buildin\\rdata.asm");
	ofstream ofs;
	ofs.open(filename);
	ofs << "format PE console" << endl <<
		"entry main" << endl <<
		"include 'win32a.inc'" << endl <<
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
