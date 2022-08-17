#include "lagNonOverlapping.h"
#include <set>

void lagNonOverlapping::getDualSolution(double *rowprice){
    std::cout<< "Branch and bound for no overlapping not defined yet.\n";
}

void lagNonOverlapping::clearSlacks(){
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

void lagNonOverlapping::init(bool initMult){

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

void lagNonOverlapping::startMultipliers(double *row ,int size,int objSignal){
    std::cout << "Non overlapping formulation with Branch and Bound not defined yet.\n";
}

/* Sets the initial lagrangian multipliers for the subgradient to run. */
void lagNonOverlapping::initMultipliers(){
    bool warmstart = getInstance().getInput().getWarmstart();
    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);

    if(warmstart){
        /**** Initializing all with 0.o and then solving the warmstart procedure *****/
        initializeLengthMultipliers(0.0);
        initializeSourceTargetMultipliers(0.0);
        initializeFlowMultipliers(0.0);

        if(chosenMetric == Input::OBJECTIVE_METRIC_8){
           initializeMaxUsedSliceOverallMultipliers(0.0);
            initializeMaxUsedSliceOverall2Multipliers(0.0);
            initializeMaxUsedSliceOverall3Multipliers(0.0);
        }
        initMultipliersWarmstart();
    }else{
        /**** Initializing all with a given initial multiplier *****/
        double initialMultiplier = instance.getInput().getInitialLagrangianMultiplier();
        initializeLengthMultipliers(initialMultiplier);
        initializeSourceTargetMultipliers(initialMultiplier);
        initializeFlowMultipliers(initialMultiplier);

        if(chosenMetric == Input::OBJECTIVE_METRIC_8){
           initializeMaxUsedSliceOverallMultipliers(initialMultiplier);
            initializeMaxUsedSliceOverall2Multipliers(initialMultiplier);
            initializeMaxUsedSliceOverall3Multipliers(initialMultiplier);
        }
    }
    std::cout << "> Initial Lagrangian multipliers were defined. " << std::endl;
}

/*********************************************** Warmstart  procedure  **************************************************/

void lagNonOverlapping::initMultipliersWarmstart(){
    
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        /** Copying the graph **/
        std::shared_ptr<ListDigraph> vecGraphAux = std::make_shared<ListDigraph>();
        DigraphCopy<ListDigraph, ListDigraph> cg(*vecGraph[d], *vecGraphAux);
        ListDigraph::NodeMap<ListDigraph::Node> nodes(*vecGraph[d]);
        cg.nodeRef(nodes);
        ListDigraph::ArcMap<ListDigraph::Arc> arcs(*vecGraphAux);
        cg.arcCrossRef(arcs);
        //ListDigraph::ArcMap<ListDigraph::Arc> arcs(*vecGraph[d]);
        //cg.arcRef(arcs);
        NodeMap new_map(*vecGraphAux);
        cg.nodeMap(*vecNodeLabel[d], new_map);
        cg.run();
        

        /** Modyfing the graph : contracting all the nodes with the same label **/
        for (int label = 0; label < instance.getNbNodes(); label++){
            if(label != getToBeRouted_k(d).getSource() && label!=  getToBeRouted_k(d).getTarget()){
                ListDigraph::NodeIt previousNode(*vecGraph[d]);
                ListDigraph::Node n = getFirstNodeFromLabel(d, label);
                ListDigraph::NodeIt v(*vecGraph[d]);
                ListDigraph::NodeIt currentNode(*vecGraph[d], v);
                if(n != INVALID){
                    //(*vecNodeSlice[d])[n] = -1;
                    while (v != INVALID){
                        currentNode = v;
                        ListDigraph::NodeIt nextNode(*vecGraph[d], ++currentNode);
                        currentNode = v;
                        if ( (getNodeLabel(v, d) == label) && ((*vecGraph[d]).id(n) != (*vecGraph[d]).id(v)) ){
                            (*vecGraphAux).contract(nodes[n],nodes[v]);
                            //(*vecGraph[d]).contract(n, v);
                        }
                        v = nextNode;
                    }
                }
            }
        }
        /** Creating maps for the COST and CAPACITY**/
        ListDigraph::ArcMap<double> COST(*vecGraphAux);
        ListDigraph::ArcMap<int> CAPACITY(*vecGraphAux);
        ListDigraph::NodeMap<int> SUPPLY(*vecGraphAux);
        for(ListDigraph::ArcIt a(*vecGraphAux); a != INVALID; ++a){
            COST[a] = getCoeff(arcs[a],d);
        }
        for(ListDigraph::ArcIt a(*vecGraphAux); a != INVALID; ++a){
            CAPACITY[a] = 1;
        }
        for(ListDigraph::NodeIt v(*vecGraphAux); v != INVALID; ++v){
            SUPPLY[v] = 0;
        }

        /** Finding the source and destination */
        const ListDigraph::Node SOURCE_ORIGINAL = getFirstNodeFromLabel(d, getToBeRouted_k(d).getSource());
        const ListDigraph::Node TARGET_ORIGINAL = getFirstNodeFromLabel(d, getToBeRouted_k(d).getTarget());
        const ListDigraph::Node SOURCE = nodes[SOURCE_ORIGINAL];
        const ListDigraph::Node TARGET = nodes[TARGET_ORIGINAL];

        SUPPLY[SOURCE] = 1;
        SUPPLY[TARGET] = -1;
        
        /** Running the CapacityScaling problem **/

        //CapacityScaling<ListDigraph,ListDigraph::ArcMap<int>,ListDigraph::ArcMap<double>,ListDigraph::NodeMap<int>> capScale(*vecGraphAux,CAPACITY,COST,SUPPLY);	
        CapacityScaling<ListDigraph,int,double> capScale(*vecGraphAux);//,CAPACITY,COST,SUPPLY);	
        capScale.upperMap(CAPACITY);
        capScale.costMap(COST);
        capScale.supplyMap(SUPPLY);
        capScale.run();

        /** Finding the node potentials **/
        for(ListDigraph::NodeIt v(*vecGraphAux); v != INVALID; ++v){
            int label = new_map[v];
            if(label == getToBeRouted_k(d).getSource() || label == getToBeRouted_k(d).getTarget()){
                lagrangianMultiplierSourceTarget[d][label] = capScale.potential(v);
            }else{
                lagrangianMultiplierFlow[d][label] = capScale.potential(v);
            }

        }

    }    
}

