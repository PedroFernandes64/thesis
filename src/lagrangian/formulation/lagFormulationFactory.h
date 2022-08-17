#ifndef LAG_FORMULATION_FACTORY_H
#define LAG_FORMULATION_FACTORY_H

// include all concrete formulations
//#include "lagNonOverlap.h"
#include "lagNonOverlapping.h"
#include "lagNewNonOverlapping.h"
#include "lagFlow.h"

/*********************************************************************************************
* This class implements a factory for lagrangian Formulations. It provides a concrete formulation.
*********************************************************************************************/
class lagFormulationFactory{
    public:
        /** Factory Method. Returns a new concrete formulation based on the chosen Formulation.  @param instance The instance to be modelled in the formulation. **/
        inline AbstractLagFormulation* createFormulation(const Instance &instance){
            Input::LagFormulation chosenForm = instance.getInput().getChosenLagFormulation();
            switch (chosenForm)
            {
                case Input::LAG_FLOW:{
                    return new lagFlow(instance);
                    break;
                }
                case Input::LAG_OVERLAPPING:{
                    return new lagNewNonOverlapping(instance);
                    break;
                }

                case Input::LAG_OVERLAP:{
                    return new lagNonOverlapping(instance);
                    break;
                }
                
                default:{
                    std::cout << "ERROR: Invalid Formulation." << std::endl;
                    exit(0);
                    break;
                }
            }
            return NULL;
            
            // TODO -> include as a parameter in the input file
            /*Input::lagFormulation chosenForm = instance.getInput().getChosenLagFormulation();
            switch (chosenForm){
                case Input::LAG_FORMULATION_FLOW:{
                    return new FlowForm(instance);
                    break;
                }
                case Input::LAG_FORMULATION_OVERLAP:{
                    return new EdgeNodeForm(instance);
                    break;
                }
                default:{
                    std::cout << "ERROR: Invalid Formulation." << std::endl;
                    exit(0);
                    break;
                }
            }
            return NULL;
            */
        }
};

#endif