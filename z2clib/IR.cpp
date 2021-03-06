#include "IR.h"
#include "Assembly.h"

String strops[] = { "@add", "@sub", "@mul", "@div", "@mod", "@shl", "@shr", "@less", "@lesseq", "@more", "@moreeq", "@eq", "@neq", "@minus", "@plus", "@not", "@bitnot" };
String ops[]  = { "add", "sub", "mul", "div", "mod", "shl",  "shr",  "less", "lseq", "more", "mreq", "eq",  "neq" "band", "bxor", "bor",  "land", "lor"  };
String opss[] = { "+",   "-",   "*",   "/",   "%",   "<<",   ">>",   "<",    "<=",   ">",    ">=",   "==",  "!=", "&",    "^",    "|",    "&&",   "|| "  , "=",  "+",   "-",  "!",   "~",   "++", "--", "?"  };

int tabAdd[][14] = {
	              /*   b,  s8, u8, s16, u16, s32, u32, s64, u64, f32, f64,  f80, c,   p */
	/*  0: Bool    */ { 1,  1,  2,  3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13 },
	/*  1: Small   */ { 1,  1,  2,  3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13 },
	/*  2: Byte    */ { 2,  2,  2,  3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13 },
	/*  3: Short   */ { 3,  3,  3,  3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13 },
	/*  4: Word    */ { 4,  4,  4,  4,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13 },
	/*  5: Int     */ { 5,  5,  5,  5,   5,   5,   6,   7,   8,   9,   10,  11,  12,  13 },
	/*  6: DWord   */ { 6,  6,  6,  6,   6,   6,   6,   7,   8,   9,   10,  11,  12,  13 },
	/*  7: Long    */ { 7,  7,  7,  7,   7,   7,   7,   7,   8,   9,   10,  11,  12,  13 },
	/*  8: QWord   */ { 8,  8,  8,  8,   8,   8,   8,   8,   8,   9,   10,  11,  12,  13 },
	/*  9: Float   */ { 9,  9,  9,  9,   9,   9,   9,   9,   9,   9,   10,  11,  -1,  -1 },
	/* 10: Double  */ { 10, 10, 10, 10,  10,  10,  10,  10,  10,  10,  10,  11,  -1,  -1 },
	/* 11: Real80  */ { 11, 11, 11, 11,  11,  11,  11,  11,  11,  11,  11,  11,  -1,  -1 },
	/* 12: Char    */ { 12, 12, 12, 12,  12,  12,  12,  12,  12,  -1,  -1,  -1,  12,  -1 },
	/* 13: PtrSize */ { 13, 13, 13, 13,  13,  13,  13,  13,  13,  -1,  -1,  -1,  -1,  13 },
};

int tabRel[][14] = {
	              /*   b,  s8, u8, s16, u16, s32, u32, s64, u64, f32, f64,  f80, c,   p */
	/*  0: Bool    */ { 5,  5,  5,  5,   5,   5,   5,   5,   5,  10,   10,  10,  5,   5  },
	/*  1: Small   */ { 5,  5,  5,  5,   5,   5,   5,   5,   5,  10,   10,  10,  5,   5  },
	/*  2: Byte    */ { 5,  5,  5,  5,   5,   5,   5,   5,   5,  10,   10,  10,  5,   5  },
	/*  3: Short   */ { 5,  5,  5,  5,   5,   5,   5,   5,   5,  10,   10,  10,  5,   5  },
	/*  4: Word    */ { 4,  4,  4,  4,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13 },
	/*  5: Int     */ { 5,  5,  5,  5,   5,   5,   5,   5,   5,  10,   10,  10,  5,   5  },
	/*  6: DWord   */ { 5,  5,  5,  5,   5,   5,   5,   5,   5,  10,   10,  10,  5,   5  },
	/*  7: Long    */ { 5,  5,  5,  5,   5,   5,   5,   5,   5,  10,   10,  10,  5,   5  },
	/*  8: QWord   */ { 5,  5,  5,  5,   5,   5,   5,   5,   5,  10,   10,  10,  5,   5  },
	/*  9: Float   */ { 10, 10, 10, 10,  10,  10,  10,  10,  10, 10,   10,  10,  10,  10 },
	/* 10: Double  */ { 10, 10, 10, 10,  10,  10,  10,  10,  10, 10,   10,  10,  10,  10 },
	/* 11: Real80  */ { 10, 10, 10, 10,  10,  10,  10,  10,  10, 10,   10,  10,  10,  10 },
	/* 12: Char    */ { 5,  5,  5,  5,   5,   5,   5,   5,   5,  10,   10,  10,  5,   5  },
	/* 13: PtrSize */ { 5,  5,  5,  5,   5,   5,   5,   5,   5,  10,   10,  10,  5,   5  },
};

int tabSft[][14] = {
	              /*    b,  s8, u8, s16, u16, s32, u32, s64, u64, f32, f64, f80, c,   p       */
	/*  0: Bool    */ { -1, -1, -1, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1 },
	/*  1: Small   */ { -1,  5,  5,  5,   5,   5,   5,   5,   5,  -1,  -1,  -1,  -1,   5 },
	/*  2: Byte    */ { -1,  6,  6,  6,   6,   6,   6,   6,   6,  -1,  -1,  -1,  -1,   6 },
	/*  3: Short   */ { -1,  5,  5,  5,   5,   5,   5,   5,   5,  -1,  -1,  -1,  -1,   5 },
	/*  4: Word    */ { -1,  6,  6,  6,   6,   6,   6,   6,   6,  -1,  -1,  -1,  -1,   6 },
	/*  5: Int     */ { -1,  5,  5,  5,   5,   5,   5,   5,   5,  -1,  -1,  -1,  -1,   5 },
	/*  6: DWord   */ { -1,  6,  6,  6,   6,   6,   6,   6,   6,  -1,  -1,  -1,  -1,   6 },
	/*  7: Long    */ { -1,  7,  7,  7,   7,   7,   7,   7,   7,  -1,  -1,  -1,  -1,   7 },
	/*  8: QWord   */ { -1,  8,  8,  8,   8,   8,   8,   8,   8,  -1,  -1,  -1,  -1,   8 },
	/*  9: Float   */ { -1,  9,  9,  9,   9,   9,   9,   9,   9,  -1,  -1,  -1,  -1,   9 },
	/* 10: Double  */ { -1, 10, 10, 10,  10,  10,  10,  10,  10,  -1,  -1,  -1,  -1,  10 },
	/* 11: Real80  */ { -1, 11, 11, 11,  11,  11,  11,  11,  11,  -1,  -1,  -1,  -1,  11 },
	/* 12: Char    */ { -1, 12, 12, 12,  12,  12,  12,  12,  12,  -1,  -1,  -1,  -1,  12 },
	/* 13: PtrSize */ { -1, 13, 13, 13,  13,  13,  13,  13,  13,  -1,  -1,  -1,  -1,  13 },
};

