#include "drForm.h"
 

DrFormulation::DrFormulation(const Instance &inst) : AbstractFormulation(inst){
    std::cout <<"DrAOV formulation has been chosen." << displayDimensions() << "---" << std::endl; 
    this->setVariables();
    this->setConstraints();
    this->setObjectives();
    std::cout << "--- DRAOV formulation has been defined ---" << std::endl;
}
std::string DrFormulation::displayDimensions(){
    return "|D| = " + std::to_string(getNbDemandsToBeRouted()) + ", |S| = " + std::to_string(getNbSlicesGlobalLimit()) + ".";
}

std::vector<Variable>  DrFormulation::objective8_fixing(const double upperBound){}

/****************************************************************************************/
/*										Variables    									*/ 
/****************************************************************************************/


 void DrFormulation::setVariables() { 
    // Variables y[a][d]
    int nbEdges = countEdges(compactGraph);
    y.resize(2*nbEdges);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        int uId = getCompactNodeLabel(compactGraph.u(e)) + 1;
        int vId = getCompactNodeLabel(compactGraph.v(e)) + 1;
        y[edge].resize(getNbDemandsToBeRouted());  
        y[edge + nbEdges].resize(getNbDemandsToBeRouted());
        for (int k = 0; k < getNbDemandsToBeRouted(); k++){
            std::ostringstream varName;
             std::ostringstream varName2;
            varName << "y";
            varName << "(" + std::to_string(uId) + "," ;
            varName << std::to_string(vId) + "," ;
            varName << std::to_string(getToBeRouted_k(k).getId() + 1) + ")";
            varName2 << "y";
            varName2 << "(" + std::to_string(vId) + "," ;
            varName2 << std::to_string(uId) + "," ;
            varName2 << std::to_string(getToBeRouted_k(k).getId() + 1) + ")";
            int upperBound = 1;
            int varId = getNbVar(); 
            int varId2 = varId + 1;
            if(instance.getInput().isRelaxed()){
                y[edge][k] = Variable(varId, 0, upperBound, Variable::TYPE_REAL, 0, varName.str());
                y[edge + nbEdges][k] = Variable(varId2, 0, upperBound, Variable::TYPE_REAL, 0, varName2.str());
            }
            else{
                y[edge][k] = Variable(varId, 0, upperBound, Variable::TYPE_BOOLEAN, 0, varName.str());
               y[edge + nbEdges][k] = Variable(varId2, 0, upperBound, Variable::TYPE_BOOLEAN, 0, varName2.str());
            }
            incNbVar(); 
            incNbVar(); 
            //std::cout << "Created variable:  " << y[edge][k].getName() << std::endl;
            //std::cout << "Created variable:  " << y[edge+ nbEdges][k].getName() << std::endl;
      
        }

     }
    std::cout << "Yde variables have been defined"  << std::endl;

    // Variables l[d]. Le plus petit slot attribué a une demand d
    lm.resize(getNbDemandsToBeRouted());
        for(int d=0;d <getNbDemandsToBeRouted(); d++){
            int upperBound = getNbSlicesGlobalLimit();
            int lowerBound = 0;
            int varId = getNbVar();
            std::string varName = "lm ("+ std::to_string(getToBeRouted_k(d).getId() + 1) + ")";
           
             if(instance.getInput().isRelaxed()){
                lm[d] = Variable(varId, lowerBound,upperBound, Variable::TYPE_REAL, 0, varName);   
            }
            else{
                lm[d]= Variable(varId, lowerBound, upperBound, Variable::TYPE_INTEGER, 0, varName); 
            }
            incNbVar();
        }
        

    // Variables rm[d]. Le plus grand slot attribué a une demand d*/
   
    rm.resize(getNbDemandsToBeRouted());
        for(int d=0;d <getNbDemandsToBeRouted(); d++){
            int upperBound = getNbSlicesGlobalLimit();
            int lowerBound = 0;
            int varId = getNbVar();
            std::string varName = "rm ("+ std::to_string(getToBeRouted_k(d).getId() + 1) + ")";
            
         if(instance.getInput().isRelaxed()){
                rm[d] = Variable(varId, lowerBound,upperBound, Variable::TYPE_REAL, 0, varName);    
            }
            else{
                rm[d]= Variable(varId, lowerBound, upperBound, Variable::TYPE_BOOLEAN, 0, varName);  
            }
            incNbVar();
        }

        //  Variables n[d][d].  
        n.resize(getNbDemandsToBeRouted());
        for(int d2=0;d2 <getNbDemandsToBeRouted(); d2++){
           n[d2].resize(getNbDemandsToBeRouted());
            for(int d=0;d <getNbDemandsToBeRouted(); d++){
                if(d==d2){
                   continue;;
                }
            int upperBound = 1;
            int lowerBound = 0;
            int varId = getNbVar();
            std::string varName = "n ("+ std::to_string(getToBeRouted_k(d).getId()  + 1) + ","  + std::to_string(getToBeRouted_k(d2).getId()  + 1) + ")";
          
            if(instance.getInput().isRelaxed()){
                n[d2][d] = Variable(varId, lowerBound,upperBound, Variable::TYPE_REAL, 0, varName);    
            }
            else{
                n[d2][d]= Variable(varId, lowerBound, upperBound, Variable::TYPE_BOOLEAN, 0, varName);  
            }
  
            incNbVar();
            }
        }
    maxSlicePerLink.resize(nbEdges);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        std::string varName = "maxSlice(" + std::to_string(edge+1) + ")";
        int lowerBound = 0;
        int upperBound = getNbSlicesLimitFromEdge(edge);
        int varId = getNbVar();
        if (instance.getInput().isRelaxed()){
            maxSlicePerLink[edge] = Variable(varId, lowerBound, upperBound, Variable::TYPE_REAL, 0, varName);
        }
        else{
            maxSlicePerLink[edge] = Variable(varId, lowerBound, upperBound, Variable::TYPE_INTEGER, 0, varName);
        }
        incNbVar();       
    }
    std::cout << "MaxSlicePerLink variables were created." << std::endl;
    // Max slice overall
    std::string varName = "maxSliceOverall";
    int lowerBound = 0;
    int upperBound = getNbSlicesGlobalLimit();
    int varId = getNbVar();
    if (instance.getInput().isRelaxed()){
        maxSliceOverall = Variable(varId, lowerBound, upperBound, Variable::TYPE_REAL, 0, varName);
    }
    else{
        maxSliceOverall = Variable(varId, lowerBound, upperBound, Variable::TYPE_INTEGER, 0, varName);
    }
    incNbVar();
    std::cout << "MaxSliceOverall variable was created."  << std::endl;
    std::cout << "DR-AOV variables have been defined..." << std::endl;
    
}


