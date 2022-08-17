#include "shortestPathHeuristic.h"

/* *******************************************************************************
*                              CONSTRUCTOR
******************************************************************************* */

shortestPathHeuristic::shortestPathHeuristic(AbstractLagFormulation* form): AbstractHeuristic(form,STATUS_UNKNOWN){
    for (int d = 0; d < formulation->getNbDemandsToBeRouted(); d++){
        /* Initializing costs */
        heuristicCosts.emplace_back( std::make_shared<ArcCost>(*formulation->getVecGraphD(d), 0.0)); 
        freeArcs.emplace_back(std::make_shared<ListDigraph::ArcMap<bool>>(*formulation->getVecGraphD(d),true));
        mapItLabel.emplace_back(std::make_shared<IterableIntMap<ListDigraph, ListDigraph::Arc>>(*formulation->getVecGraphD(d)));
        mapCopy<ListDigraph,ArcMap,IterableIntMap<ListDigraph, ListDigraph::Arc>>((*formulation->getVecGraphD(d)),(*formulation->getArcLabelMap(d)),(*mapItLabel[d]));
        mapItLower.emplace_back(std::make_shared<IterableIntMap<ListDigraph, ListDigraph::Arc>>(*formulation->getVecGraphD(d)));
    }  
}

/* *******************************************************************************
*                              INITIALIZATION
******************************************************************************* */

/* Initializes the Heuristics elements to run the algorithm */
void shortestPathHeuristic::init(bool costs){  
    initSolution();
    initDemandsSets();
    if(costs){
        initCosts();
    }
    statusheuristic = STATUS_UNKNOWN;
}

/* Initialize the sets of not analysed demands and analysed demands for the Heuristic - to run the algorithm*/
void shortestPathHeuristic::initDemandsSets(){
    notAnalysedDemands.clear();
    analysedDemands.clear();
    for (int d = 0; d < formulation->getNbDemandsToBeRouted(); d++){ 
        notAnalysedDemands.insert(d);
        mapCopy<ListDigraph,ArcMap,IterableIntMap<ListDigraph, ListDigraph::Arc>>((*formulation->getVecGraphD(d)),(*formulation->getArcLowerMap(d)),(*mapItLower[d]));
    }
}