Node* GetOp(Overload* overload, const String& s, Node* left, Node* right, Assembly& ass, IR* ast, Compiler& comp, const Point&p);
Node* GetOp(Overload* overload, const String& s, Node* left, Assembly& ass, IR* ast, Compiler& comp, const Point& p);

inline bool testOpRel(Assembly& ass, Node* left, Node* right, int op) {
	bool valid = ass.IsNumeric(left->Tt) && ass.IsNumeric(right->Tt);
	if (valid == false) {
		valid = valid || (left->Tt.Class == ass.CPtr && right->Tt.Class == ass.CPtr);
		if (op == 11 || op == 12) {
			valid = valid || (left->Tt.Class == ass.CPtr && right->Tt.Class == ass.CNull);
			valid = valid || (left->Tt.Class == ass.CNull && right->Tt.Class == ass.CPtr);
		}
	}
	return valid;
}

void IR::fillSignedTypeInfo(int64 l, Node* node, ZClass* cls) {
	node->Tt = ass.CInt->Tt;
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
		node->Tt = ass.CLong->Tt;
		node->C1 = ass.CLong;
		if (l >= 0 && l <= 4294967295)
			node->C2 = ass.CDWord;
		else if (l >= 0 && l <= 9223372036854775807ul)
			node->C2 = ass.CQWord;
	}
	//if (cls)
	//	node->Tt = cls->Tt;
}

void IR::fillUnsignedTypeInfo(uint64 l, Node* node, ZClass* cls) {
	node->Tt = ass.CDWord->Tt;
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
		node->Tt = ass.CQWord->Tt;
		node->C1 = ass.CQWord;
		node->C2 = ass.CLong;
	}
	else {
		node->Tt = ass.CQWord->Tt;
		node->C1 = ass.CQWord;
	}
	if (cls)
		node->Tt = cls->Tt;
}

ConstNode* IR::const_i(int64 l, ZClass* cls, int base) {
	ConstNode* node = constNodes.Get();
	
	fillSignedTypeInfo(l, node, cls);
	
	node->IntVal = l;
	node->IsConst = true;
	node->IsCT = true;
	node->IsLiteral = true;
	node->Base = base;
	node->IsMove = true;
	ASSERT(node->Tt.Class);
	
	return node;
}

ConstNode* IR::const_u(uint64 l, ZClass* cls, int base) {
	ConstNode* node = constNodes.Get();
	
	fillUnsignedTypeInfo(l, node, cls);
	
	node->IntVal = l;
	node->IsConst = true;
	node->IsCT = true;
	node->IsLiteral = true;
	node->Base = base;
	node->IsMove = true;
	ASSERT(node->Tt.Class);
	
	return node;
}

ConstNode* IR::const_r32(double l) {
	ConstNode* node = constNodes.Get();
	
	node->SetType(ass.CFloat->Tt);
	node->IsConst = true;
	node->IsLiteral = true;
	node->IsCT = true;
	node->DblVal = l;
	node->IsMove = true;
	ASSERT(node->Tt.Class);
	
	return node;
}

ConstNode* IR::const_r64(double l) {
	ConstNode* node = constNodes.Get();
	
	node->SetType(ass.CDouble->Tt);
	node->IsConst = true;
	node->IsLiteral = true;
	node->IsCT = true;
	node->DblVal = l;
	node->IsMove = true;
	ASSERT(node->Tt.Class);
	
	return node;
}

ConstNode* IR::const_bool(bool l) {
	ConstNode* node = constNodes.Get();
	
	node->SetType(ass.CBool->Tt);
	node->IsConst = true;
	node->IsLiteral = true;
	node->IsCT = true;
	node->IntVal = l;
	node->IsMove = true;
	ASSERT(node->Tt.Class);
	
	return node;
}

ConstNode* IR::const_char(int l, int base) {
	ConstNode* node = constNodes.Get();
	
	if (l < 128)
		node->SetType(&ass.CChar->Tt, ass.CByte, ass.CSmall);
	else if (l < 256)
		node->SetType(&ass.CChar->Tt, ass.CByte, nullptr);
	else
		node->SetType(&ass.CChar->Tt, ass.CChar, nullptr);
	
	node->IsConst = true;
	node->IsLiteral = true;
	node->IsCT = true;
	node->IntVal = l;
	node->Base = base;
	node->IsMove = true;
	
	ASSERT(node->Tt.Class);
	return node;
}

ConstNode* IR::const_void() {
	ConstNode* node = constNodes.Get();
	node->SetType(ass.CVoid->Tt);
	node->IsConst = true;
	node->IsLiteral = true;
	node->IsCT = true;
	
	ASSERT(node->Tt.Class);
	return node;
}

ConstNode* IR::const_null() {
	ConstNode* node = constNodes.Get();
	node->SetType(ass.CNull->Tt);
	node->IsConst = true;
	node->IsLiteral = true;
	node->IsCT = true;
	
	ASSERT(node->Tt.Class);
	return node;
}

ConstNode* IR::const_str(int index) {
	ConstNode* node = constNodes.Get();
	node->SetType(ass.CString->Tt);
	node->IsConst = true;
	node->IsLiteral = true;
	node->IsCT = true;
	node->IntVal = index;
	node->IsAddressable = true;
	
	ASSERT(node->Tt.Class);
	return node;
}

