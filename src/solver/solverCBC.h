#ifndef __solverCBC__h
#define __solverCBC__h

#include "abstractSolver.h"
#include "CbcModel.hpp"
// Using as solver
#include "OsiCbcSolverInterface.hpp"
#include "OsiClpSolverInterface.hpp"

#include "ClpSimplex.hpp"

/***********************************************************************************************
* This class implements a Formulation of the Online Routing and Spectrum Allocation using CBC.
************************************************************************************************/
class SolverCBC : public AbstractSolver{

private:
	OsiClpSolverInterface solver;	/**< The Clp engine. **/
    CbcModel model;					/**< The CBC model. **/
	static int count;				/**< Counts how many times the solver is called. **/
	bool isrelaxed;

public:
	/****************************************************************************************/
	/*										Constructors									*/
	/****************************************************************************************/

	/** Constructor. Builds the Online RSA mixed-integer program and solves it using CBC.  @param instance The instance to be solved. **/
    SolverCBC(const Instance &instance);

	/****************************************************************************************/
	/*										Getters											*/
	/****************************************************************************************/

	AbstractSolver::Status getStatus() override;

	std::vector<double> getSolution() override;
	/****************************************************************************************/
	/*										Setters											*/
	/****************************************************************************************/

	/** Defines the decision variables needed in the MIP formulation. **/
    void setVariables(const std::vector<Variable> &myVars);

	/** Defines the constraints needed in the MIP formulation. **/
    void setConstraints(const std::vector<Constraint> &myConstraints);

	/** Defines the objective function. **/
    void setObjective(const ObjectiveFunction &myObjective);

	/** Defines the cbc optimization parameters. **/
	void setCBCParams(const Input &input);
	/****************************************************************************************/
	/*										Methods											*/
	/****************************************************************************************/

	/** Recovers the obtained MIP solution and builds a path for each demand on its associated graph from RSA. **/
    void updatePath();

	void implementFormulation() override;

	//void updateRSA(Instance &instance) override;
	
	void exportFormulation(const Instance &instance);
	
	void solve() override;

	//IloExpr to_IloExpr(const Expression &e);
	
	/** Returns the total number of CPLEX default cuts applied during optimization. **/
	//IloInt getNbCutsFromCplex();
	
	/* Builds file results.csv containing information about the main obtained results. */
	void outputLogResults(std::string fileName) override;
	/****************************************************************************************/
	/*										Display											*/
	/****************************************************************************************/

	/** Displays the obtained paths. **/
    void displayOnPath();

	/** Displays the value of each variable in the obtained solution. **/
    void displaySolution();
	/** Displays the value of each variable in the obtained fractionary solution. **/
    void displayFractSolution();

	/****************************************************************************************/
	/*										Destructor										*/
	/****************************************************************************************/

	/** Destructor. Clears the variable matrices, cplex model and environment. **/
	//~SolverCBC();

};

#endif