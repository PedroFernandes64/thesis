#include "drForm.h"
 

DrFormulation::DrFormulation(const Instance &inst) : AbstractFormulation(inst){
    ClockTime time(ClockTime::getTimeNow());
    ClockTime time2(ClockTime::getTimeNow());
    std::cout <<"DRAOV formulation has been chosen." << displayDimensions() << "---" << std::endl; 
    this->setVariables();
    varImpleTime = time.getTimeInSecFromStart() ;
    time.setStart(ClockTime::getTimeNow());
    this->setConstraints();
    constImpleTime = time.getTimeInSecFromStart() ;
    time.setStart(ClockTime::getTimeNow());
    this->setObjectives();
    objImpleTime = time.getTimeInSecFromStart() ;
    std::cout << "--- DRAOV formulation has been defined ---" << std::endl;
    totalImpleTime = time2.getTimeInSecFromStart() ;
}

std::vector<Variable>  DrFormulation::objective8_fixing(const double upperBound){}

/****************************************************************************************/
/*										Variables    									*/ 
/****************************************************************************************/

void DrFormulation::setVariables() { 
    this->setRoutingVariables();
    this->setLeftVariables();
    this->setRightVariables();
    this->setDemandDemandVariables();
    
    const std::vector<Input::ObjectiveMetric> & chosenObjectives = instance.getInput().getChosenObj();
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_SLUS){
        this->setMaxUsedSlicePerEdgeVariable();
    }
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_NLUS){
        this->setMaxUsedSliceOverallVariable();
    }
    std::cout << "DR-AOV variables have been defined..." << std::endl;
}

 void DrFormulation::setRoutingVariables() { 
    // Variables y[a][d]
    int nbEdges = countEdges(compactGraph);
    y.resize(2*nbEdges);
    for (int k = 0; k < getNbDemandsToBeRouted(); k++){
        for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
            int edge = getCompactEdgeLabel(e);
            int uId = getCompactNodeLabel(compactGraph.u(e)) + 1;
            int vId = getCompactNodeLabel(compactGraph.v(e)) + 1;
            y[edge].resize(getNbDemandsToBeRouted());  
            y[edge + nbEdges].resize(getNbDemandsToBeRouted());
            std::ostringstream varName;
            std::ostringstream varName2;
            varName << "y";
            varName << "(" + std::to_string(getToBeRouted_k(k).getId() + 1) + "," ;
            varName << std::to_string(uId) + "," ;
            varName << std::to_string(vId) + ")";
            varName2 << "y";
            varName2 << "(" + std::to_string(getToBeRouted_k(k).getId() + 1) + "," ;
            varName2 << std::to_string(vId) + "," ;
            varName2 << std::to_string(uId) + ")";
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
      
        }

     }
 }

void DrFormulation::setLeftVariables() { 
    // Variables l[d]. Le plus petit slot attribué a une demand d
    lm.resize(getNbDemandsToBeRouted());
        for(int d=0;d <getNbDemandsToBeRouted(); d++){
            int upperBound = getNbSlicesGlobalLimit();
            int lowerBound = 1;
            int varId = getNbVar();
            std::string varName = "l("+ std::to_string(getToBeRouted_k(d).getId() + 1) + ")";
           
            if(instance.getInput().isRelaxed()){
                lm[d] = Variable(varId, lowerBound,upperBound, Variable::TYPE_REAL, 0, varName);   
            }
            else{
                lm[d]= Variable(varId, lowerBound, upperBound, Variable::TYPE_INTEGER, 0, varName); 
            }
            incNbVar();
        }
        

    // Variables rm[d]. Le plus grand slot attribué a une demand d*/
}