/** Initialize the considered cost for the heuristic using the values of the assgnmentMatrix. cost arc = 1-x - to run the algorithm **/
void shortestPathHeuristic::initCosts(){
    Input::NodeMethod chosenMethod = formulation->getInstance().getInput().getChosenNodeMethod();
    Input::ObjectiveMetric chosenMetric = formulation->getInstance().getInput().getChosenObj_k(0);
    switch (chosenMethod){
        case Input::NODE_METHOD_VOLUME:{
            HeuristicCostVar operPrimal(formulation->getPrimalVariables()); // operator to combine the information
            if(chosenMetric == Input::OBJECTIVE_METRIC_8){
                for (int d = 0; d < formulation->getNbDemandsToBeRouted(); d++){
                    operPrimal.setDemand(d);
                    CombineHeuristicCostPrimal combPrimal((*formulation->getCoeffMapObj8(d)),(*formulation->getIndexMap(d)),operPrimal);
                    mapCopy<ListDigraph,CombineHeuristicCostPrimal,ArcCost>((*formulation->getVecGraphD(d)),combPrimal,(*heuristicCosts[d]));
                    mapFill<ListDigraph,ListDigraph::ArcMap<bool>>((*formulation->getVecGraphD(d)),(*freeArcs[d]),true);
                }
               
            }else{
                for (int d = 0; d < formulation->getNbDemandsToBeRouted(); d++){
                    operPrimal.setDemand(d);
                    CombineHeuristicCostPrimal combPrimal((*formulation->getCoeffMap(d)),(*formulation->getIndexMap(d)),operPrimal);
                    mapCopy<ListDigraph,CombineHeuristicCostPrimal,ArcCost>((*formulation->getVecGraphD(d)),combPrimal,(*heuristicCosts[d]));
                    mapFill<ListDigraph,ListDigraph::ArcMap<bool>>((*formulation->getVecGraphD(d)),(*freeArcs[d]),true);
                }
            }
            break;
        }
        case Input::NODE_METHOD_SUBGRADIENT:{
            HeuristicCostAssign operVar(formulation->getVariables());
            if(chosenMetric == Input::OBJECTIVE_METRIC_8){
                for (int d = 0; d < formulation->getNbDemandsToBeRouted(); d++){
                    operVar.setDemand(d);
                    CombineHeuristicCostAssign combVar((*formulation->getCoeffMapObj8(d)),(*formulation->getIndexMap(d)),operVar);
                    mapCopy<ListDigraph,CombineHeuristicCostAssign,ArcCost>((*formulation->getVecGraphD(d)),combVar,(*heuristicCosts[d]));
                    mapFill<ListDigraph,ListDigraph::ArcMap<bool>>((*formulation->getVecGraphD(d)),(*freeArcs[d]),true);
                }
            }else{    
                for (int d = 0; d < formulation->getNbDemandsToBeRouted(); d++){
                    operVar.setDemand(d);
                    CombineHeuristicCostAssign combVar((*formulation->getCoeffMap(d)),(*formulation->getIndexMap(d)),operVar);
                    mapCopy<ListDigraph,CombineHeuristicCostAssign,ArcCost>((*formulation->getVecGraphD(d)),combVar,(*heuristicCosts[d]));
                    mapFill<ListDigraph,ListDigraph::ArcMap<bool>>((*formulation->getVecGraphD(d)),(*freeArcs[d]),true);
                }
            }
            break;
        }
        default:{
            HeuristicCostAssign operVar(formulation->getVariables());
            if(chosenMetric == Input::OBJECTIVE_METRIC_8){
                for (int d = 0; d < formulation->getNbDemandsToBeRouted(); d++){
                    operVar.setDemand(d);
                    CombineHeuristicCostAssign combVar((*formulation->getCoeffMapObj8(d)),(*formulation->getIndexMap(d)),operVar);
                    mapCopy<ListDigraph,CombineHeuristicCostAssign,ArcCost>((*formulation->getVecGraphD(d)),combVar,(*heuristicCosts[d]));
                    mapFill<ListDigraph,ListDigraph::ArcMap<bool>>((*formulation->getVecGraphD(d)),(*freeArcs[d]),true);
                }
            }else{    
                for (int d = 0; d < formulation->getNbDemandsToBeRouted(); d++){
                    operVar.setDemand(d);
                    CombineHeuristicCostAssign combVar((*formulation->getCoeffMap(d)),(*formulation->getIndexMap(d)),operVar);
                    mapCopy<ListDigraph,CombineHeuristicCostAssign,ArcCost>((*formulation->getVecGraphD(d)),combVar,(*heuristicCosts[d]));
                    mapFill<ListDigraph,ListDigraph::ArcMap<bool>>((*formulation->getVecGraphD(d)),(*freeArcs[d]),true);
                }
            }
            break;
        }
    } 
}

void shortestPathHeuristic::initAdaptedCosts(const double *costs){
    operatorHeuristicAdaptedCost oper(costs);
    if(formulation->getInstance().getInput().isObj8(0)){
        for (int d = 0; d < formulation->getNbDemandsToBeRouted(); d++){
            oper.setDemand(d);
            CombineArcMapArcCostHeuristicAdaptedCost comb((*formulation->getVarIdMap(d)),(*formulation->getCoeffMapObj8(d)),oper);
            mapCopy<ListDigraph,CombineArcMapArcCostHeuristicAdaptedCost,ArcCost>((*formulation->getVecGraphD(d)),comb,(*heuristicCosts[d]));
            mapFill<ListDigraph,ListDigraph::ArcMap<bool>>((*formulation->getVecGraphD(d)),(*freeArcs[d]),true);
        }
    }else{
        for (int d = 0; d < formulation->getNbDemandsToBeRouted(); d++){
            oper.setDemand(d);
            CombineArcMapArcCostHeuristicAdaptedCost comb((*formulation->getVarIdMap(d)),(*formulation->getCoeffMap(d)),oper);
            mapCopy<ListDigraph,CombineArcMapArcCostHeuristicAdaptedCost,ArcCost>((*formulation->getVecGraphD(d)),comb,(*heuristicCosts[d]));
            mapFill<ListDigraph,ListDigraph::ArcMap<bool>>((*formulation->getVecGraphD(d)),(*freeArcs[d]),true);
        
        }
    }
}

