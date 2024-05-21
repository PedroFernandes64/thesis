#include "tFlowForm.h"

/****************************************************************************************/
/*							     		Constructor										*/
/****************************************************************************************/

/* Constructor. Builds the Online RSA mixed-integer program and solves it using a defined solver (CPLEX or SCIP). */
TFlowForm::TFlowForm(const Instance &instance) : AbstractFormulation(instance){
    //Seting type of non overlapping constraints and variables
    this->setNonOverlappingType();   
    slicesC = instance.getTabEdge()[0].getNbSlicesC();
    slicesL = instance.getTabEdge()[0].getNbSlicesL();
    slicesTotal = slicesC + slicesL;

    ClockTime time(ClockTime::getTimeNow());
    ClockTime time2(ClockTime::getTimeNow());
    std::cout << "--- T-Flow formulation has been chosen. " << displayDimensions() << " ---" << std::endl;
    this->setVariables();
    varImpleTime = time.getTimeInSecFromStart() ;
    time.setStart(ClockTime::getTimeNow());
    this->setConstraints();
    constImpleTime = time.getTimeInSecFromStart() ;
    time.setStart(ClockTime::getTimeNow());
    this->setObjectives();
    objImpleTime = time.getTimeInSecFromStart() ;
    std::cout << "--- T-flow formulation has been defined ---" << std::endl;
    totalImpleTime = time2.getTimeInSecFromStart() ;
}

/****************************************************************************************/
/*										Variables    									*/
/****************************************************************************************/

void TFlowForm::printVariables(){
    VarArray v = this->getVariables();
    for(int i = 0; i < v.size(); i++){
        std::cout << std::endl << v[i].getName() << std::endl;
        std::cout << v[i].getVal() << std::endl;
    }
}

void TFlowForm::setVariables(){
    this->setFlowVariables();
    this->setChannelVariables();
    if((NonOverlappingType == 3) ||(NonOverlappingType == 4) ){
        this->setAuxiliaryVariables();
    }
    const std::vector<Input::ObjectiveMetric> & chosenObjectives = instance.getInput().getChosenObj();
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_SLUS){
        this->setMaxUsedSlicePerEdgeVariables();
    }
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_NLUS){
        this->setMaxUsedSliceOverallVariable();
    }
    //if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_ADS){
    //    this->setAcceptedDemandVariable();
    //}
    if(nbBands>1){                              // IF OF
        if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_LLB){
            this->setMultibandVariables();
        }
    }
    std::cout << "T-Flow variables have been defined..." << std::endl; 
}


/* Defines variables x[a][d] and y[s][d] for every demand d, arc a and slot s in the extedend graph #d. */
void TFlowForm::setFlowVariables(){
    // Variables x[a][d]
    int nbEdges = countEdges(compactGraph);
    x.resize(2*nbEdges);
    int uId, vId;
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        if (getCompactNodeLabel(compactGraph.u(e)) < getCompactNodeLabel(compactGraph.v(e))){
            uId = getCompactNodeLabel(compactGraph.u(e)) + 1;
            vId = getCompactNodeLabel(compactGraph.v(e)) + 1;
        }
        else{
            vId = getCompactNodeLabel(compactGraph.u(e)) + 1;
            uId = getCompactNodeLabel(compactGraph.v(e)) + 1;
        }

        x[edge].resize(getNbDemandsToBeRouted());  
        x[edge + nbEdges].resize(getNbDemandsToBeRouted());
        for (int k = 0; k < getNbDemandsToBeRouted(); k++){
            std::ostringstream varName, varName2;
            varName << "f";
            varName << "(" + std::to_string(getToBeRouted_k(k).getId() + 1)  + "," ;
            varName << std::to_string(uId)+ "," ;
            varName << std::to_string(vId) + ")";
            varName2 << "f";
            varName2 <<  "(" + std::to_string(getToBeRouted_k(k).getId() + 1)  + "," ;
            varName2 << std::to_string(vId)+ "," ;
            varName2 << std::to_string(uId) + ")";

            int upperBound = 1;
            int varId = getNbVar();
            
            if(instance.getInput().isRelaxed()){
                x[edge][k] = Variable(varId, 0, upperBound, Variable::TYPE_REAL, 0, varName.str());
                x[edge + nbEdges][k] = Variable(varId + 1, 0, upperBound, Variable::TYPE_REAL, 0, varName2.str());
            }
            else{
                x[edge][k] = Variable(varId, 0, upperBound, Variable::TYPE_BOOLEAN, 0, varName.str());
                x[edge + nbEdges][k] = Variable(varId + 1, 0, upperBound, Variable::TYPE_BOOLEAN, 0, varName2.str());
            }
            incNbVar();
            incNbVar();
        }
    }
}

void TFlowForm::setChannelVariables(){
    // Variables y[s][d]
    y.resize(slicesTotal);
    for (int s = 0; s < slicesTotal; s++){
        y[s].resize(getNbDemandsToBeRouted());  
        for (int k = 0; k < getNbDemandsToBeRouted(); k++){
            std::ostringstream varName;
            varName << "c";
            varName << "(" +  std::to_string(getToBeRouted_k(k).getId() + 1) + "," ;
            varName <<  std::to_string(s + 1) + ")";
            int upperBound = 1;
            int varId = getNbVar();
            if(instance.getInput().isRelaxed()){
                y[s][k] = Variable(varId, 0, upperBound, Variable::TYPE_REAL, 0, varName.str());
            }
            else{
                y[s][k] = Variable(varId, 0, upperBound, Variable::TYPE_BOOLEAN, 0, varName.str());
            }
            
            incNbVar();
        }
    }
}

void TFlowForm::setAuxiliaryVariables(){    
    // Variables z[a][b]
    z.resize(getNbDemandsToBeRouted());
    for (int a = 0; a < getNbDemandsToBeRouted(); a++){
        z[a].resize(getNbDemandsToBeRouted());
        for (int b = a + 1; b < getNbDemandsToBeRouted(); b++){
            std::ostringstream varName;
            varName << "z";
            varName << "(" + std::to_string(getToBeRouted_k(a).getId() + 1) + "," ;
            varName <<  std::to_string(getToBeRouted_k(b).getId() + 1) + ")";
            int upperBound = 1;
            int varId = getNbVar();
            if(instance.getInput().isRelaxed()){
                z[a][b] = Variable(varId, 0, upperBound, Variable::TYPE_REAL, 0, varName.str(),1);
            }
            else{
                z[a][b] = Variable(varId, 0, upperBound, Variable::TYPE_BOOLEAN, 0, varName.str(),1);
            }
            
            incNbVar();
        }
    }
}

void TFlowForm::setMaxUsedSlicePerEdgeVariables(){
    // Max slice variables
    int nbEdges = countEdges(compactGraph);
    maxSlicePerLink.resize(nbEdges);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        std::string varName = "maxSlice(" + std::to_string(edge) + ")";
        int lowerBound = std::max(0,instance.getPhysicalLinkFromIndex(edge).getMaxUsedSlicePosition());
        int upperBound = slicesTotal;
        int varId = getNbVar();
        if (instance.getInput().isRelaxed()){
            maxSlicePerLink[edge] = Variable(varId, lowerBound, upperBound, Variable::TYPE_REAL, 0, varName);
        }
        else{
            maxSlicePerLink[edge] = Variable(varId, lowerBound, upperBound, Variable::TYPE_INTEGER, 0, varName);
        }
        incNbVar();
    }
}

void TFlowForm::setMaxUsedSliceOverallVariable(){
    // Max slice overall
    std::string varName = "maxSliceOverall";
    int lowerBound = std::max(0,instance.getMaxUsedSlicePosition());
    int upperBound = slicesTotal;
    int varId = getNbVar();
    if (instance.getInput().isRelaxed()){
        maxSliceOverall = Variable(varId, lowerBound, upperBound, Variable::TYPE_REAL, 0, varName);
    }
    else{
        maxSliceOverall = Variable(varId, lowerBound, upperBound, Variable::TYPE_INTEGER, 0, varName);
    }
    incNbVar();
}

void TFlowForm::setMultibandVariables(){
    // C Band routing variables
    int nbEdges = countEdges(compactGraph);
    l.resize(nbEdges);
    l2.resize(nbEdges);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        std::string varName = "l(" + std::to_string(edge+1) + ")";
        int lowerBound = 0;
        int upperBound = 1;
        int varId = getNbVar();
        if (instance.getInput().isRelaxed()){
            l[edge] = Variable(varId, lowerBound, upperBound, Variable::TYPE_REAL, 0, varName);
        }
        else{
            l[edge] = Variable(varId, lowerBound, upperBound, Variable::TYPE_INTEGER, 0, varName);
        }
        incNbVar();
        l2[edge].resize(getNbDemandsToBeRouted()); 
        for (int k = 0; k < getNbDemandsToBeRouted(); k++){
            std::ostringstream varName;
            varName << "l2";
            varName << "(" +  std::to_string(getToBeRouted_k(k).getId() + 1) + "," ;
            varName <<  std::to_string(edge+1) + ")";
            int upperBound = 1;
            int varId = getNbVar();
            if(instance.getInput().isRelaxed()){
                l2[edge][k] = Variable(varId, 0, upperBound, Variable::TYPE_REAL, 0, varName.str());
            }
            else{
                l2[edge][k] = Variable(varId, 0, upperBound, Variable::TYPE_BOOLEAN, 0, varName.str());
            }
            incNbVar();
        }

    }
}