VarArray DrFormulation::getVariables(){
    VarArray vec;
    vec.resize(getNbVar());
    // Variables y[e][d]
    int nbEdges = countEdges(compactGraph);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e) {
        int edge = getCompactEdgeLabel(e);
        for (int k = 0; k < getNbDemandsToBeRouted(); k++) {
            int pos = y[edge][k].getId();
            int pos2 = y[edge + nbEdges][k].getId();
            vec[pos] = y[edge][k];
            vec[pos2] = y[edge + nbEdges][k];
        } 
    }  
    // Variables n[d][d]
    for (int d2 = 0; d2 < getNbDemandsToBeRouted(); d2++) {
        for (int d = 0; d < getNbDemandsToBeRouted(); d++) {
           if (d == d2) {
                continue;
            }
            int pos = n[d2][d].getId();
            vec[pos] = n[d2][d];
        }
    }
    // Variables lm[d]
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            int pos = lm[d].getId();
            vec[pos] = lm[d];
        }
    // Variables rm[d]
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            int pos = rm[d].getId();
            vec[pos] = rm[d];
        }
    // Max slice variables
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        int pos = maxSlicePerLink[edge].getId();
        vec[pos] = maxSlicePerLink[edge];
    }  

    // Max slice overall
    int pos = maxSliceOverall.getId();
    vec[pos] = maxSliceOverall;
    return vec;
    
}


