#ifndef __FlowForm__h
#define __FlowForm__h

#include "abstractFormulation.h"

typedef std::vector<Variable> VarArray;
typedef std::vector<VarArray> VarMatrix;

#define EPS 1e-4
#define INFTY std::numeric_limits<double>::max()
/*********************************************************************************************
* This class implements the Online Routing and Spectrum Allocation through a flow based MIP 
* formulation using the data structures defined in FormulationComponents.h.	
*********************************************************************************************/
class FlowForm : public AbstractFormulation{

private:
    VarMatrix x;				    /**< The matrix of assignement variables used in the MIP. x[d][a]=1 if the d-th demand is routed through the arc from index a. **/
    VarArray maxSlicePerLink;	    /**< The array of variables used in the MIP for verifying the max used slice position for each link in the topology network. maxSlicePerLink[i]=p if p is the max used slice position from the link with id i. **/
	Variable maxSliceOverall;		/**< The max used slice position throughout all the network. **/
	
	VarArray l;	    				    			
	
	int slicesC;
	int slicesL;
	int slicesTotal;

public:
	/****************************************************************************************/
	/*										Constructors									*/
	/****************************************************************************************/
	/** Constructor. Builds the Formulation.  @param instance The instance to be solved. **/
    FlowForm(const Instance &instance);

	/****************************************************************************************/
	/*										Variables										*/
	/****************************************************************************************/
	/** Puts all variables into a single array of variables and returns it. @note The position of a variable in the array is given by its id. **/
	VarArray getVariables() override;

	/** Puts all variables into a single array of variables (considered in the lagrangian formulation) and returns it. @note The position of a variable in the array is given by its id. **/
	VarArray getLagVariables();

	Variable getMaxSliceOverall() const {return maxSliceOverall;}

	/** Defines the decision variables need in the MIP formulation. **/
    void setVariables() override;

    void setWarmValues() override;

	/** Defines the flow variables. **/
	void setFlowVariables();

	/** Defines the max used slice per edge variables. **/
	void setMaxUsedSlicePerEdgeVariables();

	/** Defines the max used slice overall variable. **/
	void setMaxUsedSliceOverallVariable();

	/** Defines the C band routing variable. **/
	void setCBandRoutingVariable();

	/** Changes the variable values. @param value The vector of values. **/
	void setVariableValues(const std::vector<double> &value) override;


	/** Defines the multiband variables. **/
	void setMultibandVariables();

	/****************************************************************************************/
	/*										Constraints										*/
	/****************************************************************************************/
	
	//-------------------------------- Original constraints -------------------------------- //

	/** Defines the set of constraints. **/
    void setConstraints() override;

	/** Defines the set of constraints considered in the lagrangian formulation. **/
	void setLagConstraints();

	/** Defines Source constraints. At most one arc leaves each node and exactly one arc leaves the source. **/
    void setSourceConstraints();

	/** Defines Flow Conservation constraints. If an arc enters a node, then an arc must leave it. **/
    void setFlowConservationConstraints();
	
	/** Defines Target constraints. Exactly one arc enters the demand's target. **/
    void setTargetConstraints();
	
	/** Defines CD constraints. Demands must be routed within a CD limit. **/
    void setCDConstraints();

	/** Defines OSNR constraints. Demands must be routed within a OSNR limit. **/
    void setOSNRConstraints();

	/** Defines Non-Overlapping constraints. Demands must not overlap eachother's slices. **/
    void setNonOverlappingConstraints();
	
	/** Returns the source constraint associated with a demand and a node. @param demand The demand. @param d The demand index. @param nodeLabel The node label. **/
    Constraint getSourceConstraint_d_n(const Demand & demand, int d, int nodeLabel);
	
	/** Returns the flow conservation constraint associated with a demand and a node. @param v The node. @param demand The demand. @param d The demand index. **/
    Constraint getFlowConservationConstraint_i_d(ListDigraph::Node &v, const Demand & demand, int d);

	/** Returns the target constraint associated with a demand. @param demand The demand. @param d The demand index. **/
    Constraint getTargetConstraint_d(const Demand & demand, int d);
	
    /** Returns the CD constraint associated with a demand. @param demand The demand. @param d The demand index. **/
    Constraint getCDConstraintC(const Demand &demand, int d);

	/** Returns the CD constraint associated with a demand. @param demand The demand. @param d The demand index. **/
    Constraint getCDConstraintL(const Demand &demand, int d);


