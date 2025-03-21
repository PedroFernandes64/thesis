#include "solverCP.h"

int SolverCP::count = 0;

/****************************************************************************************/
/*										Constructor										*/
/****************************************************************************************/

/* Constructor. The RSA constructor is called and the arc map storing the index of the preprocessed graphs associated is built. */
SolverCP::SolverCP(const Instance &inst) : AbstractSolver(inst, STATUS_UNKNOWN), model(env), cp(model), obj(env){
    std::cout << "--- CP has been initialized ---" << std::endl;
    totalImpleTime = formulation->getTotalImpleTime();
    varImpleTime = formulation->getVarImpleTime();
    constImpleTime = formulation->getConstImpleTime();
    cutImpleTime = formulation->getCutImpleTime();
    objImpleTime = formulation->getObjImpleTime();
    
    setCPParams(inst.getInput());
    
    implementFormulation();
    
    //exportFormulation(inst);
    count++;
}


std::vector<double> SolverCP::getSolution(){
    std::vector<double> solution;
    solution.resize(var.getSize());
    for (unsigned int i = 0; i < solution.size(); i++){
        solution[i] = cp.getValue(var[i]);
    }
    return solution;
}

 
void SolverCP::solve(){    
    IloNum timeStart = cp.getTime();
    std::cout << "Solving..." << std::endl;
    std::vector<ObjectiveFunction> myObjectives = formulation->getObjectiveSet();
    for (unsigned int i = 0; i < myObjectives.size(); i++){
        if (i >= 1){
            model.remove(obj);
            setObjective(myObjectives[i]);
        }
        cp.exportModel("nom_do_cp.cpo");
        
        std::cout << "Chosen objective: " << myObjectives[i].getName() << std::endl;
        std::string	filename = formulation->getInstance().getInput().getParameterFile() + ".sol";
        try {
            cp.solve();
        }
        catch(IloException& ex) {
            std::cerr << "Error: " << ex << std::endl;
        }
        //outfile.close();
        if ((cp.getStatus() == IloAlgorithm::Optimal) || (cp.getStatus() == IloAlgorithm::Feasible)){
            IloNum objValue = cp.getObjValue();
            std::cout << "Objective Function Value: " << objValue << std::endl;            
        }else{
            if(cp.getStatus() == IloAlgorithm::Infeasible){
                std::cout << "Solver proved infeasibility solution..." << std::endl;
            }else{
                // CPLEX STATUS UNKNOWN
                // Stop optimizing.
                std::cout << "Could not find a feasible solution..." << std::endl;
                std::cout << cp.getStatus() << std::endl;
                i = myObjectives.size()+1;
            }
        }
    }
    
    IloNum timeFinish = cp.getTime();
    setDurationTime(timeFinish - timeStart);
    //std::cout << cplex.getStatus() << std::endl;
    if ((cp.getStatus() == IloAlgorithm::Optimal) || (cp.getStatus() == IloAlgorithm::Feasible)){
        setUpperBound(cp.getObjValue());
        setLowerBound(cp.getObjBound());
        setMipGap(cp.getObjGap()*100);
    }else{
        if (cp.getStatus() == IloAlgorithm::Infeasible){
            setUpperBound(-1);
            setLowerBound(-1);
            setMipGap(0);    
        }else{
            // CPLEX STATUS UNKNOWN
            // No feasible solution found but infeasibility not proven, recover relaxation value
            setUpperBound(-1);
            setLowerBound(cp.getObjBound());
            setMipGap(0);
            if ((myObjectives[0].getId() == Input::OBJECTIVE_METRIC_ADS) || (myObjectives[0].getId()  == Input::OBJECTIVE_METRIC_DCB)){
                setUpperBound(cp.getObjBound());
                setLowerBound(-1);
            }
        }
    }
	//setTreeSize(cplex.getNnodes());
    //setAlgorithm(cplex.getAlgorithm());
   
    std::cout << "Optimization done in " << timeFinish - timeStart << " secs." << std::endl;
    if ((cp.getStatus() == IloAlgorithm::Optimal) || (cp.getStatus() == IloAlgorithm::Feasible)){    
        std::cout << "Status: " << cp.getStatus() << std::endl;
        std::cout << "Objective Function Value: " << cp.getObjValue() << std::endl;
        displaySolution();
    }else{
        if (cp.getStatus() == IloAlgorithm::Infeasible){
            std::cout << "Infeasible problem..." << std::endl;
        }else{
            std::cout << "Could not find a feasible solution..." << std::endl;
        }
    }
}

double SolverCP::getObjValue(){
    return cp.getObjValue();
}