VarArray TFlowForm::getVariables(){
    // Variables x[a][d]
    VarArray vec;
    vec.resize(getNbVar());
    int nbEdges = countEdges(compactGraph);
    
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        
        int edge = getCompactEdgeLabel(e);
        for (int k = 0; k < getNbDemandsToBeRouted(); k++){
            int pos = x[edge][k].getId();
            int pos2 = x[edge + nbEdges][k].getId();
            vec[pos] = x[edge][k];
            vec[pos2] = x[edge + nbEdges][k];
        }
    }
    
    // Variables y[s][d]
    for (int s = 0; s < slicesTotal; s++){
        for (int k = 0; k < getNbDemandsToBeRouted(); k++){
            int pos = y[s][k].getId();
            vec[pos] = y[s][k];
        }
    }
            // Variables z[a][b]
    if((NonOverlappingType == 3) ||(NonOverlappingType == 4) ){
        for (int a = 0; a < getNbDemandsToBeRouted(); a++){
            for (int b = a + 1; b < getNbDemandsToBeRouted(); b++){
                int pos = z[a][b].getId();
                vec[pos] = z[a][b];
            }
        }
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

    if(nbBands>1){
        if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_LLB){
        // multiband variables
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                int edge = getCompactEdgeLabel(e);
                int pos = l[edge].getId();
                vec[pos] = l[edge];
            }  
            
            // multiband variables
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){ 
                int edge = getCompactEdgeLabel(e);
                for (int k = 0; k < getNbDemandsToBeRouted(); k++){
                    int pos = l2[edge][k].getId();
                    vec[pos] = l2[edge][k];
                }
            }
        }
    }
    return vec;
}

void TFlowForm::setVariableValues(const std::vector<double> &vals){
    int nbEdges = countEdges(compactGraph);
    
    // Variables x[a][d]
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        for (int k = 0; k < getNbDemandsToBeRouted(); k++){
            int pos = x[edge][k].getId();
            int pos2 = x[edge + nbEdges][k].getId();
            double newValue = vals[pos];
            double newValue2 = vals[pos2];
            x[edge][k].setVal(newValue);
            x[edge + nbEdges][k].setVal(newValue2);
        }
    }
    // Variables y[s][d]
    int sliceLimit = slicesTotal;
    for (int s = 0; s < sliceLimit; s++){
        for (int k = 0; k < getNbDemandsToBeRouted(); k++){
            int pos = y[s][k].getId();
            double newValue = vals[pos];
            y[s][k].setVal(newValue);
        }
    }
    // Variables z[a][b]
    if((NonOverlappingType == 3) ||(NonOverlappingType == 4) ){
        for (int a = 0; a < getNbDemandsToBeRouted(); a++){
            for (int b = a + 1; b < getNbDemandsToBeRouted(); b++){
                int pos = z[a][b].getId();
                double newValue = vals[pos];
                z[a][b].setVal(newValue);
            }
        }
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
    // Max slice overall
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_NLUS){
        int pos = maxSliceOverall.getId();
        double newValue = vals[pos];
        maxSliceOverall.setVal(newValue);
    }

    if(nbBands>1){
        if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_LLB){
    // multiband variables
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                int edge = getCompactEdgeLabel(e);
                int pos = l[edge].getId();
                double newValue = vals[pos];
                l[edge].setVal(newValue);
            }  

            // multiband variables
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){ 
                int edge = getCompactEdgeLabel(e);
                for (int k = 0; k < getNbDemandsToBeRouted(); k++){
                    int pos = l2[edge][k].getId();
                    double newValue = vals[pos];
                    l2[edge][k].setVal(newValue);
                }
            }
        }
    }
}

/****************************************************************************************/
/*									Objective Function    								*/
/****************************************************************************************/

/* Sets the objective Function */
void TFlowForm::setObjectives(){
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
Expression TFlowForm::getObjFunctionFromMetric(Input::ObjectiveMetric chosenObjective){
    Expression obj;
    int nbEdges = countEdges(compactGraph);
    switch (chosenObjective){
        case Input::OBJECTIVE_METRIC_0:{
            break;
        }
        case Input::OBJECTIVE_METRIC_1:{
            for (int s = 0; s < slicesTotal; s++){
                for (int k = 0; k < getNbDemandsToBeRouted(); k++){
                    Term term(y[s][k], s+1);
                    obj.addTerm(term);
                }
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
                    Term term(x[edge][k], 1);
                    Term term2(x[edge + nbEdges][k], 1);
                    obj.addTerm(term);
                    obj.addTerm(term2);
                }
            }
            break;
        }
        case Input::OBJECTIVE_METRIC_TUS:{
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                int edge = getCompactEdgeLabel(e);
                for (int k = 0; k < getNbDemandsToBeRouted(); k++){
                    Term term(x[edge][k], getToBeRouted_k(k).getLoadC());
                    Term term2(x[edge + nbEdges][k], getToBeRouted_k(k).getLoadC());
                    obj.addTerm(term);
                    obj.addTerm(term2);
                    if(nbBands>1){
                        for (int s = slicesC; s < slicesC + slicesL; s++){
                            Term term3(y[s][k], (getToBeRouted_k(k).getLoadL() - getToBeRouted_k(k).getLoadC()));
                            obj.addTerm(term3);
                        }
                    }
                }
            }
            break;
        }
        case Input::OBJECTIVE_METRIC_TRL:{
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                int edge = getCompactEdgeLabel(e);
                for (int k = 0; k < getNbDemandsToBeRouted(); k++){
                    Term term(x[edge][k], getCompactLength(e));
                    Term term2(x[edge + nbEdges][k], getCompactLength(e));
                    obj.addTerm(term);
                    obj.addTerm(term2);
                }
            }
            break;
        }
        case Input::OBJECTIVE_METRIC_TUA:{
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                int edge = getCompactEdgeLabel(e);
                for (int k = 0; k < getNbDemandsToBeRouted(); k++){
                    Term term(x[edge][k], getCompactLineAmplifiers(e));
                    Term term2(x[edge + nbEdges][k], getCompactLineAmplifiers(e));
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
        //TODO
        case Input::OBJECTIVE_METRIC_TOS:{
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                int edge = getCompactEdgeLabel(e);
                for (int k = 0; k < getNbDemandsToBeRouted(); k++){
                    double pnli = ceil(getCompactPnliC(e)* pow(10,8)*100)/100; //ROUNDING
                    double paseLine = ceil(getCompactPaseLineC(e)* pow(10,8)*100)/100; //ROUNDING
                    double paseNode = ceil(instance.getPaseNodeC() * pow(10,8)*100)/100; //ROUNDING
                    Term term(x[edge][k], -((pnli + paseLine + paseNode)/getToBeRouted_k(k).getLoadC()));
                    Term term2(x[edge + nbEdges][k], -((pnli + paseLine + paseNode)/getToBeRouted_k(k).getLoadC()));
                    obj.addTerm(term);
                    obj.addTerm(term2);
                }
            }
            break;
        }

        case Input::OBJECTIVE_METRIC_ADS:{
            for (int k = 0; k < getNbDemandsToBeRouted(); k++){    
                for (int s = 0; s < slicesC; s++){            
                    Term term(y[s][k], -getToBeRouted_k(k).getLoadC());
                    obj.addTerm(term);
                }
                if(nbBands>1){
                    for (int s = slicesC; s < slicesC + slicesL; s++){
                        Term term2(y[s][k], -getToBeRouted_k(k).getLoadL());
                        obj.addTerm(term2);
                    }
                }
            }
            break;
        }

        case Input::OBJECTIVE_METRIC_LLB:{
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                int edge = getCompactEdgeLabel(e);   
                Term term(l[edge], 1);
                obj.addTerm(term);
            }
            break;
        }

        case Input::OBJECTIVE_METRIC_DCB:{
            for (int k = 0; k < getNbDemandsToBeRouted(); k++){    
                for (int s = 0; s < slicesC; s++){            
                    Term term(y[s][k], -1);
                    obj.addTerm(term);
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
/*									Base Constraints    								*/
/****************************************************************************************/

void TFlowForm::setConstraints(){
    setSourceConstraints();
    setTargetConstraints();
    setDegreeConstraints();
    setFlowConservationConstraints();
    setSliceConstraint();

    if (this->getInstance().getInput().isMaxReachEnabled() == true){
        this->setLengthConstraints();
    }
    if (this->getInstance().getInput().isOSNREnabled() == true){
        this->setOSNRConstraints();
    }

    const std::vector<Input::ObjectiveMetric> & chosenObjectives = instance.getInput().getChosenObj();
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_SLUS){
        this->setMaxUsedSlicePerLinkConstraints();
    }
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_NLUS){
        this->setMaxUsedSliceOverallConstraints();
    }
    
    if (NonOverlappingType == 0){
        std::cout << "No non-overlapping constraints have been defined..." << std::endl;
    }else{
        if (NonOverlappingType == 1){
            setNonOverlappingConstraintsPair();
        }else{
            if(NonOverlappingType == 2){
                setNonOverlappingConstraintsMinChannel();
            }else{
                if(NonOverlappingType == 3){
                    setNonOverlappingConstraintsSharedLink();
                }else{
                    if(NonOverlappingType == 4){
                        setNonOverlappingConstraintsSpectrumPosition();
                    }
                    else{
                        std::cout << "WARNING: No Non-overlapping constraints policy chosen." << std::endl;
                    }
                }
            }
        }
    }

    if(nbBands>1){                              // IF OF
        if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_LLB){
            this->setMultibandConstraints();
        }
    }

    
    nbConstraint = constraintSet.size();
    if (this->getInstance().getInput().getChosenPreprLvl() >= Input::PREPROCESSING_LVL_PARTIAL){
        setPreprocessingConstraints();
    }
}

