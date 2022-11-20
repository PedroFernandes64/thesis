#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

#include <bits/stdc++.h> 
#include <chrono> 

#include <lemon/list_graph.h>
#include <lemon/concepts/graph.h>

#include "tools/clockTime.h"
#include "topology/instance.h"
#include "solver/solverFactory.h"

using namespace lemon;

int main(int argc, char *argv[]) {
	std::cout << "PIKA DE CRISTAL 33: " << std::endl;
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
		std::cout << "--- READING NEW DEMANDS TO BE ROUTED... --- " << std::endl;
		std::string nextFile = instance.getInput().getDemandToBeRoutedFilesFromIndex(i);
		instance.generateDemandsFromFile(nextFile);
		//instance.generateRandomDemands(1);
		instance.displayNonRoutedDemands();
		std::cout << instance.getNbNonRoutedDemands() << " demands to be routed." << std::endl;

		
		/********************************************************************/
		/* 							Start optimizing 						*/
		/********************************************************************/
		int optimizationCounter = 0;
		std::string outputCode = getInBetweenString(nextFile, "/", ".") + "_" + std::to_string(optimizationCounter);
		//instance.output(outputCode);
		bool feasibility = true;
		bool lastIterationFeas = true;
		
		while(instance.getNextDemandToBeRoutedIndex() < instance.getNbDemands() && feasibility == true && (instance.getInput().getOptimizationTimeLimit() >= OPTIMIZATION_TIME.getTimeInSecFromStart())){
			
			/********************************************************************/
			/* 							Initialization	 						*/
			/********************************************************************/
			optimizationCounter++;
			outputCode = getInBetweenString(nextFile, "/", ".") + "_" + std::to_string(optimizationCounter);
			ClockTime ITERATION_TIME(ClockTime::getTimeNow());
			if ((instance.getInput().getOptimizationTimeLimit() - OPTIMIZATION_TIME.getTimeInSecFromStart()) < instance.getInput().getIterationTimeLimit()){
				instance.setTimeLimit(std::max(0, instance.getInput().getOptimizationTimeLimit() - (int)OPTIMIZATION_TIME.getTimeInSecFromStart()));
			}
			
			/********************************************************************/
			/* 								Solve	 							*/
			/********************************************************************/
			SolverFactory factory;
			AbstractSolver *solver = factory.createSolver(instance);
			solver->solve();
			
			/*PEDRO PEDRO PEDRO*/
			//solver->outputLogResults(outputCode);
			/*PEDRO PEDRO PEDRO*/

			std::cout << " Time: " << solver->getDurationTime() << std::endl << std::endl;
			solver->updateRSA(instance);
			
			//PEDRO PEDRO PEDRO
			//std::cout << "GN MODEL ANALYSIS" << std::endl;
			//instance.followPaths();
			//Aqui, no objeto instance, posso consultar cada edge da rede, em cada edge da rede posso consultar como esta a ocupação dos slices"
			//PEDRO PEDRO PEDRO

			/********************************************************************/
			/* 							Finalization							*/
			/********************************************************************/
			if (instance.getInput().getChosenOutputLvl() == Input::OUTPUT_LVL_DETAILED){
				instance.output(outputCode);
			}
			if (instance.getInput().isBlockingAllowed() == false && instance.getWasBlocked() == true){
				feasibility = false;
			}
			std::cout << "Time taken by iteration is : ";
			std::cout << std::fixed  << ITERATION_TIME.getTimeInSecFromStart() << std::setprecision(9); 
			std::cout << " sec" << std::endl; 

			std::cout << "UB: " << solver->getUpperBound() << std::endl;
			std::cout << "LB: " << solver->getLowerBound() << std::endl;
			std::cout << "GAP: " << solver->getMipGap() << std::endl;
			std::cout << "Tree size: " << solver->getTreeSize() << std::endl;
			std::cout << "Time: " << solver->getDurationTime() << std::endl << std::endl;
			std::cout << "Algo: " << ((SolverCplex*)solver)->getAlgorithm()  << std::endl;
		}
		
		/********************************************************************/
		/* 								Output	 							*/
		/********************************************************************/
		if (instance.getInput().getChosenOutputLvl() >= Input::OUTPUT_LVL_NORMAL){
			outputCode = getInBetweenString(nextFile, "/", ".");
			instance.output(outputCode + "_FINAL");
			instance.outputLogResults(outputCode, OPTIMIZATION_TIME.getTimeInSecFromStart());
		}
		instance.displayAllDemands();
		std::cout << "Time taken by optimization is : ";
		std::cout << std::fixed  << OPTIMIZATION_TIME.getTimeInSecFromStart() << std::setprecision(9); 
		std::cout << " sec" << std::endl; 
	}

	std::cout << "Total time taken by program is : ";
	std::cout << std::fixed  << GLOBAL_TIME.getTimeInSecFromStart() << std::setprecision(9); 
	std::cout << " sec" << std::endl;

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file