Node* IR::opArit(Node* left, Node* right, OpNode::Type op, const Point& p) {
	bool valid = ass.IsNumeric(left->Tt) && ass.IsNumeric(right->Tt);
	bool valid2 = false;
	bool valid3 = false;
	
	if ((op == 0 || op == 1) && (
			(left->Tt.Class == ass.CPtr && ass.IsInteger(right->Tt)) ||
			(right->Tt.Class == ass.CPtr && ass.IsInteger(left->Tt))))
		valid2 = true;
	else if (op == 1 &&
			(left->Tt.Class == ass.CPtr && right->Tt.Class == ass.CPtr))
		valid3 = true;
	
	Overload* over = nullptr;
	ZClass* cls = nullptr;
	if (!valid && !valid2 && !valid3)
		return GetOp(Over, strops[op], left, right, ass, this, *Comp, Point(1, 1));

	ObjectType* type = 0;
	ObjectType* e = 0;
	ZClass* secType = 0;
	bool cst = false;
	
	if (valid2 == false && valid3 == false) {
		int t1 = left->Tt.Class->Index - 4;
		int t2 = right->Tt.Class->Index - 4;
		ASSERT(t1 >= 0 && t1 <= 13);
		ASSERT(t2 >= 0 && t2 <= 13);
		
		int t = tabAdd[t1][t2];

		if (t == -1)
			return NULL;

		t += 4;

		cst = left->IsCT && right->IsCT;
		type = &ass.Classes[t].Tt;
		
		if (left->Tt.Class != right->Tt.Class) {
			if (left->Tt.Class == right->C1)
				secType = right->C1;
			if (left->Tt.Class == right->C2)
				secType = right->C2;
			
			if (right->Tt.Class == left->C1)
				secType = left->C1;
			if (right->Tt.Class == left->C2)
				secType = left->C2;
		}
		
		e = type;
	}
	else if (valid2 == true) {
		if (ass.IsPtr(left->Tt))
			type = &left->Tt;
		else if (ass.IsPtr(right->Tt))
			type = &right->Tt;
		e = type;
	}
	else  if (valid3 == true) {
		type = &ass.CPtrSize->Tt;
		e = type;
	}

	bool backCst = cst;
		
	int64 dInt;
	double dDouble;

	if (cst) {
		if (op == OpNode::opAdd) {
			if (e->Class == ass.CByte) {
				dInt = (byte)((uint32)left->IntVal + (uint32)right->IntVal);
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CWord) {
				dInt = (word)((uint32)left->IntVal + (uint32)right->IntVal);
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CDWord) {
				dInt = (dword)((uint32)left->IntVal + (uint32)right->IntVal);
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CQWord) {
				dInt = (qword)((uint64)left->IntVal + (uint64)right->IntVal);
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CSmall) {
				dInt = (int8)((int32)left->IntVal + (int32)right->IntVal);
				if (FoldConstants)
					return const_i(dInt, type->Class);
			}
			else if (e->Class == ass.CShort) {
				dInt = (int16)((int32)left->IntVal + (int32)right->IntVal);
				if (FoldConstants)
					return const_i(dInt, type->Class);
			}
			else if (e->Class == ass.CInt) {
				dInt = (int32)((int32)left->IntVal + (int32)right->IntVal);
				if (FoldConstants)
					return const_i(dInt, type->Class);
			}
			else if (e->Class == ass.CLong) {
				dInt = (int64)((int64)left->IntVal + (int64)right->IntVal);
				if (FoldConstants)
					return const_i(dInt, type->Class);
			}
			else if (e->Class == ass.CPtrSize) {
				dInt = (uint64)left->IntVal + (uint64)right->IntVal;
				type = &ass.CPtrSize->Tt;
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CFloat) {
				left->PromoteToFloatValue(ass);
				right->PromoteToFloatValue(ass);
				dDouble = left->DblVal + right->DblVal;
				if (FoldConstants)
					return const_r32(dDouble);
			}
			else if (e->Class == ass.CDouble) {
				left->PromoteToFloatValue(ass);
				right->PromoteToFloatValue(ass);
				dDouble = left->DblVal + right->DblVal;
				if (FoldConstants)
					return const_r64(dDouble);
			}
			else
				ASSERT(0);
		}
		else if (op == OpNode::opSub) {
			if (e->Class == ass.CByte) {
				dInt = (byte)((uint32)left->IntVal - (uint32)right->IntVal);
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CWord) {
				dInt = (word)((uint32)left->IntVal - (uint32)right->IntVal);
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CDWord) {
				dInt = (dword)((uint32)left->IntVal - (uint32)right->IntVal);
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CQWord) {
				dInt = (qword)((uint64)left->IntVal - (uint64)right->IntVal);
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CSmall) {
				dInt = (int8)((int32)left->IntVal - (int32)right->IntVal);
				if (FoldConstants)
					return const_i(dInt, type->Class);
			}
			else if (e->Class == ass.CShort) {
				dInt = (int16)((int32)left->IntVal - (int32)right->IntVal);
				if (FoldConstants)
					return const_i(dInt, type->Class);
			}
			else if (e->Class == ass.CInt) {
				dInt = (int32)((int32)left->IntVal - (int32)right->IntVal);
				if (FoldConstants)
					return const_i(dInt, type->Class);
			}
			else if (e->Class == ass.CLong) {
				dInt = (int64)((int64)left->IntVal - (int64)right->IntVal);
				if (FoldConstants)
					return const_i(dInt, type->Class);
			}
			else if (e->Class == ass.CPtrSize) {
				dInt = (uint64)left->IntVal - (uint64)right->IntVal;
				type = &ass.CPtrSize->Tt;
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CFloat) {
				left->PromoteToFloatValue(ass);
				right->PromoteToFloatValue(ass);
				dDouble = left->DblVal - right->DblVal;
				if (FoldConstants)
					return const_r32(dDouble);
			}
			else if (e->Class == ass.CDouble) {
				left->PromoteToFloatValue(ass);
				right->PromoteToFloatValue(ass);
				dDouble = left->DblVal - right->DblVal;
				if (FoldConstants)
					return const_r64(dDouble);
			}
			else
				ASSERT(0);
		}
		else if (op == OpNode::opMul) {
			if (e->Class == ass.CByte) {
				dInt = (byte)((uint32)left->IntVal * (uint32)right->IntVal);
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CWord) {
				dInt = (word)((uint32)left->IntVal * (uint32)right->IntVal);
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CDWord) {
				dInt = (dword)((uint32)left->IntVal * (uint32)right->IntVal);
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CQWord) {
				dInt = (qword)((uint64)left->IntVal * (uint64)right->IntVal);
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CSmall) {
				dInt = (int8)((int32)left->IntVal * (int32)right->IntVal);
				if (FoldConstants)
					return const_i(dInt, type->Class);
			}
			else if (e->Class == ass.CShort) {
				dInt = (int16)((int32)left->IntVal * (int32)right->IntVal);
				if (FoldConstants)
					return const_i(dInt, type->Class);
			}
			else if (e->Class == ass.CInt) {
				dInt = (int32)((int32)left->IntVal * (int32)right->IntVal);
				if (FoldConstants)
					return const_i(dInt, type->Class);
			}
			else if (e->Class == ass.CLong) {
				dInt = (int64)((int64)left->IntVal * (int64)right->IntVal);
				if (FoldConstants)
					return const_i(dInt, type->Class);
			}
			else if (e->Class == ass.CPtrSize) {
				dInt = (uint64)left->IntVal * (uint64)right->IntVal;
				type = &ass.CPtrSize->Tt;
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CFloat) {
				left->PromoteToFloatValue(ass);
				right->PromoteToFloatValue(ass);
				dDouble = left->DblVal * right->DblVal;
				if (FoldConstants)
					return const_r32(dDouble);
			}
			else if (e->Class == ass.CDouble) {
				left->PromoteToFloatValue(ass);
				right->PromoteToFloatValue(ass);
				dDouble = left->DblVal * right->DblVal;
				if (FoldConstants)
					return const_r64(dDouble);
			}
			else
				ASSERT(0);
		}
		else if (op == OpNode::opDiv) {
			if (e->Class == ass.CByte) {
				dInt = (byte)((uint32)left->IntVal / (uint32)right->IntVal);
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CWord) {
				dInt = (word)((uint32)left->IntVal / (uint32)right->IntVal);
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CDWord) {
				dInt = (dword)((uint32)left->IntVal / (uint32)right->IntVal);
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CQWord) {
				dInt = (qword)((uint64)left->IntVal / (uint64)right->IntVal);
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CSmall) {
				dInt = (int8)((int32)left->IntVal / (int32)right->IntVal);
				if (FoldConstants)
					return const_i(dInt, type->Class);
			}
			else if (e->Class == ass.CShort) {
				dInt = (int16)((int32)left->IntVal / (int32)right->IntVal);
				if (FoldConstants)
					return const_i(dInt, type->Class);
			}
			else if (e->Class == ass.CInt) {
				dInt = (int32)((int32)left->IntVal / (int32)right->IntVal);
				if (FoldConstants)
					return const_i(dInt, type->Class);
			}
			else if (e->Class == ass.CLong) {
				dInt = (int64)((int64)left->IntVal / (int64)right->IntVal);
				if (FoldConstants)
					return const_i(dInt, type->Class);
			}
			else if (e->Class == ass.CPtrSize) {
				dInt = (uint64)left->IntVal / (uint64)right->IntVal;
				type = &ass.CPtrSize->Tt;
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CFloat) {
				left->PromoteToFloatValue(ass);
				right->PromoteToFloatValue(ass);
				dDouble = left->DblVal / right->DblVal;
				if (FoldConstants)
					return const_r32(dDouble);
			}
			else if (e->Class == ass.CDouble) {
				left->PromoteToFloatValue(ass);
				right->PromoteToFloatValue(ass);
				dDouble = left->DblVal / right->DblVal;
				if (FoldConstants)
					return const_r64(dDouble);
			}
			else
				ASSERT(0);
		}
		else if (op == OpNode::opMod) {
			if (e->Class == ass.CByte) {
				dInt = (byte)((uint32)left->IntVal % (uint32)right->IntVal);
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CWord) {
				dInt = (word)((uint32)left->IntVal % (uint32)right->IntVal);
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CDWord) {
				dInt = (dword)((uint32)left->IntVal % (uint32)right->IntVal);
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CQWord) {
				dInt = (qword)((uint64)left->IntVal % (uint64)right->IntVal);
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CSmall) {
				dInt = (int8)((int32)left->IntVal % (int32)right->IntVal);
				if (FoldConstants)
					return const_i(dInt, type->Class);
			}
			else if (e->Class == ass.CShort) {
				dInt = (int16)((int32)left->IntVal % (int32)right->IntVal);
				if (FoldConstants)
					return const_i(dInt, type->Class);
			}
			else if (e->Class == ass.CInt) {
				dInt = (int32)((int32)left->IntVal % (int32)right->IntVal);
				if (FoldConstants)
					return const_i(dInt, type->Class);
			}
			else if (e->Class == ass.CLong) {
				dInt = (int64)((int64)left->IntVal % (int64)right->IntVal);
				if (FoldConstants)
					return const_i(dInt, type->Class);
			}
			else if (e->Class == ass.CPtrSize) {
				dInt = (uint64)left->IntVal % (uint64)right->IntVal;
				type = &ass.CPtrSize->Tt;
				if (FoldConstants)
					return const_u(dInt, type->Class);
			}
			else if (e->Class == ass.CFloat) {
				left->PromoteToFloatValue(ass);
				right->PromoteToFloatValue(ass);
				dDouble = fmod(left->DblVal, right->DblVal);
				if (FoldConstants)
					return const_r32(dDouble);
			}
			else if (e->Class == ass.CDouble) {
				left->PromoteToFloatValue(ass);
				right->PromoteToFloatValue(ass);
				dDouble = fmod(left->DblVal, right->DblVal);
				if (FoldConstants)
					return const_r64(dDouble);
			}
			else
				ASSERT(0);
		}
		else
			ASSERT(0);

	}

	OpNode* node = opNodes.Get();

	if (left->C1 == ass.CBool && right->C1 == ass.CBool && e->Class == ass.CSmall)
		secType = ass.CByte;

	node->OpA = left->IsRef ? deref(left) : left;
	node->OpB = right->IsRef ? deref(right) : right;
	node->Op = op;

	node->IsCT = backCst;
	node->SetType(type, e->Class, secType);
	node->IntVal = dInt;
	node->DblVal = dDouble;

	ASSERT(node->Tt.Class);
	
	return node;
}

