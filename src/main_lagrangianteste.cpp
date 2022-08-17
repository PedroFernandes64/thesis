#include <ilcplex/ilocplex.h>
ILOSTLBEGIN
// cplex10Test.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <bits/stdc++.h> 
#include <chrono> 

#include <lemon/list_graph.h>
#include <lemon/concepts/graph.h>

#include "tools/clockTime.h"
#include "topology/instance.h"
#include "solver/solverFactory.h"

#include "lagrangian/solver/lagSolverFactory.h"


using namespace lemon;

void createFile(std::string parameterFile,std::string linkfile,std::string demandfolder,int nbdemands,
                int rl,int lagRelax,int relaxMethod,int nodeMethod,int solver, int lagFormulation,
                int heuristic,int projection,int warmstart,int alternativeStop,int directionMethod,
                double crowderParam,double carmeriniParam,double lagrangianLambda_zero,int nbIterationsWithoutImprovement,
                int maxNbIterations,int obj, std::string outputFolder){

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
        fichier << "solver=" <<solver << " "<< std::endl;
        fichier << "method=" << nodeMethod << " " << std::endl;
        fichier << "preprocessingLevel=0 " << std::endl;
        fichier << "linearRelaxation="<< rl << " " << std::endl;
        fichier << "relaxMethod="<< relaxMethod << " " << std::endl;
        fichier << "lagrangianRelaxation="<< lagRelax << " " << std::endl;
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
        fichier << std::endl;
        fichier << "lagOutputPath=" << outputFolder << " " << std::endl;
    }
    fichier.close();
}


