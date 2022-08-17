#ifndef LAG_NEW_OVERLAPPING_H
#define LAG_NEW_OVERLAPPING_H

#include "AbstractLagrangianFormulation.h"
#include <lemon/core.h>
#include <lemon/capacity_scaling.h>

using namespace lemon;

class lagNewNonOverlapping:public AbstractLagFormulation{ 

    private:

        /******************************************* BUILDING AUXILIARY GRAPH **********************************************/

        /** A list of pointers to the extended graph associated with each EDGE to be routed. 
        \note (*vecEGraph[i]) is the graph associated with the i-th EDGE to be routed. **/
        std::vector<std::shared_ptr<ListDigraph>> vecEGraph;

        /** A list of pointers to the ArcMap storing the arc LEMON IDs of the graph associated with each EDGE to be routed. 
        \note (*vecEArcId[i])[a] is the LEMON ID of arc a in the graph associated with the i-th EDGE to be routed. **/
        std::vector<std::shared_ptr<ArcMap>> vecEArcId;

        /** A list of pointers to the ArcMap storing the arc INDEXs of the graph associated with each EDGE to be routed. 
        \note (*vecEArcIndex[i])[a] is the INDEX of arc a in the graph associated with the i-th EDGE to be routed. **/
        std::vector<std::shared_ptr<ArcMap>> vecEArcIndex;

        /** A list of pointers to the ArcMap storing the arc INDEXs of the arcs in graph DEMAND associated with each
         * arc in the graph EDGE to be routed. 
        \note (*vecEArcIndexArcGraphD[i])[a] is the INDEX of arc in graph DEMAND associated with the arc a in the graph 
        associated with the i-th DEMAND to be routed. **/
        std::vector<std::shared_ptr<ArcMap>> vecEArcIndexArcGraphD;

        /** A list of pointers to the ArcMap storing the node DEMANDS of the graph associated with EDGE to be routed
        \note (*vecEArcDemand[i])[a] is the DEMAND of arc a in the graph associated with the i-th EDGE to be routed. **/
        /** Indeed, here each arc refers to an arc in the graph for each d (G(D)), so, its the DEMAND of the original arc **/
        std::vector<std::shared_ptr<ArcMap>> vecEArcDemand;

        /** A list of pointers to the ArcMap storing the node SLICES of the graph associated with EDGE to be routed
        \note (*vecEArcSlice[i])[a] is the SLICE of arc a in the graph associated with the i-th EDGE to be routed. **/
        /** Indeed, here each arc refers to an arc in the graph for each d (G(D)), so, its the SLICE of the original arc **/
        std::vector<std::shared_ptr<ArcMap>> vecEArcSlice;

        /** A list of pointers to the map storing the arc COEFFs of the graph associated with each EDGE to be routed. 
        \note (*vecECoeff[i])[a] is the COEFF of arc a in the graph associated with the i-th EDGE to be routed. **/
        /** The cost is associated with the arc, this arc represents an arc in the  graph for each d (G(D)) */
        std::vector<std::shared_ptr<ArcCost>> vecECoeff;

        /** A list of pointers to the map storing the arc LENGTHs of the graph associated with each EDGE to be routed. 
        \note (*vecEArcLength[i])[a] is the LENGTH of arc a in the graph associated with the i-th EDGE to be routed. **/
        /** The cost is associated with the arc, this arc represents an arc in the  graph for each d (G(D)) */
        std::vector<std::shared_ptr<ArcCost>> vecEArcLength;

        /** A list of pointers to the map storing the node ORIGINAL ARCS of the graph associated with each EDGE to be routed. 
        \note (*vecEArcArc[i])[a] is the ARC of nde a in the graph associated with the i-th EDGE to be routed. **/
        /** The ARC is in the  graph for each d (G(D)) */
        std::vector<std::shared_ptr<ListDigraph::ArcMap<std::shared_ptr<ListDigraph::Arc>>>> vecEArcArc;

        /** A list of pointers to the map storing the arc SOURCE LABELs of the graph associated with each EDGE to be routed. 
        \note (*vecEArcSourceLabel[i])[a] is the SOURCE LABEL of arc a in the graph associated with the i-th EDGE to be routed. **/
        /** The source label is associated with the arc, this arc represents an arc in the  graph for each d (G(D)) */
        std::vector<std::shared_ptr<ArcMap>> vecEArcSourceLabel;

        /** A list of pointers to the map storing the arc TARGET LABELs of the graph associated with each EDGE to be routed. 
        \note (*vecEArcTargetLabel[i])[a] is the TARGET LABEL of arc a in the graph associated with the i-th EDGE to be routed. **/
        /** The target label is associated with the arc, this arc represents an arc in the  graph for each d (G(D)) */
        std::vector<std::shared_ptr<ArcMap>> vecEArcTargetLabel;