Node* IR::opRel(Node* left, Node* right, OpNode::Type op, const Point& p) {
	Node* l = (left->IsRef) ? deref(left) : left;
	Node* r = (right->IsRef) ? deref(right) : right;

	if (l->Tt.Class->Scan.IsEnum && r->Tt.Class->Scan.IsEnum && l->Tt.Class == r->Tt.Class) {
		l = cast(l, &ass.CInt->Tt);
		r = cast(r, &ass.CInt->Tt);
	}

	bool valid = false;
	
	if ((op == OpNode::opEq || op == OpNode::opNeq) && left->Tt.Class == ass.CCls && right->Tt.Class == ass.CCls)
		return const_bool(left->IntVal == right->IntVal);
	else if (testOpRel(ass, l, r, op) == false) {
		Node* over = GetOp(Over, strops[op], l, r, ass, this, *Comp, p);
		if (over == nullptr) {
			if (op == OpNode::opLess)
				return GetOp(Over, strops[OpNode::opMore], r, l, ass, this, *Comp, p);
			else if (op == OpNode::opLessEq)
				return GetOp(Over, strops[OpNode::opMoreEq], r, l, ass, this, *Comp, p);
			else if (op == OpNode::opMore)
				return GetOp(Over, strops[OpNode::opLess], r, l, ass, this, *Comp, p);
			else if (op == OpNode::opMoreEq)
				return GetOp(Over, strops[OpNode::opLessEq], r, l, ass, this, *Comp, p);
			else
				return nullptr;
		}
		else
			return over;
	}
	
	valid = left->Tt.Class == ass.CPtr && right->Tt.Class == ass.CPtr;
	valid = valid || (left->Tt.Class == ass.CPtr && right->Tt.Class == ass.CNull);
	valid = valid || (left->Tt.Class == ass.CNull && right->Tt.Class == ass.CPtr);

	left = l;
	right = r;

	bool cst = left->IsCT && right->IsCT;
	bool b;

	if (valid) {
		OpNode* node = opNodes.Get();

		node->OpA = left;
		node->OpB = right;
		node->Op = op;

		node->IsConst = cst;
		node->IsCT = cst;
		node->SetType(ass.CBool->Tt);

		return node;
	}

	int t1 = left->Tt.Class->Index - 4;
	int t2 = right->Tt.Class->Index - 4;
	ASSERT(t1 >= 0 && t1 <= 13);
	ASSERT(t2 >= 0 && t2 <= 13);
	int t = tabRel[t1][t2];

	if (t == -1)
		return NULL;

	t += 4;

	ObjectType* e = &ass.Classes[t].Tt;

	if (e->Class == ass.CByte || e->Class == ass.CWord || e->Class == ass.CDWord) {
		if (cst) {
			if (op == OpNode::opLess)
				b = (uint64)left->IntVal < (uint64)right->IntVal;
			else if (op == OpNode::opLessEq)
				b = (uint64)left->IntVal <= (uint64)right->IntVal;
			else if (op == OpNode::opMore)
				b = (uint64)left->IntVal > (uint64)right->IntVal;
			else if (op == OpNode::opMoreEq)
				b = (uint64)left->IntVal >= (uint64)right->IntVal;
			else if (op == OpNode::opEq)
				b = (uint64)left->IntVal == (uint64)right->IntVal;
			else if (op == OpNode::opNeq)
				b = (uint64)left->IntVal != (uint64)right->IntVal;
			else
				ASSERT(0);
			
			if (FoldConstants)
				return const_bool(b);
		}
	}
	else if (e->Class == ass.CSmall || e->Class == ass.CShort || e->Class == ass.CInt) {
		if (cst) {
			if (op == OpNode::opLess)
				b = left->IntVal < right->IntVal;
			else if (op == OpNode::opLessEq)
				b = left->IntVal <= right->IntVal;
			else if (op == OpNode::opMore)
				b = left->IntVal > right->IntVal;
			else if (op == OpNode::opMoreEq)
				b = left->IntVal >= right->IntVal;
			else if (op == OpNode::opEq)
				b = left->IntVal == right->IntVal;
			else if (op == OpNode::opNeq)
				b = left->IntVal != right->IntVal;
			else
				ASSERT(0);
			
			if (FoldConstants)
				return const_bool(b);
		}
	}
	else if (e->Class == ass.CFloat) {
		if (cst) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			if (op == OpNode::opLess)
				b = left->DblVal < right->DblVal;
			else if (op == OpNode::opLessEq)
				b = left->DblVal <= right->DblVal;
			else if (op == OpNode::opMore)
				b = left->DblVal > right->DblVal;
			else if (op == OpNode::opMoreEq)
				b = left->DblVal >= right->DblVal;
			else if (op == OpNode::opEq)
				b = left->DblVal == right->DblVal;
			else if (op == OpNode::opNeq)
				b = left->DblVal != right->DblVal;
			else
				ASSERT(0);
			
			if (FoldConstants)
				return const_bool(b);
		}
	}
	else if (e->Class == ass.CDouble) {
		if (cst) {
			left->PromoteToFloatValue(ass);
			right->PromoteToFloatValue(ass);
			if (op == OpNode::opLess)
				b = left->DblVal < right->DblVal;
			else if (op == OpNode::opLessEq)
				b = left->DblVal <= right->DblVal;
			else if (op == OpNode::opMore)
				b = left->DblVal > right->DblVal;
			else if (op == OpNode::opMoreEq)
				b = left->DblVal >= right->DblVal;
			else if (op == OpNode::opEq)
				b = left->DblVal == right->DblVal;
			else if (op == OpNode::opNeq)
				b = left->DblVal != right->DblVal;
			else
				ASSERT(0);
			
			if (FoldConstants)
				return const_bool(b);
		}
	}
	else if (cst)
		ASSERT_(0, ops[op]);

	OpNode* node = opNodes.Get();

	node->OpA = left;
	node->OpB = right;
	node->Op = op;

	node->IsConst = cst;
	node->IsCT = cst;
	node->SetType(ass.CBool->Tt);
	node->SetValue(b, 0);
	ASSERT(node->Tt.Class);
	return node;
}

