#ifndef __TFlowForm__h
#define __TFlowForm__h

#include "abstractFormulation.h"

typedef std::vector<Variable> VarArray;
typedef std::vector<VarArray> VarMatrix;

#define EPS 1e-4
#define INFTY std::numeric_limits<double>::max()
/*********************************************************************************************
* This class implements the Online Routing and Spectrum Allocation through a new version of 
* the flow based MIP formulation using the data structures defined in FormulationComponents.h.	
*********************************************************************************************/
class TFlowForm : public AbstractFormulation{

private:
    VarMatrix x;				    /**< Routing variables. x[a][k] = 1 if if demand k is routed through arc a. **/
    VarMatrix y;                    /**< Spectrum assignment variables. y[s][k] = 1 if slot s is the last slot allocated for demand k. **/
	VarMatrix z; 					/**< Non overlapping variables. z[a][b] = 1 if demand a is allocated to a slot that has a smaller position than demand b's allocated slot. **/
    VarArray maxSlicePerLink;	    /**< The array of variables used in the MIP for verifying the max used slice position for each link in the topology network. maxSlicePerLink[i]=p if p is the max used slice position from the link with id i. **/
	Variable maxSliceOverall;		/**< The max used slice position throughout all the network. **/

public:
	/****************************************************************************************/
	/*										Constructors									*/
	/****************************************************************************************/
	/** Constructor. Builds the Formulation.  @param instance The instance to be solved. **/
    TFlowForm(const Instance &instance);
	void printVariables();

	/****************************************************************************************/
	/*										Variables										*/
	/****************************************************************************************/
	/** Puts all variables into a single array of variables and returns it. @note The position of a variable in the array is given by its id. **/
	VarArray getVariables() override; //OK

    /** Defines the decision variables need in the MIP formulation. **/
    void setVariables() override; //OK

    /** Changes the variable values. @param value The vector of values. **/
	void setVariableValues(const std::vector<double> &value) override; //OK

	Variable getMaxSliceOverall() const {return maxSliceOverall;} //OK
	
	/****************************************************************************************/
	/*										Constraints										*/
	/****************************************************************************************/
	
	//-------------------------------- Original constraints -------------------------------- //

    /** Defines the set of constraints. **/
    void setConstraints() override;
	
	void setSourceConstraints();

	void setTargetConstraints();

	void setLengthConstraints();

    void setOSNRConstraints();

	void setDegreeConstraints();

	void setFlowConservationConstraints();

	void setMaxUsedSlicePerLinkConstraints();

	void setMaxUsedSliceOverallConstraints();

	void setSliceConstraint();

	//void setNonOverlappingConstraints();
	
	void setNonOverlappingConstraintsPair();
	void setNonOverlappingConstraintsMinChannel();

	void setPreprocessingConstraints();

	Constraint getSourceConstraint_k(int k);

	Constraint getSourceConstraint_k2(int k);

	Constraint getSourceConstraint_k3(int k);

	Constraint getTargetConstraint_k(int k);

	Constraint getTargetConstraint_k2(int k);

	Constraint getLengthConstraint_k(int k);

    Constraint getOSNRCConstraint(const Demand &demand, int d);

	Constraint getDegreeConstraint_k(int k, const ListGraph::Node &v);

	Constraint getFlowConservationConstraint_v_k(const ListGraph::Node &v, int k);

	Constraint getMaxUsedSlicePerLinkConstraints(int k, int e);

	Constraint getMaxUsedSliceOverallConstraints(int k);

	Constraint getSliceConstraint(int k);

	//Constraint getNonOverlappingConstraint(int a, int b, int arc);

	//Constraint getNonOverlappingConstraint_2(int a, int b);

	Constraint getNonOverlappingConstraintPair(int a, int b, int arc, int s);
	
	Constraint getPreprocessingConstraint(int k);
	

	/****************************************************************************************/
	/*									Objective Functions									*/
	/****************************************************************************************/

    /** Defines the objective function. @param chosenObjective The chosen objective metrics. **/
    void setObjectives() override; //OK

	/** Returns the objective function expression for the given metric. @param chosenObjective The objective metric identifier. **/
    Expression getObjFunctionFromMetric(Input::ObjectiveMetric chosenObjective); //OK
	

	/****************************************************************************************/
	/*										Methods											*/
	/****************************************************************************************/

    /** Recovers the obtained MIP solution and builds a path for each demand on its associated graph from RSA. **/
    void updatePath(const std::vector<double> &vals) override;

	std::vector<Constraint> solveSeparationProblemFract(const std::vector<double> &solution) override; 

    //std::vector<Constraint> solveSeparationProblemInt(const std::vector<double> &solution, const int threadNo) override; 


	/****************************************************************************************/
	/*									Variable Fixing										*/
	/****************************************************************************************/

	/** Returns a set of variables to be fixed to 0 according to the current upper bound. **/
    std::vector<Variable> objective8_fixing(const double upperBound) override;

	/****************************************************************************************/
	/*										Display											*/
	/****************************************************************************************/

	/** Displays the value of each variable in the obtained solution. **/
    void displayVariableValues() override;

	std::string displayDimensions();

	/****************************************************************************************/
	/*										Destructor										*/
	/****************************************************************************************/

	/** Destructor. Clears the variable matrices, cplex model and environment. **/
	~TFlowForm();
};


#endif