void DrFormulation::setRightVariables() { 
    rm.resize(getNbDemandsToBeRouted());
        for(int d=0;d <getNbDemandsToBeRouted(); d++){
            int upperBound = getNbSlicesGlobalLimit();
            int lowerBound = getToBeRouted_k(d).getLoadC();
            int varId = getNbVar();
            std::string varName = "r("+ std::to_string(getToBeRouted_k(d).getId() + 1) + ")";
            
        if(instance.getInput().isRelaxed()){
            rm[d] = Variable(varId, lowerBound,upperBound, Variable::TYPE_REAL, 0, varName);    
        }else{
            rm[d]= Variable(varId, lowerBound, upperBound, Variable::TYPE_BOOLEAN, 0, varName);  
        }
            incNbVar();
        }
}
void DrFormulation::setDemandDemandVariables() { 
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
        std::string varName = "n("+ std::to_string(getToBeRouted_k(d).getId()  + 1) + ","  + std::to_string(getToBeRouted_k(d2).getId()  + 1) + ")";
        
        if(instance.getInput().isRelaxed()){
            n[d2][d] = Variable(varId, lowerBound,upperBound, Variable::TYPE_REAL, 0, varName);    
        }
        else{
            n[d2][d]= Variable(varId, lowerBound, upperBound, Variable::TYPE_BOOLEAN, 0, varName);  
        }

        incNbVar();
        }
    }
}
void DrFormulation::setMaxUsedSlicePerEdgeVariable() { 
    int nbEdges = countEdges(compactGraph);
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
}

void DrFormulation::setMaxUsedSliceOverallVariable() { 
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
    const std::vector<Input::ObjectiveMetric> & chosenObjectives = instance.getInput().getChosenObj();
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_SLUS){
        // Max slice variables
        for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
            int edge = getCompactEdgeLabel(e);
            int pos = maxSlicePerLink[edge].getId();
            vec[pos] = maxSlicePerLink[edge];
        }  
    }
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_NLUS){
        // Max slice overall
        int pos = maxSliceOverall.getId();
        vec[pos] = maxSliceOverall;
    }
        
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

    const std::vector<Input::ObjectiveMetric> & chosenObjectives = instance.getInput().getChosenObj();
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_SLUS){
        // Max slice variables
        for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
            int edge = getCompactEdgeLabel(e);
            int pos = maxSlicePerLink[edge].getId();
            double newValue = vals[pos];
            maxSlicePerLink[edge].setVal(newValue);
        }
    }
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_NLUS){
        // Max slice overall
        int pos = maxSliceOverall.getId();
        double newValue = vals[pos];
        maxSliceOverall.setVal(newValue);  
    }  
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
        if (chosenObjectives[i] == Input::OBJECTIVE_METRIC_ADS){
            objectiveSet[i].setDirection(ObjectiveFunction::DIRECTION_MAX);
        } 
        objectiveSet[i].setName(instance.getInput().getObjName(chosenObjectives[i]));
        objectiveSet[i].setId(chosenObjectives[i]);
        std::cout << "Objective " << objectiveSet[i].getName() << " has been defined." << std::endl;
    }
}

