#include "flowForm.h"


/* Constructor. Builds the Online RSA mixed-integer program and solves it using a defined solver (CPLEX or CBC). */
FlowForm::FlowForm(const Instance &inst) : AbstractFormulation(inst){
    if(inst.getInput().getChosenNodeMethod() == Input::NODE_METHOD_LINEAR_RELAX){

        ClockTime time(ClockTime::getTimeNow());
        ClockTime time2(ClockTime::getTimeNow());
        std::cout << "--- Flow formulation has been chosen. " << displayDimensions() << " ---" << std::endl;
        this->setVariables();
        varImpleTime = time.getTimeInSecFromStart() ;
        time.setStart(ClockTime::getTimeNow());
        this->setConstraints();
        constImpleTime = time.getTimeInSecFromStart() ;
        time.setStart(ClockTime::getTimeNow());
        this->setCutPool();
        cutImpleTime = time.getTimeInSecFromStart() ;
        time.setStart(ClockTime::getTimeNow());
        this->setObjectives();
        objImpleTime = time.getTimeInSecFromStart() ;
        std::cout << "--- Flow formulation has been defined ---" << std::endl;
        totalImpleTime = time2.getTimeInSecFromStart() ;
        //std::cout << "Time: " << time.getTimeInSecFromStart() << std::endl;
    }
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
    this->setMaxUsedSlicePerEdgeVariables();
    this->setMaxUsedSliceOverallVariable();
    this->setCBandRoutingVariable();
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
            if( (instance.getInput().isRelaxed() && (instance.getInput().getChosenNodeMethod() == Input::NODE_METHOD_LINEAR_RELAX) ) || ( instance.getInput().isLagrangianRelaxed() && (instance.getInput().getChosenNodeMethod()!=Input::NODE_METHOD_LINEAR_RELAX) )  ){
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
    //int upperBound = auxNbSlicesGlobalLimit-1;
    int varId = getNbVar();
    maxSliceOverall = Variable(varId, lowerBound, upperBound, Variable::TYPE_REAL, 0, varName);
    incNbVar();
    std::cout << "Max slice overall variable has been defined..." << std::endl;
}

void FlowForm::setCBandRoutingVariable(){
    // C Band routing variables
    routedCBand.resize(getNbDemandsToBeRouted());
    for (int k = 0; k < getNbDemandsToBeRouted(); k++){
        std::ostringstream varName;
        varName << "C";
        varName << "(" +  std::to_string(getToBeRouted_k(k).getId() + 1) + ")";
        int upperBound = 1;
        int varId = getNbVar();
        if(instance.getInput().isRelaxed()){
            routedCBand[k] = Variable(varId, 0, upperBound, Variable::TYPE_REAL, 0, varName.str());
        }
        else{
            routedCBand[k] = Variable(varId, 0, upperBound, Variable::TYPE_BOOLEAN, 0, varName.str());
        }   
            incNbVar();
    }
    std::cout << "C band variables have been defined..." << std::endl; 
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
    for (int i = 0; i < instance.getNbEdges(); i++){
        int pos = maxSlicePerLink[i].getId();
        vec[pos] = maxSlicePerLink[i];
    }
    int pos = maxSliceOverall.getId();
    vec[pos] = maxSliceOverall;

    for (int d = 0; d < getNbDemandsToBeRouted(); d++){ 
        int pos = routedCBand[d].getId();
        vec[pos] = routedCBand[d];
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
        for (int i = 0; i < instance.getNbEdges(); i++){
            int pos = maxSlicePerLink[i].getId();
            double newValue = vals[pos];
            maxSlicePerLink[i].setVal(newValue);
        }
        int pos = maxSliceOverall.getId();
        double newValue = vals[pos];
        maxSliceOverall.setVal(newValue);
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){ 
            int pos = routedCBand[d].getId();
            double newValue = vals[pos];
            routedCBand[d].setVal(newValue);
        }

    }
}

/********************************** Lagrangian Inclusion ********************************/

