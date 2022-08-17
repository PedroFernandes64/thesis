#include "lagNewNonOverlapping.h"


void lagNewNonOverlapping::getDualSolution(double *rowprice){
    std::cout<< "Branch and bound for no overlapping not defined yet.\n";
}

void lagNewNonOverlapping::clearSlacks(){
    lengthDirection.clear();
    lengthSlack.clear();
    lengthSlack_v2.clear();

    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        sourceTargetSlack[d].clear();
        sourceTargetSlack_v2[d].clear();
        sourceTargetDirection[d].clear();

        flowSlack[d].clear();
        flowSlack_v2[d].clear();
        flowDirection[d].clear();
    }
    sourceTargetSlack.clear();
    sourceTargetSlack_v2.clear();
    sourceTargetDirection.clear();

    flowSlack.clear();
    flowSlack_v2.clear();
    flowDirection.clear();

    for (int e = 0; e < instance.getNbEdges(); e++){
        oneSlicePerDemandDirection[e].clear();
        oneSlicePerDemandSlack[e].clear();
        oneSlicePerDemandSlack_v2[e].clear();
    }

    oneSlicePerDemandDirection.clear();
    oneSlicePerDemandSlack.clear();
    oneSlicePerDemandSlack_v2.clear();

    if(instance.getInput().isObj8(0)){
        maxUsedSliceOverallSlack.clear();
        maxUsedSliceOverallSlack_v2.clear();
        maxUsedSliceOverallDirection.clear();
    }
}

/* *******************************************************************************************************************
*                                              INITIALIZATION METHODS
******************************************************************************************************************** */

void lagNewNonOverlapping::init(bool initMult){

    /** Lagrangian Values **/
    if(initMult){
        initMultipliers();
    }
    initSlacks();
    initDirection();
    initCoeff();
    time.setStart(ClockTime::getTimeNow());
    build_Graphs_e();
    setConstAuxGraphTime(time.getTimeInSecFromStart());
    initAssignmentMatrix();

    /** Time **/
    setUpdateVariablesTime(0.0);
    setShorstestPathTime(0.0);
    setSubstractMultipliersTime(0.0);
    setCostTime(0.0);
}

/*************************************************** MULTIPLIERS ******************************************************/

/************************************************* Initial multiplier ************************************************/

void lagNewNonOverlapping::startMultipliers(double *row ,int size,int objSignal){
    std::cout << "Non overlapping formulation with Branch and Bound not defined yet.\n";
}

/* Sets the initial lagrangian multipliers for the subgradient to run. */
void lagNewNonOverlapping::initMultipliers(){
    bool warmstart = getInstance().getInput().getWarmstart();
    if(warmstart){
        /**** Initializing all with 0.o and then solving the warmstart procedure *****/
        initializeLengthMultipliers(0.0);
        initializeSourceTargetMultipliers(0.0);
        initializeFlowMultipliers(0.0);

        initializeOneSlicePerDemandMultipliers(0.0);

        if(instance.getInput().isObj8(0)){
            initializeMaxUsedSliceOverallMultipliers(0.0);
            //initializeMaxUsedSliceOverallAuxMultipliers(0.0);
            //initializeMaxUsedSliceOverall2Multipliers(0.0);
            //initializeMaxUsedSliceOverall3Multipliers(0.0);
        }
        initMultipliersWarmstart();
    }else{
        /**** Initializing all with a given initial multiplier *****/
        double initialMultiplier = instance.getInput().getInitialLagrangianMultiplier();
        initializeLengthMultipliers(initialMultiplier);
        initializeSourceTargetMultipliers(initialMultiplier);
        initializeFlowMultipliers(initialMultiplier);

        initializeOneSlicePerDemandMultipliers(initialMultiplier);

        if(instance.getInput().isObj8(0)){
            initializeMaxUsedSliceOverallMultipliers(initialMultiplier);
            //initializeMaxUsedSliceOverallAuxMultipliers(initialMultiplier);
            //initializeMaxUsedSliceOverall2Multipliers(initialMultiplier);
            //initializeMaxUsedSliceOverall3Multipliers(initialMultiplier);
        }
    }
    std::cout << "> Initial Lagrangian multipliers were defined. " << std::endl;
}

/*********************************************** Warmstart  procedure  **************************************************/

/* Sets the initial flow lagrangian multipliers with a warmstart procedure */
void lagNewNonOverlapping::initMultipliersWarmstart(){
    
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){

        /* Creating maps for the COST and CAPACITY  */
        ListDigraph::ArcMap<double> COST(*vecGraph[d]);
        ListDigraph::ArcMap<int> CAPACITY(*vecGraph[d],1);
        ListDigraph::NodeMap<int> SUPPLY(*vecGraph[d],0);

        for(ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a)
            COST[a] = getCoeff(a,d);

        /* Finding the source and destination */
        const ListDigraph::Node SOURCE_ORIGINAL = getFirstNodeFromLabel(d, getToBeRouted_k(d).getSource());
        const ListDigraph::Node TARGET_ORIGINAL = getFirstNodeFromLabel(d, getToBeRouted_k(d).getTarget());

        SUPPLY[SOURCE_ORIGINAL] = 1;
        SUPPLY[TARGET_ORIGINAL] = -1;
        
        /* Running the CapacityScaling problem */
        CapacityScaling<ListDigraph,int,double> capScale(*vecGraph[d]);
        capScale.upperMap(CAPACITY);
        capScale.costMap(COST);
        capScale.supplyMap(SUPPLY);
        capScale.run();

        /* Finding the node potentials */
        for(ListDigraph::NodeIt v(*vecGraph[d]); v != INVALID; ++v){
            int label = getNodeLabel(v,d);
            int index = getNodeIndex(v,d);
            if(label == getToBeRouted_k(d).getSource() || label == getToBeRouted_k(d).getTarget()){
                lagrangianMultiplierSourceTarget[d][label] = capScale.potential(v);
            }else{
                lagrangianMultiplierFlow[d][index] = capScale.potential(v);
            }
        }
    }    
}

/************************************************ STABILITY CENTER ****************************************************/

void lagNewNonOverlapping::initStabilityCenter(){
    initializeLengthSC();
    initializeSourceTargetSC();
    initializeFlowSC();

    initializeOneSlicePerDemandSC();

    if(instance.getInput().isObj8(0)){
        initializeMaxUsedSliceOverallSC();
        //initializeMaxUsedSliceOverallAuxSC();
        //initializeMaxUsedSliceOverall2SC();
        //initializeMaxUsedSliceOverall3SC();
    }
}

/**************************************************** SLACKS **********************************************************/

/* Initializes the slack of relaxed constraints. */
void lagNewNonOverlapping::initSlacks(){
    initializeLengthSlacks();
    initializeSourceTargetSlacks();
    initializeFlowSlacks();

    initializeOneSlicePerDemandSlacks();

    if(instance.getInput().isObj8(0)){
        initializeMaxUsedSliceOverallSlacks();
        //initializeMaxUsedSliceOverallAuxSlacks();
        //initializeMaxUsedSliceOverall2Slacks();
        //initializeMaxUsedSliceOverall3Slacks();
    }   
    std::cout << "> Initial Slacks were defined. " << std::endl;
}

void lagNewNonOverlapping::resetSlacks(){
    resetLengthSlacks();
    resetSourceTargetSlacks();
    resetFlowSlacks();

    resetOneSlicePerDemandSlacks();

    if(instance.getInput().isObj8(0)){
        resetMaxUsedSliceOverallSlacks();
        //resetMaxUsedSliceOverallAuxSlacks();
        //resetMaxUsedSliceOverall2Slacks();
        //resetMaxUsedSliceOverall3Slacks();
    }   
}

/********************************************* SLACKS PRIMAL APPROXIMATION *************************************************/