/* *******************************************************************************
*                              RUNNING METHODS
******************************************************************************* */

void shortestPathHeuristic::run(bool modifiedProblem,bool costs){
    /* Initialization*/
    init(costs);
    setStatus(STATUS_UNKNOWN);
    infeasibleByBound = false;
    //std::cout << "Heuristic's initialization" << std::endl;

    /* If the problem is modified (fixed variables), we have to do a preprocessing.*/
    bool infeasible = false;
    if(modifiedProblem){
        infeasible = adaptedPreprocessing();
        //std::cout << "Adapted preprocessing\n";
        if(infeasible){
            setStatus(STATUS_INFEASIBLE);
            infeasibleByBound = true;
            std::cout << "Fixed variables make the problem infeasible (heuristic)." << std::endl;
            updateCostFromHeuristic();
            return;
        }
    }
    
    int maxChanges = 5*formulation->getNbDemandsToBeRouted();
    maxChangesPossible = false;
    
    int count = 0;
    while(!notAnalysedDemands.empty()){
        /* Choses a demand */
        int demand = choseDemand(notAnalysedDemands);
        int lixo = notAnalysedDemands.erase(demand);
        std::pair<std::set<int>::iterator,bool> aux = analysedDemands.insert(demand);
        bool STOP = false;
        //std::cout << demand << "chosen demand\n";

        while(!STOP){
            /* Apply the shortest path */   
            if(modifiedProblem){
                STOP = heuristicAdaptedRun(demand); 
            }else{
                STOP = heuristicRun(demand);  
            } 
            if(getStatus() == STATUS_INFEASIBLE){
                STOP = true;
            }
            count ++; 
            if (count  > maxChanges){
                maxChangesPossible = true;
                break;
            }  
        }
        if(getStatus() == STATUS_INFEASIBLE){
            std::cout << "Infea" <<std::endl;
            break;
        }
        if (count  > maxChanges){
            maxChangesPossible = true;
            break;
        }
    }
    updateCostFromHeuristic();
}

/* Find a feasible shortest path  for demand d - return true*/
/* If coul not find the path - return false */
bool shortestPathHeuristic::heuristicRun(int d){
    
    /** SOURCE and DEST from the graph d **/
    const ListDigraph::Node SOURCE = formulation->getFirstNodeFromLabel(d, formulation->getToBeRouted_k(d).getSource());
    const ListDigraph::Node TARGET = formulation->getFirstNodeFromLabel(d, formulation->getToBeRouted_k(d).getTarget());

    /** Shortest path with the heuristic cost**/
    FilterArcs<ListDigraph> subgraph((*formulation->getVecGraphD(d)), (*freeArcs[d]));
    Dijkstra< FilterArcs<ListDigraph>, ListDigraph::ArcMap<double> > shortestPath(subgraph, (*heuristicCosts[d]));
    //Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > shortestPath(*formulation->getVecGraphD(d), (*heuristicCosts[d]));
    shortestPath.run(SOURCE, TARGET);
   
    time.setStart(ClockTime::getTimeNow());
    if (shortestPath.reached(TARGET) == false){   
        /** There is no path between the source and the destination for this demand **/
        /** It does not respect the non overlap constraints **/
        /** Remove a demand from the analysed Demands **/
        if(!analysedDemands.empty()){
            int demand2 = choseDemand(analysedDemands);
            analysedDemands.erase(demand2);
            notAnalysedDemands.insert(demand2);
            removePath_k(demand2);
        }else{
            setStatus(STATUS_INFEASIBLE);
        }
        return false;

    }else if(getPathLength(d, shortestPath, SOURCE, TARGET) > formulation->getToBeRouted_k(d).getMaxLength()){
        /** The solution is infeasible because of the length constraints**/
        remove_Arc(d,shortestPath,SOURCE,TARGET);
        return false;
    }else{
        /** A feasible path for the demand was found **/
        insertPath_k(d,shortestPath,SOURCE,TARGET);
        timeAux += time.getTimeInSecFromStart();
        return true;
    }
}