VarArray FlowForm::getLagVariables(){
    VarArray vec;
    vec.resize(getNbVar());
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){ 
        for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
            int arc = getArcIndex(a, d);
            int pos = x[d][arc].getId();
            vec[pos] = x[d][arc];
        }
    }
    if(instance.getInput().isObj8(0)){
        int pos = maxSliceOverall.getId();
        vec[pos] = maxSliceOverall;
    }
    return vec;
}

void FlowForm::setLagVariables(){
    this->setFlowVariables();
    if(instance.getInput().isObj8(0)){
        this->setMaxUsedSliceOverallVariable();
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
    case Input::OBJECTIVE_METRIC_1p:
        {
        for (int i = 0; i < instance.getNbEdges(); i++){
            Term term(maxSlicePerLink[i], 1);
            obj.addTerm2(term);
        }
        break;
        }
    case Input::OBJECTIVE_METRIC_2:
        {
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                int arc = getArcIndex(a, d);
                double coeff = getCoeffObj2(a, d);
                Term term(x[d][arc], coeff);
                obj.addTerm2(term);
            }
        }
        break;
        }
    case Input::OBJECTIVE_METRIC_2p:
        {
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                int arc = getArcIndex(a, d);
                double coeff = getCoeffObj2p(a, d);
                Term term(x[d][arc], coeff);
                obj.addTerm2(term);
            }
        }
        break;
        }
    case Input::OBJECTIVE_METRIC_4:
        {
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                int arc = getArcIndex(a, d);
                double coeff = getCoeffObj4(a, d);
                Term term(x[d][arc], coeff);
                obj.addTerm2(term);
            }
        }
        break;
        }
    case Input::OBJECTIVE_METRIC_4p:
        {
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                int arc = getArcIndex(a, d);
                double coeff = getCoeffObj4p(a, d);
                Term term(x[d][arc], coeff);
                obj.addTerm2(term);
            }
        }
        break;
        }
    case Input::OBJECTIVE_METRIC_8:
        {
        Term term(maxSliceOverall, 1);
        obj.addTerm2(term);
        break;
        }

    case Input::OBJECTIVE_METRIC_10:
        {
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                int arc = getArcIndex(a, d);
                double coeff = getCoeffObj10(a, d);
                Term term(x[d][arc], coeff);
                obj.addTerm2(term);
            }
        }
        break;
        }
    case Input::OBJECTIVE_METRIC_10p:
        {
        for (int k = 0; k < getNbDemandsToBeRouted(); k++){                
            Term term(routedCBand[k], -getToBeRouted_k(k).getLoad());
            obj.addTerm(term);
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
    
    //this->setLengthConstraints();
    if (this->getInstance().getInput().isMaxReachEnabled() == true){
        this->setLengthConstraints();
    }
    if (this->getInstance().getInput().isOSNREnabled() == true){
        this->setOSNRConstraints();
    }
    //this->setStrongLengthConstraints();
    this->setNonOverlappingConstraints();    

    this->setMaxUsedSlicePerLinkConstraints();    
    this->setMaxUsedSliceOverallConstraints();    
    this->setMaxUsedSliceOverallConstraints2();    
    //this->setMaxUsedSliceOverallConstraints3();   
    this->setCBandRoutingConstraints();
    nbConstraint = constraintSet.size();
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
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_10p){
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
    if (chosenObjectives[0] == Input::OBJECTIVE_METRIC_10p){
        lhs = 0;
    }
    std::ostringstream constraintName;
    constraintName << "Target_" << demand.getId()+1 ;
    Constraint constraint(lhs, exp, rhs, constraintName.str());
    return constraint;
}

/* Defines the reinforced max reach constraints. */
void FlowForm::setStrongLengthConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){   
        //for (int s = 0; s < getNbSlicesGlobalLimit(); s++){  
        for (int s = 0; s < auxNbSlicesGlobalLimit; s++){  
            const Constraint & strongLengthConstraint = getStrongLengthConstraint(getToBeRouted_k(d), d, s);
            constraintSet.push_back(strongLengthConstraint);
        }
    }
    std::cout << "Strong length constraints have been defined..." << std::endl;
}