/* Initializes the slack of relaxed constraints. */
void lagNewNonOverlapping::initPrimalSlacks(){
    initializeLengthPrimalSlacks();
    initializeSourceTargetPrimalSlacks();
    initializeFlowPrimalSlacks();

    initializeOneSlicePerDemandPrimalSlacks();

    if(instance.getInput().isObj8(0)){
        initializeMaxUsedSliceOverallPrimalSlacks();
        //initializeMaxUsedSliceOverallAuxPrimalSlacks();
        //initializeMaxUsedSliceOverall2Slacks();
        //initializeMaxUsedSliceOverall3Slacks();
    }   
    std::cout << "> Initial Slacks were defined. " << std::endl;
}

/*************************************************** DIRECTION ********************************************************/

void lagNewNonOverlapping::initDirection(){
    initializeLengthDirection();
    initializeSourceTargetDirection();
    initializeFlowDirection();

    initializeOneSlicePerDemandDirection();

    if(instance.getInput().isObj8(0)){
        initializeMaxUsedSliceOverallDirection();
        //initializeMaxUsedSliceOverallAuxDirection();
        //initializeMaxUsedSliceOverall2Direction();
        //initializeMaxUsedSliceOverall3Direction();
    }
    std::cout << "> Initial Direction were defined. " << std::endl;
}

/********************************************* BUILD AUXILIARY GRAPH **************************************************/

void lagNewNonOverlapping::build_Graphs_e(){
    vecENode.resize(instance.getNbEdges());
    for (int e = 0; e < instance.getNbEdges(); e++){

        vecEGraph.emplace_back(std::make_shared<ListDigraph>());
        vecEArcId.emplace_back(std::make_shared<ArcMap>((*vecEGraph[e])));
        vecEArcDemand.emplace_back(std::make_shared<ArcMap>((*vecEGraph[e])));
        vecEArcSlice.emplace_back(std::make_shared<ArcMap>((*vecEGraph[e])));
        vecEArcIndexArcGraphD.emplace_back(std::make_shared<ArcMap>((*vecEGraph[e])));
        vecEArcIndex.emplace_back(std::make_shared<ArcMap>((*vecEGraph[e])));
        vecECoeff.emplace_back(std::make_shared<ArcCost>((*vecEGraph[e])));
        vecEArcLength.emplace_back(std::make_shared<ArcCost>((*vecEGraph[e])));
        vecEArcSourceLabel.emplace_back(std::make_shared<ArcMap>((*vecEGraph[e])));
        vecEArcTargetLabel.emplace_back(std::make_shared<ArcMap>((*vecEGraph[e])));
        vecEArcSourceIndex.emplace_back(std::make_shared<ArcMap>((*vecEGraph[e])));
        vecEArcTargetIndex.emplace_back(std::make_shared<ArcMap>((*vecEGraph[e])));

        cost.emplace_back(std::make_shared<ArcCost>((*vecEGraph[e])));

        int sliceLimit = getNbSlicesLimitFromEdge(e);
        for (int s = 0; s <= sliceLimit; s++){
            ListDigraph::Node node = vecEGraph[e]->addNode();
            int id = vecEGraph[e]->id(node);
            vecENode[e].emplace_back(std::make_shared<ListDigraph::Node>(node));
            if(s > 0){
                ListDigraph::Node nodeSource = (*vecENode[e][s-1]);
                ListDigraph::Node nodeTarget = (*vecENode[e][s]);
    
                ListDigraph::Arc a = vecEGraph[e]->addArc(nodeSource,nodeTarget);
                int id = vecEGraph[e]->id(a);

                setArcEId(a,e,id);
                setArcEDemand(a,e,-1);
                setArcESlice(a,e,-1);
                setArcEIndexArcD(a,e,-1);
                setArcELength(a,e,0.0);
                setArcECoeff(a,e,-0.0);
                setArcESourceLabel(a,e,-1);
                setArcETargetLabel(a,e,-1);
                setArcESourceIndex(a,e,-1);
                setArcETargetIndex(a,e,-1);
                setArcEArc(a,e,INVALID);
            }
        }
    }

    
    vecTargetLabels.resize(getNbDemandsToBeRouted());
    vecSourceLabels.resize(getNbDemandsToBeRouted());
    vecMaxLength.resize(getNbDemandsToBeRouted());
    for(int d = 0; d < getNbDemandsToBeRouted(); d++){
        vecTargetLabels[d] = getToBeRouted_k(d).getTarget();
        vecSourceLabels[d] = getToBeRouted_k(d).getSource();
        vecMaxLength[d] = getToBeRouted_k(d).getMaxLength();
        for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){ 
            int label = getArcLabel(a,d);
            int slice = getArcSlice(a,d);
            int index = getArcIndex(a,d);
            double length = getArcLength(a,d);
            double coefficient = (*coeff[d])[a];
            int source = getNodeLabel((*vecGraph[d]).source(a), d);
            int target = getNodeLabel((*vecGraph[d]).target(a), d);
            int sourceIndex = getNodeIndex((*vecGraph[d]).source(a), d);
            int targetIndex = getNodeIndex((*vecGraph[d]).target(a), d);
            addArcE(label,d,slice,index,length,coefficient,source,target,sourceIndex,targetIndex,a);
        }
    }
    
    for (int e = 0; e < instance.getNbEdges(); e++){
        int index = 0;
        for (ListDigraph::ArcIt a(*vecEGraph[e]); a != INVALID; ++a){ 
            setArcEIndex(a,e,index);
            index++;
        }
    }
}

void lagNewNonOverlapping::addArcE(int label, int demand, int slice, int index,double length, double coeff,int source, int target, int sourceIndex,int targetIndex,const ListDigraph::Arc & arc){
    int load  = getToBeRouted_k(demand).getLoad();
    int first = slice - load + 1;
    int last  = slice + 1;
    
    ListDigraph::Node nodeSource = (*vecENode[label][first]);
    ListDigraph::Node nodeTarget = (*vecENode[label][last]);
    

    ListDigraph::Arc a = vecEGraph[label]->addArc(nodeSource,nodeTarget);
    int id = vecEGraph[label]->id(a);

    setArcEId(a,label,id);
    setArcEDemand(a,label,demand);
    setArcESlice(a,label,slice);
    setArcEIndexArcD(a,label,index);
    setArcELength(a,label,length);
    setArcECoeff(a,label,coeff);
    setArcESourceLabel(a,label,source);
    setArcETargetLabel(a,label,target);
    setArcESourceIndex(a,label,sourceIndex);
    setArcETargetIndex(a,label,targetIndex);
    setArcEArc(a,label,arc);
}

/* **********************************************************************************************************************
*                                                  UPDATE METHODS
*********************************************************************************************************************** */

/*************************************************** MULTIPLIERS ********************************************************/

/* Updates lagrangian multiplier with the rule: u[k+1] = u[k] + t[k]*violation */
void lagNewNonOverlapping::updateMultiplier(double step){
    updateLengthMultiplier(step);
    updateSourceTargetMultiplier(step);
    updateFlowMultiplier(step);

    updateOneSlicePerDemandMultiplier(step);

    if(instance.getInput().isObj8(0)){
        updateMaxUsedSliceOverallMultiplier(step);
        //updateMaxUsedSliceOverallAuxMultiplier(step);
        //updateMaxUsedSliceOverall2Multiplier(step);
        //updateMaxUsedSliceOverall3Multiplier(step);
    }
}

/************************************** MULTIPLIER CONSIDERING THE STABILITY CENTER **************************************/

void lagNewNonOverlapping::updateMultiplier_v2(double step){
    updateLengthMultiplier_v2(step);
    updateSourceTargetMultiplier_v2(step);
    updateFlowMultiplier_v2(step);

    updateOneSlicePerDemandMultiplier_v2(step);

    if(instance.getInput().isObj8(0)){
        updateMaxUsedSliceOverallMultiplier_v2(step);
        //updateMaxUsedSliceOverallAuxMultiplier_v2(step);
        //updateMaxUsedSliceOverall2Multiplier_v2(step);
        //updateMaxUsedSliceOverall3Multiplier_v2(step);
    }
}

/***************************************************** STABILITY CENTER ***************************************************/

