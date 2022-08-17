#ifndef ABSTRACT_LAG_SOLVER_H
#define ABSTRACT_LAG_SOLVER_H

#include "../formulation/lagFormulationFactory.h"
#include "../heuristic/heuristicFactory.h"
#include "../../tools/clockTime.h"
#include "../../topology/instance.h"

/**********************************************************************************************
 * This is an abstract class modelling a Lagrangian solver. Derived classes need to specify how the 
 * RSA formulation is solved, i.e., which lagrangian formulation is used.
 * *******************************************************************************************/

class AbstractLagSolver{
    public:
        /** Enumerates the possible algorithm status according to the current model and solution in hand. **/
        enum Status {
            STATUS_UNKNOWN	= 0,        /**< The algorithm has no information about the solution of the model. **/					
            STATUS_FEASIBLE = 1,        /**< The algorithm found a feasible solution that may not necessarily be optimal. **/
            STATUS_OPTIMAL = 2,         /**< The algorithm found an optimal solution. **/
            STATUS_INFEASIBLE = 3,      /**< The algorithm proved the model infeasible; that is, it is not possible to find a feasible solution. **/
            STATUS_UNBOUNDED = 4,       /**< The algorithm proved the model unbounded. **/
            STATUS_INFEASIBLE_OR_UNBOUNDED = 5, /**< The model is infeasible or unbounded. **/
            STATUS_ERROR = 6,           /**< An error occurred. **/
            STATUS_MAX_IT = 7,          /**< Maximum number of iterations.**/
            STATUS_ABORTED = 8          /**< .**/
        };

    protected:
        
        const int MAX_NB_IT_WITHOUT_IMPROVEMENT;    /**< Maximum number of iterations without lower bound improvement. **/
        int MAX_NB_IT;                              /**< Maximum number of performed iterations. **/
        const double INITIAL_STEPSIZE;
        const double MIN_STEPSIZE;
        double PRIMAL_ABS_PRECISION;
        double UBINIT;
        double DUAL_LIMIT;
        bool feasibleHeuristic;

        Status currentStatus;
        bool dualinf;
        ClockTime time;
        ClockTime generalTime;
        AbstractLagFormulation *formulation;
        AbstractHeuristic *heuristic;

        double formulationConstTime;
        double heuristicConstTime;
        double initializationTime;
        double constAuxGraphTime;
        double solvingSubProblemTime;
        double updatingSlackTime;
        double updatingBoundsTime;
        double heuristicBoundTime;
        double updatingMultipliersTime;
        double updatingCostsTime;
        double stoppingCriterionTime;
        double updatingPrimalVariablesTime;
        double updateVariablesTime;
        double ShorstestPathTime;
        double substractMultipliersTime;
        double updateStepLambdaTime;
        double costTime;
        double RSAGraphConstructionTime;
        double PreprocessingTime;

        double totalTime;

        int iteration;
        int itWithoutImprovement;
        int globalItWithoutImprovement;

        double UB;
        double LB;
        double currentLagrCost;
        double currentRealCost;

        /** Stores the value of the step size used for updating the Lagrangian multipliers. **/
        double stepSize;

        /** Stores the value of lambda used for updating the step size. **/
        double lambda;

        std::string stop;

        std::ofstream fichier;
        std::ofstream fichier2;

    public:
        /****************************************************************************************/
        /*										Constructor										*/
        /****************************************************************************************/

        /** Constructor. The RSA constructor is called and the arc map storing the index of the preprocessed graphs associated is built. @param inst The instance to be solved. **/
        AbstractLagSolver(const Instance &instance, const Status &s = STATUS_UNKNOWN);

        /****************************************************************************************/
        /*											Getters										*/
        /****************************************************************************************/

        /** Returns the status. **/
        Status getStatus() const {return currentStatus;}

        AbstractLagFormulation* getLagrangianFormulation() {return formulation;}

        std::string getStop() const { return stop;}

