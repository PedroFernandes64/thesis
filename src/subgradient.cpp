#include "subgradient.h"


Subgradient::Subgradient(const Instance &inst) : RSA(inst), 
        MAX_NB_IT_WITHOUT_IMPROVEMENT(instance.getInput().getNbIterationsWithoutImprovement()), 
        MAX_NB_IT(instance.getInput().getMaxNbIterations()),
        MIN_STEPSIZE(0.00001){
    
    std::cout << "--- Subgradient was invoked ---" << std::endl;
    displayToBeRouted();

    initialization();
    std::cout << "> Subgradient was initialized. " << std::endl;

    bool STOP = false;
    while (!STOP){
        runIteration();
        if (getStatus() != STATUS_INFEASIBLE){
            displayMainParameters();

            updateLambda();
            updateStepSize();
            updateMultiplier();
            updateCosts();
            
            if (getLB() >= getUB() - DBL_EPSILON){
                //setIsOptimal(true);
                setStatus(STATUS_OPTIMAL);
                STOP = true;
            }
            if (getIteration() >= MAX_NB_IT){
                STOP = true;
            }
            if (isGradientMoving() == false){
                STOP = true;
            }
        }
        else{
            STOP = true;
        }
    }
}

bool Subgradient::isGradientMoving(){
    if (getStepSize() >= MIN_STEPSIZE){
        return true;
    }
    return false;
}

/* Sets the initial lagrangian multipliers associated with length constraints. */
void Subgradient::initializeLengthMultipliers(){
    double initialMultiplier = instance.getInput().getInitialLagrangianMultiplier();
    lagrangianMultiplierLength.resize(getNbDemandsToBeRouted());
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        lagrangianMultiplierLength[d] = initialMultiplier;
    }
}

/* Sets the initial lagrangian multipliers associated with non-overlapping constraints. */
void Subgradient::initializeOverlapMultipliers(){
    double initialMultiplier = instance.getInput().getInitialLagrangianMultiplier();
    lagrangianMultiplierOverlap.resize(getNbDemandsToBeRouted());
    for (int d1 = 0; d1 < getNbDemandsToBeRouted(); d1++){
        lagrangianMultiplierOverlap[d1].resize(getNbDemandsToBeRouted());
        for (int d2 = 0; d2 < getNbDemandsToBeRouted(); d2++){
            lagrangianMultiplierOverlap[d1][d2].resize(instance.getNbEdges());
            for (int e = 0; e < instance.getNbEdges(); e++){
                lagrangianMultiplierOverlap[d1][d2][e].resize(instance.getPhysicalLinkFromIndex(e).getNbSlices());
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    lagrangianMultiplierOverlap[d1][d2][e][s] = initialMultiplier;
                }
            }
        }
    }
}


/* Sets the initial lagrangian multipliers associated with 1st set of improved non-overlapping constraints. */
void Subgradient::initializeOverlap_1Multipliers(){
    double initialMultiplier = instance.getInput().getInitialLagrangianMultiplier();
    lagrangianMultiplierOverlap_1.resize(getNbDemandsToBeRouted());
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        lagrangianMultiplierOverlap_1[d].resize(getNbLoadsToBeRouted());
        for (int w = 0; w < getNbLoadsToBeRouted(); w++){
            lagrangianMultiplierOverlap_1[d][w].resize(instance.getNbEdges());
            for (int e = 0; e < instance.getNbEdges(); e++){
                lagrangianMultiplierOverlap_1[d][w][e].resize(instance.getPhysicalLinkFromIndex(e).getNbSlices());
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    lagrangianMultiplierOverlap_1[d][w][e][s] = initialMultiplier;
                }
            }
        }
    }
}

/* Sets the initial lagrangian multipliers associated with 2nd set of improved non-overlapping constraints. */
void Subgradient::initializeOverlap_2Multipliers(){
    double initialMultiplier = instance.getInput().getInitialLagrangianMultiplier();
    lagrangianMultiplierOverlap_2.resize(getNbLoadsToBeRouted());
    for (int w1 = 0; w1 < getNbLoadsToBeRouted(); w1++){
        lagrangianMultiplierOverlap_2[w1].resize(getNbLoadsToBeRouted());
        for (int w2 = 0; w2 < getNbLoadsToBeRouted(); w2++){
            lagrangianMultiplierOverlap_2[w1][w2].resize(instance.getNbEdges());
            for (int e = 0; e < instance.getNbEdges(); e++){
                lagrangianMultiplierOverlap_2[w1][w2][e].resize(instance.getPhysicalLinkFromIndex(e).getNbSlices());
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    lagrangianMultiplierOverlap_2[w1][w2][e][s] = initialMultiplier;
                }
            }
        }
    }
}

