#include <ilcplex/ilocplex.h>
ILOSTLBEGIN
// cplex10Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <bits/stdc++.h> 
#include <chrono> 

#include <lemon/list_graph.h>
#include <lemon/concepts/graph.h>

#include "tools/clockTime.h"
#include "topology/instance.h"
#include "solver/solverFactory.h"


#include "lagrangian/solver/lagSolverFactory.h"


//#include "YoussoufForm.h"
#include "subgradient.h"


using namespace lemon;

int main(int argc, char *argv[]) {
	ClockTime GLOBAL_TIME(ClockTime::getTimeNow());
	/********************************************************************/
	/* 						Get Parameter file 							*/
	/********************************************************************/
	std::string parameterFile;
	if (argc < 2){
		std::cerr << "A parameter file is required in the arguments. PLease run the program as \n./exec parameterFile.par\n";
		throw std::invalid_argument( "did not receive an argument" );
	}
	else{
		parameterFile = argv[1];
	}
	std::cout << "PARAMETER FILE: " << parameterFile << std::endl;
	Input input(parameterFile);
	
	/********************************************************************/
	/* 				For each file of demands, optimize it 				*/
	/********************************************************************/
	std::cout << "> Number of online demand files: " << input.getNbDemandToBeRoutedFiles() << std::endl;
	for (int i = 0; i < input.getNbDemandToBeRoutedFiles(); i++) {
		ClockTime OPTIMIZATION_TIME(ClockTime::getTimeNow());
		/********************************************************************/
		/* 						Create initial mapping 						*/
		/********************************************************************/
		std::cout << "--- READING INSTANCE... --- " << std::endl;
		Instance instance(input);
		std::cout << instance.getNbRoutedDemands() << " are present in the initial mapping." << std::endl;
		
		
		/********************************************************************/
		/* 					Define set of demands to be routed 				*/
		/********************************************************************/
		//instance.displayDetailedTopology();
		std::cout << "--- READING NEW ONLINE DEMANDS... --- " << std::endl;
		std::string nextFile = instance.getInput().getDemandToBeRoutedFilesFromIndex(i);
		instance.generateDemandsFromFile(nextFile);
		//instance.generateRandomDemands(1);
		instance.displayNonRoutedDemands();
		std::cout << instance.getNbNonRoutedDemands() << " demands to be routed." << std::endl;
		
		/********************************************************************/
		/* 								Solve	 							*/
		/********************************************************************/
        ClockTime LAG1_OPTIMIZATION_TIME(ClockTime::getTimeNow());
		lagSolverFactory factory;
		AbstractLagSolver *solver = factory.createSolver(instance);
		solver->run();
		//Subgradient sub(instance);

		std::cout << "UB: "<< solver->getUB() << std::endl;
    	std::cout <<"LB: " << solver->getLB() << std::endl;
    	std::cout << "It: "<< solver->getIteration() << std::endl;
   		std::cout << "Lambda: "<< solver->getLambda() << std::endl;
    	std::cout << "Step size: "<< solver->getStepSize() << std::endl;
    	std::cout << "Get Stop: " << solver->getStop() << std::endl;
    	

		std::cout << "General Time: " << solver->getTotalTime() << std::endl;	
		std::cout << "Sub problem time: " << solver->getSolvingSubProblemTime() << std::endl;
		std::cout << "\tShortest path time: " << solver->getShorstestPathTime() << std::endl;
		std::cout << "\tUpdating variables time: " << solver->getUpdateVariablesTime() << std::endl;
		std::cout << "\tUpdating substract multipliers: " << solver->getSubstractMultipliersTime() << std::endl;
		std::cout << "\tUpdating Cost Time: " << solver->getCostTime() << std::endl;

		std::cout << "Formulation construction: " << solver->getFormulationConstTime() << std::endl;
		std::cout << "Heuristic construction: " << solver->getHeuristicConstTime() << std::endl;
		std::cout << "Initialization: " << solver->getInitializationTime() << std::endl;
		std::cout << "\tConst graph aux: " << solver->getConstAuxGraphTime() << std::endl;
		std::cout << "Slacks: " << solver->getUpdatingSlackTime() << std::endl;
		std::cout << "Bounds: " << solver->getUpdatingBoundsTime() << std::endl;
		
		std::cout << "Heuristic Bound: " << solver->getUpdatingHeuristicBoundTime() << std::endl;
		std::cout << "Multipliers: " << solver->getUpdatingMultipliersTime() << std::endl;
		std::cout << "Costs: " << solver->getUpdatingCostsTime() << std::endl;
		std::cout << "Stop criterion: " << solver->getStoppingCriterionTime() << std::endl;
		std::cout << "Primal variables: " << solver->getUpdatingPrimalVariablesTime() << std::endl;
       
      		
		std::cout << "Time taken by optimization is : ";
		std::cout << std::fixed  << LAG1_OPTIMIZATION_TIME.getTimeInSecFromStart() << std::setprecision(9); 
		std::cout << " sec" << std::endl; 

		delete solver;		
	}

	std::cout << "Total time taken by program is : ";
	std::cout << std::fixed  << GLOBAL_TIME.getTimeInSecFromStart() << std::setprecision(9); 
	std::cout << " sec" << std::endl;
	return 0;
}