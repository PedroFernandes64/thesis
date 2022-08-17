#include "solverCBC.h"

#include "../tools/clockTime.h"

int SolverCBC::count = 0;

/****************************************************************************************/
/*										Constructor										*/
/****************************************************************************************/

/** Constructor. Builds the Online RSA mixed-integer program and solves it using CBC.**/
SolverCBC::SolverCBC(const Instance &inst) : AbstractSolver(inst, STATUS_UNKNOWN), model(solver){
    std::cout << "--- CBC has been initialized ---" << std::endl;
    implementFormulation();
    setCBCParams(inst.getInput());
    isrelaxed = inst.getInput().isRelaxed();
    count++;
}

void SolverCBC::setCBCParams(const Input &input){
    model.setMaximumSeconds(input.getIterationTimeLimit());
    //model.setDblParam(CbcModel::CbcMaximumSeconds,input.getIterationTimeLimit());
   
    Input::RootMethod rootMethod = formulation->getInstance().getInput().getChosenRootMethod();
    if (rootMethod == Input::ROOT_METHOD_AUTO){
        ClpSolve clpSolve;
        clpSolve.setSolveType(ClpSolve::automatic);
        clpSolve.setPresolveType(ClpSolve::presolveOn);
        dynamic_cast<OsiClpSolverInterface*>(model.solver())->setSolveOptions(clpSolve);
        solver.setSolveOptions(clpSolve);
    }
    else if (rootMethod == Input::ROOT_METHOD_PRIMAL){
        ClpSolve clpSolve;
        clpSolve.setSolveType(ClpSolve::usePrimal);
        clpSolve.setPresolveType(ClpSolve::presolveOn);
        dynamic_cast<OsiClpSolverInterface*>(model.solver())->setSolveOptions(clpSolve);
        solver.setSolveOptions(clpSolve);
    }
    else if (rootMethod == Input::ROOT_METHOD_DUAL){
        ClpSolve clpSolve;
        clpSolve.setSolveType(ClpSolve::useDual);
        clpSolve.setPresolveType(ClpSolve::presolveOn);
        dynamic_cast<OsiClpSolverInterface*>(model.solver())->setSolveOptions(clpSolve);
        solver.setSolveOptions(clpSolve);
    }
    else if (rootMethod == Input::ROOT_METHOD_NETWORK){
    }
    else if (rootMethod == Input::ROOT_METHOD_BARRIER){
        ClpSolve clpSolve;
        clpSolve.setSolveType(ClpSolve::useBarrier);
        clpSolve.setPresolveType(ClpSolve::presolveOn);
        dynamic_cast<OsiClpSolverInterface*>(model.solver())->setSolveOptions(clpSolve); //b&b
        solver.setSolveOptions(clpSolve); // relaxed
    }
    dynamic_cast<OsiClpSolverInterface*>(model.solver())->getModelPtr()->setMaximumSeconds(input.getIterationTimeLimit()); 
    solver.getModelPtr()->setMaximumSeconds(input.getIterationTimeLimit()); 
    model.setNumberStrong(0);
    std::cout << "CBC parameters have been defined..." << std::endl;
}

void SolverCBC::implementFormulation(){
    ClockTime time(ClockTime::getTimeNow());
    ClockTime time2(ClockTime::getTimeNow());
    // add variables.
    setVariables(formulation->getVariables());
    //std::cout << "Time: " << time.getTimeInSecFromStart() << std::endl;
    varChargeTime =  time.getTimeInSecFromStart();
    time.setStart(ClockTime::getTimeNow());
    // add constraints.
    setConstraints(formulation->getConstraints());
    //std::cout << "Time: " << time.getTimeInSecFromStart() << std::endl;
    constChargeTime = time.getTimeInSecFromStart();
    time.setStart(ClockTime::getTimeNow());
    // add the first objective function.
    setObjective(formulation->getObjFunction(0));
    //std::cout << "Time: " << time.getTimeInSecFromStart() << std::endl;
    objChargeTime = time.getTimeInSecFromStart();
    time.setStart(ClockTime::getTimeNow());
    //solver.writeLp("test");
    model = CbcModel(solver);
    // free formulation memory.
    formulation->clearConstraints();
    //std::cout << "Time: " << time.getTimeInSecFromStart() << std::endl;
    totalChargeTime = time2.getTimeInSecFromStart();
}

void SolverCBC::setVariables(const std::vector<Variable> &myVars){
    int n = myVars.size();
    for (unsigned int i = 0; i < n; i++){ 
        solver.addCol(0,NULL,NULL, myVars[i].getLb(), myVars[i].getUb(), 0, myVars[i].getName());
        // std::cout << "Created variable: " << var[d][arc].getName() << std::endl;
        int pos = myVars[i].getId();
        switch (myVars[i].getType()){
            case Variable::TYPE_BOOLEAN:
                solver.setInteger(pos);
                break;
            case Variable::TYPE_INTEGER:
                solver.setInteger(pos);
                break;
            case Variable::TYPE_REAL:
                break;
            default:
                std::cout << "ERROR: Variable type has not been recognized." << std::endl;
                exit(0);
                break;
        }
    }
    std::cout << "CBC variables have been defined..." << std::endl;
}