void lagNewNonOverlapping::updateStabilityCenter(){
    updateLengthSC();
    updateSourceTargetSC();
    updateFlowSC();

    updateOneSlicePerDemandSC();

    if(instance.getInput().isObj8(0)){
        updateMaxUsedSliceOverallSC();
        //updateMaxUsedSliceOverallAuxSC();
        //updateMaxUsedSliceOverall2SC();
        //updateMaxUsedSliceOverall3SC();
    }
}

/********************************************************** SLACK *********************************************************/

void lagNewNonOverlapping::updateSlack(int d, const ListDigraph::Arc & arc){
    updateLengthSlack(d,arc);
    updateSourceTargetSlack(d,arc);
    updateFlowSlack(d,arc);

    updateOneSlicePerDemandSlack(d,arc);

    if(instance.getInput().isObj8(0)){
        updateMaxUsedSliceOverallSlack(d,arc);
        //updateMaxUsedSliceOverallAuxSlack(d,arc);
        //updateMaxUsedSliceOverall2Slack(d,arc);
        //updateMaxUsedSliceOverall3Slack(d,arc);
    }
}

void lagNewNonOverlapping::updatePrimalSlack(double alpha){
    updateLengthPrimalSlack(alpha);
    updateSourceTargetPrimalSlack(alpha);
    updateFlowPrimalSlack(alpha);

    updateOneSlicePerDemandPrimalSlack(alpha);

    if(instance.getInput().isObj8(0)){
        updateMaxUsedSliceOverallPrimalSlack(alpha);
        //updateMaxUsedSliceOverallAuxPrimalSlack(alpha);
        //updateMaxUsedSliceOverall2PrimalSlack(alpha);
        //updateMaxUsedSliceOverall3PrimalSlack(alpha);
    }
}

/********************************************************** DIRECTION *****************************************************/

void lagNewNonOverlapping::updateDirection(){
    double theta = getDirectionMult();
    updateLengthDirection(theta);
    updateSourceTargetDirection(theta);
    updateFlowDirection(theta);

    updateOneSlicePerDemandDirection(theta);

    if(instance.getInput().isObj8(0)){
        updateMaxUsedSliceOverallDirection(theta);
        //updateMaxUsedSliceOverallAuxDirection(theta);
        //updateMaxUsedSliceOverall2Direction(theta);
        //updateMaxUsedSliceOverall3Direction(theta);
    }
}

/**************************************************** ASSIGNMENT MATRIX *****************************************************/

/* Updates the assignment of a edge based on the a given path. */
void lagNewNonOverlapping::updateAssignment_k(int label, BellmanFordCostE &path, const ListDigraph::Node &SOURCE, const ListDigraph::Node &TARGET){
    
    ListDigraph::Node currentNode = TARGET;
    while (currentNode != SOURCE){
        ListDigraph::Arc arc = path.predArc(currentNode);
        int index = getArcEIndexArcD(arc, label);
        int demand = getArcEDemand(arc,label);
        if(index != -1){

            /* Update Assignment */
            assignmentMatrix_d[demand][index] = true;

            /* Update Slack */
            updateSlack(demand,getArcEArc(arc,label));
            
        }
        currentNode = path.predNode(currentNode);
    }
}

/**************************************************** CHECK FEASIBILITY *****************************************************/

/* Checks if all slacks are non-negative. */
bool lagNewNonOverlapping::checkFeasibility(){
    if (checkLengthFeasibility() == false){
        return false;
    }
    if (checkSourceTargetFeasibility() == false){
        return false;
    }
    if (checkFlowFeasibility() == false){
        return false;
    }
    if(instance.getInput().isObj8(0)){
        if(checkMaxUsedSliceOverallFeasibility()){
            return false;
        }
    }
    return true;
}

/* Checks if all slacks are non-negative. */
bool lagNewNonOverlapping::checkFeasibility_v2(){
    if (checkLengthFeasibility_v2() == false){
        return false;
    }
    if (checkSourceTargetFeasibility_v2() == false){
        return false;
    }
    if (checkFlowFeasibility_v2() == false){
        return false;
    }
    if(instance.getInput().isObj8(0)){
        if(checkMaxUsedSliceOverallFeasibility_v2()){
            return false;
        }
    }
    return true;
}

bool lagNewNonOverlapping::checkSlacknessCondition(){
    if (checkLengthSlacknessCondition() == false){
        return false;
    }
    if (checkSourceTargetSlacknessCondition() == false){
        return false;
    }
    if (checkFlowSlacknessCondition() == false){
        return false;
    }
    if(instance.getInput().isObj8(0)){
        if(checkMaxUsedSliceOverallSlacknessCondition()){
            return false;
        }
    }
    return true;
}

/* ***************************************************************************************************************************
*                                                         GET METHODS
**************************************************************************************************************************** */

double lagNewNonOverlapping::getRealCostFromPath(int label, BellmanFordCostE &path, const ListDigraph::Node &SOURCE, const ListDigraph::Node &TARGET){
    double total = 0.0;
    ListDigraph::Node currentNode = TARGET;
    while (currentNode != SOURCE){
        ListDigraph::Arc arc = path.predArc(currentNode);
        total += (*vecECoeff[label])[arc];
        currentNode = path.predNode(currentNode);
    }
    return total;
}

/** Returns the constraints slack module **/
double lagNewNonOverlapping::getSlackModule(double alpha) {
    double denominator = 0.0;
    /* Length */
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        double slack = alpha*(-getLengthSlack_k(d)) + (1.0 - alpha)*(-getLengthSlack_v2_k(d));
        double mult = getLengthMultiplier_k(d);
        if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
            denominator += std::pow(getLengthSlack_k(d),2);
        }
    }

    /* Source/Target */
    for(int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (int v = 0; v < instance.getNbNodes(); v++){
            double slack = alpha*(-getSourceTargetSlack_k(d,v)) + (1.0 - alpha)*(-getSourceTargetSlack_v2_k(d,v));
            double mult = getSourceTargetMultiplier_k(d,v);
            if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
                denominator += std::pow(getSourceTargetSlack_k(d,v),2);
            }
        }
    }

    /* Flow */
    for (int d= 0; d < getNbDemandsToBeRouted(); d++){
        for (int v = 0; v < countNodes(*vecGraph[d]); v++){
            if((v != getToBeRouted_k(d).getSource()) && v != getToBeRouted_k(d).getTarget()){
                double slack = alpha*(-getFlowSlack_k(d,v)) + (1.0 - alpha)*(-getFlowSlack_v2_k(d,v));
                double mult = getFlowMultiplier_k(d,v);
                if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
                    denominator += std::pow(getFlowSlack_k(d,v),2);
                }
            }
        }   
    } 
    /* One slice Per demand */
    for (int e = 0; e < instance.getNbEdges(); e++){
        for (int d= 0; d < getNbDemandsToBeRouted(); d++){
            double slack = alpha*(-getOneSlicePerDemandSlack_k(e,d)) + (1.0 - alpha)*(-getOneSlicePerDemandSlack_v2_k(e,d));
            double mult = getOneSlicePerDemandMultiplier_k(e,d);
            if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
                denominator += std::pow(getOneSlicePerDemandSlack_k(e,d),2);
            }
        }
    }
    if(instance.getInput().isObj8(0)){
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            double slack = alpha*(-getMaxUsedSliceOverallSlack_k(d)) + (1.0 - alpha)*(-getMaxUsedSliceOverallSlack_v2_k(d));
            double mult = getMaxUsedSliceOverallMultiplier_k(d);
            if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
                denominator += std::pow(getMaxUsedSliceOverallSlack_k(d),2);
            }       
        }
        /*
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            double slack = alpha*(-getMaxUsedSliceOverallAuxSlack_k(d)) + (1.0 - alpha)*(-getMaxUsedSliceOverallAuxSlack_v2_k(d));
            double mult = getMaxUsedSliceOverallAuxMultiplier_k(d);
            if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
                denominator += std::pow(getMaxUsedSliceOverallAuxSlack_k(d),2);
            }       
        }
        */
        /*
        for (int e = 0; e < instance.getNbEdges(); e++){
            for (int s = 0; s < getNbSlicesLimitFromEdge(e); s++){
                double slack = alpha*(-getMaxUsedSliceOverall2Slack_k(e,s)) + (1.0 - alpha)*(-getMaxUsedSliceOverall2Slack_v2_k(e,s));
                double mult = getMaxUsedSliceOverall2Multiplier_k(e,s);
                if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
                    denominator += std::pow(getMaxUsedSliceOverall2Slack_k(e,s),2);
                }    
            }
        }
        */
        /*
        for (int v = 0; v < instance.getNbNodes(); v++){
            for (int s = 0; s < getNbSlicesGlobalLimit(); s++){
                double slack = alpha*(-getMaxUsedSliceOverall3Slack_k(v,s)) + (1.0 - alpha)*(-getMaxUsedSliceOverall3Slack_v2_k(v,s));
                double mult = getMaxUsedSliceOverall3Multiplier_k(v,s);
                if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
                    denominator += std::pow(getMaxUsedSliceOverall3Slack_k(v,s),2);
                }     
            }
        }
        */
    }
    return denominator;
}