bool shortestPathHeuristic::heuristicAdaptedRun(int d){
    
    /** SOURCE and DEST from the graph d **/
    const ListDigraph::Node SOURCE = formulation->getFirstNodeFromLabel(d, formulation->getToBeRouted_k(d).getSource());
    const ListDigraph::Node TARGET = formulation->getFirstNodeFromLabel(d, formulation->getToBeRouted_k(d).getTarget());

    /** Shortest path with the heuristic cost**/
    FilterArcs<ListDigraph> subgraph(*formulation->getVecGraphD(d), (*freeArcs[d]));
    CapacityScaling<FilterArcs<ListDigraph>,int,double> costScale(subgraph);
    costScale.costMap((*heuristicCosts[d]));
    costScale.lowerMap((*formulation->getArcLowerMap(d)));
    costScale.upperMap((*formulation->getArcUpperMap(d)));
    costScale.stSupply(SOURCE,TARGET,1);
    CapacityScaling<FilterArcs<ListDigraph>,int,double>::ProblemType problemType = costScale.run();

    if(problemType == CapacityScaling<ListDigraph,int,double>::INFEASIBLE){
        /** There is no path between the source and the destination for this demand **/
        /** It does not respect the non overlap constraints **/
        /** Remove a demand from the analysed Demands **/
        if(!analysedDemands.empty()){
            int demand2 = choseDemand(analysedDemands);
            analysedDemands.erase(demand2);
            notAnalysedDemands.insert(demand2);
            removePath_k(demand2);
            //std::cout << "Remove " << demand2+1 <<std::endl;
        }else{
            setStatus(STATUS_INFEASIBLE);
        }
        return false;
    }else if(getPathLength(d, costScale) > formulation->getToBeRouted_k(d).getMaxLength()){
        /** The solution is infeasible because of the length constraints**/
        remove_Arc(d,costScale,SOURCE,TARGET);
        //std::cout<< "length" << d+1 << std::endl;
        return false;
    }else if(solutionInfeasible(d,costScale)){
        //std::cout<< "sol inf" << d+1 << std::endl;
        return false;
    }
    else{
        /** A feasible path for the demand was found **/
        insertPath_k(d,costScale,SOURCE,TARGET);
        //std::cout<< "insert" << d+1 << std::endl;
        return true;
    }
}