/* Sets the initial lambda used for updating the step size. */
void Subgradient::initLambda(){
    setLambda(instance.getInput().getInitialLagrangianLambda());
    std::cout << "> Initial lambda was defined. " << std::endl;
}


/* Sets the initial lagrangian multipliers for the subgradient to run. */
void Subgradient::initMultipliers(){
    initializeLengthMultipliers();
    //initializeOverlapMultipliers();
    initializeOverlap_1Multipliers();
    initializeOverlap_2Multipliers();
    
    std::cout << "> Initial Lagrangian multipliers were defined. " << std::endl;
}

/* Initializes the assignement matrix. */
void Subgradient::initAssignmentMatrix(){
    assignmentMatrix.resize(getNbDemandsToBeRouted());
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        assignmentMatrix[d].resize(countArcs(*vecGraph[d]));
        std::fill(assignmentMatrix[d].begin(), assignmentMatrix[d].end(), false);
    }
    
    std::cout << "> Initial Assignment matrix was defined. " << std::endl;
}

/* Initializes the slack of length constraints. */
void Subgradient::initializeLengthSlacks(){
    lengthSlack.resize(getNbDemandsToBeRouted(), 0.0);
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        lengthSlack[d] = getToBeRouted_k(d).getMaxLength();
    }
}

/* Initializes the slack of non-overlap constraints. */
void Subgradient::initializeOverlapSlacks(){
    overlapSlack.resize(getNbDemandsToBeRouted());
    for (int d1 = 0; d1 < getNbDemandsToBeRouted(); d1++){
        overlapSlack[d1].resize(getNbDemandsToBeRouted());
        for (int d2 = 0; d2 < getNbDemandsToBeRouted(); d2++){
            overlapSlack[d1][d2].resize(instance.getNbEdges());
            for (int e = 0; e < instance.getNbEdges(); e++){
                overlapSlack[d1][d2][e].resize(instance.getPhysicalLinkFromIndex(e).getNbSlices());
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    overlapSlack[d1][d2][e][s] = 1;
                }
            }
        }
    }
}

/* Initializes the slack of 1st set of improved non-overlap constraints. */
void Subgradient::initializeOverlap_1Slacks(){
    overlapSlack_1.resize(getNbDemandsToBeRouted());
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        overlapSlack_1[d].resize(getNbLoadsToBeRouted());
        for (int w = 0; w < getNbLoadsToBeRouted(); w++){
            overlapSlack_1[d][w].resize(instance.getNbEdges());
            for (int e = 0; e < instance.getNbEdges(); e++){
                overlapSlack_1[d][w][e].resize(instance.getPhysicalLinkFromIndex(e).getNbSlices());
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    overlapSlack_1[d][w][e][s] = 1;
                }
            }
        }
    }
}

/* Initializes the slack of 2nd set of improved non-overlap constraints. */
void Subgradient::initializeOverlap_2Slacks(){
    overlapSlack_2.resize(getNbLoadsToBeRouted());
    for (int w1 = 0; w1 < getNbLoadsToBeRouted(); w1++){
        overlapSlack_2[w1].resize(getNbLoadsToBeRouted());
        for (int w2 = 0; w2 < getNbLoadsToBeRouted(); w2++){
            overlapSlack_2[w1][w2].resize(instance.getNbEdges());
            for (int e = 0; e < instance.getNbEdges(); e++){
                overlapSlack_2[w1][w2][e].resize(instance.getPhysicalLinkFromIndex(e).getNbSlices());
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    overlapSlack_2[w1][w2][e][s] = 1;
                }
            }
        }
    }
}
/* Initializes the slack of relaxed constraints. */
void Subgradient::initSlacks(){
    initializeLengthSlacks();
    //initializeOverlapSlacks();
    initializeOverlap_1Slacks();
    initializeOverlap_2Slacks();
    
    std::cout << "> Initial Slacks were defined. " << std::endl;
}

/* Initializes the costs in the objective function. */
void Subgradient::initCosts(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        cost.emplace_back( std::make_shared<ArcCost>((*vecGraph[d]), 0.0)); 
        //Old version: If compile is ok, delete this line.
        //cost.emplace_back(new ArcCost((*vecGraph[d]), 0.0));
    }
    updateCosts();

    std::cout << "> Initial costs were defined. " << std::endl;
}