/* Contract nodes with the same given label from the given graph, that is connected with the d-graph. */
void lagNonOverlapping::contractNodesFromLabel_v2(std::shared_ptr<ListDigraph> vecGraphAux, ListDigraph::NodeMap<ListDigraph::Node> nodes, int d, int label){
    int nb = 0;
    ListDigraph::NodeIt previousNode(*vecGraph[d]);
    ListDigraph::Node n = getFirstNodeFromLabel(d, label);
    ListDigraph::NodeIt v(*vecGraph[d]);
    ListDigraph::NodeIt currentNode(*vecGraph[d], v);
    if(n != INVALID){
        //(*vecNodeSlice[d])[n] = -1;
        while (v != INVALID){
            currentNode = v;
            ListDigraph::NodeIt nextNode(*vecGraph[d], ++currentNode);
            currentNode = v;
            if ( (getNodeLabel(v, d) == label) && ((*vecGraph[d]).id(n) != (*vecGraph[d]).id(v)) ){
                (*vecGraphAux).contract(nodes[n],nodes[v]);
                //(*vecGraph[d]).contract(n, v);
                nb++;
            }
            v = nextNode;
        }
    }
    //std::cout << "> Number of nodes with label " << label << " contracted: " << nb << std::endl; 
}

/************************************************ STABILITY CENTER ****************************************************/

void lagNonOverlapping::initStabilityCenter(){
    initializeLengthSC();
    initializeSourceTargetSC();
    initializeFlowSC();

    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
        initializeMaxUsedSliceOverallSC();
        initializeMaxUsedSliceOverall2SC();
        initializeMaxUsedSliceOverall3SC();
    }
}

/**************************************************** SLACKS **********************************************************/

/* Initializes the slack of relaxed constraints. */
void lagNonOverlapping::initSlacks(){
    initializeLengthSlacks();
    initializeSourceTargetSlacks();
    initializeFlowSlacks();

    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
        initializeMaxUsedSliceOverallSlacks();
        initializeMaxUsedSliceOverall2Slacks();
        initializeMaxUsedSliceOverall3Slacks();
    }
    
    std::cout << "> Initial Slacks were defined. " << std::endl;
}

void lagNonOverlapping::resetSlacks(){
    resetLengthSlacks();
    resetSourceTargetSlacks();
    resetFlowSlacks();

    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
        resetMaxUsedSliceOverallSlacks();
        resetMaxUsedSliceOverall2Slacks();
        resetMaxUsedSliceOverall3Slacks();
    }
}

/********************************************* SLACKS PRIMAL APPROXIMATION *************************************************/

/* Initializes the slack of relaxed constraints. */
void lagNonOverlapping::initPrimalSlacks(){
    initializeLengthPrimalSlacks();
    initializeSourceTargetPrimalSlacks();
    initializeFlowPrimalSlacks();

    initializeOneSlicePerDemandPrimalSlacks();

    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
        initializeMaxUsedSliceOverallPrimalSlacks();
        initializeMaxUsedSliceOverallAuxPrimalSlacks();
        //initializeMaxUsedSliceOverall2Slacks();
        //initializeMaxUsedSliceOverall3Slacks();
    }   
    std::cout << "> Initial Slacks were defined. " << std::endl;
}

/*************************************************** DIRECTION ********************************************************/

void lagNonOverlapping::initDirection(){
    initializeLengthDirection();
    initializeSourceTargetDirection();
    initializeFlowDirection();

    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
        initializeMaxUsedSliceOverallDirection();
        initializeMaxUsedSliceOverall2Direction();
        initializeMaxUsedSliceOverall3Direction();
    }
}

/********************************************* BUILD AUXILIARY GRAPH **************************************************/

/* Builds an auxiliary graph for each edge */
void lagNonOverlapping::build_Graphs_e(){  
    vecESourceIndex.resize(instance.getNbEdges());
    vecEDestinationIndex.resize(instance.getNbEdges());
    for(int d = 0; d < getNbDemandsToBeRouted(); d++){
        cost.emplace_back(std::make_shared<ArcCost>((*vecGraph[d])));
    }
    for (int e = 0; e < instance.getNbEdges(); e++){
        /* Initialization - memory allocation with std::make_shared - it allocates the memory and returs the pointer */
        /* Its a vector of graphs, for each edge, it includes its graph in the list (as push_back but do not need the constructor) */   
        vecEGraph.emplace_back(std::make_shared<ListDigraph>());
        vecENodeID.emplace_back(std::make_shared<NodeMap>((*vecEGraph[e])));
        vecENodeDemand.emplace_back(std::make_shared<NodeMap>((*vecEGraph[e])));
        vecENodeSlice.emplace_back(std::make_shared<NodeMap>((*vecEGraph[e])));
        vecENodeFirstConst.emplace_back(std::make_shared<NodeMap>((*vecEGraph[e])));
        vecENodeArc.emplace_back(std::make_shared<ListDigraph::NodeMap<std::shared_ptr<ListDigraph::Arc>>>((*vecEGraph[e])));
        vecEArcId.emplace_back(std::make_shared<ArcMap>((*vecEGraph[e])));
        vecECost.emplace_back(std::make_shared<ArcCost>((*vecEGraph[e])));
        vecENodeIndex.emplace_back(std::make_shared<NodeMap>((*vecEGraph[e])));

        int linklabel = instance.getPhysicalLinkFromIndex(e).getId(); /* label of link e */
        /** For all arcs in G(D) -> create a corresponding node **/
        for(int d = 0; d < getNbDemandsToBeRouted(); d++){
            int load = getToBeRouted_k(d).getLoad();
            for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){ /* vecGraph is protected in RSA **/
                /** if the arc corresponds to edge e **/  
                if((getArcLabel(a, d)  == linklabel)){
                    int slice = getArcSlice(a,d);
                    /** add a node - label(same as the arc), demand(analysed), slice(same as the arc), firstconstraint(slice-load[k]+1)**/
                    addENode(linklabel,d,slice,(slice-load+1),a); 
                }
            }
        }

        /** Adding two auxiliary nodes to do the routing: source and destination  **/
        addENode(e,-1,-1,-1,INVALID); //source
        addENode(e,-2,instance.getPhysicalLinkFromIndex(e).getNbSlices(),instance.getPhysicalLinkFromIndex(e).getNbSlices(),INVALID); //destination
        
        for(ListDigraph::NodeIt v(*vecEGraph[e]); v != INVALID; ++v){
            int lastConst = getNodeESlice(v,e);
            int demand1 = getNodeEDemand(v,e);
            for(ListDigraph::NodeIt v2(*vecEGraph[e]); v2 != INVALID; ++v2){
                int firstConst = getNodeEFirstConst(v2,e);
                int demand2 = getNodeEDemand(v2,e);
                /** If they are different nodes, and the firstconst the v2 appears is greater than the last const v appears **/
                //if((demand1 == -1) || (demand1 == -2) || (demand2== -1) || (demand2 == -2) || (demand1 != demand2)){
                    if((v2 != v) && (firstConst > lastConst)){
                        /** Adding an arc between v and v2 **/
                        ListDigraph::Arc a = vecEGraph[e]->addArc(v,v2);
                        int id = vecEGraph[e]->id(a);
                        setArcEId(a, e, id);
                        setArcECost(a, e, 0.0);
                    }
                //}
            }
        }

        /* Sets node index. */
        int index=0;
        for (ListDigraph::NodeIt v(*vecEGraph[e]); v != INVALID; ++v){
            setNodeEIndex(v, e, index);
            index++;
        }
    }
    std::cout << "> Graphs per edge were defined. " << std::endl;
}