/* Returns the objective function expression. */
Expression DrFormulation::getObjFunctionFromMetric(Input::ObjectiveMetric chosenObjective){
    Expression obj;
    int nbEdges = countEdges(compactGraph);
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
        case Input::OBJECTIVE_METRIC_SLUS:{
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                int edge = getCompactEdgeLabel(e);
                Term term(maxSlicePerLink[edge], 1);
                obj.addTerm(term);
                
            }
            break;
        }
        case Input::OBJECTIVE_METRIC_SULD:{
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                int edge = getCompactEdgeLabel(e);
                for (int k = 0; k < getNbDemandsToBeRouted(); k++){
                    Term term(y[edge][k], 1);
                    obj.addTerm(term);
                     Term term2(y[edge+ nbEdges][k], 1);
                    obj.addTerm(term2);
                }
            }
            break;
        }
        case Input::OBJECTIVE_METRIC_TUS:{
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                int edge = getCompactEdgeLabel(e);
                for (int d = 0; d < getNbDemandsToBeRouted(); d++){
                    Term term(y[edge][d], getToBeRouted_k(d).getLoadC());
                    obj.addTerm(term);
                     Term term2(y[edge + nbEdges][d], getToBeRouted_k(d).getLoadC());
                    obj.addTerm(term2);
                }
            }
            break;
        }
        case Input::OBJECTIVE_METRIC_TRL:{
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                int edge = getCompactEdgeLabel(e);
                for (int d = 0; d < getNbDemandsToBeRouted(); d++){
                    Term term(y[edge][d], getCompactLength(e));
                    obj.addTerm(term);
                    Term term2(y[edge + nbEdges][d], getCompactLength(e));
                    obj.addTerm(term2);
                }
            }
            break;
        }
        case Input::OBJECTIVE_METRIC_TASE:{
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                int edge = getCompactEdgeLabel(e);
                for (int k = 0; k < getNbDemandsToBeRouted(); k++){
                    Term term(y[edge][k], (getCompactLineAmplifiers(e)+1)* (getToBeRouted_k(k).getGBits()/getToBeRouted_k(k).getLoadC()));
                    Term term2(y[edge + nbEdges][k], (getCompactLineAmplifiers(e)+1)* (getToBeRouted_k(k).getGBits()/getToBeRouted_k(k).getLoadC()));
                    obj.addTerm(term);
                    obj.addTerm(term2);
                }
            }
            break;
        }
        case Input::OBJECTIVE_METRIC_NLUS:{
            Term term(maxSliceOverall, 1);
            obj.addTerm(term);
            break;
        }

        case Input::OBJECTIVE_METRIC_TOS:{
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                int edge = getCompactEdgeLabel(e);
                for (int k = 0; k < getNbDemandsToBeRouted(); k++){
                    double pnli = ceil(getCompactPnliC(e)* pow(10,8)*100)/100; //ROUNDING
                    double paseLine = ceil(getCompactPaseLineC(e)* pow(10,8)*100)/100; //ROUNDING
                    double paseNode = ceil(instance.getPaseNodeC() * pow(10,8)*100)/100; //ROUNDING
                    Term term(y[edge][k], -((pnli + paseLine + paseNode)/getToBeRouted_k(k).getLoadC()));
                    Term term2(y[edge + nbEdges][k], -((pnli + paseLine + paseNode)/getToBeRouted_k(k).getLoadC()));
                    obj.addTerm(term);
                    obj.addTerm(term2);
                }
            }
            break;
        }

        case Input::OBJECTIVE_METRIC_ADS:{
            int nbEdges = countEdges(compactGraph);
            for (int k = 0; k < getNbDemandsToBeRouted(); k++){      
                int origin = getToBeRouted_k(k).getSource();
                for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                    int labelU = getCompactNodeLabel(compactGraph.u(e));
                    int labelV = getCompactNodeLabel(compactGraph.v(e));
                    int edge = getCompactEdgeLabel(e);
                    if (labelU==origin){
                        Term term(y[edge][k], 1);
                        obj.addTerm(term);
                    }
                    if (labelV==origin){
                        Term term2(y[edge + nbEdges][k], 1);
                        obj.addTerm(term2);  
                    }
                }
            }
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
	

	this->setSlotsVolumeConstraints();

    this->setDegreeConstraints();
	this->setSourceConstraints();
    this->setTargetConstraints();
    this->setFlowConstraints();

	this->setNonOverlappingConstraints();
    this->setNonOverlappingConstraints2();
    

    const std::vector<Input::ObjectiveMetric> & chosenObjectives = instance.getInput().getChosenObj();
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_SLUS){
        this->setMaxUsedSlicePerLinkConstraints();
    }
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_NLUS){
        this->setMaxUsedSliceOverallConstraints();
    }
    
    if (this->getInstance().getInput().isMaxCDEnabled() == true){
        this->setCDConstraints();
    }
    if (this->getInstance().getInput().isMinOSNREnabled() == true){
        this->setOSNRConstraints();
    }
    //this->setCBandRoutingConstraints();
    nbConstraint = constraintSet.size();
    if (this->getInstance().getInput().getChosenPreprLvl() >= Input::PREPROCESSING_LVL_PARTIAL){
        this->setPreprocessingConstraints();
    }
     
}

void DrFormulation::setCDConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){   
        const Constraint & lengthConstraint = getCDConstraint(d);
        constraintSet.push_back(lengthConstraint);
    }
    std::cout << "CD constraints have been defined..." << std::endl;
}

void DrFormulation::setOSNRConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){   
        const Constraint & osnrConstraint = getOSNRConstraint(d);
        constraintSet.push_back(osnrConstraint);
    }
    std::cout << "OSNR constraints have been defined..." << std::endl;
}


Constraint DrFormulation::getCDConstraint(int d){
    Expression exp;
    double upperBound = getToBeRouted_k(d).getmaxCDC();
    int lowerBound = 0;
    int nbEdges = countEdges(compactGraph);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        double coeff = getCompactDispersionC(e);
        Term term(y[edge][d], coeff);
        exp.addTerm(term);
        Term term2(y[edge + nbEdges][d], coeff);
        exp.addTerm(term2);
    } 
    
    std::ostringstream constraintName;
    constraintName << "ChromaticDispersion(" << getToBeRouted_k(d).getId()+1 << ")"; 
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

