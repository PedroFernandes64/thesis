#ifndef LAG_VOLUME_H
#define LAG_VOLUME_H

#include "AbstractLagrangianSolver.h"

class lagVolume: public AbstractLagSolver{

    private:

        const int MAX_NB_IT_RED;                    /**< Maximum number of red iterations. **/
        const int MAX_NB_IT_YELLOW;                 /**< Maximum number of yellow iterations. **/
        const double UPD_STEPSIZE_GREEN_YELLOW;     /**< Update stepsize when green or consecutive yellow iterations. **/
        const double UPD_STEPSIZE_RED;              /**< Update stepsize when consecutive red iterations. **/
        const double MIN_MODULE_VALUE;              /**< If module smaller than this, stop. **/
        const double MIN_DIF_OBJ_VALUE;             /**< If difference between lower bound and primal cost smaller than this, stop. **/
        const int ASCENT_FIRST_CHECK;               /**< First iteration to see if it had improvement. **/
        const int ASCENT_CHECK_INVL;                /**< Diferrence between the iterations that must be compared to see if it had an improvement. **/
        const double MINIMUM_REL_ASCENT;            /**< Minimum relative improvement expected. */
        double MAX_ALPHA;                           /**< Initial maximum for alpha. **/

        double target;

        bool greenIt;
        int nbRedIt;
        int nbYellowIt;

        double previous_lb;

        std::vector<double> sequence_dualCost;

    public:
        /************************************************/
	    /*				    Constructors 		   		*/
	    /************************************************/
        lagVolume(const Instance &inst):AbstractLagSolver(inst),MAX_NB_IT_RED(20),MAX_NB_IT_YELLOW(2),UPD_STEPSIZE_GREEN_YELLOW(1.1),UPD_STEPSIZE_RED(0.67),MIN_MODULE_VALUE(0.01),MIN_DIF_OBJ_VALUE(0.001),ASCENT_FIRST_CHECK(500),ASCENT_CHECK_INVL(500),MINIMUM_REL_ASCENT(0.001),MAX_ALPHA(0.1){}

        /************************************************/
	    /*				    GETTERS      		   		*/
	    /************************************************/

        bool getGreenIt() const { return greenIt;}
        int getNbRedIt() const { return nbRedIt;}
        int getNbYellowIt() const {return nbYellowIt;}
        double getTarget() const { return target; }

        void getSolution(double *colsol) { formulation->getPrimalAppSolution(colsol);
                                           formulation->clearAssignmentMatrix();
                                           formulation->clearPrimalApproximationMatrix();
                                           formulation->clearSlacks(); }

        /************************************************/
	    /*				    SETTERS      		   		*/
	    /************************************************/

        void setGreenIt(bool b) {greenIt = b;}
        void setNbRedIt(int nb) {nbRedIt = nb;}
        void incNbRedIt() {nbRedIt++;}
        void setNbYellowIt(int nb) {nbYellowIt = nb;}
        void incNbYellowIt() {nbYellowIt++;}
        void setTarget(double val) { target=val; }

        /************************************************/
	    /*				    Methods      		   		*/
	    /************************************************/

        void initialization(bool=true);

        void run(bool=true,bool=false);

        void runIteration(bool=false);

        void updateLB(double);

        void updateParamAlpha();

        void updateLambda();

        void updateStepSize();

        double getAlphaValue();

        void updateTarget();

        /************************************************/
	    /*				    Display      		   		*/
	    /************************************************/

        void displayMainParameters(std::ostream & sortie);

};

#endif