#pragma once

#include <string>
#include <vector>
#include <map>
#include <stack>
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
	set<string> m_precompiled;
	bool m_generateCode;
	string m_curProc;
	stack<size_t> m_stackSize;
	map<size_t, set<ERegister>> m_freeRegs;
	map<size_t, set<ERegister>> m_busyRegs;
	stack<size_t> m_LabelStack;
	size_t m_LabelNum;
	size_t m_offset;
private:
	class OffsetIncrementer
	{
		size_t &m_offset;
	public:
		OffsetIncrementer(size_t &offset) : m_offset(offset) { m_offset++; }
		~OffsetIncrementer() { m_offset--; }
	};
	void add(const string &cmd);
	void add(const string &cmd, const string &param1);
	void add(const string &cmd, const string &param1, const string &param2);
public:
	CGenerator();
	~CGenerator();
	const string &getCode() const;
	size_t getAddrSize() const;

	void pushToStack(CVarIdent *var);
	void pushIntToStack(string val);
	void pushFloatToStack(string val);
	void pushToStack(CRegister reg);
	CRegister popFromStack(size_t bytes);
	void freeStackSpace(size_t bytes);
	void saveVarAddr(CVarIdent *var, size_t deep);
	void saveIndexedVarAddr(CRegister &varAddr, const CArrayTypeIdent* vartype);
	void copyToTop(const CTypeIdent *var);

	void writeToStack(CRegister what, int offset);
	void writeToStack(CRegister what, CRegister addr);
	CRegister readFromStack(int offset, size_t bytes);
	CRegister readFromStack(CVarIdent *var, size_t deep, bool indexed = false);
	CRegister readVarAddr(CVarIdent *var, size_t deep, bool indexed = false);
	
	size_t getStackSize() const;
	void restoreStackStart();

	CRegister allocReg(size_t bytes);
	CRegister allocIntReg(int val);
	void freeReg(CRegister reg);

	void startProc(const string &procName);
	void continueProc(const string &procName);
	void endProc();
	void Eval(const CTypeIdent *lefttype, const CTypeIdent *righttype, EOperator op);
	void EvalAssgn(const CTypeIdent *lefttype, const CTypeIdent *righttype);
	void EvalProc(const string &procName);

	void For_Start();
	void For_Check(CVarIdent *var, size_t deep, bool inc);
	void For_Next(CVarIdent *var, size_t deep, bool inc);
	void For_End();

	void While_Start();
	void While_Check(size_t boolVarSize);
	void While_Next();
	void While_End();

	void If_Start();
	void If_Check();
	void If_Else();
	void If_End();

	void printf(CVarIdent *var, size_t deep);

	void buildFile(const string &filename);
	void LoadProcompiled(const string &filename);
	void StopGenerating();
};