/* Defines the constraints needed in the MIP formulation. */
void SolverCBC::setConstraints(const std::vector<Constraint> &myConstraints){
    for (unsigned int i = 0; i < myConstraints.size(); i++){ 
        CoinPackedVector constraint;
        Expression expression = myConstraints[i].getExpression();
        int n = expression.getTerms().size();
        int index; double coefficient;
        for (unsigned int j = 0; j < n; j++){
            Term term = expression.getTerm_i(j);
            index = term.getVar().getId();
            coefficient = term.getCoeff();
            constraint.insert(index, coefficient);
        }
        solver.addRow(constraint, myConstraints[i].getLb(), myConstraints[i].getUb(), myConstraints[i].getName());
    }
    std::cout << "CBC constraints have been defined..." << std::endl;
}

/** Defines the objective function. **/
void SolverCBC::setObjective(const ObjectiveFunction &myObjective){
    // Define objective sense: 1 for minimize; -1 for maximize.
    int objSense = 1;
    solver.setObjSense(1);

    // Fill objective coefficients.
    Expression expression =  myObjective.getExpression();
    int n = expression.getTerms().size();
    int index; double coefficient;
    for (unsigned int i = 0; i < n; i++){
        Term term= expression.getTerm_i(i);
        index = term.getVar().getId();
        coefficient = term.getCoeff();
        solver.setObjCoeff(index, coefficient);
    }
    std::cout << "CBC objective has been defined..." << std::endl;
}

void SolverCBC::solve(){
    // Implement time limit and count it here.
	ClockTime solveTime(ClockTime::getTimeNow());
    std::cout << "Solving with CBC..." << std::endl;
    std::vector<ObjectiveFunction> myObjectives = formulation->getObjectiveSet();
    for (unsigned int i = 0; i < myObjectives.size(); i++){
        if (i >= 1){
            for (unsigned int j = 0; j < formulation->getVariables().size(); j++){
                int index = formulation->getVariables()[j].getId();
                solver.setObjCoeff(index, 0);
            }
            setObjective(myObjectives[i]);
        }
        std::cout << "Chosen objective: " << myObjectives[i].getName() << std::endl;
        if(isrelaxed){
            solver.initialSolve(); // Using this method so the time limit is respected.
        }
        else{
            model.branchAndBound();
            if (model.bestSolution() != NULL){
                double objValue = model.getObjValue();
                std::cout << "Objective Function Value: " << objValue << std::endl;
                if (i < myObjectives.size() - 1){
                    CoinPackedVector objectiveExpression;
                    for (unsigned int j = 0; j < myObjectives[i].getExpression().getTerms().size(); j++){
                        int index = myObjectives[i].getExpression().getTerm_i(j).getVar().getId();
                        double coefficient = myObjectives[i].getExpression().getTerm_i(j).getCoeff();
                        objectiveExpression.insert(index, coefficient);
                    }
                    model.solver()->addRow(objectiveExpression, objValue, objValue, myObjectives[i].getName());
                }
            }
            else{
                // Stop optimizing.
                std::cout << "Could not find a feasible solution..." << std::endl;
                i = myObjectives.size()+1;
            }
        }
    }
    setDurationTime(solveTime.getTimeInSecFromStart());
    if(isrelaxed){
        setUpperBound(solver.getObjValue());
        setMipGap(0);
        setTreeSize(0);
    }else{
        setUpperBound(model.getObjValue());
        setLowerBound(model.getCurrentObjValue());
        setMipGap(model.getCurrentObjValue(), model.getObjValue());
        setTreeSize(model.getNodeCount());
    }
    std::cout << "Optimization done in " << std::fixed  << getDurationTime() << std::setprecision(2) << " secs." << std::endl;
    if ((getStatus() == STATUS_OPTIMAL || getStatus() == STATUS_FEASIBLE) && !isrelaxed){    
        //std::cout << "Status: " << cplex.getStatus() << std::endl;
        displaySolution();
        std::cout << "Objective Function Value: " << model.getObjValue() << std::endl;
    }
    else{
        if (model.currentSolution() != NULL){
            displayFractSolution();
            std::cout << "Current obj value: " << model.getCurrentObjValue() << std::endl;
        }
        std::cout << "Could not find an integer feasible solution..." << std::endl;
    }

}

std::vector<double> SolverCBC::getSolution(){
    std::vector<double> solution;
    solution.resize(model.getNumCols());
    for (unsigned int i = 0; i < solution.size(); i++){
        solution[i] = model.bestSolution()[i];
    }
    return solution;
}

/* Builds file results.csv containing information about the main obtained results. */
void SolverCBC::outputLogResults(std::string fileName){ }

AbstractSolver::Status SolverCBC::getStatus(){
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

void SolverCBC::displaySolution(){
    for (int i = 0; i < model.getNumCols(); i++){
        if (model.bestSolution()[i] >= EPS){
            std::cout << model.solver()->getColName(i) << " = " << model.bestSolution()[i] << std::endl;
        }
    }
}

void SolverCBC::displayFractSolution(){
    for (int i = 0; i < model.getNumCols(); i++){
        if (model.currentSolution()[i] >= EPS){
            std::cout << solver.getColName(i) << " = " << model.currentSolution()[i] << std::endl;
        }
    }
}