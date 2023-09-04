#include "callbackCplex.h"

CplexCallback::CplexCallback(const IloNumVarArray _var, AbstractFormulation* &_formulation, const Input& _input, bool _obj8): var(_var), input(_input), obj8(_obj8){ 
    formulation = _formulation;
    upperBound = IloInfinity;
}

void CplexCallback::invoke (const IloCplex::Callback::Context &context){
    std::cout<<"Verifying if callback should be called" << std::endl;
    if ( context.inRelaxation() ){
        if (isObj8()){
            //fixVariables(context);
        }
        if ( input.isUserCutsActivated() ){
            addUserCuts(context);
        }
    }
    if ( context.inCandidate()){
        if (input.getChosenFormulation() == Input::FORMULATION_EDGE_NODE){
            addLazyConstraints(context);
        }
        if (input.isGNPYEnabled()){
            addGnpyConstraints(context);
        }
        if (input.isOSNREnabled()){
            addGNModelConstraints(context);
        }
    }
}

void CplexCallback::addGnpyConstraints(const IloCplex::Callback::Context &context) const{
    //std::cout << "Callback gnpy constraints..." << std::endl;
    if ( !context.isCandidatePoint() ){
        throw IloCplex::Exception(-1, "Unbounded solution");
    }
    try {
        
        int const threadNo = context.getIntInfo(IloCplex::Callback::Context::Info::ThreadId);
        std::vector<Constraint> constraint = formulation->solveSeparationGnpy(getIntegerSolution(context), threadNo);
        if (!constraint.empty()){
            //std::cout << "A lazy constraint was found:";
            for (unsigned int i = 0; i < constraint.size(); i++){
                //constraint.display();
                IloRange cut(context.getEnv(), constraint[i].getLb(), to_IloExpr(context, constraint[i].getExpression()), constraint[i].getUb());
                //std::cout << "CPLEX add lazy: " << cut << std::endl;
                context.rejectCandidate(cut);
            }
        }
    }
    catch (...) {
        throw;
    }
}

void CplexCallback::addGNModelConstraints(const IloCplex::Callback::Context &context) const{
    std::cout << "Callback GN Model constraints..." << std::endl;
    if ( !context.isCandidatePoint() ){
        throw IloCplex::Exception(-1, "Unbounded solution");
    }
    try {
        
        int const threadNo = context.getIntInfo(IloCplex::Callback::Context::Info::ThreadId);
        //std::vector<Constraint> constraint = formulation->solveSeparationGnpy(getIntegerSolution(context), threadNo);
        std::vector<Constraint> constraint = formulation->solveSeparationGNModel(getIntegerSolution(context), threadNo);
        /*
        if (!constraint.empty()){
            std::cout << "A lazy constraint was found:";
            for (unsigned int i = 0; i < constraint.size(); i++){
                constraint[i].display();
                IloRange cut(context.getEnv(), constraint[i].getLb(), to_IloExpr(context, constraint[i].getExpression()), constraint[i].getUb());
                //std::cout << "CPLEX add lazy: " << cut << std::endl;
                context.rejectCandidate(cut);
            }
        }*/
    }
    catch (...) {
        throw;
    }
}

void CplexCallback::addUserCuts(const IloCplex::Callback::Context &context) const{
    
    //std::cout << "Callback user cuts..." << std::endl;
    try {
        std::vector<Constraint> constraint = formulation->solveSeparationProblemFract(getFractionalSolution(context));
        if (!constraint.empty()){
            //std::cout << "A violated cut was found: ";
            for (unsigned int i = 0; i < constraint.size(); i++){
                //std::cout << "Adding user cut..." << std::endl;
                context.addUserCut( IloRange(context.getEnv(), constraint[i].getLb(), 
                                        to_IloExpr(context, constraint[i].getExpression()),
                                        constraint[i].getUb(), constraint[i].getName().c_str()),
                                    IloCplex::UseCutForce, IloFalse);
            }
        }
    }
    catch (...) {
        throw;
    }
}

void CplexCallback::addLazyConstraints(const IloCplex::Callback::Context &context) const{
    //std::cout << "Callback lazy constraints..." << std::endl;
    if ( !context.isCandidatePoint() ){
        throw IloCplex::Exception(-1, "Unbounded solution");
    }
    try {
        
        int const threadNo = context.getIntInfo(IloCplex::Callback::Context::Info::ThreadId);
        std::vector<Constraint> constraint = formulation->solveSeparationProblemInt(getIntegerSolution(context), threadNo);
        if (!constraint.empty()){
            //std::cout << "A lazy constraint was found:";
            for (unsigned int i = 0; i < constraint.size(); i++){
                //constraint.display();
                IloRange cut(context.getEnv(), constraint[i].getLb(), to_IloExpr(context, constraint[i].getExpression()), constraint[i].getUb());
                //std::cout << "CPLEX add lazy: " << cut << std::endl;
                context.rejectCandidate(cut);
            }
        }
        else{
            //std::cout << "The solution is valid." << std::endl;
        }
    }
    catch (...) {
        throw;
    }
}


void CplexCallback::fixVariables(const IloCplex::Callback::Context &context){
    //std::cout << "Callback fixing..." << std::endl;
    try {
        /* TODO call formulation fixing*/ 
        if (context.getIntInfo(IloCplex::Callback::Context::Info::Feasible)){
            double currentUB = context.getIncumbentObjective();
            if (currentUB < getUpperBound()){ 
                std::cout << "Callback fixing..." << std::endl;
                setUpperBound(currentUB);
                std::vector<Variable> vars = formulation->objective8_fixing(currentUB);
                for (unsigned int i = 0; i < vars.size(); i++){
                    int index = vars[i].getId();
                    if (context.getRelaxationPoint(var[index]) >= 0.0001){
                        IloExpr exp(context.getEnv());
                        exp += var[index];
                        IloRange cut(context.getEnv(), 0, exp, 0);
                        std::cout << "Add fixing..." << std::endl;
                        context.addUserCut(cut, IloCplex::UseCutForce, IloTrue);
                    }
                }
            }
        }
    }
    catch (...) {
        throw;
    }
}



std::vector<double> CplexCallback::getIntegerSolution(const IloCplex::Callback::Context &context) const{
    const int NB_VAR = var.getSize();
    std::vector<double> solution(NB_VAR);
    for (int i = 0; i < NB_VAR; i++){
        solution[i] = context.getCandidatePoint(var[i]);
    }
    return solution;
}

std::vector<double> CplexCallback::getFractionalSolution(const IloCplex::Callback::Context &context) const{
    const int NB_VAR = var.getSize();
    std::vector<double> solution(NB_VAR);
    for (int i = 0; i < NB_VAR; i++){
        solution[i] = context.getRelaxationPoint(var[i]);
    }
    return solution;
}


IloExpr CplexCallback::to_IloExpr(const IloCplex::Callback::Context &context, const Expression &e) const{
    IloExpr exp(context.getEnv());
    for (int i = 0; i < e.getNbTerms(); i++){
        int index = e.getTerm_i(i).getVar().getId();
        double coefficient = e.getTerm_i(i).getCoeff();
        exp += coefficient*var[index];
    }
    return exp;
}

// Destructor
CplexCallback::~CplexCallback(){}

