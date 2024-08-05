#include "solverSCIP.h"
#include "objscip/objscip.h"

int SolverSCIP::count = 0;

SolverSCIP::SolverSCIP(const Instance &inst) : AbstractSolver(inst, STATUS_UNKNOWN), _scip(0), _vars(), _cons(){
    std::cout << "--- SCIP has been initialized ---" << std::endl;

    // Creation of scip pointer
    SCIPcreate(& _scip);

    // Default plugins inclusion 
    SCIPincludeDefaultPlugins(_scip);

    // Deactivation of the standard output
    SCIPmessagehdlrSetQuiet(SCIPgetMessagehdlr(_scip), TRUE);

    // Output level definition
    SCIPsetIntParam(_scip, "display/verblevel", 3);

    // Creation of an empty problem
    SCIPcreateProb(_scip, "RSA", NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    setSCIPParams(inst.getInput());
    implementFormulation();
    //exportFormulation(inst);

    //SCIPincludeObjEventhdlr(_scip, new EventhdlrNonOverlapping(_scip, formulation, this), TRUE);

    count++;
}

std::vector<SCIP_VAR *> SolverSCIP::getVars(){
    return this->_vars;
}

AbstractSolver::Status SolverSCIP::getStatus(){
    setStatus(STATUS_ERROR);

    if (SCIPgetStatus(_scip) == SCIP_STATUS_UNKNOWN) {
        setStatus(STATUS_UNKNOWN);
    }
    if (SCIPgetBestSol(_scip) != NULL) {
        setStatus(STATUS_FEASIBLE);
    }
    if (SCIPgetStatus(_scip) == SCIP_STATUS_OPTIMAL) {
        setStatus(STATUS_OPTIMAL);
    }
    if (SCIPgetStatus(_scip) == SCIP_STATUS_INFEASIBLE) {
        setStatus(STATUS_INFEASIBLE);
    }
    if (SCIPgetStatus(_scip) == SCIP_STATUS_UNBOUNDED) {
        setStatus(STATUS_UNBOUNDED);
    }
    if (SCIPgetStatus(_scip) == SCIP_STATUS_INFORUNBD) {
        setStatus(STATUS_INFEASIBLE_OR_UNBOUNDED);
    }
    
    if (currentStatus == STATUS_ERROR){
		std::cout << "Got an status error." << std::endl;
		exit(0);
	}
    return currentStatus;
}

std::vector<double> SolverSCIP::getSolution(){
    std::vector<double> solution;
    solution.resize(_vars.size());
    SCIP_SOL * sol = SCIPgetBestSol(_scip);
    for (unsigned int i = 0; i < solution.size(); i++){
        solution[i] = SCIPgetSolVal(_scip, sol, _vars[i]);
    }
    return solution;
}

std::vector<double> SolverSCIP::getIntermediateSolution(){
    std::vector<double> intermSolution;
    intermSolution.resize(_vars.size());
    SCIP_SOL * sol;
    SCIPcreateCurrentSol(_scip, &sol, NULL);
    for (unsigned int i = 0; i < intermSolution.size(); i++){
        intermSolution[i] = SCIPgetSolVal(_scip, sol, _vars[i]);
    }
    return intermSolution;
}

/** Displays the value of each variable in the obtained solution. **/
void SolverSCIP::displaySolution(){
    SCIP_SOL * sol = SCIPgetBestSol(_scip);
    for (int i = 0; i < _vars.size(); i++){
        if (SCIPgetSolVal(_scip, sol, _vars[i]) >= EPS){
            std::cout << SCIPvarGetName(_vars[i]) << " = " << SCIPgetSolVal(_scip, sol, _vars[i]) << std::endl;
        }
    }
}

void SolverSCIP::setVariables(const std::vector<Variable> &myVars){
    //_vars.resize(myVars.size());
    ObjectiveFunction obj = formulation->getObjFunction(0); 
    for (unsigned int i = 0; i < myVars.size(); i++){ 
        double coefficient = 0;
        int pos = myVars[i].getId();
        for (unsigned int j = 0; j < obj.getExpression().getTerms().size(); j++){
            int pos_f = obj.getExpression().getTerm_i(j).getVar().getId();
            if(pos == pos_f){
                coefficient = obj.getExpression().getTerm_i(j).getCoeff();
            }
        } 
        SCIP_VAR* var;
        switch (myVars[i].getType())
        {
        case Variable::TYPE_BOOLEAN:
            SCIPcreateVar(_scip, & var, myVars[i].getName().c_str(),
                    myVars[i].getLb(), myVars[i].getUb(), coefficient, 
                    SCIP_VARTYPE_BINARY, TRUE, FALSE,
                    NULL, NULL, NULL, NULL, NULL);
            break;

        case Variable::TYPE_INTEGER:
            SCIPcreateVar(_scip, & var, myVars[i].getName().c_str(),
                    myVars[i].getLb(), myVars[i].getUb(), coefficient, 
                    SCIP_VARTYPE_INTEGER, TRUE, FALSE,
                    NULL, NULL, NULL, NULL, NULL);
            break;

        case Variable::TYPE_REAL:
            SCIPcreateVar(_scip, & var, myVars[i].getName().c_str(),
                    myVars[i].getLb(), myVars[i].getUb(), coefficient, 
                    SCIP_VARTYPE_CONTINUOUS, TRUE, FALSE,
                    NULL, NULL, NULL, NULL, NULL);
            break;

        default:
            std::cout << "ERROR: Variable type has not been recognized." << std::endl;
            exit(0);
            break;
        }
        SCIPaddVar(_scip, var);
        //_vars[pos] = var; 
        _vars.push_back(var);
        // std::cout << "Created variable: " << var[d][arc].getName() << std::endl;
    }
    std::cout << "SCIP variables have been defined..." << std::endl;
}

/* Defines the constraints needed in the MIP formulation. */
void SolverSCIP::setConstraints(const std::vector<Constraint> &myConstraints){
    int index; double coefficient; int n; 
    VarArray v = formulation->getVariables();
    for (unsigned int i = 0; i < myConstraints.size(); i++){ 
        SCIP_CONS * cons;
        Expression expression = myConstraints[i].getExpression();
        n = expression.getTerms().size();
        SCIPcreateConsLinear(_scip, & cons, myConstraints[i].getName().c_str(), 
                            0, NULL, NULL, myConstraints[i].getLb(), myConstraints[i].getUb(), 
                           TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE);
        for (unsigned int j = 0; j < n; j++){
            Term term = expression.getTerm_i(j);
            index = term.getVar().getId();
            coefficient = term.getCoeff();
            for(unsigned int k = 0; k < v.size(); k++){
                if(v[k].getId() == index){
                    SCIPaddCoefLinear(_scip, cons, _vars[k], coefficient); 
                }  
            }
        }
        SCIPaddCons(_scip, cons);
        _cons.push_back(cons);
    }
    std::cout << "SCIP constraints have been defined..." << std::endl;
}

/** Defines the objective function's direction (The objective function's coefficients are alredy defined when the variables are created). **/
void SolverSCIP::setObjective(const ObjectiveFunction &myObjective){
    switch (myObjective.getDirection())
    {
    case ObjectiveFunction::DIRECTION_MIN:
        SCIPsetObjsense(_scip, SCIP_OBJSENSE_MINIMIZE);
        break;
    case ObjectiveFunction::DIRECTION_MAX:
        SCIPsetObjsense(_scip, SCIP_OBJSENSE_MAXIMIZE);
        break;
    default:
        std::cout << "ERROR: Invalid direction of objective function." << std::endl;
        exit(0);
        break;
    }
    std::cout << "SCIP objective has been defined..." << std::endl;
}

void SolverSCIP::setSCIPParams(const Input &input) {
    SCIPsetIntParam(_scip, "display/verblevel", 3);
    SCIPsetRealParam(_scip, "limits/time", input.getIterationTimeLimit());
    // SCIPsetIntParam(_scip, "parallel/threads", 1);

    if (formulation->getInstance().getInput().isRelaxed()) {
        Input::RootMethod rootMethod = formulation->getInstance().getInput().getChosenRootMethod();
        switch (rootMethod) 
        {
        case Input::ROOT_METHOD_AUTO:
            //SCIPsetIntParam(_scip, "lp/initalg", SCIP_LPALGO_AUTO);
            std::cout << "ERROR: Root Method not supported." << std::endl;
            break;
        case Input::ROOT_METHOD_PRIMAL:
            SCIPsetIntParam(_scip, "lp/initalg", SCIP_LPALGO_PRIMALSIMPLEX);
            break;
        case Input::ROOT_METHOD_DUAL:
            SCIPsetIntParam(_scip, "lp/initalg", SCIP_LPALGO_DUALSIMPLEX);
            break;
        case Input::ROOT_METHOD_NETWORK:
            //SCIPsetIntParam(_scip, "lp/initalg", SCIP_LPALGO_NET);
            std::cout << "ERROR: Root Method not supported." << std::endl;
            break;
        case Input::ROOT_METHOD_BARRIER:
            SCIPsetIntParam(_scip, "lp/initalg", SCIP_LPALGO_BARRIER);
            break;
        default:
            std::cout << "ERROR: Root Method has not been recognized." << std::endl;
            exit(0);
            break;
        }
    }
    std::cout << "SCIP parameters have been defined..." << std::endl;
}

void SolverSCIP::implementFormulation(){
    ClockTime time(ClockTime::getTimeNow());
    ClockTime time2(ClockTime::getTimeNow());
    // add variables
    setVariables(formulation->getVariables());
    //std::cout << "Time: " << time.getTimeInSecFromStart() << std::endl;
    varChargeTime =  time.getTimeInSecFromStart();
    time.setStart(ClockTime::getTimeNow());
    // add constraints
    setConstraints(formulation->getConstraints());
    //std::cout << "Time: " << time.getTimeInSecFromStart() << std::endl;
    constChargeTime = time.getTimeInSecFromStart();
    time.setStart(ClockTime::getTimeNow());
    // add the first objective function
    setObjective(formulation->getObjFunction(0));
    //std::cout << "Time: " << time.getTimeInSecFromStart() << std::endl;
    objChargeTime = time.getTimeInSecFromStart();
    time.setStart(ClockTime::getTimeNow());
    // free formulation memory
    formulation->clearConstraints();
    //std::cout << "Time: " << time.getTimeInSecFromStart() << std::endl;
    totalChargeTime = time2.getTimeInSecFromStart();
}

void SolverSCIP::solve(){
    std::cout << "Solving with SCIP..." << std::endl;
    std::vector<ObjectiveFunction> myObjectives = formulation->getObjectiveSet();
    VarArray v = formulation->getVariables();
    for (unsigned int i = 0; i < myObjectives.size(); i++){
        if (i >= 1){
            setObjective(myObjectives[i]);
            for (unsigned int j = 0; j < myObjectives[i].getExpression().getTerms().size(); j++){
                int index = myObjectives[i].getExpression().getTerm_i(j).getVar().getId();
                double coefficient = myObjectives[i].getExpression().getTerm_i(j).getCoeff();
                for(unsigned int k = 0; k < v.size(); k++){
                    if(v[k].getId() == index){
                        SCIPchgVarObj(_scip,_vars[k],coefficient); 
                    }  
                }
            }
        }
        std::cout << "Chosen objective: " << myObjectives[i].getName() << std::endl;
        SCIPsolve(_scip);

        if ((SCIPgetStatus(_scip) == SCIP_STATUS_OPTIMAL) || (SCIPgetBestSol(_scip) != NULL)){
            SCIP_SOL * sol = SCIPgetBestSol(_scip);
            std::cout << "Status: " << SCIPgetStatus(_scip) << std::endl;
            std::cout << "Objective Function Value: " << SCIPgetSolOrigObj(_scip, sol) << std::endl;
            displaySolution();
        }
        else{
            // Stop optimizing.
            std::cout << "Could not find a feasible solution..." << std::endl;
            i = myObjectives.size()+1;
        }
    }
    setDurationTime(SCIPgetSolvingTime(_scip));
    if ((SCIPgetStatus(_scip) == SCIP_STATUS_OPTIMAL) || (SCIPgetBestSol(_scip) != NULL)){
        setUpperBound(SCIPgetPrimalbound(_scip));
        setLowerBound(SCIPgetDualbound(_scip));
        setMipGap(SCIPgetGap(_scip));	
    }
    setTreeSize(SCIPgetNNodes(_scip));

    std::cout << "Optimization done in " << SCIPgetSolvingTime(_scip) << " secs." << std::endl;
}

double SolverSCIP::getObjValue(){
    SCIP_SOL * sol = SCIPgetBestSol(_scip);
    return SCIPgetSolOrigObj(_scip, sol);
}

void SolverSCIP::exportFormulation(const Instance &instance){
    std::string file = "model" + std::to_string(count) + ".lp";
    SCIPwriteOrigProblem(_scip, file.c_str(), "lp", FALSE);
    std::cout << "LP model has been exported." << std::endl;
}

/* Builds file results.csv containing information about the main obtained results. */
void SolverSCIP::outputLogResults(std::string fileName){
	std::string delimiter = ";";
	std::string filePath = formulation->getInstance().getInput().getOutputPath() + "log_results.csv";
	std::ofstream myfile(filePath.c_str(), std::ios_base::app);
	if (myfile.is_open()){
		myfile << fileName << delimiter;
        myfile << SCIPgetStatus(_scip) << delimiter;
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

SolverSCIP::~SolverSCIP(){
    for(size_t i = 0; i < _vars.size(); ++i)
    {
        SCIPreleaseVar(_scip, & _vars[i]);
    }
    _vars.clear();

    for(unsigned int i = 0; i < _cons.size(); ++i)
    {
        SCIPreleaseCons(_scip, &_cons[i]);
    }
    _cons.clear();

    SCIPfree(&_scip);

    std::cout << "The end." << std::endl;
}
