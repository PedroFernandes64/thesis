#ifndef LAG_TOOLS_H
#define LAG_TOOLS_H

#include <lemon/list_graph.h>
#include <lemon/concepts/graph.h>
#include <lemon/dijkstra.h>
#include <lemon/bellman_ford.h>
#include <lemon/list_graph.h>
#include <lemon/concepts/graph.h>
#include <lemon/maps.h>
#include <lemon/concepts/maps.h>
#include "../../topology/input.h"
#include "../../formulation/rsa.h"
#include "../../formulation/flowForm.h"


using namespace lemon;

/* Auxiliary Functors antecipated definition */
class operatorCost;
class operatorCostObj8;
class operatorCoefficient;
class operatorSliceCoefficient;
class operatorCostEFlow;
class operatorCostESource;
class operatorCostETarget;
class operatorCostELength;
class operatorCostEOneSlicePerDemand;
class operatorCostHeuristicRemoveArcs;
template <typename T> class operatorHeuristicCost;
template <typename T> class operatorArcCostArcIndex;
class operatorLowerUpperBound;
class operatorHeuristicAdaptedCost;

/* Typedef Auxiliary Functors */
typedef operatorHeuristicCost<bool>   HeuristicCostAssign;
typedef operatorHeuristicCost<double> HeuristicCostVar;
typedef operatorArcCostArcIndex<bool>   operatorArcCostArcIndexAssign;
typedef operatorArcCostArcIndex<double> operatorArcCostArcIndexVar;

/* Typedef Maps Adaptors */

/* Coefficient computation */
typedef CombineMap<ArcMap,SourceMap<ListDigraph>,operatorSliceCoefficient,int> CombineMapCoeffSlice;
typedef CombineMap<ArcCost,CombineMapCoeffSlice,operatorCoefficient,double> CombineMapCoeff;

/* Cost flow formulation */
typedef ScaleMap< ArcCost, double > ScaleMapCost;
typedef AddMap<ArcCost,ScaleMap<ArcCost, double>> AddMapCost;
typedef CombineMap<ArcMap,ArcMap,operatorCost,double> CombineMapCost;
typedef AddMap<CombineMapCost,AddMapCost> AddMapFinalCost;
typedef CombineMap<ArcMap,SourceMap<ListDigraph>,operatorCostObj8,double> CombineMapCostObj8;
typedef AddMap<AddMapFinalCost,CombineMapCostObj8> AddMapFinalCostObj8;

/* Cost  heuristic */
typedef CombineMap<ArcCost,ArcMap,HeuristicCostAssign,double > CombineHeuristicCostAssign;
typedef CombineMap<ArcCost,ArcMap,HeuristicCostVar,double >    CombineHeuristicCostPrimal;

typedef CombineMap<ArcMap,ArcMap,operatorCostHeuristicRemoveArcs,double> CombineCostHeuristicRemoveArcs;
typedef AddMap<CombineCostHeuristicRemoveArcs,ArcCost> AddMapCostHeuristicRemoveArcs;

typedef CombineMap<ArcCost,ArcMap,operatorArcCostArcIndexAssign,double> CombineArcCostArcIndexAssign;
typedef CombineMap<ArcCost,ArcMap,operatorArcCostArcIndexVar,double>    CombineArcCostArcIndexVar;

/* Cost  non overlapping formulation */
typedef CombineMap<ArcMap,ArcMap,operatorCostEFlow,double> CombineArcMapArcMapCostEFlow;
typedef CombineMap<ArcMap,ArcMap,operatorCostESource,double> CombineArcMapArcMapCostESource;
typedef CombineMap<ArcMap,ArcMap,operatorCostETarget,double> CombineArcMapArcMapCostETarget;
typedef CombineMap<ArcMap,ArcCost,operatorCostELength,double> CombineArcMapArcMapCostELength;

typedef CombineMap<ArcMap,ArcMap,operatorCostEOneSlicePerDemand,double> CombineArcMapArcMapCostEOneSlicePerDemand;