/* Returns the source constraint associated with a demand. */

//ONE SLOT MUST BE CHOSEN
//IF MIN REJECTION CAN BE CHOSEN
Constraint TFlowForm::getSourceConstraint_k(int k){
    Expression exp;
    int lowerBound = 1;
    const std::vector<Input::ObjectiveMetric> & chosenObjectives = instance.getInput().getChosenObj();
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_ADS){
        lowerBound = 0;
    }
    upperBound = 1;

    for (int s = 0; s < slicesTotal; s++){
        Term term(y[s][k], 1);
        exp.addTerm(term);
    }
    std::ostringstream constraintName;
    constraintName << "Source_" << getToBeRouted_k(k).getId()+1 ;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

//ONE ARC MUST LEAVE THE ORIGIN
//IF MIN REJECTION ONE CAN
Constraint TFlowForm::getSourceConstraint_k2(int k){
    Expression exp;
    int upperBound = 1;
    int lowerBound = 1;
    int source = getToBeRouted_k(k).getSource();
    int nbEdges = countEdges(compactGraph);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int labelU = getCompactNodeLabel(compactGraph.u(e));
        int labelV = getCompactNodeLabel(compactGraph.v(e));
        int edge = getCompactEdgeLabel(e);
        if(labelU < labelV){
            if(labelU == source){
                Term term(x[edge][k], 1);
                exp.addTerm(term);
            }
            else if(labelV == source){
                Term term(x[edge + nbEdges][k], 1);
                exp.addTerm(term);
            }
        }
        else{
            if(labelU == source){
                Term term(x[edge + nbEdges][k], 1);
                exp.addTerm(term);
            }
            else if(labelV == source){
                Term term(x[edge][k], 1);
                exp.addTerm(term);
            }
        }
    }
    
    const std::vector<Input::ObjectiveMetric> & chosenObjectives = instance.getInput().getChosenObj();
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_ADS){
        for (int s = 0; s < slicesTotal; s++){
            Term term(y[s][k], -1);
            exp.addTerm(term);
        }
        lowerBound = 0;
    }
    std::ostringstream constraintName;
    constraintName << "Source2_" << getToBeRouted_k(k).getId()+1 ;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

//NO ARC MUST ENTER THE ORIGIN
Constraint TFlowForm::getSourceConstraint_k3(int k){
    Expression exp;
    int upperBound = 0;
    int lowerBound = 0;
    int source = getToBeRouted_k(k).getSource();
    int nbEdges = countEdges(compactGraph);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int labelU = getCompactNodeLabel(compactGraph.u(e));
        int labelV = getCompactNodeLabel(compactGraph.v(e));
        int edge = getCompactEdgeLabel(e);
        if(labelU < labelV){
            if(labelU == source){
                Term term(x[edge + nbEdges][k], 1);
                exp.addTerm(term);
            }
            else if(labelV == source){
                Term term(x[edge][k], 1);
                exp.addTerm(term);
            }
        }
        else{
            if(labelU == source){
                Term term(x[edge][k], 1);
                exp.addTerm(term);
            }
            else if(labelV == source){
                Term term(x[edge + nbEdges][k], 1);
                exp.addTerm(term);
            }
        }
    }

    std::ostringstream constraintName;
    constraintName << "Source3_" << getToBeRouted_k(k).getId()+1 ;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;

}

/* Defines Source constraints. For each demand, exactly one arc leaves the source, no arc enters the source and exactly one slot is assigned (as the last slot). */
void TFlowForm::setSourceConstraints(){
    for (int k = 0; k < getNbDemandsToBeRouted(); k++){
        const Constraint & originConstraint = getSourceConstraint_k(k); //ONE SLOT MUST BE CHOSEN IF MAX ACCEPT ONE CAN BE CHOSEN
        const Constraint & originConstraint2 = getSourceConstraint_k2(k); //ONE ARC MUST LEAVE THE ORIGIN IF MAX ACCEPT ONE CAN
        const Constraint & originConstraint3 = getSourceConstraint_k3(k); //NO ARC MUST ENTER THE ORIGIN
        constraintSet.push_back(originConstraint);
        constraintSet.push_back(originConstraint2);
        constraintSet.push_back(originConstraint3);
    }
    std::cout << "Origin constraints have been defined..." << std::endl;
}

//NO ARC MUST LEAVE THE DESTINATION
Constraint TFlowForm::getTargetConstraint_k(int k){
    Expression exp;
    int upperBound = 0;
    int lowerBound = 0;
    int target = getToBeRouted_k(k).getTarget();
    int nbEdges = countEdges(compactGraph);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int labelU = getCompactNodeLabel(compactGraph.u(e));
        int labelV = getCompactNodeLabel(compactGraph.v(e));
        int edge = getCompactEdgeLabel(e);
        if(labelU < labelV){
            if(labelU == target){
                Term term(x[edge][k], 1);
                exp.addTerm(term);
            }
            else if(labelV == target){
                Term term(x[edge + nbEdges][k], 1);
                exp.addTerm(term);
            }
        }
        else{
            if(labelU == target){
                Term term(x[edge + nbEdges][k], 1);
                exp.addTerm(term);
            }
            else if(labelV == target){
                Term term(x[edge][k], 1);
                exp.addTerm(term);
            }
        }
    }
    std::ostringstream constraintName;
    constraintName << "Target_" << getToBeRouted_k(k).getId()+1 ;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;

}

//ONE ARC MUST ENTER THE DESTINATION
//IF MIN REJECTION MAY ENTER
Constraint TFlowForm::getTargetConstraint_k2(int k){
    Expression exp;
    int lowerBound = 1;
    const std::vector<Input::ObjectiveMetric> & chosenObjectives = instance.getInput().getChosenObj();
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_ADS){
        lowerBound = 0;
    }
    int upperBound = 1;
    int target = getToBeRouted_k(k).getTarget();
    int nbEdges = countEdges(compactGraph);

    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int labelU = getCompactNodeLabel(compactGraph.u(e));
        int labelV = getCompactNodeLabel(compactGraph.v(e));
        int edge = getCompactEdgeLabel(e);
        if(labelU < labelV){
            if(labelU == target){
                Term term(x[edge + nbEdges][k], 1);
                exp.addTerm(term);
            }
            else if(labelV == target){
                Term term(x[edge][k], 1);
                exp.addTerm(term);
            }
        }
        else{
            if(labelU == target){
                Term term(x[edge][k], 1);
                exp.addTerm(term);
            }
            else if(labelV == target){
                Term term(x[edge + nbEdges][k], 1);
                exp.addTerm(term);
            }
        }
    }

    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_ADS){
        for (int s = 0; s < slicesTotal; s++){
            Term term(y[s][k], -1);
            exp.addTerm(term);
        }
        lowerBound = 0;
    }

    std::ostringstream constraintName;
    constraintName << "Target2_" << getToBeRouted_k(k).getId()+1 ;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

/* Defines Target constraints. Exactly one arc enters the target and no arc leaves the target. */
void TFlowForm::setTargetConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){   
        const Constraint & targetConstraint = getTargetConstraint_k(d); //NO ARC MUST LEAVE THE DESTINATION
        const Constraint & targetConstraint2 = getTargetConstraint_k2(d); //ONE ARC MUST ENTER THE DESTINATION IF Max accept may ENTER
        constraintSet.push_back(targetConstraint);
        constraintSet.push_back(targetConstraint2);
    }
    std::cout << "Target constraints have been defined..." << std::endl;
}

/* Returns the Length constraint associated with a demand. */
Constraint TFlowForm::getLengthCConstraint_k(int k){
    Expression exp;
    double upperBound = getToBeRouted_k(k).getMaxLengthC();
    int lowerBound = 0;
    int nbEdges = countEdges(compactGraph);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        double coeff = getCompactLength(e);
        Term term(x[edge][k], coeff);
        Term term2(x[edge + nbEdges][k], coeff);
        exp.addTerm(term);
        exp.addTerm(term2);
    }
    if(nbBands>1){
        double coeff = getToBeRouted_k(k).getMaxLengthL()   - getToBeRouted_k(k).getMaxLengthC();
        for (int s = slicesC; s < slicesC+slicesL; s++){
            Term term(y[s][k], coeff);
            exp.addTerm(term);
        }
    }
    std::ostringstream constraintName;
    constraintName << "Length_" << getToBeRouted_k(k).getId()+1;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