Constraint DrFormulation::getOSNRConstraint(int d){
    Expression exp;
    double rhs; double lhs;

    double osnrLimdb = getToBeRouted_k(d).getOsnrLimitC();
    double osnrLim = pow(10,osnrLimdb/10);
    double pch = getToBeRouted_k(d).getPchC();

    double roundingFactor = pow(10,8);
    
    rhs = pch/osnrLim ; //- instance.getPaseNodeC() ; //RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
    
    rhs = ceil(rhs * roundingFactor*100)/100 ; //ROUNDING
    lhs = 0;

    int nbEdges = countEdges(compactGraph);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        
        double coeff = getCompactNoiseC(e)* roundingFactor;
        coeff = ceil(coeff*100)/100; //ROUNDING
        Term term(y[edge][d], coeff);
        Term term2(y[edge + nbEdges][d], coeff);
        exp.addTerm(term);
        exp.addTerm(term2);
    } 
    std::ostringstream constraintName;
    constraintName << "OSNR(" << getToBeRouted_k(d).getId()+1 << ")"; 
    Constraint constraint(lhs, exp, rhs, constraintName.str());
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
    int upperBound = getToBeRouted_k(d).getLoadC() -1;
    int lowerBound = getToBeRouted_k(d).getLoadC() -1;
    Term term_rm( rm[d] , 1);
    exp.addTerm(term_rm);
    Term term_lm(lm[d], -1);
    exp.addTerm(term_lm);
    std::ostringstream constraintName;
    constraintName << "Volume Slots Demand(" << getToBeRouted_k(d).getId()+1 << ")";
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
        if (uId == node) { 
            Term term(y[edge][k], 1);
            exp.addTerm(term);
        }
        if (vId==node) { 
            Term term2(y[edge + nbEdges][k], 1);
            exp.addTerm(term2);
        }      
    }
    std::ostringstream constraintName;
    constraintName << "Degree_" << getToBeRouted_k(k).getId()+1 << "_" << getCompactNodeLabel(v)+1;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}
void DrFormulation::setSourceConstraints(){
    for (int k = 0; k < getNbDemandsToBeRouted(); k++){  
        Constraint sourceConstraint = getSourceConstraint_k(k);
        constraintSet.push_back(sourceConstraint);  
        Constraint originConstraint = getOriginConstraint_k(k);
        constraintSet.push_back(originConstraint);  
    }
    std::cout << "Source constraints have been defined..." << std::endl;
}

//One arc must leave the origin, if ADS one arc can leave
Constraint DrFormulation::getSourceConstraint_k(int k){
    Expression exp;
    int upperBound = 1;
    int lowerBound = 1;
    int nbEdges = countEdges(compactGraph);
    int origin = getToBeRouted_k(k).getSource();
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int labelU = getCompactNodeLabel(compactGraph.u(e));
        int labelV = getCompactNodeLabel(compactGraph.v(e));
        int edge = getCompactEdgeLabel(e);
        if (labelU==origin){
            Term term(y[edge][k], 1);
            exp.addTerm(term);
        }
        if (labelV==origin){ 
            Term term2(y[edge + nbEdges][k], 1);
            exp.addTerm(term2);  
        }
    }
    const std::vector<Input::ObjectiveMetric> & chosenObjectives = instance.getInput().getChosenObj();
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_ADS){
        lowerBound = 0;
    }
    std::ostringstream constraintName;
    constraintName << "Source Demand" << getToBeRouted_k(k).getId()+1 ;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

//no arc must enter the origin
Constraint DrFormulation::getOriginConstraint_k(int k){
    Expression exp;
    int upperBound = 0;
    int lowerBound = 0;
    int nbEdges = countEdges(compactGraph);
    int origin = getToBeRouted_k(k).getSource();
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int labelU = getCompactNodeLabel(compactGraph.u(e));
        int labelV = getCompactNodeLabel(compactGraph.v(e));
        int edge = getCompactEdgeLabel(e);
        if (labelU==origin){
            Term term2(y[edge + nbEdges][k], 1);
            exp.addTerm(term2);  
        }
        if (labelV==origin){ 
            Term term(y[edge][k], 1);
            exp.addTerm(term); 
        }
    }
    std::ostringstream constraintName;
    constraintName << "Origin Demand" << getToBeRouted_k(k).getId()+1 ;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