bool shortestPathHeuristic::solutionInfeasible(int d,CapacityScaling<FilterArcs<ListDigraph>,int,double> &costScale){
    IterableValueMap<ListDigraph,ListDigraph::Arc,double> auxiliary(*formulation->getVecGraphD(d));
    costScale.flowMap(auxiliary);

    bool infeasible = false; // if the solution is infeasible or not
    int load = formulation->getToBeRouted_k(d).getLoad();
    std::vector<int> label; std::vector<int> slice;  // for the non overlapping.
    std::vector<int> source; std::vector<int> target;  // for the source/target.
    std::vector<bool> free_; // if the arc is free or not according to the modifications
    std::vector<int> lower; // if the variable is fixed to 1
    std::vector<ListDigraph::Arc> arcs; //the arcs
   
    for(IterableValueMap<ListDigraph,ListDigraph::Arc,double>::ItemIt arc(auxiliary,1); arc != INVALID; ++arc){
        label.push_back(formulation->getArcLabel(arc,d));
        slice.push_back(formulation->getArcSlice(arc,d));   
        source.push_back(formulation->getNodeLabel((*formulation->getVecGraphD(d)).source(arc),d));
        target.push_back(formulation->getNodeLabel((*formulation->getVecGraphD(d)).target(arc),d));
        free_.push_back(true);
        lower.push_back(formulation->getArcLower(arc,d));
        arcs.push_back(arc);

        int size = label.size()-1;
        for(int i = 0; i < (size);i++){
            if(label[i] == label[size]){
                if(!((slice[size] < slice[i]-load+1)||(slice[size]-load+1 > slice[i]))){
                    /* The variables overlap */
                    infeasible = true;
                    if((free_[i] == true) && (free_[size]== true)){
                        if(lower[size] != 1){
                            (*freeArcs[d])[arc] = false;
                            free_[size] =false;
                        }else{
                            (*freeArcs[d])[arcs[i]] = false;
                            free_[i] =false;
                        }
                    }
                }
            }
            if(source[i]==source[size] || target[i]== target[size]){
                /* Infeasible for the source target constraints. */
                infeasible = true;
                if((free_[i] == true) && (free_[size]== true)){
                    if(lower[size] != 1){
                        (*freeArcs[d])[arc] = false;
                        free_[size] =false;
                    }else{
                        (*freeArcs[d])[arcs[i]] = false;
                        free_[i] =false;
                    }
                }      
            }
        }
    }
    label.clear(); slice.clear(); 
    source.clear(); target.clear(); 
    free_.clear(); lower.clear(); arcs.clear();
    return infeasible;
}

/* This function analyses the fixed variables values, searching for infeasibilities. */
bool shortestPathHeuristic::adaptedPreprocessing(){
    /* Analysing arcs with lower bound equals to 1. */
    /* We have to analyse if the fixed variables generate some infeasibility. */
    /* We could have source/target or non overlapping infeasibilities. */
    bool infeasible = false;
    std::vector<int> label; std::vector<int> slice; std::vector<int> load; // for the non overlapping.
    std::vector<int> source; std::vector<int> target; std::vector<int> demand; // for the source/target.
    for (int d = 0; d < formulation->getNbDemandsToBeRouted(); d++){
        for(IterableIntMap< ListDigraph, ListDigraph::Arc >::ItemIt arc((*mapItLower[d]),1); arc != INVALID; ++arc){
            label.push_back(formulation->getArcLabel(arc,d));
            slice.push_back(formulation->getArcSlice(arc,d));
            load.push_back(formulation->getToBeRouted_k(d).getLoad());
            source.push_back(formulation->getNodeLabel((*formulation->getVecGraphD(d)).source(arc),d));
            target.push_back(formulation->getNodeLabel((*formulation->getVecGraphD(d)).target(arc),d));
            demand.push_back(d);

            int size = label.size()-1;
            for(int i = 0; i < (size);i++){
                // If there are two fixed variables that overlaps.
                if(label[i] == label[size]){
                    if(!((slice[size] < slice[i]-load[i]+1)||(slice[size]-load[size]+1 > slice[i]))){
                        /* The fixed variables overlap */
                        infeasible = true;
                        label.clear(); slice.clear(); load.clear();
                        source.clear(); target.clear(); demand.clear();
                        return infeasible;
                    }
                }
                // If there are two fixed variables with same source or same target.
                if(demand[i]== demand[size]){
                    if(source[i]==source[size] || target[i]== target[size]){
                        /* Infeasible for the source target constraints. */
                        infeasible = true;
                        label.clear(); slice.clear(); load.clear();
                        source.clear(); target.clear(); demand.clear();
                        return infeasible;
                    }
                }
            }
            //std::cout << "Remove arcs for the fixed variables\n";
            /* Removing variables due to the non overlapping for the fixed bounds. */
            remove_arcs(slice[size],label[size],load[size]);
            (*freeArcs[d])[arc] = true;
        }
    }
    label.clear(); slice.clear(); load.clear();
    source.clear(); target.clear(); demand.clear();
    return infeasible;
}