Node* IR::opLog(Node* left, Node* right, OpNode::Type op) {
	bool valid = left->Tt.Class == ass.CBool && right->Tt.Class == ass.CBool;
	if (valid == false)
		return NULL;

	bool cst = left->IsCT && right->IsCT;
	bool b;

	if (cst) {
		if (op == OpNode::opLogAnd)
			b = (bool)left->IntVal && (bool)right->IntVal;
		else if (op == OpNode::opLogOr)
			b = (bool)left->IntVal || (bool)right->IntVal;
		else
			ASSERT(0);
		if (FoldConstants)
			return const_bool(b);
	}
	else {
		if (op == OpNode::opLogAnd && ((left->IsCT && left->IntVal == 0) || (right->IsCT && right->IntVal == 0))) {
			if (FoldConstants)
				return const_bool(false);
		}
		else if (op == OpNode::opLogOr && ((left->IsCT && left->IntVal == 1) || (right->IsCT && right->IntVal == 1))) {
			if (FoldConstants)
				return const_bool(true);
		}
	}

	OpNode* node = opNodes.Get();

	node->OpA = left;
	node->OpB = right;
	node->Op = op;

	node->IsConst = cst;
	node->IsCT = cst;
	node->SetType(ass.CBool->Tt);
	node->SetValue(b, 0);
	ASSERT(node->Tt.Class);
	
	return node;
}

