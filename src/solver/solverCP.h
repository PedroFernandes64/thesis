#ifndef __solverCP__h
#define __solverCP__h

#include <ilcp/cp.h>
#include "abstractSolver.h"


/***********************************************************************************************
* This class implements a Formulation of the Online Routing and Spectrum Allocation using CP.
************************************************************************************************/
class SolverCP : public AbstractSolver{

private:
    IloEnv env;						/**< The CP environment. **/
    IloModel model;					/**< The CP model. **/
    IloCP cp;					/**< The CP engine. **/
	static int count;				/**< Counts how many times the solver is called. **/
    IloNumVarArray var;				/**< The array of variables used in the MIP. **/
	IloObjective obj;

	IloNumArray warmVar;

	//IloCP::Algorithm algo;       /**< Algorithm used by cplex to solve the problem. **/

public:
	/****************************************************************************************/
	/*										Constructors									*/
	/****************************************************************************************/

	/** Constructor. Builds the Online RSA mixed-integer program and solves it using CPLEX.  @param instance The instance to be solved. **/
    SolverCP(const Instance &instance);

	/****************************************************************************************/
	/*										Getters											*/
	/****************************************************************************************/
	IloCP getCP(){ return cp; }

	AbstractSolver::Status getStatus() override;

	std::vector<double> getSolution() override;

	//IloCP::Algorithm getAlgorithm() const {return algo;}

	double getObjValue();
	
	/****************************************************************************************/
	/*										Setters											*/
	/****************************************************************************************/

	/** Defines the decision variables needed in the MIP formulation. **/
    void setVariables(const std::vector<Variable> &myVars);

	/** Defines the constraints needed in the MIP formulation. **/
    void setConstraints(const std::vector<Constraint> &myConstraints);

	/** Defines the constraints needed in the MIP formulation. **/
	void setNonLinearConstraints(const std::vector<NonLinearConstraint> &myNonLinearConstraints);

	/** Defines the objective function. **/
    void setObjective(const ObjectiveFunction &myObjective);

	/** Defines the cplex optimization parameters. **/
	void setCPParams(const Input &input);

	//void setAlgorithm(const IloCP::Algorithm &a) { algo = a;}

	void setWarmVariables(const std::vector<Variable> &myVars);
	/****************************************************************************************/
	/*										Methods											*/
	/****************************************************************************************/

	/** Recovers the obtained MIP solution and builds a path for each demand on its associated graph from RSA. **/
    void updatePath();

	void implementFormulation() override;

	//void updateRSA(Instance &instance) override;
	
	void exportFormulation(const Instance &instance);
	
	void solve() override;

	IloExpr to_IloExpr(const Expression &e);
	
	/** Returns the total number of CPLEX default cuts applied during optimization. **/
	//IloInt getNbCutsFromCplex();
	
	/* Builds file results.csv containing information about the main obtained results. */
	void outputLogResults(std::string fileName) override;

	/** Returns a context mask used in callback. No callback is used, return 0. **/
	//CPXLONG context(Input::ObjectiveMetric obj, const Input &i);
	/****************************************************************************************/
	/*										Display											*/
	/****************************************************************************************/

	/** Displays the obtained paths. **/
    void displayOnPath();

	/** Displays the value of each variable in the obtained solution. **/
    void displaySolution();

	/****************************************************************************************/
	/*										Destructor										*/
	/****************************************************************************************/

	/** Destructor. Clears the variable matrices, cplex model and environment. **/
	~SolverCP();

};

#endif