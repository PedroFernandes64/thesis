#ifndef __RSA__h
#define __RSA__h

#include <ilcplex/ilocplex.h>
#include <lemon/list_graph.h>
#include <lemon/concepts/graph.h>
#include <lemon/dijkstra.h>
#include <lemon/list_graph.h>
#include <lemon/concepts/graph.h>
#include <lemon/bfs.h>

#include "../topology/instance.h"
#include "../tools/clockTime.h"


using namespace lemon;

typedef ListDigraph::NodeMap<int> NodeMap;
typedef ListDigraph::ArcMap<int> ArcMap;
typedef ListDigraph::ArcMap<double> ArcCost;
typedef ListGraph::EdgeMap<int> EdgeMap;
typedef ListGraph::EdgeMap<double> EdgeCost;
typedef ListGraph::NodeMap<int> CompactNodeMap;

/**********************************************************************************************
 * This class stores the input needed for solving the Routing and Spectrum Allocation problem.
 * This consists of an initial mapping and a set of demands to be routed. A graph associated 
 * to this initial mapping is built as well as an extended graph for each demand to be routed. 
 * \note It uses the LEMON library to build the associated graphs. 
 * *******************************************************************************************/
class RSA{

public: 
	/** Enumerates the possible algorithm status according to the current model and solution in hand. **/
	enum Status {
        STATUS_UNKNOWN	= 0,        /**< The algorithm has no information about the solution of the model. **/					
		STATUS_FEASIBLE = 1,        /**< The algorithm found a feasible solution that may not necessarily be optimal. **/
		STATUS_OPTIMAL = 2,         /**< The algorithm found an optimal solution. **/
        STATUS_INFEASIBLE = 3,      /**< The algorithm proved the model infeasible; that is, it is not possible to find a feasible solution. **/
        STATUS_UNBOUNDED = 4,       /**< The algorithm proved the model unbounded. **/
        STATUS_INFEASIBLE_OR_UNBOUNDED = 5, /**< The model is infeasible or unbounded. **/
        STATUS_ERROR = 6            /**< An error occurred. **/
	};

protected:
    Instance instance;                  /**< An instance describing the initial mapping. **/

    std::vector<Demand> toBeRouted;     /**< The list of demands to be routed in the next optimization. **/
    std::vector<int> loadsToBeRouted;   /**< The set of different loads present in the set of demands to be routed. **/

    double RSAGraphConstructionTime;
    double PreprocessingTime;

    /** A list of pointers to the extended graph associated with each demand to be routed. 
        \note (*vecGraph[i]) is the graph associated with the i-th demand to be routed. **/
    std::vector< std::shared_ptr<ListDigraph> > vecGraph;

    /** A list of pointers to the ArcMap storing the arc ids of the graph associated with each demand to be routed. 
        \note (*vecArcId[i])[a] is the id of arc a in the graph associated with the i-th demand to be routed. **/
    std::vector< std::shared_ptr<ArcMap> > vecArcId;
    
    /** A list of pointers to the ArcMap storing the arc labels of the graph associated with each demand to be routed. 
        \note (*vecArcLabel[i])[a] is the label of arc a in the graph associated with the i-th demand to be routed. **/
    std::vector< std::shared_ptr<ArcMap> > vecArcLabel;

    /** A list of pointers to the ArcMap storing the arc slices of the graph associated with each demand to be routed. 
        \note (*vecArcSlice[i])[a] is the slice of arc a in the graph associated with the i-th demand to be routed. **/
    std::vector< std::shared_ptr<ArcMap> > vecArcSlice;

    /** A list of pointers to the map storing the arc lengths of the graph associated with each demand to be routed. 
        \note (*vecArcLength[i])[a] is the length of arc a in the graph associated with the i-th demand to be routed. **/
    std::vector< std::shared_ptr<ArcCost> > vecArcLength;
    
