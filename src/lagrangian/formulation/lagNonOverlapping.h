#ifndef LAG_NON_OVERLAPPING_H
#define LAG_NON_OVERLAPPING_H

#include "AbstractLagrangianFormulation.h"
#include <set>
#include <lemon/core.h>
#include <lemon/capacity_scaling.h>

using namespace lemon;

class lagNonOverlapping: public AbstractLagFormulation{

    private:

        /***************************** ASSIGNMENT MATRIX *******************************/

        std::vector< std::vector<bool> > assignmentMatrix;

        /***************************** BUILDING AUXILIARY GRAPH *******************************/

        /** A list of pointers to the extended graph associated with each EDGE to be routed. 
        \note (*vecGraph[i]) is the graph associated with the i-th EDGE to be routed. **/
        std::vector<std::shared_ptr<ListDigraph>> vecEGraph;

        /** A list of pointers to the NodeMap storing the node LEMON IDs of the graph associated with each EDGE to be routed. 
        \note (*vecENodeID[i])[v] is the LEMON ID of node v in the graph associated with the i-th EDGE to be routed. **/
        /* using in the assigment matrix */
        std::vector<std::shared_ptr<NodeMap>> vecENodeID;

        /** A list of pointers to the NodeMap storing the node INDEXs of the graph associated with each EDGE to be routed. 
        \note (*vecENodeID[i])[v] is the LEMON ID of node v in the graph associated with the i-th EDGE to be routed. **/
        std::vector<std::shared_ptr<NodeMap>> vecENodeIndex;

        /** A list of pointers to the NodeMap storing the node DEMANDS of the graph associated with EDGE to be routed
        \note (*vecENodeDemand[i])[v] is the DEMAND of node v in the graph associated with the i-th EDGE to be routed. **/
        /** Indeed, here each node refers to an arc in the graph for each d (G(D)), so, its the DEMAND of the original arc **/
        std::vector<std::shared_ptr<NodeMap>> vecENodeDemand;

        /** A list of pointers to the NodeMap storing the node SLICES of the graph associated with EDGE to be routed
        \note (*vecENodeSlice[i])[v] is the SLICE of node v in the graph associated with the i-th EDGE to be routed. **/
        /** Indeed, here each node refers to an arc in the graph for each d (G(D)), so, its the SLICE of the original arc **/
        std::vector<std::shared_ptr<NodeMap>> vecENodeSlice;

        /** A list of pointers to the NodeMap storing the node FIRST CONSTRAINT of the graph associated with EDGE to be routed
        \note (*vecENodeFirstConst[i])[v] is the INDEX OF THE FIRST CONSTRAINT of node v in the graph associated with the i-th 
        EDGE to be routed. **/
        /** Indeed, here each node refers to an arc in the graph for each d (G(D)), so, its the FIRST CONSTRAINT of the original arc **/
        std::vector<std::shared_ptr<NodeMap>> vecENodeFirstConst;

        /** A list of pointers to the ArcMap storing the arc ids of the graph associated with each demand to be routed. 
        \note (*vecArcId[i])[a] is the id of arc a in the graph associated with the i-th demand to be routed. **/
        std::vector<std::shared_ptr<ArcMap>> vecEArcId;

        /** A list of pointers to the map storing the arc COSTS of the graph associated with each EDGE to be routed. 
        \note (*vecECost[i])[a] is the COST of arc a in the graph associated with the i-th EDGE to be routed. **/
        /** The cost is associated with the destination node of a, this node represents an arc in the  graph for each d (G(D)) */
        std::vector<std::shared_ptr<ArcCost>> vecECost;

        /* Refers to the cost of an arc during iteration k. cost = c_{ij} + u_k*length_{ij} */
        /* This is the cost of the original variables, corresponding to the original graph G(D)*/
        std::vector<std::shared_ptr<ArcCost>> cost; 

        /** A list of pointers to the map storing the node ORIGINAL ARCS of the graph associated with each EDGE to be routed. 
        \note (*vecENodeArc[i])[a] is the ARC of nde a in the graph associated with the i-th EDGE to be routed. **/
        /** The ARC is in the  graph for each d (G(D)) */
        std::vector<std::shared_ptr<ListDigraph::NodeMap<std::shared_ptr<ListDigraph::Arc>>>> vecENodeArc;

        /* Vector of the id of the node source for each edge*/
        std::vector<int> vecESourceIndex;

        /* Vector of the id of the destination node for each edge*/
        std::vector<int> vecEDestinationIndex;

    
    public:
        /* *******************************************************************************
        *                             INITIALIZATION METHODS
        ******************************************************************************* */
        lagNonOverlapping(const Instance &instance):AbstractLagFormulation(instance){}

        /** Sets all initial parameters **/
        void init(bool=true);

