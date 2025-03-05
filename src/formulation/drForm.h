#ifndef __DrForm__h
#define __DrForm__h

#include "abstractFormulation.h"

typedef std::vector<Variable> VarArray; 
typedef std::vector<VarArray> VarMatrix;

#define EPS 1e-4
#define INFTY std::numeric_limits<double>::max()
/*********************************************************************************************
* This class implements the Online Routing and Spectrum Allocation through a flow based MIP 
* formulation using the data structures defined in FormulationComponents.h.	
*********************************************************************************************/
class DrFormulation : public AbstractFormulation{
   private:
		VarMatrix y; // Demand-Range variable. y[e][d] equals 1 if demand d is routed through arc e.
		VarMatrix n; //  Demand-Demand variable. n[d][d'] equals 1 if the path assined to the demand d, 0 otherwise
		VarArray lm; // left-most slot occupe par la demand. 
		VarArray rm; //  right-most slot occupe par la demand. 
		VarArray maxSlicePerLink;	// The array of variables used in the MIP for verifying the max used slice position for each link in the topology network. maxSlicePerLink[i]=p if p is the max used slice position from the link with id i. *
	 	Variable maxSliceOverall;	// The max used slice position throughout all the network. *

	public:
	/**************************************************************************************
									Constructors									
	***************************************************************************************
	Constructor. Builds the Formulation.  @param instance The instance to be solved. */
    DrFormulation (const Instance &instance);

	/**************************************************************************************
										Variables										
	**************************************************************************************/

	/* Puts all variables into a single array of variables and returns it. 
	@note The position of a variable in the array is given by its id. */
    VarArray getVariables() override;

	void setWarmValues() override{};

	Variable getMaxSliceOverall() const {return maxSliceOverall;}

    void printVariables();
	
	/* Defines the decision variables need in the MIP formulation. */
    void setVariables()override;

	/** Defines the flow variables. **/
	void setRoutingVariables();

	/** Defines the channel variables. **/
	void setDemandDemandVariables();

	/** Defines the max used slice per edge variables. **/
	void setLeftVariables();

	/** Defines the max used slice overall variable. **/
	void setRightVariables();

	/** Defines the max used slice per edge variables. **/
	void setMaxUsedSliceOverallVariable();

	/** Defines the max used slice overall variable. **/
	void setMaxUsedSlicePerEdgeVariable();

	/** Changes the variable values. @param value The vector of values. **/
    void setVariableValues(const std::vector<double> &value)override;

	/****************************************************************************************/
	/*										Constraints										*/
	/****************************************************************************************/
   
	/** Defines the set of constraints. **/
    void setConstraints()override;

	/** Defines Flow Conservation constraints. If an arc enters a node, then an arc must leave it. **/	
	void setFlowConstraints();	
	void setDegreeConstraints();
	
	void setSourceConstraints();
	void setTargetConstraints();
	
	/** Defines CD constraints. Demands must be routed within a CD limit. **/
	void setCDConstraints();

	/** Defines OSNR constraints. Demands must be routed within a OSNR limit. **/
	void setOSNRConstraints();

	/*obliges the allocation to respect the volume of slots required in each request*/
	void setSlotsVolumeConstraints();

	/** Defines Non-Overlapping constraints. Demands must not overlap eachother's slices. **/
	void setNonOverlappingConstraints();
	void setNonOverlappingConstraints2();
	void setPreprocessingConstraints();
	
/** Returns the flow conservation constraint associated with a demand and a node. @param v The node. @param demand The demand. @param d The demand index. **/
	Constraint getDegreeConstraint_k(int k, const ListGraph::Node &v);
	Constraint getSourceConstraint_k(int k);
	Constraint getTargetConstraint_k(int d);
	Constraint getFlowConstraint_k(int d, const ListGraph::Node &v);
	

	/** Returns the transmission reach constraint associated with a demand. @param d The demand index. **/
	Constraint getCDConstraint(int d);

	/** Returns the OSNR constraint associated with a demand. @param d The demand index. **/
	Constraint getOSNRConstraint(int d);

	Constraint getSlotsVolumeConstraints(int d);
	
	Constraint getNonOverlappingConstraints(int e,int d, int d2);
	Constraint getNonOverlappingConstraints2(int d, int d2);

	void setMaxUsedSlicePerLinkConstraints();
	void setMaxUsedSliceOverallConstraints();
	Constraint getMaxUsedSlicePerLinkConstraints(int k, int e);
	Constraint getMaxUsedSliceOverallConstraints(int d);
	
	Constraint getPreprocessingConstraint(int k);

	/****************************************************************************************/
	/*									Objective Functions									*/
	/****************************************************************************************/
	/** Defines the objective function. @param chosenObjective The chosen objective metrics. **/
    void setObjectives() override;
    /* Returns the objective function expression for the given metric. @param chosenObjective The objective metric identifier. */
    Expression getObjFunctionFromMetric(Input::ObjectiveMetric chosenObjective);
;
	/****************************************************************************************/
	/*										Methods											*/
	/****************************************************************************************/
	void updatePath(const std::vector<double> &vals)override;

	std::vector<Constraint> solveSeparationProblemFract(const std::vector<double> &solution) override;

    std::vector<Constraint> solveSeparationProblemInt(const std::vector<double> &solution, const int threadNo) override; 

	std::vector<Constraint> solveSeparationGnpy(const std::vector<double> &value, const int threadNo);
	
	void writePathFile(const std::string &file);

	std::vector<int> getPathNodeSequence(int d);

	Constraint getPathEliminationConstraint(int d);

   /****************************************************************************************/
	/*									Variable Fixing										*/
	/****************************************************************************************/

	/** Returns a set of variables to be fixed to 0 according to the current upper bound. **/
   std::vector<Variable> objective8_fixing(const double upperBound)override;

   	/****************************************************************************************/
	/*										Display											*/
	/****************************************************************************************/
    void displayVariableValues()override;

	std::string displayDimensions();
	/****************************************************************************************/
	/*										Destructor										*/
	/****************************************************************************************/
    ~DrFormulation();
};


#endif





	