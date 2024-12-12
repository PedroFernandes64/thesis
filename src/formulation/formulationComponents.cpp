#include "formulationComponents.h"

Variable::Variable(int identifier, double lowerBound, double upperBound, Type varType, double val, std::string varName, int prio): id(identifier), lb(lowerBound), ub(upperBound), type(varType), value(val), name(varName), priority(prio), warmstartValue(0.0){}

Variable::Variable(const Variable& var){
    id = var.id;
    lb = var.lb;
    ub = var.ub;
    type = var.type;
    value = var.value;
    name = var.name;
    priority = var.priority;
    warmstartValue = var.warmstartValue;
}

Variable& Variable::operator=(const Variable& var){
    id = var.id;
    lb = var.lb;
    ub = var.ub;
    type = var.type;
    value = var.value;
    name = var.name;
    priority = var.priority;
    warmstartValue = var.warmstartValue;
    return (*this);
}

Term::Term(Variable variable, double coefficient): coeff(coefficient), var(variable){}

Term::Term(const Term & term):coeff(term.coeff),var(term.var){}

Expression::Expression(const Expression &e): termsArray(e.getTerms()){}

Expression::Expression(const Variable &v){
    termsArray.push_back(Term(v, 1));
}

Constraint::Constraint(double lowerBound, Expression &e, double upperBound, std::string constName): lb(lowerBound), expr(e), ub(upperBound), name(constName){}

ObjectiveFunction::ObjectiveFunction(Expression &e, Direction d): expr(e), direction(d){}

void Expression::addTerm(const Term &term) {
    for (unsigned int i = 0; i < termsArray.size(); i++){
        if (termsArray[i].getVar().getId() == term.getVar().getId()){
            double oldCoeff = termsArray[i].getCoeff();
            double newCoeff = oldCoeff + term.getCoeff();
            termsArray[i].setCoeff(newCoeff);
            return;
        }
    }
    termsArray.push_back(term); 
}

double Expression::getExpressionValue(){
    double val = 0.0;
    for (unsigned int i = 0; i < termsArray.size(); i++){
        val += (termsArray[i].getCoeff()*termsArray[i].getVar().getVal());
    }
    return val;
}

double Expression::getTrivialUb(){
    double val = 0.0;
    for (unsigned int i = 0; i < termsArray.size(); i++){
        if (termsArray[i].getCoeff() >= 0) 
            val += termsArray[i].getCoeff()*termsArray[i].getVar().getUb();
        else 
            val += termsArray[i].getCoeff()*termsArray[i].getVar().getLb();
    }
    return val;
}
double Expression::getTrivialLb(){
    double val = 0.0;
    for (unsigned int i = 0; i < termsArray.size(); i++){
        if (termsArray[i].getCoeff() <= 0) 
            val += termsArray[i].getCoeff()*termsArray[i].getVar().getUb();
        else 
            val += termsArray[i].getCoeff()*termsArray[i].getVar().getLb();
    }
    return val;
}

std::string Expression::to_string() const {
    std::string exp = "";
    for (unsigned int i = 0; i < termsArray.size(); i++){
        exp += std::to_string(termsArray[i].getCoeff()) + "*" + termsArray[i].getVar().getName() + " + ";
    }
    return exp;
}

void Constraint::display(){
    int size = this->getExpression().getNbTerms();
    std::cout << this->getLb() << " <= " << std::endl;
    for (int i = 0; i < size; i++){
        std::string coefficient = "";
        double c = this->getExpression().getTerm_i(i).getCoeff();
        if (c < 0){
            coefficient = "(" + std::to_string(c) + ")";
        }
        else{
            coefficient = std::to_string(c);
        }
        if (i > 0){
            std::cout << " + ";
        }
        std::cout << coefficient << "*" << this->getExpression().getTerm_i(i).getVar().getName();
    }
    std::cout << " <= " << this->getUb() << std::endl;
}