/* Defines Length constraints. Demands must be routed within a length limit. */
void TFlowForm::setLengthConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){   
        const Constraint & lengthCConstraint = getLengthCConstraint_k(d);
        constraintSet.push_back(lengthCConstraint);
    }
    std::cout << "Length constraints have been defined..." << std::endl;
}

/* Defines OSNR constraints. Demands must be routed within a OSNR limit. */
void TFlowForm::setOSNRConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){   
        const Constraint & OSNRConstraintC = getOSNRCConstraint(getToBeRouted_k(d), d);
        constraintSet.push_back(OSNRConstraintC);
        if (getInstance().getInput().getNbBands() >= 2) {
            const Constraint & OSNRConstraintL = getOSNRLConstraint(getToBeRouted_k(d), d);
            constraintSet.push_back(OSNRConstraintL);
        } 
    }
    std::cout << "OSNR constraints have been defined..." << std::endl;
}

/* Returns the OSNR constraint associated with a demand. */
Constraint TFlowForm::getOSNRCConstraint(const Demand &demand, int k){
    Expression exp;
    double rhs; double lhs;
    
    double osnrLimdb = demand.getOsnrLimitC();
    double osnrLim = pow(10,osnrLimdb/10);
    double pch = demand.getPchC();

    double roundingFactor = pow(10,8);
    
    rhs = pch/osnrLim - instance.getPaseNodeC() ;
    
    rhs = ceil(rhs * roundingFactor*100)/100 ; //ROUNDING
    lhs = 0;

    int nbEdges = countEdges(compactGraph);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);

        //First term
        double coeff = getCompactPaseLineC(e)* roundingFactor;
        coeff = ceil(coeff*100)/100; //ROUNDING
        Term term(x[edge][k], coeff);
        Term term2(x[edge + nbEdges][k], coeff);
        exp.addTerm(term);
        exp.addTerm(term2);

        //Second term
        coeff = 1.0 * ceil(instance.getPaseNodeC() * roundingFactor*100)/100; //ROUNDING
        Term term3(x[edge][k], coeff);
        Term term4(x[edge + nbEdges][k], coeff);
        exp.addTerm(term3);
        exp.addTerm(term4);

        //Third term
        coeff = getCompactPnliC(e)* roundingFactor;
        coeff = ceil(coeff*100)/100; //ROUNDING
        Term term5(x[edge][k], coeff);
        Term term6(x[edge + nbEdges][k], coeff);
        exp.addTerm(term5);
        exp.addTerm(term6);
    }
    if(nbBands>1){
        double coeff2 = 0.0;
        for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
            int edge = getCompactEdgeLabel(e);
            coeff2 = coeff2 + ceil(getCompactPaseLineC(e)*roundingFactor*100)/100 + ceil(getCompactPnliC(e)*roundingFactor*100)/100 + ceil(instance.getPaseNodeC() * roundingFactor*100)/100;
            //std::cout <<coeff2<< std::endl;
        }
        for (int s = slicesC; s < slicesC+slicesL; s++){
            Term term(y[s][k], -coeff2);
            exp.addTerm(term);
        }
        lhs = -coeff2;
    }
    std::ostringstream constraintName;
    constraintName << "OSNR_" << demand.getId()+1;
    Constraint constraint(lhs, exp, rhs, constraintName.str());
    //std::cout << "For demand " << demand << std::endl;
    //constraint.display();
    return constraint;
}
/* Returns the OSNR constraint associated with a demand. */
Constraint TFlowForm::getOSNRLConstraint(const Demand &demand, int k){
    Expression exp;
    double rhs; double lhs;
    
    double osnrLimdb = demand.getOsnrLimitL();
    double osnrLim = pow(10,osnrLimdb/10);
    double pch = demand.getPchL();

    double roundingFactor = pow(10,8);
    
    rhs = pch/osnrLim - instance.getPaseNodeL() ;
    
    rhs = ceil(rhs * roundingFactor*100)/100 ; //ROUNDING
    lhs = 0;

    int nbEdges = countEdges(compactGraph);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);

        //First term
        double coeff = getCompactPaseLineL(e)* roundingFactor;
        coeff = ceil(coeff*100)/100; //ROUNDING
        Term term(x[edge][k], coeff);
        Term term2(x[edge + nbEdges][k], coeff);
        exp.addTerm(term);
        exp.addTerm(term2);

        //Second term
        coeff = 1.0 * ceil(instance.getPaseNodeL() * roundingFactor*100)/100; //ROUNDING
        Term term3(x[edge][k], coeff);
        Term term4(x[edge + nbEdges][k], coeff);
        exp.addTerm(term3);
        exp.addTerm(term4);

        //Third term
        coeff = getCompactPnliL(e)* roundingFactor;
        coeff = ceil(coeff*100)/100; //ROUNDING
        Term term5(x[edge][k], coeff);
        Term term6(x[edge + nbEdges][k], coeff);
        exp.addTerm(term5);
        exp.addTerm(term6);
    }
    if(nbBands>1){
        double coeff2 = 0.0;
        for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
            int edge = getCompactEdgeLabel(e);
            coeff2 = coeff2 + ceil(getCompactPaseLineL(e)*roundingFactor*100)/100 + ceil(getCompactPnliL(e)*roundingFactor*100)/100 + ceil(instance.getPaseNodeL() * roundingFactor*100)/100;
        }
        for (int s = 0; s < slicesC; s++){
            Term term(y[s][k], -coeff2);
            exp.addTerm(term);
        }
        lhs = -coeff2;
    }
    std::ostringstream constraintName;
    constraintName << "OSNR_" << demand.getId()+1;
    Constraint constraint(lhs, exp, rhs, constraintName.str());
    //std::cout << "For demand " << d<< std::endl;
    //constraint.display();
    return constraint;
}

/* Returns the degree constraint associated with a demand k and a node v. */
Constraint TFlowForm::getDegreeConstraint_k(int k, const ListGraph::Node &v){
    Expression exp;
    int upperBound = 1;
    int lowerBound = 0;
    int nbEdges = countEdges(compactGraph);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int labelU = getCompactNodeLabel(compactGraph.u(e));
        int labelV = getCompactNodeLabel(compactGraph.v(e));
        int edge = getCompactEdgeLabel(e);
        if(labelU < labelV){
            if(labelU == getCompactNodeLabel(v)){
                Term term(x[edge][k], 1);
                exp.addTerm(term);
            }
            else if(labelV == getCompactNodeLabel(v)){
                Term term(x[edge + nbEdges][k], 1);
                exp.addTerm(term);
            }
        }
        else{
            if(labelU == getCompactNodeLabel(v)){
                Term term(x[edge + nbEdges][k], 1);
                exp.addTerm(term);
            }
            else if(labelV == getCompactNodeLabel(v)){
                Term term(x[edge][k], 1);
                exp.addTerm(term);
            }
        }
    }
    
    std::ostringstream constraintName;
    constraintName << "Degree_" << getToBeRouted_k(k).getId()+1 << "_" << getCompactNodeLabel(v)+1;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

/* Defines Degree constraints. At most one arc leaves each node. */
void TFlowForm::setDegreeConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){  
        for (ListGraph::NodeIt v(compactGraph); v != INVALID; ++v){
            int node = getCompactNodeLabel(v);
            if (node != getToBeRouted_k(d).getSource() && node != getToBeRouted_k(d).getTarget()){
                const Constraint & degreeConstraint = getDegreeConstraint_k(d, v);
                constraintSet.push_back(degreeConstraint);
            }
        }
    }
    std::cout << "Degree constraints have been defined..." << std::endl;
}

/* Returns the flow conservation constraint associated with a demand and a node. */
Constraint TFlowForm::getFlowConservationConstraint_v_k(const ListGraph::Node &v, int k){
    Expression exp;
    int rhs = 0;
    int nbEdges = countEdges(compactGraph);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int labelU = getCompactNodeLabel(compactGraph.u(e));
        int labelV = getCompactNodeLabel(compactGraph.v(e));
        int edge = getCompactEdgeLabel(e);
        if(labelU < labelV){
            if(labelU == getCompactNodeLabel(v)){
                Term term(x[edge][k], 1);
                Term term2(x[edge + nbEdges][k], -1);
                exp.addTerm(term);
                exp.addTerm(term2);
            }
            else if(labelV == getCompactNodeLabel(v)){
                Term term(x[edge + nbEdges][k], 1);
                Term term2(x[edge][k], -1);
                exp.addTerm(term);
                exp.addTerm2(term2);
            }
        }
        else{
            if(labelU == getCompactNodeLabel(v)){
                Term term(x[edge + nbEdges][k], 1);
                Term term2(x[edge][k], -1);
                exp.addTerm(term);
                exp.addTerm(term2);
            }
            else if(labelV == getCompactNodeLabel(v)){
                Term term(x[edge][k], 1);
                Term term2(x[edge + nbEdges][k], -1);
                exp.addTerm(term);
                exp.addTerm(term2);
            }
        }
    }
    
    std::ostringstream constraintName;
    constraintName << "Flow_" << getCompactNodeLabel(v)+1 << "_" << getToBeRouted_k(k).getId()+1;
    Constraint constraint(rhs, exp, rhs, constraintName.str());
    return constraint;
}