/** Returns the constraints slack module (slack considering the primal variables) **/
double lagNewNonOverlapping::getSlackModule_v2(double alpha){
    double denominator = 0.0;
    /* Length */
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        double slack = alpha*(-getLengthSlack_k(d)) + (1.0 - alpha)*(-getLengthSlack_v2_k(d));
        double mult = getLengthMultiplier_k(d);
        if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
            denominator += std::pow(getLengthSlack_v2_k(d),2);
        }
    }

    /* Source/Target */
    for(int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (int v = 0; v < instance.getNbNodes(); v++){
            double slack = alpha*(-getSourceTargetSlack_k(d,v)) + (1.0 - alpha)*(-getSourceTargetSlack_v2_k(d,v));
            double mult = getSourceTargetMultiplier_k(d,v);
            if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
                denominator += std::pow(getSourceTargetSlack_v2_k(d,v),2);
            }
        }
    }

    /* Flow */
    for (int d= 0; d < getNbDemandsToBeRouted(); d++){
        for (int v = 0; v < countNodes(*vecGraph[d]); v++){
            if((v != getToBeRouted_k(d).getSource()) && v != getToBeRouted_k(d).getTarget()){
                double slack = alpha*(-getFlowSlack_k(d,v)) + (1.0 - alpha)*(-getFlowSlack_v2_k(d,v));
                double mult = getFlowMultiplier_k(d,v);
                if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
                    denominator += std::pow(getFlowSlack_v2_k(d,v),2);
                }
            }
        }   
    } 
    /* One slice Per demand */
    for (int e = 0; e < instance.getNbEdges(); e++){
        for (int d= 0; d < getNbDemandsToBeRouted(); d++){
            double slack = alpha*(-getOneSlicePerDemandSlack_k(e,d)) + (1.0 - alpha)*(-getOneSlicePerDemandSlack_v2_k(e,d));
            double mult = getOneSlicePerDemandMultiplier_k(e,d);
            if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
                denominator += std::pow(getOneSlicePerDemandSlack_v2_k(e,d),2);
            }
        }
    }
    if(instance.getInput().isObj8(0)){
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            double slack = alpha*(-getMaxUsedSliceOverallSlack_k(d)) + (1.0 - alpha)*(-getMaxUsedSliceOverallSlack_v2_k(d));
            double mult = getMaxUsedSliceOverallMultiplier_k(d);
            if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
                denominator += std::pow(getMaxUsedSliceOverallSlack_v2_k(d),2);
            }       
        }
        /*
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            double slack = alpha*(-getMaxUsedSliceOverallAuxSlack_k(d)) + (1.0 - alpha)*(-getMaxUsedSliceOverallAuxSlack_v2_k(d));
            double mult = getMaxUsedSliceOverallAuxMultiplier_k(d);
            if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
                denominator += std::pow(getMaxUsedSliceOverallAuxSlack_v2_k(d),2);
            }       
        }
        */
        /*
        for (int e = 0; e < instance.getNbEdges(); e++){
            for (int s = 0; s < getNbSlicesLimitFromEdge(e); s++){
                double slack = alpha*(-getMaxUsedSliceOverall2Slack_k(e,s)) + (1.0 - alpha)*(-getMaxUsedSliceOverall2Slack_v2_k(e,s));
                double mult = getMaxUsedSliceOverall2Multiplier_k(e,s);
                if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
                    denominator += std::pow(getMaxUsedSliceOverall2Slack_v2_k(e,s),2);
                }    
            }
        }
        */
        /*
        for (int v = 0; v < instance.getNbNodes(); v++){
            for (int s = 0; s < getNbSlicesGlobalLimit(); s++){
                double slack = alpha*(-getMaxUsedSliceOverall3Slack_k(v,s)) + (1.0 - alpha)*(-getMaxUsedSliceOverall3Slack_v2_k(v,s));
                double mult = getMaxUsedSliceOverall3Multiplier_k(v,s);
                if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
                    denominator += std::pow(getMaxUsedSliceOverall3Slack_v2_k(v,s),2);
                }     
            }
        }
        */
    }
    return denominator;
}

/** Returns the constraints direction module **/
double lagNewNonOverlapping::getDirectionModule(){
    double denominator = 0.0;
    /* Length */
    denominator += std::accumulate(lengthDirection.begin(),lengthDirection.end(),0.0,[](double sum, double direction){return sum +=std::pow(direction,2);});
    
    /* Source/Target */
    for(int d = 0; d < getNbDemandsToBeRouted(); d++){
        denominator += std::accumulate(sourceTargetDirection[d].begin(),sourceTargetDirection[d].end(),0.0,[](double sum, double direction){return sum +=std::pow(direction,2);});
    }

    /* Flow */
    for (int d= 0; d < getNbDemandsToBeRouted(); d++){
        denominator += std::accumulate(flowDirection[d].begin(),flowDirection[d].end(),0.0,[](double sum, double direction){return sum +=std::pow(direction,2);});
    } 

    /* One slice Per demand */
    for (int e = 0; e < instance.getNbEdges(); e++){
        denominator += std::accumulate(oneSlicePerDemandDirection[e].begin(),oneSlicePerDemandDirection[e].end(),0.0,[](double sum, double direction){return sum +=std::pow(direction,2);});
    }
    if(instance.getInput().isObj8(0)){
        denominator += std::accumulate(maxUsedSliceOverallDirection.begin(),maxUsedSliceOverallDirection.end(),0.0,[](double sum, double direction){return sum +=std::pow(direction,2);});
        //denominator += std::accumulate(maxUsedSliceOverallAuxDirection.begin(),maxUsedSliceOverallAuxDirection.end(),0.0,[](double sum, double direction){return sum +=std::pow(direction,2);});
        /*
        for (int e = 0; e < instance.getNbEdges(); e++){
            denominator += std::accumulate(maxUsedSliceOverall2Direction[e].begin(),maxUsedSliceOverall2Direction[e].end(),0.0,[](double sum, double direction){return sum +=std::pow(direction,2);});
        }
        for (int v = 0; v < instance.getNbNodes(); v++){
            denominator += std::accumulate(maxUsedSliceOverall3Direction[v].begin(),maxUsedSliceOverall3Direction[v].end(),0.0,[](double sum, double direction){return sum +=std::pow(direction,2);});
        }
        */
    }
    return denominator;
}

/** Returns the scalar product between the slack (gradient) and the direction **/
double lagNewNonOverlapping::getSlackDirectionProd(){
    Input::ProjectionType projection = getInstance().getInput().getChosenProjection();
    if((projection == Input::IMPROVED) ||(projection == Input::PROJECTED)){
        return getSlackDirectionProdProjected(projection);
    }   
    return getSlackDirectionProdNormal();  
}