    //PEDRO PEDRO PEDRO
    /** A list of pointers to the map storing the arc amplifiers of the graph associated with each demand to be routed. 
        \note (*vecArcLineAmplifiers[i])[a] is the arc amplifiers of arc a in the graph associated with the i-th demand to be routed. **/
    std::vector< std::shared_ptr<ArcMap> > vecArcLineAmplifiers;
    /** A list of pointers to the map storing the arc pnli of the graph associated with each demand to be routed. 
        \note (*vecArcLineAmplifiers[i])[a] is the arc amplifiers of arc a in the graph associated with the i-th demand to be routed. **/
    std::vector< std::shared_ptr<ArcCost> > vecArcPnliC;
    std::vector< std::shared_ptr<ArcCost> > vecArcPaseLineC;
    std::vector< std::shared_ptr<ArcCost> > vecArcNoiseC;
    std::vector< std::shared_ptr<ArcCost> > vecArcPnliL;
    std::vector< std::shared_ptr<ArcCost> > vecArcPaseLineL;
    std::vector< std::shared_ptr<ArcCost> > vecArcNoiseL;

    /** A list of pointers to the map storing the arc lengths with hop penalties included of the graph associated with each demand to be routed. 
        \note (*vecArcLengthWithPenalty[i])[a] is the length with penalties of arc a in the graph associated with the i-th demand to be routed. **/
    std::vector< std::shared_ptr<ArcCost> > vecArcLengthWithPenalty;

    /** A list of pointers to the NodeMap storing the node ids of the graph associated with each demand to be routed. 
        \note (*vecNodeId[i])[v] is the id of node v in the graph associated with the i-th demand to be routed. **/  
    std::vector< std::shared_ptr<NodeMap> > vecNodeId;

    /** A list of pointers to the NodeMap storing the node labels of the graph associated with each demand to be routed. 
        \note (*vecNodeLabel[i])[v] is the label of node v in the graph associated with the i-th demand to be routed. **/
    std::vector< std::shared_ptr<NodeMap> > vecNodeLabel;
    
    /** A list of pointers to the NodeMap storing the node slices of the graph associated with each demand to be routed. 
        \note (*vecNodeSlice[i])[v] is the slice of node v in the graph associated with the i-th demand to be routed. **/
    std::vector< std::shared_ptr<NodeMap> > vecNodeSlice;

    /** A list of pointers to the ArcMap storing the RSA solution. It stores the id of the demand that is routed through each arc of each graph. 
        \note (*vecOnPath[i])[a] is the id the demand routed through arc a in the graph associated with the i-th demand to be routed. **/
    std::vector< std::shared_ptr<ArcMap> > vecOnPath;

	/** A list of pointers to the ArcMap storing the arc index of the preprocessed graph associated with each demand to be routed. 
        \note (*vecArcIndex[i])[a] is the index of the arc a in the preprocessed graph associated with the i-th demand to be routed. **/
	std::vector< std::shared_ptr<ArcMap> > vecArcIndex; 

    /** A list of pointers to the NodeMap storing the node index of the preprocessed graph associated with each demand to be routed. 
        \note (*vecNodeIndex[i])[v] is the index of the node v in the preprocessed graph associated with the i-th demand to be routed. **/
    std::vector< std::shared_ptr<NodeMap> > vecNodeIndex;

    /** Vector with the node index of the source node and target node of each graph associated with each demand to be routed 
        \note sourceNodeIndex[i] is the index of the source node in the preprocessed graph associated with the i-th demand to be routed. 
        \note targetNodeIndex[i] is the index of the target node in the preprocessed graph associated with the i-th demand to be routed. **/
    std::vector<int> sourceNodeIndex;
    std::vector<int> targetNodeIndex;

    /** The ID of the flow variables. **/
    std::vector< std::shared_ptr<ArcMap> > vecArcVarId;
    int maxSliceOverallVarId;

    ListGraph compactGraph;             /**< The simple graph associated with the initial mapping. **/
    EdgeMap compactEdgeId;              /**< EdgeMap storing the edge ids of the simple graph associated with the initial mapping. **/
    EdgeMap compactEdgeLabel;           /**< EdgeMap storing the edge labels of the simple graph associated with the initial mapping. **/
    EdgeCost compactEdgeLength;         /**< EdgeMap storing the edge lengths of the simple graph associated with the initial mapping. **/
    //PEDRO PEDRO PEDRO
    EdgeMap compactEdgeLineAmplifiers;  /**< EdgeMap storing the edge line amplifiers of the simple graph associated with the initial mapping. **/
    EdgeCost compactEdgePnliC;           /**< EdgeMap storing the edge pnli of the simple graph associated with the initial mapping. **/
    EdgeCost compactEdgePaseLineC;           /**< EdgeMap storing the edge pase line of the simple graph associated with the initial mapping. **/
    EdgeCost compactEdgePnliL;           /**< EdgeMap storing the edge pnli of the simple graph associated with the initial mapping. **/
    EdgeCost compactEdgePaseLineL;           /**< EdgeMap storing the edge pase line of the simple graph associated with the initial mapping. **/

