#ifndef __Instance__h
#define __Instance__h

// PEDRO PEDRO PEDRO
#include <lemon/list_graph.h>
#include <lemon/concepts/graph.h>
#include <lemon/dijkstra.h>
#include <lemon/list_graph.h>
#include <lemon/concepts/graph.h>
using namespace lemon;

#include <float.h>
#include <math.h>

#include "demand.h"
#include "input.h"
#include "physicalLink.h"
#include "../tools/CSVReader.h"

/********************************************************************************************
 * This class stores the initial mapping that serves as input for the Online Routing and 
 * Spectrum Allocation problem. This consists of a topology graph where some slices of some 
 * edges are already occupied by some given demands.												
********************************************************************************************/
class Instance {
public:
	
	/** Enumerates the possible output policies to be used. **/
	enum Metric {
		METRIC_ONE = 0,			/**< Sum over demands of last slot used for each demand. **/
		METRIC_ONE_P = 1,		/**< Sum over edges of the last slot used on each edge. **/
		METRIC_TWO = 2,			/**< Sum of hops. **/
		METRIC_FOUR = 4,		/**< Sum of path lengths. **/
		METRIC_EIGHT = 8		/**< Last slot used overall. **/
	};
private:
	Input input;						/**< An instance needs an input. **/
	int nbNodes;						/**< Number of nodes in the physical network. **/
	std::vector<Fiber> tabEdge;			/**< A set of Fiber. **/
	std::vector<Demand> tabDemand;		/**< A set of Demand (already routed or not). **/
	int nbInitialDemands;				/**< The number of demands routed in the first initial mapping. **/
	int nextDemandToBeRoutedIndex;		/**< Stores the index of the next demand to be analyzed in tabDemand. **/
	bool wasBlocked;

	// PEDRO PEDRO PEDRO
	// Stores the value of the Pase noise of a node amplifier
	double paseNodeC;
	double paseNodeL;
	double paseNodeS;

public:

	/****************************************************************************************/
	/*										Constructor										*/
	/****************************************************************************************/

	/** Constructor initializes the object with the information of an Input. @param i The input used for creating the instance.**/
	Instance(const Input &i);

	/** Copy constructor. @param i The instance to be copied. **/
	Instance(const Instance & i);

	/****************************************************************************************/
	/*										Getters											*/
	/****************************************************************************************/
	
	bool getWasBlocked() const { return wasBlocked; }

	/** Returns the total number of demands. **/
	int getNbDemands() const { return (int)this->tabDemand.size(); }

	/** Returns the number of demands already routed. **/
	int getNbRoutedDemands() const;	

	/** Returns the number of non-routed demands. **/
	int getNbNonRoutedDemands() const {return getNbDemands() - getNbRoutedDemands(); }

	/** Returns the number of demands routed in the first initial mapping. **/
	int getNbInitialDemands() const { return nbInitialDemands; }

	/** Returns the number of links in the physical network. **/
	int getNbEdges() const { return (int)this->tabEdge.size(); }

	/** Returns the number of nodes in the physical network. **/
	int getNbNodes() const { return this->nbNodes; }

	/** Returns the instance's input. **/
	const Input & getInput() const { return this->input; }

	/** Returns the Fiber with given index. @param index The index of Fiber required in tabEdge. **/
	const Fiber & getPhysicalLinkFromIndex(int index) const { return this->tabEdge[index]; }	

	/** Returns the first Fiber with the given source and target.  @warning Should only be called if method hasLink returns true. If there is no such link, the program is aborted! @param s Source node id. @param t Target node id. **/
	Fiber getPhysicalLinkBetween(int s, int t);	

	/** Returns the vector of Fiber. **/
	std::vector<Fiber> getTabEdge() const { return this->tabEdge; }

	/** Returns the demand with given index. @param index The index of Demand required in tabDemand.**/
	const Demand & getDemandFromIndex(int index) const { return this->tabDemand[index]; }

	/** Returns the vector of Demand. **/
	const std::vector<Demand> & getTabDemand() const { return this->tabDemand; }
	
	/** Returns the vector of demands to be routed in the next optimization. **/
	std::vector<Demand> getNextDemands() const;

	/** Returns the max used slice position throughout the whole network. **/
	int getMaxUsedSlicePosition() const;

	/** Returns the max slice position (used or not) throughout the whole network. **/
	int getMaxSlice() const;

	/** Returns the index of the next demand to be analyzed in tabDemand. **/
	int getNextDemandToBeRoutedIndex() const { return this->nextDemandToBeRoutedIndex; }
	
	double getMetricValue(Metric m) const;

	int getNumberOfOccupiedSlices() const;

	// PEDRO PEDRO PEDRO
	/** Returns the value of the Pase noise of a node amplifier **/
	double getPaseNodeC() const { return this->paseNodeC; }
	double getPaseNodeL() const { return this->paseNodeL; }
	double getPaseNodeS() const { return this->paseNodeS; }
	
	/****************************************************************************************/
	/*										Setters											*/
	/****************************************************************************************/

	void setWasBlocked(bool flag) { this->wasBlocked = flag; }

	/** Change the total number of demands. @param nb New total number of demands. @warning This function resizes vector tabDemand, creating default demands if nb is greater than the previous size.**/
	void setNbDemands(int nb) { this->tabDemand.resize(nb); }

