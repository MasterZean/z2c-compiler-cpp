#include "Node.h"
#include "Assembly.h"

namespace Z2 {
	
bool Node::IsZero(Assembly& ass) {
	if (IsCT == false)
		return false;
	
	if (this->Class->MIsInteger || Class == ass.CBool || Class == ass.CChar) {
		if (IntVal == 0)
			return true;
	}
	else if (Class == ass.CFloat || Class == ass.CDouble) {
		if (DblVal == 0)
			return true;
	}
	else
		ASSERT_(0, "is zero");

	return false;
}

}