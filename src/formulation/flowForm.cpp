#include "flowForm.h"


/* Constructor. Builds the Online RSA mixed-integer program and solves it using a defined solver (CPLEX or CBC). */
FlowForm::FlowForm(const Instance &inst) : AbstractFormulation(inst){

    ClockTime time(ClockTime::getTimeNow());
    ClockTime time2(ClockTime::getTimeNow());
    std::cout << "--- Flow formulation has been chosen. " << displayDimensions() << " ---" << std::endl;

    slicesC = instance.getTabEdge()[0].getNbSlicesC();
    slicesL = instance.getTabEdge()[0].getNbSlicesL();
    slicesTotal = slicesC + slicesL;

    this->setVariables();
    varImpleTime = time.getTimeInSecFromStart() ;
    time.setStart(ClockTime::getTimeNow());
    this->setConstraints();
    constImpleTime = time.getTimeInSecFromStart() ;
    time.setStart(ClockTime::getTimeNow());
    //this->setCutPool();
    cutImpleTime = time.getTimeInSecFromStart() ;
    time.setStart(ClockTime::getTimeNow());
    this->setObjectives();
    objImpleTime = time.getTimeInSecFromStart() ;
    //this->setWarmValues();
    std::cout << "--- Flow formulation has been defined ---" << std::endl;
    totalImpleTime = time2.getTimeInSecFromStart() ;
    //std::cout << "Time: " << time.getTimeInSecFromStart() << std::endl;
}

std::string FlowForm::displayDimensions(){
    return "|K| = " + std::to_string(getNbDemandsToBeRouted()) + ", |S| = " + std::to_string(getNbSlicesGlobalLimit()) + ".";
}

/****************************************************************************************/
/*										Variables    									*/
/****************************************************************************************/

/* Define variables x[d][a] for every arc a in the extedend graph #d. */
void FlowForm::setVariables(){
    this->setFlowVariables();

    const std::vector<Input::ObjectiveMetric> & chosenObjectives = instance.getInput().getChosenObj();
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_SLUS){
        this->setMaxUsedSlicePerEdgeVariables();
    }
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_NLUS){
    this->setMaxUsedSliceOverallVariable();
    }

    if(nbBands>1){                          // IF OF
        this->setMultibandVariables();
    }
}

void FlowForm::setFlowVariables(){
    x.resize(getNbDemandsToBeRouted());
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){ 
        x[d].resize(countArcs(*vecGraph[d]));  
        for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
            int arc = getArcIndex(a, d); 
            int label = getArcLabel(a, d); 
            int labelSource = getNodeLabel((*vecGraph[d]).source(a), d);
            int labelTarget = getNodeLabel((*vecGraph[d]).target(a), d);
            int slice = getArcSlice(a, d);
            std::ostringstream varName;
            varName << "x";
            varName << "(" + std::to_string(getToBeRouted_k(d).getId() + 1) + "," ;
            varName <<  std::to_string(labelSource + 1) + "," + std::to_string(labelTarget + 1) + ",";
            varName <<  std::to_string(slice + 1) + ")";
            int upperBound = 1;
            if (instance.hasEnoughSpace(label, slice, getToBeRouted_k(d)) == false){
                upperBound = 0;
                std::cout << "STILL REMOVING VARIABLES IN FORMULATION. \n" ;
            }
            //int varId = getNbVar();
            int varId = getVarId(a,d);
            /* When solving with subgradient methods we do not change the variable to continuous. */
            if(instance.getInput().isRelaxed() && (instance.getInput().getChosenNodeMethod() == Input::NODE_METHOD_LINEAR_RELAX)) {
                x[d][arc] = Variable(varId, 0, upperBound, Variable::TYPE_REAL, 0, varName.str());
            }
            else{
                x[d][arc] = Variable(varId, 0, upperBound, Variable::TYPE_BOOLEAN, 0, varName.str());
            }
            incNbVar();
            // std::cout << "Created variable: " << var[d][arc].getName() << std::endl;
        }
    }
    std::cout << "Flow variables have been defined..." << std::endl;

}

void FlowForm::setMaxUsedSlicePerEdgeVariables(){
    maxSlicePerLink.resize(instance.getNbEdges());
    for (int i = 0; i < instance.getNbEdges(); i++){
        std::string varName = "maxSlice(" + std::to_string(instance.getPhysicalLinkFromIndex(i).getId() + 1) + ")";
        int lowerBound = std::max(0, instance.getPhysicalLinkFromIndex(i).getMaxUsedSlicePosition());
        //int upperBound = getNbSlicesLimitFromEdge(i);
        int upperBound = auxNbSlicesLimitFromEdge[i];
        int varId = getNbVar();
        maxSlicePerLink[i] = Variable(varId, lowerBound, upperBound, Variable::TYPE_REAL, 0, varName);
        incNbVar();
    }
    std::cout << "Max slice variables have been defined..." << std::endl;
}

void FlowForm::setMaxUsedSliceOverallVariable(){
    std::string varName = "maxSliceOverall";
    int lowerBound = std::max(0,instance.getMaxUsedSlicePosition());
    int upperBound = getNbSlicesGlobalLimit();
    if(nbBands>1){  
        upperBound =slicesTotal;
    }
    //int upperBound = auxNbSlicesGlobalLimit-1;
    int varId = getNbVar();
    maxSliceOverall = Variable(varId, lowerBound, upperBound, Variable::TYPE_REAL, 0, varName);
    incNbVar();
    std::cout << "Max slice overall variable has been defined..." << std::endl;
}

void FlowForm::setMultibandVariables(){
    // C Band routing variables
    int nbEdges = countEdges(compactGraph);

    l.resize(nbEdges);
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
    }
    std::cout << "Multiband variables has been defined..." << std::endl;
}

VarArray FlowForm::getVariables(){
    VarArray vec;
    vec.resize(getNbVar());
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){ 
        for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
            int arc = getArcIndex(a, d);
            int pos = x[d][arc].getId();
            vec[pos] = x[d][arc];
        }
    }


    const std::vector<Input::ObjectiveMetric> & chosenObjectives = instance.getInput().getChosenObj();
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_SLUS){
        for (int i = 0; i < instance.getNbEdges(); i++){
            int pos = maxSlicePerLink[i].getId();
            vec[pos] = maxSlicePerLink[i];
        }
    }
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_NLUS){
        int pos = maxSliceOverall.getId();
        vec[pos] = maxSliceOverall;
    }
    if(nbBands>1){                              // IF OF
        for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
            int edge = getCompactEdgeLabel(e);
            int pos = l[edge].getId();
            vec[pos] = l[edge];
        }  
    }
    return vec;
}