typedef AddMap<CombineArcMapArcMapCostEFlow,CombineArcMapArcMapCostEFlow> AddMapFlow;
typedef AddMap<CombineArcMapArcMapCostESource,CombineArcMapArcMapCostETarget> AddMapST;
typedef AddMap<CombineArcMapArcMapCostELength,ArcCost> AddMapLength;
typedef AddMap<AddMapFlow,AddMapST> AddMapFlowST;
typedef AddMap<AddMapFlowST,AddMapLength> AddMappFinal;
typedef AddMap<AddMappFinal,CombineArcMapArcMapCostEOneSlicePerDemand> AddMapFinalOneSlicePerDemand;

/* Upper and lower bound */
typedef CombineMap<ArcMap,ArcMap,operatorLowerUpperBound,int> CombineArcMapArcMapLowerUpperBound;

typedef CombineMap<ArcMap,ArcCost,operatorHeuristicAdaptedCost,double> CombineArcMapArcCostHeuristicAdaptedCost;

/* Auxiliaries */
typedef Dijkstra< ListDigraph, AddMapFinalCost > DijkstraCost;
typedef Dijkstra< ListDigraph, AddMapFinalCostObj8> DijkstraCostObj8;

typedef Dijkstra<ListDigraph,AddMapFinalOneSlicePerDemand> DijkstraCostE;
typedef BellmanFord<ListDigraph,AddMapFinalOneSlicePerDemand> BellmanFordCostE;
typedef ArcCost::MapIt ArcCostIt;

/* Class to compute the cost related with Non Overlap Multipliers.
 *  -> Used in the CombineMap class to return a new map with the cost 
 *  related to Overlap Multipliers*/
class operatorCost{
    private:
        int demandLoad;
        std::vector<std::vector<double>> lagrangianMultiplierOverlap;
                
    public:
        operatorCost(std::vector<std::vector<double>> lag);
        void setDemandLoad(int load){demandLoad = load;}
        double getDemandLoad() const {return demandLoad;}       
        double operator() (int label,int slice) const;
        ~operatorCost(){ lagrangianMultiplierOverlap.clear();}
};

/* Class to compute the cost related with objective 8 constraints Multipliers.
 *  -> Used in the CombineMap class to return a new map with the cost 
 *  related to objective 8 constraints Multipliers*/
class operatorCostObj8{
    private:
        ListDigraph::Node source;
        double multiplier;
        double multiplierAux;
    public:
        operatorCostObj8(ListDigraph::Node node,double mult = 0.0, double multAux = 0.0):source(node),multiplier(mult),multiplierAux(multAux){}
        void setMultiplier(double value) { multiplier = value; }
        void setMultiplierAux(double value) { multiplierAux = value; }
        double operator()(int,ListDigraph::Node) const;
};

/*********************************************** COMPUTING COEFF MAP ***********************************************/
/* Class to compute the arcs coefficient in the objective function.
 *   It combines the maps vecArcLength and vecArcSlice (an adaptation where
 * only arcs leaving the source have values != from 0 - see operatorSliceCoefficient).
 *  -> Used in the CombineMap class to return a new map with the arcs coefficients values */
class operatorCoefficient{
    private:
        int demandLoad;
        Input::ObjectiveMetric chosenObj; 
    public:
        operatorCoefficient(Input::ObjectiveMetric obj):chosenObj(obj){}
        void setDemandLoad(double load) { demandLoad = load; }
        double operator()(double,int) const;
};

/** Class to compute the arcs slices for the objective function.
 *  Arcs leaving the source maintain their slice, the others have their slice
 *  changed to 0. */
class operatorSliceCoefficient{
    private:
        ListDigraph::Node source;
    public:
        operatorSliceCoefficient(){}
        void setSource(const ListDigraph::Node & n) { source = n;}
        int operator()(int,ListDigraph::Node) const;
};

/******************************************* COMPUTING HEURISTIC COST******************************************************/
/** Class to compute the arcs cost for the shortest path heuristic.
 *  It combines the vecArcIndex with the coeff. **/
template <typename T>
class operatorHeuristicCost{
    private:
        std::vector<std::vector<T> > variables;
        int demand;
    public:
        operatorHeuristicCost(std::vector<std::vector<T> >);
        void setDemand(int d){ demand = d;}
        double operator()(double,int) const;
        ~operatorHeuristicCost();
};