void DrFormulation::setVariableValues(const std::vector<double> &vals){
    
    // Variables y[e][d] e y[e][d] espelho
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        for (int k = 0; k < getNbDemandsToBeRouted(); k++){
            int nbEdges = countEdges(compactGraph) ;
            int pos = y[edge][k].getId();
            int pos2 = y[edge+nbEdges][k].getId();
            double newValue = vals[pos];
            double newValue2 = vals[pos2];
            y[edge][k].setVal(newValue);
            y[edge + nbEdges][k].setVal(newValue2);
        }
    }
    // Variables n[d2][d]
    for (int d2 = 0; d2 < getNbDemandsToBeRouted(); d2++){
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            int pos = n[d2][d].getId();
            double newValue = vals[pos];
            n[d2][d].setVal(newValue);
    
        }
    }
     
    // variable lm[d]
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        int pos = lm[d].getId();
        double newValue = vals[pos];
        lm[d].setVal(newValue);
    }
    // variable rm[d]
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        int pos = rm[d].getId();
        double newValue = vals[pos];
        rm[d].setVal(newValue);
    }

    // Max slice variables
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        int pos = maxSlicePerLink[edge].getId();
        double newValue = vals[pos];
        maxSlicePerLink[edge].setVal(newValue);
    }
    // Max slice overall
    int pos = maxSliceOverall.getId();
    double newValue = vals[pos];
    maxSliceOverall.setVal(newValue);    
}




 
/****************************************************************************************/
/*									Objective Function    								*/
/****************************************************************************************/
//
/* Sets the objective Function */
void DrFormulation::setObjectives(){
    const std::vector<Input::ObjectiveMetric> & chosenObjectives = instance.getInput().getChosenObj();
    objectiveSet.resize(chosenObjectives.size());
    for (unsigned int i = 0; i < chosenObjectives.size(); i++){
        Expression myObjective = this->getObjFunctionFromMetric(chosenObjectives[i]);
        objectiveSet[i].setExpression(myObjective);
        objectiveSet[i].setDirection(ObjectiveFunction::DIRECTION_MIN);
        objectiveSet[i].setName(instance.getInput().getObjName(chosenObjectives[i]));
        objectiveSet[i].setId(chosenObjectives[i]);
        std::cout << "Objective " << objectiveSet[i].getName() << " has been defined." << std::endl;
    }
}

/* Returns the objective function expression. */
Expression DrFormulation::getObjFunctionFromMetric(Input::ObjectiveMetric chosenObjective){
    Expression obj;
    switch (chosenObjective){
        case Input::OBJECTIVE_METRIC_0:{
            break;
        }
        case Input::OBJECTIVE_METRIC_1:{
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
                Term term(rm[d], 1);
                obj.addTerm(term);            
            }
        break;
        }
        case Input::OBJECTIVE_METRIC_1p:{
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                int edge = getCompactEdgeLabel(e);
                Term term(maxSlicePerLink[edge], 1);
                obj.addTerm(term);
                
            }
            break;
        }
        case Input::OBJECTIVE_METRIC_2:{
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                int edge = getCompactEdgeLabel(e);
                int nbEdges = countEdges(compactGraph);
                for (int k = 0; k < getNbDemandsToBeRouted(); k++){
                    Term term(y[edge][k], 1);
                    obj.addTerm(term);
                     Term term2(y[edge+ nbEdges][k], 1);
                    obj.addTerm(term2);
                }
            }
            break;
        }
        case Input::OBJECTIVE_METRIC_2p:{
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                int edge = getCompactEdgeLabel(e);
                int nbEdges = countEdges(compactGraph);
                for (int d = 0; d < getNbDemandsToBeRouted(); d++){
                    Term term(y[edge][d], getToBeRouted_k(d).getLoad());
                    obj.addTerm(term);
                     Term term2(y[edge + nbEdges][d], getToBeRouted_k(d).getLoad());
                    obj.addTerm(term2);
                }
            }
            break;
        }
        case Input::OBJECTIVE_METRIC_4:{
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                int edge = getCompactEdgeLabel(e);
                int nbEdges = countEdges(compactGraph);
                for (int d = 0; d < getNbDemandsToBeRouted(); d++){
                    Term term(y[edge][d], getCompactLength(e));
                    obj.addTerm(term);
                    Term term2(y[edge + nbEdges][d], getCompactLength(e));
                    obj.addTerm(term2);
                }
            }
            break;
        }
        case Input::OBJECTIVE_METRIC_8:{
            Term term(maxSliceOverall, 1);
            obj.addTerm(term);
            break;
        }
        default:{
            std::cout << "ERROR: Objective '" << chosenObjective << "' is not known." << std::endl;
            exit(0);
            break;
        }
    
    }
    return obj;
}



