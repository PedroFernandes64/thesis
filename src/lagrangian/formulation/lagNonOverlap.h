#ifndef LAG_NON_OVERLAP_H
#define LAG_NON_OVERLAP_H

#include "AbstractLagrangianFormulation.h"
#include <set>
#include <lemon/core.h>
#include <lemon/capacity_scaling.h>

using namespace lemon;

class lagNonOverlap: public AbstractLagFormulation{
    private:

        /***************************** ASSIGNMENT MATRIX *******************************/

        std::vector< std::vector<bool> > assignmentMatrix;

        //std::vector< std::vector<bool> > assignmentMatrix_d;

        /***************************** BUILDING AUXILIARY GRAPH *******************************/

        /** A pointer to the extended graph associated with the EDGES to be routed. **/
        std::shared_ptr<ListDigraph> EGraph;

        /** A pointer to the NodeMap storing the node LEMON IDs of the graph associated with the EDGES to be routed. 
        \note (*ENodeID[v]) is the LEMON ID of node v in the graph associated with the EDGE to be routed. **/
        std::shared_ptr<NodeMap> ENodeID;

        /** A pointer to the NodeMap storing the node INDEX of the graph associated with the EDGES to be routed. 
        \note (*ENodeID[v]) is the LEMON ID of node v in the graph associated with the EDGES to be routed. **/
        /* using in the assigment matrix */
        std::shared_ptr<NodeMap> ENodeIndex;

        /** A pointer to the NodeMap storing the node DEMANDS of the graph associated with the EDGES to be routed
        \note (*ENodeDemand[v]) is the DEMAND of node v in the graph associated with the EDGES to be routed. **/
        /** Indeed, here each node refers to an arc in the graph for each d (G(D)), so, its the DEMAND of the original arc **/
        std::shared_ptr<NodeMap> ENodeDemand;

        /** A pointer to the NodeMap storing the node DIRECTION of the graph associated with the EDGES to be routed
        \note (*ENodeDemand[v]) is the DIRECTION of node v in the graph associated with the EDGES to be routed. **/
        /** Indeed, here each node refers to an arc in the graph for each d (G(D)), so, its the DIRECTION of the original arc **/
        /** edge 1--2 : (direction -1) -> (2->1) , (direction 1) -> (1->2), for atificial source and destination is 0 **/
        std::shared_ptr<NodeMap> ENodeDirection;

        /** A pointer to the NodeMap storing the node SLICES of the graph associated with the EDGES to be routed
        \note (*ENodeSlice[v]) is the SLICE of node v in the graph associated with the EDGES to be routed. **/
        /** Indeed, here each node refers to an arc in the graph for each d (G(D)), so, its the SLICE of the original arc **/
        std::shared_ptr<NodeMap> ENodeSlice;

        /** A pointer to the NodeMap storing the node FIRST CONSTRAINT of the graph associated with the EDGES to be routed
        \note (*ENodeFirstConst[v]) is the INDEX OF THE FIRST CONSTRAINT of node v in the graph associated with the
        EDGES to be routed. **/
        /** Indeed, here each node refers to an arc in the graph for each d (G(D)), so, its the FIRST CONSTRAINT of the original arc **/
        std::shared_ptr<NodeMap> ENodeFirstConst;

        /** A pointer to the ArcMap storing the arc ids of the graph associated with the EDGES to be routed. 
        \note (*EArcId[i])[a] is the id of arc a in the graph associated with the i-th demand to be routed. **/
        std::shared_ptr<ArcMap> EArcId;

        /*****************************  COSTS *******************************/

        /** A lpointer to the map storing the arc COSTS of the graph associated with the EDGES to be routed. 
        \note (*ECost[a]) is the COST of arc a in the graph associated with the EDGES to be routed. **/
        /** The cost is associated with the destination node of a, this node represents an arc in the  graph for each d (G(D)) */
        /* Refers to the cost of an arc during iteration k. cost = c_{ij} + u_k*length_{ij} */
        std::shared_ptr<ArcCost> ECost;

        /* This is the cost of the original variables, corresponding to the original graph G(D)*/
        std::vector<std::shared_ptr<ArcCost>> cost;  

