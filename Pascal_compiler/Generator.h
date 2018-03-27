#pragma once

#include <string>
#include <vector>
#include <map>
#include "Ident.h"
using namespace std;

enum ERegister
{
	al, cl, dl, bl, ah, ch, dh, bh,
	ax, cx, dx, bx, sp, bp, si, di,
	eax, ecx, edx, ebx, esp, esi, edi, ebp
};

class CRegister
{
	ERegister m_reg;
	size_t m_size;
	bool m_byVal;
public:
	CRegister(ERegister reg, size_t size);
	ERegister getReg() const;
	size_t getSize() const;
	void setByVal(bool byval);
	string toString() const;
};

class CGenerator
{
	string m_code;
	map<string, string> m_procs;
	bool m_generateCode;
	string m_curProc;
	size_t m_stackSize;
	map<size_t, set<ERegister>> m_freeRegs;
	map<size_t, set<ERegister>> m_busyRegs;
public:
	CGenerator();
	~CGenerator();
	const string &getCode() const;
	void add(const string &row);
	size_t getAddrSize() const;

	void pushToStack(CVarIdent *var);
	void pushIntToStack(string val);
	void pushFloatToStack(string val);
	void pushToStack(CRegister reg);
	CRegister popFromStack(size_t bytes);
	void freeStackSpace(size_t bytes);

	void writeToStack(CRegister what, int offset);
	void writeToStack(CRegister what, CRegister addr);
	CRegister readFromStack(int offset, size_t bytes);
	CRegister readFromStack(CVarIdent *var, size_t deep, bool indexed = false);
	CRegister readAddr(CVarIdent *var, size_t deep, bool indexed = false);
	size_t getStackSize() const;
	void restoreStackStart();

	CRegister allocReg(size_t bytes);
	void freeReg(CRegister reg);

	void startProc(const string &procName);
	void continueProc(const string &procName);
	void endProc();
	void Eval(CRegister leftop, CRegister rightop, EOperator op);
	void EvalProc(const string &procName);

	void printf(CVarIdent *var, size_t deep);

	void buildFile(const string &filename) const;
	void StopGenerating();
};