/****************************************************************************************/
/*									    Base Contraints  								*/
/****************************************************************************************/
 void DrFormulation::setConstraints(){
	
	this->setLengthConstraints(); //somatório yde*le<=Ld
	this-> setSlotsVolumeConstraints();
    this-> setSlotsVolumeConstraints2();
    this->setDegreeConstraints();
	this-> setOriginConstraints();
    this-> setTargetConstraints();
    this->setFluxConstraints();
	this-> setNonOverlappingConstraints();
    this-> setNonOverlappingConstraints2();
    this->setDestinationConstraints();
    this-> setMaxUsedSlicePerLinkConstraints();
    this-> setMaxUsedSliceOverallConstraints();
     
}

void DrFormulation::setLengthConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){   
        const Constraint & lengthConstraint = getLengthConstraint(d);
        constraintSet.push_back(lengthConstraint);
    }
    std::cout << "Length constraints have been defined..." << std::endl;
}

Constraint DrFormulation::getLengthConstraint(int d){
    Expression exp;
    double upperBound = getToBeRouted_k(d).getMaxLength();
    int lowerBound = 0;
    int nbEdges = countEdges(compactGraph);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        double coeff = getCompactLength(e);
        Term term(y[edge][d], coeff);
        exp.addTerm(term);
        Term term2(y[edge + nbEdges][d], coeff);
        exp.addTerm(term2);
    } 
    
    std::ostringstream constraintName;
    constraintName << "TrasmissionReach(" << getToBeRouted_k(d).getId()+1 << ")"; 
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}


void DrFormulation::setSlotsVolumeConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){   
        const Constraint & volumeDemandConstraints = getSlotsVolumeConstraints(d);
        constraintSet.push_back(volumeDemandConstraints);
    }
    std::cout << "Volume constraints 1 have been defined..." << std::endl;
   
}
Constraint DrFormulation::getSlotsVolumeConstraints(int d){
    Expression exp;
    int upperBound = getToBeRouted_k(d).getLoad() -1;
    int lowerBound = getToBeRouted_k(d).getLoad() -1;
    Term term_rm( rm[d] , 1);
    exp.addTerm(term_rm);
    Term term_lm(lm[d], -1);
    exp.addTerm(term_lm);
    std::ostringstream constraintName;
    constraintName << "Volume Slots Demand(" << getToBeRouted_k(d).getId()+1 << ")";
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

void DrFormulation::setSlotsVolumeConstraints2(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){   
        const Constraint & volumeDemandConstraints2 = getSlotsVolumeConstraints2(d);
        constraintSet.push_back(volumeDemandConstraints2);
    }
    std::cout << "Volume constraints 2 have been defined..." << std::endl;
}

