#include "lagSubgradient.h"

/****************************************************************************************************************************/
/*					                                 INITIALIZATION 		    		                                    */
/****************************************************************************************************************************/
/* Sets the initial parameters for the subgradient to run. */
void lagSubgradient::initialization(bool initMultipliers){
    /** Setting initial time **/
    setInitializationTime(0.0);
    setConstAuxGraphTime(0.0);
    setSolvingSubProblemTime(0.0);
    setUpdateVariablesTime(0.0);
    setShorstestPathTime(0.0);
    setSubstractMultipliersTime(0.0);
    setUpdatingSlackTime(0.0);
    setUpdatingBoundsTime(0.0);
    setHeuristicBoundTime(0.0);
    setUpdatingMultipliersTime(0.0);
    setUpdatingCostsTime(0.0);
    setStoppingCriterionTime(0.0);
    setUpdatingPrimalVariablesTime(0.0);
    setUpdateStepLambdaTime(0.0);
    setCostTime(0.0);
    
    time.setStart(ClockTime::getTimeNow());
    
    setIteration(0);
    setItWithoutImprovement(0);
    setGlobalItWithoutImprovement(0);
    setStepSize(0.000);

    setLB(-__DBL_MAX__);
    setUB(1000000); //setUB(__DBL_MAX__/2);

    formulation->setStatus(RSA::STATUS_UNKNOWN);

    initLambda();
    formulation->init(initMultipliers);

    UBINIT = formulation->initialUBValue();
    setUB(UBINIT);
    std::cout << "Subgradient: Initial UB: " << UBINIT << std::endl;

    setStatus(STATUS_UNKNOWN);
    setDualInf(false);

    setInitializationTime(time.getTimeInSecFromStart());
    setConstAuxGraphTime(formulation->getConstAuxGraphTime()); 

    feasibleHeuristic = true;

    std::cout << "> Initialization is done. " << std::endl;

    /* Printing information to file fichier2. */
    //fichier2 << "> Initialization is done. " << std::endl;
    //fichier2 << " ****** Iteration: " << getIteration() << " ****** "<< std::endl;
    //formulation->displaySlack(fichier2);
    //formulation->displayMultiplier(fichier2);
    //formulation->createGraphFile(getIteration());
}

/****************************************************************************************************************************/
/*					                                 RUNNING METHODS 		    	                               	        */
/****************************************************************************************************************************/

void lagSubgradient::run(bool initMultipliers, bool modifiedSubproblem){
    std::cout << "--- Subgradient was invoked ---" << std::endl;
    //formulation->displayToBeRouted();

    initialization(initMultipliers);
    std::cout << "> Subgradient was initialized. " << std::endl;

    bool STOP = false;
    while (!STOP){
        runIteration(modifiedSubproblem);
        if (formulation->getStatus() != RSA::STATUS_INFEASIBLE){
            displayMainParameters(fichier);

            time.setStart(ClockTime::getTimeNow());
            updateLambda();
            updateStepSize();
            incUpdateStepLambdaTime(time.getTimeInSecFromStart());
            
            time.setStart(ClockTime::getTimeNow());
            formulation->updateMultiplier(getStepSize());
            incUpdatingMultipliersTime(time.getTimeInSecFromStart());

            time.setStart(ClockTime::getTimeNow());
            bool alternativeStop = formulation->getInstance().getInput().getAlternativeStop();
            if((getLB() >= getUB() - 0.001) && (getLB() < (UBINIT-0.001))){ 
                STOP = true;
                formulation->setStatus(RSA::STATUS_OPTIMAL);
                setStatus(STATUS_OPTIMAL);
                setStop("Optimal");
                std::cout << "Subgradient: Integer Optimal by UB: " << getLB() << getIteration() << std::endl;
            }
            else if(formulation->checkSlacknessCondition() && formulation->checkFeasibility()){
                STOP = true;
                formulation->setStatus(RSA::STATUS_OPTIMAL);
                setStatus(STATUS_OPTIMAL);
                setStop("Optimal");
                std::cout << "Subgradient: Integer Optimal by slackness: " << getLB() << " " << formulation->getLagrCurrentCost() << std::endl;
            }
            else if (getIteration() >= MAX_NB_IT){
                STOP = true;
                setStatus(STATUS_OPTIMAL);
                //setStatus(STATUS_MAX_IT);
                setStop("Max It");
                std::cout << "Subgradient: Maximum number iterations: " << getLB() << std::endl;
            }
            else if (isGradientMoving() == false){
                STOP = true;
                //setStatus(STATUS_FEASIBLE);
                setStatus(STATUS_OPTIMAL);
                setStop("Small Step");
                std::cout << "Subgradient: Small Step: " << getLB() << std::endl;
            }
            else if(alternativeStop){
                if(getGlobalItWithoutImprovement() >= 5*MAX_NB_IT_WITHOUT_IMPROVEMENT){
                    STOP = true;
                    //setStatus(STATUS_MAX_IT);
                    setStatus(STATUS_OPTIMAL);
                    setStop("Alternative stop");
                    std::cout << "Subgradient: Alternative stop." << std::endl;
                }
            }
            if(getLB() >= UBINIT -1){
                STOP = true;
                setStatus(STATUS_INFEASIBLE);
                setStop("Infeasible");
                std::cout << "Subgradient: Primal infeasible, dual unbounded." << std::endl;
            }
            if(getLB() >= DUAL_LIMIT){
                STOP = true;
                setStop("dual limit");
                std::cout << "Subgradient: Dual limit reached." << std::endl;
            }
            incStoppingCriterionTime(time.getTimeInSecFromStart());
        }
        else{
            STOP = true;
            setStatus(STATUS_INFEASIBLE);
            setDualInf(true);
            setStop("Infeasible");
            std::cout << "Subgradient: infeasible sub problem." << std::endl;
        }
        if(STOP==true){
            setTotalTime(getGeneralTime().getTimeInSecFromStart());
            setUpdateVariablesTime(formulation->getUpdateVariablesTime());
            setShorstestPathTime(formulation->getShorstestPathTime());
            setSubstractMultipliersTime(formulation->getSubstractMultipliersTime());
            setCostTime(formulation->getCostTime());
            setRSAGraphConstructionTime(formulation->getRSAGraphConstructionTime());
            setPreprocessingTime(formulation->getPreprocessingTime());
        }
    }

}