	/** Returns the OSNR constraint associated with a demand. @param demand The demand. @param d The demand index. **/
    Constraint getOSNRCConstraint(const Demand &demand, int d);

	/** Returns the OSNR constraint associated with a demand. @param demand The demand. @param d The demand index. **/
    Constraint getOSNRLConstraint(const Demand &demand, int d);

	/** Returns the non-overlapping constraint associated with an edge and a slice. @param linkLabel The arc label. @param slice The arc slice. **/
	Constraint getNonOverlappingConstraint(int linkLabel, int slice);

	void setThresholdConstraints();

	Constraint getThresholdConstraint(int e);

	void setMultibandConstraints();	

	Constraint getMultibandConstraint(int k, int e);

	void setForcedMonobandConstraints();
	Constraint getForcedMonobandConstraint_k(const Demand &demand, int d);
	
	//---------------------------------- New constraints ---------------------------------- //

	/** Defines the reinforced max reach constraints. **/
	void setStrongCDConstraints();

	void setStrongOSNRConstraints();

	/** Defines the Link's Max Used Slice Position constraints. The max used slice position on each link must be greater than every slice position used in the link. **/
	void setMaxUsedSlicePerLinkConstraints();

	/** Defines the Overall Max Used Slice Position constraints. The max used slice position overall must be greater than every other slice position used in the network. **/
	void setMaxUsedSliceOverallConstraints();


	void setLowerBoundReinforcementsConstraints();
	
	void setLBTUSConstraints();
	Constraint getLbTUSConstraints();

	void setLinkLoadConstraints();
	Constraint getLinkLoadConstraints(int linkLabel);

	void setMinSliceAtVertexConstraints();
	Constraint getMinSliceAtVertexConstraint_v(ListGraph::Node &v);

	void setMinSliceAtOriginConstraints();
	Constraint getMinSliceAtOriginConstraint();

	void setMinSliceLeavingEdgeConstraints();
	Constraint getMinSliceLeavingEdgeConstraint(ListGraph::Edge &e);

	void setMinSliceLeavingEdgeInternalDemandConstraints();
	std::vector<Constraint> getMinSliceLeavingEdgeInternalDemandConstraint(ListGraph::Edge &e, int intDemand, std::vector<int> demandList);

	/** Returns the strong max reach constraint associated with a demand and a slice. @param demand The demand. @param d The demand index. @param s The slice index.**/
	Constraint getStrongCDConstraint(const Demand &demand, int d, int s);
	
	/** Returns the OSNR constraint associated with a demand. @param demand The demand. @param d The demand index. **/
    Constraint getStrongOSNRConstraint(const Demand &demand, int d, int s);

	/** Returns the Link's Max Used Slice Position constraint associated with a link and a demand. @param linkIndex The link's index. @param d The demand's index. **/
	Constraint getMaxUsedSlicePerLinkConstraints(int linkIndex, int d);
	/** Returns the Link's Max Used Slice Position constraint associated with a link and a demand. @param linkIndex The link's index. @param d The demand's index. **/
	Constraint getMaxUsedSlicePerLinkConstraints2(int linkIndex, int s);

	/** Returns the Overall Max Used Slice Position constraints associated with a link and a demand. @param linkIndex The link's index. @param d The demand's index. **/
	Constraint getMaxUsedSliceOverallConstraints(int d);

	/** Returns the constraint rejecting the path stored for the d-th demand. @param d The d-th demand. **/	
	Constraint getPathEliminationConstraint(int d);

    std::vector<Constraint> solveSeparationProblemFract(const std::vector<double> &solution) override;
	
	std::vector<Constraint> solveSeparationProblemInt(const std::vector<double> &solution, const int threadNo) override;

	std::vector<Constraint> solveSeparationGnpy(const std::vector<double> &value, const int threadNo);

	/** Defines the pool of cuts. **/
    void setCutPool() override;
	
	/** Defines the flow cuts for nodes. **/
	void setFlowNodeCuts();

	/** Returns the cutset flow cuts associated with a cutset and a slice. **/
	Constraint getCutSetFlowConstraint(const std::vector<int> &cutSet, int slice, int cutCardinality);

	/** Defines the flow cuts for edges. **/
	void setFlowEdgeCuts();
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

	void writeServiceFile(const std::string &file);

	void writePathFile(const std::string &file);

	void writePathRequest(std::ofstream &serviceFile, int d);

	/** Returns a vector of node id's corresponding to the sequence of nodes that the d-th demand passes through. **/
	std::vector<int> getPathNodeSequence(int d);
	
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
	~FlowForm();
};


#endif