/* Remove a found path for demand d. Then, we have to find another path for this demand*/
void shortestPathHeuristic::removePath_k(int d){
    
    /* For the true variables in demand's d path. */
    for(IterableBoolMap< ListDigraph, ListDigraph::Arc >::TrueIt arc((*heuristicSolutionItBoolMap[d])); arc != INVALID; ++arc){
        /* the path uses this arc */
        int index = formulation->getArcIndex(arc,d);
        /* removing path from the solution */
        heuristicSolution[d][index] = false; 
        (*heuristicSolutionItBoolMap[d])[arc] = false;
        /* Informations about the variable */
        int slice = formulation->getArcSlice(arc,d);
        int label = formulation->getArcLabel(arc,d);
        int load = formulation->getToBeRouted_k(d).getLoad();
        /* include the arcs were removed when this path was chosen */
        /* they were removed from the posibilities in order to find feasible solutions considering non overlap constraints*/
        include_arcs(slice,label,load); 
    }

    for (int d1 = 0; d1 < formulation->getNbDemandsToBeRouted(); d1++){
        /* Remove arcs that affects other arcs. */
        for(IterableBoolMap< ListDigraph, ListDigraph::Arc >::TrueIt arc((*heuristicSolutionItBoolMap[d1])); arc != INVALID; ++arc){
            int slice = formulation->getArcSlice(arc,d1);
            int label = formulation->getArcLabel(arc,d1);
            int load = formulation->getToBeRouted_k(d1).getLoad();
            remove_arcs(slice,label,load);
        }
        /* Remove arcs due to the fixed variables. */
        for(IterableIntMap< ListDigraph, ListDigraph::Arc >::ItemIt arc((*mapItLower[d1]),1); arc != INVALID; ++arc){
            int slice = formulation->getArcSlice(arc,d1);
            int label = formulation->getArcLabel(arc,d1);
            int load = formulation->getToBeRouted_k(d1).getLoad();
            remove_arcs(slice,label,load);
            (*freeArcs[d1])[arc] = true;
        }
    }
}

/* Changes the cost of some arcs so they can be chosen - respect the non overlapping constraints*/
/* For all arcs, considering every demand, that have @param  slice-load+1 <= slice_k <= slice @param label_k = label */
void shortestPathHeuristic::include_arcs(int slice, int label, int load){
    for (int k = 0; k < formulation->getNbDemandsToBeRouted(); k++){
        int load_k = formulation->getToBeRouted_k(k).getLoad();
        for(IterableIntMap< ListDigraph, ListDigraph::Arc >::ItemIt arc((*mapItLabel[k]),label); arc != INVALID; ++arc){
            int slice_k = formulation->getArcSlice(arc,k);
            if(!((slice_k < slice-load+1)||(slice_k-load_k+1 > slice))){
                (*freeArcs[k])[arc] = true;
            }
        }
    }
}

/* Changes the heuristic Solution considering the found path for demand d*/
void shortestPathHeuristic::insertPath_k(int d, CapacityScaling<FilterArcs<ListDigraph>,int,double> &costScale, const ListDigraph::Node &SOURCE, const ListDigraph::Node &TARGET){
    
    IterableValueMap<ListDigraph,ListDigraph::Arc,double> auxiliary(*formulation->getVecGraphD(d));
    costScale.flowMap(auxiliary);
    int flow = 1;

    for(IterableValueMap<ListDigraph,ListDigraph::Arc,double>::ItemIt arc(auxiliary,flow); arc != INVALID; ++arc){
        int index = formulation->getArcIndex(arc, d);
        heuristicSolution[d][index] = true;
        (*heuristicSolutionItBoolMap[d])[arc] = true;
        int slice = formulation->getArcSlice(arc,d);
        int label = formulation->getArcLabel(arc,d);
        int load = formulation->getToBeRouted_k(d).getLoad();
        remove_arcs(slice,label,load);
    }
    //std::cout << "Remove d:" << d+1 << std::endl;
}