void lagSubgradient::runIteration(bool modifiedSubproblem){ 
    incIteration();

    /** Solving sub problem **/
    time.setStart(ClockTime::getTimeNow());
    formulation->run(modifiedSubproblem);
    incSolvingSubProblemTime(time.getTimeInSecFromStart());

    /** Updating direction **/
    time.setStart(ClockTime::getTimeNow());
    formulation->updateDirection();
    incUpdatingSlackTime(time.getTimeInSecFromStart());

    
    /** Updating feasibility **/
    time.setStart(ClockTime::getTimeNow());
    if (formulation->checkFeasibility() == true && formulation->getStatus()!= RSA::STATUS_INFEASIBLE){
        formulation->setStatus(RSA::STATUS_FEASIBLE);
    }

    /** Updating lower bound **/
    updateLB(formulation->getLagrCurrentCost());
    
    /** Updating upper bound **/
    if(formulation->getStatus() == RSA::STATUS_FEASIBLE){
        double feasibleSolutionCost = formulation->getRealCurrentCost();
        if (feasibleSolutionCost < getUB()){
            updateUB(feasibleSolutionCost);
            if(formulation->getInstance().getInput().isObj8(0)){
                formulation->updateMaxUsedSliceOverallUpperBound(feasibleSolutionCost);
            }
            std::cout << " Ub by feasibility." << std::endl;
        }
    }   
    incUpdatingBoundsTime(time.getTimeInSecFromStart());

    time.setStart(ClockTime::getTimeNow());
    if(!modifiedSubproblem){
        if(getIteration()<=5 || getIteration()%30 ==0){
            heuristic->run(modifiedSubproblem);
            double feasibleSolutionCostHeur = heuristic->getCurrentHeuristicCost();
            updateUB(feasibleSolutionCostHeur);
            if(formulation->getInstance().getInput().isObj8(0)){
                formulation->updateMaxUsedSliceOverallUpperBound(feasibleSolutionCostHeur);
            }
        }
    }
    incHeuristicBoundTime(time.getTimeInSecFromStart());
    
    /* Display information at file fichier2 */
    //fichier2 << "\n\n> ****** Iteration: " << getIteration() << " ******"<< std::endl;
    //formulation->displaySlack(fichier2);
    //formulation->displayMultiplier(fichier2);
    //formulation->displaySlack();
    //formulation->displayMultiplier();
}

/****************************************************************************************************************************/
/*					                                      UPDATE 		    		                                        */
/****************************************************************************************************************************/

/* Updates the lambda used in the update of step size. Lambda is halved if LB has failed to increade in some fixed number of iterations */
void lagSubgradient::updateLambda(){
    double last_lambda = getLambda();
    if (last_lambda >= 10e-3){
        if (getItWithoutImprovement() >= MAX_NB_IT_WITHOUT_IMPROVEMENT){
            setItWithoutImprovement(0);
            setLambda(last_lambda / 2.0);
        }
    }
}

