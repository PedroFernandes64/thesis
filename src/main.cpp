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
			/*
			//AUXILIAR OUTPUT FILE
			std::string obj2 = to_string(input.getChosenObj_k(0));
			std::string formulation2 = to_string(input.getChosenFormulation());
			if (input.getChosenFormulation() == 2){
				formulation2 = formulation2 + to_string(input.getNonOverTFlow());
			}
			std::string maxReach2 = to_string(input.isMaxReachEnabled());
			std::string minOsnr2 = to_string(input.isOSNREnabled());
			std::string cuts2 = to_string(input.isUserCutsActivated());			
			std::ofstream outfile2;
			outfile2.open("started.csv", std::ios_base::app); // append instead of overwrite
			//output for massive experiments
			//removing junk from demand string
			std::string instanceName2 = input.getTopologyFile();
			size_t pos2 = instanceName2.find("Instances"); //find location of word
    		instanceName2.erase(0,pos2); //delete everything prior to location found
			std::string l3 = "_demands/Link.csv";
			std::string l22 = "/Links";
			std::string i2 = "Instances/";
			instanceName2.erase(instanceName2.find(l3),l3.length()); //remove l from string
			instanceName2.erase(instanceName2.find(l22),l22.length()); //remove l from string
			instanceName2.erase(instanceName2.find(i2),i2.length()); //remove i from string			
			int a2 = instanceName2.size();
			// loop to traverse in the string
			char b2 = '/';
			char c2 = ';';
			for (int i = 0; i < a2; i++) {
				if(instanceName2[i] == b2){
					instanceName2[i] = c2;
				}
			}
			//opening file and writing
  			outfile2 << "\n" + instanceName2 + ";" + obj2 +";"+ formulation2+ ";"+maxReach2+";"+
				minOsnr2+";"+cuts2<<std::flush; 
			*/
			/********************************************************************/
			/* 								Solve	 							*/
			/********************************************************************/
			SolverFactory factory;
			AbstractSolver *solver = factory.createSolver(instance);
			solver->solve();
			//solver->outputLogResults(outputCode);
			std::cout << " Time: " << solver->getDurationTime() << std::endl << std::endl;
			solver->updateRSA(instance);
			
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
			/* To be modified in the future. SolverCplex should not be instantiated in main. Scip does not offer a getAlgorithm equivalent.*/
			//std::cout << "Algo: " << ((SolverCplex*)solver)->getAlgorithm()  << std::endl;
			
			std::string ub = to_string(solver->getUpperBound());
			std::string lb = to_string(solver->getLowerBound());
			std::string gap = to_string(solver->getMipGap());
			std::string time = to_string(round(solver->getDurationTime()*1000)/1000);
			std::string obj = to_string(input.getChosenObj_k(0));
			std::string formulation = to_string(input.getChosenFormulation());
			if (input.getChosenFormulation() == 2){
				formulation = formulation + to_string(input.getNonOverTFlow());
			}
			std::string maxReach = to_string(input.isMaxReachEnabled());
			std::string minOsnr = to_string(input.isOSNREnabled());
			std::string cuts = to_string(input.isUserCutsActivated());
			std::string variables = to_string(solver->getNbVariable());
			std::string	constraints = to_string(solver->getNbConstraint());
			std::string possiblePaths =  to_string(solver->getPossiblePaths());
			std::string feasiblePaths =to_string(solver->getFeasiblePaths());
			std::string onlyOsnrFeasiblePaths =to_string(solver->getOsnrFeasiblePaths());
			std::string onlyReachFeasiblePaths = to_string(solver->getReachFeasiblePaths());
			std::string infeasiblePaths = to_string(solver->getInfeasiblePaths());

			//AUXILIAR OUTPUT FILE
			std::ofstream outfile;
			outfile.open("results.csv", std::ios_base::app); // append instead of overwrite
			//output for massive experiments
			//removing junk from demand string
			std::string instanceName = input.getTopologyFile();

			//size_t pos = instanceName.find("../4_Experiments/Outputs/Instances"); //find location of word
			size_t pos = instanceName.find("Instances"); //find location of word
    		instanceName.erase(0,pos); //delete everything prior to location found
			std::string l = "_demands";
			std::string l3 = "/Link.csv";
			std::string l2 = "/Links";
			std::string i = "Instances/";

			instanceName.erase(instanceName.find(l),l.length()); //remove l from string
			instanceName.erase(instanceName.find(l3),l3.length()); //remove l from string
			instanceName.erase(instanceName.find(l2),l2.length()); //remove l from string
			instanceName.erase(instanceName.find(i),i.length()); //remove i from string			
			int a = instanceName.size();
			// loop to traverse in the string
			char b = '/';
			char c = ';';
			for (int i = 0; i < a; i++) {
				if(instanceName[i] == b){
					instanceName[i] = c;
				}
			}
			//opening file and writing
  			outfile << "\n" + instanceName + ";" << ub + ";" + lb + ";" + gap +";" + time +";" + obj +";"+ formulation+ ";"+maxReach+";"+
				minOsnr+";"+cuts+";"+variables+";"+constraints+";"+possiblePaths+";"+feasiblePaths+";"+infeasiblePaths+";"+
				onlyOsnrFeasiblePaths+";"+onlyReachFeasiblePaths; 
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

	std::cout << "Total time taken by program is : " << std::flush;
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