Node* IR::shl(Node* left, Node* right, const Point& p) {
	bool n = ass.IsNumeric(left->Tt) && ass.IsNumeric(right->Tt);
	
	if (!n)
		return GetOp(Over, strops[5], left, right, ass, this, *Comp, p);
	
	bool cst = false;

	int t1 = left->Tt.Class->Index - 4;
	int t2 = right->Tt.Class->Index - 4;
	ASSERT(t1 >= 0 && t1 <= 13);
	ASSERT(t2 >= 0 && t2 <= 13);
	int t = tabSft[t1][t2] + 4;

	cst = left->IsCT && right->IsCT;
	ObjectType* e = &ass.Classes[t].Tt;
	if (cst) {
		if (e->Class == ass.CByte || e->Class == ass.CWord || e->Class == ass.CDWord) {
			uint32 dInt = (uint32)left->IntVal << (uint32)right->IntVal;
			if (FoldConstants)	{
				Node* fold = const_i(dInt);
				fold->Tt = ass.CDWord->Tt;
				return fold;
			}
		}
		else if (e->Class == ass.CSmall || e->Class == ass.CShort || e->Class == ass.CInt) {
			int dInt = (int32)left->IntVal << (int32)right->IntVal;
			if (FoldConstants)
				return const_i(dInt);
		}
		else
			ASSERT_(0, "shl");
	}

	OpNode* node = opNodes.Get();

	node->OpA = left->IsRef ? deref(left) : left;
	node->OpB = right->IsRef ? deref(right) : right;
	node->Op = OpNode::opShl;

	node->IsConst = cst;
	node->IsCT = cst;
	node->SetType(e);
	node->DblVal = 0;
	ASSERT(node->Tt.Class);
	
	return node;
}

Node* IR::shr(Node* left, Node* right, const Point& p) {
	bool n = ass.IsNumeric(left->Tt) && ass.IsNumeric(right->Tt);
	
	if (!n)
		return GetOp(Over, strops[6], left, right, ass, this, *Comp, p);

	int t1 = left->Tt.Class->Index - 4;
	int t2 = right->Tt.Class->Index - 4;
	ASSERT(t1 >= 0 && t1 <= 13);
	ASSERT(t2 >= 0 && t2 <= 13);
	int t = tabSft[t1][t2] + 4;

	bool cst = left->IsCT && right->IsCT;
	ObjectType* e = &ass.Classes[t].Tt;
	if (cst) {
		if (e->Class == ass.CByte || e->Class == ass.CWord || e->Class == ass.CDWord) {
			uint32 dInt = (uint32)left->IntVal >> (uint32)right->IntVal;
			
			if (FoldConstants)	{
				Node* fold = const_i(dInt);
				fold->Tt = ass.CDWord->Tt;
				
				return fold;
			}
		}
		else if (e->Class == ass.CSmall || e->Class == ass.CShort || e->Class == ass.CInt) {
			int dInt = (int32)left->IntVal >> (int32)right->IntVal;
			
			if (FoldConstants)
				return const_i(dInt);
		}
		else
			ASSERT_(0, "shr");
	}

	OpNode* node = opNodes.Get();

	node->OpA = left;
	node->OpB = right;
	node->Op = OpNode::opShr;

	node->IsConst = cst;
	node->IsCT = cst;
	node->SetType(e);
	node->DblVal = 0;
	ASSERT(node->Tt.Class);
	
	return node;
}

Node* IR::deref(Node* node) {
	DerefNode* l = derefNodes.Get();
	
	l->Object = node;
	if (node->IsRef) {
		l->SetType(node->Tt);
	}
	else {
		l->Tt = *node->Tt.Next;
		l->C1 = node->Tt.Next->Class;
	}
	
	l->C2 = nullptr;
	l->LValue = node->LValue;
	l->IsConst = node->IsConst;
	
	l->IsAddressable = true;
	
	ASSERT(l->Tt.Class);
	return l;
}