void FlowForm::setVariableValues(const std::vector<double> &vals){
    
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){ 
        for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
            int arc = getArcIndex(a, d);
            int pos = x[d][arc].getId();
            double newValue = vals[pos];
            x[d][arc].setVal(newValue);
        }
    }
    if(instance.getInput().getChosenNodeMethod()!= Input::NODE_METHOD_LINEAR_RELAX){
        if(instance.getInput().isObj8(0)){
            int pos = maxSliceOverall.getId();
            double newValue = vals[pos];
            maxSliceOverall.setVal(newValue);
        }

    }else{
        const std::vector<Input::ObjectiveMetric> & chosenObjectives = instance.getInput().getChosenObj();
        if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_SLUS){    
            // Max slice variables
            for (int i = 0; i < instance.getNbEdges(); i++){
                int pos = maxSlicePerLink[i].getId();
                double newValue = vals[pos];
                maxSlicePerLink[i].setVal(newValue);
            }
        }
        if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_NLUS){
            int pos = maxSliceOverall.getId();
            double newValue = vals[pos];
            maxSliceOverall.setVal(newValue);
        }
        if(nbBands>1){
            // multiband variables
            for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
                int edge = getCompactEdgeLabel(e);
                int pos = l[edge].getId();
                double newValue = vals[pos];
                
                l[edge].setVal(newValue);
            } 
        }
    }
}

void FlowForm::setWarmValues(){
    
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        int demand = d;
        int lastSlot = feasibleSolutionLastSlotDemand[d];
        for (int node = 0; node < feasibleSolutionNodesByDemand[d].size()-1; node++){
            int origin = feasibleSolutionNodesByDemand[d][node];
            int destination = feasibleSolutionNodesByDemand[d][node+1];
            //verifying VAR
            for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                int labelSource = getNodeLabel((*vecGraph[d]).source(a), d)+1 ;
                int labelTarget = getNodeLabel((*vecGraph[d]).target(a), d)+1 ;
                int slice = getArcSlice(a, d)+1;
                if((lastSlot == slice)&&(origin == labelSource)&&(destination == labelTarget)){
                    //std::cout<<"f(" << d+1<<","<< origin<<","<< destination<<","<< lastSlot<<")"<<std::endl;
                    //std::cout<<"f(" << d+1<<","<< labelSource<<","<< labelTarget<<","<< slice<<")"<<std::endl;
                    int arc = getArcIndex(a, d); 
                    x[d][arc].setWarmstartValue(1.0);
                }
            }
        }
    }
    

    const std::vector<Input::ObjectiveMetric> & chosenObjectives = instance.getInput().getChosenObj();
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_NLUS){
        int max = 1;
        for (int i = 0; i < feasibleSolutionLastSlotDemand.size(); i++){
            if(feasibleSolutionLastSlotDemand[i]>max){
                max=feasibleSolutionLastSlotDemand[i];
            }
        }
        //std::cout<<"p = " << max<<std::endl;                
        maxSliceOverall.setWarmstartValue(max);
    }
    
}


/****************************************************************************************/
/*									Objective Function    								*/
/****************************************************************************************/

/* Set the objective Function */
void FlowForm::setObjectives(){
    const std::vector<Input::ObjectiveMetric> & chosenObjectives = instance.getInput().getChosenObj();
    objectiveSet.resize(chosenObjectives.size());
    for (unsigned int i = 0; i < chosenObjectives.size(); i++){
        Expression myObjective = this->getObjFunctionFromMetric(chosenObjectives[i]);
        objectiveSet[i].setExpression(myObjective);
        objectiveSet[i].setDirection(ObjectiveFunction::DIRECTION_MIN);
        if ((chosenObjectives[i] == Input::OBJECTIVE_METRIC_ADS) || (chosenObjectives[i] == Input::OBJECTIVE_METRIC_DCB)){
            objectiveSet[i].setDirection(ObjectiveFunction::DIRECTION_MAX);
        } 
        objectiveSet[i].setName(instance.getInput().getObjName(chosenObjectives[i]));
        objectiveSet[i].setId(chosenObjectives[i]);
        std::cout << "Objective " << objectiveSet[i].getName() << " has been defined." << std::endl;
    }
}

/* Returns the objective function expression. */
Expression FlowForm::getObjFunctionFromMetric(Input::ObjectiveMetric chosenObjective){
    Expression obj;
    switch (chosenObjective)
    {
    case Input::OBJECTIVE_METRIC_0:
        {
        break;
        }
    case Input::OBJECTIVE_METRIC_1:
        {
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                int arc = getArcIndex(a, d);
                double coeff = getCoeffObj1(a, d);
                Term term(x[d][arc], coeff);
                obj.addTerm2(term);
            }
        }
        break;
        }
    case Input::OBJECTIVE_METRIC_SLUS:
        {
        for (int i = 0; i < instance.getNbEdges(); i++){
            Term term(maxSlicePerLink[i], 1);
            obj.addTerm2(term);
        }
        break;
        }
    case Input::OBJECTIVE_METRIC_SULD:
        {
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                int arc = getArcIndex(a, d);
                double coeff = getCoeffObjSULD(a, d);
                Term term(x[d][arc], coeff);
                obj.addTerm2(term);
            }
        }
        break;
        }
    case Input::OBJECTIVE_METRIC_TUS:
        {
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                int arc = getArcIndex(a, d);
                double coeff = getCoeffObjTUS(a, d);
                Term term(x[d][arc], coeff);
                obj.addTerm2(term);
            }
        }
        break;
        }
    case Input::OBJECTIVE_METRIC_TRL:
        {
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                int arc = getArcIndex(a, d);
                double coeff = getCoeffObjTRL(a, d);
                Term term(x[d][arc], coeff);
                obj.addTerm2(term);
            }
        }
        break;
        }
    case Input::OBJECTIVE_METRIC_TASE:
        {
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                int arc = getArcIndex(a, d);
                double coeff = getCoeffObjTASE(a, d);
                Term term(x[d][arc], coeff);
                obj.addTerm2(term);
            }
        }
        break;
        }
    case Input::OBJECTIVE_METRIC_NLUS:
        {
        Term term(maxSliceOverall, 1);
        obj.addTerm2(term);
        break;
        }

    case Input::OBJECTIVE_METRIC_TOS:
        {
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                int arc = getArcIndex(a, d);
                double coeff = getCoeffObjTOS(a, d);
                Term term(x[d][arc], coeff);
                obj.addTerm2(term);
            }
        }
        break;
        }
    case Input::OBJECTIVE_METRIC_ADS:
        {
        for (int k = 0; k < getNbDemandsToBeRouted(); k++){                
            for(IterableIntMap< ListDigraph, ListDigraph::Node >::ItemIt v((*mapItNodeLabel[k]),getToBeRouted_k(k).getSource()); v != INVALID; ++v){ 
                for (ListDigraph::OutArcIt a((*vecGraph[k]), v); a != INVALID; ++a){
                    int arc = getArcIndex(a, k); 
                    int slice = getArcSlice(a, k);
                    if (slice < slicesC){
                        //Term term(x[k][arc], -getToBeRouted_k(k).getLoadC());
                        Term term(x[k][arc],1);
                        obj.addTerm(term);
                    }
                    if(nbBands>1){
                        if (slice >= slicesC){
                            //Term term2(x[k][arc], -getToBeRouted_k(k).getLoadL());
                            Term term2(x[k][arc], 1);
                            obj.addTerm(term2);
                        }
                    }
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
                for(IterableIntMap< ListDigraph, ListDigraph::Node >::ItemIt v((*mapItNodeLabel[k]),getToBeRouted_k(k).getSource()); v != INVALID; ++v){ 
                    for (ListDigraph::OutArcIt a((*vecGraph[k]), v); a != INVALID; ++a){
                        int arc = getArcIndex(a, k); 
                        int slice = getArcSlice(a, k);
                        if (slice < slicesC){
                            Term term(x[k][arc], 1);
                            obj.addTerm(term);
                        }
                    }
                }
            }
            break;
        }
    default:
        {
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

void FlowForm::setConstraints(){
    this->setSourceConstraints();
    this->setFlowConservationConstraints();
    this->setTargetConstraints();
    
    //this->setCDConstraints();
    if (this->getInstance().getInput().isMaxCDEnabled() == true){
        if (this->getInstance().getInput().areReinforcementsEnabled() == true){
            this->setStrongCDConstraints();
        }
        else{
            this->setCDConstraints();
        }
    }
    if (this->getInstance().getInput().isMinOSNREnabled() == true){
        if (this->getInstance().getInput().areReinforcementsEnabled() == true){
            this->setStrongOSNRConstraints();
        }else{
            this->setOSNRConstraints();
        }
    }
    this->setNonOverlappingConstraints();   

    const std::vector<Input::ObjectiveMetric> & chosenObjectives = instance.getInput().getChosenObj();
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_SLUS){
        this->setMaxUsedSlicePerLinkConstraints(); 
    }
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_NLUS){
        this->setMaxUsedSliceOverallConstraints();  
    }
    if(nbBands>1){                              // IF OF
        if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_LLB){
            this->setMultibandConstraints();
        }
        this->setThresholdConstraints();
    }
       
    nbConstraint = constraintSet.size();
    //NO L BAND PREPRO
    if(nbBands>1){                              // IF OF
        this->setForcedMonobandConstraints();
    }
} 