AbstractSolver::Status SolverCP::getStatus(){
    setStatus(STATUS_ERROR);

    if (cp.getStatus() == IloAlgorithm::Unknown) {
        setStatus(STATUS_UNKNOWN);
    }
    if (cp.getStatus() == IloAlgorithm::Feasible) {
        setStatus(STATUS_FEASIBLE);
    }
    if (cp.getStatus() == IloAlgorithm::Optimal) {
        setStatus(STATUS_OPTIMAL);
    }
    if (cp.getStatus() == IloAlgorithm::Infeasible) {
        setStatus(STATUS_INFEASIBLE);
    }
    if (cp.getStatus() == IloAlgorithm::Unbounded) {
        setStatus(STATUS_UNBOUNDED);
    }
    if (cp.getStatus() == IloAlgorithm::InfeasibleOrUnbounded) {
        setStatus(STATUS_INFEASIBLE_OR_UNBOUNDED);
    }
    
    if (currentStatus == STATUS_ERROR){
		std::cout << "Got an status error." << std::endl;
		exit(0);
	}
    return currentStatus;
}


void SolverCP::exportFormulation(const Instance &instance){
    std::string file = "model" + std::to_string(count) + ".cpo";
    cp.exportModel(file.c_str());
    std::cout << "CP model has been exported." << std::endl;
}

void SolverCP::setCPParams(const Input &input){
    
    cp.setParameter(IloCP::IntParam::LogVerbosity, IloCP::Normal);
    
    cp.setParameter(IloCP::NumParam::RelativeOptimalityTolerance, 0.0000001);
    
    cp.setParameter(IloCP::IntParam::Workers, 4); //CHANGE TO 4
    
    cp.setParameter(IloCP::NumParam::TimeLimit, input.getIterationTimeLimit());   
    std::cout << "CP parameters have been defined..." << std::endl;
}

void SolverCP::implementFormulation(){
    ClockTime time(ClockTime::getTimeNow());
    ClockTime time2(ClockTime::getTimeNow());
    setVariables(formulation->getVariables());
    //std::cout << "Time: " << time.getTimeInSecFromStart() << std::endl;
    varChargeTime =  time.getTimeInSecFromStart();
    time.setStart(ClockTime::getTimeNow());
    try {
        setConstraints(formulation->getConstraints());
        }
    catch(IloException& ex) {
        std::cerr << "Error: " << ex << std::endl;
    }
    try {
        setNonLinearConstraints(formulation->getNonLinearConstraints());
        }
    catch(IloException& ex) {
        std::cerr << "Error: " << ex << std::endl;
    }
    //std::cout << "Time: " << time.getTimeInSecFromStart() << std::endl;
    constChargeTime = time.getTimeInSecFromStart();
    time.setStart(ClockTime::getTimeNow());
    setObjective(formulation->getObjFunction(0));
    //std::cout << "Time: " << time.getTimeInSecFromStart() << std::endl;
    objChargeTime = time.getTimeInSecFromStart();
    time.setStart(ClockTime::getTimeNow());
    formulation->clearConstraints();
    //std::cout << "Time: " << time.getTimeInSecFromStart() << std::endl;
    totalChargeTime = time2.getTimeInSecFromStart();
}

IloExpr SolverCP::to_IloExpr(const Expression &e){
    IloExpr exp(model.getEnv());
    for (unsigned int i = 0; i < e.getTerms().size(); i++){
        int index = e.getTerm_i(i).getVar().getId();
        double coefficient = e.getTerm_i(i).getCoeff();
        exp += coefficient*var[index];
    }
    return exp;
}

/** Defines the objective function. **/
void SolverCP::setObjective(const ObjectiveFunction &myObjective){
    
    IloExpr exp(model.getEnv());
    exp = to_IloExpr(myObjective.getExpression());
    switch (myObjective.getDirection())
    {
    case ObjectiveFunction::DIRECTION_MIN:
        obj = IloMinimize(model.getEnv(), exp, myObjective.getName().c_str());
        break;
    case ObjectiveFunction::DIRECTION_MAX:
        obj = IloMaximize(model.getEnv(), exp, myObjective.getName().c_str());
        break;
    default:
        std::cout << "ERROR: Invalid direction of objective function." << std::endl;
        exit(0);
        break;
    }
    
    model.add(obj);
    exp.end();
    std::cout << "CP objective has been defined..." << std::endl;
}

/* Defines the constraints needed in the MIP formulation. */
void SolverCP::setConstraints(const std::vector<Constraint> &myConstraints){
    int index; double coefficient; int n;
    for (unsigned int i = 0; i < myConstraints.size(); i++){ 
        IloExpr exp(model.getEnv());
        Expression expression = myConstraints[i].getExpression();
        n = expression.getTerms().size();
        for (unsigned int j = 0; j < n; j++){
            Term term = expression.getTerm_i(j);
            index = term.getVar().getId();
            coefficient = term.getCoeff();
            exp += coefficient*var[index];
        }
        IloRange constraint(model.getEnv(), myConstraints[i].getLb(), exp, myConstraints[i].getUb(), myConstraints[i].getName().c_str());
        model.add(constraint);
        exp.end();
    }
    std::cout << "CP constraints have been defined..." << std::endl;
}