        /** A list of pointers to the map storing the arc SOURCE INDEXs of the graph associated with each EDGE to be routed. 
        \note (*vecEArcSourceIndex[i])[a] is the SOURCE INDEX of arc a in the graph associated with the i-th EDGE to be routed. **/
        /** The source index is associated with the arc, this arc represents an arc in the  graph for each d (G(D)) */
        std::vector<std::shared_ptr<ArcMap>> vecEArcSourceIndex;

        /** A list of pointers to the map storing the arc TARGET INDEXs of the graph associated with each EDGE to be routed. 
        \note (*vecEArcTargetIndex[i])[a] is the TARGET INDEX of arc a in the graph associated with the i-th EDGE to be routed. **/
        /** The target index is associated with the arc, this arc represents an arc in the  graph for each d (G(D)) */
        std::vector<std::shared_ptr<ArcMap>> vecEArcTargetIndex;

        /** Vector with the lemon id of the nodes for each graph E. vecENodeId[label][slice],
         *  where label is the edge graph and slice represent the slice the nodes represent **/
        std::vector<std::vector<std::shared_ptr<ListDigraph::Node> > >  vecENode;

        std::vector<int > vecTargetLabels;
        std::vector<int > vecSourceLabels;
        std::vector<double > vecMaxLength;

        std::vector<std::shared_ptr<ArcCost>> cost;
        
    public:

        /* *********************************************************************************************************
        *                                                    SETTERS
        ********************************************************************************************************** */
        void setArcEId(const ListDigraph::Arc & a, int label, int id) { (*vecEArcId[label])[a] = id;}
        void setArcEIndex(const ListDigraph::Arc & a, int label, int index) { (*vecEArcIndex[label])[a] = index;}
        void setArcEDemand(const ListDigraph::Arc & a, int label, int demand) { (*vecEArcDemand[label])[a] = demand;}
        void setArcESlice(const ListDigraph::Arc & a, int label, int slice) { (*vecEArcSlice[label])[a] = slice;}
        void setArcEIndexArcD(const ListDigraph::Arc & a, int label, int index) { (*vecEArcIndexArcGraphD[label])[a] = index;}
        void setArcELength(const ListDigraph::Arc & a, int label, double length){ (*vecEArcLength[label])[a] = length;}
        void setArcECoeff(const ListDigraph::Arc & a, int label, double coeff){ (*vecECoeff[label])[a] = coeff;}
        void setArcESourceLabel(const ListDigraph::Arc & a, int label, int source){ (*vecEArcSourceLabel[label])[a] = source;}
        void setArcETargetLabel(const ListDigraph::Arc & a, int label, int target){ (*vecEArcTargetLabel[label])[a] = target;}
        void setArcESourceIndex(const ListDigraph::Arc & a, int label, int source){ (*vecEArcSourceIndex[label])[a] = source;}
        void setArcETargetIndex(const ListDigraph::Arc & a, int label, int target){ (*vecEArcTargetIndex[label])[a] = target;}
        void setArcEArc(const ListDigraph::Arc &a, int label, const ListDigraph::Arc &arc){ (*vecEArcArc[label])[a] =  std::make_shared<ListDigraph::Arc>(arc); }

        /* *********************************************************************************************************
        *                                                    GETTERS
        ********************************************************************************************************** */

        int getArcEIndexArcD(const ListDigraph::Arc & a, int label) const { return (*vecEArcIndexArcGraphD[label])[a];}
        int getArcEDemand(const ListDigraph::Arc & a, int label) const { return (*vecEArcDemand[label])[a];}
        double getArcELength(const ListDigraph::Arc & a, int label) const { return (*vecEArcLength[label])[a];}
        int getArcESourceLabel(const ListDigraph::Arc & a, int label) const { return (*vecEArcSourceLabel[label])[a];}
        int getArcETargetLabel(const ListDigraph::Arc & a, int label) const { return (*vecEArcTargetLabel[label])[a];}
        int getArcESourceIndex(const ListDigraph::Arc & a, int label) const { return (*vecEArcSourceIndex[label])[a];}
        int getArcETargetIndex(const ListDigraph::Arc & a, int label) const { return (*vecEArcTargetIndex[label])[a];}
        const ListDigraph::Arc & getArcEArc(const ListDigraph::Arc &a, int label) const  {return (*(*vecEArcArc[label])[a]); }

        /* *********************************************************************************************************
        *                                         INITIALIZATION METHODS
        ********************************************************************************************************** */
        lagNewNonOverlapping(const Instance &instance):AbstractLagFormulation(instance){}

        /** Sets all initial parameters **/
        void init(bool=true);

        /********************************************** MULTIPLIERS ************************************************/
        