/* Defines Source constraints. At most one arc leaves each node and exactly one arc leaves the source. */
void FlowForm::setSourceConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){  
        for (int label = 0; label < instance.getNbNodes(); label++){
            const Constraint & sourceConstraint = getSourceConstraint_d_n(getToBeRouted_k(d), d, label);
            constraintSet.push_back(sourceConstraint);
        }
    }
    std::cout << "Source constraints have been defined..." << std::endl;
}

/* Returns the source constraint associated with a demand and a node. */
/* When we use it in the lagrangian, we multiply the constraint by -1 so it will be >= and it will be 
* consistent with the minimization problem. Source constraints whre v != source and v != destination. */
Constraint FlowForm::getSourceConstraint_d_n(const Demand & demand, int d, int nodeLabel){
    Expression exp;

    double upperBound = 1; double lowerBound = 0;
    Input::NodeMethod nodeMethod = instance.getInput().getChosenNodeMethod();
    if((nodeMethod != Input::NODE_METHOD_LINEAR_RELAX) && (nodeLabel != demand.getSource()) && (nodeLabel != demand.getTarget())){
        lowerBound = -1; upperBound = 0;
    }

    for(IterableIntMap< ListDigraph, ListDigraph::Node >::ItemIt v((*mapItNodeLabel[d]),nodeLabel); v != INVALID; ++v){ 
        for (ListDigraph::OutArcIt a((*vecGraph[d]), v); a != INVALID; ++a){
            if((nodeMethod != Input::NODE_METHOD_LINEAR_RELAX) && (nodeLabel != demand.getSource()) && (nodeLabel != demand.getTarget())){
                int arc = getArcIndex(a, d); 
                Term term(x[d][arc], -1);
                exp.addTerm(term);
            }else{
                int arc = getArcIndex(a, d); 
                Term term(x[d][arc], 1);
                exp.addTerm(term);
            }
        }
    }
    std::ostringstream constraintName;
    constraintName << "Source_" << nodeLabel+1 << "_" << demand.getId()+1;
    if (nodeLabel == demand.getSource()){
        lowerBound = 1;
    }
    if (nodeLabel == demand.getTarget()){
        upperBound = 0;
    }
    const std::vector<Input::ObjectiveMetric> & chosenObjectives = instance.getInput().getChosenObj();
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_ADS){
        lowerBound = 0;
    }
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

/* Defines Flow Conservation constraints. If an arc enters a node, then an arc must leave it. */
void FlowForm::setFlowConservationConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){   
        for (ListDigraph::NodeIt v(*vecGraph[d]); v != INVALID; ++v){
            int label = getNodeLabel(v, d);
            if( (label != getToBeRouted_k(d).getSource()) && (label != getToBeRouted_k(d).getTarget()) ){
                const Constraint & flow = getFlowConservationConstraint_i_d(v, getToBeRouted_k(d), d);
                constraintSet.push_back(flow);
            }
        }
    }
    std::cout << "Flow conservation constraints have been defined..." << std::endl;
}

/* Returns the flow conservation constraint associated with a demand and a node. */
Constraint FlowForm::getFlowConservationConstraint_i_d(ListDigraph::Node &v, const Demand & demand, int d){
    Expression exp;
    int rhs = 0;
    for (ListDigraph::OutArcIt a((*vecGraph[d]), v); a != INVALID; ++a){
        int arc = getArcIndex(a, d);
        Term term(x[d][arc], 1);
        exp.addTerm(term);
    }
    for (ListDigraph::InArcIt a((*vecGraph[d]), v); a != INVALID; ++a){
        int arc = getArcIndex(a, d); 
        Term term(x[d][arc], -1);
        exp.addTerm(term);
    }
    std::ostringstream constraintName;
    int label = getNodeLabel(v, d);
    int slice = getNodeSlice(v, d);
    constraintName << "Flow_" << label+1 << "_" << slice+1 << "_" << demand.getId()+1;
    Constraint constraint(rhs, exp, rhs, constraintName.str());
    return constraint;
}

/* Defines Target constraints. Exactly one arc enters the target. */
void FlowForm::setTargetConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){   
        const Constraint & targetConstraint = getTargetConstraint_d(getToBeRouted_k(d), d);
        constraintSet.push_back(targetConstraint);
    }
    std::cout << "Target constraints have been defined..." << std::endl;
}