/* Updates the known lower bound. */
void lagSubgradient::updateLB(double bound){
    if (bound >= getLB() + __DBL_EPSILON__){
        setLB(bound);
        setItWithoutImprovement(0);
        setGlobalItWithoutImprovement(0);
    }
    else{
        incItWithoutImprovement();
        incGlobalItWithoutImprovement();
    } 
}

/* Updates the step size with the rule: lambda*(UB - Z[u])/|slack| */
void lagSubgradient::updateStepSize(){
    double numerator = getLambda()*(getUB() - formulation->getLagrCurrentCost());
    double denominator = formulation->getSlackDirectionProd();
    double new_stepSize = (numerator/denominator); 
    setStepSize(new_stepSize);
}

void AbstractLagSolver::displayMainParameters(std::ostream & sortie){
    int k = getIteration();
    std::vector<int> sizeOfField;
    sizeOfField.resize(10);
    sizeOfField[0] = 5;
    sizeOfField[1] = 8;
    sizeOfField[2] = 10;
    sizeOfField[3] = 9;
    sizeOfField[4] = 9;
    sizeOfField[5] = 6;
    sizeOfField[6] = 6;
    sizeOfField[7] = 8;
    sizeOfField[8] = 11;
    sizeOfField[9] = 9;
    char space = ' ';

    std::vector<std::string> field;
    field.resize(10);
    if (k == 1){
        field[0] = "Iter";
        field[1] = "Wout Impr";
        field[2] = "LB";
        field[3] = "Lagr Cost";
        field[4] = "Real Cost";
        field[5] = "UB";
        field[6] = "Step";
        field[7] = "Lambda";
        field[8] = "Feasibility";
        field[9] = "Time";
        
        for (unsigned int i = 0; i < field.size(); i++){
            field[i].resize(sizeOfField[i], space);
            sortie << field[i] << " | ";
        }
        sortie << std::endl;;
    }
    field[0] = std::to_string(k);
    field[1] = std::to_string(getItWithoutImprovement());
    field[2] = std::to_string(getLB());
    field[3] = std::to_string(formulation->getLagrCurrentCost());
    field[4] = std::to_string(formulation->getRealCurrentCost());
    field[5] = std::to_string(getUB());
    field[6] = std::to_string(getStepSize());
    field[7] = std::to_string(getLambda());

    if (formulation->checkFeasibility()){
        field[8] = "YES";
    }
    else{
        field[8] = "NO";
    }

    field[9] = std::to_string(generalTime.getTimeInSecFromStart());
    
    for (unsigned int i = 0; i < field.size(); i++){
        field[i].resize(sizeOfField[i], space);
        sortie << field[i] << " | ";

    }
    sortie << std::endl;
}

/******************************************************************************************************************************/
/*										                    DISPLAY  									                      */
/******************************************************************************************************************************/


void lagSubgradient::displayMainParameters(std::ostream & sortie){
    int k = getIteration();
    std::vector<int> sizeOfField;
    sizeOfField.resize(10);
    sizeOfField[0] = 5;
    sizeOfField[1] = 8;
    sizeOfField[2] = 10;
    sizeOfField[3] = 9;
    sizeOfField[4] = 9;
    sizeOfField[5] = 6;
    sizeOfField[6] = 6;
    sizeOfField[7] = 8;
    sizeOfField[8] = 11;
    sizeOfField[9] = 9;
    char space = ' ';

    std::vector<std::string> field;
    field.resize(10);
    if (k == 1){
        field[0] = "Iter";
        field[1] = "Wout Impr";
        field[2] = "LB";
        field[3] = "Lagr Cost";
        field[4] = "Real Cost";
        field[5] = "UB";
        field[6] = "Step";
        field[7] = "Lambda";
        field[8] = "Feasibility";
        field[9] = "Time";
        
        for (unsigned int i = 0; i < field.size(); i++){
            field[i].resize(sizeOfField[i], space);
            sortie << field[i] << " | ";
        }
        sortie << std::endl;;
    }
    field[0] = std::to_string(k);
    field[1] = std::to_string(getItWithoutImprovement());
    field[2] = std::to_string(getLB());
    field[3] = std::to_string(formulation->getLagrCurrentCost());
    field[4] = std::to_string(formulation->getRealCurrentCost());
    field[5] = std::to_string(getUB());
    field[6] = std::to_string(getStepSize());
    field[7] = std::to_string(getLambda());

    if (formulation->checkFeasibility()){
        field[8] = "YES";
    }
    else{
        field[8] = "NO";
    }

    field[9] = std::to_string(generalTime.getTimeInSecFromStart());
    
    for (unsigned int i = 0; i < field.size(); i++){
        field[i].resize(sizeOfField[i], space);
        sortie << field[i] << " | ";

    }
    sortie << std::endl;
}