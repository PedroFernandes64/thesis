#include "solverCplex.h"

ILOMIPINFOCALLBACK3(infocallback, std::ofstream&, file, IloCplex, cplex, IloNum, timeBegin){
    //if(hasIncumbent() == IloTrue){
    double elapsedTime = (cplex.getCplexTime() - timeBegin);
    file << getIncumbentObjValue() << ";" << getBestObjValue() << ";" << getNnodes() << ";" << getNremainingNodes() << ";" << elapsedTime <<std::endl;
    //}
}


int SolverCplex::count = 0;

/****************************************************************************************/
/*										Constructor										*/
/****************************************************************************************/

/* Constructor. The RSA constructor is called and the arc map storing the index of the preprocessed graphs associated is built. */
SolverCplex::SolverCplex(const Instance &inst) : AbstractSolver(inst, STATUS_UNKNOWN), model(env), cplex(model), obj(env){
    std::cout << "--- CPLEX has been initialized ---" << std::endl;
    totalImpleTime = formulation->getTotalImpleTime();
    varImpleTime = formulation->getVarImpleTime();
    constImpleTime = formulation->getConstImpleTime();
    cutImpleTime = formulation->getCutImpleTime();
    objImpleTime = formulation->getObjImpleTime();
    setCplexParams(inst.getInput());
    implementFormulation();
    //exportFormulation(inst);
    count++;
}


std::vector<double> SolverCplex::getSolution(){
    std::vector<double> solution;
    solution.resize(var.getSize());
    for (unsigned int i = 0; i < solution.size(); i++){
        solution[i] = cplex.getValue(var[i]);
    }
    return solution;
}

CPXLONG SolverCplex::context(Input::ObjectiveMetric obj, const Input &i){ 
    CPXLONG contextMask = 0;

    if (obj == Input::OBJECTIVE_METRIC_NLUS){
        contextMask |= IloCplex::Callback::Context::Id::Relaxation;
    }
    if(i.getChosenFormulation() == Input::FORMULATION_EDGE_NODE){
        contextMask |= IloCplex::Callback::Context::Id::Candidate;
        contextMask |= IloCplex::Callback::Context::Id::Relaxation;
    }
    if((i.getChosenFormulation() == Input::FORMULATION_T_FLOW) && (i.getNonOverTFlow() == 0)){
        contextMask |= IloCplex::Callback::Context::Id::Candidate;
        contextMask |= IloCplex::Callback::Context::Id::Relaxation;
    }
    if(i.isGNPYEnabled()){
        contextMask |= IloCplex::Callback::Context::Id::Candidate;
    }
    if(i.isUserCutsActivated()){
        contextMask |= IloCplex::Callback::Context::Id::Relaxation;
    }
    bool report = true;
    /*
    if(report){
        contextMask |= IloCplex::Callback::Context::Id::Candidate;
    }*/
    return contextMask;
}
 
