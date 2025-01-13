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
			if(input.getChosenObj().size()>1){
				obj = obj + "-" + to_string(input.getChosenObj_k(1));
			}
			std::string formulation = to_string(input.getChosenFormulation());
			if (input.getChosenFormulation() == 2){
				formulation = formulation + to_string(input.getNonOverTFlow());
			}
			std::string maxCD = to_string(input.isMaxCDEnabled());
			std::string minOsnr = to_string(input.isMinOSNREnabled());
			std::string gnpy = to_string(input.isGNPYEnabled());
			std::string bands = to_string(input.getNbBands());
			std::string reinforcements = to_string(input.areReinforcementsEnabled());
			std::string cuts = to_string(input.isUserCutsActivated());
			std::string prepro = to_string(input.getChosenPreprLvl());
			std::string genetic = to_string(input.activateGeneticAlgorithm());
			std::string it = to_string(input.geneticAlgorithmIterations());
			std::string pop = to_string(input.geneticAlgorithmPopulation());
			std::string cross = to_string(input.geneticAlgorithmCrossing());
			std::string mut = to_string(input.geneticAlgorithmMutation());
			std::string cK = to_string(input.geneticGetChosenK());
			std::string eK = to_string(input.geneticGetExtraK());
	
			std::string genTime= to_string(solver->getHeuristicTime());
			std::string genSol= to_string(solver->getBestSol());
			std::string genSolIt= to_string(solver->getItToBest());
			std::string genSolTime= to_string(solver->getTimeToBest());
			std::string preproTime = to_string(solver->getPreprocessingTime());
			std::string variables = to_string(solver->getNbVariable());
			std::string	constraints = to_string(solver->getNbConstraint());
			std::string possiblePaths =  to_string(solver->getPossiblePaths());
			std::string feasiblePathsC =to_string(solver->getFeasiblePathsC());
			std::string onlyOsnrFeasiblePathsC =to_string(solver->getOsnrFeasiblePathsC());
			std::string onlyReachFeasiblePathsC = to_string(solver->getReachFeasiblePathsC());
			std::string infeasiblePathsC = to_string(solver->getInfeasiblePathsC());
			std::string feasiblePathsL =to_string(solver->getFeasiblePathsL());
			std::string onlyOsnrFeasiblePathsL =to_string(solver->getOsnrFeasiblePathsL());
			std::string onlyReachFeasiblePathsL = to_string(solver->getReachFeasiblePathsL());
			std::string infeasiblePathsL = to_string(solver->getInfeasiblePathsL());
			std::string	ads = to_string(solver->getADS());
			std::string	dcb = to_string(solver->getDCB());
			std::string	llb = to_string(solver->getLLB());
			std::string	nlus = to_string(solver->getNLUS());
			std::string	slus = to_string(solver->getSLUS());
			std::string	suld = to_string(solver->getSULD());
			std::string	trl = to_string(solver->getTRL());
			std::string	tus = to_string(solver->getTUS());
			std::string	tase = to_string(solver->getTASE());
			std::string	v0 = to_string(solver->getVariablesSetTo0());
			std::string	ppC = to_string(solver->getPreprocessingConstraints());

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
  			outfile << "\n" + instanceName + ";" << ub + ";" + lb + ";" + gap +";" + time +";" + obj +";"+ formulation+ ";"
				+maxCD+";" + minOsnr+";"+ gnpy+";"+ bands+";"+ reinforcements+";"+ cuts+";"+ prepro+";"+genetic+";"+it+";"
				+pop+";"+cross+";"+mut+";"+cK+";"+eK+";"+genTime+";"+genSol+";"+genSolIt+";"+genSolTime+";"+preproTime +";"
				+variables+";"+constraints+";"+v0+";"+ppC+";"+possiblePaths+";"+feasiblePathsC+";"+infeasiblePathsC+";"
				+onlyOsnrFeasiblePathsC+";"+onlyReachFeasiblePathsC+";"+feasiblePathsL+";"+infeasiblePathsL+";"+onlyOsnrFeasiblePathsL+";"
				+onlyReachFeasiblePathsL+";"+ads+";"+dcb+";"+llb+";"+nlus+";"+slus+";"+suld+";"+trl+";"+tus+";"+tase;
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