/* Returns the strong max reach constraint associated with a demand and a slice. */
Constraint FlowForm::getStrongLengthConstraint(const Demand &demand, int d, int s){
    Expression exp;
    int source = demand.getSource();
    int hop = instance.getInput().getHopPenalty();
    for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
        int arc = getArcIndex(a, d); 
        if (getArcSlice(a, d) == s){
            double coeff = getArcLength(a, d);
            int tail = getNodeLabel((*vecGraph[d]).source(a), d);
            if (tail != source){
                coeff += hop;
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
                    Term term(x[d][arc], -demand.getMaxLength());
                    exp.addTerm(term);
                }
            }
        }
    }
    std::ostringstream constraintName;
    constraintName << "StrongLength_" << demand.getId()+1 << "_" << s+1;
    Constraint constraint(exp.getTrivialLb(), exp, 0, constraintName.str());
    return constraint;
}

/* Defines Length constraints. Demands must be routed within a length limit. */
void FlowForm::setLengthConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){   
        const Constraint & lengthConstraint = getLengthConstraint(getToBeRouted_k(d), d);
        constraintSet.push_back(lengthConstraint);
    }
    std::cout << "Length constraints have been defined..." << std::endl;
}

/* Returns the length constraint associated with a demand. */
/* When we use it in the lagrangian, we multiply the constraint by -1 so it will be >= and it will be 
* consistent with the minimization problem. */
Constraint FlowForm::getLengthConstraint(const Demand &demand, int d){
    Expression exp;
    Input::NodeMethod nodeMethod = instance.getInput().getChosenNodeMethod();
    double rhs; double rls;
    if(nodeMethod != Input::NODE_METHOD_LINEAR_RELAX){
        rls = -demand.getMaxLength(); rhs = 0;
    }else{
        rhs = demand.getMaxLength(); rls = 0;
    }
    int source = demand.getSource();
    int hop = instance.getInput().getHopPenalty();
    
    for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
        int arc = getArcIndex(a, d); 
        double coeff = getArcLength(a, d);
        int tail = getNodeLabel((*vecGraph[d]).source(a), d);
        if (tail != source){
            coeff += hop;
        }
        if(nodeMethod != Input::NODE_METHOD_LINEAR_RELAX){
            Term term(x[d][arc], -coeff);
            exp.addTerm(term);
        }else{
            Term term(x[d][arc], coeff);
            exp.addTerm(term);
        }
    }
    std::ostringstream constraintName;
    constraintName << "Length_" << demand.getId()+1;
    Constraint constraint(rls, exp, rhs, constraintName.str());
    return constraint;
}

/* Defines OSNR constraints. Demands must be routed within a OSNR limit. */
void FlowForm::setOSNRConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){   
        const Constraint & OSNRConstraintC = getOSNRCConstraint(getToBeRouted_k(d), d);
        constraintSet.push_back(OSNRConstraintC);
        if (getInstance().getInput().getNbBands() == 2) {
            std::cout << "TODO: no OSNR constraints for band L yet" << std::endl;
            //const Constraint & OSNRConstraintL = getOSNRLConstraint(getToBeRouted_k(d), d);
            //constraintSet.push_back(OSNRConstraintL);
        } 
        if (getInstance().getInput().getNbBands() == 3) {
            std::cout << "TODO: no OSNR constraints for band S yet" << std::endl;
            //const Constraint & OSNRConstraintS = getOSNRSConstraint(getToBeRouted_k(d), d);
            //constraintSet.push_back(OSNRConstraintS);
        }  
    }
    std::cout << "OSNR constraints have been defined..." << std::endl;
}