double lagNewNonOverlapping::getSlackDirectionProdNormal(){
    double denominator = 0.0;
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        denominator += getLengthSlack_k(d)*getLengthDirection_k(d);
    }
    for(int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (int v = 0; v < instance.getNbNodes(); v++){
            denominator += getSourceTargetSlack_k(d, v)*getSourceTargetDirection_k(d,v);
        }
    }
    for (int d= 0; d < getNbDemandsToBeRouted(); d++){
        for (int v = 0; v < countNodes(*vecGraph[d]); v++){
            if((v != getToBeRouted_k(d).getSource()) && v != getToBeRouted_k(d).getTarget()){   
                denominator += getFlowSlack_k(d, v)*getFlowDirection_k(d,v);
            }
        }
    }

    for (int e = 0; e < instance.getNbEdges(); e++){
        for (int d= 0; d < getNbDemandsToBeRouted(); d++){
            denominator += getOneSlicePerDemandSlack_k(e,d)*getOneSlicePerDemandDirection_k(e,d);
        }
    }
    if(instance.getInput().isObj8(0)){
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            denominator += getMaxUsedSliceOverallSlack_k(d)*getMaxUsedSliceOverallDirection_k(d);
            //denominator += getMaxUsedSliceOverallAuxSlack_k(d)*getMaxUsedSliceOverallAuxDirection_k(d);
        }

        /*
        for (int e = 0; e < instance.getNbEdges(); e++){
            int sliceLimit = getNbSlicesLimitFromEdge(e);
            for (int s = 0; s < sliceLimit; s++){
                denominator += getMaxUsedSliceOverall2Slack_k(e,s)*getMaxUsedSliceOverall2Direction_k(e,s);
            }
        }

        for (int v = 0; v < instance.getNbNodes(); v++){
            for (int s = 0; s < getNbSlicesGlobalLimit(); s++){
                denominator += getMaxUsedSliceOverall3Slack_k(v,s)*getMaxUsedSliceOverall3Direction_k(v,s);
            }
        }
        */
    }
    return denominator;
}

double lagNewNonOverlapping::getSlackDirectionProdProjected(Input::ProjectionType projection){
    double denominator = 0.0;
    if(projection == Input::IMPROVED){
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            if(!((-getLengthDirection_k(d) < - DBL_EPSILON) && (getLengthMultiplier_k(d) > -DBL_EPSILON && getLengthMultiplier_k(d) < DBL_EPSILON))){ // if non negative direction or multiplier different from zero
                denominator += getLengthSlack_k(d)*getLengthDirection_k(d);
            }
        }
        for(int d = 0; d < getNbDemandsToBeRouted(); d++){
            for (int v = 0; v < instance.getNbNodes(); v++){
                if((v == getToBeRouted_k(d).getSource()) || v == getToBeRouted_k(d).getTarget()){   
                    denominator += getSourceTargetSlack_k(d, v)*getSourceTargetDirection_k(d,v);
                }else{
                    if(!((-getSourceTargetDirection_k(d,v) < - DBL_EPSILON ) &&  (getSourceTargetMultiplier_k(d,v) > -DBL_EPSILON && getSourceTargetMultiplier_k(d,v) < DBL_EPSILON))){ // if non negative direction or multiplier different from zero
                        denominator += getSourceTargetSlack_k(d, v)*getSourceTargetDirection_k(d,v);
                    }
                } 
            }
        }
        for (int d= 0; d < getNbDemandsToBeRouted(); d++){
            for (int v = 0; v < countNodes(*vecGraph[d]); v++){
                if((v != getToBeRouted_k(d).getSource()) && v != getToBeRouted_k(d).getTarget()){   
                    denominator += getFlowSlack_k(d, v)*getFlowDirection_k(d,v);
                }
            }
        }

        for (int e = 0; e < instance.getNbEdges(); e++){
            for (int d= 0; d < getNbDemandsToBeRouted(); d++){
                if(!((-getOneSlicePerDemandDirection_k(e,d) < - DBL_EPSILON) && (getOneSlicePerDemandMultiplier_k(e,d) > -DBL_EPSILON && getOneSlicePerDemandMultiplier_k(e,d) < DBL_EPSILON))){ // if non negative direction or multiplier different from zero
                    denominator += getOneSlicePerDemandSlack_k(e,d)*getOneSlicePerDemandDirection_k(e,d);
                }
            }
        }
        if(instance.getInput().isObj8(0)){
            for (int d = 0; d < getNbDemandsToBeRouted(); d++){
                if(!((-getMaxUsedSliceOverallDirection_k(d) < - DBL_EPSILON)&&(getMaxUsedSliceOverallMultiplier_k(d) > - DBL_EPSILON && getMaxUsedSliceOverallMultiplier_k(d) < DBL_EPSILON))){
                    denominator += getMaxUsedSliceOverallSlack_k(d)*getMaxUsedSliceOverallDirection_k(d);
                }
                //if(!((-getMaxUsedSliceOverallAuxDirection_k(d) < - DBL_EPSILON)&&(getMaxUsedSliceOverallAuxMultiplier_k(d) > - DBL_EPSILON && getMaxUsedSliceOverallAuxMultiplier_k(d) < DBL_EPSILON))){
                //    denominator += getMaxUsedSliceOverallAuxSlack_k(d)*getMaxUsedSliceOverallAuxDirection_k(d);
                //}
            }
            /*
            for (int e = 0; e < instance.getNbEdges(); e++){
                int sliceLimit = getNbSlicesLimitFromEdge(e);
                for (int s = 0; s < sliceLimit; s++){
                    if(!((-getMaxUsedSliceOverall2Direction_k(e,s) < - DBL_EPSILON)&&(getMaxUsedSliceOverall2Multiplier_k(e,s) > - DBL_EPSILON && getMaxUsedSliceOverall2Multiplier_k(e,s) < DBL_EPSILON))){
                        denominator += getMaxUsedSliceOverall2Slack_k(e,s)*getMaxUsedSliceOverall2Direction_k(e,s);
                    }
                }
            }

            for (int v = 0; v < instance.getNbNodes(); v++){
                for (int s = 0; s < getNbSlicesGlobalLimit(); s++){
                    if(!((-getMaxUsedSliceOverall3Direction_k(v,s) < - DBL_EPSILON)&&(getMaxUsedSliceOverall3Multiplier_k(v,s) > - DBL_EPSILON && getMaxUsedSliceOverall3Multiplier_k(v,s) < DBL_EPSILON))){
                        denominator += getMaxUsedSliceOverall3Slack_k(v,s)*getMaxUsedSliceOverall3Direction_k(v,s);
                    }
                }
            }
            */
        }
    }else if(projection == Input::PROJECTED){
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            if(!(-getLengthDirection_k(d) < - DBL_EPSILON)){ // if non negative direction 
                denominator += getLengthSlack_k(d)*getLengthDirection_k(d);
            }
        }
        for(int d = 0; d < getNbDemandsToBeRouted(); d++){
            for (int v = 0; v < instance.getNbNodes(); v++){
                if((v == getToBeRouted_k(d).getSource()) || v == getToBeRouted_k(d).getTarget()){   
                    denominator += getSourceTargetSlack_k(d, v)*getSourceTargetDirection_k(d,v);
                }else{
                    if(!(-getSourceTargetDirection_k(d,v) < - DBL_EPSILON )){ // if non negative direction
                        denominator += getSourceTargetSlack_k(d, v)*getSourceTargetDirection_k(d,v);
                    }
                } 
            }
        }
        for (int d= 0; d < getNbDemandsToBeRouted(); d++){
            for (int v = 0; v < countNodes(*vecGraph[d]); v++){
                if((v != getToBeRouted_k(d).getSource()) && v != getToBeRouted_k(d).getTarget()){   
                    denominator += getFlowSlack_k(d, v)*getFlowDirection_k(d,v);
                }
            }
        }

        for (int e = 0; e < instance.getNbEdges(); e++){
            for (int d= 0; d < getNbDemandsToBeRouted(); d++){
                if(!(-getOneSlicePerDemandDirection_k(e,d) < - DBL_EPSILON)){ 
                    denominator += getOneSlicePerDemandSlack_k(e,d)*getOneSlicePerDemandDirection_k(e,d);
                }
            }
        }
        
        Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
        if(chosenMetric == Input::OBJECTIVE_METRIC_8){
            for (int d = 0; d < getNbDemandsToBeRouted(); d++){
                if(!(-getMaxUsedSliceOverallDirection_k(d) < - DBL_EPSILON )){
                    denominator += getMaxUsedSliceOverallSlack_k(d)*getMaxUsedSliceOverallDirection_k(d);
                }
                //if(!(-getMaxUsedSliceOverallAuxDirection_k(d) < - DBL_EPSILON )){
                //    denominator += getMaxUsedSliceOverallAuxSlack_k(d)*getMaxUsedSliceOverallAuxDirection_k(d);
                //}
            }

            /*
            for (int e = 0; e < instance.getNbEdges(); e++){
                int sliceLimit = getNbSlicesLimitFromEdge(e);
                for (int s = 0; s < sliceLimit; s++){
                    if(!(-getMaxUsedSliceOverall2Direction_k(e,s) < - DBL_EPSILON)){
                        denominator += getMaxUsedSliceOverall2Slack_k(e,s)*getMaxUsedSliceOverall2Direction_k(e,s);
                    }
                }
            }

            for (int v = 0; v < instance.getNbNodes(); v++){
                for (int s = 0; s < getNbSlicesGlobalLimit(); s++){
                    if(!(-getMaxUsedSliceOverall3Direction_k(v,s) < - DBL_EPSILON)){
                        denominator += getMaxUsedSliceOverall3Slack_k(v,s)*getMaxUsedSliceOverall3Direction_k(v,s);
                    }
                }
            }
            */
        }
    }
    return denominator;
}

