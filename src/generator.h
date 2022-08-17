#ifndef INSTANCES_GENERATOR
#define INSTANCES_GENERATOR

#include <float.h>

#include "topology/demand.h"
#include "topology/input.h"
#include "topology/physicalLink.h"
#include "tools/CSVReader.h"
#include "topology/instance.h"
#include "tools/clockTime.h"

#include <lemon/list_graph.h>
#include <lemon/concepts/graph.h>
#include <lemon/dijkstra.h>
#include <lemon/list_graph.h>
#include <lemon/concepts/graph.h>

using namespace lemon;

typedef ListDigraph::NodeMap<int> NodeMap;
typedef ListDigraph::ArcMap<int> ArcMap;
typedef ListDigraph::ArcMap<double> ArcCost;
typedef ListGraph::EdgeMap<int> EdgeMap;
typedef ListGraph::EdgeMap<double> EdgeCost;
typedef ListGraph::NodeMap<int> CompactNodeMap;

class Generator{
    private:
        std::string topologyFile;           /**< Topology file. **/
        std::string demandsFile;            /**< Resulting demands file. **/
        int nbNodes;						/**< Number of nodes in the physical network. **/
	    std::vector<Fiber> tabEdge;			/**< A set of Fiber. **/

        ListDigraph compactGraph;             
        ArcMap compactArcId;              
        ArcMap compactArcLabel;           
        ArcCost compactArcLength;         
        NodeMap compactNodeId;       
        NodeMap compactNodeLabel;    
    
    public:
        Generator(std::string, std::string);
        
        /** Reads the topology information from input's topologyFile. Builds the set of links. @warning File should be structured as in Link.csv. **/
        void readTopology();

        void buildCompactGraph();

        /** Returns the Fiber with given index. @param index The index of Fiber required in tabEdge. **/
	    Fiber getPhysicalLinkFromIndex(int index) const { return this->tabEdge[index]; }	

        /** Returns the number of links in the physical network. **/
        int getNbEdges() const { return (int)this->tabEdge.size(); }

        /** Returns the number of nodes in the physical network. **/
        int getNbNodes() const { return this->nbNodes; }

        /** Change the number of nodes in the physical network. @param nb New number of nodes. **/
	    void setNbNodes(int nb) { this->nbNodes = nb; }	

        void generateDemands(int);

        ListDigraph::Node getFirstNodeFromLabel(int) const;
};


#endif