/* Sets the initial parameters for the subgradient to run. */
void Subgradient::initialization(){
    setIteration(0);
    setItWithoutImprovement(0);

    setLB(-__DBL_MAX__);
    setUB(__DBL_MAX__);
    //setUB(50000);

    setStatus(STATUS_UNKNOWN);
    //setIsOptimal(false);
    //setIsFeasible(false);
    //setIsUnfeasible(false);

    initMultipliers();
    initLambda();
    initAssignmentMatrix();
    initSlacks();
    initCosts();

    std::cout << "> Initialization is done. " << std::endl;
}


/* Updates the arc costs according to the last lagrangian multiplier available. cost = c + u_k*length */
void Subgradient::updateCosts(){
    // Original costs
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
            setArcCost(a, d, getCoeff(a, d));
        }
    }

    // Length multipliers
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        double multiplier =  getLengthMultiplier_k(d);
        for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
            double incrementValue = multiplier*getArcLength(a, d);
            incArcCost(a, d, incrementValue);
        }
    }

    // Overlap 1 multipliers
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        int demandLoad = getToBeRouted_k(d).getLoad();
        for (int w = 0; w < getNbLoadsToBeRouted(); w++){
            int minLoad = getLoadsToBeRouted_k(w);
            for (int e = 0; e < instance.getNbEdges(); e++){
                int linkLabel = instance.getPhysicalLinkFromIndex(e).getId();
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    double multiplier = getOverlapMultiplier1_k(d, w, e, s);
                    for (int d1 = 0; d1 < getNbDemandsToBeRouted(); d1++){
                        if ((d1 != d) && (getToBeRouted_k(d1).getLoad() >= minLoad)){
                            for (ListDigraph::ArcIt a(*vecGraph[d1]); a != INVALID; ++a){
                                if( (getArcLabel(a, d1) == linkLabel) && (getArcSlice(a, d1) == s) ){
                                    double incrementValue = multiplier;
                                    incArcCost(a, d1, incrementValue);
                                }
                            }
                        }
                    }
                    for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                        if( (getArcLabel(a, d) == linkLabel) && (getArcSlice(a, d) >= s - minLoad + 1) && (getArcSlice(a, d) <= s + demandLoad - 1) ){
                            double incrementValue = multiplier;
                            incArcCost(a, d, incrementValue);
                        }
                    }
                }
            }
        }
    }

    
    // Overlap 2 multipliers
    for (int w1 = 0; w1 < getNbLoadsToBeRouted(); w1++){
        int minLoad_1 = getLoadsToBeRouted_k(w1);
        for (int w2 = 0; w2 < getNbLoadsToBeRouted(); w2++){
            int minLoad_2 = getLoadsToBeRouted_k(w2);
            for (int e = 0; e < instance.getNbEdges(); e++){
                int linkLabel = instance.getPhysicalLinkFromIndex(e).getId();
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    double multiplier = getOverlapMultiplier2_k(w1, w2, e, s);
                    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
                        int demandLoad = getToBeRouted_k(d).getLoad();
                        if ( (demandLoad >= minLoad_1) && (demandLoad <= minLoad_2 - 1) ){
                            for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                                if( (getArcLabel(a, d) == linkLabel) && (getArcSlice(a, d) >= s)  && (getArcSlice(a, d) <= s + minLoad_1 - 1) ){
                                    double incrementValue = multiplier;
                                    incArcCost(a, d, incrementValue);
                                }
                            }
                        }
                    }
                    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
                        int demandLoad = getToBeRouted_k(d).getLoad();
                        if (demandLoad >= minLoad_2){
                            for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                                if( (getArcLabel(a, d) == linkLabel) && (getArcSlice(a, d) >= s)  && (getArcSlice(a, d) <= s + minLoad_2 - 1) ){
                                    double incrementValue = multiplier;
                                    incArcCost(a, d, incrementValue);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

/* Updates the assignment of a demand based on the a given path. */
void Subgradient::updateAssignment_k(int d, Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > &path, const ListDigraph::Node &SOURCE, const ListDigraph::Node &TARGET){
    std::fill(assignmentMatrix[d].begin(), assignmentMatrix[d].end(), false);
    ListDigraph::Node currentNode = TARGET;
    while (currentNode != SOURCE){
        ListDigraph::Arc arc = path.predArc(currentNode);
        int index = getArcIndex(arc, d);
        assignmentMatrix[d][index] = true;
        currentNode = path.predNode(currentNode);
    }
}

/* Checks if all length slacks are non-negative. */
bool Subgradient::checkLengthFeasibility(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        if (lengthSlack[d] < -DBL_EPSILON){
            return false;
        }
    }
    return true;
}
/* Checks if all overlap1 slacks are non-negative. */
bool Subgradient::checkOverlap_1Feasibility(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (int w = 0; w < getNbLoadsToBeRouted(); w++){
            for (int e = 0; e < instance.getNbEdges(); e++){
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    if (overlapSlack_1[d][w][e][s] < -DBL_EPSILON){
                        return false;
                    }
                }
            }
        }
    }
    return true;
}
/* Checks if all overlap2 slacks are non-negative. */
bool Subgradient::checkOverlap_2Feasibility(){
    for (int w1 = 0; w1 < getNbLoadsToBeRouted(); w1++){
        for (int w2 = 0; w2 < getNbLoadsToBeRouted(); w2++){
            for (int e = 0; e < instance.getNbEdges(); e++){
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    if (overlapSlack_1[w1][w2][e][s] < -DBL_EPSILON){
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

/* Checks if all slacks are non-negative. */
bool Subgradient::checkFeasibility(){
    if (checkLengthFeasibility() == false){
        return false;
    }

    if (checkOverlap_1Feasibility() == false){
        return false;
    }

    if (checkOverlap_2Feasibility() == false){
        return false;
    }
    return true;
    
    /*
    for (int d1 = 0; d1 < getNbDemandsToBeRouted(); d1++){
        for (int d2 = 0; d2 < getNbDemandsToBeRouted(); d2++){
            if (d1 != d2){
                for (int e = 0; e < instance.getNbEdges(); e++){
                    for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                        if (overlapSlack[d1][d2][e][s] < -DBL_EPSILON){
                            return false;
                        }
                    }
                }
            }
        }
    }
    */
}

double Subgradient::getRealCostFromPath(int d, Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > &path, const ListDigraph::Node &SOURCE, const ListDigraph::Node &TARGET){
    double total = 0.0;
    ListDigraph::Node currentNode = TARGET;
    while (currentNode != SOURCE){
        ListDigraph::Arc arc = path.predArc(currentNode);
        total += getCoeff(arc, d);
        currentNode = path.predNode(currentNode);
    }
    return total;
}

void Subgradient::runIteration(){
    incIteration();
    setCurrentLagrCost(0.0);
    setCurrentRealCost(0.0);
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        const ListDigraph::Node SOURCE = getFirstNodeFromLabel(d, getToBeRouted_k(d).getSource());
        const ListDigraph::Node TARGET = getFirstNodeFromLabel(d, getToBeRouted_k(d).getTarget());
        // run a first shortest path not taking into account length (i.e., u=0)
        Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > shortestPath((*vecGraph[d]), (*cost[d]));
        shortestPath.run(SOURCE, TARGET);
        //displayPath(shortestPath, s, t);

        if (shortestPath.reached(TARGET) == false){
            //setIsFeasible(false);
            //setIsUnfeasible(true);
            setStatus(STATUS_INFEASIBLE);
            std::cout << "> RSA is infeasible because there is no path from " << getToBeRouted_k(d).getSource()+1 << " to " << getToBeRouted_k(d).getTarget()+1 << " required for routing demand " << getToBeRouted_k(d).getId()+1 << "." << std::endl;
            return;
        }

        updateAssignment_k(d, shortestPath, SOURCE, TARGET);
        incCurrentLagrCost(shortestPath.dist(TARGET));
        incCurrentRealCost(getRealCostFromPath(d, shortestPath, SOURCE, TARGET));
        const int PATH_LENGTH = getPathLength(d, shortestPath, SOURCE, TARGET);
        updateLengthSlack(d, PATH_LENGTH);
    }
    subtractConstantValuesFromLagrCost();
    //updateOverlapSlack();
    updateOverlapSlack_1();
    updateOverlapSlack_2();

    updateLB(getLagrCurrentCost());
    if (checkFeasibility() == true){
        //setIsFeasible(true);
        setStatus(STATUS_FEASIBLE);
        //setIsUnfeasible(false);
        double feasibleSolutionCost = getRealCurrentCost();
        if (feasibleSolutionCost < getUB()){
            updateOnPath();
            updateUB(feasibleSolutionCost);
        }
    }
}

void Subgradient::subtractConstantValuesFromLagrCost(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        double val = - getLengthMultiplier_k(d)*getToBeRouted_k(d).getMaxLength();
        incCurrentLagrCost(val);
    }
    
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (int w = 0; w < getNbLoadsToBeRouted(); w++){
            for (int e = 0; e < instance.getNbEdges(); e++){
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    double val = - getOverlapMultiplier1_k(d, w, e, s);
                    incCurrentLagrCost(val);
                }
            }
        }
    }
    for (int w1 = 0; w1 < getNbLoadsToBeRouted(); w1++){
        for (int w2 = 0; w2 < getNbLoadsToBeRouted(); w2++){
            for (int e = 0; e < instance.getNbEdges(); e++){
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    double val = - getOverlapMultiplier2_k(w1, w2, e, s);
                    incCurrentLagrCost(val);
                }
            }
        }
    }
}
/* Updates the known lower bound. */
void Subgradient::updateLB(double bound){
    if (bound >= getLB() + __DBL_EPSILON__){
        setLB(bound);
        setItWithoutImprovement(0);
        //std::cout << "> Set LB: " << bound << std:: endl;
    }
    else{
        incItWithoutImprovement();
        //std::cout << "> Nb iterations without improvement: " << getItWithoutImprovement() << std:: endl;
    }
    
}