void SolverCplex::solve(){    
    IloNum timeStart = cplex.getCplexTime();
    std::cout << "Solving..." << std::endl;
    std::vector<ObjectiveFunction> myObjectives = formulation->getObjectiveSet();
    for (unsigned int i = 0; i < myObjectives.size(); i++){
        if (i >= 1){
            model.remove(obj);
            setObjective(myObjectives[i]);
        }
        
        CplexCallback myGenericCallback(var, formulation,
                                        formulation->getInstance().getInput(),
                                        formulation->getInstance().getInput().isObj8(i));
        CPXLONG contextMask = context(myObjectives[i].getId(), formulation->getInstance().getInput());

        if(!formulation->getInstance().getInput().isRelaxed()){
            cplex.use(&myGenericCallback, contextMask);
        }
        cplex.exportModel("nom_do_lp.lp");
        //std::ofstream outfile;
        //outfile.open("test.csv");
        //outfile << "UB;LB;nodes;remainingNodes;time"<<std::endl; 
        //cplex.use(infocallback(cplex.getEnv(),outfile,cplex,timeStart));
        std::cout << "Chosen objective: " << myObjectives[i].getName() << std::endl;
        //std::ofstream fout("mylog.log");
        //cplex.setOut(fout);
        //cplex.setWarning(fout);
        //cplex.setError(fout);
        std::string	filename = formulation->getInstance().getInput().getParameterFile() + ".sol";
        if(myObjectives.size()>1){
            try {
                cplex.readMIPStarts(filename.c_str());
                //std::remove(filename.c_str());
            }
            catch(IloException& ex) {
                std::cerr << "No firstRound.sol " << ex << std::endl;
            }
        }

        try {
            cplex.solve();
        }
        catch(IloException& ex) {
            std::cerr << "Error: " << ex << std::endl;
        }
        //outfile.close();
        if ((cplex.getStatus() == IloAlgorithm::Optimal) || (cplex.getStatus() == IloAlgorithm::Feasible)){
            IloNum objValue = cplex.getObjValue();
            std::cout << "Objective Function Value: " << objValue << std::endl;
            /*
            if (i < myObjectives.size() - 1){
                IloExpr objectiveExpression = to_IloExpr(myObjectives[i].getExpression());
                IloRange constraint(model.getEnv(), objValue, objectiveExpression, objValue);
                //std::cout << "Add constraint: " << objectiveExpression << " = " << objValue << std::endl;
                model.add(constraint);
                objectiveExpression.end();
            }
            */
            if(myObjectives.size()>1){
                cplex.writeMIPStarts(filename.c_str());
            }
            
        }else{
            if(cplex.getStatus() == IloAlgorithm::Infeasible){
                std::cout << "Solver proved infeasibility solution..." << std::endl;
            }else{
                // CPLEX STATUS UNKNOWN
                // Stop optimizing.
                std::cout << "Could not find a feasible solution..." << std::endl;
                std::cout << cplex.getStatus() << std::endl;
                i = myObjectives.size()+1;
            }
        }
    }
    
    IloNum timeFinish = cplex.getCplexTime();
    setDurationTime(timeFinish - timeStart);
    //std::cout << cplex.getStatus() << std::endl;
    if ((cplex.getStatus() == IloAlgorithm::Optimal) || (cplex.getStatus() == IloAlgorithm::Feasible)){
        setUpperBound(cplex.getObjValue());
        setLowerBound(cplex.getBestObjValue());
        setMipGap(cplex.getMIPRelativeGap()*100);
    }else{
        if (cplex.getStatus() == IloAlgorithm::Infeasible){
            setUpperBound(-1);
            setLowerBound(-1);
            setMipGap(0);    
        }else{
            // CPLEX STATUS UNKNOWN
            // No feasible solution found but infeasibility not proven, recover relaxation value
            setUpperBound(-1);
            setLowerBound(cplex.getBestObjValue());
            setMipGap(0);
            if ((myObjectives[0].getId() == Input::OBJECTIVE_METRIC_ADS) || (myObjectives[0].getId()  == Input::OBJECTIVE_METRIC_DCB)){
                setUpperBound(cplex.getBestObjValue());
                setLowerBound(-1);
            }
        }
    }
	setTreeSize(cplex.getNnodes());
    setAlgorithm(cplex.getAlgorithm());
    std::cout << "User cuts "<< getCplex().getNcuts(IloCplex::CutUser)<< std::endl;
    std::cout << "Total cuts "<< getNbCutsFromCplex()<< std::endl;
    //int root = cplex.getParam(IloCplex::RootAlg);
    //int node = cplex.getParam(IloCplex::NodeAlg);
    
    std::cout << "Optimization done in " << timeFinish - timeStart << " secs." << std::endl;
    if ((cplex.getStatus() == IloAlgorithm::Optimal) || (cplex.getStatus() == IloAlgorithm::Feasible)){    
        std::cout << "Status: " << cplex.getStatus() << std::endl;
        std::cout << "Objective Function Value: " << cplex.getObjValue() << std::endl;
        displaySolution();
    }else{
        if (cplex.getStatus() == IloAlgorithm::Infeasible){
            std::cout << "Infeasible problem..." << std::endl;
        }else{
            std::cout << "Could not find a feasible solution..." << std::endl;
        }
    }
}

double SolverCplex::getObjValue(){
    return cplex.getObjValue();
}


AbstractSolver::Status SolverCplex::getStatus(){
    setStatus(STATUS_ERROR);

    if (cplex.getStatus() == IloAlgorithm::Unknown) {
        setStatus(STATUS_UNKNOWN);
    }
    if (cplex.getStatus() == IloAlgorithm::Feasible) {
        setStatus(STATUS_FEASIBLE);
    }
    if (cplex.getStatus() == IloAlgorithm::Optimal) {
        setStatus(STATUS_OPTIMAL);
    }
    if (cplex.getStatus() == IloAlgorithm::Infeasible) {
        setStatus(STATUS_INFEASIBLE);
    }
    if (cplex.getStatus() == IloAlgorithm::Unbounded) {
        setStatus(STATUS_UNBOUNDED);
    }
    if (cplex.getStatus() == IloAlgorithm::InfeasibleOrUnbounded) {
        setStatus(STATUS_INFEASIBLE_OR_UNBOUNDED);
    }
    
    if (currentStatus == STATUS_ERROR){
		std::cout << "Got an status error." << std::endl;
		exit(0);
	}
    return currentStatus;
}