        /* Vector of the id of the node source for each edge*/
        int ESourceId;

        /* Vector of the id of the destination node for each edge*/
        int EDestinationId;
    
    public:
        /* *******************************************************************************
        *                             INITIALIZATION METHODS
        ******************************************************************************* */
        lagNonOverlap(const Instance &instance):AbstractLagFormulation(instance){}

        /** Sets all initial parameters **/
        void init();

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

        void clearSlacks();

       /********************************** DIRECTION ***************************************/

        void initDirection();

        /***************************** BUILDING AUXILIARY GRAPH *******************************/

        /** Constructs the auxiliary graphs for each edge e **/
        void build_Graph_E();

        /** Add a node in the auxiliary graph **/
        void addENode(int,int,int,int);

        /******************************** COSTS *********************************/

        /** Initializes the costs in the objective function. **/
        void initCosts();

        /******************************** ASSIGMENT MATRIX *********************************/

        /** Initializes the assignement matrix. **/
        void initAssignmentMatrix();

        /* *******************************************************************************
        *                             RUNING METHODS
        ******************************************************************************* */
        void run();

        void subtractConstantValuesFromLagrCost();

        void solveProblemMaxUsedSliceOverall();

        /** Checks with the slacks if the solution is feasible. **/
        bool checkFeasibility();

        /****************************************************************************************/
        /*										Getters 										*/
        /****************************************************************************************/
        
        /** Returns the LEMON id of a node in a graph. @param n The node. **/
        int getNodeEId(const ListDigraph::Node &n) const { return (*ENodeID)[n]; }

        /** Returns the index of a node in a graph. @param n The node. **/
        int getNodeEIndex(const ListDigraph::Node &n) const { return (*ENodeIndex)[n]; }

        /** Returns the demand of a node in a graph. @param n The node. **/
        int getNodeEDemand(const ListDigraph::Node &n) const { return (*ENodeDemand)[n]; }

        /** Returns the direction of a node in a graph. @param n The node. **/
        int getNodeEDirection(const ListDigraph::Node &n) const { return (*ENodeDirection)[n];}

        /** Returns the slice of a node in a graph. @param n The node. **/
        int getNodeESlice(const ListDigraph::Node &n) const { return (*ENodeSlice)[n]; }

        /** Returns the firstconst of a node in a graph. @param n The node. @param label The graph index. **/
        int getNodeEFirstConst(const ListDigraph::Node &n) const{ return (*ENodeFirstConst)[n]; }

        /** Returns the LEMON id of an arc in a graph. @param a The arc. **/
        int getArcEId(const ListDigraph::Arc &a) const { return (*EArcId)[a]; }

        /** Returns the cost of an arc in a graph. @param a The arc.  **/
        double getArcECost(const ListDigraph::Arc &a) const  {return (*ECost)[a]; } 

        /** Changes the cost of an arc in a graph. @param a The arc. @param d The graph index. **/
	    double getArcCost(const ListDigraph::Arc &a, int d) const { return (*cost[d])[a]; }

        /** Returns the source index of the graph corresponding to label**/
        int getIdSource(){ return ESourceId; }

        /** Returns the destination of the graph corresponding to label**/
        int getIdDestination(){ return EDestinationId; }

        /** Returns the first nod with the given id**/
        ListDigraph::Node getNodeFromIndex(int);

        /** Compute the cost of the path considering the real objective function **/
        double getRealCostFromPath(int, BellmanFord< ListDigraph, ListDigraph::ArcMap<double> > &, const ListDigraph::Node &, const ListDigraph::Node &);

        /** Returns the constraints slack module **/
        double getSlackModule();

        /** Returns the constraints slack module considering the primal values **/
        double getSlackModule_v2();
        
        /** Returns the constraints slack module considering the primal values **/
        double getMeanSlackModule_v2();

        /** Returns the constraints direction module **/
        double getDirectionModule();

        /** Returns the scalar product between the slack (gradient) and the direction **/
        double getSlackDirectionProd();

        double getSlackDirectionProdNormal();

        double getSlackDirectionProdProjected(Input::ProjectionType);

