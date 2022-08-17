#include "lagVolume.h"

/*****************************************************************************************************************************/
/*					                                      INITIALIZATION 		    		                                 */
/*****************************************************************************************************************************/

/* Sets the initial parameters for the subgradient to run. */
void lagVolume::initialization(bool initMultipliers){

    /** Setting initial time **/
    setInitializationTime(0.0);
    setConstAuxGraphTime(0.0);
    setSolvingSubProblemTime(0.0);
    setUpdatingSlackTime(0.0);
    setUpdatingBoundsTime(0.0);
    setHeuristicBoundTime(0.0);
    setUpdatingMultipliersTime(0.0);
    setUpdatingCostsTime(0.0);
    setStoppingCriterionTime(0.0);
    setUpdatingPrimalVariablesTime(0.0);

    time.setStart(ClockTime::getTimeNow());
    setIteration(0);
    setGreenIt(false);
    setNbRedIt(0);
    setNbYellowIt(0);
    setItWithoutImprovement(0);
    setGlobalItWithoutImprovement(0);

    setStepSize(0.000);
    initLambda();

    setLB(-__DBL_MAX__);
    setUB(1000000);
    setTarget(-__DBL_MAX__/2);

    formulation->init(initMultipliers);
    formulation->setStatus(RSA::STATUS_UNKNOWN);

    UBINIT = formulation->initialUBValue();
    setUB(UBINIT);
    std::cout << "Volume: Initial UB: " << UBINIT << std::endl;

    setStatus(STATUS_UNKNOWN);
    setDualInf(false);

    setInitializationTime(time.getTimeInSecFromStart());
    setConstAuxGraphTime(formulation->getConstAuxGraphTime());

    feasibleHeuristic = true; 

    //std::cout << "> Initialization is done. " << std::endl;
    //fichier << "> Initialization is done. " << std::endl;
    //fichier << "> ******* Iteration: " << getIteration() << " ********" << std::endl;
    //formulation->displaySlack(fichier);
    //formulation->displayMultiplier(fichier);
}

/*****************************************************************************************************************************/
/*					                                       RUNNING METHODS 		    		                                 */
/*****************************************************************************************************************************/