Constraint DrFormulation::getSlotsVolumeConstraints2(int d){
    Expression exp;
    int upperBound = getNbSlicesGlobalLimit();
    int lowerBound =  0;
    Term term_rm(rm[d], 1);
    exp.addTerm(term_rm);
    Term term_lm(lm[d], -1);
    exp.addTerm(term_lm);
    std::ostringstream constraintName;
    constraintName << "Volume Slots 2 Demand (" << getToBeRouted_k(d).getId()+1 << ")";
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

/* Defines Degree constraints. At most two edges are adjacent to any node. */
void DrFormulation::setDegreeConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){  
        for (ListGraph::NodeIt v(compactGraph); v != INVALID; ++v){
                Constraint degreeConstraint = getDegreeConstraint_k(d, v);
                constraintSet.push_back(degreeConstraint);
            
        }
    }
    std::cout << "Degree constraints have been defined..." << std::endl;
}

/* Returns the degree constraint associated with a demand k and a node v. */
Constraint DrFormulation::getDegreeConstraint_k(int k, const ListGraph::Node &v){
    Expression exp;
    int upperBound = 1;
    int lowerBound = 0;
    int nbEdges = countEdges(compactGraph);
    int node = getCompactNodeLabel(v) + 1;
    for (ListGraph::InArcIt  e(compactGraph, v); e != INVALID; ++e) {
        int uId = getCompactNodeLabel(compactGraph.u(e)) + 1;
        int vId = getCompactNodeLabel(compactGraph.v(e)) + 1;
        int edge = getCompactEdgeLabel(e);
        if (vId == node) { 
            Term term(y[edge][k], 1);
            exp.addTerm(term);
        }
        if (uId==node) { 
            Term term2(y[edge + nbEdges][k], 1);
            exp.addTerm(term2);
        }      
    }
    std::ostringstream constraintName;
    constraintName << "Degree_" << getToBeRouted_k(k).getId()+1 << "_" << getCompactNodeLabel(v)+1;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}
void DrFormulation::setOriginConstraints(){
    for (int k = 0; k < getNbDemandsToBeRouted(); k++){  
        for (ListGraph::NodeIt v(compactGraph); v != INVALID; ++v){
            int node = getCompactNodeLabel(v);
            if (node == getToBeRouted_k(k).getSource()){
                Constraint originConstraint = getOriginConstraint_k(k,v);
                constraintSet.push_back(originConstraint);
            }
        }
    }
    std::cout << "Origin constraints have been defined..." << std::endl;
}


Constraint DrFormulation::getOriginConstraint_k(int k,const ListGraph::Node &v){
    Expression exp;
    int upperBound = -1;
    int lowerBound = -1;
    int nbEdges = countEdges(compactGraph);
    int origin = getToBeRouted_k(k).getSource();
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int labelU = getCompactNodeLabel(compactGraph.u(e));
        int labelV = getCompactNodeLabel(compactGraph.v(e));
        
    if (labelU == origin || labelV == origin){
            int edge = getCompactEdgeLabel(e);
            if (labelU==origin){
                Term term(y[edge][k], -1);
                exp.addTerm(term);
                Term term2(y[edge + nbEdges][k], +1);
                exp.addTerm(term2);  
            }
            if (labelV==origin){ 
                Term term(y[edge][k], 1);
                exp.addTerm(term);
                Term term2(y[edge + nbEdges][k], -1);
                exp.addTerm(term2);  
            }
            
        }
    }
    std::ostringstream constraintName;
    constraintName << "Origin Demand" << getToBeRouted_k(k).getId()+1 << "_Node_" << getCompactNodeLabel(v)+1 ;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}