        void startMultipliers(double *,int,int);
        
        /** Sets the initial lagrangian multipliers values for the subgradient to run. **/
        void initMultipliers();

        /** Sets the initial lagrangian flow multipliers values using a warmstart procedure. **/
        void initMultipliersWarmstart();

        /******************************************** STABILITY CENTER *********************************************/

        /** Sets the initial lagrangian stability center values for the subgradient to run. **/
        void initStabilityCenter();

        /************************************************* SLACK ***************************************************/

        /** Initializes the slack of relaxed constraints. **/
        void initSlacks();

        void resetSlacks();

        void initPrimalSlacks();

        void clearSlacks();

        /*********************************************** DIRECTION *************************************************/

        /** Initializes the direction of relaxed constraints. **/
        void initDirection();

        /*************************************** BUILDING AUXILIARY GRAPH ******************************************/

        /** Constructs auxiliary graphs for each edge. **/
        void build_Graphs_e();

        /** Auxiliary method to add arcs to the auxiliary graphs per edge. **/
        void addArcE(int,int,int,int,double,double,int,int,int,int,const ListDigraph::Arc &);

        /* ****************************************************************************************************************
        *                                               UPDATE METHODS
        ***************************************************************************************************************** */

        /************************************************ MULTIPLIERS *****************************************************/

        /* Updates lagrangian multipliers with the rule: u[k+1] = u[k] + t[k]*violation */
        void updateMultiplier(double);

        /*********************************** MULTIPLIER CONSIDERING THE STABILITY CENTER **********************************/

        /* Updates lagrangian multipliers with the rule: u[k+1] = SC[k] + t[k]*violation */
        void updateMultiplier_v2(double);

        /************************************************ STABILITY CENTER *************************************************/

        /* Updates stability center: best result found so far. */
        void updateStabilityCenter();
        
        /**************************************************** SLACK ********************************************************/

        /* Updates relaxed constraints slacks */
        void updateSlack(int, const ListDigraph::Arc &);

        void updatePrimalSlack(double);

        /*************************************************** DIRECTION*******************************************************/

        /* Updates relaxed constraints direction */
        void updateDirection();

        /************************************************ ASSIGNMENT MATRIX *************************************************/

        /* Updates the assignment considering the obtained path result in each edge subproblem*/
        /* (label,path,source,target), label is the edge, path the result of the shortest path, source and target the nodes*/
        void updateAssignment_k(int, BellmanFordCostE &, const ListDigraph::Node &, const ListDigraph::Node &);

        /************************************************* CHECK FEASIBILITY *************************************************/
        
        /* Verifies if the relaxed constraints are respected. */
        bool checkFeasibility();

        /* Verifies if the relaxed constraints are respected, considering primal apprximation. */
        bool checkFeasibility_v2();

        /** Checks if the slackness condition is satisfied ( mu*(Ax-b) = 0) **/
        bool checkSlacknessCondition();

        /* ********************************************************************************************************************
        *                                                     GET METHODS
        ******************************************************************************************************************** */

        void getDualSolution(double *);
        
        /* Returns the real objective function cost of the result obtained with the shortest path (lagrangian subproblem)*/
        /* (label,path,source,target), label is the edge, path the result of the shortest path, source and target the nodes*/
        double getRealCostFromPath(int, BellmanFordCostE &, const ListDigraph::Node &, const ListDigraph::Node &);

        /** return ||slack||^2 **/
        double  getSlackModule(double = -1.0);

        /** return ||slack_v2||^2 (slack considering primal variables)**/
        double getSlackModule_v2(double = -1.0);

        /** return ||direction||^2 **/
        double getDirectionModule();

        /** return slack*direction **/
        double getSlackDirectionProd();

        /** return slack*direction **/
        double getSlackDirectionProdNormal();

        /** return slack*direction considering projected or improved direction **/
        double getSlackDirectionProdProjected(Input::ProjectionType projection);

        /** return ||slack_v2||/M, considering primal variables, M the number of relaxed constraints **/
        double getMeanSlackModule_v2();

        /** return slack*slack_v2 **/
        double getSlackPrimalSlackProd(double = -1.0);

        /* *********************************************************************************************************************
        *                                                 RUNING METHODS
        ********************************************************************************************************************* */
        
        /** One iteration of the Lagrangian relaxation **/
        void run(bool=false);

        void runGeneralObj();

        void runObj8();

        void subtractConstantValuesFromLagrCost();

        void solveProblemMaxUsedSliceOverall();

        /* *********************************************************************************************************************
        *                                                 DISPLAY METHODS
        ********************************************************************************************************************* */

        void displaySlack(std::ostream & = std::cout);

        void displayMultiplier(std::ostream & = std::cout);

        void updateCost();
};


#endif