/* Defines the constraints needed in the MIP formulation. */
void SolverCP::setNonLinearConstraints(const std::vector<NonLinearConstraint> &myNonLinearConstraints) {
    int index; 
    double coefficient; 
    int n;
    for (unsigned int i = 0; i < myNonLinearConstraints.size(); i++) { 
        IloExpr exp(model.getEnv());  // Create expression in the environment
        NonLinearExpression expression = myNonLinearConstraints[i].getExpression();
        //myNonLinearConstraints[i].display();
        n = expression.getNonLinearTerms().size();
        
        for (unsigned int j = 0; j < n; j++) {
            NonLinearTerm term = expression.getNonLinearTerm_i(j);
            IloExpr product(model.getEnv());
            
            // Create the product for the non-linear term
            int index0 = term.getVarVec()[0].getId();
            product = var[index0]; 
            
            for (unsigned int k = 1; k < term.getVarVec().size(); k++) {
                index = term.getVarVec()[k].getId();
                product = product*var[index];  // Multiply the variables
            }
            coefficient = term.getCoeff();  // Get the coefficient
            exp = exp + (coefficient * product);  // Add to the expression
        }

        // Define the constraint and add it to the model
        IloRange constraint(model.getEnv(), myNonLinearConstraints[i].getLb(), exp, myNonLinearConstraints[i].getUb(), myNonLinearConstraints[i].getName().c_str());
        model.add(constraint);
        exp.end();  // Clean up IloExpr object
    }

    std::cout << "CP constraints have been defined..." << std::endl;
}

void SolverCP::setVariables(const std::vector<Variable> &myVars){
    var = IloNumVarArray(model.getEnv(), myVars.size());
    for (unsigned int i = 0; i < myVars.size(); i++){ 
        int pos = myVars[i].getId();
        switch (myVars[i].getType())
        {
        case Variable::TYPE_BOOLEAN:
            var[pos] = IloBoolVar(model.getEnv(), myVars[i].getLb(), myVars[i].getUb(), myVars[i].getName().c_str());
            break;
        
        case Variable::TYPE_INTEGER:
            var[pos] = IloIntVar(model.getEnv(), myVars[i].getLb(), myVars[i].getUb(), myVars[i].getName().c_str());
            break;
        
        case Variable::TYPE_REAL:
            var[pos] = IloFloatVar(model.getEnv(), myVars[i].getLb(), myVars[i].getUb(), myVars[i].getName().c_str());
            break;
        
        default:
            std::cout << "ERROR: Variable type has not been recognized." << std::endl;
            exit(0);
            break;
        }

        //std::cout << "Created variable: " << var[pos] << std::endl;
        model.add(var[pos]);
        //getCplex().setPriority(var[pos],myVars[i].getPriority());

    }
    std::cout << "CP variables have been defined..." << std::endl;
}

void SolverCP::setWarmVariables(const std::vector<Variable> &myVars){
    warmVar = IloNumArray(model.getEnv(), myVars.size());
    for (unsigned int i = 0; i < myVars.size(); i++){ 
        int pos = myVars[i].getId();
        warmVar[pos] = IloNum(myVars[i].getWarmstartValue());
        //std::cout << "Created warm value: " << myVars[i].getWarmstartValue() << std::endl;
        //std::cout << "Created warm value: " << warmVar[pos] << std::endl;
        //model.addMIPStart(var[pos]);
        //getCplex().setPriority(var[pos],myVars[i].getPriority());
    }
    std::cout << "CPLEX warmstart values have been defined..." << std::endl;
}

/** Displays the value of each variable in the obtained solution. **/
void SolverCP::displaySolution(){
    for (IloInt i = 0; i < var.getSize(); i++){
        if (cp.getValue(var[i]) >= EPS){
            std::cout << var[i].getName() << " = " << cp.getValue(var[i]) << std::endl;
        }
    }
}


/* Builds file results.csv containing information about the main obtained results. */
void SolverCP::outputLogResults(std::string fileName){
	std::string delimiter = ";";
	std::string filePath = formulation->getInstance().getInput().getOutputPath() + "log_results.csv";
	std::ofstream myfile(filePath.c_str(), std::ios_base::app);
	if (myfile.is_open()){
		myfile << fileName << delimiter;
        myfile << cp.getStatus() << delimiter;
		myfile << getDurationTime() << delimiter;
		myfile << getLowerBound() << delimiter;
		myfile << getUpperBound() << delimiter;
		myfile << getMipGap() << delimiter;
		myfile << getTreeSize() << "\n";
		myfile.close();
	}
	else{
		std::cerr << "Unable to open file " << filePath << "\n";
	}
}


/****************************************************************************************/
/*										Destructor										*/
/****************************************************************************************/
/* Destructor. Free solver memory. */
SolverCP::~SolverCP(){
    obj.end();
    var.end();
    cp.end();
    model.end();
    env.end();
}