void lagVolume::run(bool initMultipliers, bool modifiedSubproblem){
    //std::cout << "--- Volume was invoked ---" << std::endl;
    //formulation->displayToBeRouted();

    initialization(initMultipliers);

    const int ascent_first_check = std::max(ASCENT_CHECK_INVL,ASCENT_FIRST_CHECK);
    sequence_dualCost.resize(ascent_first_check,0.0);

    //std::cout << "> Volume was initialized. " << std::endl;

    bool STOP = false;
    while (!STOP){
        runIteration(modifiedSubproblem);
        if (formulation->getStatus() != RSA::STATUS_INFEASIBLE){
            //displayMainParameters(fichier);

            updateLambda();
            updateStepSize();
            
            time.setStart(ClockTime::getTimeNow());
            formulation->updateMultiplier_v2(getStepSize());
            incUpdatingMultipliersTime(time.getTimeInSecFromStart());

            time.setStart(ClockTime::getTimeNow());

            const bool primal_feas = formulation->getMeanSlackModule_v2()  < MIN_MODULE_VALUE;
            const double gap = std::abs(formulation->getPrimalCurrentCost() - getLB());
            const bool small_gap = std::abs(getLB()) < 0.0001 ?(gap < 0.0) : ((gap < 0.0) || (gap/std::abs(getLB()) < MIN_DIF_OBJ_VALUE)); 
            const int k = getIteration() % ASCENT_CHECK_INVL;
            bool alternativeStop = formulation->getInstance().getInput().getAlternativeStop();

            if(formulation->checkSlacknessCondition() && formulation->checkFeasibility()){
                STOP = true;
                formulation->setStatus(RSA::STATUS_OPTIMAL);
                setStatus(STATUS_OPTIMAL);
                setStop("Optimal");
                formulation->changePrimalApproximation();
                std::cout << "Volume: Integer Optimal by slackness: " << getLB() << " " << formulation->getLagrCurrentCost() << std::endl;
            }
            else if((getLB() >= (getUB() - 0.001)) && (getLB() < (UBINIT-0.001))){ // Test optimality IP
                STOP = true;
                formulation->setStatus(RSA::STATUS_OPTIMAL);
                setStatus(STATUS_OPTIMAL);
                setStop("Optimal");
                std::cout << "Volume: Integer Optimal by UB: " << getLB() << " " << getUB() << std::endl;
                formulation->changePrimalApproximationToBestFeasibleSol();
            }
            else if ((getIteration()>1) && primal_feas && small_gap){ // Test optimality LP
                STOP = true;
                setStatus(STATUS_OPTIMAL);
                setStop("Small lp gap ");
                std::cout << "Volume: Small linear program gap: " << getLB() << std::endl;
            }
            else if(getIteration() >= MAX_NB_IT){
                STOP = true;
                //setStatus(STATUS_OPTIMAL);
                setStatus(STATUS_MAX_IT);
                setStop("Max It");
                std::cout << "Volume: Maximum number iterations: " << getLB() << std::endl;
            }
            else if(getIteration() > ascent_first_check){
                if(getLB() - sequence_dualCost[k] < std::abs(sequence_dualCost[k])* MINIMUM_REL_ASCENT){
                    STOP = true;
                    setStop("Small improvement");
                    std::cout <<   std::fixed<< getLB() << " " << getUB() << std::setprecision(9)<< std::endl;
                    //setStatus(STATUS_FEASIBLE);
                    //setStatus(STATUS_ABORTED);
                    setStatus(STATUS_OPTIMAL);
                    std::cout << "Volume: LB with small improvement: " << getLB() << std::endl;
                }
            }           
            else if(alternativeStop){
                if(getGlobalItWithoutImprovement() >= 5*MAX_NB_IT_WITHOUT_IMPROVEMENT){
                    STOP = true;
                    //setStatus(STATUS_FEASIBLE);
                    setStatus(STATUS_OPTIMAL);
                    setStop("Alternative stop");
                    std::cout << "Volume: Alternative stop: " << getLB() << std::endl;
                }
            }
            if(getLB() >= UBINIT){
                STOP = true;
                setStatus(STATUS_INFEASIBLE);
                setStop("Infeasible");
                std::cout << "Volume: Primal infeasible, dual unbounded." << std::endl;
            }
            if(getLB() >= DUAL_LIMIT){
                STOP = true;
                setStop("dual limit");
                std::cout << "Volume: Dual limit reached." << std::endl;
            }
            sequence_dualCost[k] = getLB();
            incStoppingCriterionTime(time.getTimeInSecFromStart());
        }
        else{
            STOP = true;
            setStatus(STATUS_INFEASIBLE);
            setDualInf(true);
            setStop("Infeasible");
            std::cout << "Volume: infeasible sub problem." << std::endl;
        }
        if(STOP==true){
            setTotalTime(getGeneralTime().getTimeInSecFromStart());
            setUpdateVariablesTime(formulation->getUpdateVariablesTime());
            setShorstestPathTime(formulation->getShorstestPathTime());
            setSubstractMultipliersTime(formulation->getSubstractMultipliersTime());
            setCostTime(formulation->getCostTime());
            setRSAGraphConstructionTime(formulation->getRSAGraphConstructionTime());
            setPreprocessingTime(formulation->getPreprocessingTime());
            if(formulation->isInteger()){
                std::cout << "The solution is integer." << std::endl;
            }
            if(modifiedSubproblem){
                formulation->verifyLowerUpperBound();
            }
        }
    }
}

