#include "abstractSolver.h"


/****************************************************************************************/
/*										Constructor										*/
/****************************************************************************************/

/* Constructor. The RSA constructor is called and the arc map storing the index of the preprocessed graphs associated is built. */
AbstractSolver::AbstractSolver(const Instance &instance, const Status &s) : currentStatus(s) {
	time = -1;
	upperBound = -1;
	lowerBound = -1;
	gap = -1;
	treeSize = -1;
	rootValue = -1;
    FormulationFactory factory;
    formulation = factory.createFormulation(instance);
}

void AbstractSolver::updateRSA(Instance &instance){
    std::cout << "Update RSA" << std::endl;
    if (this->getStatus() == STATUS_OPTIMAL || this->getStatus() == STATUS_FEASIBLE){   
        std::cout << "Feasible" << std::endl;
        formulation->updatePath(this->getSolution());
        formulation->updateInstance(instance);
        instance.setWasBlocked(false);
        formulation->displayPaths();
        //Display OSNR corrected if Obj 10 chosen
        formulation->displayOSNR(instance);
        formulation->displayOFData(instance);
    }
    else{
        std::cout << "Decrease the number of demands to be treated." << std::endl;
        instance.decreaseNbDemandsAtOnce();
        instance.setWasBlocked(true);
    }
}


