#ifndef __solverFactory__h
#define __solverFactory__h

// include all concrete solvers
#include "solverCplex.h"
#include "solverSCIP.h"


/*********************************************************************************************
* This class implements a factory for Formulations. It provides a concrete formulation.
*********************************************************************************************/
class SolverFactory{

public:
	/** Factory Method. Returns a new concrete solver based on the chosen MIP_Solver.  @param instance The instance to be solved. **/
    inline AbstractSolver* createSolver(const Instance &instance){
        Input::MIP_Solver chosenSolver = instance.getInput().getChosenMIPSolver();
        switch (chosenSolver){
            case Input::MIP_SOLVER_CPLEX:{
                return new SolverCplex(instance);
                break;
            }
            case Input::MIP_SOLVER_SCIP:{
                return new SolverSCIP(instance);
                break;
            }
            case Input::MIP_SOLVER_GUROBI:{
                std::cout << "Not implemented yet. Should create solverGurobi." << std::endl;
                exit(0);
                break;
            }
            case Input::MIP_SOLVER_CBC:{
                std::cout << "Not implemented yet. Should create solverCBC." << std::endl;
                exit(0);
                break;
            }
            default:{
                std::cout << "ERROR: Invalid MIP_Solver." << std::endl;
                exit(0);
                break;
            }
        }
        return NULL;
    }

};
#endif