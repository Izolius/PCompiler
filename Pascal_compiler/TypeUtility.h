#include "Ident.h"
#include "Context.h"


class CTypeUtility
{
public:
	static bool Compatable(CTypeIdent *typeA, CTypeIdent *typeB);
	//a:=b;
	static bool CompatableAssign(CTypeIdent *typeA, CTypeIdent *typeB);
	static CTypeIdent *Result(CTypeIdent *left, EOperator oper, CTypeIdent *right = nullptr, CContext *context = nullptr);
};