void lagVolume::runIteration(bool modifiedSubproblem){ 

    incIteration();

    /* Running sub problem */
    time.setStart(ClockTime::getTimeNow());
    formulation->run(modifiedSubproblem);
    incSolvingSubProblemTime(time.getTimeInSecFromStart());

    /* Initialize primal solution if first iteration */
    time.setStart(ClockTime::getTimeNow());
    if(getIteration()==1){
        formulation->initPrimalApproximation();
    }
    incUpdatingPrimalVariablesTime(time.getTimeInSecFromStart());
    
    time.setStart(ClockTime::getTimeNow());
    /* Updating feasibility */
    if (formulation->checkFeasibility() == true && formulation->getStatus()!= RSA::STATUS_INFEASIBLE ){
        formulation->setStatus(RSA::STATUS_FEASIBLE);
    }

    /* Updating LOWER BOUND*/
    updateLB(formulation->getLagrCurrentCost());

    updateTarget();

    /* Updating UPPER BOUND*/
    if(formulation->getStatus() == RSA::STATUS_FEASIBLE){
        double feasibleSolutionCost = formulation->getRealCurrentCost();
        if (feasibleSolutionCost < getUB()){
            updateUB(feasibleSolutionCost);
            if(formulation->getInstance().getInput().isObj8(0)){
                formulation->updateMaxUsedSliceOverallUpperBound(feasibleSolutionCost);
            }
            formulation->changeBestSolutionWithPrimalSolution();
            std::cout << " Ub by feasibility." << std::endl;
        }
    }
    incUpdatingBoundsTime(time.getTimeInSecFromStart());

    if(getIteration()%100==0){
        updateParamAlpha();
    }
    time.setStart(ClockTime::getTimeNow());
    if(getIteration()==1){
        previous_lb = getLB();
    }
    else{
        double alpha = getAlphaValue(); 
        formulation->updatePrimalApproximation(alpha);
    }
    incUpdatingPrimalVariablesTime(time.getTimeInSecFromStart());

    time.setStart(ClockTime::getTimeNow());
    if(getIteration()<=5 || getIteration()%30 ==0){
        if(feasibleHeuristic){
            heuristic->run(modifiedSubproblem);
            if(heuristic->getStatus() == AbstractHeuristic::STATUS_INFEASIBLE){
                feasibleHeuristic = false;
            }
            if(heuristic->getInfeasibilityByBound()){
                formulation->setStatus(AbstractFormulation::STATUS_INFEASIBLE);
            }
            double feasibleSolutionCostHeur = heuristic->getCurrentHeuristicCost();
            //std::cout << feasibleSolutionCostHeur << std::endl;
            if (feasibleSolutionCostHeur < getUB()){
                //heuristic->display();
                updateUB(feasibleSolutionCostHeur);
                if(formulation->getInstance().getInput().isObj8(0)){
                    formulation->updateMaxUsedSliceOverallUpperBound(feasibleSolutionCostHeur);
                }
                formulation->changeBestSolution(heuristic->getSolution(),heuristic->getVarP());
            }
        }
    }
    incHeuristicBoundTime(time.getTimeInSecFromStart());

    //fichier << "\n\n> ****** Iteration: " << getIteration() << " ******"<< std::endl;
    //formulation->displaySlack(fichier);
    //formulation->displayMultiplier(fichier);
    //formulation->displaySlack();
    //formulation->displayMultiplier();
}

/******************************************************************************************************************************/
/*					                                           UPDATE 		    		                                      */
/******************************************************************************************************************************/

/* Updates the known lower bound. */
void lagVolume::updateLB(double bound){
    if (bound >= getLB() + __DBL_EPSILON__){
        setLB(bound);
        formulation->updateStabilityCenter();
        double prod = formulation->getSlackPrimalSlackProd();
        if(prod < 0){ //yellow iteration
            setGreenIt(false);
            setNbRedIt(0);
            incNbYellowIt();
        }else{  // green iteration
            setGreenIt(true);
            setNbRedIt(0);
            setNbYellowIt(0);
        }   
        setGlobalItWithoutImprovement(0);
    }
    else{  // red iteration
        setGreenIt(false);
        setNbYellowIt(0);
        incNbRedIt();
        incGlobalItWithoutImprovement();
    } 
}

/* Updates the known step size. */
void lagVolume::updateStepSize(){
    //double numerator = getLambda()*(getUB() - getLB());
    double numerator = getLambda()*(getTarget() - getLB());
    double denominator = formulation->getSlackModule_v2();
    double new_stepSize = (numerator/denominator); 
    setStepSize(new_stepSize);
}

/* Updates the known lambda - according to red, green, yellow iteration. */
void lagVolume::updateLambda(){
    double last_lambda = getLambda();
    if(getGreenIt()){
        setLambda(std::min(2.0,last_lambda*UPD_STEPSIZE_GREEN_YELLOW));
        setNbYellowIt(0);
        setNbRedIt(0);
    }
    else if(getNbYellowIt() >= MAX_NB_IT_YELLOW){
        setLambda(std::min(2.0,last_lambda*UPD_STEPSIZE_GREEN_YELLOW));
        setNbYellowIt(0);
        setNbRedIt(0);
    }
    else if(last_lambda >= 0.0005){
        if(getNbRedIt() >= MAX_NB_IT_RED){
            setLambda(last_lambda*UPD_STEPSIZE_RED);
            setNbRedIt(0);
            setNbYellowIt(0);
        }
    }
}

