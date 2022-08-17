#ifndef LAG_HEURISTIC_FACTORY_H
#define LAG_HEURISTIC_FACTORY_H

// include all concrete solvers
#include "shortestPathHeuristic.h"

class heuristicFactory{
    public:
        inline AbstractHeuristic* createHeuristic(AbstractLagFormulation* formulation, const Instance &instance){
            Input::Heuristic chosenHeuristic = instance.getInput().getChosenHeuristic();
            switch (chosenHeuristic){
                case Input::SHORT_PATH:{
                    return new shortestPathHeuristic(formulation);
                    break;
                }
                case Input::PROBABILITY:{
                    std::cout << "Not implemented yet. Should create Probability heuristic." << std::endl;
                    exit(0);
                    break;
                }
                default:{
                    std::cout << "ERROR: Invalid LAG_Solver." << std::endl;
                    exit(0);
                    break;
                }
            }
            return NULL;
        }
};

#endif