    std::vector<std::vector<int> > preProcessingTFlow;
    
    CompactNodeMap compactNodeId;       /**< NodeMap storing the LEMON node ids of the simple graph associated with the initial mapping. **/
    CompactNodeMap compactNodeLabel;    /**< NodeMap storing the node labels of the simple graph associated with the initial mapping. **/
    
    Status currentStatus;		/**< Provides information about the current model and solution. **/

    /* Map iterator for the nodes labels. */
    std::vector< std::shared_ptr<IterableIntMap<ListDigraph, ListDigraph::Node>>> mapItNodeLabel;

    /* Map iterator for the arcs labels. */
    std::vector< std::shared_ptr<IterableIntMap<ListDigraph, ListDigraph::Arc>>> mapItArcLabel;

    std::vector<int> auxNbSlicesLimitFromEdge;
	int auxNbSlicesGlobalLimit;

public:
	/****************************************************************************************/
	/*										Constructor										*/
	/****************************************************************************************/

    /** Constructor. A graph associated with the initial mapping (instance) is built as well as an extended graph for each demand to be routed. **/
    RSA(const Instance &instance);

	/****************************************************************************************/
	/*										Getters 										*/
	/****************************************************************************************/

    double getRSAGraphConstructionTime() const { return RSAGraphConstructionTime;}
    double getPreprocessingTime() const { return PreprocessingTime; }
    
    /** Returns the input instance. **/
    Instance getInstance() const{ return instance; }

    /** Returns the vector of demands to be routed. **/
    std::vector<Demand> getToBeRouted() { return toBeRouted; } 
    
    /** Returns the vector of loads to be routed. **/
    std::vector<int> getLoadsToBeRouted() { return loadsToBeRouted; } 
    
    /** Returns the i-th demand to be routed. @param k The index of the required demand. **/
    Demand getToBeRouted_k(int k) const { return toBeRouted[k]; }

    /** Returns the i-th load to be routed. @param k The index of the required load. **/
    int getLoadsToBeRouted_k(int k){ return loadsToBeRouted[k]; }

    /** Returns the number of demands to be routed. **/
    int getNbDemandsToBeRouted() const { return toBeRouted.size(); }

    /** Returns the number of loads to be routed. **/
    int getNbLoadsToBeRouted() const { return loadsToBeRouted.size(); }

    /** Returns the total number of loads to be routed. **/
    int getTotalLoadsToBeRouted() const;

    /** Returns the LEMON id of a node in a graph. @param n The node. @param d The graph index. **/
    int getNodeId(const ListDigraph::Node &n, int d) const { return (*vecNodeId[d])[n]; }
    
    /** Returns the label of a node in a graph. @param n The node. @param d The graph index. **/
    int getNodeLabel(const ListDigraph::Node &n, int d) const { return (*vecNodeLabel[d])[n]; }

    /** Returns the slice of a node in a graph. @param n The node. @param d The graph index. **/
    int getNodeSlice(const ListDigraph::Node &n, int d) const { return (*vecNodeSlice[d])[n]; }
    
    /** Returns the LEMON id of an arc in a graph. @param a The arc. @param d The graph index. **/
    int getArcId(const ListDigraph::Arc &a, int d) const { return (*vecArcId[d])[a]; }
    
	/** Returns the index of an arc in a preprocessed graph. @param a The arc. @param d The graph index. **/
    int getArcIndex(const ListDigraph::Arc &a, int d) const { return (*vecArcIndex[d])[a]; }

    /** Returns the label of an arc in a graph. @param a The arc. @param d The graph index. **/
    int getArcLabel(const ListDigraph::Arc &a, int d) const { return (*vecArcLabel[d])[a]; }
    
    /** Returns the slice of an arc in a graph. @param a The arc. @param d The graph index. **/
    int getArcSlice(const ListDigraph::Arc &a, int d) const { return (*vecArcSlice[d])[a]; }
    