/** Returns the module **/
double lagNewNonOverlapping::getMeanSlackModule_v2(){
    
    double module = std::accumulate(lengthSlack_v2.begin(),lengthSlack_v2.end(),0.0,[](double sum, double slack){return sum +=std::abs(slack);});

    for(int d = 0; d < getNbDemandsToBeRouted(); d++){
        std::accumulate(sourceTargetSlack_v2[d].begin(),sourceTargetSlack_v2[d].end(),0.0,[](double sum, double slack){return sum +=std::abs(slack);});
    }
    for(int d = 0; d < getNbDemandsToBeRouted(); d++){
        std::accumulate(flowSlack_v2[d].begin(),flowSlack_v2[d].end(),0.0,[](double sum, double slack){return sum +=std::abs(slack);});
    }
    for (int e = 0; e < instance.getNbEdges(); e++){
        std::accumulate(oneSlicePerDemandSlack_v2[e].begin(),oneSlicePerDemandSlack_v2[e].end(),0.0,[](double sum, double slack){return sum +=std::abs(slack);});
    } 

    double numRest = getNbDemandsToBeRouted() + getNbDemandsToBeRouted()*instance.getNbNodes() + (getNbDemandsToBeRouted()*(instance.getNbNodes()-2));
    numRest += getNbDemandsToBeRouted()*instance.getNbEdges();

    if(instance.getInput().isObj8(0)){
        module += std::accumulate(maxUsedSliceOverallSlack_v2.begin(),maxUsedSliceOverallSlack_v2.end(),0.0,[](double sum, double slack){return sum +=std::abs(slack);});
        //module += std::accumulate(maxUsedSliceOverallAuxSlack_v2.begin(),maxUsedSliceOverallAuxSlack_v2.end(),0.0,[](double sum, double slack){return sum +=std::abs(slack);});

        /*
        for (int e = 0; e < instance.getNbEdges(); e++){
            module += std::accumulate(maxUsedSliceOverall2Slack_v2[e].begin(),maxUsedSliceOverall2Slack_v2[e].end(),0.0,[](double sum, double slack){return sum +=std::abs(slack);});
        }

        for (int v = 0; v < instance.getNbNodes(); v++){
            module += std::accumulate(maxUsedSliceOverall3Slack_v2[v].begin(),maxUsedSliceOverall3Slack_v2[v].end(),0.0,[](double sum, double slack){return sum +=std::abs(slack);});
        }
        */

        numRest += getNbDemandsToBeRouted();
        //numRest += getNbDemandsToBeRouted();
        /*
        for (int e = 0; e < instance.getNbEdges(); e++){
            numRest += getNbSlicesLimitFromEdge(e);
        }
        for (int e = 0; e < instance.getNbNodes(); e++){
            numRest += getNbSlicesGlobalLimit();
        }
        */
    }
    return module/numRest;
}

/** Returns the scalar product of the normal slack with the slack considering the primal variables **/
double lagNewNonOverlapping::getSlackPrimalSlackProd(double alpha){

    double denominator = 0.0;
    /* Length */
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        double slack = alpha*(-getLengthSlack_k(d)) + (1.0 - alpha)*(-getLengthSlack_v2_k(d));
        double mult = getLengthMultiplier_k(d);
        if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
            denominator += getLengthSlack_k(d)*getLengthSlack_v2_k(d);
        }
    }

    /* Source/Target */
    for(int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (int v = 0; v < instance.getNbNodes(); v++){
            double slack = alpha*(-getSourceTargetSlack_k(d,v)) + (1.0 - alpha)*(-getSourceTargetSlack_v2_k(d,v));
            double mult = getSourceTargetMultiplier_k(d,v);
            if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
                denominator += getSourceTargetSlack_k(d,v)*getSourceTargetSlack_v2_k(d,v);
            }
        }
    }

    /* Flow */
    for (int d= 0; d < getNbDemandsToBeRouted(); d++){
        for (int v = 0; v < countNodes(*vecGraph[d]); v++){
            if((v != getToBeRouted_k(d).getSource()) && v != getToBeRouted_k(d).getTarget()){
                double slack = alpha*(-getFlowSlack_k(d,v)) + (1.0 - alpha)*(-getFlowSlack_v2_k(d,v));
                double mult = getFlowMultiplier_k(d,v);
                if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
                    denominator += getFlowSlack_k(d,v)*getFlowSlack_v2_k(d,v);
                }
            }
        }   
    } 
    /* One slice Per demand */
    for (int e = 0; e < instance.getNbEdges(); e++){
        for (int d= 0; d < getNbDemandsToBeRouted(); d++){
            double slack = alpha*(-getOneSlicePerDemandSlack_k(e,d)) + (1.0 - alpha)*(-getOneSlicePerDemandSlack_v2_k(e,d));
            double mult = getOneSlicePerDemandMultiplier_k(e,d);
            if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
                denominator += getOneSlicePerDemandSlack_k(e,d)*getOneSlicePerDemandSlack_v2_k(e,d);
            }
        }
    }
    if(instance.getInput().isObj8(0)){
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            double slack = alpha*(-getMaxUsedSliceOverallSlack_k(d)) + (1.0 - alpha)*(-getMaxUsedSliceOverallSlack_v2_k(d));
            double mult = getMaxUsedSliceOverallMultiplier_k(d);
            if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
                denominator += getMaxUsedSliceOverallSlack_k(d)*getMaxUsedSliceOverallSlack_v2_k(d);
            }       
        }
        /*
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            double slack = alpha*(-getMaxUsedSliceOverallAuxSlack_k(d)) + (1.0 - alpha)*(-getMaxUsedSliceOverallAuxSlack_v2_k(d));
            double mult = getMaxUsedSliceOverallAuxMultiplier_k(d);
            if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
                denominator += getMaxUsedSliceOverallAuxSlack_k(d)*getMaxUsedSliceOverallAuxSlack_v2_k(d);
            }       
        }
        */
        /*
        for (int e = 0; e < instance.getNbEdges(); e++){
            for (int s = 0; s < getNbSlicesLimitFromEdge(e); s++){
                double slack = alpha*(-getMaxUsedSliceOverall2Slack_k(e,s)) + (1.0 - alpha)*(-getMaxUsedSliceOverall2Slack_v2_k(e,s));
                double mult = getMaxUsedSliceOverall2Multiplier_k(e,s);
                if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
                    denominator += getMaxUsedSliceOverall2Slack_k(e,s)*getMaxUsedSliceOverall2Slack_v2_k(e,s);
                }    
            }
        }
        */
        /*
        for (int v = 0; v < instance.getNbNodes(); v++){
            for (int s = 0; s < getNbSlicesGlobalLimit(); s++){
                double slack = alpha*(-getMaxUsedSliceOverall3Slack_k(v,s)) + (1.0 - alpha)*(-getMaxUsedSliceOverall3Slack_v2_k(v,s));
                double mult = getMaxUsedSliceOverall3Multiplier_k(v,s);
                if((alpha == -1.0) || !((slack < - DBL_EPSILON) && (mult > -DBL_EPSILON && mult < DBL_EPSILON))){ 
                    denominator += getMaxUsedSliceOverall3Slack_k(v,s)*getMaxUsedSliceOverall3Slack_v2_k(v,s);
                }     
            }
        }
        */
    }
    return denominator;
}

