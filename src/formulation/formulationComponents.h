#ifndef __FormulationComponents__h
#define __FormulationComponents__h

#include "../topology/input.h"

#include <string>
#include <vector>
#include <iostream>
/********************************************************************************************
 * This class identifies a Variable in a MIP formulation. A demand is defined by its id, its 
 * lower and upper bounds, and its type. 
 ********************************************************************************************/
class Variable
{    
    
public: 
	/** Enumerates the possible types of variable. **/
    enum Type {						
		TYPE_REAL = 0,    /**< Variable can admit real values. **/
		TYPE_BOOLEAN = 1, /**< Variable can only admit boolean values. **/
		TYPE_INTEGER = 2  /**< Variable can only admit integer values. **/
	};

private:
    int id;         	/** The variable identifier. @note The variable id is used for identifying its position in the vector of variables used in the implementation of the model. **/
    double lb;      	/** The variable lower bound. **/
    double ub;      	/** The variable upper bound. **/
    Type type;      	/** The variable type. **/
	double value;		/** The variable value. **/
    std::string name;	/** The variable name. **/
	int priority;		/** The variable priority during branching. **/

	double warmstartValue;

public:
    /** Constructor. @param lowerBound The variable lower bound. @param upperBound The variable upper bound. @param varType The variable type. @param val The variable value. @param varName The variable name. **/
    Variable(int identifier=-1, double lowerBound=0, double upperBound=1, Type varType=TYPE_BOOLEAN, double val=0, std::string varName="",int prio =0);

	/** Copy constructor */
	Variable(const Variable&);

	Variable& operator=(const Variable&);

	/****************************************************************************************/
	/*										Getters											*/
	/****************************************************************************************/
	/** Returns the variable's id. **/
    const int & getId() const { return id; }

	/** Returns the variable's lower bound. **/
    double getLb() const { return lb; }

	/** Returns the variable's upper bound. **/
    double getUb() const { return ub; }

	/** Returns the variable's type. **/
    const Type & getType() const { return type; }

	/** Returns the variable's name. **/
    const std::string & getName() const { return name; }

	/** Returns the variable's value. **/
    double getVal() const { return value; }

	/** Returns the variable's priority. **/
    double getPriority() const { return priority; }

	/** Returns the variable's priority. **/
    double getWarmstartValue() const { return warmstartValue; }

	/****************************************************************************************/
	/*										Setters											*/
	/****************************************************************************************/
	/** Changes the variable's id. @param identifier The new id. **/
    void setId(int identifier) { this->id = identifier; }

	/** Changes the variable's lower bound. @param lowerBound The new lower bound. **/
    void setLb(double lowerBound) { this->lb = lowerBound; }

	/** Changes the variable's upper bound. @param upperBound The new upper bound. **/
    void setUb(double upperBound) { this->ub = upperBound; }

	/** Changes the variable's type.  @param varType The new type. **/
    void setType(Type varType) { this->type = varType; }

	/** Changes the variable's value. @param val The new value. **/
    void setVal(double val) { this->value = val; }

	/** Changes the variable's priority. @param prio The new value. **/
    void setPriority(double prio) { this->priority = prio; }


	/** Changes the variable's priority. @param prio The new value. **/
    void setWarmstartValue(double val) { this->warmstartValue = val; }
};


/***************************************************************************************************
 * This class identifies a Term in an Expression. A term is defined by a Variable and a coefficent. 
 ***************************************************************************************************/
class Term
{    

private:
    double coeff;      /**< The term coefficient. **/
    Variable var;      /**< The term variable. **/

public:
    /** Constructor. @param variable The term variable. @param coefficient The term coeffiencient. **/
    Term(Variable variable, double coefficient);
	Term(const Term &);

	/****************************************************************************************/
	/*										Getters											*/
	/****************************************************************************************/
	/** Returns the term's coefficient. **/
    const double & getCoeff() const { return coeff; }

	/** Returns the term's variable. **/
    const Variable & getVar() const { return var; }

	/****************************************************************************************/
	/*										Setters											*/
	/****************************************************************************************/
	/** Changes the term's coefficient. @param coefficient The new coefficient. **/
    void setCoeff(double coefficient) { this->coeff = coefficient; }

	/** Changes the term's variable. @param v The new variable. **/
    void setVar(const Variable &v) { this->var = v; }
};


/********************************************************************************************
 * This class identifies an Expression in a MIP formulation. An Expression is defined by a 
 * vector of Term. 
 ********************************************************************************************/
class Expression
{    

private:
    std::vector<Term> termsArray;  /**< The array of terms. **/

public:
    /** Default constructor. **/
    Expression(){}

    /** Copy constructor. @param e The expression to be copied. **/
    Expression(const Expression &e);
    Expression(const Variable &v);

	/****************************************************************************************/
	/*										Getters											*/
	/****************************************************************************************/
	/** Returns the vector of terms defining the expression. **/
    const std::vector<Term> & getTerms() const { return termsArray; }