void DrFormulation::setTargetConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){  
        Constraint targetConstraint = getTargetConstraint_k(d);
        constraintSet.push_back(targetConstraint);
        Constraint destinationConstraint = getDestinationConstraint_k(d);
        constraintSet.push_back(destinationConstraint);
    }
    std::cout << "Target constraint have been defined..." << std::endl;
}
//One arc must arrive at destination, if ADS one arc can arrive
Constraint DrFormulation::getTargetConstraint_k(int d){
    Expression exp;
    int lowerBound = 1;
    int upperBound = 1;
    int destination = getToBeRouted_k(d).getTarget();
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int labelU = getCompactNodeLabel(compactGraph.u(e)) ;
        int labelV = getCompactNodeLabel(compactGraph.v(e)) ;
        int edge = getCompactEdgeLabel(e);
        if(labelU==destination){
            int nbEdges = countEdges(compactGraph);
            Term term2(y[edge + nbEdges][d], 1);
            exp.addTerm(term2);
        }
        if(labelV==destination){
            Term term(y[edge][d], 1);
            exp.addTerm(term);
        }
    }
    const std::vector<Input::ObjectiveMetric> & chosenObjectives = instance.getInput().getChosenObj();
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_ADS){
        lowerBound = 0;
    }
    //std::cout << exp.to_string() << std::endl; 
    std::ostringstream constraintName;
    constraintName << "Target" << "_"<<getToBeRouted_k(d).getId()+1;
    Constraint constraint( lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

//no arc must leave at destination
Constraint DrFormulation::getDestinationConstraint_k(int d){
    Expression exp;
    int lowerBound = 0;
    int upperBound = 0;
    int destination = getToBeRouted_k(d).getTarget();
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int labelU = getCompactNodeLabel(compactGraph.u(e)) ;
        int labelV = getCompactNodeLabel(compactGraph.v(e)) ;
        int edge = getCompactEdgeLabel(e);
        if(labelU==destination){
            Term term(y[edge][d], 1);
            exp.addTerm(term);
        }
        if(labelV==destination){
            int nbEdges = countEdges(compactGraph);
            Term term2(y[edge + nbEdges][d], 1);
            exp.addTerm(term2);
        }
    }
    //std::cout << exp.to_string() << std::endl; 
    std::ostringstream constraintName;
    constraintName << "Target" << "_"<<getToBeRouted_k(d).getId()+1;
    Constraint constraint( lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}


/* Defines Flow constraints. At most two edges are adjacent to any node. */
void DrFormulation::setFlowConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){  
        for (ListGraph::NodeIt v(compactGraph); v != INVALID; ++v){
            int node = getCompactNodeLabel(v);
            if (node != getToBeRouted_k(d).getSource() && node != getToBeRouted_k(d).getTarget()){
                Constraint fluxConstraint = getFlowConstraint_k(d, v);
                constraintSet.push_back(fluxConstraint);
            }
        }
    }
    std::cout << "Flow have been defined..." << std::endl;
}
Constraint DrFormulation::getFlowConstraint_k(int d, const ListGraph::Node &v){
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

void DrFormulation::setPreprocessingConstraints(){
    for (int k = 0; k < getNbDemandsToBeRouted(); k++){
        const Constraint & preprocessingConstraint = getPreprocessingConstraint(k);
        constraintSet.push_back(preprocessingConstraint);
    }
    std::cout << "Preprocessing constraints have been defined..." << std::endl;
}

Constraint DrFormulation::getPreprocessingConstraint(int k){
    Expression exp;
    double upperBound = 0;
    int lowerBound = 0;
    int nbEdges = countEdges(compactGraph);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        double coeff = 1;
        if (preProcessingErasedArcs[k][edge][0] == 1){
            Term term(y[edge][k], coeff);
            exp.addTerm(term);
        }
        if (preProcessingErasedArcs[k][edge + nbEdges][0] == 1){
            Term term2(y[edge + nbEdges][k], coeff);
            exp.addTerm(term2);
        }
    }
    std::ostringstream constraintName;
    constraintName << "Prepro_" << getToBeRouted_k(k).getId()+1;
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
    int nbEdges = countEdges(compactGraph);
    int lowerBound = -getNbSlicesGlobalLimit();
    int upperBound = getNbSlicesGlobalLimit();
    Term term1(y[e][k], getNbSlicesGlobalLimit());
    exp.addTerm(term1);
    Term term2(y[e + nbEdges][k], getNbSlicesGlobalLimit());
    exp.addTerm(term2);
    Term term(rm[k],1);
    exp.addTerm(term);
    Term term4(maxSlicePerLink[e], -1);
    exp.addTerm(term4);

    std::ostringstream constraintName;
    constraintName << "MaxUsedSlicePerLink_" << e+1 << "_" << getToBeRouted_k(k).getId()+1;
    Constraint constraint (lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

std::vector<Constraint> DrFormulation::solveSeparationProblemInt(const std::vector<double> &solution, const int threadNo){
    //std::cout << "Entering separation problem of a fractional point for Flow Form." << std::endl;
    return std::vector<Constraint>();
}
std::vector<Constraint> DrFormulation::solveSeparationProblemFract(const std::vector<double> &solution){
    //std::cout << "Entering separation problem of a fractional point for Flow Form." << std::endl;
    return std::vector<Constraint>();
}

void DrFormulation::printVariables(){
    VarArray v = this->getVariables();
    for(int i = 0; i < v.size(); i++){
        std::cout << std::endl << v[i].getName() << std::endl;
        std::cout << v[i].getVal() << std::endl;
    }
}

/****************************************************************************************/
/*						                Methods                 						*/
/****************************************************************************************/
/* Recovers the obtained MIP solution and builds a path for each demand on its associated graph from RSA. */
void DrFormulation::updatePath(const std::vector<double> &vals){   
   setVariableValues(vals);
    int nbEdges = countEdges(compactGraph);
    // Reinitialize OnPath
    std::cout << "Enter update." << std::endl;
    for(int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
               (*vecOnPath[d])[a] = -1;
        
        }
    }
    // Fill the mapping with the corresponding demands id.
    for(int d = 0; d < getNbDemandsToBeRouted(); d++){
        int origin = getToBeRouted_k(d).getSource();
        int destination = getToBeRouted_k(d).getTarget();
        ListDigraph::Node SOURCE = INVALID;
        ListDigraph::Node TARGET = INVALID;
        for (ListDigraph::NodeIt v(*vecGraph[d]); v != INVALID; ++v){
            if (getNodeLabel(v, d) == origin){
                SOURCE = v;
            }
            if (getNodeLabel(v, d) == destination){
                TARGET = v;
            }
        }
        if (TARGET == INVALID || SOURCE == INVALID){
            std::cout << "ERROR: Could not find source or target from demand." << std::endl;
            exit(0);
        }
        
        //std::cout << "Search the path from origin to destination." << std::endl;
        ListDigraph::Node currentNode = TARGET;
        while (currentNode != SOURCE){
            ListDigraph::Arc previousArc = INVALID;
            bool leftTarget = false;
            for (ListDigraph::InArcIt a(*vecGraph[d], currentNode); a != INVALID; ++a){
                int edge = getArcLabel(a, d);
                int slice = getArcSlice(a, d)+1;
                int u = getNodeLabel((*vecGraph[d]).source(a), d) + 1;
                int v = getNodeLabel((*vecGraph[d]).target(a), d) + 1;
                if (u < v){
                    if ((y[edge][d].getVal() >= 1 - EPS ) && (rm[d].getVal()+EPS >= slice)&& (rm[d].getVal()+EPS < slice+1)){
                        (*vecOnPath[d])[a] = getToBeRouted_k(d).getId();
                        previousArc = a;
                        leftTarget = true;
                    }
                    else{
                        (*vecOnPath[d])[a] = -1;
                    }
                }else{
                    if ((y[edge + nbEdges][d].getVal() >= 1 - EPS) && (rm[d].getVal()+EPS >= slice)&& (rm[d].getVal()+EPS <= slice+1)){
                        (*vecOnPath[d])[a] = getToBeRouted_k(d).getId();
                        previousArc = a;
                        leftTarget = true;
                    }
                    else{
                        (*vecOnPath[d])[a] = -1;
                    }
                }
            }
            if (leftTarget == true){
                if (previousArc == INVALID){
                    std::cout << "ERROR: Could not find path continuity.." << std::endl;
                    exit(0);
                }
            }else{
                if (leftTarget == false){
                    std::cout << "ATTENTION: non routed demand: "<< getToBeRouted_k(d).getId()+1 << std::endl;
                    break;
                }
            }
            currentNode = (*vecGraph[d]).source(previousArc);
        }
    }
    //std::cout << "Leave update." << std::endl;
}


std::vector<Constraint> DrFormulation::solveSeparationGnpy(const std::vector<double> &solution, const int threadNo){
    std::cout << "Solving separation problem integer..." << std::endl; 
    std::cout << "Checking OSNR Constraint..." << std::endl; 
    setVariableValues(solution);
    std::vector<Constraint> cuts;
    int nbEdges = countEdges(compactGraph);
    std::cout << "Writing integer solution to file" << std::endl;
    std::string QoTfolder = getInstance().getInput().getQotFolder();
    writePathFile(QoTfolder+"/paths.csv");
    std::cout << "Removing any previous network file" << std::endl;
    std::string fileToRemove = QoTfolder+"/network.json";
    std::remove(fileToRemove.c_str());
    std::cout << "Generating network file" << std::endl;
    std::string command = "python3 " +QoTfolder+ "/translator.py " +QoTfolder+"/";
    //std::cout<< command << std::endl;
    system(command.c_str());
    std::cout << "Removing any previous request file" << std::endl;
    fileToRemove = QoTfolder+"/request.json";
    std::remove(fileToRemove.c_str());
    std::cout << "Generating request file" << std::endl;
    command = "python3 " +QoTfolder+ "/requestWriter.py " +QoTfolder+"/";
    //std::cout<< command << std::endl;
    system(command.c_str());
    std::cout << "Removing any previous requestOut file" << std::endl;
    fileToRemove = QoTfolder+"/requestOut.json";
    std::remove(fileToRemove.c_str());
    std::cout << "Executing GNpy" << std::endl;
    command = "gnpy-path-request -o " +QoTfolder+ "/requestOut.json " + QoTfolder+ "/network.json " + QoTfolder+ "/request.json -e "+  QoTfolder+ "/equipments.json" ;
    //std::cout<< command << std::endl;
    system(command.c_str());
    std::cout << "Removing any previous outAux file" << std::endl;
    fileToRemove = QoTfolder+"/outAux.txt";
    std::remove(fileToRemove.c_str());
    std::cout << "Reding  GNpy output" << std::endl;
    std::string resultFile = QoTfolder+"/requestOut.json";
    command = "python3 " +QoTfolder+ "/requestOutputReader.py " +QoTfolder+"/";
    //std::cout<< command << std::endl;
    system(command.c_str());
    std::string auxResultFile = QoTfolder+"/outAux.txt";
    std::cout << "Verifying GNpy output" << std::endl;
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        std::string pattern = "Request-" + std::to_string(getToBeRouted_k(d).getId()+1) + "=";
        //std::cout << "For demand " + std::to_string(getToBeRouted_k(d).getId()+1) + " looking for " + pattern;
        std::string line;
        std::string value = "";
        std::ifstream myfile (auxResultFile.c_str());
        if (myfile.is_open()) {
            while ( std::getline (myfile, line) ) {
                std::size_t pos = line.find(pattern);
                if (pos != std::string::npos){
                    value = line.substr(pos + pattern.size());
                    //value.pop_back();
                    if (value.empty()){
                        std::cout << "WARNING: Field '" << pattern << "' is empty." << std::endl; 
                    }
                }
            }
            myfile.close();
        }
        else {
            std::cerr << "ERROR: Unable to open parameters file '" << auxResultFile << "'." << std::endl; 
            exit(0);
        }
        std::cout << value << std::endl;
        if (value == "refused"){
            std::cout << "Demand " << std::to_string(getToBeRouted_k(d).getId()+1) << ": Unfeasible." << std::endl;
            cuts.push_back(getPathEliminationConstraint(d));
        }
        else if (value == ""){
            std::cerr << "Request" << getToBeRouted_k(d).getId()+1 << " not found!" << std::endl;
            exit(0);
        }else{
            std::cout << "Demand " << std::to_string(getToBeRouted_k(d).getId()+1) << ": OK." << std::endl;
        }
    }
    return cuts;
}