/* ***********************************************************************************************************************
*                                                   RUNING METHODS
*********************************************************************************************************************** */

void lagNewNonOverlapping::run(bool adaptedSubproblem){
    setCurrentLagrCost(0.0);
    setCurrentRealCost(0.0);

    if(getStatus() == STATUS_FEASIBLE){
        setStatus(STATUS_UNKNOWN);
    }

    updateAssignment();
    resetSlacks();

    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
        runObj8();
    }
    else{
        runGeneralObj();
    }
}

void lagNewNonOverlapping::runGeneralObj(){

    //updateCost();
    operatorCostELength operLength(lagrangianMultiplierLength, vecMaxLength);
    operatorCostEFlow operFlow(lagrangianMultiplierFlow);
    operatorCostESource operSource(lagrangianMultiplierSourceTarget, vecSourceLabels);
    operatorCostETarget operTarget(lagrangianMultiplierSourceTarget, vecTargetLabels);
    operatorCostEOneSlicePerDemand operOneSlicePerDemand(lagrangianMultiplierOneSlicePerDemand);
    
    for (int e = 0; e < instance.getNbEdges(); e++){
        /*Updating Cost */
        time.setStart(ClockTime::getTimeNow());
        
        CombineArcMapArcMapCostELength combLength((*vecEArcDemand[e]),(*vecEArcLength[e]),operLength);
        AddMapLength addLength(combLength,(*vecECoeff[e]));

        CombineArcMapArcMapCostESource combSource((*vecEArcDemand[e]),(*vecEArcSourceLabel[e]),operSource);
        CombineArcMapArcMapCostETarget combTarget((*vecEArcDemand[e]),(*vecEArcTargetLabel[e]),operTarget);
        AddMapST addST(combSource,combTarget);

        operFlow.setSignal(1);
        CombineArcMapArcMapCostEFlow combFlowSource((*vecEArcDemand[e]),(*vecEArcSourceIndex[e]),operFlow);
        operFlow.setSignal(-1);
        CombineArcMapArcMapCostEFlow combFlowTarget((*vecEArcDemand[e]),(*vecEArcTargetIndex[e]),operFlow);
        AddMapFlow addFlow(combFlowSource,combFlowTarget);

        AddMapFlowST addFlowST(addFlow,addST);
        AddMappFinal addFinal(addFlowST,addLength);

        operOneSlicePerDemand.setLabel(e);
        CombineArcMapArcMapCostEOneSlicePerDemand combOneSlicePerDemand((*vecEArcDemand[e]),(*vecEArcDemand[e]),operOneSlicePerDemand);
        AddMapFinalOneSlicePerDemand addFinalOneSlicePerDemand(addFinal,combOneSlicePerDemand);

        incCostTime(time.getTimeInSecFromStart());

        time.setStart(ClockTime::getTimeNow());

        const ListDigraph::Node SOURCE = (*vecENode[e][0]);
        const ListDigraph::Node TARGET = (*vecENode[e][getNbSlicesLimitFromEdge(e)]);

        /* Solving a shortest path for each edge considering the auxiliary graph */
        /* From the artificial source to the artificial target*/
        BellmanFordCostE shortestPath((*vecEGraph[e]), addFinalOneSlicePerDemand);
        shortestPath.run(SOURCE);
        
        /* There is always a path analysing the auxiliary graph */
        if(shortestPath.reached(TARGET) == false){
            setStatus(STATUS_INFEASIBLE);
            std::cout << "> RSA is infeasible because there is no path from the artificial source to the artificial destination. Edge " << e << "." << std::endl;
            return;
        }
        incShorstestPathTime(time.getTimeInSecFromStart());
        
        time.setStart(ClockTime::getTimeNow());
        updateAssignment_k(e, shortestPath, SOURCE, TARGET);
        incCurrentLagrCost(shortestPath.dist(TARGET));    
        incCurrentRealCost(getRealCostFromPath(e, shortestPath, SOURCE, TARGET));
        incUpdateVariablesTime(time.getTimeInSecFromStart());

    }
    time.setStart(ClockTime::getTimeNow());
    subtractConstantValuesFromLagrCost();
    incSubstractMultipliersTime(time.getTimeInSecFromStart());
}

void lagNewNonOverlapping::runObj8(){
    operatorCostELength operLength(lagrangianMultiplierLength, vecMaxLength);
    operatorCostEFlow operFlow(lagrangianMultiplierFlow);
    operatorCostESource operSource(lagrangianMultiplierSourceTarget, vecSourceLabels);
    operatorCostETarget operTarget(lagrangianMultiplierSourceTarget, vecTargetLabels);
    operatorCostEOneSlicePerDemand operOneSlicePerDemand(lagrangianMultiplierOneSlicePerDemand);
    
    for (int e = 0; e < instance.getNbEdges(); e++){

        /*Updating Cost */
        time.setStart(ClockTime::getTimeNow());
        
        CombineArcMapArcMapCostELength combLength((*vecEArcDemand[e]),(*vecEArcLength[e]),operLength);
        AddMapLength addLength(combLength,(*vecECoeff[e]));

        CombineArcMapArcMapCostESource combSource((*vecEArcDemand[e]),(*vecEArcSourceLabel[e]),operSource);
        CombineArcMapArcMapCostETarget combTarget((*vecEArcDemand[e]),(*vecEArcTargetLabel[e]),operTarget);
        AddMapST addST(combSource,combTarget);

        operFlow.setSignal(1);
        CombineArcMapArcMapCostEFlow combFlowSource((*vecEArcDemand[e]),(*vecEArcSourceIndex[e]),operFlow);
        operFlow.setSignal(-1);
        CombineArcMapArcMapCostEFlow combFlowTarget((*vecEArcDemand[e]),(*vecEArcTargetIndex[e]),operFlow);
        AddMapFlow addFlow(combFlowSource,combFlowTarget);

        AddMapFlowST addFlowST(addFlow,addST);
        AddMappFinal addFinal(addFlowST,addLength);

        operOneSlicePerDemand.setLabel(e);
        CombineArcMapArcMapCostEOneSlicePerDemand combOneSlicePerDemand((*vecEArcDemand[e]),(*vecEArcDemand[e]),operOneSlicePerDemand);
        AddMapFinalOneSlicePerDemand addFinalOneSlicePerDemand(addFinal,combOneSlicePerDemand);

        incCostTime(time.getTimeInSecFromStart());

        time.setStart(ClockTime::getTimeNow());

        const ListDigraph::Node SOURCE = (*vecENode[e][0]);
        const ListDigraph::Node TARGET = (*vecENode[e][getNbSlicesLimitFromEdge(e)]);

        /* Solving a shortest path for each edge considering the auxiliary graph */
        /* From the artificial source to the artificial target*/
        BellmanFordCostE shortestPath((*vecEGraph[e]), addFinalOneSlicePerDemand);
        shortestPath.run(SOURCE);
        
        /* There is always a path analysing the auxiliary graph */
        if(shortestPath.reached(TARGET) == false){
            setStatus(STATUS_INFEASIBLE);
            std::cout << "> RSA is infeasible because there is no path from the artificial source to the artificial destination. Edge " << e << "." << std::endl;
            return;
        }
        incShorstestPathTime(time.getTimeInSecFromStart());
        
        time.setStart(ClockTime::getTimeNow());
        updateAssignment_k(e, shortestPath, SOURCE, TARGET);
        incCurrentLagrCost(shortestPath.dist(TARGET));    
        incUpdateVariablesTime(time.getTimeInSecFromStart());

    }
    std::cout << "Cost 1: " << getLagrCurrentCost() << std::endl;
    time.setStart(ClockTime::getTimeNow());
    subtractConstantValuesFromLagrCost();
    incSubstractMultipliersTime(time.getTimeInSecFromStart());
    std::cout << "Cost 2: " << getLagrCurrentCost() << std::endl;

    solveProblemMaxUsedSliceOverall();

    int soma = 0;
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (int e = 0; e < instance.getNbEdges(); e++){
            soma =0;
            for(ListDigraph::ArcIt a(*vecGraph[d]); a!= INVALID;++a){
                if(getArcLabel(a,d)==e){
                    int index = getArcIndex(a,d);
                    soma += assignmentMatrix_d[d][index];
                }    
            }
            std::cout << "SOMA: " << d << " " << e <<  " " << soma << std::endl;
        }
    }

}