        int getNbMaxIterations() const {return MAX_NB_IT; }
        int& getNbMaxIterations() {return MAX_NB_IT; } /*for the hot start in the OsiLagSolverInterface */
        double getPrimalAbsPrecision() const { return PRIMAL_ABS_PRECISION;}
        double getUBInit() const { return UBINIT;}
        double getDualLimit() const { return DUAL_LIMIT;}
        bool getDualInf() const { return dualinf;}

        ClockTime getGeneralTime() const { return generalTime;}

        double getFormulationConstTime() const { return formulationConstTime; }
        double getHeuristicConstTime() const { return heuristicConstTime; }
        double getInitializationTime() const { return initializationTime; }
        double getConstAuxGraphTime() const { return constAuxGraphTime; }
        double getSolvingSubProblemTime() const { return solvingSubProblemTime;}
        double getUpdatingSlackTime() const { return updatingSlackTime; }
        double getUpdatingBoundsTime() const { return updatingBoundsTime; }
        double getUpdatingHeuristicBoundTime() const { return heuristicBoundTime;}
        double getUpdatingMultipliersTime() const{ return updatingMultipliersTime;}
        double getUpdatingCostsTime() const { return updatingCostsTime;}
        double getStoppingCriterionTime() const{ return stoppingCriterionTime;}
        double getUpdatingPrimalVariablesTime() const { return updatingPrimalVariablesTime;}
        double getTotalTime() const {return totalTime;}
        double getUpdateVariablesTime() const { return updateVariablesTime;}
        double getShorstestPathTime() const { return ShorstestPathTime;}
        double getSubstractMultipliersTime() const { return substractMultipliersTime;}
        double getUpdateStepLambdaTime() const{ return updateStepLambdaTime;}  
        double getCostTime() const{ return costTime;}  
        double getRSAGraphConstructionTime() const { return RSAGraphConstructionTime;}
        double getPreprocessingTime() const { return PreprocessingTime;}


        int getIteration() const { return iteration; }
        int getItWithoutImprovement() const { return itWithoutImprovement; }

        int getGlobalItWithoutImprovement() const { return globalItWithoutImprovement;}

        double getUB() const { return UB; }
        double & getUBValue() { return UB;}
        double getLB() const { return LB; }

        double getStepSize() const { return stepSize; }
        double getLambda() const { return lambda; }

        virtual void getSolution(double *)=0;

        void getBestSolution(double *feaSol_) { formulation->getBestFeasibleSolution(feaSol_);
                                                formulation->clearBestFeasibleSolution();}

        void getDualSolution(double *rowprice) { formulation->getDualSolution(rowprice);}

        /************************************************/
        /*					   Setters 		    		*/
        /************************************************/

        /** Changes the status. **/
        void setStatus(const Status &s){ currentStatus = s; }

        void setStop(std::string s){ stop = s;}

        void setNbMaxIterations(int value) {MAX_NB_IT = value;}
        void setPrimalAbsPrecision(double value) { PRIMAL_ABS_PRECISION = value;}
        void setUBInit(double value) { UBINIT = value;}
        void setDualLimit(double value) { DUAL_LIMIT = value;}
        void setDualInf(bool value) { dualinf = value;}