/* Updates the known upper bound. */
void Subgradient::updateUB(double bound){
    if (bound < getUB()){
        setUB(bound);
        //std::cout << "> Set UB: " << bound << std:: endl;
    }
}

/* Updates lagrangian multiplier with the rule: u[k+1] = u[k] + t[k]*violation */
void Subgradient::updateMultiplier(){
    //length
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        double violation = -getLengthSlack_k(d);
        double new_multipliplier = getLengthMultiplier_k(d) + (getStepSize()*violation);
        setLengthMultiplier_k(d, std::max(new_multipliplier, 0.0));
    }
    /*
    for (int d1 = 0; d1 < getNbDemandsToBeRouted(); d1++){
        for (int d2 = 0; d2 < getNbDemandsToBeRouted(); d2++){
            for (int e = 0; e < instance.getNbEdges(); e++){
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    double violation = -getOverlapSlack_k(d1, d2, e, s);
                    double new_multipliplier = getOverlapMultiplier_k(d1, d2, e, s) + (getStepSize()*violation);
                    setOverlapMultiplier_k(d1, d2, e, s, std::max(new_multipliplier, 0.0));
                }
            }
        }
    }
    */
    //overlap 1
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (int w = 0; w < getNbLoadsToBeRouted(); w++){
            for (int e = 0; e < instance.getNbEdges(); e++){
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    double violation = -getOverlap_1Slack_k(d, w, e, s);
                    double new_multipliplier = getOverlapMultiplier1_k(d, w, e, s) + (getStepSize()*violation);
                    setOverlapMultiplier1_k(d, w, e, s, std::max(new_multipliplier, 0.0));
                }
            }
        }
    }
    //overlap 2
    for (int w1 = 0; w1 < getNbLoadsToBeRouted(); w1++){
        for (int w2 = 0; w2 < getNbLoadsToBeRouted(); w2++){
            for (int e = 0; e < instance.getNbEdges(); e++){
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    double violation = -getOverlap_2Slack_k(w1, w2, e, s);
                    double new_multipliplier = getOverlapMultiplier2_k(w1, w2, e, s) + (getStepSize()*violation);
                    setOverlapMultiplier2_k(w1, w2, e, s, std::max(new_multipliplier, 0.0));
                }
            }
        }
    }
    //displayMultiplier();
}