	/** Returns the i-th term. @param pos Term position in the array. **/
    const Term & getTerm_i(int pos) const { return termsArray[pos]; }

	/** Returns the number of terms in the expression. **/
	int getNbTerms() const { return termsArray.size(); }
	/****************************************************************************************/
	/*										Setters											*/
	/****************************************************************************************/
	/** Changes the vector of terms. @param vec The new vector of terms. **/
    void setTerms(std::vector<Term> &vec) { this->termsArray = vec; }

	/** Changes the i-th term. @param pos Term position. @param term The new term. **/
    void setTerm_i(int pos, Term &term) { this->termsArray[pos] = term; }

	/****************************************************************************************/
	/*										Methods											*/
	/****************************************************************************************/
	/** Adds a new term to the expression. @param term The term to be added. **/
    void addTerm(const Term &term);

	void addTerm2(const Term &term) {termsArray.push_back(term); }

	double getExpressionValue();

	double getTrivialUb();
	double getTrivialLb();

	std::string to_string() const;

    /** Clears the array of terms. **/
    void clear() { termsArray.clear(); }
};


/********************************************************************************************
 * This class identifies an objective function in a MIP formulation. An Objective Function
 * is defined by an Expression and a Direction.
 ********************************************************************************************/
class ObjectiveFunction
{    
public:
	/** Enumerates the possible directions to optimize. **/
    enum Direction {						
		DIRECTION_MIN = 0,    /**< Minimize expression. **/
		DIRECTION_MAX = 1,    /**< Maximize expression. **/
	};

private:
    Expression expr;        /**< The objective function expression. **/
    Direction direction;    /**< The direction to be optimized. **/
	std::string name;		/**< The objective name. **/
	Input::ObjectiveMetric id;	/** The objective identifier. **/

public:
    /** Constructor. @param e The expression. @param d The direction. **/
    ObjectiveFunction(Expression &e, Direction d);
    /** Default constructor.**/
    ObjectiveFunction(std::string n = "") : name(n) {}

	/****************************************************************************************/
	/*										Getters											*/
	/****************************************************************************************/
	/** Returns the objective function's direction. **/
    Direction getDirection() const { return direction; }
    
	/** Returns the objective function's expression. **/
    const Expression & getExpression() const { return expr; }

	/** Returns the objective function's name. **/
    std::string getName() const { return name; }

	/** Returns the objective function's id. **/
    Input::ObjectiveMetric getId() const { return id; }

	/****************************************************************************************/
	/*										Setters											*/
	/****************************************************************************************/

	/** Changes the objective function's direction. @param d The new direction. **/
    void setDirection(Direction d) { this->direction = d; }

	/** Changes the objective function's expression. @param e The new expression. **/
    void setExpression(Expression &e) {this->expr = e; }
    
	/** Changes the objective function's name. **/
    void setName(std::string str) { this->name = str; }

	/** Changes the objective function's id. **/
    void setId(Input::ObjectiveMetric obj) { this->id = obj; }

    /** Clear the objective function's expression. **/
    void clear() { this->expr.clear(); }
};


/********************************************************************************************
 * This class identifies an Expression in a MIP formulation. An Expression is defined by a 
 * vector of Term. 
 ********************************************************************************************/
class Constraint
{    

private:
    double lb;          /**< The constraint's lower bound. **/
    Expression expr;    /**< The constraint's expression. **/
    double ub;          /**< The constraint's upper bound. **/
    std::string name;   /**< The constraint's name. **/

public:
	Constraint(){}
    /** Constructor. @param lowerBound The constraint's lower bound. @param e The constraint's expression. @param upperBound The constraint's upper bound. @param constName The constraint's name. **/
    Constraint(double lowerBound, Expression &e, double upperBound, std::string constName="");

	/****************************************************************************************/
	/*										Getters											*/
	/****************************************************************************************/
	/** Returns the constraint's lower bound. **/
    double getLb() const { return lb; }

	/** Returns the constraint's upper bound. **/
    double getUb() const { return ub; }
    
	/** Returns the constraint's expression. **/
    Expression getExpression() const { return expr; }

	/** Returns the constraint's name. **/
    std::string getName() const { return name; }

	/** Returns the number of terms in the constraint expression. **/
    int getSize() const { return this->expr.getNbTerms(); }
	
	/****************************************************************************************/
	/*										Setters											*/
	/****************************************************************************************/
	/** Changes the constraint's lower bound. @param lowerBound The new lower bound. **/
    void setLb(double lowerBound) { this->lb = lowerBound; }

	/** Changes the constraint's upper bound. @param upperBound The new upper bound. **/
    void setUb(double upperBound) { this->ub = upperBound; }

	/** Changes the constraint's expression. @param e The new expression. **/
    void setExpression(Expression &e) {this->expr = e; }
    
    /** Clear the constraint's expression. **/
    void clear() { this->expr.clear(); }

	void display();
};