Node* IR::list(Node* node) {
	if (node->NT == NodeType::List)
		return node;
	
	ListNode* l = listNodes.Get();
	
	l->Params.Add(node);
	l->Tt = node->Tt;
	l->C1 = node->C1;
	l->C2 = node->C2;
	l->IsConst = node->IsConst;
	l->IsCT = node->IsCT;
	l->IsLiteral = node->IsLiteral;
	l->HasSe = node->HasSe;
	l->IntVal = node->IntVal;
	l->DblVal = node->DblVal;
	l->IsAddressable = node->IsAddressable;
	ASSERT(l->Tt.Class);
	
	return l;
}

Node* IR::size(ZClass& cls) {
	Node* l = sizeOfNodes.Get();
	
	l->IntVal = cls.Tt.Class->Index;
	l->SetType(ass.CPtrSize->Tt);
	ASSERT(l->Tt.Class);
	
	return l;
}

Node* IR::cast(Node* left, ObjectType* tt, bool sc, bool ptr) {
	if (left->NT == NodeType::Cast) {
		CastNode* c = (CastNode*)left;
		if (c->Tt.Class == tt->Class)
			return left;
	}
	
	if (sc && left->IsCT && ass.IsFloat(left->Tt) && left->Tt == tt)
		return left;

	if (left->IsCT) {
		if (ass.IsNumeric(left->Tt) && ass.IsNumeric(*tt)) {
			if (tt->Class == ass.CFloat) {
				if (ass.IsSignedInt(left->Tt))
					return const_r32((double)left->IntVal);
				else if (ass.IsUnsignedInt(left->Tt))
					return const_r32((double)(uint64)(left->IntVal));
				else if (left->Tt.Class == ass.CDouble) {
					left->SetType(ass.CFloat->Tt);
					return left;
				}
			}
			else if (tt->Class == ass.CDouble) {
				if (ass.IsSignedInt(left->Tt))
					return const_r64((double)left->IntVal);
				else if (ass.IsUnsignedInt(left->Tt))
					return const_r64((double)(uint64)(left->IntVal));
				else if (left->Tt.Class == ass.CFloat) {
					left->SetType(ass.CDouble->Tt);
					return left;
				}
			}
		}
	}

	CastNode* node = castNodes.Get();
	node->Object = left;
	node->Tt = *tt;
	node->C1 = tt->Class;
	node->IsConst = left->IsConst;
	node->IsCT = left->IsCT;
	node->DblVal = left->DblVal;
	node->IntVal = left->IntVal;
	node->IsLiteral = left->IsLiteral;
	node->Ptr = ptr;

	ASSERT(node->Tt.Class);
	return node;
}

Node* IR::minus(Node* node) {
	if (ass.IsInteger(node->Tt) || ass.IsFloat(node->Tt)) {
		if (node->NT == NodeType::UnaryOp && ((UnaryOpNode*)node)->Op == OpNode::opMinus)
			return node->First;
		if (node->IsCT && node->NT != NodeType::List) {
			if (ass.IsSignedInt(node->Tt))
				node->IntVal = -node->IntVal;
			else
				node->DblVal = -node->DblVal;
			return node;
		}
		
		UnaryOpNode* minus = unaryOpNodes.Get();
		minus->OpA = node;
		minus->Op = OpNode::opMinus;

		minus->IsConst = node->IsConst;
		minus->IsCT = node->IsCT;
		minus->SetType(node->Tt);
		ASSERT(minus->Tt.Class);
		
		return minus;
	}
	else
		return GetOp(Over, strops[13], node, ass, this, *Comp, Point(1, 1));
}

Node* IR::op_not(Node* node) {
	if (node->Tt.Class == ass.CBool) {
		if (node->NT == NodeType::UnaryOp && ((UnaryOpNode*)node)->Op == OpNode::opNot)
			return node->First;
		
		if (node->IsCT) {
			node->IntVal = !((bool)node->IntVal);
			return node;
		}
		
		UnaryOpNode* minus = unaryOpNodes.Get();
		minus->OpA = node;
		minus->Op = OpNode::opNot;

		minus->IsConst = node->IsConst;
		minus->IsCT = node->IsCT;
		minus->SetType(node->Tt);
		ASSERT(minus->Tt.Class);
		
		return minus;
	}
	else
		return GetOp(Over, strops[15], node, ass, this, *Comp, Point(1, 1));
}

Node* IR::bitnot(Node* node) {
	if (ass.IsNumeric(node->Tt)) {
		if (node->NT == NodeType::UnaryOp && ((UnaryOpNode*)node)->Op == OpNode::opComp)
			return node->First;
		
		UnaryOpNode* minus = unaryOpNodes.Get();
		minus->OpA = node;
		minus->Op = OpNode::opComp;

		minus->IsConst = node->IsConst;
		minus->IsCT = node->IsCT;
		minus->SetType(node->Tt);
		ASSERT(minus->Tt.Class);
		
		return minus;
	}
	else
		return GetOp(Over, strops[16], node, ass, this, *Comp, Point(1, 1));
}

Node* IR::inc(Node* node, bool prefix) {
	UnaryOpNode* n = unaryOpNodes.Get();
	
	n->OpA = node;
	n->Op = OpNode::opInc;
	n->Prefix = prefix;

	n->SetType(node->Tt);
	n->HasSe = true;
	ASSERT(n->Tt.Class);
	
	return n;
}

Node* IR::dec(Node* node, bool prefix) {
	UnaryOpNode* n = unaryOpNodes.Get();
	
	n->OpA = node;
	n->Op = OpNode::opDec;
	n->Prefix = prefix;

	n->SetType(ass.CVoid->Tt);
	n->HasSe = true;
	ASSERT(n->Tt.Class);
	
	return n;
}

Node* IR::plus(Node* node) {
	if (ass.IsNumeric(node->Tt))
		return node;
	else
		return GetOp(Over, strops[14], node, ass, this, *Comp, Point(1, 1));

	return node;
}

