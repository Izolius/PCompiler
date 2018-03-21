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
public:
	CRegister(ERegister reg, size_t size);
	ERegister getReg() const;
	size_t getSize() const;
	string toString() const;
};

class CGenerator
{
	string m_code;
	bool m_generateCode;
	string m_curProc;
	map<size_t, set<ERegister>> m_freeRegs;
	map<size_t, set<ERegister>> m_busyRegs;
public:
	CGenerator();
	~CGenerator();
	const string &getCode() const;
	void add(const string &row);

	void pushToStack(CVarIdent *var);
	void pushIntToStack(string val);
	void pushFloatToStack(string val);
	void pushToStack(CRegister reg);
	CRegister popFromStack(size_t bytes);
	void writeToStack(CRegister reg, int offset);
	CRegister readFromStack(int offset, size_t bytes);

	CRegister allocReg(size_t bytes);
	void freeReg(CRegister reg);

	void startProc(const string &procName);
	void endProc();
	void Eval(CRegister leftop, CRegister rightop, EOperator op);

	void buildFile(const string &filename) const;
	void StopGenerating();
};