/* Updates the alpha for the convex combinational of the primal vector. */
double lagVolume::getAlphaValue(){

    double alpha;

    double prod_hv = formulation->getSlackPrimalSlackProd(MAX_ALPHA);
    double prod_vv = formulation->getSlackModule(MAX_ALPHA);
    double prod_hh = formulation->getSlackModule_v2(MAX_ALPHA);

    double aux = (MAX_ALPHA*prod_vv - prod_hv)/ (prod_vv- prod_hv);

    if(prod_vv - 2*prod_hv + prod_hh > 0){
        alpha = (prod_hh- prod_hv)/(prod_vv - 2*prod_hv + prod_hh);
    }else{
        alpha = MAX_ALPHA;
    }

    if(alpha > MAX_ALPHA)
        alpha = MAX_ALPHA;
    if(alpha < aux)
        alpha = aux;
    if(alpha > 1.0)
        alpha = MAX_ALPHA;
    if(alpha < 0.0)
        alpha = MAX_ALPHA/10;
    
    return alpha;
}

/* Update the bound for alpha */
void lagVolume::updateParamAlpha(){
    if(MAX_ALPHA >= 10e-5){
        if((getLB()-previous_lb)/previous_lb < 0.01){
            MAX_ALPHA = MAX_ALPHA/2;
        }
    }
    previous_lb = getLB();
}

void lagVolume::updateTarget(){

    double target_ = getTarget();
    if (getLB() >= target_ - std::abs(target_)*0.05) {
        if (std::abs(getLB()) < 10.0) {
            target_ = 10.0;
        }else { 
	        target_ += 0.025 * std::abs(target_);
	        target_ = std::max(target_, getLB() + 0.05 * std::abs(getLB()));
        }
    }
    setTarget(target_);
}

/******************************************************************************************************************************/
/*										                    DISPLAY  									                      */
/******************************************************************************************************************************/

void lagVolume::displayMainParameters(std::ostream & sortie){
    int k = getIteration();
    std::vector<int> sizeOfField;
    sizeOfField.resize(15);
    sizeOfField[0] = 5;
    sizeOfField[1] = 8;
    sizeOfField[2] = 8;
    sizeOfField[3] = 8;
    sizeOfField[4] = 8;
    sizeOfField[5] = 10;
    sizeOfField[6] = 9;
    sizeOfField[7] = 9;
    sizeOfField[8] = 9;
    sizeOfField[9] = 6;
    sizeOfField[10] = 6;
    sizeOfField[11] = 8;
    sizeOfField[12] = 8;
    sizeOfField[13] = 11;
    sizeOfField[14] = 9;
    char space = ' ';

    std::vector<std::string> field;
    field.resize(15);
    if (k == 1){
        field[0] = "Iter";
        field[1] = "Wout Impr";
        field[2] = "Green It";
        field[3] = "Nb Yellow it";
        field[4] = "Nb Red It";
        field[5] = "LB";
        field[6] = "Lagr Cost";
        field[7] = "Real Cost";
        field[8] = "Primal Cost";
        field[9] = "UB";
        field[10] = "Step";
        field[11] = "Lambda";
        field[12] = "Alpha";
        field[13] = "Feasibility";
        field[14] = "Time";
        
        for (unsigned int i = 0; i < field.size(); i++){
            field[i].resize(sizeOfField[i], space);
            sortie << field[i] << " | ";
        }
        sortie << std::endl;;
    }
    field[0] = std::to_string(k);
    field[1] = std::to_string(getGlobalItWithoutImprovement());
    field[2] = std::to_string(getGreenIt());
    field[3] = std::to_string(getNbYellowIt());
    field[4] = std::to_string(getNbRedIt());
    field[5] = std::to_string(getLB());
    field[6] = std::to_string(formulation->getLagrCurrentCost());
    field[7] = std::to_string(formulation->getRealCurrentCost());
    field[8] = std::to_string(formulation->getPrimalCurrentCost());
    field[9] = std::to_string(getUB());
    field[10] = std::to_string(getStepSize());
    field[11] = std::to_string(getLambda());
    field[12] = std::to_string(MAX_ALPHA);

    if (formulation->checkFeasibility_v2()){
        field[13] = "YES";
    }
    else{
        field[13] = "NO";
    }

    field[14] = std::to_string(generalTime.getTimeInSecFromStart());
    
    for (unsigned int i = 0; i < field.size(); i++){
        field[i].resize(sizeOfField[i], space);
        sortie << field[i] << " | ";

    }
    sortie << std::endl;
}