/* Changes the heuristic Solution considering the found path for demand d*/
void shortestPathHeuristic::insertPath_k(int d, Dijkstra< FilterArcs<ListDigraph>, ListDigraph::ArcMap<double> > &path, const ListDigraph::Node &SOURCE, const ListDigraph::Node &TARGET){
    ListDigraph::Node currentNode = TARGET;
    while (currentNode != SOURCE){
        ListDigraph::Arc arc = path.predArc(currentNode);
        int index = formulation->getArcIndex(arc, d);
        heuristicSolution[d][index] = true;
        (*heuristicSolutionItBoolMap[d])[arc] = true;
        int slice = formulation->getArcSlice(arc,d);
        int label = formulation->getArcLabel(arc,d);
        int load = formulation->getToBeRouted_k(d).getLoad();
        remove_arcs(slice,label,load);
        currentNode = path.predNode(currentNode);
    }
}

/* Changes the heuristic Solution considering the found path for demand d*/
void shortestPathHeuristic::insertPath_k(int d, Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > &path, const ListDigraph::Node &SOURCE, const ListDigraph::Node &TARGET){
    ListDigraph::Node currentNode = TARGET;
    while (currentNode != SOURCE){
        ListDigraph::Arc arc = path.predArc(currentNode);
        int index = formulation->getArcIndex(arc, d);
        heuristicSolution[d][index] = true;
        (*heuristicSolutionItBoolMap[d])[arc] = true;
        int slice = formulation->getArcSlice(arc,d);
        int label = formulation->getArcLabel(arc,d);
        int load = formulation->getToBeRouted_k(d).getLoad();
        remove_arcs(slice,label,load);
        currentNode = path.predNode(currentNode);
    }
}

/* Changes the cost (to infinite) of some arcs so they will not be chosen - respect the non overlapping constraints*/
/* For all arcs, considering every demand, that have @param  slice-load+1 <= slice_k <= slice @param label_k = label */
void shortestPathHeuristic::remove_arcs(int slice, int label, int load){    
    for (int k = 0; k < formulation->getNbDemandsToBeRouted(); k++){
        int load_k = formulation->getToBeRouted_k(k).getLoad();
        for(IterableIntMap< ListDigraph, ListDigraph::Arc >::ItemIt arc((*mapItLabel[k]),label); arc != INVALID; ++arc){
            int slice_k = formulation->getArcSlice(arc,k);
            if(!((slice_k < slice-load+1)||(slice_k-load_k+1 > slice))){
                (*freeArcs[k])[arc] = false;
                if(formulation->getArcLower(arc,k)==1){
                    //std::cout << "Removing arc fixed to 1"<< std::endl;
                    //std::cout << "Analysing demand " << k+1 << " slice: " << slice + 1 << " label: " << label + 1 << "load: " << load+1 << std::endl;
                }
            }
        }
    }
}

/* "Remove" (cost infinite) arc with highest length, so  it can not be selected -> respect length constraints  */
void shortestPathHeuristic::remove_Arc(int d, Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > &path, const ListDigraph::Node &SOURCE, const ListDigraph::Node &TARGET){
    ListDigraph::Node currentNode = TARGET;
    ListDigraph::Arc arcHighestLength = path.predArc(currentNode);
    double length = formulation->getArcLength(arcHighestLength,d);
    while (currentNode != SOURCE){
        ListDigraph::Arc arc = path.predArc(currentNode);
        if(formulation->getArcLength(arc,d) > length){
            arcHighestLength = arc;
        }
        currentNode = path.predNode(currentNode);
    }
    (*freeArcs[d])[arcHighestLength] = false;
}

/* "Remove" (cost infinite) arc with highest length, so  it can not be selected -> respect length constraints  */
void shortestPathHeuristic::remove_Arc(int d, Dijkstra< FilterArcs<ListDigraph>, ListDigraph::ArcMap<double> > &path, const ListDigraph::Node &SOURCE, const ListDigraph::Node &TARGET){
    ListDigraph::Node currentNode = TARGET;
    ListDigraph::Arc arcHighestLength = path.predArc(currentNode);
    double length = formulation->getArcLength(arcHighestLength,d);
    while (currentNode != SOURCE){
        ListDigraph::Arc arc = path.predArc(currentNode);
        if(formulation->getArcLength(arc,d) > length){
            arcHighestLength = arc;
        }
        currentNode = path.predNode(currentNode);
    }
    (*freeArcs[d])[arcHighestLength] = false;
}