/* Returns the target constraint associated with a demand. */
Constraint FlowForm::getTargetConstraint_d(const Demand & demand, int d){
    Expression exp;
    int rhs = 1;
    int lhs = 1;

    for(IterableIntMap< ListDigraph, ListDigraph::Node >::ItemIt v((*mapItNodeLabel[d]),demand.getTarget()); v != INVALID; ++v){ 
        for (ListDigraph::InArcIt a((*vecGraph[d]), v); a != INVALID; ++a){
            int arc = getArcIndex(a, d); 
            Term term(x[d][arc], 1);
            exp.addTerm(term);
        }
    }
    const std::vector<Input::ObjectiveMetric> & chosenObjectives = instance.getInput().getChosenObj();
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_ADS){
        lhs = 0;
    }
    std::ostringstream constraintName;
    constraintName << "Target_" << demand.getId()+1 ;
    Constraint constraint(lhs, exp, rhs, constraintName.str());
    return constraint;
}

/* Defines the reinforced max reach constraints. */
void FlowForm::setStrongCDConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){   
        //for (int s = 0; s < getNbSlicesGlobalLimit(); s++){  
        int sliceLimit = auxNbSlicesGlobalLimit;
        if(nbBands>1){
            sliceLimit = slicesTotal;
        }
        for (int s = 0; s < sliceLimit; s++){  
            const Constraint & strongCDConstraint = getStrongCDConstraint(getToBeRouted_k(d), d, s);
            constraintSet.push_back(strongCDConstraint);
        }
    }
    std::cout << "Strong CD constraints have been defined..." << std::endl;
}

/* Returns the strong max reach constraint associated with a demand and a slice. */
Constraint FlowForm::getStrongCDConstraint(const Demand &demand, int d, int s){
    Expression exp;
    int source = demand.getSource();

    for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
        int arc = getArcIndex(a, d); 
        if (getArcSlice(a, d) == s){
            double coeff = getArcDispersionC(a, d);
            if(s>=slicesC){
                coeff = getArcDispersionL(a, d);
            }
            Term term(x[d][arc], coeff);
            exp.addTerm(term);
        }
    }
    for (ListDigraph::NodeIt v(*vecGraph[d]); v != INVALID; ++v){
        if (getNodeLabel(v, d) == source){
            for (ListDigraph::OutArcIt a((*vecGraph[d]), v); a != INVALID; ++a){
                if (getArcSlice(a, d) == s){
                    int arc = getArcIndex(a, d);
                    int coeff2 = -demand.getmaxCDC();
                    if(s>=slicesC){
                        coeff2 = -demand.getmaxCDL();
                    } 
                    Term term(x[d][arc], coeff2);
                    exp.addTerm(term);
                }
            }
        }
    }
    std::ostringstream constraintName;
    constraintName << "StrongCD_" << demand.getId()+1 << "_" << s+1;
    Constraint constraint(exp.getTrivialLb(), exp, 0, constraintName.str());
    return constraint;
}

/* Defines CD constraints. Demands must be routed within a CD limit. */
void FlowForm::setCDConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){   
        const Constraint & cdConstraintC = getCDConstraintC(getToBeRouted_k(d), d);
        constraintSet.push_back(cdConstraintC);
        if (getInstance().getInput().getNbBands() >= 2) {
            const Constraint & cdConstraintL = getCDConstraintL(getToBeRouted_k(d), d);
            constraintSet.push_back(cdConstraintL);
        } 
    }
    std::cout << "CD constraints have been defined..." << std::endl;
}

/* Returns the CD constraint associated with a demand. */
/* When we use it in the lagrangian, we multiply the constraint by -1 so it will be >= and it will be 
* consistent with the minimization problem. */
Constraint FlowForm::getCDConstraintC(const Demand &demand, int d){
    Expression exp;
    double rhs; double rls;

    rhs = demand.getmaxCDC(); rls = 0;
    
    for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
        int arc = getArcIndex(a, d); 
        double coeff = getArcDispersionC(a, d);
        Term term(x[d][arc], coeff);
        if (getArcSlice(a,d) < slicesC){
            exp.addTerm(term);
        }

    }
    std::ostringstream constraintName;
    constraintName << "CD_" << demand.getId()+1;
    Constraint constraint(rls, exp, rhs, constraintName.str());
    return constraint;
}

Constraint FlowForm::getCDConstraintL(const Demand &demand, int d){
    Expression exp;
    double rhs; double rls;

    rhs = demand.getmaxCDL(); rls = 0;
    
    for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
        int arc = getArcIndex(a, d); 
        double coeff = getArcDispersionL(a, d);
        Term term(x[d][arc], coeff);
        if (getArcSlice(a,d) >= slicesC){
            exp.addTerm(term);
        }

    }
    std::ostringstream constraintName;
    constraintName << "CD_" << demand.getId()+1;
    Constraint constraint(rls, exp, rhs, constraintName.str());
    return constraint;
}


/* Defines OSNR constraints. Demands must be routed within a OSNR limit. */
void FlowForm::setStrongOSNRConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){   
        int sliceLimit = auxNbSlicesGlobalLimit;
        if(nbBands>1){
            sliceLimit = slicesTotal;
        }
        for (int s = 0; s < sliceLimit; s++){   
            const Constraint & strongOSNRConstraint = getStrongOSNRConstraint(getToBeRouted_k(d), d,s);
            constraintSet.push_back(strongOSNRConstraint);
        }
 
    }
    std::cout << "Strong OSNR constraints have been defined..." << std::endl;
}


/* Returns the OSNR constraint associated with a demand. */
Constraint FlowForm::getStrongOSNRConstraint(const Demand &demand, int d, int s){
    int source = demand.getSource();
    Expression exp;
    double rhs;
    
    double osnrLimdb = demand.getOsnrLimitC();
    if(s>=slicesC){
        osnrLimdb =demand.getOsnrLimitL();
    }
    
    double osnrLim = pow(10,osnrLimdb/10);
    double pch = demand.getPchC();

    double roundingFactor = pow(10,8);
    
    rhs = pch/osnrLim ;//- instance.getPaseNodeC() ; //RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
    if(s>=slicesC){
        rhs = pch/osnrLim; // - instance.getPaseNodeL() ; //RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
    }
    //std::cout << rhs << std::endl;
    rhs = ceil(rhs * roundingFactor*100)/100 ; //ROUNDING
    //std::cout << rhs << std::endl;
    for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
        if (getArcSlice(a, d) == s){
            //First term
            int arc = getArcIndex(a, d); 
            double coeff = getArcNoiseC(a, d) * roundingFactor;
            if(s>=slicesC){
                coeff = getArcNoiseL(a, d) * roundingFactor;
            }
            coeff = ceil(coeff*100)/100; //ROUNDING
            //std::cout  << "-Pase line arredondado" << coeff << " Pase line" << getArcPaseLine(a, d) << std::endl;
            Term term(x[d][arc], coeff);
            exp.addTerm(term);
        }
    }
    for (ListDigraph::NodeIt v(*vecGraph[d]); v != INVALID; ++v){
        if (getNodeLabel(v, d) == source){
            for (ListDigraph::OutArcIt a((*vecGraph[d]), v); a != INVALID; ++a){
                if (getArcSlice(a, d) == s){
                    int arc = getArcIndex(a, d);
                    int coeff2 = -rhs;
                    Term term(x[d][arc], coeff2);
                    exp.addTerm(term);
                }
            }
        }
    }


    std::ostringstream constraintName;
    constraintName << "StrongOSNR_" << demand.getId()+1 << "_" << s+1;
    Constraint constraint(exp.getTrivialLb(), exp, 0, constraintName.str());
    //std::cout << "For demand " << d<< std::endl;
    //constraint.display();
    return constraint;
}