        /********************************* MULTIPLIERS ***********************************/

        void startMultipliers(double *,int,int);
        
        /** Sets the initial lagrangian multipliers values for the subgradient to run. **/
        void initMultipliers();

        void initMultipliersWarmstart();

        void contractNodesFromLabel_v2(std::shared_ptr<ListDigraph>, ListDigraph::NodeMap<ListDigraph::Node>, int, int);

        /********************************* STABILITY CENTER ***********************************/

        /** Sets the initial lagrangian stability center values for the subgradient to run. **/
        void initStabilityCenter();

        /********************************** SLACK ***************************************/

        /** Initializes the slack of relaxed constraints. **/
        void initSlacks();

        void resetSlacks();

        void initPrimalSlacks();

        void clearSlacks();

        /********************************** DIRECTION **********************************/

        void initDirection();

        /***************************** BUILDING AUXILIARY GRAPH *******************************/

        /** Constructs the auxiliary graphs for each edge e **/
        void build_Graphs_e();

        /** Add a node in the auxiliary graph **/
        void addENode(int,int,int,int, const ListDigraph::Arc &);

        /* *******************************************************************************
        *                             RUNING METHODS
        ******************************************************************************* */
        void run(bool=false);

        double getRealCostFromPath(int, BellmanFord< ListDigraph, ListDigraph::ArcMap<double> > &, const ListDigraph::Node &, const ListDigraph::Node &);
        
        void updateAssignment_k(int, BellmanFord< ListDigraph, ListDigraph::ArcMap<double> > &, const ListDigraph::Node &, const ListDigraph::Node &);

        void subtractConstantValuesFromLagrCost();

        void solveProblemMaxUsedSliceOverall();

        /** Checks with the slacks if the solution is feasible. **/
        bool checkFeasibility();

        /* Verifies if the relaxed constraints are respected, considering primal apprximation. */
        bool checkFeasibility_v2();

        /** Checks if the slackness condition is satisfied ( mu*(Ax-b) = 0) **/
        bool checkSlacknessCondition();
 
        /****************************************************************************************/
        /*										Getters 										*/
        /****************************************************************************************/

        void getDualSolution(double *);
        
        /** Returns the LEMON id of a node in a graph. @param n The node. @param label The graph index. **/
        int getNodeEId(const ListDigraph::Node &n, int label) const { return (*vecENodeID[label])[n]; }
        
        /** Returns the demand of a node in a graph. @param n The node. @param label The graph index. **/
        int getNodeEDemand(const ListDigraph::Node &n, int label) const { return (*vecENodeDemand[label])[n]; }

        /** Returns the slice of a node in a graph. @param n The node. @param label The graph index. **/
        int getNodeESlice(const ListDigraph::Node &n, int label) const { return (*vecENodeSlice[label])[n]; }

        /** Returns the firstconst of a node in a graph. @param n The node. @param label The graph index. **/
        int getNodeEFirstConst(const ListDigraph::Node &n, int label) const{ return (*vecENodeFirstConst[label])[n]; }

        /** Returns the index of a node in a graph. @param n The node. @param label The graph index. **/
        int getNodeEIndex(const ListDigraph::Node &n, int label) const { return (*vecENodeIndex[label])[n]; }

        /** Returns the LEMON id of an arc in a graph. @param a The arc. @param label The graph index. **/
        int getArcEId(const ListDigraph::Arc &a, int label) const { return (*vecEArcId[label])[a]; }

        /** Returns the cost of an arc in a graph. @param a The arc. @param label The graph index. **/
        double getArcECost(const ListDigraph::Arc &a, int label) const  {return (*vecECost[label])[a]; } 

        /** Returns the original ARC of an node in a graph. @param a The arc. @param label The graph index. **/
        const ListDigraph::Arc & getNodeEArc(const ListDigraph::Node &n, int label) const  {return (*(*vecENodeArc[label])[n]); }

        /** Changes the cost of an arc in a graph. @param a The arc. @param d The graph index. **/
	    double getArcCost(const ListDigraph::Arc &a, int d) const { return (*cost[d])[a]; }

        /** Returns the source index of the graph corresponding to label**/
        int getIndexSource(int label){ return vecESourceIndex[label]; }

        /** Returns the destination of the graph corresponding to label**/
        int getIndexDestination(int label){ return vecEDestinationIndex[label]; }

        /** Returns the corresponding node for the heuristic auxiliary graph. @ param v The node from original graph d. @param d The graph index**/
        //ListDigraph::Node getNodeRef(const ListDigraph::Node &v, int d) const { return (*vecNodeRef[d])[v]; }

        ListDigraph::Node getNodeFromIndex(int, int);

        /** Returns the constraints slack module **/
        double getSlackModule(double = -1.0);

