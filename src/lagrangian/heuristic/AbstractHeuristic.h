#ifndef ABSTRACT_HEURISTIC_H
#define ABSTRACT_HEURISTIC_H

#include "../formulation/AbstractLagrangianFormulation.h"
#include "../../tools/clockTime.h"
#include "../tools/lagTools.h"
#include <set>
//#include "formulation/lagFormulationFactory.h"

class AbstractLagFormulation;

class AbstractHeuristic{

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

        AbstractLagFormulation* formulation;

        double currentHeuristicCost;

        /* Maximum number of changes -> stops the heuristic and does not return a solution. */
        bool maxChangesPossible;

        /* A vector for the heuristic solution */
        std::vector<std::vector<bool>> heuristicSolution;

        double varP;

        std::vector<std::shared_ptr<IterableBoolMap<ListDigraph, ListDigraph::Arc>>> heuristicSolutionItBoolMap;

        Status statusheuristic;

        ClockTime time;
        double timeAux;
        bool infeasibleByBound;

    public:

        /* *******************************************************************************
        *                              CONSTRUCTOR
        ******************************************************************************* */

        AbstractHeuristic(AbstractLagFormulation* form, const Status &s = STATUS_UNKNOWN);

        /* *******************************************************************************
        *                              GETTERS
        ******************************************************************************* */

        /** Returns the status. **/
        Status getStatus() const {return statusheuristic;}

        bool getInfeasibilityByBound() const {return infeasibleByBound;}

        /* Get the current heuristic solution cost */
        double getCurrentHeuristicCost() const { return currentHeuristicCost;}

        double getTime() const { return timeAux;}

        const std::vector<std::vector<bool>> & getSolution() const { return heuristicSolution;}
        double getVarP() const {return varP;}

        /* *******************************************************************************
        *                              SETTERS
        ******************************************************************************* */

        /** Changes the status. **/
        void setStatus(const Status &s){ statusheuristic = s; }

        /* Set the current heuristic solution cost */
        void setCurrentHeuristicCost(double val){currentHeuristicCost = val;}

        void incCurrentHeuristicCost(double val){currentHeuristicCost += val;}

        /* *******************************************************************************
        *                                 METHODS
        ******************************************************************************* */

        virtual void init(bool=true) = 0;

        virtual void initAdaptedCosts(const double *) = 0;

        void initSolution();

        virtual void run(bool=false,bool=true) = 0;

        void updateCostFromHeuristic();

        double * getAdaptedSolution();

        void printSolution();

        void display();

        /* *******************************************************************************
        *                                DESTRUCTOR
        ******************************************************************************* */

        virtual ~AbstractHeuristic();

};

#endif