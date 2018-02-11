#include "Ident.h"
#include "Context.h"


class CTypeUtility
{
public:
	static bool Compatable(const CTypeIdent *typeA, const CTypeIdent *typeB);
	//a:=b;
	static bool CompatableAssign(const CTypeIdent *typeA, const CTypeIdent *typeB);
	static const CTypeIdent *Result(const CTypeIdent *left, EOperator oper, const CTypeIdent *right = nullptr, CContext *context = nullptr);
};