        /** Returns the product of the normal slack with the slack considering the primal variables **/
        double get_prod_slack();

        /****************************************************************************************/
        /*										Setters											*/
        /****************************************************************************************/
        /** Changes the id of a node in a graph. @param n The node. @param val The new id. **/
        void setNodeEId(const ListDigraph::Node &n, int val){ (*ENodeID)[n] = val; }

        /** Changes the index of a node in a graph. @param n The node. @param val The new id. **/
        void setNodeEIndex(const ListDigraph::Node &n, int val){ (*ENodeIndex)[n] = val; }

        /** Changes the demand of a node in a graph. @param n The node. @param val The new demand. **/
        void setNodeEDemand(const ListDigraph::Node &n, int val){ (*ENodeDemand)[n] = val; }

        /** Changes the direction of a node in a graph. @param n The node. @param val The new demand. **/
        void setNodeEDirection(const ListDigraph::Node &n, int val){ (*ENodeDirection)[n] = val;}

        /** Changes the slice of a node in a graph. @param n The node. @param val The new slice. **/
        void setNodeESlice(const ListDigraph::Node &n, int val){ (*ENodeSlice)[n] = val; }

        /** Changes the firstconst of a node in a graph. @param n The node. @param val The new firstconst. **/
        void setNodeEFirstConst(const ListDigraph::Node &n, int val){ (*ENodeFirstConst)[n] = val; }

        /** Changes the id of an arc in a graph. @param n The node. @param val The new id. **/
        void setArcEId(const ListDigraph::Arc &a,int val) { (*EArcId)[a] = val; }

        /** Changes the cost of an arc in a graph. @param a The arc. @param val The new length. **/
        void setArcECost(const ListDigraph::Arc &a, double val) { (*ECost)[a] = val; }

        /** Changes the cost of an arc in a graph. @param a The arc. @param d The graph index. @param val The new cost value. **/
	    void setArcCost(const ListDigraph::Arc &a, int d, double val) { (*cost[d])[a] = val; }

	    /** Increments the cost of an arc in a graph. @param a The arc. @param d The graph index. @param val The value to be added to cost. **/
	    void incArcCost(const ListDigraph::Arc &a, int d, double val) { (*cost[d])[a] += val; }

        /** Changes the source index of the graph **/
        void setIdSource(int id){ ESourceId = id; }

        /** Changes the destination index of the graph **/
        void setIdDestination(int id){ EDestinationId= id; }

        /****************************************************************************************/
        /*										Update											*/
        /****************************************************************************************/

        /********************************* MULTIPLIERS ***********************************/

        /* Updates lagrangian multipliers with the rule: u[k+1] = u[k] + t[k]*violation */
        void updateMultiplier(double);

        /********************* MULTIPLIER CONSIDERING THE STABILITY CENTER *****************/

        void updateMultiplier_v2(double);

        /********************************* STABILITY CENTER ***********************************/

        void updateStabilityCenter();

        /********************************* SLACK ***********************************/

        void updateSlack();

        /********************************* DIRECTION ***********************************/

        void updateDirection();

        /********************************* COSTS ***********************************/

        /** Updates the arc costs. @note cost = coeff + u_d*length **/
        void updateCosts();

         /** Updates the arc costs acording to the analysed label. @note cost = coeff + u_d*length **/
        void updateCost_E(int);

        /********************************* ASSIGNMENT MATRIX ***********************************/

        void updateAssignment_e(int, BellmanFord< ListDigraph, ListDigraph::ArcMap<double> > &, const ListDigraph::Node &, const ListDigraph::Node &);

        void updateAssignment_d();

        /****************************************************************************************/
        /*										Display											*/
        /****************************************************************************************/

        void displaySlack(std::ostream & = std::cout);

        void displayMultiplier(std::ostream & = std::cout);

        void createGraphFile(int, int);

        void displayEArc(int, const ListDigraph::Arc &);

        void displayEGraph(int);

        void displayAssignmentMatrix(int);

        void displayENode(const ListDigraph::Node &, int);

        /* *******************************************************************************
        *                             DESTRUCTOR
        ******************************************************************************* */

        ~lagNonOverlap();
};

#endif