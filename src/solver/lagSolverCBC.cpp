#include "lagSolverCBC.h"
#include "CbcHeuristicGreedy.hpp"
int lagSolverCBC::count = 0;

/** Constructor. Builds the Online RSA mixed-integer program and solves it using CBC.**/
lagSolverCBC::lagSolverCBC(const Instance &inst) : AbstractSolver(inst, STATUS_UNKNOWN), solver(inst){
    std::cout << "--- CBC has been initialized ---" << std::endl;
    implementFormulation();
    setCBCParams(inst.getInput());
    isrelaxed = inst.getInput().isRelaxed();
    count++;
}

void lagSolverCBC::implementFormulation(){
    solver.loadModelFormulation();
    //solver.writeLp("test");
    model = CbcModel(solver);
    formulation = solver.getLagrangianSolver()->getLagrangianFormulation();
    dynamic_cast<OsiLagSolverInterface*>(model.solver())->setCbcModel(&model);
}

void lagSolverCBC::setCBCParams(const Input &input){
    model.setMaximumSeconds(input.getIterationTimeLimit());
    //model.messageHandler()->setLogLevel(4);
    //model.setNumberStrong(0);
    std::cout << "CBC parameters have been defined..." << std::endl;
}

void lagSolverCBC::solve(){
    // Implement time limit and count it here.
    
	ClockTime solveTime(ClockTime::getTimeNow());
    std::cout << "Solving with CBC..." << std::endl;
    std::vector<ObjectiveFunction> myObjectives = solver.getLagrangianSolver()->getLagrangianFormulation()->getObjectiveSet();
    std::cout << "Chosen objective: " << myObjectives[0].getName() << std::endl;

    //CbcRounding heuristic1(model);
    //model.addHeuristic(&heuristic1);
    //model.setCutoff(68);
    model.branchAndBound();

    if (model.bestSolution() != NULL){
        double objValue = model.getObjValue();
        std::cout << "Objective Function Value: " << objValue << std::endl;    
    }
    else{
        // Stop optimizing.
        std::cout << "Could not find a feasible solution..." << std::endl;
    }

    setDurationTime(solveTime.getTimeInSecFromStart());
    setUpperBound(model.getObjValue());
    setLowerBound(model.getBestPossibleObjValue());
    setMipGap(model.getBestPossibleObjValue(), model.getObjValue());
	setTreeSize(model.getNodeCount());
    std::cout << "Optimization done in " << std::fixed  << getDurationTime() << std::setprecision(2) << " secs." << std::endl;
    if (getStatus() == STATUS_OPTIMAL || getStatus() == STATUS_FEASIBLE){    
        //displaySolution();
        std::cout << "Objective Function Value: " << model.getObjValue() << std::endl;
    }
    else{
        if (model.getBestPossibleObjValue() != NULL){
            //displayFractSolution();
            std::cout << "Current obj value: " << model.getBestPossibleObjValue() << std::endl;
        }
        std::cout << "Could not find an integer feasible solution..." << std::endl;
    }
}

std::vector<double> lagSolverCBC::getSolution(){
    std::vector<double> solution;
    solution.resize(model.getNumCols());
    for (unsigned int i = 0; i < solution.size(); i++){
        solution[i] = model.bestSolution()[i];
    }
    return solution;
}


/* Builds file results.csv containing information about the main obtained results. */
void lagSolverCBC::outputLogResults(std::string fileName){
	
}

AbstractSolver::Status lagSolverCBC::getStatus(){
    setStatus(STATUS_UNKNOWN);

    if (model.isAbandoned()) {
        setStatus(STATUS_ERROR);
    }
    if (model.bestSolution() != NULL) {
        setStatus(STATUS_FEASIBLE);
    }
    if (model.isProvenOptimal()){
        setStatus(STATUS_OPTIMAL);
    }
    if (model.isProvenInfeasible()) {
        setStatus(STATUS_INFEASIBLE);
    }
    if (model.isProvenDualInfeasible()) {
        setStatus(STATUS_INFEASIBLE_OR_UNBOUNDED);
    }
    if (model.isContinuousUnbounded()) {
        setStatus(STATUS_UNBOUNDED);
    }
    
    if (currentStatus == STATUS_ERROR){
		std::cout << "ERROR: The CBC model has encountered numerical dificulties." << std::endl;
		exit(0);
	}
    return currentStatus;
}

void lagSolverCBC::displaySolution(){
    for (int i = 0; i < model.getNumCols(); i++){
        if (model.bestSolution()[i] >= EPS){
            std::cout << model.solver()->getColName(i) << " = " << model.bestSolution()[i] << std::endl;
        }
    }
}

void lagSolverCBC::displayFractSolution(){
    for (int i = 0; i < model.getNumCols(); i++){
        if (model.currentSolution()[i] >= EPS){
            std::cout << solver.getColName(i) << " = " << model.currentSolution()[i] << std::endl;
        }
    }
}