/* Returns the OSNR constraint associated with a demand. */
Constraint FlowForm::getOSNRCConstraint(const Demand &demand, int d){
    Expression exp;
    double rhs; double rls;
    
    double osnrLimdb = demand.getOsnrLimit();
    double osnrLim = pow(10,osnrLimdb/10);
    double pch = demand.getPchC();

    double roundingFactor = pow(10,8);
    
    rhs = pch/osnrLim - instance.getPaseNodeC() ;
    //std::cout << rhs << std::endl;
    rhs = ceil(rhs * roundingFactor*100)/100 ; //ROUNDING
    //std::cout << rhs << std::endl;
    rls = 0;
    for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
        //First term
        int arc = getArcIndex(a, d); 
        double coeff = getArcPaseLineC(a, d) * roundingFactor;
        coeff = ceil(coeff*100)/100; //ROUNDING
        //std::cout  << "-Pase line arredondado" << coeff << " Pase line" << getArcPaseLine(a, d) << std::endl;
        Term term(x[d][arc], coeff);
        exp.addTerm(term);
        //Second term
        coeff = 1.0 * ceil(instance.getPaseNodeC() * roundingFactor*100)/100; //ROUNDING
        //std::cout  << "-Pase node arredondado" << coeff << " Pase node" << instance.getPaseNode() << std::endl;
        Term term2(x[d][arc], coeff);
        exp.addTerm(term2);
        //Third term
        coeff = getArcPnliC(a, d)* roundingFactor;
        coeff = ceil(coeff*100)/100; //ROUNDING
        //std::cout << "-Pnli arredondado" << coeff << " pnli" << getArcPnli(a, d) << std::endl;
        Term term3(x[d][arc], coeff);
        exp.addTerm(term3);
        //std::cout  << "--" << std::endl;
    }
    std::ostringstream constraintName;
    constraintName << "OSNR_" << demand.getId()+1;
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
        int sliceLimit = auxNbSlicesLimitFromEdge[i];
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
        int demandLoad = getToBeRouted_k(d).getLoad();
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

/* Defines the Link's Max Used Slice Position constraints. The max used slice position on each link must be greater than every slice position used in the link. */
void FlowForm::setMaxUsedSlicePerLinkConstraints2(){
    for (int i = 0; i < instance.getNbEdges(); i++){
        //int sliceLimit = getNbSlicesLimitFromEdge(i);
        int sliceLimit = auxNbSlicesLimitFromEdge[i];
        for (int s = 0; s < sliceLimit; s++){
            const Constraint & maxUsedSlicePerLinkConst = getMaxUsedSlicePerLinkConstraints2(i, s);
            constraintSet.push_back(maxUsedSlicePerLinkConst);
        }
    }
    std::cout << "Max Used Slice Per Link constraints 2 have been defined..." << std::endl;
}

Constraint FlowForm::getMaxUsedSlicePerLinkConstraints2(int linkIndex, int s){
    Expression exp;
    int rhs = 0;
    int linkLabel = instance.getPhysicalLinkFromIndex(linkIndex).getId();
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        int demandLoad = getToBeRouted_k(d).getLoad();
        for(IterableIntMap< ListDigraph, ListDigraph::Arc >::ItemIt a((*mapItArcLabel[d]),linkLabel); a != INVALID; ++a){ 
            if ((getArcSlice(a, d) >= s) && (getArcSlice(a, d) <= s + demandLoad - 1)){
                int index = getArcIndex(a, d);
                int slice = getArcSlice(a, d)+1;
                Term term(x[d][index], slice);
                exp.addTerm(term);
            }
        }
    }
    Term term(maxSlicePerLink[linkIndex], -1);
    exp.addTerm(term);
    
    std::ostringstream constraintName;
    constraintName << "MaxUsedSlicePerLink_" << linkLabel+1 << "_" << s+1;
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