/* Defines Flow Conservation constraints. The number of arcs entering a node is equal to the number of arcs leaving it. */
void TFlowForm::setFlowConservationConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){   
        for (ListGraph::NodeIt v(compactGraph); v != INVALID; ++v){
            int node = getCompactNodeLabel(v);
            if( (node != getToBeRouted_k(d).getSource()) && (node != getToBeRouted_k(d).getTarget()) ){
                const Constraint & flow = getFlowConservationConstraint_v_k(v, d);
                constraintSet.push_back(flow);
            }
        }
    }
    std::cout << "Flow conservation constraints have been defined..." << std::endl;
}

Constraint TFlowForm::getMaxUsedSlicePerLinkConstraints(int k, int e){
    Expression exp;
    int nbEdges = countEdges(compactGraph);
    int lowerBound = -slicesTotal;
    int upperBound = slicesTotal;
    Term term1(x[e][k], slicesTotal);
    exp.addTerm(term1);
    Term term2(x[e + nbEdges][k], slicesTotal);
    exp.addTerm(term2);
    for (int s = 0; s < slicesTotal; s++){
        Term term(y[s][k], s+1);
        exp.addTerm(term);
    }
    Term term4(maxSlicePerLink[e], -1);
    exp.addTerm(term4);

    std::ostringstream constraintName;
    constraintName << "MaxUsedSlicePerLink_" << e+1 << "_" << getToBeRouted_k(k).getId()+1;
    Constraint constraint (lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

void TFlowForm::setMaxUsedSlicePerLinkConstraints(){
    for (int k = 0; k < getNbDemandsToBeRouted(); k++){
        for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
            int edge = getCompactEdgeLabel(e);
            const Constraint & maxUsedSlicePerLinkConstraint = getMaxUsedSlicePerLinkConstraints(k, edge);
            constraintSet.push_back(maxUsedSlicePerLinkConstraint);
        }
    }
    std::cout << "Max Used Slice Per Link constraints have been defined..." << std::endl;

}

Constraint TFlowForm::getMaxUsedSliceOverallConstraints(int k){
    Expression exp;
    int lowerBound = -slicesTotal;
    int upperBound = 0;
    for (int s = 0; s < slicesTotal; s++){
        Term term(y[s][k], s+1);
        exp.addTerm(term);
    }
    Term term2(maxSliceOverall, -1);
    exp.addTerm(term2);

    std::ostringstream constraintName;
    constraintName << "MaxUsedSliceOverall_" << getToBeRouted_k(k).getId()+1;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

void TFlowForm::setMaxUsedSliceOverallConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        const Constraint & maxUsedSliceOverallConst = getMaxUsedSliceOverallConstraints(d);
        constraintSet.push_back(maxUsedSliceOverallConst);
    }
    std::cout << "Max Used Slice Overall constraints have been defined..." << std::endl;
}

Constraint TFlowForm::getSliceConstraint(int k){
    Expression exp;
    int rhs = 0;
    for (int s = 0; s < getToBeRouted_k(k).getLoadC() - 1; s++){
        Term term(y[s][k], 1);
        exp.addTerm(term);
    }
    if(nbBands>1){
        for (int s = slicesC; s < slicesC + getToBeRouted_k(k).getLoadL() - 1; s++){
            Term term(y[s][k], 1);
            exp.addTerm(term);
        }
    }
    std::ostringstream constraintName;
    constraintName << "Slice_" << getToBeRouted_k(k).getId()+1;
    Constraint constraint(rhs, exp, rhs, constraintName.str());
    return constraint;
}

void TFlowForm::setSliceConstraint(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        const Constraint & slice = getSliceConstraint(d);
        constraintSet.push_back(slice);
    }
    std::cout << "Slice constraints have been defined..." << std::endl;
}

void TFlowForm::setNonOverlappingConstraintsPair(){
    /*
    int counter = 0;
    for (int a = 0; a < getNbDemandsToBeRouted(); a++){
        for (int b = 0; b < getNbDemandsToBeRouted(); b++){
            if(a < b){
                for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                    int edge = getCompactEdgeLabel(e);
                    for (int s = 0; s < slicesBand*bands; s++){
                        const Constraint & nonOverlappingPair = getNonOverlappingConstraintPair(a, b, edge, s);
                        constraintSet.push_back(nonOverlappingPair);
                        counter = counter + 1;
                    }
                }
            }
        }
    }
    std::cout << "Non-overlapping constraints pair have been defined..." << std::endl;
    std::cout << "Constraints = " << counter << std::endl;
    */
}

void TFlowForm::setNonOverlappingConstraintsMinChannel(){
    /*
    int counter = 0;
    for (int a = 0; a < getNbDemandsToBeRouted(); a++){
        for (int b = 0; b < getNbDemandsToBeRouted(); b++){
            if(a < b){
                int minWk = 0;
                if (getToBeRouted_k(a).getLoad() <= getToBeRouted_k(b).getLoad()){
                    minWk = getToBeRouted_k(a).getLoad();
                }else{
                    minWk = getToBeRouted_k(b).getLoad();
                }
                for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                    int edge = getCompactEdgeLabel(e);
                    for (int s = 0; s < getNbSlicesGlobalLimit(); s++){
                        if ((s+1)%minWk == 0){
                            const Constraint & nonOverlappingPair = getNonOverlappingConstraintPair(a, b, edge, s);
                            constraintSet.push_back(nonOverlappingPair);
                            counter = counter + 1;
                        }
                    }
                }
            }
        }
    }
    std::cout << "Non-overlapping constraints have been defined..." << std::endl;
    std::cout << "Constraints = " << counter << std::endl;
    */
}

Constraint TFlowForm::getNonOverlappingConstraintPair(int a, int b, int arc, int s){
    /*
    int sliceLimit = getNbSlicesGlobalLimit();
    Expression exp;
    int lowerBound = 0;
    int upperBound = 3;
    int nbEdges = countEdges(compactGraph);
    Term term1(x[arc][a], 1);
    Term term2(x[arc][b], 1);
    Term term3(x[arc + nbEdges][a], 1);
    Term term4(x[arc + nbEdges][b], 1);
    exp.addTerm(term1);
    exp.addTerm(term2);
    exp.addTerm(term3);
    exp.addTerm(term4);
    int sWk_1 = s + getToBeRouted_k(a).getLoad()-1;
    if (sWk_1 >= sliceLimit){
        sWk_1 = sliceLimit-1;
    }
    for (int sa = s; sa <= sWk_1; sa++){
        Term term(y[sa][a], 1);
        exp.addTerm(term);
    }
    sWk_1 = s + getToBeRouted_k(b).getLoad()-1;
    if (sWk_1 >= sliceLimit){
        sWk_1 = sliceLimit-1;
    }
    for (int sa = s; sa <= sWk_1; sa++){
        Term term(y[sa][b], 1);
        exp.addTerm(term);
    }
    
    std::ostringstream constraintName;
    constraintName << "NonOverlapping_" << getToBeRouted_k(a).getId()+1 << "_" << getToBeRouted_k(b).getId()+1 << "_" << arc;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
    */
}

void TFlowForm::setNonOverlappingConstraintsSharedLink(){
    int counter = 0;
    for (int a = 0; a < getNbDemandsToBeRouted(); a++){
        for (int b = a + 1; b < getNbDemandsToBeRouted(); b++){
            for (int s = 0; s < slicesTotal; s++){
                const Constraint & nonOverlappingSharedLinkConstraint = getNonOverlappingConstraintSharedLink1(a,b, s);
                constraintSet.push_back(nonOverlappingSharedLinkConstraint);
                counter = counter + 1;
            }
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                int edge = getCompactEdgeLabel(e);
                const Constraint & nonOverlappingSharedLinkConstraint2 = getNonOverlappingConstraintSharedLink2(a,b,edge);
                constraintSet.push_back(nonOverlappingSharedLinkConstraint2);
                counter = counter + 1;
            }
        }
    }
    std::cout << "Non Overlapping Shared Link constraints have been defined..." << std::endl;
    std::cout << "Constraints = " << counter << std::endl;
}

Constraint TFlowForm::getNonOverlappingConstraintSharedLink1(int a, int b, int s){
    Expression exp;
    int lowerBound = 0;
    int upperBound = 2;

    int sliceLimit;
    int sWk_1 = s + getToBeRouted_k(a).getLoadC()-1;;
    int sWk_2 = s + getToBeRouted_k(b).getLoadC()-1;;

    if(s>=slicesC){
        sliceLimit = slicesC+slicesL;
        sWk_1 = s + getToBeRouted_k(a).getLoadL()-1;
        sWk_2 = s + getToBeRouted_k(b).getLoadL()-1;
    }else{
        sliceLimit = slicesC;
    }
    if (sWk_1 >= sliceLimit){
        sWk_1 = sliceLimit-1;
    }
    for (int sa = s; sa <= sWk_1; sa++){
        Term term(y[sa][a], 1);
        exp.addTerm(term);
    }

    if (sWk_2 >= sliceLimit){
        sWk_2 = sliceLimit-1;
    }
    for (int sa = s; sa <= sWk_2; sa++){
        Term term(y[sa][b], 1);
        exp.addTerm(term);
    }
    Term term3(z[a][b], 1);
    exp.addTerm(term3);
    std::ostringstream constraintName;
    constraintName << "NonOverlapping1_" << getToBeRouted_k(a).getId()+1 << "_" << getToBeRouted_k(b).getId()+1;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}


