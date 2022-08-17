#ifndef SHORT_PATH_HEURISTIC
#define SHORT_PATH_HEURISTIC

#include "AbstractHeuristic.h"
#include <lemon/adaptors.h>

class shortestPathHeuristic : public AbstractHeuristic{

    private:
    
        /* A list of pointers to the Map Arc storing the COSTS used in the Heuristic Shortest Path */
        std::vector<std::shared_ptr<ArcCost>> heuristicCosts;

        /* A list of pointers to the bool Map Arc storing if the arc is used in the Heuristic Shortest Path */
        std::vector<std::shared_ptr<ListDigraph::ArcMap<bool>>> freeArcs;

        std::vector<std::shared_ptr<IterableIntMap<ListDigraph, ListDigraph::Arc>>> mapItLabel; 

        std::vector<std::shared_ptr<IterableIntMap<ListDigraph, ListDigraph::Arc>>> mapItLower;

        /* Creating auxiliary sets with the analysed and not analysed demands */
        std::set<int> notAnalysedDemands;
        std::set<int> analysedDemands;

    public:

        /* *******************************************************************************
        *                              CONSTRUCTOR
        ******************************************************************************* */

        shortestPathHeuristic(AbstractLagFormulation* form);

        /* *******************************************************************************
        *                              INITIALIZATION
        ******************************************************************************* */

        /* Initializes the Heuristics elements to run the algorithm */
        void init(bool=true);

        /* Initialize the sets of not analysed demands and analysed demands for the Heuristic - to run the algorithm*/
        void initDemandsSets();

        /* Updates the heuristic costs using the assignment matrix */
        void initCosts();

        void initAdaptedCosts(const double *);

        /* *******************************************************************************
        *                              RUNNING METHODS
        ******************************************************************************* */

        void run(bool=false,bool=true);

        /* Selects one demand to be analysed */
        int choseDemand(std::set<int>);

        bool adaptedPreprocessing();

        bool solutionInfeasible(int,CapacityScaling<FilterArcs<ListDigraph>,int,double> &costScale);

        /* Find a shortest path for demand d*/
        bool heuristicRun(int);

        bool heuristicAdaptedRun(int);

        /* Changes the heuristic Solutin including the found path to demand d */
        void insertPath_k(int, Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > &, const ListDigraph::Node &, const ListDigraph::Node &);

        void insertPath_k(int d, Dijkstra< FilterArcs<ListDigraph>, ListDigraph::ArcMap<double> > &, const ListDigraph::Node &, const ListDigraph::Node &);

        void insertPath_k(int d, CapacityScaling<FilterArcs<ListDigraph>,int,double> &path, const ListDigraph::Node &SOURCE, const ListDigraph::Node &TARGET);

        /* Remove a found path for demand d. Then, we have to find another path for this demand*/
        void removePath_k(int);

        /* Changes the cost of some arcs so they will not be chosen - respect the non overlapping constraints*/
        void remove_arcs(int,int,int);

        /* Changes the cost of some arcs so they can be chosen - respect the non overlapping constraints*/
        /* Changes it for the original cost */
        void include_arcs(int,int,int);

        /* "Remove" (cost infinite) arc with highest length, so  it can not be selected -> respect length constraints  */
        void remove_Arc(int, Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > &, const ListDigraph::Node &, const ListDigraph::Node &);

        void remove_Arc(int, Dijkstra< FilterArcs<ListDigraph>, ListDigraph::ArcMap<double> > &, const ListDigraph::Node &, const ListDigraph::Node &);

        void remove_Arc(int d, CapacityScaling<FilterArcs<ListDigraph>,int,double> &, const ListDigraph::Node &, const ListDigraph::Node &);
 
        /* Returns the physical length of the path. */
        double getPathLength(int d, Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > &path, const ListDigraph::Node &s, const ListDigraph::Node &t);

        double getPathLength(int d, Dijkstra< FilterArcs<ListDigraph>, ListDigraph::ArcMap<double> > &path, const ListDigraph::Node &s, const ListDigraph::Node &t);

        double getPathLength(int d, CapacityScaling<FilterArcs<ListDigraph>,int,double> &costScale);


        //ListDigraph::Node getNodeFromIndex2(int, int);

        /* *******************************************************************************
        *                                   DESTRUCTOR
        ******************************************************************************* */

        ~shortestPathHeuristic();

};

#endif