    /** Returns the length of an arc in a graph. @param a The arc. @param d The graph index. **/
    double getArcLength(const ListDigraph::Arc &a, int d) const  {return (*vecArcLength[d])[a]; }

    //PEDRO PEDRO PEDRO
    /** Returns the line amplifiers of an arc in a graph. @param a The arc. @param d The graph index. **/
    int getArcLineAmplifiers(const ListDigraph::Arc &a, int d) const  {return (*vecArcLineAmplifiers[d])[a]; }
    /** Returns the pnli of an arc in a graph. @param a The arc. @param d The graph index. **/
    double getArcPnliC(const ListDigraph::Arc &a, int d) const  {return (*vecArcPnliC[d])[a]; }
    double getArcPaseLineC(const ListDigraph::Arc &a, int d) const  {return (*vecArcPaseLineC[d])[a]; }
    double getArcNoiseC(const ListDigraph::Arc &a, int d) const  {return (*vecArcNoiseC[d])[a]; }
    double getArcPnliL(const ListDigraph::Arc &a, int d) const  {return (*vecArcPnliL[d])[a]; }
    double getArcPaseLineL(const ListDigraph::Arc &a, int d) const  {return (*vecArcPaseLineL[d])[a]; }
    double getArcNoiseL(const ListDigraph::Arc &a, int d) const  {return (*vecArcNoiseL[d])[a]; }

    /** Returns the length with hop penalties of an arc in a graph. @param a The arc. @param d The graph index. **/
    double getArcLengthWithPenalties(const ListDigraph::Arc &a, int d) const  {return (*vecArcLengthWithPenalty[d])[a]; }

    /** Returns the index of a node in a graph. @param v The node. @param d The graph index. **/
    int getNodeIndex(const ListDigraph::Node &v,int d) const{ return (*vecNodeIndex[d])[v];}

    /** Returns the index of the source in a graph. @param d The graph index. **/
    int getSourceNodeIndex(int d) const{ return sourceNodeIndex[d];}

    /** Returns the index of the target in a graph. @param d The graph index. **/
    int getTargetNodeIndex(int d) const{ return targetNodeIndex[d];}

    /** Returns the id of the flow variables **/
    int getVarId(const ListDigraph::Arc &a, int d) const{ return (*vecArcVarId[d])[a];}

    /** Returns the first node identified by (label, slice) on a graph. @param d The graph index. @param label The node's label. @param slice The node's slice. \warning If it does not exist, returns INVALID. **/
    ListDigraph::Node getNode(int d, int label, int slice);

    std::vector<int> indexArcsEdge;
    
    /** Returns the coefficient of an arc (according to the chosen metric) on a graph. @param a The arc. @param d The graph index. **/
    double getCoeff(const ListDigraph::Arc &a, int d);
    
    /** Returns the coefficient of an arc according to metric 1 on a graph. @param a The arc. @param d The graph index. \note Min sum(max used slice positions) over demands. **/
    double getCoeffObj1(const ListDigraph::Arc &a, int d);

    /** Returns the coefficient of an arc according to metric 1p on a graph. @param a The arc. @param d The graph index. \note Min sum(max used slice positions) over edges. **/
    double getCoeffObj1p(const ListDigraph::Arc &a, int d);

    /** Returns the coefficient of an arc according to metric 2 on a graph. @param a The arc. @param d The graph index. \note Min number of hops (number of edges in path). **/
    double getCoeffObj2(const ListDigraph::Arc &a, int d);

    /** Returns the coefficient of an arc according to metric 2p on a graph. @param a The arc. @param d The graph index. \note Min number of hops (number of edges in path). **/
    double getCoeffObj2p(const ListDigraph::Arc &a, int d);

    /** Returns the coefficient of an arc according to metric 4 on a graph. @param a The arc. @param d The graph index. \note Min path lengths. **/
    double getCoeffObj4(const ListDigraph::Arc &a, int d);

    /** Returns the coefficient of an arc according to metric 4p on a graph. @param a The arc. @param d The graph index. \note Min path lengths. **/
    double getCoeffObj4p(const ListDigraph::Arc &a, int d);

    /** Returns the coefficient of an arc according to metric 8 on a graph. @param a The arc. @param d The graph index. \note Min max global used slice position. **/
    double getCoeffObj8(const ListDigraph::Arc &a, int d);