        /** Returns the constraints slack module considering the primal values **/
        double getSlackModule_v2(double =-1.0);
        
        /** Returns the constraints slack module considering the primal values **/
        double getMeanSlackModule_v2();

        /** Returns the constraints direction module **/
        double getDirectionModule();

        /** Returns the scalar product between the slack (gradient) and the direction **/
        double getSlackDirectionProd();

        double getSlackDirectionProdNormal();

        double getSlackDirectionProdProjected(Input::ProjectionType);

        /** Returns the product of the normal slack with the slack considering the primal variables **/
        double getSlackPrimalSlackProd(double = -1.0);


        /****************************************************************************************/
        /*										Setters											*/
        /****************************************************************************************/

        /** Changes the id of a node in a graph. @param n The node. @param label The graph index. @param val The new id. **/
        void setNodeEId(const ListDigraph::Node &n, int label, int val){ (*vecENodeID[label])[n] = val; }

        /** Changes the demand of a node in a graph. @param n The node. @param label The graph index. @param val The new demand. **/
        void setNodeEDemand(const ListDigraph::Node &n, int label, int val){ (*vecENodeDemand[label])[n] = val; }

        /** Changes the slice of a node in a graph. @param n The node. @param label The graph index. @param val The new slice. **/
        void setNodeESlice(const ListDigraph::Node &n, int label, int val){ (*vecENodeSlice[label])[n] = val; }

        /** Changes the firstconst of a node in a graph. @param n The node. @param label The graph index. @param val The new firstconst. **/
        void setNodeEFirstConst(const ListDigraph::Node &n, int label, int val){ (*vecENodeFirstConst[label])[n] = val; }

        /** Changes the id of an arc in a graph. @param n The node. @param label The graph index. @param val The new id. **/
        void setArcEId(const ListDigraph::Arc &a, int label, int val) { (*vecEArcId[label])[a] = val; }

        /** Changes the cost of an arc in a graph. @param a The arc. @param label The graph index. @param val The new length. **/
        void setArcECost(const ListDigraph::Arc &a, int label, double val) { (*vecECost[label])[a] = val; }

        /** Changes the cost of an arc in a graph. @param a The arc. @param d The graph index. @param val The new cost value. **/
	    void setArcCost(const ListDigraph::Arc &a, int d, double val) { (*cost[d])[a] = val; }

	    /** Increments the cost of an arc in a graph. @param a The arc. @param d The graph index. @param val The value to be added to cost. **/
	    void incArcCost(const ListDigraph::Arc &a, int d, double val) { (*cost[d])[a] += val; }

        /** Changes the firstconst of a node in a graph. @param n The node. @param label The graph index. @param val The new firstconst. **/
        void setNodeEArc(const ListDigraph::Node &n, int label, const ListDigraph::Arc &a){ (*vecENodeArc[label])[n] =  std::make_shared<ListDigraph::Arc>(a); }

        /** Changes the Index of a node in a graph. @param n The node. @param label The graph index. @param val The new index. **/
        void setNodeEIndex(const ListDigraph::Node &n, int label, int val){ (*vecENodeIndex[label])[n] = val; }

        /** Changes the source index of the graph corresponding to label**/
        void setIndexSource(int id, int label){ vecESourceIndex[label] = id; }

        /** Changes the destination index of the graph corresponding to label**/
        void setIndexDestination(int id, int label){ vecEDestinationIndex[label] = id; }

        /****************************************************************************************/
        /*										Update											*/
        /****************************************************************************************/

        /********************************* MULTIPLIERS ***********************************/

        /* Updates lagrangian multipliers with the rule: u[k+1] = u[k] + t[k]*violation */
        void updateMultiplier(double);

        /******** MULTIPLIER CONSIDERING THE STABILITY CENTER ************/

        void updateMultiplier_v2(double);

        /********************************* STABILITY CENTER ***********************************/

        void updateStabilityCenter();

        /********************************* SLACK ***********************************/

        void updateSlack(int, const ListDigraph::Arc &);

        void updatePrimalSlack(double);

        /********************************* DIRECTION ***********************************/

        void updateDirection();

        /********************************* COSTS ***********************************/

        /** Updates the arc costs. @note cost = coeff + u_d*length **/
        void updateCosts();

        /****************************************************************************************/
        /*					                  Display						                    */
        /****************************************************************************************/

        void displayEArc(int, const ListDigraph::Arc &);

        void displayEGraph(int);

        void displayAssignmentMatrix(int);

        void displayENode(const ListDigraph::Node &, int);
        void displaySlack(std::ostream & = std::cout);

        void displayMultiplier(std::ostream & = std::cout);
        void createGraphFile(int);

        /* *******************************************************************************
        *                             DESTRUCTOR
        ******************************************************************************* */

        ~lagNonOverlapping();
};




#endif