/* Defines the Overall Max Used Slice Position constraints. */
void FlowForm::setMaxUsedSliceOverallConstraints2(){
    for (int i = 0; i < instance.getNbEdges(); i++){
        //int sliceLimit = getNbSlicesLimitFromEdge(i);
        int sliceLimit = auxNbSlicesLimitFromEdge[i];
        for (int s = 0; s < sliceLimit; s++){
            Constraint maxUsedSliceOverallConst = getMaxUsedSliceOverallConstraints2(instance.getPhysicalLinkFromIndex(i).getId(), s);
            constraintSet.push_back(maxUsedSliceOverallConst);
        }
    }
    std::cout << "Max Used Slice Overall2 constraints have been defined..." << std::endl;
}

Constraint FlowForm::getMaxUsedSliceOverallConstraints2(int linkLabel, int s){
    Expression exp;
    int rhs = 0;
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        int demandLoad = getToBeRouted_k(d).getLoad();
        for(IterableIntMap< ListDigraph, ListDigraph::Arc >::ItemIt a((*mapItArcLabel[d]),linkLabel); a != INVALID; ++a){ 
            if ((getArcSlice(a, d) >= s) && (getArcSlice(a, d) <= s + demandLoad - 1)){
                int index = getArcIndex(a, d);
                int slice = getArcSlice(a, d)+1;
                //int slice = getArcSlice(a, d);
                Term term(x[d][index], slice);
                exp.addTerm(term);
            }
        }
    }
    Term term(maxSliceOverall, -1);
    exp.addTerm(term);

    std::ostringstream constraintName;
    constraintName << "MaxUsedSliceOverall2_" << linkLabel+1 << "_" << s+1;
    Constraint constraint(exp.getTrivialLb(), exp, rhs, constraintName.str());
    return constraint;
}

/* Defines the Overall Max Used Slice Position constraints 3. */
void FlowForm::setMaxUsedSliceOverallConstraints3(){
    for (int i = 0; i < instance.getNbNodes(); i++){
        //for (int s = 0; s < getNbSlicesGlobalLimit(); s++){
        for (int s = 0; s < auxNbSlicesGlobalLimit; s++){
            Constraint maxUsedSliceOverallConst = getMaxUsedSliceOverallConstraints3(i, s);
            constraintSet.push_back(maxUsedSliceOverallConst);
        }
    }
    std::cout << "Max Used Slice Overall3 constraints have been defined..." << std::endl;
}

Constraint FlowForm::getMaxUsedSliceOverallConstraints3(int nodeLabel, int s){
    Expression exp;
    int rhs = 0;
    int degree = 0;
    for (ListGraph::NodeIt v(compactGraph); v != INVALID; ++v){
        if ((getCompactNodeLabel(v) == nodeLabel)){
            for (ListGraph::IncEdgeIt a(compactGraph, v); a != INVALID; ++a){
                degree++;
            }
        }
    }

    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        int demandLoad = getToBeRouted_k(d).getLoad();
        for(IterableIntMap< ListDigraph, ListDigraph::Node >::ItemIt v((*mapItNodeLabel[d]),nodeLabel); v != INVALID; ++v){ 
            for (ListDigraph::OutArcIt a(*vecGraph[d], v); a != INVALID; ++a){
                if ( (getArcSlice(a, d) >= s) && (getArcSlice(a, d) <= s + demandLoad - 1) ){
                    int index = getArcIndex(a, d);
                    int slice = getArcSlice(a, d)+1;
                    Term term(x[d][index], slice);
                    exp.addTerm(term);
                }
            }
        }
    }
    Term term(maxSliceOverall, -degree);
    exp.addTerm(term);
    
    std::ostringstream constraintName;
    constraintName << "MaxUsedSliceOverall3_" << nodeLabel+1 << "_" << s+1;
    Constraint constraint(exp.getTrivialLb(), exp, rhs, constraintName.str());
    return constraint;
}

/* Defines the Overall Max Used Slice Position constraints 3. */
void FlowForm::setMaxUsedSliceOverallConstraints4(){
    for (int i = 0; i < instance.getNbNodes(); i++){
        Constraint maxUsedSliceOverallConst = getMaxUsedSliceOverallConstraints4(i);
        constraintSet.push_back(maxUsedSliceOverallConst);
    }
    std::cout << "Max Used Slice Overall4 constraints have been defined..." << std::endl;
}