    /** Returns the coefficient of an arc according to metric 10 on a graph. @param a The arc. @param d The graph index. \note Min path lengths. **/
    double getCoeffObj10(const ListDigraph::Arc &a, int d);

	/** Returns the algorithm status. **/
    Status getStatus() const { return currentStatus; }

    /** Returns the LEMON id of a node in the compact graph. @param n The node. **/
    int getCompactNodeId(const ListGraph::Node &n) const { return compactNodeId[n]; }
    
    /** Returns the label of a node in the compact graph. @param n The node. **/
    int getCompactNodeLabel(const ListGraph::Node &n) const { return compactNodeLabel[n]; }
    
    /** Returns the LEMON id of an edge in the compact graph. @param e The edge. **/
    int getCompactEdgeId(const ListGraph::Edge &e) const { return compactEdgeId[e]; }

    /** Returns the label of an edge in the compact graph. @param e The edge. **/
    int getCompactEdgeLabel(const ListGraph::Edge &e) const { return compactEdgeLabel[e]; }
    
    /** Returns the length of an edge on the compact graph. @param e The edge. */
    double getCompactLength(const ListGraph::Edge &e) { return compactEdgeLength[e]; }

     //PEDRO PEDRO PEDRO
    /** Returns the line amplifiers of an edge on the compact graph. @param e The edge. */
    int getCompactLineAmplifiers(const ListGraph::Edge &e) { return compactEdgeLineAmplifiers[e]; }
    double getCompactPnliC(const ListGraph::Edge &e) { return compactEdgePnliC[e]; }
    double getCompactPaseLineC(const ListGraph::Edge &e) { return compactEdgePaseLineC[e]; }
    double getCompactPnliL(const ListGraph::Edge &e) { return compactEdgePnliL[e]; }
    double getCompactPaseLineL(const ListGraph::Edge &e) { return compactEdgePaseLineL[e]; }

    ListGraph::Node getCompactNodeFromLabel(int label) const;
    
    int getNbSlicesGlobalLimit() const{ return std::min(instance.getMaxSlice(), instance.getMaxUsedSlicePosition() + 1 + getTotalLoadsToBeRouted());}
	int getNbSlicesLimitFromEdge(int edge) const{ return std::min(instance.getPhysicalLinkFromIndex(edge).getNbSlices(), instance.getMaxUsedSlicePosition() + 1 + getTotalLoadsToBeRouted());}

    int getDegree(const ListGraph::Node& node) const;

    int getCutCardinality(const std::vector<int> & cutSet) const;
	/****************************************************************************************/
	/*										Setters											*/
	/****************************************************************************************/

    void setRSAGraphConstructionTime(double val) { RSAGraphConstructionTime=val;}
    void setPreprocessingTime(double val) { PreprocessingTime=val; }

    /** Changes the vector of demands to be routed. @param vec The new vector of demands. **/
    void setToBeRouted(const std::vector<Demand> &vec){this->toBeRouted = vec;}

    /** Changes the id of a node in a graph. @param n The node. @param d The graph index. @param val The new id. **/
    void setNodeId(const ListDigraph::Node &n, int d, int val) { (*vecNodeId[d])[n] = val; }
    
    /** Changes the label of a node in a graph. @param n The node. @param d The graph index. @param val The new label. **/
    void setNodeLabel(const ListDigraph::Node &n, int d, int val) { (*vecNodeLabel[d])[n] = val; }
    
    /** Changes the slice of a node in a graph. @param n The node. @param d The graph index. @param val The new slice position. **/
    void setNodeSlice(const ListDigraph::Node &n, int d, int val) { (*vecNodeSlice[d])[n] = val; }
    
    /** Changes the id of an arc in a graph. @param a The arc. @param d The graph index. @param val The new id. **/
    void setArcId(const ListDigraph::Arc &a, int d, int val) { (*vecArcId[d])[a] = val; }
    
	/** Changes the index of an arc in a graph. @param a The arc. @param d The graph index. @param val The new index value. **/
	void setArcIndex(const ListDigraph::Arc &a, int d, int val) { (*vecArcIndex[d])[a] = val; }

    /** Changes the label of an arc in a graph. @param a The arc. @param d The graph index. @param val The new label. **/
    void setArcLabel(const ListDigraph::Arc &a, int d, int val) { (*vecArcLabel[d])[a] = val; }
    