void lagNewNonOverlapping::subtractConstantValuesFromLagrCost(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        double val = -getLengthMultiplier_k(d);
        incCurrentLagrCost(val);
    }
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (int v = 0; v < instance.getNbNodes(); v++){ 
            if(v == getToBeRouted_k(d).getSource() ){
                double val = -getSourceTargetMultiplier_k(d,v);
                incCurrentLagrCost(val);
            }
            else if( v== getToBeRouted_k(d).getTarget()){
                double val = -getSourceTargetMultiplier_k(d,v);
                incCurrentLagrCost(val);
            }else{
                double val = -getSourceTargetMultiplier_k(d,v);
                incCurrentLagrCost(val);
            }
            
        }
    }
    for (int e = 0; e < instance.getNbEdges(); e++){
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            double val = -getOneSlicePerDemandMultiplier_k(e,d);
            incCurrentLagrCost(val);
        }
    }
}

void lagNewNonOverlapping::solveProblemMaxUsedSliceOverall(){
    double exp = 0.0;

    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        exp += getMaxUsedSliceOverallMultiplier_k(d);
    }
    for (int e = 0; e < instance.getNbEdges(); e++){
        int sliceLimit = getNbSlicesLimitFromEdge(e);
        for (int s = 0; s < sliceLimit; s++){
            exp += getMaxUsedSliceOverall2Multiplier_k(e,s);
        }
    }

    for (int v = 0; v < instance.getNbNodes(); v++){
        for (int s = 0; s < getNbSlicesGlobalLimit(); s++){
            exp += getMaxUsedSliceOverall3Multiplier_k(v,s);
        }
    }

    if(exp > 1.000){
        maxUsedSliceOverall = (getInstance().getMaxSlice()-1);
    }else{
        maxUsedSliceOverall = 0.0;
    }

    incCurrentLagrCost(maxUsedSliceOverall*(1-exp));
    incCurrentRealCost(maxUsedSliceOverall);
}

/* ***********************************************************************************************************************
*                                                     DISPLAY
*********************************************************************************************************************** */
void lagNewNonOverlapping::displaySlack(std::ostream & saida){
    std::string display = "Length Slack = [ \n";
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        display += "Demand " + std::to_string(d+1) + ": " +std::to_string(-getLengthSlack_k(d)) + "\n"; 
    }
    display += "]";
    saida << display << std::endl;

    display = "Source Target Slack = [ \n";
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        display += "Demand " + std::to_string(d+1) + "\n ";
        for (int v = 0; v < instance.getNbNodes(); v++){ 
            display += "\t Node " + std::to_string(v+1) + ": "+std::to_string(-getSourceTargetSlack_k(d,v)) + "\n"; 
        }
    }
    display += "]";
    saida << display << std::endl;

    display = "Flow Slack = [ \n";
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        display += "Demand " + std::to_string(d+1) + "\n ";
        for (int v = 0; v < countNodes(*vecGraph[d]); v++){ 
            display += "\t Node " + std::to_string(v+1) + ": "+ std::to_string(-getFlowSlack_k(d,v)) + "\n"; 
        }
    }

    display += "]";
    saida << display << std::endl;

}

void lagNewNonOverlapping::displayMultiplier(std::ostream & saida){
    std::string display = "Length Multiplier = [ \n";
    for (unsigned int i = 0; i < lagrangianMultiplierLength.size(); i++){
        display += "Demand " + std::to_string(i+1) + ": " + std::to_string(getLengthMultiplier_k(i)) + "\n"; 
    }
    display += "]";
    saida << display << std::endl;

    display = "Source Target Multiplier = [ \n";
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        display += "Demand " + std::to_string(d+1) + ":\n ";
        for (int v = 0; v < instance.getNbNodes(); v++){ 
            display += "\t Node " + std::to_string(v+1) + ": "+ std::to_string(getSourceTargetMultiplier_k(d,v)) + "\n"; 

        }
    }
    display += "]";
    saida << display << std::endl;
    
    display = "Flow Multiplier = [ \n";
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        display += "Demand " + std::to_string(d+1) + "\n ";
        for (int v = 0; v < countNodes(*vecGraph[d]); v++){ 
            display += "\t Node "  + std::to_string(v+1) +": " + std::to_string(getFlowMultiplier_k(d,v)) + "\n"; 

        }
    }
    display += "]";
    saida << display << std::endl;
}

/*********************************************************************************************************************** */

void lagNewNonOverlapping::updateCost(){
    
    for (int e = 0; e < instance.getNbEdges(); e++){
        for (ListDigraph::ArcIt a(*vecEGraph[e]); a != INVALID; ++a){
            (*cost[e])[a] = (*vecECoeff[e])[a];
        }
    }
    for(int e = 0; e < instance.getNbEdges(); e++){
        for (ListDigraph::ArcIt a(*vecEGraph[e]); a != INVALID; ++a){
            int demand = getArcEDemand(a,e);
            if(demand != -1){
                (*cost[e])[a] += lagrangianMultiplierLength[demand]*getArcELength(a,e)/getToBeRouted_k(demand).getMaxLength();
            }
        }
    }
    for(int e = 0; e < instance.getNbEdges(); e++){
        for (ListDigraph::ArcIt a(*vecEGraph[e]); a != INVALID; ++a){
            int demand = getArcEDemand(a,e);
            int sourcelabel = getArcESourceLabel(a,e);
            int targetlabel = getArcETargetLabel(a,e);
            if(demand != -1){
                if(sourcelabel == getToBeRouted_k(demand).getTarget()){
                    std::cout << "OIIIIIIIIIIIIIIIIIIIIIIIIIIII" << std::endl;
                }
                (*cost[e])[a] += lagrangianMultiplierSourceTarget[demand][sourcelabel];
                if(targetlabel == getToBeRouted_k(demand).getTarget()){
                    (*cost[e])[a] += lagrangianMultiplierSourceTarget[demand][targetlabel];
                }
            }
        }
    }

    for(int e = 0; e < instance.getNbEdges(); e++){
        for (ListDigraph::ArcIt a(*vecEGraph[e]); a != INVALID; ++a){
            int demand = getArcEDemand(a,e);
            int sourcelabel = getArcESourceLabel(a,e);
            int targetlabel = getArcETargetLabel(a,e);
            int sourceIndex = getArcESourceIndex(a,e);
            int targetIndex = getArcETargetIndex(a,e);
            if(demand != -1){
                if( (sourcelabel != getToBeRouted_k(demand).getTarget()) && (sourcelabel != getToBeRouted_k(demand).getSource()) ){
                    (*cost[e])[a] += lagrangianMultiplierFlow[demand][sourceIndex];
                }
                if( (targetlabel != getToBeRouted_k(demand).getTarget()) && (targetlabel != getToBeRouted_k(demand).getSource()) ){
                    (*cost[e])[a] -= lagrangianMultiplierFlow[demand][targetIndex];
                }
            }
        }
    }

    for(int e = 0; e < instance.getNbEdges(); e++){
        for (ListDigraph::ArcIt a(*vecEGraph[e]); a != INVALID; ++a){
            int demand = getArcEDemand(a,e);
            if(demand == -1){
                //std::cout << "-1    " << (*cost[e])[a] << std::endl;
            }
        }
    }

}