/* Defines OSNR constraints. Demands must be routed within a OSNR limit. */
void FlowForm::setOSNRConstraints(){
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
Constraint FlowForm::getOSNRCConstraint(const Demand &demand, int d){
    Expression exp;
    double rhs; double rls;
    
    double osnrLimdb = demand.getOsnrLimitC();
    double osnrLim = pow(10,osnrLimdb/10);
    double pch = demand.getPchC();

    double roundingFactor = pow(10,8);
    
    rhs = pch/osnrLim; //- instance.getPaseNodeC() ; //RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
    //std::cout << rhs << std::endl;
    rhs = ceil(rhs * roundingFactor*100)/100 ; //ROUNDING
    //std::cout << rhs << std::endl;
    rls = 0;
    for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
        //First term
        int arc = getArcIndex(a, d); 
        double coeff = getArcNoiseC(a, d) * roundingFactor;
        coeff = ceil(coeff*100)/100; //ROUNDING
        //std::cout  << "-Pase line arredondado" << coeff << " Pase line" << getArcPaseLine(a, d) << std::endl;
        Term term(x[d][arc], coeff);
        if (getArcSlice(a,d) < slicesC){
            exp.addTerm(term);
        }
            
    }
    std::ostringstream constraintName;
    constraintName << "OSNRC_" << demand.getId()+1;
    Constraint constraint(rls, exp, rhs, constraintName.str());
    //std::cout << "For demand " << d<< std::endl;
    //constraint.display();
    return constraint;
}

/* Returns the OSNR constraint associated with a demand. */
Constraint FlowForm::getOSNRLConstraint(const Demand &demand, int d){
    Expression exp;
    double rhs; double rls;
    
    double osnrLimdb = demand.getOsnrLimitL();
    double osnrLim = pow(10,osnrLimdb/10);
    double pch = demand.getPchL();

    double roundingFactor = pow(10,8);
    
    rhs = pch/osnrLim ; //- instance.getPaseNodeL() ; //RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
    //std::cout << rhs << std::endl;
    rhs = ceil(rhs * roundingFactor*100)/100 ; //ROUNDING
    //std::cout << rhs << std::endl;
    rls = 0;
    for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
        //First term
        int arc = getArcIndex(a, d); 
        double coeff = getArcNoiseL(a, d) * roundingFactor;
        coeff = ceil(coeff*100)/100; //ROUNDING
        //std::cout  << "-Pase line arredondado" << coeff << " Pase line" << getArcPaseLine(a, d) << std::endl;
        Term term(x[d][arc], coeff);
        if (getArcSlice(a,d) >= slicesC){
            exp.addTerm(term);
        }
            
    }
    std::ostringstream constraintName;
    constraintName << "OSNRL_" << demand.getId()+1;
    Constraint constraint(rls, exp, rhs, constraintName.str());
    //std::cout << "For demand " << d<< std::endl;
    //constraint.display();
    return constraint;
}


/* Defines the second set of Improved Non-Overlapping constraints. */
void FlowForm::setNonOverlappingConstraints(){
    int counter = 0;
    for (int i = 0; i < instance.getNbEdges(); i++){
        //int sliceLimit = getNbSlicesLimitFromEdge(i);
        int sliceLimit = slicesTotal;
        for (int s = 0; s < sliceLimit; s++){
            const Constraint & nonOverlap = getNonOverlappingConstraint(instance.getPhysicalLinkFromIndex(i).getId(), s);
            constraintSet.push_back(nonOverlap);
            counter = counter + 1;
        }
    }
    std::cout << "Non-Overlapping constraints has been defined..." << std::endl;
    std::cout << "Constraints = " << counter << std::endl;
}

/* Returns the non-overlapping constraint associated with an edge and a slice */
/* When we use it in the lagrangian, we multiply the constraint by -1 so it will be >= and it will be 
* consistent with the minimization problem. */
Constraint FlowForm::getNonOverlappingConstraint(int linkLabel, int slice){
    Input::NodeMethod nodeMethod = instance.getInput().getChosenNodeMethod();
	Expression exp;
    int rhs; int rls;
    if(nodeMethod != Input::NODE_METHOD_LINEAR_RELAX){
        rhs = 0; rls = -1;
    }else{
        rhs = 1; rls = 0;
    }

    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        int demandLoad = getToBeRouted_k(d).getLoadC();
        if (slice >= slicesC){
            demandLoad = getToBeRouted_k(d).getLoadL();
        }
        for(IterableIntMap< ListDigraph, ListDigraph::Arc >::ItemIt a((*mapItArcLabel[d]),linkLabel); a != INVALID; ++a){      
            if((getArcSlice(a, d) >= slice)  && (getArcSlice(a, d) <= slice + demandLoad - 1)){    
                if(nodeMethod != Input::NODE_METHOD_LINEAR_RELAX){
                    int index = getArcIndex(a, d);
                    Term term(x[d][index], -1);
                    exp.addTerm(term);
                }else{
                    int index = getArcIndex(a, d);
                    Term term(x[d][index], 1);
                    exp.addTerm(term);
                }
            }
        }
    }
    
    std::ostringstream constraintName;
    constraintName << "NonOverlap_" << linkLabel+1 << "_" << slice+1;
    Constraint constraint(rls, exp, rhs, constraintName.str());
    return constraint;
}

/****************************************************************************************/
/*						Objective function related constraints    						*/
/****************************************************************************************/

/* Defines the Link's Max Used Slice Position constraints. The max used slice position on each link must be greater than every slice position used in the link. */
void FlowForm::setMaxUsedSlicePerLinkConstraints(){
    for (int i = 0; i < instance.getNbEdges(); i++){
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            Constraint maxUsedSlicePerLinkConst = getMaxUsedSlicePerLinkConstraints(i, d);
            constraintSet.push_back(maxUsedSlicePerLinkConst);
        }
    }
    std::cout << "Max Used Slice Per Link constraints have been defined..." << std::endl;
}

