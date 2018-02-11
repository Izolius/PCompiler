#include "TypeUtility.h"


bool CTypeUtility::Compatable(const CTypeIdent * typeA,const CTypeIdent * typeB)
{
	typeA = typeA->type();
	typeB = typeB->type();
	if (typeA->isT(ttError) || typeB->isT(ttError))
		return true;
	bool res = typeA->isEqual(typeB) ||
		typeA->isT(ttInt) && typeB->isT(ttInt) ||
		typeA->isT(ttReal) && typeB->isT(ttReal) ||
		typeA->isT(ttInt) && typeB->isT(ttReal) ||
		typeA->isT(ttReal) && typeB->isT(ttInt);
	if (res)
		return true;
	auto Abased = dynamic_cast<const CBasedTypeIdent*>(typeA);
	auto Bbased = dynamic_cast<const CBasedTypeIdent*>(typeB);
	if (Abased && !Bbased) {
		res |= Abased->base()->isEqual(typeB);
	}
	if (Bbased && !Abased) {
		res |= Bbased->base()->isEqual(typeA);
	}
	if (Abased && Bbased) {
		res |= Abased->base()->isEqual(Bbased->base());
	}
	return res;
}

bool CTypeUtility::CompatableAssign(const CTypeIdent * typeA,const CTypeIdent * typeB)
{
	typeA = typeA->type();
	typeB = typeB->type();
	if (typeA->isT(ttError) || typeB->isT(ttError))
		return true;
	bool res =
		typeA->isEqual(typeB);
	if (typeA->isOrdered() && typeB->isOrdered() && Compatable(typeA, typeB) && typeA->contain(typeB)) {
		return true;
	}
	if (typeA->isT(ttReal) && typeB->isT(ttInt))
		return true;
	if (typeA->isT(ttString) && typeB->isT(ttString))
		return true;
	if (typeA->isT(ttString) && typeB->isT(ttChar))
		return true;
	return res;
}

const CTypeIdent * CTypeUtility::Result(const CTypeIdent * left, EOperator oper,const CTypeIdent * right/* = nullptr*/, CContext *context/* = nullptr*/)
{
	left = left->type();
	right = right ? right->type() : nullptr;
	if (left->isT(ttError))
		return left;
	if (right && right->isT(ttError))
		return right;
	switch (oper)
	{
	case EOperator::plus:
		if (left->isT(ttString) && right && right->isT({ ttString, ttChar })) {
			return left;
		}
	case EOperator::minus:
	case EOperator::star:
		if (left->isT({ ttReal, ttInt }) && right && right->isT({ ttReal, ttInt })) {
			if (left->isT(ttInt) && right->isT(ttInt))
				return left;
			else
				return left->contain(right) ? left : right;
		}
	case EOperator::slash:
		if (left->isT({ ttReal, ttInt }) && right && right->isT({ ttReal, ttInt })) {
			context->getReal();
		}
		break;
	case EOperator::modsy:
	case EOperator::divsy:
		if (left->isT(ttInt) && right && right->isT(ttInt))
			return left;
		break;
	case EOperator::orsy:
	case EOperator::andsy:
		if (left->isT(ttBoolean) && right && right->isT(ttBoolean))
			return left;
		break;
	case EOperator::notsy:
		if (left->isT(ttBoolean))
			return left;
		break;
	}
	if (context)
		return context->getError();
	else 
		return nullptr;
}