/** Initilialization operatorHeuristicCost class : it has as attribut a vector that is either the assignment matrix 
 * or the primal variables. **/
template <typename T>
operatorHeuristicCost<T>::operatorHeuristicCost(std::vector<std::vector<T> > var){
    variables.resize(var.size());
    for(int i = 0; i < var.size();i++){
        std::copy(var[i].begin(), var[i].end(),std::back_inserter(variables[i]));
    } 
}
/** Operator for the cost : (1-x)*coeff **/
template <typename T>
double operatorHeuristicCost<T>::operator()(double coeff,int index) const{
    return coeff*(1.0-variables[demand][index]);
    //return (1.0-variables[demand][index]);
}

template <typename T>
operatorHeuristicCost<T>::~operatorHeuristicCost(){ variables.clear(); }


class operatorCostHeuristicRemoveArcs{
    private:
        int load;
        int slice;
        int label;
        int load_k;
    public:
        operatorCostHeuristicRemoveArcs(int load_, int slice_, int label_): load(load_), slice(slice_), label(label_){}
        void setLoadK(int val){ load_k = val;}
        double operator()(int slice_k, int label_k) const{
            if((!((slice_k < slice-load+1)||(slice_k-load_k+1 > slice))) && (label_k == label)){
                return __DBL_MAX__;
            }
            return 0.0;
        }

};

/**********************************************************************************************************************/

template <typename T>
class operatorArcCostArcIndex{
    private:
        std::vector<std::vector<T> > variables;
        int demand;
    public:
        operatorArcCostArcIndex(std::vector<std::vector<T> >);
        void setDemand(int d){ demand = d;}
        double operator()(double,int) const;
        ~operatorArcCostArcIndex();
};

template <typename T>
operatorArcCostArcIndex<T>::operatorArcCostArcIndex(std::vector<std::vector<T> > var){
    variables.resize(var.size());
    for(int i = 0; i < var.size();i++){
        std::copy(var[i].begin(), var[i].end(),std::back_inserter(variables[i]));
    } 
}

template <typename T>
double operatorArcCostArcIndex<T>::operator()(double cost,int index) const{
    return cost*(variables[demand][index]);
}

template <typename T>
operatorArcCostArcIndex<T>::~operatorArcCostArcIndex(){ variables.clear(); }


/**************************************************** COMPUTING NON OVERLAP COST ************************************************/

class operatorCostEFlow{
    private:
        std::vector<std::vector<double> > multiplier;
        int signal;
    public:
        operatorCostEFlow(std::vector<std::vector<double> >);
        void setSignal(int s) {signal = s;}
        double operator()(int, int) const;

};

class operatorCostETarget{
    private:
        std::vector<std::vector<double> > multiplier;
        std::vector<int> labelTarget;
    public:
        operatorCostETarget(std::vector<std::vector<double> >,std::vector<int>);
        double operator()(int, int) const;
};

class operatorCostESource{
    private:
        std::vector<std::vector<double> > multiplier;
        std::vector<int> labelSource;
    public:
        operatorCostESource(std::vector<std::vector<double> >,std::vector<int>);
        double operator()(int, int) const;
};

class operatorCostELength{
    private:
        std::vector<double>  multiplier;
        std::vector<double> maxLength;
    public:
        operatorCostELength(std::vector<double> ,std::vector<double>);
        double operator()(int, double) const;
};

class operatorCostEOneSlicePerDemand{
    private:
        int label;
        std::vector<std::vector<double> > multiplier;
    public:
        operatorCostEOneSlicePerDemand(std::vector<std::vector<double>>);
        void setLabel(int val) { label = val;}
        double operator()(int,int) const;
};

/**********************************************************************************************************************/

class operatorLowerUpperBound{
    private:
        int d; /*demand*/
        double * bound;
    public:
        operatorLowerUpperBound(double*);
        void setDemand(int val) { d = val;}
        int operator()(int,int) const;
};


class operatorHeuristicAdaptedCost{
    private:
        int d; /*demand*/
        const double * bound;
    public:
        operatorHeuristicAdaptedCost(const double*);
        void setDemand(int val) { d = val;}
        double operator()(int,double) const;
};

#endif