Constraint FlowForm::getMaxUsedSlicePerLinkConstraints(int linkIndex, int d){
    Expression exp;
    int rhs = 0;
    int linkLabel = instance.getPhysicalLinkFromIndex(linkIndex).getId();
    for(IterableIntMap< ListDigraph, ListDigraph::Arc >::ItemIt a((*mapItArcLabel[d]),linkLabel); a != INVALID; ++a){ 
        int index = getArcIndex(a, d);
        int slice = getArcSlice(a, d)+1;
        Term term(x[d][index], slice);
        exp.addTerm(term);
    }
    Term term(maxSlicePerLink[linkIndex], -1);
    exp.addTerm(term);
    
    std::ostringstream constraintName;
    constraintName << "MaxUsedSlicePerLink_" << linkLabel+1 << "_" << getToBeRouted_k(d).getId()+1;
    Constraint constraint(exp.getTrivialLb(), exp, rhs, constraintName.str());
    return constraint;
}

/* Defines the Overall Max Used Slice Position constraints. */
void FlowForm::setMaxUsedSliceOverallConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        Constraint maxUsedSliceOverallConst = getMaxUsedSliceOverallConstraints(d);
        constraintSet.push_back(maxUsedSliceOverallConst);
    }
    std::cout << "Max Used Slice Overall constraints have been defined..." << std::endl;
}

/* When we use it in the lagrangian, we multiply the constraint by -1 so it will be >= and it will be 
* consistent with the minimization problem. */
Constraint FlowForm::getMaxUsedSliceOverallConstraints(int d){
    Expression exp;
    int rhs,rls;
    Input::NodeMethod nodeMethod = instance.getInput().getChosenNodeMethod();
    for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
        if (getToBeRouted_k(d).getSource() == getNodeLabel((*vecGraph[d]).source(a), d)){
            if(nodeMethod != Input::NODE_METHOD_LINEAR_RELAX){
                int index = getArcIndex(a, d);
                int slice = getArcSlice(a, d)+1;
                //int slice = getArcSlice(a, d);
                Term term(x[d][index], -slice);
                exp.addTerm(term);
            }else{
                int index = getArcIndex(a, d);
                int slice = getArcSlice(a, d)+1;
                //int slice = getArcSlice(a, d);
                Term term(x[d][index], slice);
                exp.addTerm(term);
            }
        }
    }
    if(nodeMethod != Input::NODE_METHOD_LINEAR_RELAX){
        Term term(maxSliceOverall, 1);
        exp.addTerm(term);
        rhs = exp.getTrivialUb();
        rls = 0;
    }else{
        Term term(maxSliceOverall, -1);
        exp.addTerm(term); 
        rhs = 0;
        rls = exp.getTrivialLb();
    }

    std::ostringstream constraintName;
    constraintName << "MaxUsedSliceOverall_" << getToBeRouted_k(d).getId()+1;
    Constraint constraint(rls, exp, rhs, constraintName.str());
    return constraint;
}

void FlowForm::setThresholdConstraints(){
    for (int i = 0; i < instance.getNbEdges(); i++){
        const Constraint & thresholdConstraint = getThresholdConstraint(i);
        constraintSet.push_back(thresholdConstraint);
    }
    std::cout << "Threshold constraints have been defined..." << std::endl;
}


Constraint FlowForm::getThresholdConstraint(int linkIndex){
    Expression exp;
    int upperBound = ceil(0.7*slicesC);
    int lowerBound = -slicesC;
    int linkLabel = instance.getPhysicalLinkFromIndex(linkIndex).getId();
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        for(IterableIntMap< ListDigraph, ListDigraph::Arc >::ItemIt a((*mapItArcLabel[d]),linkLabel); a != INVALID; ++a){ 
            int index = getArcIndex(a, d);
            Term term(x[d][index], getToBeRouted_k(d).getLoadC());
            exp.addTerm(term);
        }
    }
    Term term3(l[linkIndex], -slicesC);
    exp.addTerm(term3);
    std::ostringstream constraintName;
    constraintName << "Threshold_" << linkIndex ;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}



void FlowForm::setMultibandConstraints(){

    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int edge = getCompactEdgeId(e);
        for (int k = 0; k < getNbDemandsToBeRouted(); k++){ 
            const Constraint & multibandConstraint = getMultibandConstraint(k, instance.getPhysicalLinkFromIndex(edge).getId());
            constraintSet.push_back(multibandConstraint);
        }
    }
    std::cout << "Multiband constraints have been defined..." << std::endl;
}

// if the demand is routed, it must be in C band (monoband case)
Constraint FlowForm::getMultibandConstraint(int k, int e){
    Expression exp;
    int upperBound = 0;
    int lowerBound = -1;
    //int lowerBound = 0;
    int linkLabel = e;

    for(IterableIntMap< ListDigraph, ListDigraph::Arc >::ItemIt a((*mapItArcLabel[k]),linkLabel); a != INVALID; ++a){       
        int slice = getArcSlice(a,k);
        //std::cout << "Slice: " << slice << std::endl;
        if(slice >= slicesC){
            int index = getArcIndex(a, k);
            Term term(x[k][index], 1);
            exp.addTerm(term);
        }
    }
    Term term4(l[e], -1);
    exp.addTerm(term4);

    std::ostringstream constraintName;
    constraintName << "Multiband1_" << getToBeRouted_k(k).getId()+1 << e ;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}

void FlowForm::setForcedMonobandConstraints(){
    for (int k = 0; k < getNbDemandsToBeRouted(); k++){
        const Constraint & forcedMonoConstraint = getForcedMonobandConstraint_k(getToBeRouted_k(k), k);
        constraintSet.push_back(forcedMonoConstraint);
    }
    std::cout << "Forced monoband constraints have been defined..." << std::endl;
}

Constraint FlowForm::getForcedMonobandConstraint_k(const Demand &demand, int k){
    Expression exp;
    double upperBound = 0;
    int lowerBound = 0;
    if(demand.getLoadL() == 0)
        for (ListDigraph::ArcIt a(*vecGraph[k]); a != INVALID; ++a){
            int slice = getArcSlice(a, k); 
            int arc = getArcIndex(a, k); 
            if (slice >= slicesC){
                Term term(x[k][arc], 1);
                exp.addTerm(term);
            }
        }
    std::ostringstream constraintName;
    constraintName << "ForcedMono_" << getToBeRouted_k(k).getId()+1;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}



void FlowForm::setCutPool(){
    this->setFlowNodeCuts();
    this->setFlowEdgeCuts();
} 

