#ifndef __Slice__h
#define __Slice__h

/***********************************************************************************************
 * This class identifies an specific slice in the frequency spectrum of a Fiber. A Demand 
 * that is routed through a Fiber must be assigned to n continuous slices, where n is the
 * demand's load. This class allows to identify, specify and verify which Demand is assigned (or 
 * not) through a slice.
 **********************************************************************************************/
class Slice
{
private:

	int assignedDemand;	/**< Refers to the id of the assigned demand; If no demand is assigned, it equals -1. **/

public:
	/** Default constructor. @param id The id of the assigned demand. @note If slice is free, id = -1.**/
	Slice(int id = -1);

	/** Returns the id of the assigned demand. **/
	int getAssignment() const { return assignedDemand; }

	/** Specifies the id of the assigned demand. **/
	void setAssignment(int d) { this->assignedDemand = d; }

	/** Verifies if the slice is occupied by some demand. Returns true if slice is already occupied or false, otherwise. **/
	bool isUsed();
	
};

#endif