    /** Changes the slice of an arc in a graph. @param a The arc. @param d The graph index. @param val The new slice position. **/
    void setArcSlice(const ListDigraph::Arc &a, int d, int val) { (*vecArcSlice[d])[a] = val; }
    
    /** Changes the length of an arc in a graph. @param a The arc. @param d The graph index. @param val The new length. **/
    void setArcLength(const ListDigraph::Arc &a, int d, double val) { (*vecArcLength[d])[a] = val; }
    
    //PEDRO PEDRO PEDRO
    /** Changes the line amplifiers of an arc in a graph. @param a The arc. @param d The graph index. @param val The new line amplifiers. **/
    void setArcLineAmplifiers(const ListDigraph::Arc &a, int d, int val) { (*vecArcLineAmplifiers[d])[a] = val; }
    /** Changes the pnli of an arc in a graph. @param a The arc. @param d The graph index. @param val The new pnli. **/
    void setArcPnliC(const ListDigraph::Arc &a, int d, double val) { (*vecArcPnliC[d])[a] = val; }
    void setArcPaseLineC(const ListDigraph::Arc &a, int d, double val) { (*vecArcPaseLineC[d])[a] = val; }
    // Add arc noise considering one node amplifier!
    void setArcNoiseC(const ListDigraph::Arc &a, int d, double val) { (*vecArcNoiseC[d])[a] = val; }
    void setArcPnliL(const ListDigraph::Arc &a, int d, double val) { (*vecArcPnliL[d])[a] = val; }
    void setArcPaseLineL(const ListDigraph::Arc &a, int d, double val) { (*vecArcPaseLineL[d])[a] = val; }
    void setArcNoiseL(const ListDigraph::Arc &a, int d, double val) { (*vecArcNoiseL[d])[a] = val; }
    
    /** Changes the length with hop penalty of an arc in a graph. @param a The arc. @param d The graph index. @param val The new length. **/
    void setArcLengthWithPenalty(const ListDigraph::Arc &a, int d, double val) { (*vecArcLengthWithPenalty[d])[a] = val; }

    void setNodeIndex(const ListDigraph::Node &v,int d, int val){ (*vecNodeIndex[d])[v] = val;}

	/** Changes the algorithm status. **/
    void setStatus(Status val) { currentStatus = val; }

    /** Changes the id of a node in the compact graph. @param n The node. @param val The new id. **/
    void setCompactNodeId(const ListGraph::Node &n, int val) { compactNodeId[n] = val; }
    
    /** Changes the label of a node in the compact graph. @param n The node. @param val The new label. **/
    void setCompactNodeLabel(const ListGraph::Node &n, int val) { compactNodeLabel[n] = val; }
    
    /** Changes the id of an edge in the compact graph. @param e The edge. @param val The new id. **/
    void setCompactEdgeId(const ListGraph::Edge &e, int val) { compactEdgeId[e] = val; }

    /** Changes the label of an edge in the compact graph. @param e The edge. @param val The new label. **/
    void setCompactEdgeLabel(const ListGraph::Edge &e, int val) { compactEdgeLabel[e]= val; }
    
    /** Changes the length of an edge on the compact graph. @param e The edge. @param val The new length value. */
    void setCompactLength(const ListGraph::Edge &e, double val) { compactEdgeLength[e] = val; }
    
    //PEDRO PEDRO PEDRO
    /** Changes the line amplifiers of an edge on the compact graph. @param e The edge. @param val The new line amplifiers  value. */
    void setCompactLineAmplifiers(const ListGraph::Edge &e, int val) { compactEdgeLineAmplifiers[e] = val; }
    void setCompactPnliC(const ListGraph::Edge &e, double val) { compactEdgePnliC[e] = val; }
    void setCompactPaseLineC(const ListGraph::Edge &e, double val) { compactEdgePaseLineC[e] = val; }
    void setCompactPnliL(const ListGraph::Edge &e, double val) { compactEdgePnliL[e] = val; }
    void setCompactPaseLineL(const ListGraph::Edge &e, double val) { compactEdgePaseLineL[e] = val; }

	/****************************************************************************************/
	/*										Methods											*/
	/****************************************************************************************/

    /** Builds the simple graph associated with the initial mapping. **/
    void buildCompactGraph();