/* Updates the step size with the rule: lambda*(UB - Z[u])/|slack| */
void Subgradient::updateStepSize(){
    double numerator = getLambda()*(getUB() - getLagrCurrentCost());
    double denominator = 0.0;
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        denominator += std::pow(getLengthSlack_k(d), 2);
    }
    for (int d= 0; d < getNbDemandsToBeRouted(); d++){
        for (int w = 0; w < getNbLoadsToBeRouted(); w++){
            for (int e = 0; e < instance.getNbEdges(); e++){
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    denominator += std::pow(getOverlap_1Slack_k(d, w, e, s), 2);
                }
            }
        }
    }
    for (int w1= 0; w1 < getNbLoadsToBeRouted(); w1++){
        for (int w2 = 0; w2 < getNbLoadsToBeRouted(); w2++){
            for (int e = 0; e < instance.getNbEdges(); e++){
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    denominator += std::pow(getOverlap_2Slack_k(w1, w2, e, s), 2);
                }
            }
        }
    }
    double new_stepSize = (numerator/denominator); 
    setStepSize(new_stepSize);
    //displayStepSize();
}

/* Updates the lambda used in the update of step size. Lambda is halved if LB has failed to increade in some fixed number of iterations */
void Subgradient::updateLambda(){
    double last_lambda = getLambda();
    if (getItWithoutImprovement() >= MAX_NB_IT_WITHOUT_IMPROVEMENT){
        setItWithoutImprovement(0);
        setLambda(last_lambda / 2.0);
    }
    //displayLambda();
}