/* Function to add a new node to the auxiliary graph */
void lagNonOverlapping::addENode(int e, int demand, int slice, int firstConst,const ListDigraph::Arc & a){
    ListDigraph::Node node = vecEGraph[e]->addNode();
    int id = vecEGraph[e]->id(node);
    setNodeEId(node,e,id);
    setNodeEDemand(node,e,demand);
    setNodeESlice(node,e,slice);
    setNodeEFirstConst(node,e,firstConst);
    setNodeEArc(node,e,a);

    if(demand == -1){
        setIndexSource(id,e);
    }
    else if(demand == -2){
        setIndexDestination(id,e);
    }
}

/* **********************************************************************************************************************
*                                                  UPDATE METHODS
*********************************************************************************************************************** */

/*************************************************** MULTIPLIERS ********************************************************/

/* Updates lagrangian multiplier with the rule: u[k+1] = u[k] + t[k]*violation */
void lagNonOverlapping::updateMultiplier(double step){
    updateLengthMultiplier(step);
    updateSourceTargetMultiplier(step);
    updateFlowMultiplier(step);

    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
        updateMaxUsedSliceOverallMultiplier(step);
        updateMaxUsedSliceOverall2Multiplier(step);
        updateMaxUsedSliceOverall3Multiplier(step);
    }
}

/************************************** MULTIPLIER CONSIDERING THE STABILITY CENTER **************************************/

void lagNonOverlapping::updateMultiplier_v2(double step){
    updateLengthMultiplier_v2(step);
    updateSourceTargetMultiplier_v2(step);
    updateFlowMultiplier_v2(step);

    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
        updateMaxUsedSliceOverallMultiplier_v2(step);
        updateMaxUsedSliceOverall2Multiplier_v2(step);
        updateMaxUsedSliceOverall3Multiplier_v2(step);
    }
}

/***************************************************** STABILITY CENTER ***************************************************/

void lagNonOverlapping::updateStabilityCenter(){
    updateLengthSC();
    updateSourceTargetSC();
    updateFlowSC();

    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
        updateMaxUsedSliceOverallSC();
        updateMaxUsedSliceOverall2SC();
        updateMaxUsedSliceOverall3SC();
    }
}

/********************************************************** SLACK *********************************************************/

void lagNonOverlapping::updateSlack(int d, const ListDigraph::Arc & arc){
    updateLengthSlack(d,arc);
    updateSourceTargetSlack(d,arc);
    updateFlowSlack(d,arc);

    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
        updateMaxUsedSliceOverallSlack(d,arc);
        updateMaxUsedSliceOverall2Slack(d,arc);
        updateMaxUsedSliceOverall3Slack(d,arc);
    }
}

void lagNonOverlapping::updatePrimalSlack(double alpha){
    updateLengthPrimalSlack(alpha);
    updateSourceTargetPrimalSlack(alpha);
    updateFlowPrimalSlack(alpha);

    updateOneSlicePerDemandPrimalSlack(alpha);

    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
        updateMaxUsedSliceOverallPrimalSlack(alpha);
        updateMaxUsedSliceOverallAuxPrimalSlack(alpha);
        //updateMaxUsedSliceOverall2PrimalSlack(alpha);
        //updateMaxUsedSliceOverall3PrimalSlack(alpha);
    }
}

/********************************************************** DIRECTION *****************************************************/

void lagNonOverlapping::updateDirection(){
    double theta = getDirectionMult();
    updateLengthDirection(theta);
    updateSourceTargetDirection(theta);
    updateFlowDirection(theta);

    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
        updateMaxUsedSliceOverallDirection(theta);
        updateMaxUsedSliceOverall2Direction(theta);
        updateMaxUsedSliceOverall3Direction(theta);
    }
}

/**************************************************** ASSIGNMENT MATRIX *****************************************************/

/* Updates the assignment of a edge based on the a given path. */
void lagNonOverlapping::updateAssignment_k(int label, BellmanFord< ListDigraph, ListDigraph::ArcMap<double> > &path, const ListDigraph::Node &SOURCE, const ListDigraph::Node &TARGET){
    
    ListDigraph::Node currentNode = TARGET;
    while (currentNode != SOURCE){
        const ListDigraph::Arc arc = getNodeEArc(currentNode,label); 
        if(arc != INVALID){ 
            int demand = getNodeEDemand(currentNode,label);
            int index = getArcIndex(arc, demand);
            assignmentMatrix_d[demand][index] = true;

            /* Update Slack */
            updateSlack(demand,arc);
        }
        currentNode = path.predNode(currentNode);
    }
}

/**************************************************** CHECK FEASIBILITY *****************************************************/

/* Checks feasibility of sub problem solution. */
bool lagNonOverlapping::checkFeasibility(){
    if (checkLengthFeasibility() == false){
        return false;
    }
    if (checkSourceTargetFeasibility() == false){
        return false;
    }
    if (checkFlowFeasibility() == false){
        return false;
    }
    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
        if(checkMaxUsedSliceOverallFeasibility()){
            return false;
        }
    }
    return true;
}

/* Ckecks feasibility of primal approximation. */
bool lagNonOverlapping::checkFeasibility_v2(){
    if (checkLengthFeasibility_v2() == false){
        return false;
    }
    if (checkSourceTargetFeasibility_v2() == false){
        return false;
    }
    if (checkFlowFeasibility_v2() == false){
        return false;
    }
    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
        if(checkMaxUsedSliceOverallFeasibility_v2()){
            return false;
        }
    }
    return true;
}

bool lagNonOverlapping::checkSlacknessCondition(){
    if (checkLengthSlacknessCondition() == false){
        return false;
    }
    if (checkSourceTargetSlacknessCondition() == false){
        return false;
    }
    if (checkFlowSlacknessCondition() == false){
        return false;
    }
    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
        if(checkMaxUsedSliceOverallSlacknessCondition()){
            return false;
        }
    }
    return true;
}

/* ***************************************************************************************************************************
*                                                         GET METHODS
**************************************************************************************************************************** */


