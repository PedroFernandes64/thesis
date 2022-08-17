#ifndef LAG_SUBGRADIENT_H
#define LAG_SUBGRADIENT_H

#include "AbstractLagrangianSolver.h"

class lagSubgradient: public AbstractLagSolver{

    public:

        /************************************************/
	    /*				    Constructors 		   		*/
	    /************************************************/
        lagSubgradient(const Instance &inst):AbstractLagSolver(inst){}

        void getSolution(double *colsol) { formulation->getPrimalSolution(colsol);
                                           formulation->clearAssignmentMatrix();
                                           formulation->clearBestFeasibleSolution();
                                           formulation->clearSlacks();  }
        
        /************************************************/
        /*					   Update 		    		*/
        /************************************************/
        
        /* Updates the lambda used in the update of step size. Lambda is halved if LB has failed to increade in some fixed number of iterations */
        void updateLambda();

        void updateLB(double);

        void updateStepSize();

        /************************************************/
        /*					   Methods 		    		*/
        /************************************************/

        /** Sets all the initial parameters for the subgradient to run. **/
        void initialization(bool=true);

        /** Runs the subgradient method. **/
        void run(bool=true,bool=false);

        /** Solves an iteration of the Subgradient Method. **/
        void runIteration(bool=false);

        /************************************************/
        /*					   Display 		    		*/
        /************************************************/

        void displayMainParameters(std::ostream & = std::cout);

        ~lagSubgradient(){}

};

#endif