int main(int argc, char *argv[]) {
	/**************************************************************************************************************************/
	/* 						                             Get Parameter file 							                      */
	/**************************************************************************************************************************/
	std::string parameterFile;
	if (argc < 2){
		std::cerr << "A parameter file is required in the arguments. Please run the program as \n./exec parameterFile.par objective\n";
		throw std::invalid_argument( "did not receive an argument" );
	}
	else{
		parameterFile = argv[1];
	}
	std::cout << "PARAMETER FILE: " << parameterFile << std::endl;
    std::string topology;
    if (argc < 3){
        std::cerr << "A objetive function is required in the arguments. Please run the program as \n./exec parameterFile.par objective\n";
		throw std::invalid_argument( "did not receive an argument" );
    }else{
        topology = argv[2];
    }
    int obj;
    if (argc < 4){
        std::cerr << "A objetive function is required in the arguments. Please run the program as \n./exec parameterFile.par objective\n";
		throw std::invalid_argument( "did not receive an argument" );
    }else{
        obj = std::atoi(argv[3]);
    }
    
    /*************************************************************************************************************************/
	/* 						                              Instances to test 							                     */
	/*************************************************************************************************************************/
    int n = 1; // topologias
    int m = 7; // instances

    std::string generalFolder              = "../Parameters/Instances/LagrangianTests/";
    std::string topologies[5]              = {"NSF/","Spain/","German/","ubn/","ion/"};
    topologies[0] = topology;
    std::string instances[7]               = {"50demands/","60demands/","70demands/","80demands/","90demands/","100demands/","110demands/"};

    std::string linkfile[3][7];
    for(int i=0; i<n;i++){
        for(int j=0;j<m;j++){
            linkfile[i][j]= generalFolder + topologies[i] + instances[j] + "Link.csv";
        }
    }

    std::string demandfolders[3][7];
    for(int i=0;i<n;i++){
        for(int j=0;j<m;j++){
            demandfolders[i][j] = generalFolder + topologies[i] + instances[j] + "Demands1";
        }
    }

    int numdemands[7] = {50,60,70,80,90,100,110};

    for(int i=0;i<n;i++){

        /************************  Parameters *************************/
        // We always use the shortest path heuristic
        int heuristic = 0;

        // Flow formulation
        int lagFormulation = 0;

        // Default configuration for the Subgradient
        int projection = 0;
        int warmstart = 0;
        int alternativeStop = 0;
        int directionMethod = 0;
        double crowderParam = 0.0;
        double carmeriniParam = 0.0;
        double lagrangianLambda_zero = 2.0;
        int nbIterationsWithoutImprovement = 15;
        int maxNbIterations = 150;

        // Changing parameters
        int solver;
        int rl;
        int nodeMethod;
        int lagRelax;
        int relaxMethod;
        

        /************************ File with the responses *************************/
        std::string outputFolder = generalFolder + topologies[i] + "outputs/";

        std::string nom_fichier = outputFolder + "obj_"+ std::to_string(obj) +"_general.csv";
        std::ofstream fichier(nom_fichier);
        std::string delimiter = ";";
        
        /*
        fichier << std::endl << std::endl;
        fichier << " Slices ; Demands ; MIP-UB ; MIP-LB ; MIP-GAP ; MIP-Tree-Size ; MIP-Time; " << std::endl;

        for(int j=0;j<m;j++){

            rl = 0; nodeMethod = 0; lagFormulation = 0;
            createFile(parameterFile,linkfile[i][j],demandfolders[i][j],numdemands[j],rl,nodeMethod,lagFormulation,
                        heuristic,projection,warmstart,alternativeStop,directionMethod,crowderParam,carmeriniParam,
                        lagrangianLambda_zero,nbIterationsWithoutImprovement,maxNbIterations,obj,outputFolder);
            Input input(parameterFile);

            std::cout << "--- READING INSTANCE... --- " << std::endl;
            Instance instance(input);

            std::cout << "--- READING NEW ONLINE DEMANDS... --- " << std::endl;
            std::string nextFile = instance.getInput().getDemandToBeRoutedFilesFromIndex(0);
            instance.generateDemandsFromFile(nextFile);
            */
            /********************************************************************/
            /* 				        Solve - MIP SOLUTION	 					*/
            /********************************************************************/
            /*
            std::cout << "Solving with MIP-Cplex" << std::endl;
            SolverFactory factory;
            AbstractSolver *solver = factory.createSolver(instance);
            solver->solve();

            fichier << instance.getMaxSlice() << delimiter;
            fichier << instance.getNbDemands() << delimiter;
            fichier << solver->getUpperBound() << delimiter;
            fichier << solver->getLowerBound() << delimiter;
            fichier << solver->getMipGap() << delimiter;
            fichier << solver->getTreeSize() << delimiter;
            fichier << solver->getDurationTime() << delimiter << std::endl;

            std::cout << "Mip-Cplex completed" << std::endl;

            delete solver;
        }
        */

        /*fichier << std::endl << std::endl;
        fichier << " Slices ; Demands ; RELAX-Cplex-OBJ ; RELAX-Cplex-Time ; RELAX-Cplex-algorithm "<< std::endl;

        for(relaxMethod=1;relaxMethod<=4;relaxMethod++){
            fichier << std::endl;
            fichier << "PARAMETERS " << delimiter;
            fichier << "Relax method "<< relaxMethod << delimiter << std::endl;
            for(int j=0;j<m;j++){

                rl = 1; nodeMethod = 0; lagFormulation = 0; lagRelax = 0; solver = 0;
                createFile(parameterFile,linkfile[i][j],demandfolders[i][j],numdemands[j],
                            rl,lagRelax,relaxMethod,nodeMethod,solver,lagFormulation,
                            heuristic,projection,warmstart,alternativeStop,directionMethod,crowderParam,carmeriniParam,
                            lagrangianLambda_zero,nbIterationsWithoutImprovement,maxNbIterations,obj,outputFolder);
                Input input2(parameterFile);

                std::cout << "--- READING INSTANCE... --- " << std::endl;
                Instance instance2(input2);
                
                std::cout << "--- READING NEW ONLINE DEMANDS... --- " << std::endl;
                std::string nextFile2 = instance2.getInput().getDemandToBeRoutedFilesFromIndex(0);
                instance2.generateDemandsFromFile(nextFile2);
                */
                /********************************************************************/
                /* 				        Solve - RELAXATION	 					*/
                /********************************************************************/
                /*
                std::cout << "Solving with RELAX-Cplex" << std::endl;
                SolverFactory factory2;
                AbstractSolver *solver2 = factory2.createSolver(instance2);
                solver2->solve();

                fichier << instance2.getMaxSlice() << delimiter;
                fichier << instance2.getNbDemands() << delimiter;

                fichier << solver2->getUpperBound() << delimiter;
                fichier << solver2->getDurationTime() << delimiter;
                int algo = ((SolverCplex*)solver2)->getAlgorithm() ;
                fichier << ((SolverCplex*)solver2)->getAlgorithm() << delimiter << std::endl;
                std::cout << ((SolverCplex*)solver2)->getAlgorithm() << " " << algo << std::endl;

                std::cout << "RELAX-Cplex completed "<< std::endl;

                delete solver2;
            }
        }*/
        
        
        /*fichier << std::endl << std::endl;
        fichier << " Slices ; Demands ; RELAX-CBC-OBJ ; RELAX-CBC-Time ; RELAX-CBC-algorithm "<< std::endl;

        for(relaxMethod=0;relaxMethod<=3;relaxMethod++){
            if(relaxMethod==3)
                relaxMethod=4;
            fichier << std::endl;
            fichier << "PARAMETERS " << delimiter;
            fichier << "Relax method "<< relaxMethod << delimiter << std::endl;
            for(int j=0;j<m;j++){

                rl = 1; nodeMethod = 0; lagFormulation = 0; lagRelax = 0; solver = 1;
                createFile(parameterFile,linkfile[i][j],demandfolders[i][j],numdemands[j],
                            rl,lagRelax,relaxMethod,nodeMethod,solver,lagFormulation,
                            heuristic,projection,warmstart,alternativeStop,directionMethod,crowderParam,carmeriniParam,
                            lagrangianLambda_zero,nbIterationsWithoutImprovement,maxNbIterations,obj,outputFolder);
                Input input2(parameterFile);

                std::cout << "--- READING INSTANCE... --- " << std::endl;
                Instance instance2(input2);
                
                std::cout << "--- READING NEW ONLINE DEMANDS... --- " << std::endl;
                std::string nextFile2 = instance2.getInput().getDemandToBeRoutedFilesFromIndex(0);
                instance2.generateDemandsFromFile(nextFile2);
                */
                /********************************************************************/
                /* 				        Solve - RELAXATION	 					*/
                /********************************************************************/
                /*
                std::cout << "Solving with RELAX-Cplex" << std::endl;
                SolverFactory factory2;
                AbstractSolver *solver2 = factory2.createSolver(instance2);
                solver2->solve();

                fichier << instance2.getMaxSlice() << delimiter;
                fichier << instance2.getNbDemands() << delimiter;

                fichier << solver2->getUpperBound() << delimiter;
                fichier << solver2->getDurationTime() << delimiter;
                fichier << std::endl;
                //int algo = ((SolverCplex*)solver2)->getAlgorithm() ;
                //fichier << ((SolverCplex*)solver2)->getAlgorithm() << delimiter << std::endl;
                //std::cout << ((SolverCplex*)solver2)->getAlgorithm() << " " << algo << std::endl;

                std::cout << "RELAX-CLP completed "<< std::endl;

                delete solver2;
            }
        }
        */

        fichier << std::endl << std::endl;
        fichier << " SUBGRADIENT WITH FLOW FORMULATION " << std::endl << std::endl;

        rl = 0; nodeMethod = 1; lagFormulation = 0; maxNbIterations = 5000; lagrangianLambda_zero = 2.0;
        crowderParam = 0.5; carmeriniParam = 1.5; alternativeStop = 1; lagRelax =1; relaxMethod = 0; solver =1;

        for(int itProjection = 0; itProjection <= 0; itProjection++){
            for(int itDirectionMethod = 0; itDirectionMethod <= 0; itDirectionMethod++){
                nbIterationsWithoutImprovement = 10;
                while(nbIterationsWithoutImprovement<=30){
                    fichier << std::endl;
                    fichier << "PARAMETERS " << delimiter;
                    fichier << "Projection: " << itProjection << delimiter;
                    fichier << "Direction: " << itDirectionMethod << delimiter;
                    fichier << "Alternative stop: " << alternativeStop << delimiter;
                    fichier << "Nb iterations without improvement: " << nbIterationsWithoutImprovement << delimiter;

                    fichier << std::endl << std::endl;

                    fichier << " Slices ; Demands ; UB ; LB ; Iterations ; Lambda ; Step size ; Stop Criterion ; Total Time ; ;";
                    fichier << " RSA graph construction ; Preprocessing ; Formulation Construction ; Heuristic Construction ; Initialization ; ->Auxiliar graph construction ;" ;
                    fichier << " Solving sub problem ; ->Shortest path ; ->Updating variables ; ->Updating substract multipliers ; ->Updating cost ;";
                    fichier << " Updating Slack ; Updating Bounds ; Updating heuristic bound ; Updating Multipliers ; Updating Stopping Criterion ; Updating Primal Variables ; " << std::endl;

                    for(int j = 0 ; j < m ; j++){

                        createFile(parameterFile,linkfile[i][j],demandfolders[i][j],numdemands[j],
                                    rl,lagRelax,relaxMethod,nodeMethod,solver,lagFormulation,
                                    heuristic,itProjection,warmstart,alternativeStop,itDirectionMethod,crowderParam,carmeriniParam,
                                    lagrangianLambda_zero,nbIterationsWithoutImprovement,maxNbIterations,obj,outputFolder);
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

                        fichier << instance3.getMaxSlice() << delimiter;
                        fichier << instance3.getNbDemands() << delimiter;

                        lagsolver->displayResults(fichier);

                        std::cout << "Subgradient flow formulation completed "<< std::endl;

                        delete lagsolver;
                    }
                    nbIterationsWithoutImprovement = nbIterationsWithoutImprovement + 5;
                }
            }
        }
        

       
        fichier << std::endl << std::endl;
        fichier << " VOLUME WITH FLOW FORMULATION " << std::endl << std::endl;

        rl = 0; nodeMethod = 2; lagFormulation = 0; maxNbIterations = 5000; lagrangianLambda_zero = 2.0;
        crowderParam = 0.0; carmeriniParam = 0.0; alternativeStop = 1; lagRelax =1; relaxMethod = 0; solver =1;
        nbIterationsWithoutImprovement = 10;
        while(nbIterationsWithoutImprovement<=30){
            fichier << std::endl;
            fichier << "PARAMETERS " << delimiter;
            fichier << "Alternative stop: " << alternativeStop << delimiter;
            fichier << "Nb iterations without improvement: " << nbIterationsWithoutImprovement << delimiter;

            fichier << std::endl << std::endl;

            fichier << " Slices ; Demands ; UB ; LB ; Iterations ; Lambda ; Step size ; Stop Criterion ; Total Time ; ;";
            fichier << " RSA graph construction ; Preprocessing ; Formulation Construction ; Heuristic Construction ; Initialization ; ->Auxiliar graph construction ;" ;
            fichier << " Solving sub problem ; ->Shortest path ; ->Updating variables ; ->Updating substract multipliers ; ->Updating cost ;";
            fichier << " Updating Slack ; Updating Bounds ; Updating heuristic bound ; Updating Multipliers ; Updating Stopping Criterion ; Updating Primal Variables ; " << std::endl;

            for(int j = 0 ;j < m ; j++){

                createFile(parameterFile,linkfile[i][j],demandfolders[i][j],numdemands[j],
                            rl,lagRelax,relaxMethod,nodeMethod,solver,lagFormulation,heuristic,
                            projection,warmstart,alternativeStop,directionMethod,crowderParam,carmeriniParam,lagrangianLambda_zero,
                            nbIterationsWithoutImprovement,maxNbIterations,obj,outputFolder);
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

                fichier << instance5.getMaxSlice() << delimiter;
                fichier << instance5.getNbDemands() << delimiter;

                lagsolver3->displayResults(fichier);

                std::cout << "Solving with volume and flow completed " << std::endl; 

                delete lagsolver3;
            }
            nbIterationsWithoutImprovement = nbIterationsWithoutImprovement + 5;
        }
        
        fichier << std::endl;
        fichier.close();
    }
    
	return 0;
}