Constraint DrFormulation::getPathEliminationConstraint(int d){
    int nbHops = 0;
    Expression exp;
    int nbEdges = countEdges(compactGraph);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        if ((y[edge][d].getVal() >= 1 - EPS)||(y[edge + nbEdges][d].getVal() >= 1 - EPS)){
            nbHops++;
            Term term(y[edge][d], 1);
            Term term2(y[edge + nbEdges][d], 1);
            exp.addTerm(term);
            exp.addTerm(term2);
        }
    }
    int rhs = nbHops-1;
    std::ostringstream constraintName;
    constraintName << "PathElimination(" << std::to_string(getToBeRouted_k(d).getId()+1) << ")";
    return Constraint(0, exp, rhs, constraintName.str());
}

void DrFormulation::writePathFile(const std::string &file){
    std::ofstream pathsFile;
    pathsFile.open(file.c_str());
    pathsFile << "demand;transpId;band;path";
    pathsFile << std::endl;
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        int band = 1;
        int transp = getToBeRouted_k(d).getTranspIdC();
        pathsFile <<  d+1 <<";"<<  band <<";"<<  transp <<";";
        std::vector<int> path = getPathNodeSequence(d);
        for (unsigned int i = 0; i < path.size(); i++){
            pathsFile << std::to_string(path[i]+1);
            if (i < path.size()-1){
                pathsFile <<  "-";
            }
        }
        pathsFile << std::endl;
    }
    pathsFile.close();
    
}

