#include "AbstractHeuristic.h"

/* *******************************************************************************
*                              CONSTRUCTOR
******************************************************************************* */

AbstractHeuristic::AbstractHeuristic(AbstractLagFormulation* form, const Status &s):formulation(form),statusheuristic(s),time(ClockTime::getTimeNow()){
    /* Initializing solution */
    heuristicSolution.resize(formulation->getNbDemandsToBeRouted());
    for (int d = 0; d < formulation->getNbDemandsToBeRouted(); d++){
        heuristicSolution[d].resize(countArcs(*formulation->getVecGraphD(d)),false);
        heuristicSolutionItBoolMap.emplace_back(std::make_shared<IterableBoolMap<ListDigraph, ListDigraph::Arc>>((*formulation->getVecGraphD(d)),false)); 
        //mapFill<ListDigraph,IterableBoolMap<ListDigraph,ListDigraph::Arc>>((*formulation->getVecGraphD(d)),(*heuristicSolutionItBoolMap[d]),false);
    } 
    timeAux = 0.0;
    infeasibleByBound = false;
}

/* *******************************************************************************
*                              INITIALIZATION
******************************************************************************* */

/*Initializes the  heuristic Solution - to 0 for every element - to run the algorithm*/
void AbstractHeuristic::initSolution(){  
    for (int d = 0; d < formulation->getNbDemandsToBeRouted(); d++){
        std::fill(heuristicSolution[d].begin(), heuristicSolution[d].end(), false);
        mapFill<ListDigraph,IterableBoolMap<ListDigraph,ListDigraph::Arc>>((*formulation->getVecGraphD(d)),(*heuristicSolutionItBoolMap[d]),false);
    }
}

void AbstractHeuristic::updateCostFromHeuristic(){
    setCurrentHeuristicCost(0.0);
    if(maxChangesPossible || statusheuristic == STATUS_INFEASIBLE){
        setCurrentHeuristicCost(__DBL_MAX__);
    }
    else{
        Input::ObjectiveMetric chosenMetric = formulation->getInstance().getInput().getChosenObj_k(0);
        if(chosenMetric == Input::OBJECTIVE_METRIC_8){
            double val = 0.0;
            for (int d = 0; d < formulation->getNbDemandsToBeRouted(); d++){
                for (ListDigraph::ArcIt a(*formulation->getVecGraphD(d)); a != INVALID; ++a){
                    int index = formulation->getArcIndex(a,d);
                    if(heuristicSolution[d][index] == true && formulation->getCoeff(a,d) > val){
                        val = formulation->getCoeff(a,d);  
                    }
                }
            }
            incCurrentHeuristicCost(val);
            varP = val;
        }else{
            std::vector<std::vector<double>> overlapSlack;
            std::vector<double> lengthslack;
            std::vector<std::vector<double>> sourceSlack;
            lengthslack.resize(formulation->getNbDemandsToBeRouted(),0);
            overlapSlack.resize(formulation->getInstance().getNbEdges());
            sourceSlack.resize(formulation->getNbDemandsToBeRouted());
            for(int i=0;i<formulation->getInstance().getNbEdges();i++){
                overlapSlack[i].resize(formulation->getInstance().getPhysicalLinkFromIndex(i).getNbSlices(),0);
            }
            for(int i=0;i<formulation->getNbDemandsToBeRouted();i++){
                sourceSlack[i].resize(formulation->getInstance().getNbNodes(),0);
            }
            for (int d = 0; d < formulation->getNbDemandsToBeRouted(); d++){
                for (ListDigraph::ArcIt a(*formulation->getVecGraphD(d)); a != INVALID; ++a){
                    int index = formulation->getArcIndex(a,d);
                    int source = formulation->getNodeLabel((*formulation->getVecGraphD(d)).source(a),d);
                    if(heuristicSolution[d][index] == true){
                        double val = formulation->getCoeff(a,d);
                        incCurrentHeuristicCost(val);

                        double length = formulation->getArcLength(a,d);
                        lengthslack[d] += length;

                        int label = formulation->getArcLabel(a,d);
                        int slice = formulation->getArcSlice(a,d);
                        int load = formulation->getToBeRouted_k(d).getLoad();

                        for(int s = slice-load+1;s<=slice;s++){
                            overlapSlack[label][s] = overlapSlack[label][s]+1;
                        }
                        sourceSlack[d][source] = sourceSlack[d][source] +1;

                    }
                }
            }  
            bool disp =false;
            for (int d = 0; d < formulation->getNbDemandsToBeRouted(); d++){
                double maxlength = formulation->getToBeRouted_k(d).getMaxLength();
                if(lengthslack[d] > maxlength){
                    std::cout << "Heuristic found infeasible solution: length " << std::endl;
                    std::cout << "d: " << d+1 << " length: " << lengthslack[d] << " maxlength: " << maxlength << std::endl;
                    setCurrentHeuristicCost(__DBL_MAX__);
                    disp = true;
                } 
                for(int j =0; j< formulation->getInstance().getNbNodes();j++){
                    if(sourceSlack[d][j]>1){
                        std::cout << "Heuristic found infeasible solution: source " << d+1 << " " << j+1 << std::endl;
                        disp = true;
                        setCurrentHeuristicCost(__DBL_MAX__);
                    }
                    if(j==formulation->getToBeRouted_k(d).getSource()){
                        if(sourceSlack[d][j]==0){
                            std::cout << "Heuristic found infeasible solution: source (node source)" << d+1 << " " << j+1 << std::endl;
                            disp = true;
                            setCurrentHeuristicCost(__DBL_MAX__);
                        }
                    }
                }
                
            }
            for(int i=0;i<formulation->getInstance().getNbEdges();i++){
                for(int j =0; j< formulation->getInstance().getPhysicalLinkFromIndex(i).getNbSlices();j++){
                    if(overlapSlack[i][j]>1){
                        std::cout << "Heuristic found infeasible solution: overlap " << i+1 << " " << j+1 << std::endl;
                        disp = true;
                        setCurrentHeuristicCost(__DBL_MAX__);
                    }
                }
            }
            if(disp){
                display();
            }

            sourceSlack.clear();
            overlapSlack.clear();
            lengthslack.clear();
        }
    }
}

