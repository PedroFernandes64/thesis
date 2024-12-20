#ifndef __GENETIC__h
#define __GENETIC__h

#include "../topology/instance.h"
#include "../tools/clockTime.h"


using namespace lemon;

class Routing{  
public:
    std::vector<std::vector<Fiber> > routes;
    std::vector<int> loads;
    std::vector<int> colors;
    bool colored;
    bool feasible;
    int nbEdges;
    int nbSlots;
    int metricVal;

    Routing(std::vector<std::vector<Fiber> > routes, std::vector<int> loads);
    Routing(const Routing& other)
        : routes(other.routes),         // Deep copy the vector of vectors
          loads(other.loads),           // Deep copy the loads vector
          colors(other.colors),         // Deep copy the colors vector
          colored(other.colored),       // Copy the boolean value
          feasible(other.feasible),     // Copy the boolean value
          nbEdges(other.nbEdges),       // Copy the integer value
          nbSlots(other.nbSlots),       // Copy the integer value
          metricVal(other.metricVal) {} // Copy the integer value

    void building(int metr, int edges, int slots);
    void copying(Routing r);
    void computeMetric(int m);
    void setNbEdges(int nb);
    void setNbSlots(int nb);
    bool isColored() const {return colored;}
    bool tryColoring(int prio);
    void copyColoring(std::vector<int> c);
    void display();
    void setMetric(double m);
    ~Routing();
};

//lowest first
struct sortRouting{
    bool operator() (Routing a, Routing b){return (a.metricVal<b.metricVal);}
};

struct sequenciator{
    int id;
    int criteria;
};
//highest first
struct sortSequenciatorHigher{
    bool operator() (sequenciator a, sequenciator b){return (a.criteria>b.criteria);}
};
struct prePath{
    std::vector<Fiber> links;
    double length;
    double noise;
    int nbEdges;
};
//lowest first
struct sortSequenciatorLower{
    bool operator() (sequenciator a, sequenciator b){return (a.criteria<b.criteria);}
};


struct sortPrePath{
    bool operator() (prePath a, prePath b){return (a.nbEdges<b.nbEdges);}
};

class Genetic{

protected: 

    Instance instance;                  /**< An instance describing the initial mapping. **/
    std::vector<Demand> toBeRouted;     /**< The list of demands to be routed in the next optimization. **/
    std::vector<int> loads;

    std::vector<std::vector<int>> edgeSlotMap;
    std::vector<int> lastSlotDemand;
    std::vector<std::vector<int>> nodesByDemand;
    
    std::vector<std::vector<int> > adj_list;
    std::vector<std::vector<int> >  pathsdemand;
    std::vector<std::vector<std::vector<int> > > allpaths;

    std::vector<std::vector<std::vector<Fiber> > > shortestRoutesByDemand;
    int chosenK;
    int extraK;
    int metric;

    int nbInitialPop;
    std::vector<Routing> population;
    std::vector<Routing> thisIterationCrossing;
    std::vector<Routing> thisIterationMutation;
    std::vector<Routing> thisIterationTotalPop;
    std::vector<Routing> thisIterationSelected;

    double heuristicTime;
    double coloringTime;
    double kShortestTime;
    double initialPopTime;
    double crossingTime;
    double mutationTime;
    double consolidatingSelectedTime;
    double selectionTime;

public:
	/****************************************************************************************/
	/*										Constructor										*/
	/****************************************************************************************/

    /** Constructor. A graph associated with the initial mapping (instance) is built as well as an extended graph for each demand to be routed. **/
    Genetic(const Instance &instance);

	/****************************************************************************************/
	/*										Getters 										*/
	/****************************************************************************************/
    /** Returns the input instance. **/
    Instance getInstance() const{ return instance; }

    /** Returns the vector of demands to be routed. **/
    std::vector<Demand> getToBeRouted() { return toBeRouted; } 
        
    /** Returns the i-th demand to be routed. @param k The index of the required demand. **/
    Demand getToBeRouted_k(int k) const { return toBeRouted[k]; }

    /** Returns the number of demands to be routed. **/
    int getNbDemandsToBeRouted() const { return toBeRouted.size(); }

	/****************************************************************************************/
	/*										Setters											*/
	/****************************************************************************************/
    

    /** Changes the vector of demands to be routed. @param vec The new vector of demands. **/
    void setToBeRouted(const std::vector<Demand> &vec){this->toBeRouted = vec;}
    void setLoadVector();


	/****************************************************************************************/
	/*										Methods											*/
	/****************************************************************************************/

    void GenerateShortestRoutes();
    void GenerateShortestRoutes2();
    
    void AllPathsUtil(int u, int d, bool visited[], int path[], int& path_index);
    double osnrPath(double n, double pc);
    std::vector<std::vector<Fiber> > kdijkstra(std::vector<std::vector<int> > graph, int src, int dest, int K);
    int minDistance(std::vector<int> dist, std::vector<bool> sptSet);
    std::vector<int> dijkstra(std::vector<std::vector<int> > graph, int src, int dest, double & pathdistance);
    
    void GenerateInitialPopulation(int nbPop);
    void doCrossing();
    void doMutation();
	void doSelection();

    std::vector<std::vector<int> > buildMatrixKsol(int k);
    std::vector<std::vector<int> > buildPathNodesByDemand(int k);
    std::vector<int> buildLastSlotByDemand(int k);

	/****************************************************************************************/
	/*										Display											*/
	/****************************************************************************************/

    /** Displays the demands to be routed in the next optimization. **/
    void displayToBeRouted();
    void displayShortestRoutes();

	/****************************************************************************************/
	/*										Destructor										*/
	/****************************************************************************************/

	/** Destructor. Clears the spectrum. **/
	~Genetic();


};
#endif