/***************************************************************************************************
 * This class identifies a Non Linear Term in an Expression. A Non Linear term is defined by a vector of Variable that are multiplied and a coefficent. 
 ***************************************************************************************************/
class NonLinearTerm
{    

private:
    double coeff;      /**< The term coefficient. **/
    std::vector<Variable> varVec;      /**< The term variable. **/

public:
    /** Constructor. @param variable The term variable. @param coefficient The term coeffiencient. **/
    NonLinearTerm(std::vector<Variable> variableVector, double coefficient);
	NonLinearTerm(const NonLinearTerm &);

	/****************************************************************************************/
	/*										Getters											*/
	/****************************************************************************************/
	/** Returns the term's coefficient. **/
    const double & getCoeff() const { return coeff; }

	/** Returns the term's variable. **/
    const std::vector<Variable> & getVarVec() const { return varVec; }

	/****************************************************************************************/
	/*										Setters											*/
	/****************************************************************************************/
	/** Changes the term's coefficient. @param coefficient The new coefficient. **/
    void setCoeff(double coefficient) { this->coeff = coefficient; }

	/** Changes the term's variable. @param v The new variable. **/
    void addVar(const Variable &v) { this->varVec.push_back(v); }
};


/********************************************************************************************
 * This class identifies an Non Linear Expression in a MIP formulation. An Non Linear Expression is defined by a 
 * vector of Non Linear Term. 
 ********************************************************************************************/
class NonLinearExpression
{    

private:
    std::vector<NonLinearTerm> nonLinearTermsArray;  /**< The array of terms. **/

public:
    /** Default constructor. **/
    NonLinearExpression(){}

    /** Copy constructor. @param e The expression to be copied. **/
    NonLinearExpression(const NonLinearExpression &e);
 
	/****************************************************************************************/
	/*										Getters											*/
	/****************************************************************************************/
	/** Returns the vector of terms defining the expression. **/
    const std::vector<NonLinearTerm> & getNonLinearTerms() const { return nonLinearTermsArray; }

	/** Returns the i-th term. @param pos Term position in the array. **/
    const NonLinearTerm & getNonLinearTerm_i(int pos) const { return nonLinearTermsArray[pos]; }

	/** Returns the number of terms in the expression. **/
	int getNbTerms() const { return nonLinearTermsArray.size(); }
	/****************************************************************************************/
	/*										Setters											*/
	/****************************************************************************************/
	/** Changes the vector of terms. @param vec The new vector of terms. **/
    void setNonLinearTerms(std::vector<NonLinearTerm> &vec) { this->nonLinearTermsArray = vec; }

	/** Changes the i-th term. @param pos Term position. @param term The new term. **/
    void setNonLinearTerm_i(int pos, NonLinearTerm &term) { this->nonLinearTermsArray[pos] = term; }

	/****************************************************************************************/
	/*										Methods											*/
	/****************************************************************************************/
	/** Adds a new term to the expression. @param term The term to be added. **/

	void addNonLinearTerm(const NonLinearTerm &term) {nonLinearTermsArray.push_back(term); }

	double getExpressionValue();

    /** Clears the array of terms. **/
    void clear() { nonLinearTermsArray.clear(); }
};


class NonLinearConstraint
{    

private:
    double lb;          /**< The constraint's lower bound. **/
    NonLinearExpression expr;    /**< The constraint's expression. **/
    double ub;          /**< The constraint's upper bound. **/
    std::string name;   /**< The constraint's name. **/

public:
	NonLinearConstraint(){}
    /** Constructor. @param lowerBound The constraint's lower bound. @param e The constraint's expression. @param upperBound The constraint's upper bound. @param constName The constraint's name. **/
    NonLinearConstraint(double lowerBound, NonLinearExpression &e, double upperBound, std::string constName="");

	/****************************************************************************************/
	/*										Getters											*/
	/****************************************************************************************/
	/** Returns the constraint's lower bound. **/
    double getLb() const { return lb; }

	/** Returns the constraint's upper bound. **/
    double getUb() const { return ub; }
    
	/** Returns the constraint's expression. **/
    NonLinearExpression getExpression() const { return expr; }

	/** Returns the constraint's name. **/
    std::string getName() const { return name; }

	/** Returns the number of terms in the constraint expression. **/
    int getSize() const { return this->expr.getNbTerms(); }
	
	/****************************************************************************************/
	/*										Setters											*/
	/****************************************************************************************/
	/** Changes the constraint's lower bound. @param lowerBound The new lower bound. **/
    void setLb(double lowerBound) { this->lb = lowerBound; }

	/** Changes the constraint's upper bound. @param upperBound The new upper bound. **/
    void setUb(double upperBound) { this->ub = upperBound; }

	/** Changes the constraint's expression. @param e The new expression. **/
    void setNonLinearExpression(NonLinearExpression &e) {this->expr = e; }
    
    /** Clear the constraint's expression. **/
    void clear() { this->expr.clear(); }

	void display() const;;

};


#endif