/* Defines Destination constraints. Exactly one arc enters the target. */
void DrFormulation::setDestinationConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){   
        Constraint destinationConstraint = getDestinationConstraint_k(d);
        constraintSet.push_back(destinationConstraint);  
        }

    std::cout << "Destination constraints have been defined..." << std::endl;
}
/* Restrição Soma de todos arcos que saem do destino seja igual a 0. */
Constraint DrFormulation::getDestinationConstraint_k(int d){
    Expression exp;
    int upperBound = 0;
    int lowerBound = 0;
    int destination = getToBeRouted_k(d).getTarget();
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int uId = getCompactNodeLabel(compactGraph.u(e));
        int vId = getCompactNodeLabel(compactGraph.v(e));
        if (uId == destination || vId == destination){ 
            int edge = getCompactEdgeLabel(e);
            int nbEdges= countEdges(compactGraph);
            if (vId==destination) { 
            Term term(y[edge + nbEdges][d], 1);
            exp.addTerm(term);
            }
             if (uId==destination) { 
            Term term2(y[edge][d], 1);
            exp.addTerm(term2);
             }
        }
    }

    //std::cout << exp.to_string() << std::endl;
    std::ostringstream constraintName;
    constraintName << "Destination_" << getToBeRouted_k(d).getId()+1;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

void DrFormulation::setTargetConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){  
        for (ListGraph::NodeIt v(compactGraph); v != INVALID; ++v){
            int node = getCompactNodeLabel(v);
            if ( node == getToBeRouted_k(d).getTarget()){
                Constraint targetConstraint = getTargetConstraint_k(d, v);
                constraintSet.push_back(targetConstraint);
            }
        }
    }
    std::cout << "Target constraint have been defined..." << std::endl;
}

Constraint DrFormulation::getTargetConstraint_k(int d, const ListGraph::Node &v){
    Expression exp;
    int lowerBound = 1;
    int upperBound = 1;
    int node = getCompactNodeLabel(v);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int uId = getCompactNodeLabel(compactGraph.u(e)) ;
        int vId = getCompactNodeLabel(compactGraph.v(e)) ;
        int edge = getCompactEdgeLabel(e);
        if(node==uId){
            Term term(y[edge][d], -1);
            exp.addTerm(term);
            int nbEdges = countEdges(compactGraph);
            Term term2(y[edge + nbEdges][d], 1);
            exp.addTerm(term2);
        }
        if(node==vId){
            Term term(y[edge][d], 1);
            exp.addTerm(term);
            int nbEdges = countEdges(compactGraph);
            Term term2(y[edge + nbEdges][d], -1);
            exp.addTerm(term2);
        }
    }
    //std::cout << exp.to_string() << std::endl; 
    std::ostringstream constraintName;
    constraintName << "Target" << "_"<<getToBeRouted_k(d).getId()+1 << "_" << getCompactNodeLabel(v)+1;
    Constraint constraint( lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}


/* Defines Degree constraints. At most two edges are adjacent to any node. */
void DrFormulation::setFluxConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){  
        for (ListGraph::NodeIt v(compactGraph); v != INVALID; ++v){
            int node = getCompactNodeLabel(v);
            if (node != getToBeRouted_k(d).getSource() && node != getToBeRouted_k(d).getTarget()){
                Constraint fluxConstraint = getFluxConstraint_k(d, v);
                constraintSet.push_back(fluxConstraint);
            }
        }
    }
    std::cout << "Flux have been defined..." << std::endl;
}
Constraint DrFormulation::getFluxConstraint_k(int d, const ListGraph::Node &v){
    Expression exp;
    int lowerBound = 0;
    int upperBound = 0;
    int node = getCompactNodeLabel(v);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int uId = getCompactNodeLabel(compactGraph.u(e)) ;
        int vId = getCompactNodeLabel(compactGraph.v(e)) ;
        int edge = getCompactEdgeLabel(e);
        if(node==uId){
            Term term(y[edge][d], -1);
            exp.addTerm(term);
            int nbEdges = countEdges(compactGraph);
            Term term2(y[edge + nbEdges][d], 1);
            exp.addTerm(term2);
        }
        if(node==vId){
            Term term(y[edge][d], 1);
            exp.addTerm(term);
            int nbEdges = countEdges(compactGraph);
            Term term2(y[edge + nbEdges][d], -1);
            exp.addTerm(term2);
        }
    }
    //std::cout << exp.to_string() << std::endl; 
    std::ostringstream constraintName;
    constraintName << "Flux Constraint" << "_"<<getToBeRouted_k(d).getId()+1 << "_" << getCompactNodeLabel(v)+1;
    Constraint constraint( lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}


