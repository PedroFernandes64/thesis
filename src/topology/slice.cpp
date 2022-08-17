#include "slice.h"

/* Constructor, by default d = -1. That is, no demand is assigned to the slice. */
Slice::Slice(int d){
	this->setAssignment(d);
}

/* Returns true if slice is occupied; false otherwise. */
bool Slice::isUsed(){
	if (this->assignedDemand == -1) {
		return false;
	}
	else {
		return true;
	}
}