/* Updates the slack of length constraint for a given path length */
void Subgradient::updateLengthSlack(int d, double pathLength){
    const int MAX_LENGTH = getToBeRouted_k(d).getMaxLength();
    lengthSlack[d] = (MAX_LENGTH - pathLength);
    //displaySlack();
}


/* Updates the slack of non-overlap constraints. */
void Subgradient::updateOverlapSlack(){
    for (int d1 = 0; d1 < getNbDemandsToBeRouted(); d1++){
        for (int d2 = 0; d2 < getNbDemandsToBeRouted(); d2++){
            for (int e = 0; e < instance.getNbEdges(); e++){
                int linkLabel = instance.getPhysicalLinkFromIndex(e).getId();
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    double expr = 0.0;
                    for (ListDigraph::ArcIt a(*vecGraph[d1]); a != INVALID; ++a){
                        if( (getArcLabel(a, d1) == linkLabel) && (getArcSlice(a, d1) == s) ){
                            int id = getArcIndex(a, d1);
                            expr += assignmentMatrix[d1][id];
                        }
                    }
                    for (ListDigraph::ArcIt a(*vecGraph[d2]); a != INVALID; ++a){
                        if( (getArcLabel(a, d2) == linkLabel) && (getArcSlice(a, d2) >= s - getToBeRouted_k(d1).getLoad() + 1) && (getArcSlice(a, d2) <= s + getToBeRouted_k(d2).getLoad() - 1) ){
                            int id = getArcIndex(a, d2);
                            expr += assignmentMatrix[d2][id];
                        }
                    }
                    overlapSlack[d1][d2][e][s] = 1 - expr;
                }
            }
        }
    }
}

/* Updates the slack of the 1st set of improved non-overlap constraints. */
void Subgradient::updateOverlapSlack_1(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (int w = 0; w < getNbLoadsToBeRouted(); w++){
            int min_load = getLoadsToBeRouted_k(w);
            for (int e = 0; e < instance.getNbEdges(); e++){
                int linkLabel = instance.getPhysicalLinkFromIndex(e).getId();
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    int expr = 0;
                    for (int d1 = 0; d1 < getNbDemandsToBeRouted(); d1++){
                        if ((d1 != d) && (getToBeRouted_k(d1).getLoad() >= min_load)){
                            for (ListDigraph::ArcIt a(*vecGraph[d1]); a != INVALID; ++a){
                                if( (getArcLabel(a, d1) == linkLabel) && (getArcSlice(a, d1) == s) ){
                                    int id = getArcIndex(a, d1);
                                    if(assignmentMatrix[d1][id] == true){
                                        expr++;
                                    }
                                }
                            }
                        }
                    }
                    for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                        if( (getArcLabel(a, d) == linkLabel) && (getArcSlice(a, d) >= s - min_load + 1) && (getArcSlice(a, d) <= s + getToBeRouted_k(d).getLoad() - 1) ){
                            int id = getArcIndex(a, d);
                            if(assignmentMatrix[d][id] == true){
                                expr++;
                            }
                        }
                    }
                    overlapSlack_1[d][w][e][s] = 1 - expr;
                }
            }
        }
    }
}