Constraint FlowForm::getMaxUsedSliceOverallConstraints4(int nodeLabel){
    Expression exp;
    int rhs = 0;
    exp.addTerm(Term(maxSlicePerLink[nodeLabel], 1));
    exp.addTerm(Term(maxSliceOverall, -1));
    
    std::ostringstream constraintName;
    constraintName << "MaxUsedSliceOverall4_" << nodeLabel+1;
    Constraint constraint(exp.getTrivialLb(), exp, rhs, constraintName.str());
    return constraint;
}

void FlowForm::setCBandRoutingConstraints(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){  
        const Constraint & CBandRoutingConstraint = getCBandRoutingConstraint(d);
        constraintSet.push_back(CBandRoutingConstraint);
        for (int i = 0; i < instance.getNbEdges(); i++){
            const Constraint & CBandRoutingConstraint2 = getCBandRoutingConstraint2(d,i);
            constraintSet.push_back(CBandRoutingConstraint2);
        }
    }
    std::cout << "C Band routing constraints have been defined..." << std::endl;
}

// if the demand is routed, it must be in C band (monoband case)
Constraint FlowForm::getCBandRoutingConstraint(int d){
    Expression exp;
    int upperBound = 0;
    int lowerBound = 0;
    for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
        if (getToBeRouted_k(d).getSource() == getNodeLabel((*vecGraph[d]).source(a), d)){
            int index = getArcIndex(a, d);
            int slice = getArcSlice(a, d)+1;
            Term term(x[d][index], 1);
            exp.addTerm(term);
        }
    }
    Term term2(routedCBand[d], -1);
    exp.addTerm(term2);
    std::ostringstream constraintName;
    constraintName << "CBandRouting1_" << getToBeRouted_k(d).getId()+1 ;
    Constraint constraint(lowerBound, exp, upperBound, constraintName.str());
    return constraint;
}


Constraint FlowForm::getCBandRoutingConstraint2(int d, int e){
    Expression exp;
    int upperBound = 0;
    int lowerBound = -1;
    int linkLabel = instance.getPhysicalLinkFromIndex(e).getId();
    for(IterableIntMap< ListDigraph, ListDigraph::Arc >::ItemIt a((*mapItArcLabel[d]),linkLabel); a != INVALID; ++a){ 
        int index = getArcIndex(a, d);
        Term term(x[d][index], 1);
        exp.addTerm(term);
    }

    Term term3(routedCBand[d], -1);
    exp.addTerm(term3);
    std::ostringstream constraintName;
    constraintName << "CBandRouting2_" << getToBeRouted_k(d).getId()+1 ;
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
        int demandLoad = getToBeRouted_k(d).getLoad();
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
    std::string serviceFile = instance.getInput().getOutputPath() + "service_" + std::to_string(threadNo) + ".json";
    writeServiceFile(serviceFile);

    // launch GNPY
    std::string resultFile = instance.getInput().getOutputPath() + "result_" + std::to_string(threadNo) + ".json";
    std::string arguments = instance.getInput().getGNPYTopologyFile() + " " + serviceFile;
    std::string options = "-e " + instance.getInput().getGNPYEquipmentFile() + " -o " + resultFile;
    std::string command = "gnpy-path-request " + arguments + " " + options;
    system(command.c_str());

    // read result.json file
    std::ifstream ifs(resultFile.c_str());
    std::string fileContent((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
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
    
    return cuts;
}
/*
Constraint FlowForm::getastraint(int d){
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
}*/

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

/********************************** Lagrangian Inclusion ********************************/

void FlowForm::setLagConstraints(){
    this->setSourceConstraints();
    this->setFlowConservationConstraints();
    this->setTargetConstraints();
    this->setLengthConstraints();
    this->setNonOverlappingConstraints();    
    
    if(instance.getInput().isObj8(0)){
        this->setMaxUsedSliceOverallConstraints();   
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