Constraint TFlowForm::getNonOverlappingConstraintSharedLink2(int a, int b, int edge){
    Expression exp;
    int lowerBound = -1;
    int upperBound = 1;
    int nbEdges = countEdges(compactGraph);

    Term term1(x[edge][a], 1);
    Term term2(x[edge][b], 1);
    Term term3(x[edge + nbEdges][a], 1);
    Term term4(x[edge + nbEdges][b], 1);
    Term term5(z[a][b], -1);
    exp.addTerm(term1);
    exp.addTerm(term2);
    exp.addTerm(term3);
    exp.addTerm(term4);
    exp.addTerm(term5);
    
    std::ostringstream constraintName;
    constraintName << "NonOverlapping_2" << getToBeRouted_k(a).getId()+1 << "_" << getToBeRouted_k(b).getId()+1 << "_" << edge;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

void TFlowForm::setNonOverlappingConstraintsSpectrumPosition(){
    int counter = 0;
    for (int a = 0; a < getNbDemandsToBeRouted(); a++){
        for (int b = a + 1; b < getNbDemandsToBeRouted(); b++){
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                int edge = getCompactEdgeLabel(e);
                const Constraint & nonOverlappingSpectrumPositionConstraint1 = getNonOverlappingConstraintSpectrumPosition1(a,b,edge);
                constraintSet.push_back(nonOverlappingSpectrumPositionConstraint1);
                counter = counter + 1;
                const Constraint & nonOverlappingSpectrumPositionConstraint2 = getNonOverlappingConstraintSpectrumPosition2(a,b,edge);
                constraintSet.push_back(nonOverlappingSpectrumPositionConstraint2);
                counter = counter + 1;
            }
        }
    }
    std::cout << "Non Overlapping Spectrum Position constraints have been defined..." << std::endl;
        std::cout << "Constraints = " << counter << std::endl;
}

Constraint TFlowForm::getNonOverlappingConstraintSpectrumPosition1(int a, int b, int edge){
    /*
    Expression exp;
    int lowerBound = -2*(getNbSlicesGlobalLimit());
    int upperBound = getToBeRouted_k(a).getLoad();
    int nbEdges = countEdges(compactGraph);

    Term term1(x[edge][a], getToBeRouted_k(a).getLoad());
    Term term2(x[edge][b], getToBeRouted_k(a).getLoad());
    Term term3(x[edge + nbEdges][a], getToBeRouted_k(a).getLoad());
    Term term4(x[edge + nbEdges][b], getToBeRouted_k(a).getLoad());
    Term term5(z[a][b], -(getNbSlicesGlobalLimit()));
    exp.addTerm(term1);
    exp.addTerm(term2);
    exp.addTerm(term3);
    exp.addTerm(term4);
    exp.addTerm(term5);
    for (int s = 0; s < getNbSlicesGlobalLimit(); s++){
        Term term(y[s][a], -(s+1));
        exp.addTerm(term);
    }
    for (int s = 0; s < getNbSlicesGlobalLimit(); s++){
        Term term(y[s][b], s+1);
        exp.addTerm(term);
    }
    
    std::ostringstream constraintName;
    constraintName << "NonOverlapping_1" << getToBeRouted_k(a).getId()+1 << "_" << getToBeRouted_k(b).getId()+1 << "_" << edge;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
    */
}

Constraint TFlowForm::getNonOverlappingConstraintSpectrumPosition2(int a, int b, int edge){
    /*
    Expression exp;
    int lowerBound = -2*(getNbSlicesGlobalLimit());
    int upperBound = getToBeRouted_k(b).getLoad()+getNbSlicesGlobalLimit();
    int nbEdges = countEdges(compactGraph);

    Term term1(x[edge][a], getToBeRouted_k(b).getLoad());
    Term term2(x[edge][b], getToBeRouted_k(b).getLoad());
    Term term3(x[edge + nbEdges][a], getToBeRouted_k(b).getLoad());
    Term term4(x[edge + nbEdges][b], getToBeRouted_k(b).getLoad());
    Term term5(z[a][b], getNbSlicesGlobalLimit());
    exp.addTerm(term1);
    exp.addTerm(term2);
    exp.addTerm(term3);
    exp.addTerm(term4);
    exp.addTerm(term5);
    for (int s = 0; s < getNbSlicesGlobalLimit(); s++){
        Term term(y[s][a], s+1);
        exp.addTerm(term);
    }
    for (int s = 0; s < getNbSlicesGlobalLimit(); s++){
        Term term(y[s][b], -(s+1));
        exp.addTerm(term);
    }    
    std::ostringstream constraintName;
    constraintName << "NonOverlapping_2" << getToBeRouted_k(a).getId()+1 << "_" << getToBeRouted_k(b).getId()+1 << "_" << edge;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
    */
}


void TFlowForm::setMultibandConstraints(){
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        //const Constraint & multibandConstraint5 = getMultibandConstraint5(edge);
        //constraintSet.push_back(multibandConstraint5);
        for (int k = 0; k < getNbDemandsToBeRouted(); k++){  
            const Constraint & multibandConstraint = getMultibandConstraint(k, edge);
            constraintSet.push_back(multibandConstraint);
            /*
            const Constraint & multibandConstraint2 = getMultibandConstraint2(k, edge);
            constraintSet.push_back(multibandConstraint2);
            const Constraint & multibandConstraint3 = getMultibandConstraint3(k, edge);
            constraintSet.push_back(multibandConstraint3);
            
            const Constraint & multibandConstraint4 = getMultibandConstraint4(k, edge);
            constraintSet.push_back(multibandConstraint4);
            
            */

        }
    }
    std::cout << "Multiband constraints have been defined..." << std::endl;
}