/* Updates the slack of the 2nd set of improved non-overlap constraints. */
void Subgradient::updateOverlapSlack_2(){
    for (int w1 = 0; w1 < getNbLoadsToBeRouted(); w1++){
        int min_load1 = getLoadsToBeRouted_k(w1);
        for (int w2 = 0; w2 < getNbLoadsToBeRouted(); w2++){
            int min_load2 = getLoadsToBeRouted_k(w2);
            for (int e = 0; e < instance.getNbEdges(); e++){
                int linkLabel = instance.getPhysicalLinkFromIndex(e).getId();
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    int expr = 0;
                    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
                        int demandLoad = getToBeRouted_k(d).getLoad();
                        if ( (demandLoad >= min_load1) && (demandLoad <= min_load2 - 1) ){
                            for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                                if( (getArcLabel(a, d) == linkLabel) && (getArcSlice(a, d) >= s)  && (getArcSlice(a, d) <= s + min_load1 - 1) ){
                                    int id = getArcIndex(a, d);
                                    if(assignmentMatrix[d][id] == true){
                                        expr++;
                                    }
                                }
                            }
                        }
                    }
                    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
                        int demandLoad = getToBeRouted_k(d).getLoad();
                        if (demandLoad >= min_load2){
                            for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                                if( (getArcLabel(a, d) == linkLabel) && (getArcSlice(a, d) >= s)  && (getArcSlice(a, d) <= s + min_load2 - 1) ){
                                    int id = getArcIndex(a, d);
                                    if(assignmentMatrix[d][id] == true){
                                        expr++;
                                    }
                                }
                            }
                        }
                    }
                    overlapSlack_2[w1][w2][e][s] = 1 - expr;
                }
            }
        }
    }
}

/* Assigns length as the main cost of the arcs. */
void Subgradient::setLengthCost(){
    for(int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
            (*cost[d])[a] = getArcLength(a, d) ;
        }
    }
}

/* Verifies if optimality condition has been achieved and update STOP flag. */
void Subgradient::updateStop(bool &STOP){
    if(getLB() >= getUB() - DBL_EPSILON){
        setIsOptimal(true);
        STOP = true;
    }
}

/* Tests if CSP is feasible by searching for a shortest path with arc costs based on their physical length. */
/*
bool Subgradient::testFeasibility(const ListDigraph::Node &s, const ListDigraph::Node &t){
    const int MAX_LENGTH = getToBeRouted()[0].getMaxLength();
    setLengthCost();
    Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > shortestLengthPath((*vecGraph[0]), cost);
    shortestLengthPath.run(s,t);
    //displayPath(shortestLengthPath, s, t);
    double smallestLength = getPathLength(shortestLengthPath, s, t);
    if( smallestLength >= MAX_LENGTH + DBL_EPSILON ){
        std::cout << "> CSP is unfeasiable." << std:: endl;
        return false;
    }
    else{
        updateUB(getPathCost(shortestLengthPath, s, t));
        std::cout << "> Set UB: " << getUB() << std:: endl;
    }
    return true;
}
*/
/* Stores the path found in the arcMap onPath. */
void Subgradient::updateOnPath(){
    for(int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
            int index = getArcIndex(a, d);
            if (assignmentMatrix[d][index] == true){
                (*vecOnPath[d])[a] = getToBeRouted_k(d).getId();
            }
            else{
                (*vecOnPath[d])[a] = -1;
            }
        }
    }
}


/* Returns the physical length of the path. */
double Subgradient::getPathLength(int d, Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > &path, const ListDigraph::Node &s, const ListDigraph::Node &t){
    double pathLength = 0.0;
    ListDigraph::Node n = t;
    while (n != s){
        ListDigraph::Arc arc = path.predArc(n);
        n = path.predNode(n);
        pathLength += getArcLength(arc, d);
    }
    return pathLength;
}

/* Returns the actual cost of the path according to the metric used. */
double Subgradient::getPathCost(int d, Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > &path, const ListDigraph::Node &s, const ListDigraph::Node &t){
    double pathCost = 0.0;
    ListDigraph::Node n = t;
    while (n != s){
        ListDigraph::Arc arc = path.predArc(n);
        n = path.predNode(n);
        pathCost += getCoeff(arc, d);
    }
    return pathCost;
}
/*
void Subgradient::displayPath(Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > &path, const ListDigraph::Node &s, const ListDigraph::Node &t){
    std::cout << "The path found is:" << std::endl;
    ListDigraph::Node n = t;
    while (n != s){
        ListDigraph::Arc arc = path.predArc(n);
        n = path.predNode(n);
        displayArc(0, arc);
    }
    double pathLength = getPathLength(path, s, t);
    std::cout << "Path length: " << pathLength << std::endl;
    double pathCost = getPathCost(path, s, t);
    std::cout << "Path cost: " << pathCost << std::endl;
    double lagrangianObjFunc = path.dist(t);
    std::cout << "Lagrangian Objective Function: " << lagrangianObjFunc << std::endl;
}
*/
std::string Subgradient::getPathString(int d, Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > &path, const ListDigraph::Node &s, const ListDigraph::Node &t){
    std::string pathString = "";
    ListDigraph::Node n = t;
    while (n != s){
        pathString += "(" + std::to_string(getNodeLabel(n, d) + 1) + "," + std::to_string(getNodeSlice(n, d) + 1) + ")";
        pathString += "-";
        n = path.predNode(n);
    }
    pathString += "(" + std::to_string(getNodeLabel(s, d) + 1) + "," + std::to_string(getNodeSlice(s, d) + 1) + ")";
    return pathString;
}
 