std::vector<int> DrFormulation::getPathNodeSequence(int d){
    std::vector<int> path;
    int nbEdges = countEdges(compactGraph);
    int origin = getToBeRouted_k(d).getSource();
    int destination = getToBeRouted_k(d).getTarget();
    ListDigraph::Node SOURCE = getFirstNodeFromLabel(d, origin);
    ListDigraph::Node TARGET = getFirstNodeFromLabel(d, destination);
    if (TARGET == INVALID || SOURCE == INVALID){
        std::cout << "ERROR: Could not find source or target from demand " << getToBeRouted_k(d).getId() + 1 << "." << std::endl;
        exit(0);
    }
    path.push_back(getNodeLabel(SOURCE, d));
    //std::cout << "Search the path from origin to destination." << std::endl;
    ListDigraph::Node currentNode = SOURCE;
    while (currentNode != TARGET){
        ListDigraph::Arc nextArc = INVALID;
        for (ListDigraph::OutArcIt a(*vecGraph[d], currentNode); a != INVALID; ++a){
            int edge = getArcLabel(a, d);
            int u = getNodeLabel((*vecGraph[d]).source(a), d) ;
            int v = getNodeLabel((*vecGraph[d]).target(a), d) ;
            int slice = getArcSlice(a,d);
            if (u < v){
                if (y[edge][d].getVal() >= 1 - EPS){
                    nextArc = a;
                }
            }else{
                if (y[edge+nbEdges][d].getVal() >= 1 - EPS){
                    nextArc = a;
                }
            }
        }
        if (nextArc == INVALID){
            std::cout << "ERROR: Could not find path continuity.." << std::endl;
            exit(0);
        }
        int previousNode = getNodeLabel(currentNode, d);
        currentNode = (*vecGraph[d]).target(nextArc);
        path.push_back(getNodeLabel(currentNode, d));        
    }
    return path;
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
    const std::vector<Input::ObjectiveMetric> & chosenObjectives = instance.getInput().getChosenObj();
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_SLUS){
        for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
            int edge = getCompactEdgeLabel(e);
            std::cout << maxSlicePerLink[edge].getName() << " = " << maxSlicePerLink[edge].getVal() << "   ";
        }
        std::cout << std::endl;
    }
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_NLUS){
        std::cout << maxSliceOverall.getName() << " = " << maxSliceOverall.getVal() << std::endl;
        std::cout << std::endl; 
    }
    /*for (int k = 0; k < getNbDemandsToBeRouted(); k++){
        std::cout << routedCBand[k].getName() << " = " << routedCBand[k].getVal() << "   ";
    }*/
    std::cout << std::endl;
}

std::string DrFormulation::displayDimensions(){
    return "|D| = " + std::to_string(getNbDemandsToBeRouted()) + ", |S| = " + std::to_string(getNbSlicesGlobalLimit()) + ".";
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





