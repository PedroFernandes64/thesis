#include "AbstractLagrangianSolver.h"

/****************************************************************************************************************************/
/*										                 Constructor							                			*/
/****************************************************************************************************************************/

AbstractLagSolver::AbstractLagSolver(const Instance &inst, const Status &s): MAX_NB_IT_WITHOUT_IMPROVEMENT(inst.getInput().getNbIterationsWithoutImprovement()), 
        MAX_NB_IT(inst.getInput().getMaxNbIterations()), INITIAL_STEPSIZE(inst.getInput().getInitialLagrangianLambda()),
        MIN_STEPSIZE(0.0001), PRIMAL_ABS_PRECISION(0.0001), UBINIT(__DBL_MAX__/2),DUAL_LIMIT(__DBL_MAX__/2),currentStatus(s),dualinf(false),time(ClockTime::getTimeNow()),generalTime(ClockTime::getTimeNow()){

    lagFormulationFactory factory;
    formulation = factory.createFormulation(inst);

    setFormulationConstTime(time.getTimeInSecFromStart());
    time.setStart(ClockTime::getTimeNow());

    heuristicFactory factoryHeuristic;
    heuristic = factoryHeuristic.createHeuristic(formulation,inst);

    setHeuristicConstTime(time.getTimeInSecFromStart());

    Input::LagFormulation formu = inst.getInput().getChosenLagFormulation();
    Input::NodeMethod method = inst.getInput().getChosenNodeMethod();
    Input::ObjectiveMetric obj = inst.getInput().getChosenObj_k(0);
    Input::DirectionMethod direction = inst.getInput().getChosenDirectionMethod();
    Input::ProjectionType projection = inst.getInput().getChosenProjection();
    bool altstop = inst.getInput().getAlternativeStop();
    int numitwithoutimprov = inst.getInput().getNbIterationsWithoutImprovement();

    std::string lagOutputPath = inst.getInput().getLagOutputPath();

    int demands = formulation->getNbDemandsToBeRouted();

    std::string nom  =  lagOutputPath + "obj" + std::to_string(obj) + "_demands" + std::to_string(demands)+ "_method" + std::to_string(method) + "_formulation" + std::to_string(formu) + "_direction" + std::to_string(direction) + "_projection" + std::to_string(projection) + "_altStop" + std::to_string(altstop) + "_withoutImprov" + std::to_string(numitwithoutimprov) + "_detailed.txt";
    std::string nom2 =  lagOutputPath + "obj" + std::to_string(obj) + "_demands" + std::to_string(demands)+ "_method" + std::to_string(method) + "_formulation" + std::to_string(formu) + "_direction" + std::to_string(direction) + "_projection" + std::to_string(projection) + "_altStop" + std::to_string(altstop) + "_sortie2.txt";
    //std::cout << nom << std::endl;
    fichier.open(nom.c_str());
    //fichier2.open(nom2.c_str());
}

/******************************************************************************************************************************/
/*										                    METHODS  										                  */
/******************************************************************************************************************************/

/* Initializes the variables, constraints and objective function of the studied formulation. */
void AbstractLagSolver::initLagFormulation(){
    formulation->setLagVariables();
    formulation->setLagConstraints();
    formulation->setObjectives();
}
/* Sets the initial lambda used for updating the step size. */
void AbstractLagSolver::initLambda(){
    setLambda(INITIAL_STEPSIZE);
    //std::cout << "> Initial lambda was defined. " << std::endl;
}

bool AbstractLagSolver::isGradientMoving(){
    if (getStepSize() >= MIN_STEPSIZE){
        return true;
    }
    return false;
}

/******************************************************************************************************************************/
/*										                    UPDATE  								                   		  */
/******************************************************************************************************************************/

/* Updates the known upper bound. */
void AbstractLagSolver::updateUB(double bound){
    if (bound < getUB()){
        setUB(bound);
        std::cout << "> Set UB: " << bound << std:: endl;
    }
}

/******************************************************************************************************************************/
/*										                    DISPLAY  									                      */
/******************************************************************************************************************************/

void AbstractLagSolver::displayResults(std::ostream & sortie){
    
    std::string delimiter = ";";

    sortie << getUB() << delimiter;
    sortie << getLB() << delimiter;
    sortie << getIteration() << delimiter;
    sortie << getLambda() << delimiter;
    sortie << getStepSize() << delimiter;
    sortie << getStop() << delimiter;
    sortie << getTotalTime() << delimiter << delimiter;

    sortie << getRSAGraphConstructionTime() << delimiter;
    sortie << getPreprocessingTime() << delimiter;

    sortie << getFormulationConstTime() << delimiter;
    sortie << getHeuristicConstTime() << delimiter;
    sortie << getInitializationTime() << delimiter;
    sortie << getConstAuxGraphTime() << delimiter;

    sortie << getSolvingSubProblemTime() << delimiter;
    sortie << getShorstestPathTime() << delimiter;
    sortie << getUpdateVariablesTime() << delimiter;
    sortie << getSubstractMultipliersTime() << delimiter;
    sortie << getCostTime() << delimiter;

    sortie << getUpdatingSlackTime() << delimiter;
    sortie << getUpdatingBoundsTime() << delimiter;
    sortie << getUpdatingHeuristicBoundTime() << delimiter;
    sortie << getUpdatingMultipliersTime() << delimiter;
    sortie << getStoppingCriterionTime() << delimiter;
    sortie << getUpdatingPrimalVariablesTime() << delimiter;
    sortie << std::endl;
}

/*****************************************************************************************************************************/
/*										                   DESTRUCTOR  									                     */
/*****************************************************************************************************************************/

AbstractLagSolver::~AbstractLagSolver(){
    fichier.close();
    //fichier2.close();
    delete heuristic;
    delete formulation;
}
