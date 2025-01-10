#ifndef __callbackCplex__h
#define __callbackCplex__h

#include <ilcplex/ilocplex.h>
#include "abstractSolver.h"
/************************************************************************************
 * This is the class implementing the generic callback interface. It has two main 
 * functions: addUserCuts and addLazyConstraints.												
 ************************************************************************************/
class CplexCallback: public IloCplex::Callback::Function {
private:
	IloNumVarArray var;
	AbstractFormulation *formulation;
	const Input& input;
	double upperBound;
	const bool obj8;

public:
	// Constructor with data.
	CplexCallback(const IloNumVarArray _var, AbstractFormulation* &_formulation, const Input& _input, bool _obj8);

	void addUserCuts (const IloCplex::Callback::Context &context) const; 
    void getRelaxVars (const IloCplex::Callback::Context &context) const; 
    
    void addLazyConstraints(const IloCplex::Callback::Context &context) const;
    void addGnpyConstraints(const IloCplex::Callback::Context &context) const;
	void fixVariables(const IloCplex::Callback::Context &context);

	//PEDRO PEDRO
	void getBound(const IloCplex::Callback::Context &context) const;
    
	void setUpperBound(double ub){ upperBound = ub; }
	double getUpperBound() const{ return upperBound; }
	bool isObj8() const{ return obj8; }

	virtual void invoke (const IloCplex::Callback::Context &context);

	std::vector<double> getIntegerSolution(const IloCplex::Callback::Context &context) const;
	std::vector<double> getFractionalSolution(const IloCplex::Callback::Context &context) const;
	IloExpr to_IloExpr(const IloCplex::Callback::Context &context, const Expression &e) const;
    // Destructor
    virtual ~CplexCallback();
};

#endif