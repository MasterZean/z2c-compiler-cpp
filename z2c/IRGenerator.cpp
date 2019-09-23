#include "IRGenerator.h"
#include "Assembly.h"
#include "tables.h"

namespace Z2 {
	
void IRGenerator::fillSignedTypeInfo(int64 l, Node* node, ZClass* cls) {
	node->Class = ass.CInt;

	if (l >= -128 && l <= 127) {
		node->C1 = ass.CSmall;
		if (l >= 0)
			node->C2 = ass.CByte;
	}
	else if (l >= 0 && l <= 255) {
		node->C1 = ass.CShort;
		node->C2 = ass.CByte;
	}
	else if (l >= -32768 && l <= 32767) {
		node->C1 = ass.CShort;
		if (l >= 0)
			node->C2 = ass.CWord;
	}
	else if (l >= 0 && l <= 65535) {
		node->C1 = ass.CInt;
		node->C2 = ass.CWord;
	}
	else if (l >= -2147483648LL && l <= 2147483647) {
		node->C1 = ass.CInt;
		if (l >= 0)
			node->C2 = ass.CDWord;
	}
	else {
		node->Class = ass.CLong;
		node->C1 = ass.CLong;
		if (l >= 0 && l <= 4294967295)
			node->C2 = ass.CDWord;
		else if (l >= 0 && l <= 9223372036854775807ul)
			node->C2 = ass.CQWord;
	}
	
	if (cls)
		node->Class = cls;
}

void IRGenerator::fillUnsignedTypeInfo(uint64 l, Node* node, ZClass* cls) {
	node->Class = ass.CDWord;
	node->C1 = ass.CDWord;
	if (l <= 127) {
		node->C1 = ass.CByte;
		node->C2 = ass.CSmall;
	}
	else if (l <= 255)
		node->C1 = ass.CByte;
	else if (l <= 32767) {
		node->C1 = ass.CWord;
		node->C2 = ass.CShort;
	}
	else if (l <= 65535)
		node->C1 = ass.CWord;
	else if (l <= 2147483647)
		node->C2 = ass.CInt;
	else if (l <= 4294967295) {
	}
	else if (l <= 9223372036854775807ul) {
		node->Class = ass.CQWord;
		node->C1 = ass.CQWord;
		node->C2 = ass.CLong;
	}
	else {
		node->Class = ass.CQWord;
		node->C1 = ass.CQWord;
	}
	
	if (cls)
		node->Class = cls;
}

ConstNode* IRGenerator::constIntSigned(int64 l, int base, ZClass* cls) {
	ConstNode* node = constNodes.Get();
	
	fillSignedTypeInfo(l, node, cls);
	
	node->IntVal = l;
	node->IsConst = true;
	node->IsCT = true;
	node->IsLiteral = true;
	node->Base = base;
	node->IsTemporary = true;
	ASSERT(node->Class);
	
	return node;
}

ConstNode* IRGenerator::constIntUnsigned(uint64 l, int base, ZClass* cls) {
	ConstNode* node = constNodes.Get();
	
	fillUnsignedTypeInfo(l, node, cls);
	
	node->IntVal = l;
	node->IsConst = true;
	node->IsCT = true;
	node->IsLiteral = true;
	node->Base = base;
	node->IsTemporary = true;
	ASSERT(node->Class);
	
	return node;
}

ConstNode* IRGenerator::constFloatSingle(double l) {
	ConstNode* node = constNodes.Get();
	
	node->SetType(ass.CFloat);
	node->IsConst = true;
	node->IsLiteral = true;
	node->IsCT = true;
	node->DblVal = l;
	node->IsTemporary = true;
	
	ASSERT(node->Class);
	
	return node;
}

ConstNode* IRGenerator::constFloatDouble(double l) {
	ConstNode* node = constNodes.Get();
	
	node->SetType(ass.CDouble);
	node->IsConst = true;
	node->IsLiteral = true;
	node->IsCT = true;
	node->DblVal = l;
	node->IsTemporary = true;
	
	ASSERT(node->Class);
	
	return node;
}

ConstNode* IRGenerator::constChar(int l, int base) {
	ConstNode* node = constNodes.Get();
	
	if (l < 128)
		node->SetType(ass.CChar, ass.CByte, ass.CSmall);
	else if (l < 256)
		node->SetType(ass.CChar, ass.CByte);
	else
		node->SetType(ass.CChar, ass.CChar);
	
	node->IsConst = true;
	node->IsLiteral = true;
	node->IsCT = true;
	node->IntVal = l;
	node->Base = base;
	node->IsTemporary = true;
	
	ASSERT(node->Class);
	
	return node;
}

ConstNode* IRGenerator::constBool(bool l) {
	ConstNode* node = constNodes.Get();
	
	node->SetType(ass.CBool);
	node->IsConst = true;
	node->IsLiteral = true;
	node->IsCT = true;
	node->IntVal = l;
	node->IsTemporary = true;
	
	ASSERT(node->Class);
	
	return node;
}

ConstNode* IRGenerator::constVoid() {
	ConstNode* node = constNodes.Get();
	
	node->SetType(ass.CVoid);
	node->IsConst = true;
	node->IsLiteral = true;
	node->IsCT = true;
	
	ASSERT(node->Class);
	
	return node;
}

ConstNode* IRGenerator::constNull() {
	ConstNode* node = constNodes.Get();
	
	node->SetType(ass.CNull);
	node->IsConst = true;
	node->IsLiteral = true;
	node->IsCT = true;
	
	ASSERT(node->Class);
	
	return node;
}

ConstNode* IRGenerator::constClass(ZClass* cls/*, Node* e*/) {
	ConstNode* node = constNodes.Get();

	node->SetType(ass.CCls);
	node->IsConst = false;
	node->IsLiteral = true;
	node->IsCT = true;
	node->IntVal = cls->MIndex;
	
	ASSERT(node->Class);

	/*if (e != NULL && e->HasSe) {
		ListNode* l = listNodes.Get();
		l->Params.Add(e);
		l->Params.Add(node);
		l->Tt = node->Tt;
		l->C1 = node->C1;
		l->C2 = node->C2;
		ASSERT(l->Tt.Class);
		return l;
	}
	else*/
		return node;
}

VarNode* IRGenerator::defineLocalVar(Variable& v) {
	VarNode* var = varNodes.Get();
	
	ASSERT(v.Value);
	
	var->Var = &v;
	var->SetType(v.Class);
	var->HasSe = true;
	
	var->IsAddressable = true;
	
	ASSERT(var->Class);
	
	return var;
}

MemNode* IRGenerator::mem(Variable& v) {
	MemNode* var = memNodes.Get();
	
	var->Var = &v;
	var->SetType(v.Class);
	var->HasSe = true;
	
	var->IsAddressable = true;
	
	ASSERT(var->Class);
	
	return var;
}

BlockNode* IRGenerator::openBlock() {
	BlockNode* block = blockNodes.Get();
	block->IntVal = 1;
	return block;
}

BlockNode* IRGenerator::closeBlock() {
	BlockNode* block = blockNodes.Get();
	return block;
}

CastNode* IRGenerator::cast(Node* object, ZClass* cls) {
	if (object->NT == NodeType::Cast) {
		CastNode* c = (CastNode*)object;
		if (c->Class == cls)
			return c;
	}

	CastNode* cast = castNodes.Get();
	
	cast->SetType(cls);
	cast->Object = object;
	cast->Class = cls;
	cast->IsCT = object->IsCT;
	cast->DblVal = object->DblVal;
	cast->IntVal = object->IntVal;
	cast->IsLiteral = object->IsLiteral;
	
	ASSERT(cast->Class);
	
	return cast;
}

CallNode* IRGenerator::call(Overload& over) {
	CallNode* call = callNodes.Get();
	
	call->SetType(over.Return);
	call->Over = &over;
	
	ASSERT(call->Class);
	
	return call;
}

RetNode* IRGenerator::ret(Node* value) {
	RetNode* r = retNodes.Get();
	
	r->SetType(ass.CVoid);
	r->Value = value;
	
	return r;
}

Node* IRGenerator::op(Node* left, Node* right, OpNode::Type op, const Point& p) {
	if (op <= OpNode::opMod)
		return opArit(left, right, op, p);
	else if (op <= OpNode::opShl)
		return opShl(left, right, p);
	else if (op <= OpNode::opShr)
		return opShr(left, right, p);
	/*else if (op <= OpNode::opNeq)
		return opRel(left, right, op, p);
	else if (op <= OpNode::opBitAnd)
		return op_bitand(left, right);
	else if (op <= OpNode::opBitXor)
		return op_bitxor(left, right);
	else if (op <= OpNode::opBitOr)
		return op_bitor(left, right);
	else if (op <= OpNode::opLogOr)
		return opLog(left, right, op);*/
	else {
		ASSERT(0);
		return nullptr;
	}
}

Node* IRGenerator::opArit(Node* left, Node* right, OpNode::Type op, const Point& p) {
	bool valid = left->Class->MIsNumeric && right->Class->MIsNumeric;
	if (!valid)
		return nullptr;
	
	int t1 = left->Class->MIndex;
	int t2 = right->Class->MIndex;
	ASSERT(t1 >= 0 && t1 <= 13);
	ASSERT(t2 >= 0 && t2 <= 13);
	
	int t = TabArithmetic[t1][t2];
	if (t == -1)
		return nullptr;

	ZClass* cls = &ass.Classes[t];
	ZClass* e1 = cls;
	ZClass* e2 = 0;
	
	if (left->Class != right->Class) {
		if (left->Class == right->C1)
			e2 = right->C1;
		if (left->Class == right->C2)
			e2 = right->C2;
		
		if (right->Class == left->C1)
			e2 = left->C1;
		if (right->Class == left->C2)
			e2 = left->C2;
	}
		
	bool ct = left->IsCT && right->IsCT;
	
	int64 dInt;
	double dDouble;
	
	if (ct & FoldConstants) {
		Node* ret = opAritCT(left, right, op, cls, e1, dInt, dDouble);
		if (ret)
			return ret;
	}
	
	OpNode* node = opNodes.Get();

	if (left->C1 == ass.CBool && right->C1 == ass.CBool && e1 == ass.CSmall)
		e2 = ass.CByte;

	node->OpA = left;
	node->OpB = right;
	node->Op = op;

	node->IsCT = ct;
	node->SetType(cls, e1, e2);
	node->IntVal = dInt;
	node->DblVal = dDouble;

	ASSERT(node->Class);
	
	return node;
}

Node* IRGenerator::opAritCT(Node* left, Node* right, OpNode::Type op, ZClass* cls, ZClass* e, int64& dInt, double& dDouble) {
	if (op == OpNode::opAdd) {
		if (e == ass.CByte) {
			dInt = (byte)((uint32)left->IntVal + (uint32)right->IntVal);
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CWord) {
			dInt = (word)((uint32)left->IntVal + (uint32)right->IntVal);
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CDWord) {
			dInt = (dword)((uint32)left->IntVal + (uint32)right->IntVal);
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CQWord) {
			dInt = (qword)((uint64)left->IntVal + (uint64)right->IntVal);
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CSmall) {
			dInt = (int8)((int32)left->IntVal + (int32)right->IntVal);
			if (FoldConstants)
				return constIntSigned(dInt, cls);
		}
		else if (e == ass.CShort) {
			dInt = (int16)((int32)left->IntVal + (int32)right->IntVal);
			if (FoldConstants)
				return constIntSigned(dInt, cls);
		}
		else if (e == ass.CInt) {
			dInt = (int32)((int32)left->IntVal + (int32)right->IntVal);
			if (FoldConstants)
				return constIntSigned(dInt, cls);
		}
		else if (e == ass.CLong) {
			dInt = (int64)((int64)left->IntVal + (int64)right->IntVal);
			if (FoldConstants)
				return constIntSigned(dInt, cls);
		}
		else if (e == ass.CPtrSize) {
			dInt = (uint64)left->IntVal + (uint64)right->IntVal;
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CFloat) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			dDouble = left->DblVal + right->DblVal;
			if (FoldConstants)
				return constFloatSingle(dDouble);
		}
		else if (e == ass.CDouble) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			dDouble = left->DblVal + right->DblVal;
			if (FoldConstants)
				return constFloatDouble(dDouble);
		}
		else if (e == ass.CChar) {
			dInt = (int32)((int32)left->IntVal + (int32)right->IntVal);
			if (FoldConstants)
				return constChar(dInt);
		}
		else
			ASSERT(0);
	}
	else if (op == OpNode::opSub) {
		if (e == ass.CByte) {
			dInt = (byte)((uint32)left->IntVal - (uint32)right->IntVal);
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CWord) {
			dInt = (word)((uint32)left->IntVal - (uint32)right->IntVal);
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CDWord) {
			dInt = (dword)((uint32)left->IntVal - (uint32)right->IntVal);
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CQWord) {
			dInt = (qword)((uint64)left->IntVal - (uint64)right->IntVal);
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CSmall) {
			dInt = (int8)((int32)left->IntVal - (int32)right->IntVal);
			if (FoldConstants)
				return constIntSigned(dInt, cls);
		}
		else if (e == ass.CShort) {
			dInt = (int16)((int32)left->IntVal - (int32)right->IntVal);
			if (FoldConstants)
				return constIntSigned(dInt, cls);
		}
		else if (e == ass.CInt) {
			dInt = (int32)((int32)left->IntVal - (int32)right->IntVal);
			if (FoldConstants)
				return constIntSigned(dInt, cls);
		}
		else if (e == ass.CLong) {
			dInt = (int64)((int64)left->IntVal - (int64)right->IntVal);
			if (FoldConstants)
				return constIntSigned(dInt, cls);
		}
		else if (e == ass.CPtrSize) {
			dInt = (uint64)left->IntVal - (uint64)right->IntVal;
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CFloat) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			dDouble = left->DblVal - right->DblVal;
			if (FoldConstants)
				return constFloatSingle(dDouble);
		}
		else if (e == ass.CDouble) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			dDouble = left->DblVal - right->DblVal;
			if (FoldConstants)
				return constFloatDouble(dDouble);
		}
		else if (e == ass.CChar) {
			dInt = (int32)((int32)left->IntVal - (int32)right->IntVal);
			if (FoldConstants)
				return constChar(dInt);
		}
		else
			ASSERT(0);
	}
	else if (op == OpNode::opMul) {
		if (e == ass.CByte) {
			dInt = (byte)((uint32)left->IntVal * (uint32)right->IntVal);
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CWord) {
			dInt = (word)((uint32)left->IntVal * (uint32)right->IntVal);
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CDWord) {
			dInt = (dword)((uint32)left->IntVal * (uint32)right->IntVal);
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CQWord) {
			dInt = (qword)((uint64)left->IntVal * (uint64)right->IntVal);
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CSmall) {
			dInt = (int8)((int32)left->IntVal * (int32)right->IntVal);
			if (FoldConstants)
				return constIntSigned(dInt, cls);
		}
		else if (e == ass.CShort) {
			dInt = (int16)((int32)left->IntVal * (int32)right->IntVal);
			if (FoldConstants)
				return constIntSigned(dInt, cls);
		}
		else if (e == ass.CInt) {
			dInt = (int32)((int32)left->IntVal * (int32)right->IntVal);
			if (FoldConstants)
				return constIntSigned(dInt, cls);
		}
		else if (e == ass.CLong) {
			dInt = (int64)((int64)left->IntVal * (int64)right->IntVal);
			if (FoldConstants)
				return constIntSigned(dInt, cls);
		}
		else if (e == ass.CPtrSize) {
			dInt = (uint64)left->IntVal * (uint64)right->IntVal;
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CFloat) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			dDouble = left->DblVal * right->DblVal;
			if (FoldConstants)
				return constFloatSingle(dDouble);
		}
		else if (e == ass.CDouble) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			dDouble = left->DblVal * right->DblVal;
			if (FoldConstants)
				return constFloatDouble(dDouble);
		}
		else if (e == ass.CChar) {
			dInt = (int32)((int32)left->IntVal * (int32)right->IntVal);
			if (FoldConstants)
				return constChar(dInt);
		}
		else
			ASSERT(0);
	}
	else if (op == OpNode::opDiv) {
		if (e == ass.CByte) {
			dInt = (byte)((uint32)left->IntVal / (uint32)right->IntVal);
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CWord) {
			dInt = (word)((uint32)left->IntVal / (uint32)right->IntVal);
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CDWord) {
			dInt = (dword)((uint32)left->IntVal / (uint32)right->IntVal);
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CQWord) {
			dInt = (qword)((uint64)left->IntVal / (uint64)right->IntVal);
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CSmall) {
			dInt = (int8)((int32)left->IntVal / (int32)right->IntVal);
			if (FoldConstants)
				return constIntSigned(dInt, cls);
		}
		else if (e == ass.CShort) {
			dInt = (int16)((int32)left->IntVal / (int32)right->IntVal);
			if (FoldConstants)
				return constIntSigned(dInt, cls);
		}
		else if (e == ass.CInt) {
			dInt = (int32)((int32)left->IntVal / (int32)right->IntVal);
			if (FoldConstants)
				return constIntSigned(dInt, cls);
		}
		else if (e == ass.CLong) {
			dInt = (int64)((int64)left->IntVal / (int64)right->IntVal);
			if (FoldConstants)
				return constIntSigned(dInt, cls);
		}
		else if (e == ass.CPtrSize) {
			dInt = (uint64)left->IntVal / (uint64)right->IntVal;
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CFloat) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			dDouble = left->DblVal / right->DblVal;
			if (FoldConstants)
				return constFloatSingle(dDouble);
		}
		else if (e == ass.CDouble) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			dDouble = left->DblVal / right->DblVal;
			if (FoldConstants)
				return constFloatDouble(dDouble);
		}
		else if (e == ass.CChar) {
			dInt = (int32)((int32)left->IntVal / (int32)right->IntVal);
			if (FoldConstants)
				return constChar(dInt);
		}
		else
			ASSERT(0);
	}
	else if (op == OpNode::opMod) {
		if (e == ass.CByte) {
			dInt = (byte)((uint32)left->IntVal % (uint32)right->IntVal);
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CWord) {
			dInt = (word)((uint32)left->IntVal % (uint32)right->IntVal);
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CDWord) {
			dInt = (dword)((uint32)left->IntVal % (uint32)right->IntVal);
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CQWord) {
			dInt = (qword)((uint64)left->IntVal % (uint64)right->IntVal);
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CSmall) {
			dInt = (int8)((int32)left->IntVal % (int32)right->IntVal);
			if (FoldConstants)
				return constIntSigned(dInt, cls);
		}
		else if (e == ass.CShort) {
			dInt = (int16)((int32)left->IntVal % (int32)right->IntVal);
			if (FoldConstants)
				return constIntSigned(dInt, cls);
		}
		else if (e == ass.CInt) {
			dInt = (int32)((int32)left->IntVal % (int32)right->IntVal);
			if (FoldConstants)
				return constIntSigned(dInt, cls);
		}
		else if (e == ass.CLong) {
			dInt = (int64)((int64)left->IntVal % (int64)right->IntVal);
			if (FoldConstants)
				return constIntSigned(dInt, cls);
		}
		else if (e == ass.CPtrSize) {
			dInt = (uint64)left->IntVal % (uint64)right->IntVal;
			if (FoldConstants)
				return constIntUnsigned(dInt, cls);
		}
		else if (e == ass.CFloat) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			dDouble = fmod(left->DblVal, right->DblVal);
			if (FoldConstants)
				return constFloatSingle(dDouble);
		}
		else if (e == ass.CDouble) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			dDouble = fmod(left->DblVal, right->DblVal);
			if (FoldConstants)
				return constFloatDouble(dDouble);
		}
		else if (e == ass.CChar) {
			dInt = (int32)((int32)left->IntVal % (int32)right->IntVal);
			if (FoldConstants)
				return constChar(dInt);
		}
		else
			ASSERT(0);
	}
	else
		ASSERT(0);
	
	return nullptr;
}

