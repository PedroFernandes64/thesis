#include "lagTools.h"


operatorCost::operatorCost(std::vector< std::vector<double> > lag){
    lagrangianMultiplierOverlap.resize(lag.size());
    for(int i = 0; i < lag.size();i++){
        std::copy(lag[i].begin(), lag[i].end(),std::back_inserter(lagrangianMultiplierOverlap[i]));
    } 
}

double operatorCost::operator() (int label,int slice) const{
    double value = 0.0;
    for(int s = slice - demandLoad + 1; s <= slice; s++){
        value += lagrangianMultiplierOverlap[label][s];
    }
    return value;
}

double operatorCostObj8::operator()(int slice, ListDigraph::Node node) const {
    if(node == source){
        return slice*(multiplier - multiplierAux);
    }
    return 0.0;
}

double operatorCoefficient::operator()(double length,int slice) const{
    double coeff = 0.0;
    switch (chosenObj){
        case Input::OBJECTIVE_METRIC_0:
        {
            coeff = 0.0;
            break;
        }
        case Input::OBJECTIVE_METRIC_1:
        {
            if(slice != 0){
                coeff = slice;
            }else{
                coeff = 0.0;
            }
            break;
        }
        case Input::OBJECTIVE_METRIC_1p:
        {
            coeff = 0.0;
            break;
        }
        case Input::OBJECTIVE_METRIC_2:
        {
            coeff = 1.0;
            break;
        }
        case Input::OBJECTIVE_METRIC_2p:
        {
            coeff = demandLoad;
            break;
        }
        case Input::OBJECTIVE_METRIC_4:
        {
            coeff = length;
            break;
        }
        case Input::OBJECTIVE_METRIC_8:
        {   
            coeff = 0.0;
            break;
        }
        default:
        {
            std::cerr << "Objective metric out of range.\n";
            exit(0);
            break;
        }
    }   
    return coeff;
}

int operatorSliceCoefficient::operator()(int slice,ListDigraph::Node node) const{
    if(node == source){
        return slice + 1;
    }else{
        return 0;
    }
}

operatorCostEFlow::operatorCostEFlow(std::vector<std::vector<double> > multiplier_){
    multiplier.resize(multiplier_.size());
    for(int i = 0; i < multiplier_.size();i++){
        std::copy(multiplier_[i].begin(), multiplier_[i].end(),std::back_inserter(multiplier[i]));
    } 
}

double operatorCostEFlow::operator()(int demand,int nodeLabel) const{
    if(demand == -1){
        return 0.0;
    }
    return signal*multiplier[demand][nodeLabel];
}

operatorCostETarget::operatorCostETarget(std::vector<std::vector<double> > multiplier_,std::vector<int> target_){
    multiplier.resize(multiplier_.size());
    for(int i = 0; i < multiplier_.size();i++){
        std::copy(multiplier_[i].begin(), multiplier_[i].end(),std::back_inserter(multiplier[i]));
    } 
    std::copy(target_.begin(),target_.end(),std::back_inserter(labelTarget));
}

double operatorCostETarget::operator()(int demand, int labelNode) const{
    if(demand == -1){
        return 0.0;
    }
    if(labelNode == labelTarget[demand]){
        return multiplier[demand][labelNode];
    }
    return 0.0;
}

operatorCostESource::operatorCostESource(std::vector<std::vector<double> > multiplier_,std::vector<int> source_){
    multiplier.resize(multiplier_.size());
    for(int i = 0; i < multiplier_.size();i++){
        std::copy(multiplier_[i].begin(), multiplier_[i].end(),std::back_inserter(multiplier[i]));
    } 
    std::copy(source_.begin(),source_.end(),std::back_inserter(labelSource));
}

double operatorCostESource::operator()(int demand, int labelNode) const{
    if(demand == -1){
        return 0.0;
    }
    if(labelNode == labelSource[demand]){
        return multiplier[demand][labelNode];
    }
    return multiplier[demand][labelNode];
}

operatorCostELength::operatorCostELength(std::vector<double>  multiplier_,std::vector<double> max_){
    std::copy(multiplier_.begin(), multiplier_.end(),std::back_inserter(multiplier));
    std::copy(max_.begin(),max_.end(),std::back_inserter(maxLength));
}

double operatorCostELength::operator()(int demand, double length) const{
    if(demand == -1){
        return 0.0;
    }
    return multiplier[demand]*length/maxLength[demand];
}

operatorCostEOneSlicePerDemand::operatorCostEOneSlicePerDemand(std::vector<std::vector<double>> multiplier_){
    multiplier.resize(multiplier_.size());
    for(int i = 0; i < multiplier_.size();i++){
        std::copy(multiplier_[i].begin(), multiplier_[i].end(),std::back_inserter(multiplier[i]));
    } 
}

double operatorCostEOneSlicePerDemand::operator()(int demand, int demand2) const{
    if(demand == -1){
        return 0.0;
    }
    return multiplier[label][demand];
}

operatorLowerUpperBound::operatorLowerUpperBound(double * point){
    bound = point;
}

int operatorLowerUpperBound::operator()(int id, int index2) const{
    return bound[id];
}

operatorHeuristicAdaptedCost::operatorHeuristicAdaptedCost(const double * point){
    bound = point;
}

double operatorHeuristicAdaptedCost::operator()(int id, double cost) const{
    return (1-bound[id])*cost;
}