Node* IR::op_bitand(Node* left, Node* right) {
	bool n = ass.IsNumeric(left->Tt) && ass.IsNumeric(right->Tt);
	if (!n || left->Tt.Class == ass.CFloat || left->Tt.Class == ass.CDouble
			|| right->Tt.Class == ass.CFloat || right->Tt.Class == ass.CDouble)
		return NULL;

	int t1 = left->Tt.Class->Index - 4;
	int t2 = right->Tt.Class->Index - 4;
	ASSERT(t1 >= 0 && t1 <= 13);
	ASSERT(t2 >= 0 && t2 <= 13);
	int t = tabAdd[t1][t2] + 4;

	bool cst = left->IsCT && right->IsCT;
	ObjectType* e = &ass.Classes[t].Tt;
	if (cst) {
		if (e->Class == ass.CByte || e->Class == ass.CWord || e->Class == ass.CDWord) {
			uint32 dInt = (uint32)left->IntVal & (uint32)right->IntVal;
			if (FoldConstants)	{
				Node* fold = const_i(dInt);
				fold->Tt = ass.CDWord->Tt;
				return fold;
			}
		}
		else if (e->Class == ass.CSmall || e->Class == ass.CShort || e->Class == ass.CInt) {
			int dInt = (int32)left->IntVal & (int32)right->IntVal;
			if (FoldConstants)
				return const_i(dInt);
		}
		else
			ASSERT_(0, "bitand");
	}

	OpNode* node = opNodes.Get();

	node->OpA = left;
	node->OpB = right;
	node->Op = OpNode::opBitAnd;

	node->IsCT = cst;
	node->SetType(e);
	node->DblVal = 0;
	ASSERT(node->Tt.Class);
	return node;
}

Node* IR::op_bitor(Node* left, Node* right) {
	bool n = ass.IsNumeric(left->Tt) && ass.IsNumeric(right->Tt);
	if (!n || left->Tt.Class == ass.CFloat || left->Tt.Class == ass.CDouble
			|| right->Tt.Class == ass.CFloat || right->Tt.Class == ass.CDouble)
		return NULL;

	int t1 = left->Tt.Class->Index - 4;
	int t2 = right->Tt.Class->Index - 4;
	ASSERT(t1 >= 0 && t1 <= 13);
	ASSERT(t2 >= 0 && t2 <= 13);
	int t = tabAdd[t1][t2] + 4;

	bool cst = left->IsCT && right->IsCT;
	ObjectType* e = &ass.Classes[t].Tt;
	if (cst) {
		if (e->Class == ass.CByte || e->Class == ass.CWord || e->Class == ass.CDWord) {
			uint32 dInt = (uint32)left->IntVal | (uint32)right->IntVal;
			if (FoldConstants)	{
				Node* fold = const_i(dInt);
				fold->Tt = ass.CDWord->Tt;
				return fold;
			}
		}
		else if (e->Class == ass.CSmall || e->Class == ass.CShort || e->Class == ass.CInt) {
			int dInt = (int32)left->IntVal | (int32)right->IntVal;
			if (FoldConstants)
				return const_i(dInt);
		}
		else
			ASSERT_(0, "bitor");
	}

	OpNode* node = opNodes.Get();

	node->OpA = left;
	node->OpB = right;
	node->Op = OpNode::opBitOr;

	node->IsCT = cst;
	node->SetType(e);
	node->DblVal = 0;
	ASSERT(node->Tt.Class);
	return node;
}

Node* IR::op_bitxor(Node* left, Node* right) {
	bool n = ass.IsNumeric(left->Tt) && ass.IsNumeric(right->Tt);
	if (!n || left->Tt.Class == ass.CFloat || left->Tt.Class == ass.CDouble
			|| right->Tt.Class == ass.CFloat || right->Tt.Class == ass.CDouble)
		return NULL;

	int t1 = left->Tt.Class->Index - 4;
	int t2 = right->Tt.Class->Index - 4;
	ASSERT(t1 >= 0 && t1 <= 13);
	ASSERT(t2 >= 0 && t2 <= 13);
	int t = tabAdd[t1][t2] + 4;

	bool cst = left->IsCT && right->IsCT;
	ObjectType* e = &ass.Classes[t].Tt;
	if (cst) {
		if (e->Class == ass.CByte || e->Class == ass.CWord || e->Class == ass.CDWord) {
			uint32 dInt = (uint32)left->IntVal ^ (uint32)right->IntVal;
			if (FoldConstants)	{
				Node* fold = const_i(dInt);
				fold->Tt = ass.CDWord->Tt;
				return fold;
			}
		}
		else if (e->Class == ass.CSmall || e->Class == ass.CShort || e->Class == ass.CInt) {
			int dInt = (int32)left->IntVal ^ (int32)right->IntVal;
			if (FoldConstants)
				return const_i(dInt);
		}
		else
			ASSERT_(0, "bitxor");
	}

	OpNode* node = opNodes.Get();

	node->OpA = left;
	node->OpB = right;
	node->Op = OpNode::opBitXor;

	node->IsCT = cst;
	node->SetType(e);
	node->DblVal = 0;
	ASSERT(node->Tt.Class);
	return node;
}

Node* IR::opTern(Node* cond, Node* left, Node* right) {
	if (cond->IsCT) {
		if (cond->IntVal == 1)
			return left;
		else
			return right;
	}

	OpNode* n = opNodes.Get();
	n->OpA = cond;
	if (left->IsRef)
		left = deref(left);
	if (right->IsRef)
		right = deref(right);
	n->OpB = left;
	n->OpC = right;
	n->Op = OpNode::opTernary;
	n->SetType(left->Tt);
	n->HasSe = cond->HasSe || left->HasSe || right->HasSe;
	
	ASSERT(n->Tt.Class);
	return n;
}

ReturnNode* IR::ret(Node* node) {
	ReturnNode* r = retNodes.Get();
	r->Object = node;
	r->HasSe = true;

	return r;
}

AllocNode* IR::alloc(ZClass* cls, Node* count) {
	AllocNode* a = allocNodes.Get();
	a->Class = cls;
	a->Count = count;
	a->SetType(cls->Pt);
	a->HasSe = true;
	
	ASSERT(a->Tt.Class);
	return a;
}