/* Defines Non-Overlapping constraints. Demands must not overlap eachother's slices.*/ 
void DrFormulation::setNonOverlappingConstraints() {
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e) {
        for (int d = 0; d < getNbDemandsToBeRouted(); d++) {
            for (int d2 = 0; d2 < getNbDemandsToBeRouted(); d2++) {
                if (d == d2) {
                    continue;
                }
                int edge = getCompactEdgeLabel(e);
                Constraint nonOverlappingConstraint = getNonOverlappingConstraints(edge,d,d2);
                constraintSet.push_back(nonOverlappingConstraint);
            }
        }
    }
    std::cout << "Non-overlapping constraints have been defined..." << std::endl;
}

Constraint DrFormulation::getNonOverlappingConstraints(int e,int d, int d2) {
    Expression exp;
    int upperBound = 1;
    int lowerBound = -1;
    int edge = e;
    int nbEdges = countEdges(compactGraph); 
    Term term1(y[edge][d], 1);
    exp.addTerm(term1);
    Term term2(y[edge+nbEdges][d], 1);
    exp.addTerm(term2);
    Term term3(y[edge][d2], 1);
    exp.addTerm(term3);
    Term term4(y[edge+nbEdges][d2], 1);
    exp.addTerm(term4);  
    Term term5(n[d][d2], -1);
    exp.addTerm(term5);
    Term term6(n[d2][d], -1);
    exp.addTerm(term6);
 
    //std::cout << exp.to_string() << std::endl; 
    std::ostringstream constraintName;
    constraintName << "Nonoverlapping_" << getToBeRouted_k(d).getId() + 1 << "_" << getToBeRouted_k(d2).getId()+1<< "_" << e + 1 ;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

void DrFormulation::setNonOverlappingConstraints2(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){   
        for (int d2 = 0; d2 < getNbDemandsToBeRouted(); d2++){  
            if(d==d2){
                continue; 
            }
        Constraint nonOverlappingConstraint2 = getNonOverlappingConstraints2(d,d2);
        constraintSet.push_back(nonOverlappingConstraint2);
        }
    } 
    std::cout << "Non overlapping constraints 2 have been defined..." << std::endl;
}

Constraint DrFormulation::getNonOverlappingConstraints2(int d, int d2) {
    Expression exp;
    int upperBound = getNbSlicesGlobalLimit() -1;
    int lowerBound = 0;
    
    Term term1(rm[d], 1);
    exp.addTerm(term1);

    Term term2(lm[d2], -1);
    exp.addTerm(term2);

    int coeff5 = getNbSlicesGlobalLimit();
    Term term5(n[d][d2], coeff5);
    exp.addTerm(term5);
    
    //std::cout << exp.to_string() << std::endl; 
    std::ostringstream constraintName;
    constraintName << "Non overlapping 2 contraints " << getToBeRouted_k(d).getId() + 1 << "_" << getToBeRouted_k(d2).getId();

    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

/****************************************************************************************/
/*						Objective function related constraints    						*/
/****************************************************************************************/


// Defines the Overall Max Used Slice Position constraints. 
void DrFormulation::setMaxUsedSliceOverallConstraints(){ 
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        Constraint maxUsedSliceOverallConst = getMaxUsedSliceOverallConstraints(d);
        constraintSet.push_back(maxUsedSliceOverallConst);
    }
    std::cout << "Max Used Slice Overall constraints have been defined..." << std::endl;
}

Constraint DrFormulation::getMaxUsedSliceOverallConstraints(int d){
    Expression exp;
    int rhs = 0;
    Term term1(rm[d], 1);
    exp.addTerm(term1);
    Term term(maxSliceOverall, -1);
    exp.addTerm(term);
    std::ostringstream constraintName;
    constraintName << "MaxUsedSliceOverall( )";
    Constraint constraint( -getNbSlicesGlobalLimit(), exp, rhs, constraintName.str());
    return constraint;
}