void AbstractHeuristic::display(){
    for (int d = 0; d < formulation->getNbDemandsToBeRouted(); d++){
        std::cout << "For demand " << formulation->getToBeRouted_k(d).getId() + 1 << " : " << "Load = " << formulation->getToBeRouted_k(d).getLoad() << " : " << std::endl;
        for (ListDigraph::ArcIt a(*formulation->getVecGraphD(d)); a != INVALID; ++a){
            int index = formulation->getArcIndex(a,d);
            if(heuristicSolution[d][index]){
                std::cout << "(" << formulation->getNodeLabel((*formulation->getVecGraphD(d)).source(a), d) + 1;
                std::cout << "--";
                std::cout <<  formulation->getNodeLabel((*formulation->getVecGraphD(d)).target(a), d) + 1 << ", " << formulation->getArcSlice(a, d) + 1 << ")" << std::endl;
            }
                
        }
        std::cout << std::endl;
    }
        
}

double * AbstractHeuristic::getAdaptedSolution(){
    int nb_var = 0;
    for (int d = 0; d < formulation->getNbDemandsToBeRouted(); d++){
        nb_var += countArcs(*formulation->getVecGraphD(d));
    }
    if(formulation->getInstance().getInput().isObj8(0)){
        nb_var++;
    }

    double *sol = new double[nb_var];

    double p =0;
    for (int d = 0; d < formulation->getNbDemandsToBeRouted(); d++){
        for (ListDigraph::ArcIt a(*formulation->getVecGraphD(d)); a != INVALID; ++a){
            int varId = formulation->getVarId(a,d);
            int index = formulation->getArcIndex(a,d);
            int slice = formulation->getArcSlice(a,d);
            if(slice > p){
                p = slice;
            }
            sol[varId] = heuristicSolution[d][index];
        }
    }
    if(formulation->getInstance().getInput().isObj8(0)){
        sol[nb_var-1] = p;
    }
    return sol;
}

void AbstractHeuristic::printSolution(){
    int somaslice = 0;
    for (int d = 0; d < formulation->getNbDemandsToBeRouted(); d++){
        for (ListDigraph::ArcIt a(*formulation->getVecGraphD(d)); a != INVALID; ++a){
            int index = formulation->getArcIndex(a,d);
            if(heuristicSolution[d][index] == true){
                std::cout << "label: " << formulation->getArcLabel(a,d)+1 << " slice:" << formulation->getArcSlice(a,d) << " demand: " << d+1 << " load" << formulation->getToBeRouted_k(d).getLoad()<< std::endl;
                somaslice += formulation->getArcSlice(a,d);
            }
        }
    }
    std::cout<< "soma: " << somaslice << std::endl;

}

AbstractHeuristic::~AbstractHeuristic(){
    heuristicSolution.clear();
}


