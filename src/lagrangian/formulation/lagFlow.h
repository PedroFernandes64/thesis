#ifndef LAG_FLOW_H
#define LAG_FLOW_H

#include "AbstractLagrangianFormulation.h"

using namespace lemon;

class lagFlow :public AbstractLagFormulation{ 

    public:

        /* **************************************************************************************************************
        *                                             CONSTRUCTOR
        ************************************************************************************************************** */
        lagFlow(const Instance &instance):AbstractLagFormulation(instance){}

        /* **************************************************************************************************************
        *                                         INITIALIZATION METHODS
        ************************************************************************************************************** */

        /** Sets all initial parameters **/
        void init(bool=true);

        /*********************************************** MULTIPLIERS ***************************************************/

        void startMultipliers(double *,int,int);

        /** Sets the initial lagrangian multipliers values for the subgradient to run. **/
        void initMultipliers();

        void clearDualSolution();

        /********************************************** STABILITY CENTER ************************************************/

        /** Initializes the stability center of relaxed constraints. **/
        void initStabilityCenter();

        /*************************************************** SLACK ******************************************************/

        /** Initializes the slack of relaxed constraints. **/
        void initSlacks();

        void resetSlacks();

        void initPrimalSlacks();

        void clearSlacks();

        /************************************************** DIRECTION ****************************************************/

        /** Initializes the direction of relaxed constraints. **/
        void initDirection();

        /* *****************************************************************************************************************
        *                                                  RUNING METHODS
        ***************************************************************************************************************** */

        /** Solves the lagrangian flow formulation sub problem. **/
        void run(bool=false);

        /** Solves the lagrangian flow formulation sub problem for general objective functions. **/
        void runGeneralObj();

        /** Solves the lagrangian formulation modified sub problem for objective functions. **/
        void runAdaptedGeneralObj();

        /** Solves the lagrangian flow formulation sub problem for objective 8. **/
        void runObj8();

        /** Substract constant values from the current lagrangian cost. **/
        void subtractConstantValuesFromLagrCost();

        /** SOlve sub problem when objective 8 is chosen. Minimizing the maximum slice overall. **/
        void solveProblemMaxUsedSliceOverall();

        /** Checks if sub problem solution is feasible. **/
        bool checkFeasibility();

        /** Checks if primal approximation solution is feasible. **/
        bool checkFeasibility_v2();

        /** Checks if the slackness condition is satisfied ( mu*(Ax-b) = 0) **/
        bool checkSlacknessCondition();

        /******************************************************************************************************************/
        /*										               Getters 										              */
        /******************************************************************************************************************/

        void getDualSolution(double *);

        /** Returns the cost considering the objective function coefficient of the resulting sub problem solution **/
        double getRealCostFromPath(int d, DijkstraCost &path, const ListDigraph::Node &SOURCE, const ListDigraph::Node &TARGET);

        double getRealCostFromPath(int d, CapacityScaling<ListDigraph,int,double> &costScale, const ListDigraph::Node &SOURCE, const ListDigraph::Node &TARGET);

        
        /* Returns the physical length of the path. */
        double getPathLength(int d, DijkstraCost &path, const ListDigraph::Node &s, const ListDigraph::Node &t);
        
        /* Returns the actual cost of the path according to the metric used. */
        double getPathCost(int d, DijkstraCost &path, const ListDigraph::Node &s, const ListDigraph::Node &t);

        /****************************************************** MODULES ****************************************************/
        
        /* Returns |slack|^2 */
        double getSlackModule(double = -1.0);

        /* Returns |slack|^2, with the slack considering the primal variables*/
        double getSlackModule_v2(double = -1.0);

        /* Returns |Direction|^2 */
        double getDirectionModule();

        /* Returns (Slack*Direction) */
        double getSlackDirectionProd();

        /* Returns (Slack*Direction), projected or improved direction */
        double getSlackDirectionProdProjected(Input::ProjectionType);

        /* Returns (Slack*Direction), normal direction */
        double getSlackDirectionProdNormal();

        /* Returns (slack*slack_v2)*/ 
        double getSlackPrimalSlackProd(double = -1.0);

        /* Returns mean of |slack|, slack considering the primal variables */
        double getMeanSlackModule_v2();
       
        /*******************************************************************************************************************/
        /*										               Update								           			   */
        /*******************************************************************************************************************/

        /*************************************************** MULTIPLIERS ***************************************************/

        /* Updates lagrangian multipliers with the rule: u[k+1] = u[k] + t[k]*violation */
        void updateMultiplier(double);

        /************************************** MULTIPLIER CONSIDERING THE STABILITY CENTER ********************************/

        /* Updates lagrangian multipliers with the rule: u[k+1] = stability center[k] + t[k]*violation */
        void updateMultiplier_v2(double);

        /************************************************** STABILITY CENTER ***********************************************/

        /* Updates stability center */
        void updateStabilityCenter();
        
        /********************************************* SLACK PRIMAL APPROXIMATION ********************************************/

        void updatePrimalSlack(double);

        /****************************************************** DIRECTION****************************************************/

        /* Updates direction */
        void updateDirection();

        /************************************************** ASSIGNMENT MATRIX **********************************************/

        /** Updates the assignment of a demand based on the a given path. @param d The d-th demand. @param path The path on which the demand is routed. @param SOURCE The path's source. @param TARGET The path's target. **/
        void updateAssignment_k(int d, DijkstraCost &path, const ListDigraph::Node &SOURCE, const ListDigraph::Node &TARGET);

        void updateAssignment_k(int d, DijkstraCostObj8 &path, const ListDigraph::Node &SOURCE, const ListDigraph::Node &TARGET);

        void updateAssignment_k(int d, CapacityScaling<ListDigraph,int,double> &, const ListDigraph::Node &, const ListDigraph::Node &);

        
        /********************************************************************************************************************/
        /*										                Display											            */
        /********************************************************************************************************************/
    
        /* Verifies if optimality condition has been achieved and update STOP flag. */
        void updateStop(bool &STOP);

        /* Tests if CSP is feasible by searching for a shortest path with arc costs based on their physical length. */
        bool testFeasibility(const ListDigraph::Node &s, const ListDigraph::Node &t);
        
        /* Assigns length as the main cost of the arcs. */
        void setLengthCost();
        
        /* Stores the solution found in the arcMap onPath. */
        void updateOnPath();
        
        std::string getPathString(int d, DijkstraCost &path, const ListDigraph::Node &s, const ListDigraph::Node &t);

        void displayMultiplier(std::ostream & = std::cout);

        void displaySlack(std::ostream & = std::cout);

        /* *****************************************************************************************************************
        *                                                     DESTRUCTOR
        ***************************************************************************************************************** */

        ~lagFlow(){}
};  


#endif 