/* Defines the Link's Max Used Slice Position constraints. The max used slice position on each link must be greater than every slice position used in the link. */
void DrFormulation::setMaxUsedSlicePerLinkConstraints(){
    for (int k = 0; k < getNbDemandsToBeRouted(); k++){
        for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
            int edge = getCompactEdgeLabel(e);
            Constraint maxUsedSlicePerLinkConstraint = getMaxUsedSlicePerLinkConstraints(k, edge);
            constraintSet.push_back(maxUsedSlicePerLinkConstraint);
        }
    }
    std::cout << "Max Used Slice Per Link constraints have been defined..." << std::endl;
}
/* Defines the Link's Max Used Slice Position constraints. The max used slice position on each link must be greater than every slice position used in the link. */

Constraint DrFormulation::getMaxUsedSlicePerLinkConstraints(int k, int e){
    Expression exp;
    int upperBound = 0;
    Term termT(rm[k], 1);
    exp.addTerm(termT);
    Term term(maxSlicePerLink[e], -1);
    exp.addTerm(term);
    std::ostringstream constraintName;
    constraintName << "MaxUsedSlicePerLink(" << getToBeRouted_k(k).getId()+1 << "," << e+1 << ")";
    Constraint constraint(-getNbSlicesGlobalLimit(), exp, upperBound, constraintName.str());
    return constraint;
}

/****************************************************************************************/
/*						                Methods                 						*/
/****************************************************************************************/
/* Recovers the obtained MIP solution and builds a path for each demand on its associated graph from RSA. */
void DrFormulation::updatePath(const std::vector<double> &vals){   
   setVariableValues(vals);
  
    // Reinitialize OnPath
    std::cout << "Enter update." << std::endl;
    for(int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
               (*vecOnPath[d])[a] = -1;
        
        }
    }

    for(int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
            int nbEdges = countEdges(compactGraph);
            int edge = getArcLabel(a,d);
            int slot = getArcSlice(a,d);
            //std::cout << edge << std::endl;
            if (((y[edge][d].getVal() >= 1 - EPS) || (y[edge + nbEdges][d].getVal() >= 1 - EPS ) ) 
                && (rm[d].getVal() + EPS >= slot)) {
                (*vecOnPath[d])[a] = getToBeRouted_k(d).getId();
            }
        }
    }
}

void DrFormulation::displayVariableValues(){
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        for (int k = 0; k < getNbDemandsToBeRouted(); k++){
            std::cout << y[edge][k].getName() << " = " << y[edge][k].getVal() << "   ";
        }
        std::cout << std::endl;
    }
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        int nbEdges = countEdges(compactGraph); 
        for (int k = 0; k < getNbDemandsToBeRouted(); k++){
           
            std::cout << y[edge + nbEdges][k].getName() << " = " << y[edge + nbEdges][k].getVal() << "   ";
        }
        std::cout << std::endl;
    }
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            std::cout << lm[d].getName() << " = " << lm[d].getVal() << "   ";
        }
        std::cout << std::endl;
    
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            std::cout << rm[d].getName() << " = " << rm[d].getVal() << "   ";
        }
        std::cout << std::endl;
    
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            for (int d2 = 0; d2 < getNbDemandsToBeRouted(); d2++){
                if(d==d2){continue;}
                std::cout << n[d][d2].getName() << " = " << n[d][d2].getVal() << "   ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        std::cout << maxSlicePerLink[edge].getName() << " = " << maxSlicePerLink[edge].getVal() << "   ";
    }
    std::cout << std::endl;
    std::cout << maxSliceOverall.getName() << " = " << maxSliceOverall.getVal() << std::endl;
}


/****************************************************************************************/
/*										destrutor   									
/****************************************************************************************/
DrFormulation::~DrFormulation()
{
    y.clear();
    n.clear();
    lm.clear();
    rm.clear();
    maxSlicePerLink.clear();
}