// if the demand is routed, it must be in C band (monoband case)
Constraint TFlowForm::getMultibandConstraint(int k, int e){
    Expression exp;
    int upperBound = 1;
    int lowerBound = -1;
    //int lowerBound = 0;
    int nbEdges = countEdges(compactGraph);
    for (int s = slicesC; s < slicesC+slicesL; s++){
        Term term(y[s][k], 1);
        exp.addTerm(term);
    }
    Term term2(x[e][k], 1);
    Term term3(x[e + nbEdges][k], 1);
    Term term4(l[e], -1);
    //Term term4(l2[e][k], -1);
    exp.addTerm(term2);
    exp.addTerm(term3);
    exp.addTerm(term4);

    std::ostringstream constraintName;
    constraintName << "Multiband1_" << getToBeRouted_k(k).getId()+1 << e ;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

Constraint TFlowForm::getMultibandConstraint2(int k, int e){
    Expression exp;
    int upperBound = 0;
    int lowerBound = -1;
    Term term1(l2[e][k], 1);
    Term term2(l[e], -1);
    exp.addTerm(term1);
    exp.addTerm(term2);

    std::ostringstream constraintName;
    constraintName << "Multiband2_" << getToBeRouted_k(k).getId()+1 << e ;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

Constraint TFlowForm::getMultibandConstraint3(int k, int e){
    Expression exp;
    int upperBound = 0;
    int lowerBound = -1;

    Term term1(l2[e][k], 1);
    exp.addTerm(term1);

    for (int s = slicesC; s < slicesC+slicesL; s++){
        Term term(y[s][k], -1);
        exp.addTerm(term);
    }
    
    std::ostringstream constraintName;
    constraintName << "Multiband3_" << getToBeRouted_k(k).getId()+1 << e ;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

Constraint TFlowForm::getMultibandConstraint4(int k, int e){
    Expression exp;
    int upperBound = 0;
    int lowerBound = -1;
    int nbEdges = countEdges(compactGraph);

    Term term1(l2[e][k], 1);
    exp.addTerm(term1);

    Term term2(x[e][k], -1);
    Term term3(x[e + nbEdges][k], -1);
    exp.addTerm(term2);
    exp.addTerm(term3);
    
    std::ostringstream constraintName;
    constraintName << "Multiband4_" << getToBeRouted_k(k).getId()+1 << e ;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

Constraint TFlowForm::getMultibandConstraint5(int e){
    Expression exp;
    int upperBound = 0;
    int lowerBound = 0;
    int nbEdges = countEdges(compactGraph);

    Term term1(l[e], 1);
    exp.addTerm(term1);
    for (int k = 0; k < getNbDemandsToBeRouted(); k++){  
        Term term2(l2[e][k], -1);
        exp.addTerm(term2);
    }
    
    std::ostringstream constraintName;
    constraintName << "Multiband4_" << e ;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

void TFlowForm::setPreprocessingConstraints(){
    for (int k = 0; k < getNbDemandsToBeRouted(); k++){
        const Constraint & preprocessingConstraint = getPreprocessingConstraint(k);
        constraintSet.push_back(preprocessingConstraint);
    }
    /*
    Expression exp;
    Term term(y[14][1], 1);
    Term term2(y[14][0], 1);
    exp.addTerm(term);
    exp.addTerm(term2);
    Constraint preprocessingConstraint2(0.0,exp,0.0,"abba");
    constraintSet.push_back(preprocessingConstraint2);
    */
    std::cout << "Preprocessing constraints have been defined..." << std::endl;
}

Constraint TFlowForm::getPreprocessingConstraint(int k){
    Expression exp;
    double upperBound = 0;
    int lowerBound = 0;
    int nbEdges = countEdges(compactGraph);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        double coeff = 1;
        if (preProcessingErasedArcs[k][edge] == 1){
            Term term(x[edge][k], coeff);
            exp.addTerm(term);
        }
        if (preProcessingErasedArcs[k][edge + nbEdges] == 1){
            Term term2(x[edge + nbEdges][k], coeff);
            exp.addTerm(term2);
        }
    }
    std::ostringstream constraintName;
    constraintName << "Prepro_" << getToBeRouted_k(k).getId()+1;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

/****************************************************************************************/
/*						                Methods                 						*/
/****************************************************************************************/

void TFlowForm::setNonOverlappingType(){
    if (this->getInstance().getInput().getNonOverTFlow() == 0){
        NonOverlappingType = 0;
    }else{
        if (this->getInstance().getInput().getNonOverTFlow() == 1){
            this->NonOverlappingType = 1;
        }else{
            if(this->getInstance().getInput().getNonOverTFlow() == 2){
                this->NonOverlappingType = 2;
            }else{
                if(this->getInstance().getInput().getNonOverTFlow() == 3){
                    this->NonOverlappingType = 3;
                }else{
                    if(this->getInstance().getInput().getNonOverTFlow() == 4){
                        this->NonOverlappingType = 4;
                    }
                    else{
                        std::cout << "WARNING: No Non-overlapping policy chosen." << std::endl;
                    }
                }
            }
        }
    }
}


/* Recovers the obtained MIP solution and builds a path for each demand on its associated graph from RSA. */
void TFlowForm::updatePath(const std::vector<double> &vals){
    setVariableValues(vals);
    int nbEdges = countEdges(compactGraph);
    //Reinitialize OnPath
    //std::cout << "Enter update." << std::endl;
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
                int slice = getArcSlice(a, d);
                int u = getNodeLabel((*vecGraph[d]).source(a), d) + 1;
                int v = getNodeLabel((*vecGraph[d]).target(a), d) + 1;
                if (u < v){
                    if ((x[edge][d].getVal() >= 1 - EPS ) && (y[slice][d].getVal() >= 1 - EPS)){
                        (*vecOnPath[d])[a] = getToBeRouted_k(d).getId();
                        previousArc = a;
                        leftTarget = true;
                    }
                    else{
                        (*vecOnPath[d])[a] = -1;
                    }
                }else{
                    if ((x[edge + nbEdges][d].getVal() >= 1 - EPS) && (y[slice][d].getVal() >= 1 - EPS)){
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

std::vector<Variable> TFlowForm::objective8_fixing(const double upperBound){
    std::vector<Variable> vars;
    int initialSlice = std::ceil(upperBound+0.5);
    for (int k = 0; k < getNbDemandsToBeRouted(); k++){
        for (int s = initialSlice; s < slicesTotal; s++){
            vars.push_back(y[s][k]);
        }
    }
    return vars;
}
/*
std::vector<Constraint> TFlowForm::solveSeparationProblemFract(const std::vector<double> &solution){
    //std::cout << "Solving separation problem fractional..." << std::endl;
    setVariableValues(solution);
    std::vector<Constraint> cuts;
    int nbEdges = countEdges(compactGraph);
    int sliceLimit = getNbSlicesGlobalLimit();
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        for (int s = 0; s < sliceLimit; s++){
            Expression expr;
            int nbElementsC = 0;
            for (int k = 0; k < getNbDemandsToBeRouted(); ++k){
                int sWk_1 = s + getToBeRouted_k(k).getLoad()-1;
                if (sWk_1 >= sliceLimit){
                    sWk_1 = sliceLimit-1;
                }
                if (x[edge][k].getVal() >= 1 - EPS || x[edge + nbEdges][k].getVal() >= 1 - EPS){
                    for (int sa = s; sa <= sWk_1; sa++){
                        if (y[sa][k].getVal() >= 1 - EPS){
                            expr.addTerm(Term(x[edge][k], 1));
                            expr.addTerm(Term(x[edge+nbEdges][k], 1));
                            expr.addTerm(Term(y[sa][k], 1));
                            nbElementsC = nbElementsC + 1;
                        }
                    }
                }
            }
            if(nbElementsC > 1){
                cuts.push_back(Constraint(0, expr, nbElementsC + 1, "name_to_do"));
                //std::cout << "Adding user cut: " << expr.to_string() << " <= "  << nbElementsC + 1<< std::endl;
            }
        }
    }
    return cuts;
}
*/

std::vector<Constraint> TFlowForm::solveSeparationProblemFract(const std::vector<double> &solution){
    //std::cout << "Solving separation problem fractional..." << std::endl;
    setVariableValues(solution);
    std::vector<Constraint> cuts;
    if(NonOverlappingType == 3){
        ListGraph intersectionGraph;
        CompactNodeMap nodeId(intersectionGraph);  
        CompactNodeMap nodeLabel(intersectionGraph);
        EdgeMap edgeId(intersectionGraph);
        EdgeMap edgeLabel(intersectionGraph);
        EdgeCost edgeWeight(intersectionGraph);
        int edgeCounter = 0;

        for (int i = 0; i < getNbDemandsToBeRouted(); i++){
            ListGraph::Node n = intersectionGraph.addNode();
            nodeLabel[n] = i;
            nodeId[n] = intersectionGraph.id(n);
        }
        for (int i = 0; i < getNbDemandsToBeRouted(); i++){
            for (int j = i + 1; j < getNbDemandsToBeRouted(); j++){
                if(z[i][j].getVal() >= EPS){
                    int sourceLabel = i;
                    int targetLabel = j;
                    ListGraph::Node sourceNode = INVALID;
                    ListGraph::Node targetNode = INVALID;
                    for (ListGraph::NodeIt v(intersectionGraph); v != INVALID; ++v){
                        if(nodeLabel[v] == sourceLabel){
                            sourceNode = v;
                        }
                        if(nodeLabel[v] == targetLabel){
                            targetNode = v;
                        }
                    }
                    if (targetNode != INVALID && sourceNode != INVALID){
                        edgeCounter = edgeCounter + 1;
                        ListGraph::Edge e = intersectionGraph.addEdge(sourceNode, targetNode);
                        edgeId[e] = edgeCounter;
                        edgeLabel[e] = z[i][j].getId();
                        edgeWeight[e] = z[i][j].getVal();
                        //std::cout << "adding edge " << i + 1 << " " << j +1 << " com peso " << z[i][j].getVal() <<std::endl;
                    }
                }
            }
        }
        std::cout <<edgeCounter<<std::endl;
        for (ListGraph::EdgeIt e(intersectionGraph); e != INVALID; ++e){ 
            //std::cout << "avaliando edge " << nodeId[intersectionGraph.u(e)] << " " << nodeId[intersectionGraph.v(e)] <<std::endl;
            int u = nodeId[intersectionGraph.u(e)];
            int v = nodeId[intersectionGraph.v(e)];
            for (ListGraph::EdgeIt e2(intersectionGraph); e2 != INVALID; ++e2){ 
                //std::cout << "verificando intersecçao com edge " << nodeId[intersectionGraph.u(e2)] << " " << nodeId[intersectionGraph.v(e2)] <<std::endl;
                if(e!=e2){
                    int u2 = nodeId[intersectionGraph.u(e2)];
                    int v2 = nodeId[intersectionGraph.v(e2)];
                    if((u==u2) ||(u==v2) ||(v==u2) ||(v==v2)){
                        int missing1;
                        int missing2;
                        int common;
                        if(u==u2){
                            missing1 = v;
                            missing2 = v2;
                            common = u;
                        }else{                    
                            if(u==v2){
                                missing1 = v;
                                missing2 = u2;
                                common = u;
                            }else{
                                if(v==u2){
                                    missing1 = u;
                                    missing2 = v2;
                                    common = v;
                                }else{
                                    missing1 = u;
                                    missing2 = u2;
                                    common = v;
                                }
                            }
                        }
                        //std::cout << "intersection!! " <<std::endl;
                        for (ListGraph::EdgeIt e3(intersectionGraph); e3 != INVALID; ++e3){ 
                            //std::cout << "verificando intersecçao com edge " << nodeId[intersectionGraph.u(e3)] << " " << nodeId[intersectionGraph.v(e3)] <<std::endl;
                            if((e!=e2) && (e!=e3) && (e2!=e3)){
                                int u3 = nodeId[intersectionGraph.u(e3)];
                                int v3 = nodeId[intersectionGraph.v(e3)];
                                if(((missing1==u3) && (missing2==v3)) ||((missing1==v3) && (missing2==u3))){
                                    int a;
                                    int b;
                                    int c;
                                    if((missing1<missing2)&&(missing1<common)&&(missing2<common)){
                                        a =missing1;
                                        b =missing2;
                                        c =common;
                                    }else{
                                        if((missing1<missing2)&&(missing1<common)&&(common<missing2)){
                                            a =missing1;
                                            b =common;
                                            c =missing2;
                                        }else{
                                            if((missing2<missing1)&&(missing2<common)&&(missing1<common)){
                                                a =missing2;
                                                b =missing1;
                                                c =common;
                                            }else{
                                                if((missing2<missing1)&&(missing2<common)&&(common<missing1)){
                                                    a =missing2;
                                                    b =common;
                                                    c =missing1;
                                                }else{
                                                    if((common<missing1)&&(common<missing2)&&(missing1<missing2)){
                                                        a =common;
                                                        b =missing1;
                                                        c =missing2;
                                                    }else{
                                                        if((common<missing1)&&(common<missing2)&&(missing2<missing1)){
                                                            a =common;
                                                            b =missing2;
                                                            c =missing1;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    //std::cout << "Clique "<< a << " " << b <<" " << c <<std::endl;
                                    Expression expr;
                                    for (int s = 0; s < slicesTotal; s++){
                                        int sliceLimit;
                                        int sWk_1 = s + getToBeRouted_k(a).getLoadC()-1;
                                        int sWk_2 = s + getToBeRouted_k(b).getLoadC()-1;
                                        int sWk_3 = s + getToBeRouted_k(c).getLoadC()-1;
                                        if(s>=slicesC){
                                            sliceLimit = slicesC+slicesL;
                                            sWk_1 = s + getToBeRouted_k(a).getLoadL()-1;
                                            sWk_2 = s + getToBeRouted_k(b).getLoadL()-1;
                                            sWk_3 = s + getToBeRouted_k(c).getLoadL()-1;
                                        }else{
                                            sliceLimit = slicesC;
                                        }

                                        if (sWk_1 >= sliceLimit){
                                            sWk_1 = sliceLimit-1;
                                        }

                                        if (sWk_2 >= sliceLimit){
                                            sWk_2 = sliceLimit-1;
                                        }

                                        if (sWk_3 >= sliceLimit){
                                            sWk_3 = sliceLimit-1;
                                        }
                                        double c_sum = 0.0;
                                        for (int sa = s; sa <= sWk_1; sa++){
                                            //std::cout<<"c("<<a<<"'"<<sa<<") + ";
                                            c_sum = c_sum + y[sa][a].getVal();
                                            expr.addTerm(Term(y[sa][a], 1));
                                        }
                                        for (int sa = s; sa <= sWk_2; sa++){
                                            //std::cout<<"c("<<b<<"'"<<sa<<") + ";
                                            c_sum = c_sum + y[sa][b].getVal();
                                            expr.addTerm(Term(y[sa][b], 1));
                                        }
                                        for (int sa = s; sa <= sWk_3; sa++){
                                            //std::cout<<"c("<<c<<"'"<<sa<<") + ";
                                            c_sum = c_sum + y[sa][c].getVal();
                                            expr.addTerm(Term(y[sa][c], 1));
                                        }
                                        double k_sum = z[a][b].getVal() + z[a][c].getVal() + z[b][c].getVal();
                                        expr.addTerm(Term(z[a][b], 1));
                                        expr.addTerm(Term(z[a][c], 1));
                                        expr.addTerm(Term(z[b][c], 1));
                                        //std::cout<<"k"<<a<<","<<b<<" + "<<"k"<<a<<","<<c<<" + "<<"k"<<b<<","<<b<<std::endl;
                                        //std::cout<<k_sum << " + " << c_sum<<std::endl;
                                        double sum = k_sum + c_sum;
                                        if(sum>4){
                                            cuts.push_back(Constraint(0, expr, 4, "cut_tflow3"));
                                            //std::cout << "cut a " << std::endl;
                                            //std::cout << "Adding user cut: " << expr.to_string() << " <= 4" << std::endl;
                                        }
                                    }

                                }
                            }
                        }
                    }
                }
            }
        }
    }
    //std::cout << "Solving separation problem fractional..." << std::endl;
    int nbEdges = countEdges(compactGraph);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        for (int s = 0; s < slicesTotal; s++){
            Expression expr;
            int nbElementsC = 0;
            for (int k = 0; k < getNbDemandsToBeRouted(); ++k){
                int sliceLimit;
                int sWk_1 = s + getToBeRouted_k(k).getLoadC()-1;
                if(s>=slicesC){
                    sliceLimit = slicesC+slicesL;
                    sWk_1 = s + getToBeRouted_k(k).getLoadL()-1;

                }else{
                    sliceLimit = slicesC;
                }
                if (sWk_1 >= sliceLimit){
                    sWk_1 = sliceLimit-1;
                }
                if (x[edge][k].getVal() >= 1 - EPS || x[edge + nbEdges][k].getVal() >= 1 - EPS){
                    for (int sa = s; sa <= sWk_1; sa++){
                        if (y[sa][k].getVal() >= 1 - EPS){
                            expr.addTerm(Term(x[edge][k], 1));
                            expr.addTerm(Term(x[edge+nbEdges][k], 1));
                            expr.addTerm(Term(y[sa][k], 1));
                            nbElementsC = nbElementsC + 1;
                        }
                    }
                }
            }
            if(nbElementsC > 1){
                cuts.push_back(Constraint(0, expr, nbElementsC + 1, "cut_tflow"));
                std::cout << "cut b " << std::endl;
                //std::cout << "Adding user cut: " << expr.to_string() << " <= "  << nbElementsC + 1<< std::endl;
            }
        }
    }
    return cuts;
}


std::vector<Constraint> TFlowForm::solveSeparationProblemInt(const std::vector<double> &solution, const int threadNo){
    /*
    //std::cout << "Solving separation problem integer..." << std::endl; 
    setVariableValues(solution);
    std::vector<Constraint> cuts;
    int nbEdges = countEdges(compactGraph);
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeLabel(e);
        for (int s = 0; s < slicesBand*bands; s++){
            int sliceLimit;
            if(s>=slicesBand){
                sliceLimit = slicesBand*bands;
            }else{
                sliceLimit = slicesBand;
            }
            Expression expr;
            int nbElementsC = 0;
            for (int k = 0; k < getNbDemandsToBeRouted(); ++k){
                int sWk_1 = s + getToBeRouted_k(k).getLoad()-1;
                if (sWk_1 >= sliceLimit){
                    sWk_1 = sliceLimit-1;
                }
                if (x[edge][k].getVal() >= 1 - EPS || x[edge + nbEdges][k].getVal() >= 1 - EPS){
                    for (int sa = s; sa <= sWk_1; sa++){
                        if (y[sa][k].getVal() >= 1 - EPS){
                            expr.addTerm(Term(x[edge][k], 1));
                            expr.addTerm(Term(x[edge+nbEdges][k], 1));
                            expr.addTerm(Term(y[sa][k], 1));
                            nbElementsC = nbElementsC + 1;
                        }
                    }
                }
            }
            if(nbElementsC > 1){
                cuts.push_back(Constraint(0, expr, nbElementsC + 1, "name_to_do"));
                //std::cout << "Adding lazy constraint: " << expr.to_string() << " <= "  << nbElementsC + 1<< std::endl;
                break;
            }
        }
    }
    return cuts;
    */
}

/****************************************************************************************/
/*										Display											*/
/****************************************************************************************/

/* Displays the value of each variable in the obtained solution. */
void TFlowForm::displayVariableValues(){
    for (int k = 0; k < getNbDemandsToBeRouted(); k++){
        for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
            int edge = getCompactEdgeLabel(e);
            std::cout << x[edge][k].getName() << " = " << x[edge][k].getVal() << "   ";
        }
        std::cout << std::endl;
    }
    for (int k = 0; k < getNbDemandsToBeRouted(); k++){
        for (int s = 0; s < slicesTotal; s++){
            std::cout << y[s][k].getName() << " = " << y[s][k].getVal() << "   ";
        }
        std::cout << std::endl;
    }
    if((NonOverlappingType == 3) ||(NonOverlappingType == 4) ){
        for (int a = 0; a < getNbDemandsToBeRouted(); a++){
            for (int b = a + 1; b < getNbDemandsToBeRouted(); b++){
            std::cout << z[a][b].getName() << " = " << z[a][b].getVal() << "   ";
        }
        std::cout << std::endl;
        }
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
    if(nbBands>1){                              // IF OF
        if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_LLB){
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                int edge = getCompactEdgeLabel(e);
                std::cout << l[edge].getName() << " = " << l[edge].getVal() << "   ";
                std::cout << std::endl;
                for (int k = 0; k < getNbDemandsToBeRouted(); k++){  
                    std::cout << l2[edge][k].getName() << " = " << l2[edge][k].getVal() << "   ";
                }
                std::cout << std::endl;
            }
        }
    }
    std::cout <<  std::endl;
}

std::string TFlowForm::displayDimensions(){
    return "|K| = " + std::to_string(getNbDemandsToBeRouted()) + ", |S| = " + std::to_string(slicesTotal) + ".";
}

/****************************************************************************************/
/*										Destructor										*/
/****************************************************************************************/

/* Destructor. Clears the vectors of demands and links. */
TFlowForm::~TFlowForm(){
    x.clear();
    y.clear();
    if((NonOverlappingType == 3) ||(NonOverlappingType == 4) ){
        z.clear();
    }
    maxSlicePerLink.clear();
}