ListDigraph::Node lagNonOverlapping::getNodeFromIndex(int e, int id){
    for (ListDigraph::NodeIt n(*vecEGraph[e]); n != INVALID; ++n){
        if(getNodeEId(n,e) == id){
            return n;
        }
    }
    return INVALID;
}

double lagNonOverlapping::getRealCostFromPath(int e, BellmanFord< ListDigraph, ListDigraph::ArcMap<double> > &path, const ListDigraph::Node &SOURCE, const ListDigraph::Node &TARGET){
    double total = 0.0;
    ListDigraph::Node currentNode = TARGET;
    while (currentNode != SOURCE){
        const ListDigraph::Arc arc = getNodeEArc(currentNode,e); 
        if(arc != INVALID){ 
            total += getCoeff(arc, getNodeEDemand(currentNode,e)); 
        }
        currentNode = path.predNode(currentNode);
    }
    return total;
}

/** Returns the constraints slack module **/
double lagNonOverlapping::getSlackModule(double alpha) {
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
    
    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
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
double lagNonOverlapping::getSlackModule_v2(double alpha){
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

    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
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
double lagNonOverlapping::getDirectionModule(){
    double denominator = 0.0;
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        denominator += std::pow(getLengthDirection_k(d), 2);
    }
    for(int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (int v = 0; v < instance.getNbNodes(); v++){ 
            denominator += std::pow(getSourceTargetDirection_k(d, v), 2);
        }
    }
    for (int d= 0; d < getNbDemandsToBeRouted(); d++){
        for (int v = 0; v < countNodes(*vecGraph[d]); v++){
            if((v != getToBeRouted_k(d).getSource()) && v != getToBeRouted_k(d).getTarget()){   
                denominator += std::pow(getFlowDirection_k(d, v), 2);
            }
        }
    } 
    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            denominator += std::pow(getMaxUsedSliceOverallDirection_k(d), 2);
        }

        for (int e = 0; e < instance.getNbEdges(); e++){
            int sliceLimit = getNbSlicesLimitFromEdge(e);
            for (int s = 0; s < sliceLimit; s++){
                denominator += std::pow(getMaxUsedSliceOverall2Direction_k(e,s), 2);
            }
        }

        for (int v = 0; v < instance.getNbNodes(); v++){
            for (int s = 0; s < getNbSlicesGlobalLimit(); s++){
                denominator += std::pow(getMaxUsedSliceOverall3Direction_k(v,s), 2);
            }
        }
    }
    return denominator;
}

/** Returns the scalar product between the slack (gradient) and the direction **/
double lagNonOverlapping::getSlackDirectionProd(){

    Input::ProjectionType projection = getInstance().getInput().getChosenProjection();
    if((projection == Input::IMPROVED) ||(projection == Input::PROJECTED)){
        return getSlackDirectionProdProjected(projection);
    }   
    return getSlackDirectionProdNormal();  
}

double lagNonOverlapping::getSlackDirectionProdNormal(){
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
    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            denominator += getMaxUsedSliceOverallSlack_k(d)*getMaxUsedSliceOverallDirection_k(d);
        }

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
    }
    return denominator;
}

double lagNonOverlapping::getSlackDirectionProdProjected(Input::ProjectionType projection){
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
                    //if(!((getSourceTargetDirection_k(d,v) > - DBL_EPSILON && getSourceTargetDirection_k(d,v) < DBL_EPSILON ) && (getSourceTargetMultiplier_k(d,v) > -DBL_EPSILON  && getSourceTargetMultiplier_k(d,v) < DBL_EPSILON))){
                    denominator += getSourceTargetSlack_k(d, v)*getSourceTargetDirection_k(d,v);
                    //}
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
                    //if(!((getFlowDirection_k(d,v) > - DBL_EPSILON && getFlowDirection_k(d,v) < DBL_EPSILON ) &&  (getFlowMultiplier_k(d,v) > -DBL_EPSILON && getFlowMultiplier_k(d,v) < DBL_EPSILON))){   
                    denominator += getFlowSlack_k(d, v)*getFlowDirection_k(d,v);
                    //}
                }
            }
        }
        Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
        if(chosenMetric == Input::OBJECTIVE_METRIC_8){
            for (int d = 0; d < getNbDemandsToBeRouted(); d++){
                if(!((-getMaxUsedSliceOverallDirection_k(d) < - DBL_EPSILON)&&(getMaxUsedSliceOverallMultiplier_k(d) > - DBL_EPSILON && getMaxUsedSliceOverallMultiplier_k(d) < DBL_EPSILON))){
                    denominator += getMaxUsedSliceOverallSlack_k(d)*getMaxUsedSliceOverallDirection_k(d);
                }
            }

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
                    //if(!((getSourceTargetDirection_k(d,v) > - DBL_EPSILON && getSourceTargetDirection_k(d,v) < DBL_EPSILON ) && (getSourceTargetMultiplier_k(d,v) > -DBL_EPSILON  && getSourceTargetMultiplier_k(d,v) < DBL_EPSILON))){
                    denominator += getSourceTargetSlack_k(d, v)*getSourceTargetDirection_k(d,v);
                    //}
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
                    //if(!((getFlowDirection_k(d,v) > - DBL_EPSILON && getFlowDirection_k(d,v) < DBL_EPSILON ) &&  (getFlowMultiplier_k(d,v) > -DBL_EPSILON && getFlowMultiplier_k(d,v) < DBL_EPSILON))){   
                    denominator += getFlowSlack_k(d, v)*getFlowDirection_k(d,v);
                    //}
                }
            }
        }
        Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
        if(chosenMetric == Input::OBJECTIVE_METRIC_8){
            for (int d = 0; d < getNbDemandsToBeRouted(); d++){
                if(!(-getMaxUsedSliceOverallDirection_k(d) < - DBL_EPSILON )){
                    denominator += getMaxUsedSliceOverallSlack_k(d)*getMaxUsedSliceOverallDirection_k(d);
                }
            }

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
        }
    }
    return denominator;
}