Node* IRGenerator::opShl(Node* left, Node* right, const Point& p) {
	bool n = left->Class->MIsNumeric && right->Class->MIsNumeric;
	
	//if (!n)
	//	return GetOp(Over, strops[5], left, right, ass, this, *Comp, p);

	int t1 = left->Class->MIndex;
	int t2 = right->Class->MIndex;
	ASSERT(t1 >= 0 && t1 <= 13);
	ASSERT(t2 >= 0 && t2 <= 13);
	int t = TabShift[t1][t2];
	if (t == -1)
		return nullptr;

	OpNode* node = opNodes.Get();

	node->OpA = left;
	node->OpB = right;
	node->Op = OpNode::opShl;

	node->IsConst = left->IsCT && right->IsCT;
	node->IsCT = node->IsConst;
	node->SetType(ass.Classes[t]);
	node->DblVal = 0;
	
	ASSERT(node->Class);
	
	return node;
}

Node* IRGenerator::opShr(Node* left, Node* right, const Point& p) {
	bool n = left->Class->MIsNumeric && right->Class->MIsNumeric;
	
	//if (!n)
	//	return GetOp(Over, strops[6], left, right, ass, this, *Comp, p);

	int t1 = left->Class->MIndex;
	int t2 = right->Class->MIndex;
	ASSERT(t1 >= 0 && t1 <= 13);
	ASSERT(t2 >= 0 && t2 <= 13);
	int t = TabShift[t1][t2];
	if (t == -1)
		return nullptr;

	OpNode* node = opNodes.Get();

	node->OpA = left;
	node->OpB = right;
	node->Op = OpNode::opShr;

	node->IsConst = left->IsCT && right->IsCT;;
	node->IsCT = node->IsConst;
	node->SetType(ass.Classes[t]);
	node->DblVal = 0;
	
	ASSERT(node->Class);
	
	return node;
}

}
