#ifndef __GENETIC__h
#define __GENETIC__h

#include "../topology/instance.h"
#include "../tools/clockTime.h"


using namespace lemon;

class Routing{  
public:
    std::vector<std::vector<Fiber> > routes;
    std::vector<std::vector<int> >colors;
    bool colored;
    double metric;

    Routing(std::vector<std::vector<Fiber> > routes);
    bool isColored() const {return colored;}
    bool tryColoring();
    void display();
    void setMetric(double m);
};


class Genetic{

protected: 

    Instance instance;                  /**< An instance describing the initial mapping. **/
    std::vector<Demand> toBeRouted;     /**< The list of demands to be routed in the next optimization. **/
    
    std::vector<std::vector<std::vector<Fiber> > > shortestRoutesByDemand;
    int chosenK;
    int extraK;

    int nbInitialPop;
    std::vector<Routing> initalPopulation;


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


	/****************************************************************************************/
	/*										Methods											*/
	/****************************************************************************************/

    void GenerateShortestRoutes();
    
    double osnrPath(double n, double pc);
    std::vector<std::vector<Fiber> > kdijkstra(std::vector<std::vector<int> > graph, int src, int dest, int K);
    int minDistance(std::vector<int> dist, std::vector<bool> sptSet);
    std::vector<int> dijkstra(std::vector<std::vector<int> > graph, int src, int dest, double & pathdistance);
    
    void GenerateInitialPopulation(int nbPop);

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