void Subgradient::displayMainParameters(){
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
    sizeOfField[9] = 12;
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
        field[8] = "Length Feas";
        field[9] = "Overlap Feas";
        
        for (unsigned int i = 0; i < field.size(); i++){
            field[i].resize(sizeOfField[i], space);
            std::cout << field[i] << " | ";
        }
        std::cout << std::endl;
    }
    field[0] = std::to_string(k);
    field[1] = std::to_string(getItWithoutImprovement());
    field[2] = std::to_string(getLB());
    field[3] = std::to_string(getLagrCurrentCost());
    field[4] = std::to_string(getRealCurrentCost());
    field[5] = std::to_string(getUB());
    field[6] = std::to_string(getStepSize());
    field[7] = std::to_string(getLambda());
    if (checkLengthFeasibility()){
        field[8] = "YES";
    }
    else{
        field[8] = "NO";
    }
    if (checkOverlap_1Feasibility() && checkOverlap_2Feasibility()){
        field[9] = "YES";
    }
    else{
        field[9] = "NO";
    }


    for (unsigned int i = 0; i < field.size(); i++){
        field[i].resize(sizeOfField[i], space);
        std::cout << field[i] << " | ";
    }
    std::cout << std::endl;
   // displayMultiplier();
}

void Subgradient::displayMultiplier(){
    std::string display = "Length Multiplier = [ ";
    for (unsigned int i = 0; i < lagrangianMultiplierLength.size(); i++){
        display += std::to_string(getLengthMultiplier_k(i)) + " "; 
    }
    display += "]";
    std::cout << display << std::endl;

    display = "Overlap1 Multiplier = [ ";
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (int w = 0; w < getNbLoadsToBeRouted(); w++){
            for (int e = 0; e < instance.getNbEdges(); e++){
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    if (getOverlapMultiplier1_k(d, w, e, s) != 0.0)
                        display += std::to_string(getOverlapMultiplier1_k(d, w, e, s)) + " "; 
                }
            }
        }
    }
    display += "]";
    std::cout << display << std::endl;
    //overlap 2
    
    display = "Overlap2 Multiplier = [ ";
    for (int w1 = 0; w1 < getNbLoadsToBeRouted(); w1++){
        for (int w2 = 0; w2 < getNbLoadsToBeRouted(); w2++){
            for (int e = 0; e < instance.getNbEdges(); e++){
                for (int s = 0; s < instance.getPhysicalLinkFromIndex(e).getNbSlices(); s++){
                    if (getOverlapMultiplier2_k(w1, w2, e, s) != 0.0)
                        display += std::to_string(getOverlapMultiplier2_k(w1, w2, e, s)) + " "; 
                }
            }
        }
    }
    display += "]";
    std::cout << display << std::endl;
}/*
void Subgradient::displayLambda(){
    std::string display = "Lambda = [ ";
    for (unsigned int i = 0; i < lambda.size(); i++){
        display += std::to_string(getLambda_k(i)) + " "; 
    }
    display += "]";
    std::cout << display << std::endl;
}
void Subgradient::displaySlack(){
    std::string display = "Slack = [ ";
    for (unsigned int i = 0; i < slack.size(); i++){
        display += std::to_string(getSlack_k(i)) + " "; 
    }
    display += "]";
    std::cout << display << std::endl;
}
void Subgradient::displayStepSize(){
    std::string display = "StepSize = [ ";
    for (unsigned int i = 0; i < stepSize.size(); i++){
        display += std::to_string(getStepSize_k(i)) + " "; 
    }
    display += "]";
    std::cout << display << std::endl;
}*/