	/** Returns the number of demands routed in the first initial mapping. @param nb New number of demands.**/
	void setNbInitialDemands(int nb) { this->nbInitialDemands = nb; }

	/** Change the number of nodes in the physical network. @param nb New number of nodes. **/
	void setNbNodes(int nb) { this->nbNodes = nb; }	

	/** Change the set of links in the physical network. @param tab New vector of PhysicalLinks. **/
	void setTabEdge(std::vector<Fiber> tab) { this->tabEdge = tab; }

	/** Change the set of demands. @param tab New vector of Demands. **/
	void setTabDemand(std::vector<Demand> tab) { this->tabDemand = tab; }

	/** Changes the attributes of the Fiber from the given index according to the attributes of the given link. @param i The index of the Fiber to be changed. @param link the Fiber to be copied. **/
	void setEdgeFromId(int i, Fiber &link);

	/** Changes the attributes of the Demand from the given index according to the attributes of the given demand. @param i The index of the Demand to be changed. @param demand the Demand to be copied. **/
	void setDemandFromId(int i, const Demand &demand);

	/** Decreases the number of demands to be treated by one. **/
	void decreaseNbDemandsAtOnce();

	/** Changes the time limit. @param val The new time limit (in seconds). **/
	void setTimeLimit(int val){ this->input.setTimeLimit(val); };
	
	/** Changes the index of the next demand to be analyzed in tabDemand. @param val The new index.**/
	void setNextDemandToBeRoutedIndex(int val) { this->nextDemandToBeRoutedIndex = val; }

	/** Changes the number of demands to be treated in a single optimization. @param val The new number of demands. **/
    void setNbDemandsAtOnce(const int val) { this->input.setNbDemandsAtOnce(val); }

	/****************************************************************************************/
	/*										Methods											*/
	/****************************************************************************************/
	
	/** Builds the initial mapping based on the information retrived from the Input. **/
	void createInitialMapping();
	
	/** Reads the topology information from input's topologyFile. Builds the set of links. @warning File should be structured as in Link.csv. **/
	void readTopology();
	
	/** Reads the routed demand information from input's demandFile. Builds the set of demands. @warning File should be structured as in Demand.csv. **/
	void readDemands();

	/** Reads the assignment information from input's initialMappingAssignmentFile. Sets the demands to routed and update the slices of the edges. @warning File should be structured as in Demand_edges_slices.csv. **/
	void readDemandAssignment();

	/** Adds non-routed demands to the pool by reading the information from file. @param filePath The path of the file to be read. **/
	void generateDemandsFromFile(std::string filePath);

	/** Adds non-routed demands to the pool by generating random demands. @param N The number of random demands to be generated. **/
	void generateRandomDemands(const int N);

	/** Assigns a demand to a slice of a link. @param index The index of the Fiber to be modified. @param pos The last slice position. @param demand The demand to be assigned. **/
	void assignSlicesOfLink(int index, int pos, const Demand &demand);

	/** Verifies if there is enough place for a given demand to be routed through a link on a last slice position. @param index The index of the Fiber to be inspected. @param pos The last slice positon. @param demand The candidate demand to be assigned. **/
	bool hasEnoughSpace(const int index, const int pos, const Demand &demand);

	/** Verifies if there exists a link between two nodes. @param u Source node id. @param v Target node id. **/
	bool hasLink(int u, int v);

	/** Call the methods allowing the build of output files. @param i Indicates the current iteration. **/
	void output(std::string i = "0");
	
	/** Builds file Edge_Slice_Holes_i.csv containing information about the mapping after n optimizations. @param i The i-th output file to be generated. **/
	void outputEdgeSliceHols(std::string i);

	/** Builds file Demand.csv containing information about the routed demands. @param i The i-th output file to be generated. **/
	void outputDemands(std::string i);

	/** Builds file Metrics.csv containing information about the obtained metric values. @param i The i-th output file to be generated. **/
	void outputMetrics(std::string i);
	
	/** Builds file Demand_edges_slices.csv containing information about the assignment of routed demands. @param i The i-th output file to be generated. **/
	void outputDemandEdgeSlices(std::string i);

	/** Builds file results.csv containing information about the main obtained results. @param fileName The name of demand file being optimized. **/
	void outputLogResults(std::string fileName, double time);

	//PEDRO PEDRO PEDRO
	void setPaseNodeC();	
	void setPaseNodeL();	
	void setPaseNodeS();	
	/****************************************************************************************/
	/*										Display											*/
	/****************************************************************************************/
	/** Displays overall information about the current instance. **/
	void displayInstance();

	/** Displays information about the physical topology. **/	
	void displayTopology();
	
	/** Displays detailed information about state of the physical topology. **/
	void displayDetailedTopology();
	
	/** Displays summarized information about slice occupation of each Fiber. **/
	void displaySlices();
	
	/** Displays information about the routed demands. **/
	void displayRoutedDemands();

	/** Displays information about the non-routed demands. **/
	void displayNonRoutedDemands();

	/** Displays information about the all the demands. **/
	void displayAllDemands();

	/****************************************************************************************/
	/*										Destructor										*/
	/****************************************************************************************/

	/** Destructor. Clears the vectors of demands and links. **/
	~Instance();
};

#endif