/* Defines the flow cuts for nodes. */
void FlowForm::setFlowNodeCuts(){
    for (ListGraph::NodeIt n(compactGraph); n != INVALID; ++n){
        int nodeLabel = getCompactNodeLabel(n);
        int degree = getDegree(n);
        if (degree % 2 != 0){
            //for (int s = 0; s < getNbSlicesGlobalLimit(); s++){
            for (int s = 0; s < auxNbSlicesGlobalLimit; s++){
                std::vector<int> cutSet;
                cutSet.push_back(nodeLabel);
                Constraint cut = getCutSetFlowConstraint(cutSet, s, degree);
                cutPool.push_back(cut);
            }
        }
    }
    std::cout << "Flow Node cuts have been defined..." << std::endl;
}

/* Returns the cutset flow cuts associated with a cutset and a slice. */
Constraint FlowForm::getCutSetFlowConstraint(const std::vector<int> &cutSet, int s, int cutCardinality){ 
    
    Expression exp;
    int rhs = (cutCardinality - 1)/2;
    
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        int demandLoad = getToBeRouted_k(d).getLoadC();
        int demandSource = getToBeRouted_k(d).getSource();
        int demandTarget = getToBeRouted_k(d).getTarget();
        if (std::find(cutSet.begin(), cutSet.end(), demandSource) == cutSet.end()) {
            if (std::find(cutSet.begin(), cutSet.end(), demandTarget) == cutSet.end()) {
                for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                    int slice = getArcSlice(a, d);
                    if ((slice >= s) && (slice <= s + demandLoad - 1)){
                        int arc = getArcIndex(a, d); 
                        int arcSource = getNodeLabel((*vecGraph[d]).source(a),d);
                        int arcTarget = getNodeLabel((*vecGraph[d]).target(a),d);
                        if (std::find(cutSet.begin(), cutSet.end(), arcSource) != cutSet.end()) {
                            if (std::find(cutSet.begin(), cutSet.end(), arcTarget) == cutSet.end()) {
                                Term term(x[d][arc], 1);
                                exp.addTerm(term);
                            }
                        }
                    }
                }
            }
        }
    }
    
    std::ostringstream constraintName;
    constraintName << "CutSetFlow__";
    for (unsigned int i = 0; i < cutSet.size(); ++i) {
        constraintName << cutSet[i] << "_";
    }
    constraintName << "_" << s+1;
    Constraint constraint(0, exp, rhs, constraintName.str());
    return constraint;
}

/* Defines the flow cuts for edges. */
void FlowForm::setFlowEdgeCuts(){
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int labelU = getCompactNodeLabel(compactGraph.u(e));
        int labelV = getCompactNodeLabel(compactGraph.v(e));
        int labelE = getCompactEdgeLabel(e);
        std::vector<int> cutSet;
        cutSet.push_back(labelU);
        cutSet.push_back(labelV);
        int cardinality = getCutCardinality(cutSet);
        if (cardinality % 2 != 0){
            //for (int s = 0; s < getNbSlicesGlobalLimit(); s++){
            for (int s = 0; s < auxNbSlicesGlobalLimit; s++){
                Constraint cut = getCutSetFlowConstraint(cutSet, s, cardinality);
                cutPool.push_back(cut);
            }
        }
    }
    std::cout << "Flow Edge cuts have been defined..." << std::endl;
}

/** Returns a vector of node id's corresponding to the sequence of nodes that the d-th demand passes through. **/
std::vector<int> FlowForm::getPathNodeSequence(int d){
    std::vector<int> path;
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
            int arc = getArcIndex(a, d);
            if (x[d][arc].getVal() >= 1 - EPS){
                nextArc = a;
            }
        }
        if (nextArc == INVALID){
            std::cout << "ERROR: Could not find path continuity.." << std::endl;
            exit(0);
        }
        currentNode = (*vecGraph[d]).target(nextArc);
        path.push_back(getNodeLabel(currentNode, d));
    }
    return path;
}

/* Recovers the obtained MIP solution and builds a path for each demand on its associated graph from RSA. */
void FlowForm::updatePath(const std::vector<double> &vals){
    setVariableValues(vals);
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
                int arc = getArcIndex(a, d);
                if (x[d][arc].getVal() >= 1 - EPS){
                    (*vecOnPath[d])[a] = getToBeRouted_k(d).getId();
                    previousArc = a;
                    leftTarget = true;
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
    std::cout << "Leave update." << std::endl;
}

/* Displays the value of each variable in the obtained solution. */
void FlowForm::displayVariableValues(){
    for(int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
            int arc = getArcIndex(a, d);
            std::cout << x[d][arc].getName() << " = " << x[d][arc].getVal() << "   ";
        }
        std::cout << std::endl;
    }
}

/** Returns a set of variables to be fixed to 0 according to the current upper bound. **/
std::vector<Variable> FlowForm::objective8_fixing(const double upperBound){
    std::vector<Variable> vars;
    int initialSlice = std::ceil(upperBound+0.5);
    for (int k = 0; k < getNbDemandsToBeRouted(); k++){
        for (ListDigraph::ArcIt a(*vecGraph[k]); a != INVALID; ++a){
            int arc = getArcIndex(a, k);
            if (getArcSlice(a, k) >= initialSlice){
                vars.push_back(x[k][arc]);
            }
        }
    }
    return vars;
}

std::vector<Constraint> FlowForm::solveSeparationProblemInt(const std::vector<double> &solution, const int threadNo){
    //std::cout << "Entering separation problem of a fractional point for Flow Form." << std::endl;
    return std::vector<Constraint>();
}

std::vector<Constraint> FlowForm::solveSeparationProblemFract(const std::vector<double> &solution){
    //std::cout << "Entering separation problem of a fractional point for Flow Form." << std::endl;
    setVariableValues(solution);
    std::vector<Constraint> cuts;
    for (unsigned int i = 0; i < cutPool.size(); ++i) {
        double lhs = cutPool[i].getExpression().getExpressionValue();
        if ((lhs < cutPool[i].getLb() - 0.0001) || (lhs >  cutPool[i].getUb() + 0.0001 )) {
            //std::cout << "Adding: " << cutPool[i].getName() << " [lhs = " << lhs << "]" << std::endl;
            cuts.push_back(cutPool[i]);
         }
    }
    //std::cout << "Exiting separation problem of a fractional point for Flow Form." << std::endl;
    return cuts;
}

