#ifndef LAG_SOLVER_FACTORY_H
#define LAG_SOLVER_FACTORY_H

// include all concrete solvers
#include "lagSubgradient.h"
#include "lagVolume.h"

class lagSolverFactory{
    public:
        inline AbstractLagSolver* createSolver(const Instance &instance){
            Input::NodeMethod chosenNodeMethod = instance.getInput().getChosenNodeMethod();
            switch (chosenNodeMethod){
                case Input::NODE_METHOD_SUBGRADIENT:{
                    return new lagSubgradient(instance);
                    break;
                }
                case Input::NODE_METHOD_VOLUME:{
                    return new lagVolume(instance);
                    break;
                }
                default:{
                    std::cout << "ERROR: Invalid Lagrangian Node Method." << std::endl;
                    exit(0);
                    break;
                }
            }
            return NULL;
        }
};

#endif