#ifndef __abstractSolver__h
#define __abstractSolver__h

#include "../formulation/formulationFactory.h"

#define EPS 1e-4
#define EPSILON 1e-10

//typedef IloArray<IloNumVarArray> IloNumVarMatrix;
typedef IloArray<IloBoolVarArray> IloBoolVarMatrix;


/**********************************************************************************************
 * This is an abstract class modelling a MIP solver. Derived classes need to specify how the 
 * RSA formulation is solved, i.e., which MIP solved is used.
 * *******************************************************************************************/
class AbstractSolver{

public:
	/** Enumerates the possible algorithm status according to the current model and solution in hand. **/
	enum Status {
        STATUS_UNKNOWN	= 0,        /**< The algorithm has no information about the solution of the model. **/					
		STATUS_FEASIBLE = 1,        /**< The algorithm found a feasible solution that may not necessarily be optimal. **/
		STATUS_OPTIMAL = 2,         /**< The algorithm found an optimal solution. **/
        STATUS_INFEASIBLE = 3,      /**< The algorithm proved the model infeasible; that is, it is not possible to find a feasible solution. **/
        STATUS_UNBOUNDED = 4,       /**< The algorithm proved the model unbounded. **/
        STATUS_INFEASIBLE_OR_UNBOUNDED = 5, /**< The model is infeasible or unbounded. **/
        STATUS_ERROR = 6            /**< An error occurred. **/
	};
protected:
	AbstractFormulation *formulation;
	Status currentStatus;
	double time;
	double upperBound;
	double lowerBound;
	double gap;
	int treeSize;
	double rootValue;

	double totalChargeTime;
	double varChargeTime;
	double constChargeTime;
	double objChargeTime;

	double totalImpleTime;
	double varImpleTime;
	double constImpleTime;
	double cutImpleTime;
	double objImpleTime;

public:
	/****************************************************************************************/
	/*										Constructor										*/
	/****************************************************************************************/

	/** Constructor. The RSA constructor is called and the arc map storing the index of the preprocessed graphs associated is built. @param inst The instance to be solved. **/
    AbstractSolver(const Instance &instance, const Status &s = STATUS_UNKNOWN);

	/****************************************************************************************/
	/*											Getters										*/
	/****************************************************************************************/
	/** Returns the status. **/
    virtual Status getStatus() = 0;
	
	double getDurationTime() const { return time; }
	double getMipGap() const { return gap; }
	double getUpperBound() const { return upperBound; }
	double getLowerBound() const { return lowerBound; }
	int getTreeSize() const { return treeSize; }
	double getRootValue() const { return rootValue; }

	double getTotalChargeTime() const { return totalChargeTime;}
	double getVarChargeTime() const { return varChargeTime;}
	double getConstChargeTime() const { return constChargeTime;}
	double getObjChargeTime() const { return objChargeTime;}

	double getTotalImpleTime() {return totalImpleTime;}
	double getVarImpleTime(){ return varImpleTime;}
	double getConstImpleTime() { return constImpleTime;}
	double getCutImpleTime() { return cutImpleTime;}
	double getObjImpleTime() { return objImpleTime;}

	/****************************************************************************************/
	/*											Setters										*/
	/****************************************************************************************/
	/** Changes the status. **/
    void setStatus(const Status &s){ currentStatus = s; }

	void setDurationTime(const double t) { time = t; }
	void setMipGap(const double g) { gap = g; }
	void setMipGap(const double bestBound, const double bestInteger) { gap = (std::abs(bestBound - bestInteger)/(EPSILON + std::abs(bestInteger))); }
	void setUpperBound(const double ub) { upperBound = ub; }
	void setLowerBound(const double lb) { lowerBound = lb; }
	void setTreeSize(const int t) { treeSize = t; }
	void setRootValue(const double v) { rootValue = v; }
	
	/****************************************************************************************/
	/*											Methods										*/
	/****************************************************************************************/
	virtual void solve() = 0;
	
	virtual void implementFormulation() = 0;

	void updateRSA(Instance &instance);

	virtual std::vector<double> getSolution() = 0;

	/* Builds file results.csv containing information about the main obtained results. */
	virtual void outputLogResults(std::string fileName){}

	virtual ~AbstractSolver(){}

};    
#endif