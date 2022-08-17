#ifndef __formulationFactory__h
#define __formulationFactory__h

// include all concrete formulations
#include "flowForm.h"
#include "edgeNodeForm.h"

/*********************************************************************************************
* This class implements a factory for Formulations. It provides a concrete formulation.
*********************************************************************************************/
class FormulationFactory{

public:
	/** Factory Method. Returns a new concrete formulation based on the chosen Formulation.  @param instance The instance to be modelled in the formulation. **/
    inline AbstractFormulation* createFormulation(const Instance &instance){
        Input::Formulation chosenForm = instance.getInput().getChosenFormulation();
        switch (chosenForm){
            case Input::FORMULATION_FLOW:{
                Input::NodeMethod chosenMethod = instance.getInput().getChosenNodeMethod();
                if(chosenMethod == Input::NODE_METHOD_LINEAR_RELAX){
                    return new FlowForm(instance);
                    break;
                }else{
                    return NULL;
                    break;
                }  
            }
            case Input::FORMULATION_EDGE_NODE:{
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
    }

};
#endif