std::vector<Constraint> FlowForm::solveSeparationGnpy(const std::vector<double> &solution, const int threadNo){
    std::vector<Constraint> cuts;
    setVariableValues(solution);
    
    // write service.json file
    //std::string serviceFile = instance.getInput().getOutputPath() + "service_" + std::to_string(threadNo) + ".json";
    //std::string serviceFile = "service_.json";
    //writeServiceFile(serviceFile);
    // launch GNPY
    //std::string resultFile = instance.getInput().getOutputPath() + "result_" + std::to_string(threadNo) + ".json";
    //std::string arguments = instance.getInput().getGNPYTopologyFile() + " " + serviceFile;
    //std::string options = "-e " + instance.getInput().getGNPYEquipmentFile() + " -o " + resultFile;
    //std::string command = "gnpy-path-request " + arguments + " " + options;

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

    /*
    // read result.json file
    std::ifstream ifs(resultFile.c_str());
    std::string fileContent((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        std::cout << d << std::endl;
        std::string requestContent;
        std::string firstDelimiter = "Demand " + std::to_string(getToBeRouted_k(d).getId()+1);
        std::size_t first = fileContent.find(firstDelimiter);
        if (d < getNbDemandsToBeRouted() - 1){
            std::string lastDelimiter = "Demand " + std::to_string(getToBeRouted_k(d).getId()+2);
            std::size_t last = fileContent.find(lastDelimiter);
            requestContent = fileContent.substr(first, last-first);
        }
        else{
            requestContent = fileContent.substr(first);
        }

        std::size_t found = requestContent.find("no-path");
        if (found != std::string::npos){
            std::cout << "Demand " << std::to_string(getToBeRouted_k(d).getId()+1) << ": Unfeasible." << std::endl;
            cuts.push_back(getPathEliminationConstraint(d));
        }
        else{
            std::cout << "Demand " << std::to_string(getToBeRouted_k(d).getId()+1) << ": OK." << std::endl;
        }
    }
    */
    return cuts;
}

Constraint FlowForm::getPathEliminationConstraint(int d){
    int nbHops = 0;
    Expression exp;
    for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
        int arc = getArcIndex(a, d);
        if (x[d][arc].getVal() >= 1 - EPS){
            int label = getArcLabel(a, d);
            nbHops++;
            for (ListDigraph::ArcIt it(*vecGraph[d]); it != INVALID; ++it){
                if (getArcLabel(it, d) == label){
                    int arcIndex = getArcIndex(it, d);
                    exp.addTerm(Term(x[d][arcIndex], 1));
                }
            }
        }
    }
    int rhs = nbHops-1;
    std::ostringstream constraintName;
    constraintName << "PathElimination(" << std::to_string(getToBeRouted_k(d).getId()+1) << ")";
    return Constraint(0, exp, rhs, constraintName.str());
}

void FlowForm::writePathFile(const std::string &file){
    std::ofstream pathsFile;
    pathsFile.open (file.c_str());
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
    
}

void FlowForm::writeServiceFile(const std::string &file){
    std::ofstream serviceFile;
    serviceFile.open (file.c_str());
    serviceFile << "{\n";
    
    serviceFile << "\t\"path-request\": [\n";
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        writePathRequest(serviceFile, d);
        
    }
    
    serviceFile << "\t],\n";
    serviceFile << "\t\"synchronization\": [\n";
    serviceFile << "\t]\n";
    serviceFile << "}\n";
}

void FlowForm::writePathRequest(std::ofstream &serviceFile, int d){

    std::string source = std::to_string(getToBeRouted_k(d).getSource()+1) + ".1";
    std::string destination = std::to_string(getToBeRouted_k(d).getTarget()+1) + ".1";
    std::vector<int> path = getPathNodeSequence(d);
    serviceFile << "\t{\n";
    serviceFile << "\t\t" << "\"request-id\": \"Demand " << std::to_string(getToBeRouted_k(d).getId()+1) << "\",\n";
    serviceFile << "\t\t" << "\"source\": \"" << source << "\",\n";
    serviceFile << "\t\t" << "\"destination\": \"" << destination << "\",\n";
    serviceFile << "\t\t" << "\"src-tp-id\": \"" << source << "\",\n";
    serviceFile << "\t\t" << "\"dst-tp-id\": \"" << destination << "\",\n";
    serviceFile << "\t\t" << "\"bidirectional\": false,\n";
    serviceFile << "\t\t" << "\"path-constraints\": {\n";
    serviceFile << "\t\t\t" << "\"te-bandwidth\": {\n";
    serviceFile << "\t\t\t\t" << "\"technology\": \"flexi-grid\",\n";
    serviceFile << "\t\t\t\t" << "\"trx_type\": \"Voyager\",\n";
    serviceFile << "\t\t\t\t" << "\"trx_mode\": \"" << getToBeRouted_k(d).getMode() << "\",\n";
    serviceFile << "\t\t\t\t" << "\"effective-freq-slot\": [\n";
    serviceFile << "\t\t\t\t\t" << "{\n";
    serviceFile << "\t\t\t\t\t\t" << "\"N\": \"null\",\n";
    serviceFile << "\t\t\t\t\t\t" << "\"M\": \"null\"\n";
    serviceFile << "\t\t\t\t\t" << "}\n";
    serviceFile << "\t\t\t\t" << "],\n";
    serviceFile << "\t\t\t\t" << "\"spacing\":" << getToBeRouted_k(d).getSpacing() << ",\n";
    serviceFile << "\t\t\t\t" << "\"max-nb-of-channel\": null,\n";
    serviceFile << "\t\t\t\t" << "\"output-power\": null,\n";
    serviceFile << "\t\t\t\t" << "\"path_bandwidth\": " << getToBeRouted_k(d).getPathBandwidth() << "\n";
    serviceFile << "\t\t\t" << "}\n";
    serviceFile << "\t\t" << "},\n";
    serviceFile << "\t\t" << "\"explicit-route-objects\": {\n";
    serviceFile << "\t\t\t" << "\"route-object-include-exclude\": [\n";
    for (unsigned int i = 0; i < path.size(); i++){
        serviceFile << "\t\t\t\t" << "{\n";
        serviceFile << "\t\t\t\t\t" << "\"explicit-route-usage\": \"route-include-ero\",\n";
        serviceFile << "\t\t\t\t\t" << "\"index\": " << std::to_string(i) << ",\n";
        serviceFile << "\t\t\t\t\t" << "\"num-unnum-hop\": {\n";
        serviceFile << "\t\t\t\t\t\t" << "\"node-id\": \"" << std::to_string(path[i]+1) << "\",\n";
        serviceFile << "\t\t\t\t\t\t" << "\"link-tp-id\": \"link-tp-id is not used\",\n";
        serviceFile << "\t\t\t\t\t\t" << "\"hop-type\": \"STRICT\"\n";
        serviceFile << "\t\t\t\t\t" << "}\n";
        if (i < path.size()-1){
            serviceFile << "\t\t\t\t" << "},\n";
        }
        else{
            serviceFile << "\t\t\t\t" << "}\n";
        }
    }
    serviceFile << "\t\t\t" << "]\n";
    serviceFile << "\t\t" << "}\n";
    if (d < getNbDemandsToBeRouted()-1){
        serviceFile << "\t" << "},\n";
    }
    else{
        serviceFile << "\t" << "}\n";
    }
}

/****************************************************************************************/
/*										Destructor										*/
/****************************************************************************************/

/* Destructor. Clears the vectors of demands and links. */
FlowForm::~FlowForm(){
    x.clear();
    maxSlicePerLink.clear();
}