   /** Creates an arc -- and its nodes if necessary -- between nodes (source,slice) and (target,slice) on a graph. 
     * @param d The graph index. @param source The source node's id. @param target The target node's id. 
     * @param linkLabel The arc's label. @param slice The arc's slice position. @param l The arc's length. 
     * @param la The arc's line amplifiers. @param pn The arc's pnli. @param pa The arc's pase line**/
    void addArcs(int d, int source, int target, int linkLabel, int slice, double l, int la, double pnc, double pac, double pnl, double pal);    
     
    /** Updates the mapping stored in the given instance with the results obtained from RSA solution (i.e., vecOnPath). @param i The instance to be updated.*/
    void updateInstance(Instance &i);

    /** Returns the first node with a given label from the graph associated with the d-th demand to be routed. @note If such node does not exist, returns INVALID. @param d The graph index. @param label The node's label. **/
    ListDigraph::Node getFirstNodeFromLabel(int d, int label);
    
    /** Contract nodes with the same given label from the graph associated with the d-th demand to be routed. @param d The graph index. @param label The node's label. **/
    void contractNodesFromLabel(int d, int label);

    /** Delete arcs that are known 'a priori' to be unable to route on a graph. Erase arcs that do not support the demand's load. @param d The index of the graph to be inspected. **/
    void eraseNonRoutableArcs(int d);
    
    /** Runs preprocessing on every extended graph. **/
    void preprocessing();

    /** If there exists no path from (source,s) to (target,s), erases every arc with slice s. **/
    void pathExistencePreprocessing();

    /** Performs preprocessing based on the arc lengths and returns true if at least one arc is erased. An arc (u,v) can only be part of a solution if the distance from demand source to u, plus the distance from v to demand target plus the arc length is less than or equal to the demand's maximum length. **/
    bool lengthPreprocessing();
    bool OSNRPreprocessingC();

    /** Returns the distance of the shortest path from source to target passing through arc a. \note If there exists no st-path, returns +Infinity. @param d The graph index. @param source The source node.  @param a The arc required to be present. @param target The target node.  **/
    double shortestDistance(int d, ListDigraph::Node &source, ListDigraph::Arc &a, ListDigraph::Node &target);
    double shortestOSNRCPartial(int d, ListDigraph::Node &source, ListDigraph::Arc &a, ListDigraph::Node &target);

    // PEDRO PEDRO PEDRO
    /** Performs GN Model over all possibilities of path**/
    std::vector<std::vector<int> > adj_list;
    std::vector<std::vector<ListGraph::Node> >  pathsdemand;
    std::vector<std::vector<std::vector<ListGraph::Node> > > allpaths;
    double osnrPathC(double pl, double pn, double pnli, double pc);
    double osnrPathL(double pl, double pn, double pnli, double pc);
    double osnrPathS(double pl, double pn, double pnli, double pc);
    void gnModelAllPaths();
    void printAllPathsUtil(int u, int d, bool visited[], int path[], int& path_index);

	/****************************************************************************************/
	/*										Display											*/
	/****************************************************************************************/

    /** Displays the nodes of a graph that are incident to the node identified by (label,slice).  @param d The graph index. @param label The node's label. @param slice The node's slice position. **/
    void displayNodesIncidentTo(int d, int label, int slice);

    /** Displays the nodes of a graph that have a given label. @param d The graph index. @param label The node's label. **/
    void displayNodesFromLabel(int d, int label);

    /** Displays the paths found for each of the new routed demands. **/
    void displayPaths();
    /** Display OSNR corrected if Obj 10 chosen **/
    void displayOSNR();

    /** Displays an arc from a graph. @param d The graph index. @param a The arc to be displayed. **/
    void displayArc(int d, const ListDigraph::Arc &a);

    /** Displays a node from a graph. @param d The graph index. @param n The node to be displayed. */
    void displayNode(int d, const ListDigraph::Node &n);
    
    /** Display all arcs from a graph. @param d The graph index. **/
    void displayGraph(int d);

    /** Displays the demands to be routed in the next optimization. **/
    void displayToBeRouted();

    /** Displays the loads to be routed in the next optimization. @note If the there is no demand to be routed, the function will exit with an error. **/
    void displayLoadsToBeRouted();
    
	/****************************************************************************************/
	/*										Destructor										*/
	/****************************************************************************************/

	/** Destructor. Clears the spectrum. **/
	~RSA();


};
#endif