/* Returns the total number of CPLEX default cuts applied during optimization. */
IloInt SolverCplex::getNbCutsFromCplex(){
    IloInt cutsFromCplex = 0;
    cutsFromCplex += getCplex().getNcuts(IloCplex::CutCover);
    cutsFromCplex += getCplex().getNcuts(IloCplex::CutGubCover);
    cutsFromCplex += getCplex().getNcuts(IloCplex::CutFlowCover);
    cutsFromCplex += getCplex().getNcuts(IloCplex::CutClique);
    cutsFromCplex += getCplex().getNcuts(IloCplex::CutFrac);
    cutsFromCplex += getCplex().getNcuts(IloCplex::CutMir);
    cutsFromCplex += getCplex().getNcuts(IloCplex::CutFlowPath);
    cutsFromCplex += getCplex().getNcuts(IloCplex::CutDisj); 
    cutsFromCplex += getCplex().getNcuts(IloCplex::CutImplBd);
    cutsFromCplex += getCplex().getNcuts(IloCplex::CutZeroHalf);
    cutsFromCplex += getCplex().getNcuts(IloCplex::CutMCF);
    cutsFromCplex += getCplex().getNcuts(IloCplex::CutLocalCover);
    cutsFromCplex += getCplex().getNcuts(IloCplex::CutTighten);
    cutsFromCplex += getCplex().getNcuts(IloCplex::CutObjDisj);
    cutsFromCplex += getCplex().getNcuts(IloCplex::CutLiftProj);
    cutsFromCplex += getCplex().getNcuts(IloCplex::CutLocalImplBd);
    cutsFromCplex += getCplex().getNcuts(IloCplex::CutBQP);
    cutsFromCplex += getCplex().getNcuts(IloCplex::CutRLT);
    cutsFromCplex += getCplex().getNcuts(IloCplex::CutBenders);
    return cutsFromCplex;
}


void SolverCplex::exportFormulation(const Instance &instance){
    std::string file = "model" + std::to_string(count) + ".lp";
    cplex.exportModel(file.c_str());
    std::cout << "LP model has been exported." << std::endl;
}

void SolverCplex::setCplexParams(const Input &input){
    cplex.setParam(IloCplex::Param::MIP::Display, 2);
    cplex.setParam(IloCplex::Param::MIP::Limits::TreeMemory, 16384);
    //cplex.setParam(IloCplex::Param::MIP::Limits::TreeMemory, 57344);
    cplex.setParam(IloCplex::Param::TimeLimit, input.getIterationTimeLimit());
    cplex.setParam(IloCplex::Param::Threads, 4);
    cplex.setParam(IloCplex::IntParam::WriteLevel, 1);

    if(formulation->getInstance().getInput().isRelaxed()){
        Input::RootMethod rootMethod = formulation->getInstance().getInput().getChosenRootMethod();
        if (rootMethod == Input::ROOT_METHOD_AUTO){
            cplex.setParam(IloCplex::RootAlg, IloCplex::AutoAlg);
            std::cout<< "auto" <<std::endl;
        }
        else if (rootMethod == Input::ROOT_METHOD_PRIMAL){
            cplex.setParam(IloCplex::RootAlg, IloCplex::Primal);
            std::cout<< "primal" <<std::endl;
        }
        else if (rootMethod == Input::ROOT_METHOD_DUAL){
            cplex.setParam(IloCplex::RootAlg, IloCplex::Dual);
            std::cout<< "dual" <<std::endl;
        }
        else if (rootMethod == Input::ROOT_METHOD_NETWORK){
            cplex.setParam(IloCplex::RootAlg, IloCplex::Network);
            std::cout<< "net" <<std::endl;
        }
        else if (rootMethod == Input::ROOT_METHOD_BARRIER){
            cplex.setParam(IloCplex::RootAlg, IloCplex::Barrier);
            std::cout<< "barrier" <<std::endl;
        }
    }
    
    
    std::cout << "CPLEX parameters have been defined..." << std::endl;
}

void SolverCplex::implementFormulation(){
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
    catch(...) {
        std::cerr << "Error: que pasa wey" << std::endl;
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

IloExpr SolverCplex::to_IloExpr(const Expression &e){
    IloExpr exp(model.getEnv());
    for (unsigned int i = 0; i < e.getTerms().size(); i++){
        int index = e.getTerm_i(i).getVar().getId();
        double coefficient = e.getTerm_i(i).getCoeff();
        exp += coefficient*var[index];
    }
    return exp;
}

/** Defines the objective function. **/
void SolverCplex::setObjective(const ObjectiveFunction &myObjective){
    
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
    std::cout << "CPLEX objective has been defined..." << std::endl;
}

/* Defines the constraints needed in the MIP formulation. */
void SolverCplex::setConstraints(const std::vector<Constraint> &myConstraints){
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
    std::cout << "CPLEX constraints have been defined..." << std::endl;
}

void SolverCplex::setVariables(const std::vector<Variable> &myVars){
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
    std::cout << "CPLEX variables have been defined..." << std::endl;
}



/** Displays the value of each variable in the obtained solution. **/
void SolverCplex::displaySolution(){
    for (IloInt i = 0; i < var.getSize(); i++){
        if (cplex.getValue(var[i]) >= EPS){
            std::cout << var[i].getName() << " = " << cplex.getValue(var[i]) << std::endl;
        }
    }
}


/* Builds file results.csv containing information about the main obtained results. */
void SolverCplex::outputLogResults(std::string fileName){
	std::string delimiter = ";";
	std::string filePath = formulation->getInstance().getInput().getOutputPath() + "log_results.csv";
	std::ofstream myfile(filePath.c_str(), std::ios_base::app);
	if (myfile.is_open()){
		myfile << fileName << delimiter;
        myfile << cplex.getStatus() << delimiter;
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
SolverCplex::~SolverCplex(){
    obj.end();
    var.end();
    cplex.end();
    model.end();
    env.end();
}
