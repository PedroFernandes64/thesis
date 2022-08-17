#ifndef __EdgeNodeForm__h
#define __EdgeNodeForm__h

#include "abstractFormulation.h"

#include <lemon/preflow.h>

typedef std::vector<Variable> VarArray;
typedef std::vector<VarArray> VarMatrix;
typedef std::vector<VarMatrix> VarMatrix3D;

#define EPS 1e-4
#define INFTY std::numeric_limits<double>::max()
/*********************************************************************************************
* This class implements the Online Routing and Spectrum Allocation through a flow based MIP 
* formulation using the data structures defined in FormulationComponents.h.	
*********************************************************************************************/
class EdgeNodeForm : public AbstractFormulation{

private:
    VarMatrix x;				/**< Demand-Edge variables. x[e][k] equals 1 if demand k is routed through edge e. **/
    VarMatrix z;				/**< Demand-Slot variables. z[s][k] equals 1 if slot s is the last slot allocated for demand k. **/
    VarMatrix3D t;			    /**< Demand-Edge-Slot variables. t[e][s][k] equals 1 if slot s is assigned to demand k on edge e. **/
    VarArray maxSlicePerLink;	/**< The array of variables used in the MIP for verifying the max used slice position for each link in the topology network. maxSlicePerLink[i]=p if p is the max used slice position from the link with id i. **/
	Variable maxSliceOverall;	/**< The max used slice position throughout all the network. **/

public:
	/****************************************************************************************/
	/*										Constructors									*/
	/****************************************************************************************/
	/** Constructor. Builds the Formulation.  @param instance The instance to be solved. **/
    EdgeNodeForm(const Instance &instance);

	/****************************************************************************************/
	/*										Variables										*/
	/****************************************************************************************/
	/** Puts all variables into a single array of variables and returns it. @note The position of a variable in the array is given by its id. **/
	VarArray getVariables() override;

	/** Defines the decision variables need in the MIP formulation. **/
    void setVariables() override;

	/** Changes the variable values. @param value The vector of values. **/
	void setVariableValues(const std::vector<double> &value) override;

	/****************************************************************************************/
	/*										Constraints										*/
	/****************************************************************************************/
	/** Defines the set of constraints. **/
    void setConstraints() override;

	/** Defines Origin constraints. At most one arc leaves each node and exactly one arc leaves the source. @param var The variable matrix. @param mod The CPLEX model. **/
    void setOriginConstraints();

	/** Defines Destination constraints. **/
    void setDestinationConstraints();
	
	/** Defines Degree constraints. At most two edges are adjacent to any node. **/
	void setDegreeConstraints();

	/** Defines Transmission-Reach constraints. Demands must be routed within a length limit. **/
    void setTransmissionReachConstraints();

	/** Defines Channel-Selection constraints. Only one slot can be the last assigned to a demand. **/
	void setChannelSelectionConstraints();

	/** Defines Forbidden-Slot constraints. **/
	void setForbiddenSlotConstraints();
	
	/** Defines Edge-Slot constraints. **/
	void setEdgeSlotConstraints();
	
	/** Defines Demand-Edge-Slot constraints. **/
	void setDemandEdgeSlotConstraints();

	/** Defines Non-Overlapping constraints. Demands must not overlap eachother's slices. **/
    void setNonOverlappingConstraints();

	/** Defines the Link's Max Used Slice Position constraints. 
	 * The max used slice position on each link must be greater than every slice position used in the link.
	 * @note These constraints can be improved by summing them up over demands, but we left it as they are presented in the paper.*/
	void setMaxUsedSlicePerLinkConstraints();
	
	/* Defines the Overall Max Used Slice Position constraints. */
	void setMaxUsedSliceOverallConstraints();

	/** Returns the source constraint associated with a demand and a node. @param k The demand index. **/
    Constraint getOriginConstraint_k(int k);
	
	/** Returns the target constraint associated with a demand. @param k The demand index. **/
    Constraint getDestinationConstraint_k(int k);
	
	/** Returns the degree constraint associated with a demand k and a node v. @param k The demand index. @param v The node. **/
	Constraint getDegreeConstraint_k(int k, const ListGraph::Node &v);
	
    /** Returns the transmission reach constraint associated with a demand. @param k The demand index. **/
    Constraint getTransmissionReachConstraint_k(int k);
	
	/** Returns the channel-selection constraint associated with a demand. @param k The demand index. **/
	Constraint getChannelSelectionConstraint_k(int k);
	
	/** Returns the Forbidden-Slot constraint associated with a demand. @param k The demand index. **/
	Constraint getForbiddenSlotConstraint_k(int k);

	/** Returns the Edge-Slot constraint associated with a demand and an edge. @param k The demand index. @param e The edge index. **/
	Constraint getEdgeSlotConstraint_k_e(int k, int e);
	
	/** Returns the Demand-Edge-Slot constraint associated with a demand, an edge and a slice. @param k The demand index. @param e The edge index. @param s The slice index. **/
	Constraint getDemandEdgeSlotConstraint_k_e_s(int k, int e, int s);

    /** Returns the non-overlapping constraint associated with an arc and a pair of demands. @param e The edge index. @param s The slice index. **/
    Constraint getNonOverlappingConstraint_e_s(int e, int s);
    
	Constraint getMaxUsedSlicePerLinkConstraints(int k, int e, int s);

	Constraint getMaxUsedSliceOverallConstraints(int k);

    std::vector<Constraint> solveSeparationProblemFract(const std::vector<double> &solution) override; 

    std::vector<Constraint> solveSeparationProblemInt(const std::vector<double> &solution, const int threadNo) override; 

	Expression separationGNPY(const std::vector<double> &value, const int threadNo);
	/****************************************************************************************/
	/*									Objective Functions									*/
	/****************************************************************************************/
	/** Defines the objective function. @param chosenObjective The chosen objective metrics. **/
    void setObjectives() override;

	/** Returns the objective function expression for the given metric. @param chosenObjective The objective metric identifier. **/
    Expression getObjFunctionFromMetric(Input::ObjectiveMetric chosenObjective);

	/****************************************************************************************/
	/*										Methods											*/
	/****************************************************************************************/

	/** Recovers the obtained MIP solution and builds a path for each demand on its associated graph from RSA. **/
    void updatePath(const std::vector<double> &vals) override;

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
	/** Displays the value of positive x variables in the obtained solution. **/
    void displayVariableValuesOfX();

	std::string displayDimensions();
	/****************************************************************************************/
	/*										Destructor										*/
	/****************************************************************************************/

	/** Destructor. Clears the variable matrices, cplex model and environment. **/
	~EdgeNodeForm();

};


#endif