        void setFormulationConstTime(double value) { formulationConstTime = value; }
        void setHeuristicConstTime(double value) { heuristicConstTime = value; }
        void setInitializationTime(double value) { initializationTime = value; }
        void setConstAuxGraphTime(double value) { constAuxGraphTime = value;}
        void setSolvingSubProblemTime(double value) { solvingSubProblemTime = value;}
        void incSolvingSubProblemTime(double value) { solvingSubProblemTime += value;}
        void setUpdatingSlackTime(double value) {updatingSlackTime = value;}
        void incUpdatingSlackTime(double value) {updatingSlackTime += value;}
        void setUpdatingBoundsTime(double value) {updatingBoundsTime = value;}
        void incUpdatingBoundsTime(double value) {updatingBoundsTime += value;}
        void setHeuristicBoundTime(double value) {heuristicBoundTime = value;}
        void incHeuristicBoundTime(double value) {heuristicBoundTime += value;}
        void setUpdatingMultipliersTime(double value) {updatingMultipliersTime = value;}
        void incUpdatingMultipliersTime(double value) {updatingMultipliersTime += value;}
        void setUpdatingCostsTime(double value) {updatingCostsTime = value;}
        void incUpdatingCostsTime(double value) {updatingCostsTime += value;}
        void setStoppingCriterionTime(double value) {stoppingCriterionTime = value;}
        void incStoppingCriterionTime(double value) {stoppingCriterionTime += value;}
        void setUpdatingPrimalVariablesTime(double value) {updatingPrimalVariablesTime = value;}
        void incUpdatingPrimalVariablesTime(double value) {updatingPrimalVariablesTime += value;}
        void setUpdateVariablesTime(double value) { updateVariablesTime = value; }
        void incUpdateVariablesTime(double value) { updateVariablesTime += value; }
        void setShorstestPathTime(double value) { ShorstestPathTime = value; }
        void incShorstestPathTime(double value) { ShorstestPathTime += value; }
        void setSubstractMultipliersTime(double value) { substractMultipliersTime = value;}
        void incSubstractMultipliersTime(double value) { substractMultipliersTime+=value;}
        void setUpdateStepLambdaTime(double value) { updateStepLambdaTime=value;}
        void incUpdateStepLambdaTime(double value) {updateStepLambdaTime+=value;}
        void setCostTime(double value) { costTime=value;}
        void incCostTime(double value) {costTime+=value;}
        void setRSAGraphConstructionTime(double val) { RSAGraphConstructionTime = val;}
        void setPreprocessingTime(double val) { PreprocessingTime = val;}

        void setTotalTime(double value) { totalTime = value;}

        void setIteration(int i) { iteration = i; }
        void setItWithoutImprovement(int i) { itWithoutImprovement = i; }
        void incIteration() { iteration++; }
        void incItWithoutImprovement() { itWithoutImprovement++; }

        void setGlobalItWithoutImprovement(int value) {globalItWithoutImprovement = value;}
        void incGlobalItWithoutImprovement() {globalItWithoutImprovement++;}

        void setUB(double val){ UB = val; }
        void setLB(double val){ LB = val; }
       
        /** Changes the value of lambda. @param val The new value of lambda. **/
        void setLambda(double val){ lambda = val; }

        void setStepSize(double val){ stepSize = val; }

        /************************************************/
        /*					   Update 		    		*/
        /************************************************/

        /* Updates the known lower bound. */
        virtual void updateLB(double bound) = 0;

        /* Updates the known upper bound. */
        void updateUB(double bound);

        /* Updates the step size with the rule: lambda*(UB - Z[u])/|slack| */
        virtual void updateStepSize() = 0;
        
        /* Updates the lambda used in the update of step size. Lambda is halved if LB has failed to increade in some fixed number of iterations */
        virtual void updateLambda() = 0;

        /************************************************/
        /*					   Methods 		    		*/
        /************************************************/

        /* Initializes the variables, constraints and objective function of the studied formulation. */
        void initLagFormulation();

        /** Sets the initial lambda used for updating the step size. **/
        void initLambda();

        bool isGradientMoving();

        /** Sets all the initial parameters for the subgradient to run. **/
        virtual void initialization(bool=true) = 0;

        /** Runs the subgradient method. **/
        virtual void run(bool=true,bool=false) = 0;

        /** Solves an iteration of the Subgradient/Volume Method. **/
        virtual void runIteration(bool=false) = 0;

        /************************************************/
        /*					   Display 		    		*/
        /************************************************/

        virtual void displayMainParameters(std::ostream & = std::cout) = 0;

        void displayResults(std::ostream & = std::cout);

        /************************************************/
        /*				   Destructors 		    		*/
        /************************************************/

        virtual ~AbstractLagSolver();


};

#endif