/** Returns the module **/
double lagNonOverlapping::getMeanSlackModule_v2(){
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

    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
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
double lagNonOverlapping::getSlackPrimalSlackProd(double alpha){

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

    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
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

void lagNonOverlapping::run(bool adaptedSubproblem){
    setCurrentLagrCost(0.0);
    setCurrentRealCost(0.0);

    if(getStatus() == STATUS_FEASIBLE){
        setStatus(STATUS_UNKNOWN);
    }

    updateAssignment();
    updateCosts();

    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    for (int e = 0; e < instance.getNbEdges(); e++){
        
        time.setStart(ClockTime::getTimeNow());
        const ListDigraph::Node SOURCE = getNodeFromIndex(e, getIndexSource(e));
        const ListDigraph::Node TARGET = getNodeFromIndex(e, getIndexDestination(e));

        /* Solving a shortest path for each edge considering the auxiliary graph */
        /* From the artificial source to the artificial target*/
        BellmanFord<ListDigraph,ListDigraph::ArcMap<double>> shortestPath((*vecEGraph[e]), (*vecECost[e]));
        shortestPath.run(SOURCE);
        
        if(shortestPath.reached(TARGET) == false){ // There is always a path in this graph.
            setStatus(STATUS_INFEASIBLE);
            std::cout << "> RSA is infeasible because there is no path from the artificial source to the artificial destination. Edge " << e << "." << std::endl;
            return;
        }
        incShorstestPathTime(time.getTimeInSecFromStart());

        time.setStart(ClockTime::getTimeNow());
        updateAssignment_k(e, shortestPath, SOURCE, TARGET);
        incUpdateVariablesTime(time.getTimeInSecFromStart());

        incCurrentLagrCost(shortestPath.dist(TARGET));    
        if(chosenMetric != Input::OBJECTIVE_METRIC_8){
            incCurrentRealCost(getRealCostFromPath(e, shortestPath, SOURCE, TARGET));
        } 
    }

    std::cout << "Cost 1: " << getLagrCurrentCost() << std::endl;
    time.setStart(ClockTime::getTimeNow());
    subtractConstantValuesFromLagrCost();
    incSubstractMultipliersTime(time.getTimeInSecFromStart());
    std::cout << "Cost 2: " << getLagrCurrentCost() << std::endl;

    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
        solveProblemMaxUsedSliceOverall();
    }

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
    std::cout << "SOMA: " << soma << std::endl;
}

void lagNonOverlapping::subtractConstantValuesFromLagrCost(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        double val = -getLengthMultiplier_k(d);
        incCurrentLagrCost(val);
        std::cout << "length " << val << std::endl;
    }

    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        double val = -getSourceTargetMultiplier_k(d,getToBeRouted_k(d).getSource()) - getSourceTargetMultiplier_k(d,getToBeRouted_k(d).getTarget());
        incCurrentLagrCost(val);
        std::cout << "source slack " << -getSourceTargetSlack_k(d,getToBeRouted_k(d).getSource() ) << std::endl;
        std::cout << "target slack " << -getSourceTargetSlack_k(d,getToBeRouted_k(d).getTarget() ) << std::endl;
        std::cout << "sum mult " << val << std::endl;
    }

    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (int v = 0; v < instance.getNbNodes(); v++){ 
            if((v != getToBeRouted_k(d).getSource()) &&  (v != getToBeRouted_k(d).getTarget())){
                double val = -getSourceTargetMultiplier_k(d,v);
                std::cout << "other nodes " << val<< std::endl;
                incCurrentLagrCost(val);
            }
        }
    }
}

void lagNonOverlapping::solveProblemMaxUsedSliceOverall(){
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
        maxUsedSliceOverall = getInstance().getMaxSlice();
    }else{
        maxUsedSliceOverall = 0.0;
    }

    incCurrentLagrCost(maxUsedSliceOverall*(1-exp));
    incCurrentRealCost(maxUsedSliceOverall);
}

/*********************************************************** COSTS ***********************************************************/

