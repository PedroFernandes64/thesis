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
//#include "subgradient.h"


using namespace lemon;

void createFile(std::string parameterFile,std::string linkfile,std::string demandfolder,int nbdemands,int rl,int lagMethod,int lagFormulation,int heuristic,int projection,int warmstart,int alternativeStop,int directionMethod,double crowderParam,double carmeriniParam,double lagrangianLambda_zero,int nbIterationsWithoutImprovement, int maxNbIterations,int obj){

    double lagrangianMultiplier_zero = 0.0;

    std::ofstream fichier(parameterFile.c_str());
    if (!fichier.fail()) {
        fichier << "******* Input File Paths *******" << std::endl;;
        fichier << "topologyFile=" << linkfile <<""<< std::endl;
        fichier << "initialMappingDemandFile=" << std::endl;
        fichier << "initialMappingAssignmentFile=" << std::endl;
        fichier << "demandToBeRoutedFolder="<< demandfolder << "" << std::endl;
        fichier << std::endl;
        fichier << "******* GNPY parameters *******" << std::endl;
        fichier << "GNPY_activation=0 "<< std::endl;
        fichier << "GNPY_topologyFile=../oopt-gnpy/gnpy/example-data/spain_topo.json " << std::endl;
        fichier << "GNPY_equipmentFile=../oopt-gnpy/gnpy/example-data/spain_eqpt_config.json " << std::endl;
        fichier << std::endl;
        fichier << "******* Formulation parameters *******" << std::endl;
        fichier << "nbDemandsAtOnce=" << nbdemands << " " << std::endl;
        fichier << "formulation=0 " << std::endl;
        fichier << "userCuts=0 " << std::endl;
        fichier << "obj=" << obj<<"" << std::endl;
        fichier << "allowBlocking=0 " << std::endl;
        fichier << "hopPenalty=0 " << std::endl;
        fichier << "partitionPolicy=0 " << std::endl;
        fichier << "partitionLoad=4 " << std::endl;
        fichier << "partitionSlice=15 " << std::endl;
        fichier << std::endl;
        fichier << "******* Optimization parameters *******" << std::endl;
        fichier << "solver=0 " << std::endl;
        fichier << "method=0 " << std::endl;
        fichier << "preprocessingLevel=2 " << std::endl;
        fichier << "linearRelaxation="<< rl << " " << std::endl;
        fichier << std::endl;
        fichier << "******* Execution parameters *******" << std::endl;
        fichier << "outputPath=../Parameters/Instances/Spain_N5/InitialMappingMet1Slice30/SimpleTest/Output " << std::endl;
        fichier << "outputLevel=2 " << std::endl;
        fichier << "nbSlicesInOutputFile=320 " << std::endl;
        fichier << "globalTimeLimit=70000 " << std::endl;
        fichier << "timeLimit=7200 " << std::endl;
        fichier << std::endl;
        fichier << "******* Fields below are reserved for team LIMOS ********" << std::endl;
        fichier << "lagrangianMultiplier_zero=" << lagrangianMultiplier_zero<<" " << std::endl;
        fichier << "lagrangianLambda_zero=" << lagrangianLambda_zero <<" "<< std::endl;
        fichier << "nbIterationsWithoutImprovement=" << nbIterationsWithoutImprovement<< " " << std::endl;
        fichier << "maxNbIterations=" << maxNbIterations << " "<< std::endl;
        fichier << std::endl;
        fichier << "lagMethod=" << lagMethod << " " << std::endl;
        fichier << "lagFormulation=" << lagFormulation << " " << std::endl;
        fichier << "heuristic=" << heuristic << " " << std::endl;
        fichier << std::endl;
        fichier << "directionMethod=" << directionMethod << " " << std::endl;
        fichier << "crowderParam=" << crowderParam << " " << std::endl;
        fichier << "carmeriniParam=" << carmeriniParam << " " << std::endl;
        fichier << std::endl;
        fichier << "projection=" << projection << " " << std::endl;
        fichier << "alternativeStop=" << alternativeStop << " " << std::endl;
        fichier << "warmstart=" << warmstart << " " << std::endl;
    }
    fichier.close();
}