/* "Remove" (cost infinite) arc with highest length, so  it can not be selected -> respect length constraints  */
void shortestPathHeuristic::remove_Arc(int d, CapacityScaling<FilterArcs<ListDigraph>,int,double> &costScale, const ListDigraph::Node &SOURCE, const ListDigraph::Node &TARGET){
    
    IterableValueMap<ListDigraph,ListDigraph::Arc,double> auxiliary(*formulation->getVecGraphD(d));
    costScale.flowMap(auxiliary);
    int flow = 1;

    ListDigraph::Arc arcHighestLength;
    double length = 0.0;

    for(IterableValueMap<ListDigraph,ListDigraph::Arc,double>::ItemIt arc(auxiliary,flow); arc != INVALID; ++arc){
        if( (formulation->getArcLength(arc,d) > length) && (formulation->getArcLower(arc,d) != 1)){
            arcHighestLength = arc;
        }
    }
    (*freeArcs[d])[arcHighestLength] = false;    
}

/* Selects one demand to be analysed */
int shortestPathHeuristic::choseDemand(std::set<int> set){
    std::set<int>::iterator it;
    int load = 0;
    int demand = -1;
    for (it = set.begin(); it != set.end(); ++it){
        if(formulation->getToBeRouted_k(*it).getLoad() > load){
            load = formulation->getToBeRouted_k(*it).getLoad();
            demand = *it;
        }
    }
    return demand;
}

/* Returns the physical length of the path. */
double shortestPathHeuristic::getPathLength(int d, Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > &path, const ListDigraph::Node &s, const ListDigraph::Node &t){
    double pathLength = 0.0;
    ListDigraph::Node n = t;
    while (n != s){
        ListDigraph::Arc arc = path.predArc(n);
        n = path.predNode(n);
        pathLength += formulation->getArcLength(arc, d);
    }
    return pathLength;
}

/* Returns the physical length of the path. */
double shortestPathHeuristic::getPathLength(int d, Dijkstra< FilterArcs<ListDigraph>, ListDigraph::ArcMap<double> > &path, const ListDigraph::Node &s, const ListDigraph::Node &t){
    double pathLength = 0.0;
    ListDigraph::Node n = t;
    while (n != s){
        ListDigraph::Arc arc = path.predArc(n);
        n = path.predNode(n);
        pathLength += formulation->getArcLength(arc, d);
    }
    return pathLength;
}

/* Returns the physical length of the path. */
double shortestPathHeuristic::getPathLength(int d, CapacityScaling<FilterArcs<ListDigraph>,int,double> &costScale){
    
    IterableValueMap<ListDigraph,ListDigraph::Arc,double> auxiliary(*formulation->getVecGraphD(d));
    costScale.flowMap(auxiliary);
    int flow = 1;

    double pathLength = 0.0;

    for(IterableValueMap<ListDigraph,ListDigraph::Arc,double>::ItemIt arc(auxiliary,flow); arc != INVALID; ++arc){
        pathLength += formulation->getArcLength(arc, d);
    }
    return pathLength;
}

/* *******************************************************************************
*                              DESTRUCTOR
******************************************************************************* */

shortestPathHeuristic::~shortestPathHeuristic(){
    heuristicCosts.clear();
    freeArcs.clear();
    mapItLabel.clear();
    heuristicSolutionItBoolMap.clear();
    heuristicSolution.clear();

    while(!notAnalysedDemands.empty()){
        std::set<int>::iterator it;
        it = std::prev(notAnalysedDemands.end()); 
        
        notAnalysedDemands.erase(it);
    }
    notAnalysedDemands.clear();
    while(!analysedDemands.empty()){
        std::set<int>::iterator it;
        it = std::prev(analysedDemands.end()); 
        analysedDemands.erase(it);
    }
    analysedDemands.clear();
}