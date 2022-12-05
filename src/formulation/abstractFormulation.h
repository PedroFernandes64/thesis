#ifndef __abstractFormulation__h
#define __abstractFormulation__h

#include "rsa.h"
#include "formulationComponents.h"



typedef std::vector<Variable> VarArray;

/*********************************************************************************************
* This class implements an abstract MIP formulation for modelling the Routing and Spectrum 
* Allocation using the data structures defined in FormulationComponents.h.	
*********************************************************************************************/
class AbstractFormulation : public RSA{

protected:
    int nbVar;                                      /**< The total number of variables. **/
    
    std::vector<Constraint> constraintSet;			/**< The set of constraints. **/
    std::vector<Constraint> cutPool;				/**< The set of cuts. **/
    std::vector<ObjectiveFunction> objectiveSet;	/**< The set of objectives to be optimized (in order). **/
	double upperBound;

	double totalImpleTime;
	double varImpleTime;
	double constImpleTime;
	double cutImpleTime;
	double objImpleTime;

public:
	/****************************************************************************************/
	/*										Constructors									*/
	/****************************************************************************************/
	/** Constructor. Builds the Formulation.  @param instance The instance to be solved. **/
    AbstractFormulation(const Instance &instance): RSA(instance), nbVar(0){}

	double getTotalImpleTime() {return totalImpleTime;}
	double getVarImpleTime(){ return varImpleTime;}
	double getConstImpleTime() { return constImpleTime;}
	double getCutImpleTime() { return cutImpleTime;}
	double getObjImpleTime() { return objImpleTime;}

	/****************************************************************************************/
	/*										Variables										*/
	/****************************************************************************************/
	/** Returns the total number of variables. **/
    int getNbVar() const { return nbVar; }

	/** Increments the total number of variables. **/
    void incNbVar() { nbVar++; }


	/** Puts all variables into a single array of variables and returns it. @note The position of a variable in the array is given by its id. **/
	virtual VarArray getVariables() = 0;

	/** Defines the decision variables need in the MIP formulation. **/
    virtual void setVariables() = 0;

	/** Changes the variable values. @param value The vector of values. **/
	virtual void setVariableValues(const std::vector<double> &value) = 0;

	/****************************************************************************************/
	/*										Constraints										*/
	/****************************************************************************************/
	/** Returns the set of constraints. **/
	const std::vector<Constraint> & getConstraints(){ return constraintSet; }

	/** Clears the set of constraints. **/
	void clearConstraints(){ constraintSet.clear(); }


	/** Defines the set of constraints. **/
    virtual void setConstraints() = 0;
	/** Defines the pool of cuts. **/
    virtual void setCutPool() { std::cout << "WARNING: Cut pool is empty." << std::endl; }

	/****************************************************************************************/
	/*									Additional Cuts										*/
	/****************************************************************************************/
	/** Solves the separation problem for fractional points. If the solution is violated, return the constraint that cuts it, otherwise the constraint returned has an empty expression. **/
    virtual std::vector<Constraint> solveSeparationProblemFract(const std::vector<double> &solution){ 
		std::cout << "WARNING: Unimplemented fractional separation problem!" << std::endl;
    	std::vector<Constraint> cuts;
    	return cuts;
    }
	
	/** Solves the separation problem for integer points. If the solution is violated, return the constraint that cuts it, otherwise the constraint returned has an empty expression. **/
    virtual std::vector<Constraint> solveSeparationProblemInt(const std::vector<double> &solution, const int threadNo){ 
		std::cout << "WARNING: Unimplemented integer separation problem!" << std::endl;Expression exp;
		std::vector<Constraint> cuts;
		return cuts;
    }
	/** Solves the separation problem for gnpy. If the solution is violated, return the constraints that cuts it, otherwise the returned vector is empty. **/
    virtual std::vector<Constraint> solveSeparationGnpy(const std::vector<double> &solution, const int threadNo){ 
		std::cout << "WARNING: Unimplemented gnpy separation problem!" << std::endl;Expression exp;
		std::vector<Constraint> cuts;
		return cuts;
    }

	/** Solves the separation problem for GNMODEL. If the solution is violated, return the constraints that cuts it, otherwise the returned vector is empty. **/
    
	virtual std::vector<Constraint> solveSeparationGNModel(const std::vector<double> &solution, const int threadNo){ 
		std::cout << "WARNING: Unimplemented GN MODEL separation problem!" << std::endl;Expression exp;
		std::vector<Constraint> cuts;
		return cuts;
    }

	/****************************************************************************************/
	/*									Objective Functions									*/
	/****************************************************************************************/

	/** Returns the i-th objective function. @param i The objective function index. **/
    const ObjectiveFunction & getObjFunction(int i){ return objectiveSet[i]; }

	/** Returns the set of objective functions. **/
    std::vector<ObjectiveFunction> getObjectiveSet(){ return objectiveSet; }

	/** Returns the number of objective functions to be treated. **/
    int getNbObjectives(){ return objectiveSet.size(); }


	/** Defines the objective function. **/
    virtual void setObjectives() = 0;

	/****************************************************************************************/
	/*										Methods											*/
	/****************************************************************************************/

	/** Recovers the obtained MIP solution and builds a path for each demand on its associated graph from RSA. **/
    virtual void updatePath(const std::vector<double> &vals) = 0;
	
	
	/****************************************************************************************/
	/*									Variable Fixing										*/
	/****************************************************************************************/

	/** Returns a set of variables to be fixed to 0 according to the current upper bound. **/
    virtual std::vector<Variable> objective8_fixing(const double upperBound) = 0;

	/****************************************************************************************/
	/*										Display											*/
	/****************************************************************************************/

	/** Displays the value of each variable in the obtained solution. **/
    virtual void displayVariableValues() = 0;

	/****************************************************************************************/
	/*										Destructor										*/
	/****************************************************************************************/

	/** Destructor. Clears the variable matrices, cplex model and environment. **/
	virtual ~AbstractFormulation(){
        constraintSet.clear();
        objectiveSet.clear();
    }

};


#endif