int main(int argc, char *argv[]) {
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

    int obj = atoi(argv[2]);

    /********************************************************************/
	/* 						Instances to test 							*/
	/********************************************************************/
    int n = 1;
    int m = 4;

    std::string generalFolder              = "../Parameters/Instances/Tests2/NSF/";
    std::string generalConf[4]             = {"10demands/","20demands/","30demands/","40demands/"};
    std::string generalInst[1]             = {"Demands2"};

    std::string linkfile[m];         
    std::string demandfolders[m][n];
    
    for(int i=0; i<m; i++)
        linkfile[i]= generalFolder + generalConf[i] + "Link.csv";

    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++)
            demandfolders[i][j] = generalFolder + generalConf[i] + generalInst[j];
    }

    int numdemands[4] = {10,20,30,40};

    /************************  Parameters *************************/
    // We always use the shortest path heuristic
    int heuristic = 0;

    // Default configuration for the Subgradient
    int projection = 0;
    int warmstart = 0;
    int alternativeStop = 0;
    int directionMethod = 0;
    double crowderParam = 0.0;
    double carmeriniParam = 0.0;
    double lagrangianLambda_zero = 2.0;
    int nbIterationsWithoutImprovement = 10;
    int maxNbIterations = 150;

    // Changing parameters
    int rl;
    int lagMethod;
    int lagFormulation;


    for(int i=0;i<m;i++){
        /************************ File with the responses *************************/
        std::string nom_fichier = generalFolder + generalConf[i] + "obj"+ std::to_string(obj) +"_general.csv";
        std::ofstream fichier(nom_fichier);
        std::string delimiter = ";";
        
        fichier << "MIP-UB;MIP-LB;MIP-GAP;MIP-Tree-Size;MIP-Time;" << std::endl;

        for(int j=0;j<n;j++){

            rl = 0; lagMethod = 0; lagFormulation = 0;
            createFile(parameterFile,linkfile[i],demandfolders[i][j],numdemands[i],rl,lagMethod,lagFormulation,heuristic,projection,warmstart,alternativeStop,directionMethod,crowderParam,carmeriniParam,lagrangianLambda_zero,nbIterationsWithoutImprovement,maxNbIterations,obj);
            Input input(parameterFile);

            std::cout << "--- READING INSTANCE... --- " << std::endl;
            Instance instance(input);

            std::cout << "--- READING NEW ONLINE DEMANDS... --- " << std::endl;
            std::string nextFile = instance.getInput().getDemandToBeRoutedFilesFromIndex(0);
            instance.generateDemandsFromFile(nextFile);

            /********************************************************************/
            /* 				        Solve - MIP SOLUTION	 					*/
            /********************************************************************/
            std::cout << "Solving with MIP-Cplex" << std::endl;
            SolverFactory factory;
            AbstractSolver *solver = factory.createSolver(instance);
            solver->solve();
            fichier << solver->getUpperBound() << delimiter;
            fichier << solver->getLowerBound() << delimiter;
            fichier << solver->getMipGap() << delimiter;
            fichier << solver->getTreeSize() << delimiter;
            fichier << solver->getDurationTime() << delimiter << std::endl;
            std::cout << "Mip-Cplex completed" << std::endl;

            delete solver;
        }

        fichier << std::endl << std::endl;
        fichier << "RELAX-OBJ;RELAX-Time;RELAX-algorithm"<<std::endl;

        for(int j=0;j<n;j++){

            rl = 1; lagMethod = 0; lagFormulation = 0;
            createFile(parameterFile,linkfile[i],demandfolders[i][j],numdemands[i],rl,lagMethod,lagFormulation,heuristic,projection,warmstart,alternativeStop,directionMethod,crowderParam,carmeriniParam,lagrangianLambda_zero,nbIterationsWithoutImprovement,maxNbIterations,obj);
            Input input2(parameterFile);

            std::cout << "--- READING INSTANCE... --- " << std::endl;
            Instance instance2(input2);
            
            std::cout << "--- READING NEW ONLINE DEMANDS... --- " << std::endl;
            std::string nextFile2 = instance2.getInput().getDemandToBeRoutedFilesFromIndex(0);
            instance2.generateDemandsFromFile(nextFile2);

            /********************************************************************/
            /* 				        Solve - RELAXATION	 					*/
            /********************************************************************/
            std::cout << "Solving with RELAX-Cplex" << std::endl;
            SolverFactory factory2;
            AbstractSolver *solver2 = factory2.createSolver(instance2);
            solver2->solve();
            fichier << solver2->getUpperBound() << delimiter;
            fichier << solver2->getDurationTime() << delimiter;
            fichier << ((SolverCplex*)solver2)->getAlgorithm() << delimiter << std::endl;
            std::cout << "RELAX-Cplex completed "<< std::endl;

            delete solver2;
        }

        fichier << std::endl << std::endl;
        fichier << "Subgradient with flow formulation" << std::endl << std::endl;
        fichier << "UB;LB;Iterations;Lambda;Step size;Stop Criterion;Total Time;;";
        fichier << "Formulation Construction;Heuristic Construction;Initialization;Auxiliar graph construction;" ;
        fichier <<"Solving sub problem;Updating Slack;Updating Bounds;Updating heuristic bound;Updating Multipliers;";
        fichier << "Updating Costs;Updating Stopping Criterion;Updating Primal Variables;"<< std::endl;

        for(int j=0;j<n;j++){

            rl = 0; lagMethod = 0; lagFormulation = 0; maxNbIterations = 150;
            createFile(parameterFile,linkfile[i],demandfolders[i][j],numdemands[i],rl,lagMethod,lagFormulation,heuristic,projection,warmstart,alternativeStop,directionMethod,crowderParam,carmeriniParam,lagrangianLambda_zero,nbIterationsWithoutImprovement,maxNbIterations,obj);
            Input input3(parameterFile);

            std::cout << "--- READING INSTANCE... --- " << std::endl;
            Instance instance3(input3);
            
            std::cout << "--- READING NEW ONLINE DEMANDS... --- " << std::endl;
            std::string nextFile3 = instance3.getInput().getDemandToBeRoutedFilesFromIndex(0);
            instance3.generateDemandsFromFile(nextFile3);
        
            /********************************************************************/
            /* 		      Solve - SUBGRADIENT LAGFLOW FORMULATION               */
            /********************************************************************/
            std::cout << "Solving with subgradient and flow formulation" << std::endl;
            lagSolverFactory lagfactory;
            AbstractLagSolver *lagsolver = lagfactory.createSolver(instance3);
            lagsolver->run();
            lagsolver->displayResults(fichier);
            std::cout << "Subgradient flow formulation completed "<< std::endl;

            delete lagfactory;
        }

        fichier << std::endl << std::endl;
        fichier << "Subgradient with overlapping formulation" << std::endl << std::endl;
        fichier << "UB;LB;Iterations;Lambda;Step size;Stop Criterion;Total Time;;";
        fichier << "Formulation Construction;Heuristic Construction;Initialization;Auxiliar graph construction;" ;
        fichier <<"Solving sub problem;Updating Slack;Updating Bounds;Updating heuristic bound;Updating Multipliers;";
        fichier << "Updating Costs;Updating Stopping Criterion;Updating Primal Variables;"<< std::endl;

        for(int j=0;j<n;j++){

            rl = 0; lagMethod = 0; lagFormulation = 1; maxNbIterations = 300;
            createFile(parameterFile,linkfile[i],demandfolders[i][j],numdemands[i],rl,lagMethod,lagFormulation,heuristic,projection,warmstart,alternativeStop,directionMethod,crowderParam,carmeriniParam,lagrangianLambda_zero,nbIterationsWithoutImprovement,maxNbIterations,obj);
            Input input4(parameterFile);

            std::cout << "--- READING INSTANCE... --- " << std::endl;
            Instance instance4(input4);
		
            std::cout << "--- READING NEW ONLINE DEMANDS... --- " << std::endl;
            std::string nextFile4 = instance4.getInput().getDemandToBeRoutedFilesFromIndex(0);
            instance4.generateDemandsFromFile(nextFile4);

            /********************************************************************/
            /* 		Solve - SUBGRADIENT LAGNONOVERLAP FORMULATION      	 		*/
            /********************************************************************/
            std::cout << "Solving with subgradient non overlap formualtion" << std::endl;
            lagSolverFactory lagfactory2;
            AbstractLagSolver *lagsolver2 = lagfactory2.createSolver(instance4);
            lagsolver2->run();
            lagsolver2->displayResults(fichier);
            std::cout << "subgradient non overlapping completed "<<std::endl;

            delete lagsolver2;

        }

        /*fichier << std::endl << std::endl;
        fichier << "Subgradient with overlap formulation" << std::endl << std::endl;
        fichier << "UB;LB;Iterations;Lambda;Step size;Stop Criterion;Total Time;;";
        fichier << "Formulation Construction;Heuristic Construction;Initialization;Auxiliar graph construction;" ;
        fichier <<"Solving sub problem;Updating Slack;Updating Bounds;Updating heuristic bound;Updating Multipliers;";
        fichier << "Updating Costs;Updating Stopping Criterion;Updating Primal Variables;"<< std::endl;

        for(int j=0;j<n;j++){

            rl = 0; lagMethod = 0; lagFormulation = 2; maxNbIterations = 300;
            createFile(parameterFile,linkfile[i],demandfolders[i][j],numdemands[i],rl,lagMethod,lagFormulation,heuristic,projection,warmstart,alternativeStop,directionMethod,crowderParam,carmeriniParam,lagrangianLambda_zero,nbIterationsWithoutImprovement,maxNbIterations,obj);
            Input input_aux(parameterFile);

            std::cout << "--- READING INSTANCE... --- " << std::endl;
            Instance instance_aux(input_aux);
		
            std::cout << "--- READING NEW ONLINE DEMANDS... --- " << std::endl;
            std::string nextFile_aux = instance_aux.getInput().getDemandToBeRoutedFilesFromIndex(0);
            instance_aux.generateDemandsFromFile(nextFile_aux);
            */
            /********************************************************************/
            /* 		Solve - SUBGRADIENT LAGNONOVERLAP FORMULATION      	 		*/
            /********************************************************************/
            /*std::cout << "Solving with subgradient non overlap formualtion" << std::endl;
            lagSolverFactory lagfactory_aux;
            AbstractLagSolver *lagsolver_aux = lagfactory_aux.createSolver(instance_aux);
            lagsolver_aux->run();
            lagsolver_aux->displayResults(fichier);
            std::cout << "subgradient non overlap completed "<<std::endl;

        }*/

        fichier << std::endl << std::endl;
        fichier << "Volume with flow formulation" << std::endl << std::endl;
        fichier << "UB;LB;Iterations;Lambda;Step size;Stop Criterion;Total Time;;";
        fichier << "Formulation Construction;Heuristic Construction;Initialization;Auxiliar graph construction;" ;
        fichier <<"Solving sub problem;Updating Slack;Updating Bounds;Updating heuristic bound;Updating Multipliers;";
        fichier << "Updating Costs;Updating Stopping Criterion;Updating Primal Variables;"<< std::endl;

        for(int j=0;j<n;j++){

            rl = 0; lagMethod = 1; lagFormulation = 0; maxNbIterations = 150;
            createFile(parameterFile,linkfile[i],demandfolders[i][j],numdemands[i],rl,lagMethod,lagFormulation,heuristic,projection,warmstart,alternativeStop,directionMethod,crowderParam,carmeriniParam,lagrangianLambda_zero,nbIterationsWithoutImprovement,maxNbIterations,obj);
            Input input5(parameterFile);

            std::cout << "--- READING INSTANCE... --- " << std::endl;
            Instance instance5(input5);
            
            std::cout << "--- READING NEW ONLINE DEMANDS... --- " << std::endl;
            std::string nextFile5 = instance5.getInput().getDemandToBeRoutedFilesFromIndex(0);
            instance5.generateDemandsFromFile(nextFile5);
        
            /********************************************************************/
            /* 		      Solve - VOLUME LAGFLOW FORMULATION               */
            /********************************************************************/
            std::cout << "Solving with volume and flow formulation" << std::endl;
            lagSolverFactory lagfactory3;
            AbstractLagSolver *lagsolver3 = lagfactory3.createSolver(instance5);
            lagsolver3->run();
            lagsolver3->displayResults(fichier);
            std::cout << "Solving with volume and flow completed " << std::endl; 

            delete lagsolver3;
        }

        fichier << std::endl << std::endl;
        fichier << "Volume with overlapping formulation" << std::endl << std::endl;
        fichier << "UB;LB;Iterations;Lambda;Step size;Stop Criterion;Total Time;;";
        fichier << "Formulation Construction;Heuristic Construction;Initialization;Auxiliar graph construction;" ;
        fichier <<"Solving sub problem;Updating Slack;Updating Bounds;Updating heuristic bound;Updating Multipliers;";
        fichier << "Updating Costs;Updating Stopping Criterion;Updating Primal Variables;"<< std::endl;

        
        for(int j=0;j<n;j++){
            rl = 0; lagMethod = 1; lagFormulation = 1; maxNbIterations = 300;
            createFile(parameterFile,linkfile[i],demandfolders[i][j],numdemands[i],rl,lagMethod,lagFormulation,heuristic,projection,warmstart,alternativeStop,directionMethod,crowderParam,carmeriniParam,lagrangianLambda_zero,nbIterationsWithoutImprovement,maxNbIterations,obj);
            Input input6(parameterFile);

            std::cout << "--- READING INSTANCE... --- " << std::endl;
            Instance instance6(input6);
            
            std::cout << "--- READING NEW ONLINE DEMANDS... --- " << std::endl;
            std::string nextFile6 = instance6.getInput().getDemandToBeRoutedFilesFromIndex(0);
            instance6.generateDemandsFromFile(nextFile6);

            /********************************************************************/
            /* 		Solve - VOLUME LAGNONOVERLAP FORMULATION      	 		*/
            /********************************************************************/
            std::cout << "Solving with volume non overlap formualtion" << std::endl;
            lagSolverFactory lagfactory4;
            AbstractLagSolver *lagsolver4 = lagfactory4.createSolver(instance6);
            lagsolver4->run();
            lagsolver4->displayResults(fichier);
            std::cout << "Solving with volume non overlapping completed "<< std::endl; 

            delete lagsolver4;
        } 

        /*fichier << std::endl << std::endl;
        fichier << "Volume with overlap formulation" << std::endl << std::endl;
        fichier << "UB;LB;Iterations;Lambda;Step size;Stop Criterion;Total Time;;";
        fichier << "Formulation Construction;Heuristic Construction;Initialization;Auxiliar graph construction;" ;
        fichier <<"Solving sub problem;Updating Slack;Updating Bounds;Updating heuristic bound;Updating Multipliers;";
        fichier << "Updating Costs;Updating Stopping Criterion;Updating Primal Variables;"<< std::endl;

        
        for(int j=0;j<n;j++){
            rl = 0; lagMethod = 1; lagFormulation = 2; maxNbIterations = 300;
            createFile(parameterFile,linkfile[i],demandfolders[i][j],numdemands[i],rl,lagMethod,lagFormulation,heuristic,projection,warmstart,alternativeStop,directionMethod,crowderParam,carmeriniParam,lagrangianLambda_zero,nbIterationsWithoutImprovement,maxNbIterations,obj);
            Input input_aux2(parameterFile);

            std::cout << "--- READING INSTANCE... --- " << std::endl;
            Instance instance_aux2(input_aux2);
            
            std::cout << "--- READING NEW ONLINE DEMANDS... --- " << std::endl;
            std::string nextFile_aux2 = instance_aux2.getInput().getDemandToBeRoutedFilesFromIndex(0);
            instance_aux2.generateDemandsFromFile(nextFile_aux2);
            */
            /********************************************************************/
            /* 		Solve - VOLUME LAGNONOVERLAP FORMULATION      	 		*/
            /********************************************************************/
            /*std::cout << "Solving with volume non overlap formualtion" << std::endl;
            lagSolverFactory lagfactory_aux2;
            AbstractLagSolver *lagsolver_aux2 = lagfactory_aux2.createSolver(instance_aux2);
            lagsolver_aux2->run();
            lagsolver_aux2->displayResults(fichier);
            std::cout << "Solving with volume non overlap completed "<< std::endl; 
        } */

        fichier << std::endl;
        fichier.close();
    }
	return 0;
}