/* Updates the arc costs according to the last lagrangian multiplier available. cost = c + u_k*length */
/* It calcules the cost for the original variables, and then it passes the cost to the auxiliary graph */
void lagNonOverlapping::updateCosts(){
    /********* Original Variables *********/
    /* Original costs */
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
            setArcCost(a, d, getCoeff(a,d));
        }
    }
    /* Length multipliers */
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        double multiplier =  getLengthMultiplier_k(d);
        for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
            double incrementValue = multiplier*getArcLength(a, d)/getToBeRouted_k(d).getMaxLength();
            incArcCost(a, d, incrementValue);
        }
    }
    
    /* Source/Target multipliers */
    for(int d =0; d < getNbDemandsToBeRouted(); d++){
        for (int label = 0; label < instance.getNbNodes(); label++){
            double multiplier = getSourceTargetMultiplier_k(d,label);
            for (ListDigraph::NodeIt v(*vecGraph[d]); v != INVALID; ++v){
                if((getNodeLabel(v, d) == label) && (label != getToBeRouted_k(d).getTarget()) && (label != getToBeRouted_k(d).getSource())){
                    for (ListDigraph::OutArcIt a((*vecGraph[d]), v); a != INVALID; ++a){
                       double incrementValue = multiplier;
                       incArcCost(a, d, incrementValue);
                    }
                }
                else if((getNodeLabel(v, d) == label) && (label == getToBeRouted_k(d).getSource())){
                    for (ListDigraph::OutArcIt a((*vecGraph[d]), v); a != INVALID; ++a){
                       double incrementValue =  multiplier;
                       incArcCost(a, d, incrementValue);
                    }
                }
                else if((getNodeLabel(v, d) == label) && (label == getToBeRouted_k(d).getTarget())){
                    for (ListDigraph::InArcIt a((*vecGraph[d]), v); a != INVALID; ++a){
                       double incrementValue =  multiplier;
                       incArcCost(a, d, incrementValue);
                    }
                }
            }
        }
    }

    /* Flow constraints */
    for(int d =0; d < getNbDemandsToBeRouted(); d++){
        for (ListDigraph::NodeIt v(*vecGraph[d]); v != INVALID; ++v){
            int label = getNodeLabel(v, d);
            int indexNode = getNodeIndex(v, d);
            if((label != getToBeRouted_k(d).getTarget()) && (label != getToBeRouted_k(d).getSource())){
                double multiplier = getFlowMultiplier_k(d,indexNode);
                for (ListDigraph::OutArcIt a((*vecGraph[d]), v); a != INVALID; ++a){
                    double incrementValue =  multiplier;
                    incArcCost(a, d, incrementValue);
                }
                for (ListDigraph::InArcIt a((*vecGraph[d]), v); a != INVALID; ++a){
                    double incrementValue = - multiplier;
                    incArcCost(a, d, incrementValue);
                }
            }
        }
    }

    for(int d =0; d < getNbDemandsToBeRouted(); d++){
        double sum =0;
        for (ListDigraph::NodeIt v(*vecGraph[d]); v != INVALID; ++v){
            int indexNode = getNodeIndex(v, d);
            double multiplier = getFlowMultiplier_k(d,indexNode);
            sum += multiplier;
        }
        std::cout << "FLOW MULT " << sum <<std::endl;

    }
    double soma =0;
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){

        for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
            if(getArcCost(a,d)  < 0.0){
                soma += getArcCost(a,d);
            }
            //std::cout << getArcCost(a,d) << "   "; 
        }
        //std::cout << std::endl;
    }
    std::cout << "SOMA COSTT "<< soma << std::endl;

    Input::ObjectiveMetric chosenMetric = getInstance().getInput().getChosenObj_k(0);
    if(chosenMetric == Input::OBJECTIVE_METRIC_8){
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            double multiplier =  getMaxUsedSliceOverallMultiplier_k(d);
            for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                if (getToBeRouted_k(d).getSource() == getNodeLabel((*vecGraph[d]).source(a), d)){
                    double incrementValue = multiplier*getArcSlice(a, d);
                    incArcCost(a,d,incrementValue);
                }
            }
        }

        for (int linkLabel = 0; linkLabel < instance.getNbEdges(); linkLabel++){
            int sliceLimit = getNbSlicesLimitFromEdge(linkLabel);
            for (int s = 0; s < sliceLimit; s++){
                double multiplier =  getMaxUsedSliceOverall2Multiplier_k(linkLabel,s);
                for (int d = 0; d < getNbDemandsToBeRouted(); d++){
                    int demandLoad = getToBeRouted_k(d).getLoad();
                    for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                        if ((getArcLabel(a, d) == linkLabel) && (getArcSlice(a, d) >= s) && (getArcSlice(a, d) <= s + demandLoad - 1)){
                            double incrementValue = multiplier*getArcSlice(a, d);
                            incArcCost(a,d,incrementValue);
                        }
                    }
                }
            }
        }

        for (int nodeLabel = 0; nodeLabel < instance.getNbNodes(); nodeLabel++){
            for (int s = 0; s < getNbSlicesGlobalLimit(); s++){
                double multiplier = getMaxUsedSliceOverall3Multiplier_k(nodeLabel,s);
                for (int d = 0; d < getNbDemandsToBeRouted(); d++){
                    int demandLoad = getToBeRouted_k(d).getLoad();
                    for (ListDigraph::NodeIt v(*vecGraph[d]); v != INVALID; ++v){
                        if ((getNodeLabel(v, d) == nodeLabel)){
                            for (ListDigraph::OutArcIt a(*vecGraph[d], v); a != INVALID; ++a){
                                if ( (getArcSlice(a, d) >= s) && (getArcSlice(a, d) <= s + demandLoad - 1) ){
                                    double incrementValue = multiplier*getArcSlice(a, d);
                                    incArcCost(a,d,incrementValue);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /* Passing the costs to the auxiliary graph */
    for (int e = 0; e < instance.getNbEdges(); e++){
        for(ListDigraph::ArcIt a(*vecEGraph[e]); a != INVALID; ++a){
            const ListDigraph::Node DEST = (*vecEGraph[e]).target(a);
            const ListDigraph::Arc arc = getNodeEArc(DEST,e);
            if(arc != INVALID){ /* If it is not the final destination */
                setArcECost(a,e,getArcCost(arc, getNodeEDemand(DEST,e)));
            }
        }
    }
}

/* *****************************************************************************************************************
*                                                       DISPLAYS
****************************************************************************************************************** */

/* Displays an arc from the graph #e. */
void lagNonOverlapping::displayEArc(int e, const ListDigraph::Arc &a){
    /* nodes from the auxiliary graph */
    const ListDigraph::Node SOURCE = (*vecEGraph[e]).source(a);
    const ListDigraph::Node DEST = (*vecEGraph[e]).target(a);

    /* arcs it represents in the original graph */
    const ListDigraph::Arc ARC_SOURCE = getNodeEArc(SOURCE,e);
    const ListDigraph::Arc ARC_DEST = getNodeEArc(DEST,e);

    /* if its artificial nodes source/ destination in the auxiliary graph */
    //if(getArcECost(a,e) < 0){
    //    std::cout << "edge:" << e;
    if((ARC_SOURCE == INVALID) || (ARC_DEST == INVALID)){
        if((ARC_SOURCE == INVALID) && (ARC_DEST == INVALID)){
            if((getNodeEDemand(SOURCE,e) == -1) && (getNodeEDemand(DEST,e) ==-2)){
                std::cout << "(Node source: SOURCE) \t (Node destination: DEST)";
                std::cout << "(Cost: " << getArcECost(a,e) << ")"<< std::endl;
            }else if((getNodeEDemand(SOURCE,e) == -2) && (getNodeEDemand(DEST,e) ==-1)){
                std::cout << "(Node source: DEST) \t (Node destination: SOURCE)";
                std::cout << "(Cost: " << getArcECost(a,e) << ")"<< std::endl;
            }
        }else if(ARC_SOURCE == INVALID){
            if(getNodeEDemand(SOURCE,e) == -1){
                std::cout << "(Node source: SOURCE) \t ";
                int d = getNodeEDemand(DEST,e);
                std::cout << "(Node destination : " << getNodeLabel((*vecGraph[d]).source(ARC_DEST), d) + 1;
                std::cout << "--";
                std::cout <<  getNodeLabel((*vecGraph[d]).target(ARC_DEST), d) + 1 << ", " << getArcSlice(ARC_DEST, d) + 1 <<", dem:"<< d +1 <<  ")";
                std::cout << "(Cost: " << getArcECost(a,e) << ")"<< std::endl;
            }else if(getNodeEDemand(SOURCE,e) == -2){
                std::cout << "(Node source: DEST) \t ";
                int d = getNodeEDemand(DEST,e);
                std::cout << "(Node destination : " << getNodeLabel((*vecGraph[d]).source(ARC_DEST), d) + 1;
                std::cout << "--";
                std::cout <<  getNodeLabel((*vecGraph[d]).target(ARC_DEST), d) + 1 << ", " << getArcSlice(ARC_DEST, d) + 1 <<", dem:"<< d+1 << ")";
                std::cout << "(Cost: " << getArcECost(a,e) << ")"<< std::endl;
            }
        }else if(ARC_DEST == INVALID){
            if(getNodeEDemand(DEST,e) == -1){
                int d = getNodeEDemand(SOURCE,e);
                std::cout << "(Node source : " << getNodeLabel((*vecGraph[d]).source(ARC_SOURCE) , d) + 1;
                std::cout << "--";
                std::cout <<  getNodeLabel((*vecGraph[d]).target(ARC_SOURCE), d) + 1 << ", " << getArcSlice(ARC_SOURCE, d) + 1 << ", dem:"<< d+1 << ")\t";
                std::cout << "(Node destination : SOURCE)";
                std::cout << "(Cost: " << getArcECost(a,e) << ")"<< std::endl;

            }else if(getNodeEDemand(DEST,e) == -2){
                int d = getNodeEDemand(SOURCE,e);
                std::cout << "(Node source : " << getNodeLabel((*vecGraph[d]).source(ARC_SOURCE) , d) + 1;
                std::cout << "--";
                std::cout <<  getNodeLabel((*vecGraph[d]).target(ARC_SOURCE), d) + 1 << ", " << getArcSlice(ARC_SOURCE, d) + 1 << ", dem:"<< d+1 << ")\t";
                std::cout << "(Node destination : DEST)";
                std::cout << "(Cost: " << getArcECost(a,e) << ")"<< std::endl;
            }
        }
    }else{
        /* Printing the conection */
        int d = getNodeEDemand(SOURCE,e);
        std::cout << "(Node source : " << getNodeLabel((*vecGraph[d]).source(ARC_SOURCE) , d) + 1;
        std::cout << "--";
        std::cout <<  getNodeLabel((*vecGraph[d]).target(ARC_SOURCE), d) + 1 << ", " << getArcSlice(ARC_SOURCE, d) + 1 << ", dem:"<< d+1 <<")\t";

        d = getNodeEDemand(DEST,e);
        std::cout << "(Node destination : " << getNodeLabel((*vecGraph[d]).source(ARC_DEST) , d) + 1;
        std::cout << "--";
        std::cout <<  getNodeLabel((*vecGraph[d]).target(ARC_DEST), d) + 1 << ", " << getArcSlice(ARC_DEST, d) + 1 << ", dem:"<< d+1 << ")";
        std::cout << "(Cost: " << getArcECost(a,e) << ")"<< std::endl;
    }
    //}

}

/* Display all arcs from the graph #e. */
void lagNonOverlapping::displayEGraph(int e){
    for (ListDigraph::ArcIt a(*vecEGraph[e]); a != INVALID; ++a){
        displayEArc(e, a);
    }
}

void lagNonOverlapping::displayAssignmentMatrix(int e){
    for (ListDigraph::NodeIt v(*vecEGraph[e]); v != INVALID; ++v){
        displayENode(v,e);
    }
}

/* Displays a node from the graph #d. */
void lagNonOverlapping::displayENode(const ListDigraph::Node &n, int e){
    const ListDigraph::Arc arc = getNodeEArc(n,e);
    if(arc != INVALID){
        int d = getNodeEDemand(n,e);
        int index = getNodeEIndex(n,e);
        std::cout << "(Node : " << getNodeLabel((*vecGraph[d]).source(arc) , d) + 1;
        std::cout << "--";
        std::cout  << getNodeLabel((*vecGraph[d]).target(arc), d)+1 << "," << getArcSlice(arc, d)+1 << ")";
        std::cout << "(Assigment: " << assignmentMatrix[e][index] << " )"<<std::endl;
    }
}

void lagNonOverlapping::displaySlack(std::ostream & saida){
    std::string display = "Length Slack = [ \n";
    //std::cout << "Length:" << std::endl;
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        display += "Demand " + std::to_string(d+1) + ": " +std::to_string(-getLengthSlack_k(d)) + "\n"; 
        //std::cout << "Demand:" << d << " "<< getLengthSlack_k(d) << std::endl;
    }
    display += "]";
    //std::cout << display << std::endl;
    saida << display << std::endl;

    display = "Source Target Slack = [ \n";
    //std::cout << std::endl <<"Source/Target:" << std::endl;
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        display += "Demand " + std::to_string(d+1) + "\n ";
        for (int v = 0; v < instance.getNbNodes(); v++){ 
            display += "\t Node " + std::to_string(v+1) + ": "+std::to_string(-getSourceTargetSlack_k(d,v)) + "\n"; 
            //std::cout << "Demand:" << d << " Node:" << v << " "<< getSourceTargetSlack_k(d,v) << std::endl;
        }
    }
    display += "]";
    //std::cout << display << std::endl;
    saida << display << std::endl;

    display = "Flow Slack = [ \n";
    //std::cout << std::endl <<"Source/Target:" << std::endl;
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        display += "Demand " + std::to_string(d+1) + "\n ";
        for (int v = 0; v < instance.getNbNodes(); v++){ 
            //if((v != getToBeRouted_k(d).getSource()) && v != getToBeRouted_k(d).getTarget()){
                //std::cout << "Demand:" << d << " Node:" << v << " "<< getFlowSlack_k(d,v) << std::endl;
            //}
            display += "\t Node " + std::to_string(v+1) + ": "+ std::to_string(-getFlowSlack_k(d,v)) + "\n"; 
        }
    }

    display += "]";
    saida << display << std::endl;
    //std::cout << display << std::endl;

}

void lagNonOverlapping::displayMultiplier(std::ostream & saida){
    std::string display = "Length Multiplier = [ \n";
    for (unsigned int i = 0; i < lagrangianMultiplierLength.size(); i++){
        display += "Demand " + std::to_string(i+1) + ": " + std::to_string(getLengthMultiplier_k(i)) + "\n"; 
    }
    display += "]";
    //std::cout << display << std::endl;
    saida << display << std::endl;

    display = "Source Target Multiplier = [ \n";
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        display += "Demand " + std::to_string(d+1) + ":\n ";
        for (int v = 0; v < instance.getNbNodes(); v++){ 
            display += "\t Node " + std::to_string(v+1) + ": "+ std::to_string(getSourceTargetMultiplier_k(d,v)) + "\n"; 

        }
    }
    display += "]";
    //std::cout << display << std::endl;
    saida << display << std::endl;
    //overlap 2
    
    display = "Flow Multiplier = [ \n";
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        display += "Demand " + std::to_string(d+1) + "\n ";
        for (int v = 0; v < instance.getNbNodes(); v++){ 
            display += "\t Node "  + std::to_string(v+1) +": " + std::to_string(getFlowMultiplier_k(d,v)) + "\n"; 

        }
    }
    display += "]";
    //std::cout << display << std::endl;
    saida << display << std::endl;
}

void lagNonOverlapping::createGraphFile(int it){
    //std::cout<<"oi" << std::endl;
    for (int e = 0; e < instance.getNbEdges(); e++){
        std::string nom = "outputs/graph_edge_";
        nom.append(std::to_string(e+1));
        nom.append("_it_");
        nom.append(std::to_string(it));
        nom.append(".dot");
        std::ofstream fichier(nom);
        if (!fichier.fail()) {
            fichier << "digraph G {" << std::endl;
            for (ListDigraph::ArcIt a(*vecEGraph[e]); a != INVALID; ++a){

                /* nodes from the auxiliary graph */
                const ListDigraph::Node SOURCE = (*vecEGraph[e]).source(a);
                const ListDigraph::Node DEST = (*vecEGraph[e]).target(a);

                /* arcs it represents in the original graph */
                const ListDigraph::Arc ARC_SOURCE = getNodeEArc(SOURCE,e);
                const ListDigraph::Arc ARC_DEST = getNodeEArc(DEST,e);

                /* if its artificial nodes source/ destination in the auxiliary graph */
                //if(getArcECost(a,e) < 0){
                //    std::cout << "edge:" << e;
                if((ARC_SOURCE == INVALID) || (ARC_DEST == INVALID)){
                    if((ARC_SOURCE == INVALID) && (ARC_DEST == INVALID)){
                        if((getNodeEDemand(SOURCE,e) == -1) && (getNodeEDemand(DEST,e) ==-2)){
                            fichier << " \" SOURCE \" -> \" DEST \" [label=\"" << getArcECost(a,e) << "\"];" << std::endl;
                        }else if((getNodeEDemand(SOURCE,e) == -2) && (getNodeEDemand(DEST,e) ==-1)){
                            fichier << " \" DEST \" -> \" SOURCE \" [label=\"" << getArcECost(a,e) << "\"];" << std::endl;
                        }
                    }else if(ARC_SOURCE == INVALID){
                        if(getNodeEDemand(SOURCE,e) == -1){
                            int d = getNodeEDemand(DEST,e);
                            fichier << " \" SOURCE \" -> \" ";
                            fichier << (getNodeLabel((*vecGraph[d]).source(ARC_DEST), d) + 1) ;
                            fichier << "--";
                            fichier << (getNodeLabel((*vecGraph[d]).target(ARC_DEST), d) + 1) << ", ";
                            fichier << "s:" << (getArcSlice(ARC_DEST, d) + 1) << " d:" << (d+1) << " \" " ;
                            fichier << "[label=\"" << getArcECost(a,e) << "\"];" << std::endl;
                        }else if(getNodeEDemand(SOURCE,e) == -2){
                            int d = getNodeEDemand(DEST,e);
                            fichier << " \" DEST \" -> \" ";
                            fichier << getNodeLabel((*vecGraph[d]).source(ARC_DEST), d) + 1;
                            fichier << "--";
                            fichier << getNodeLabel((*vecGraph[d]).target(ARC_DEST), d) + 1 << ", ";
                            fichier << "s:" << getArcSlice(ARC_DEST, d) + 1 << " d:" << d+1 << " \" ";
                            fichier << "[label=\"" << getArcECost(a,e) << "\"];" << std::endl;
                        }
                    }else if(ARC_DEST == INVALID){
                        if(getNodeEDemand(DEST,e) == -1){
                            int d = getNodeEDemand(SOURCE,e);
                            fichier << " \" ";
                            fichier << getNodeLabel((*vecGraph[d]).source(ARC_SOURCE) , d) + 1;
                            fichier << "--";
                            fichier << getNodeLabel((*vecGraph[d]).target(ARC_SOURCE), d) + 1;
                            fichier << ", ";
                            fichier << "s:" << getArcSlice(ARC_SOURCE, d) + 1 << " d:" << d+1;
                            fichier << " \" ";
                            fichier << " -> ";
                            fichier << " \" SOURCE \" ";
                            fichier << "[label=\"" << getArcECost(a,e) << "\"];" << std::endl;
                        }else if(getNodeEDemand(DEST,e) == -2){
                            int d = getNodeEDemand(SOURCE,e);
                            fichier << " \" ";
                            fichier << getNodeLabel((*vecGraph[d]).source(ARC_SOURCE) , d) + 1;
                            fichier << "--";
                            fichier << getNodeLabel((*vecGraph[d]).target(ARC_SOURCE), d) + 1;
                            fichier << ", ";
                            fichier << "s:" << getArcSlice(ARC_SOURCE, d) + 1 << " d:" << d+1;
                            fichier << " \" ";
                            fichier << " -> ";
                            fichier << " \" DEST \" ";
                            fichier << "[label=\"" << getArcECost(a,e) << "\"];" << std::endl;
                        }
                    }
                }else{
                    /* Printing the conection */
                    int d = getNodeEDemand(SOURCE,e);
                    fichier << " \" ";
                    fichier << getNodeLabel((*vecGraph[d]).source(ARC_SOURCE) , d) + 1;
                    fichier << "--";
                    fichier << getNodeLabel((*vecGraph[d]).target(ARC_SOURCE), d) + 1;
                    fichier << ", ";
                    fichier << "s:" << getArcSlice(ARC_SOURCE, d) + 1 << " d:"<< d+1;
                    fichier << " \" ";
                    fichier << " -> ";
                    fichier << " \" ";
                    d = getNodeEDemand(DEST,e);
                    fichier << getNodeLabel((*vecGraph[d]).source(ARC_DEST) , d) + 1;
                    fichier << "--";
                    fichier << getNodeLabel((*vecGraph[d]).target(ARC_DEST), d) + 1;
                    fichier << ", ";
                    fichier << "s:" << getArcSlice(ARC_DEST, d) + 1 << " d:"<< d+1;
                    fichier << " \" ";
                    fichier << "[label=\"" << getArcECost(a,e) << "\"];" << std::endl;
                }
            }
            fichier << "}" << std::endl;
        }
        std::string command = "dot -Tjpg -o ";
        std::string nom2 = "outputs/graph_edge_";
        nom2.append(std::to_string(e+1));
        nom2.append("_it_");
        nom2.append(std::to_string(it));
        nom2.append(".jpg");
        command.append(nom2);
        command.append(" ");
        command.append(nom);
        system(command.c_str());
    }
}

/* *******************************************************************************
*                             DESTRUCTOR
******************************************************************************* */

lagNonOverlapping::~lagNonOverlapping(){
    lagrangianMultiplierLength.clear();
    lagrangianMultiplierSourceTarget.clear();
    lagrangianMultiplierFlow.clear();

    lengthSlack.clear();
    sourceTargetSlack.clear();
    flowSlack.clear();

    lengthSlack_v2.clear();
    sourceTargetSlack_v2.clear();
    flowSlack_v2.clear();

    lagrangianSCLength.clear();
    lagrangianSCSourceTarget.clear();
    lagrangianSCFlow.clear();

    lengthDirection.clear();
    sourceTargetDirection.clear();
    flowDirection.clear();

    assignmentMatrix.clear();

    for (int e = 0; e < instance.getNbEdges(); e++){
        vecEGraph[e]->clear();
    }
    vecEGraph.clear();
    vecENodeID.clear();
    vecENodeDemand.clear();
    vecENodeSlice.clear();
    vecENodeFirstConst.clear();
    vecEArcId.clear();
    vecECost.clear();
    cost.clear();
    vecENodeArc.clear();
    vecESourceIndex.clear();
    vecEDestinationIndex.clear();

}

