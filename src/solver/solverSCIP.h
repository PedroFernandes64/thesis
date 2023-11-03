
#ifndef __solverSCIP__h
#define __solverSCIP__h

#include "abstractSolver.h"
#include <scip/scip.h>
#include <scip/scipdefplugins.h>
#include <scip/def.h>
#include <scip/pub_misc.h>

/***********************************************************************************************
* This class implements a Formulation of the Online Routing and Spectrum Allocation using SCIP.
************************************************************************************************/

class SolverSCIP : public AbstractSolver{

private:
    SCIP * _scip; // SCIP data structure
    //size_t _n; // Number of variables
    std::vector<SCIP_VAR *> _vars; // Vector of variables
    std::vector<SCIP_CONS *> _cons; // Vector of constraints
    static int count; // Counts how many times the solver is called.

public:
    /****************************************************************************************/
	/*										Constructors									*/
	/****************************************************************************************/

	/** Constructor. Builds the Online RSA mixed-integer program and solves it using SCIP.  @param instance The instance to be solved. **/
    SolverSCIP(const Instance &instance);

    /****************************************************************************************/
	/*										Getters											*/
	/****************************************************************************************/

    AbstractSolver::Status getStatus() override; 

	std::vector<double> getSolution() override; 

    std::vector<double> getIntermediateSolution(); 

    double getObjValue();

    std::vector<SCIP_VAR *> getVars();

    /****************************************************************************************/
	/*										Setters											*/
	/****************************************************************************************/

    /** Defines the decision variables needed in the MIP formulation. **/
    void setVariables(const std::vector<Variable> &myVars); 

	/** Defines the constraints needed in the MIP formulation. **/
    void setConstraints(const std::vector<Constraint> &myConstraints); 

	/** Defines the objective function. **/
    void setObjective(const ObjectiveFunction &myObjective); 

	/** Defines the SCIP optimization parameters. **/
	void setSCIPParams(const Input &input); 

    /****************************************************************************************/
	/*										Methods											*/
	/****************************************************************************************/

    /** Recovers the obtained MIP solution and builds a path for each demand on its associated graph from RSA. **/
    //void updatePath();

	void implementFormulation() override;

    void exportFormulation(const Instance &instance); 
	
	void solve() override; 

    /* Builds file results.csv containing information about the main obtained results. */
    void outputLogResults(std::string fileName) override;

    /****************************************************************************************/
	/*										Display											*/
	/****************************************************************************************/

    /** Displays the obtained paths. **/
    //void displayOnPath(); //?

	/** Displays the value of each variable in the obtained solution. **/
    void displaySolution(); 

    /****************************************************************************************/
	/*										Destructor										*/
	/****************************************************************************************/

	/** Destructor. Clears the variable matrices, SCIP model and environment. **/
	~SolverSCIP();

};

#endif