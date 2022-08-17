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

using namespace lemon;


void createFile(std::string parameterFile,std::string linkfile,std::string demandfolder,int nbdemands,int lagMethod,int lagFormulation,int heuristic,int projection,int warmstart,int alternativeStop,int directionMethod,double crowderParam,double carmeriniParam,double lagrangianLambda_zero,int nbIterationsWithoutImprovement, int maxNbIterations){

    int obj = 1;
    double lagrangianMultiplier_zero = 0.0;

    std::ofstream fichier(parameterFile.c_str());
    if (!fichier.fail()) {
        fichier << "******* Input File Paths *******" << std::endl;;
        fichier << "topologyFile=" << linkfile <<" "<< std::endl;
        fichier << "initialMappingDemandFile= " << std::endl;
        fichier << "initialMappingAssignmentFile= " << std::endl;
        fichier << "demandToBeRoutedFolder="<< demandfolder << " " << std::endl;
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
        fichier << "obj=" <<obj<<" " << std::endl;
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
        fichier << "linearRelaxation=0 " << std::endl;
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

    /********************************************************************/
	/* 						Instances to test 							*/
	/********************************************************************/
    int n =1;
    std::string aux              = "../Parameters/Instances/Benchmark/leipzig/spain_21nodes_35links/15demands";//"../Parameters/Tests/NSF/10demands";
    std::string linkfile         = "../Parameters/Instances/Benchmark/leipzig/spain_21nodes_35links/15demands/Link.csv";//"../Parameters/Tests/NSF/Link.csv";
    std::string demandfolders[n] = {aux+"Demands"};//{aux+"/Demands1"};//,aux+"/Demands2",aux+"/Demands3",aux+"/Demands4",aux+"/Demands5"};//,aux+"/Demands6",aux+"/Demands7",aux+"/Demands8",aux+"/Demands9",aux+"/Demands10"};
    int numdemands[n]            = {10};//,10,10,10,10};//,10,10,10,10,10};

    /********************************************************************/
	/* 						LAGRANGIAN TESTES 							*/
	/********************************************************************/
    int lagMethod = 0;

    // we always use the shortest path heuristic
    int heuristic = 0;

    // maximum number of iterations 
    int maxNbIterations = 300; 

    // labels 
    std::string labels = "Crowder;Carmerini;Lambda zero; It without Improv;UB;LB;Number of iterations;Stopping criterion; Total Time\n";
    for(int i = 0; i<n; i++){
        std::string dossier;
        for(int lagFormulation=0; lagFormulation<=1; lagFormulation++){
            if(lagFormulation==0){
                dossier =  "/lagFlow/";
            }else{
                dossier = "/lagOverlap/";
            }
            for(int alternativeStop = 0; alternativeStop <= 1; alternativeStop++){ // Alternative stop
                if(lagFormulation==1){
                    for(int projection = 0; projection<=2;projection++){ // Projection Method
                        for(int warmstart=0;warmstart<=1;warmstart++){
                            for(int directionMethod = 0; directionMethod <= 3; directionMethod++){ // Direction Method
                                std::string nom_fichier = aux + dossier + "Instance" + std::to_string(i+1) +"/proj" + std::to_string(projection) + "_warmstart"+ std::to_string(warmstart) + "_altStop" +std::to_string(alternativeStop) + "_dirMethod" + std::to_string(directionMethod) +"2.csv";
                                std::cout << nom_fichier << std::endl;
                                std::ofstream fichier(nom_fichier);
                                fichier << "Max It: " << std::to_string(maxNbIterations) << "; Projection: " + std::to_string(projection) + "; Warmstart: "+ std::to_string(warmstart) + "; AlternativeStop: " +std::to_string(alternativeStop) + "; DirectionMethod: " + std::to_string(directionMethod) + "\n";
                                fichier << labels;
                                if(directionMethod == 1){
                                    double crowderParam=0.3;
                                    while(crowderParam<=0.901){
                                        double lagrangianLambda_zero =0.5;
                                        while(lagrangianLambda_zero<=2.001){
                                            int nbIterationsWithoutImprovement = 5;
                                            while(nbIterationsWithoutImprovement<=20){
                                                
                                                createFile(parameterFile,linkfile,demandfolders[i],numdemands[i],lagMethod,lagFormulation,heuristic,projection,warmstart,alternativeStop,directionMethod,crowderParam,0.0,lagrangianLambda_zero,nbIterationsWithoutImprovement,maxNbIterations);
                                                Input input(parameterFile);

                                                std::cout << "--- READING INSTANCE... --- " << std::endl;
                                                Instance instance(input);

                                                std::cout << "--- READING NEW ONLINE DEMANDS... --- " << std::endl;
                                                std::string nextFile = instance.getInput().getDemandToBeRoutedFilesFromIndex(0);
                                                instance.generateDemandsFromFile(nextFile);

                                                std::cout << "Solving with Lagrangian" << std::endl;
                                                ClockTime LAGRANGIAN_OPTIMIZATION_TIME(ClockTime::getTimeNow());
                                                lagSolverFactory factory;
                                                AbstractLagSolver *solver = factory.createSolver(instance);
                                                solver->run();

                                                std::cout << "Time taken by optimization is : ";
                                                std::cout << std::fixed  << LAGRANGIAN_OPTIMIZATION_TIME.getTimeInSecFromStart() << std::setprecision(9); 
                                                std::cout << " sec" << std::endl; 
                                                fichier  << crowderParam << ";" << 0.0 << ";" << lagrangianLambda_zero << ";" << nbIterationsWithoutImprovement << ";";
                                                fichier << solver->getUB() << ";" << solver->getLB() << ";" << solver->getIteration() << ";" << solver->getStop()<< ";" << std::fixed  << LAGRANGIAN_OPTIMIZATION_TIME.getTimeInSecFromStart() << std::setprecision(9);
                                                fichier << std::endl;
                                                std::cout  << solver->getUB() << ";" << solver->getLB() << ";" << solver->getIteration() << ";" << solver->getStop()<< ";" << std::fixed  << LAGRANGIAN_OPTIMIZATION_TIME.getTimeInSecFromStart() << std::setprecision(9);
                                                std::cout << std::endl;
                                                nbIterationsWithoutImprovement = nbIterationsWithoutImprovement+5;
                                            }
                                            lagrangianLambda_zero = lagrangianLambda_zero+0.5;
                                        }
                                        crowderParam = crowderParam+0.3;
                                    }
                                }else if( directionMethod == 2){
                                    double carmeriniParam=0.5;
                                    while(carmeriniParam<=2.001){
                                        double lagrangianLambda_zero =0.5;
                                        while(lagrangianLambda_zero<=2.001){
                                            int nbIterationsWithoutImprovement = 5;
                                            while( nbIterationsWithoutImprovement<=20){
                                                
                                                createFile(parameterFile,linkfile,demandfolders[i],numdemands[i],lagMethod,lagFormulation,heuristic,projection,warmstart,alternativeStop,directionMethod,0.0,carmeriniParam,lagrangianLambda_zero,nbIterationsWithoutImprovement,maxNbIterations);
                                                Input input(parameterFile);

                                                std::cout << "--- READING INSTANCE... --- " << std::endl;
                                                Instance instance(input);

                                                std::cout << "--- READING NEW ONLINE DEMANDS... --- " << std::endl;
                                                std::string nextFile = instance.getInput().getDemandToBeRoutedFilesFromIndex(0);
                                                instance.generateDemandsFromFile(nextFile);

                                                std::cout << "Solving with Lagrangian" << std::endl;
                                                ClockTime LAGRANGIAN_OPTIMIZATION_TIME(ClockTime::getTimeNow());
                                                lagSolverFactory factory;
                                                AbstractLagSolver *solver = factory.createSolver(instance);
                                                solver->run();

                                                std::cout << "Time taken by optimization is : ";
                                                std::cout << std::fixed  << LAGRANGIAN_OPTIMIZATION_TIME.getTimeInSecFromStart() << std::setprecision(9); 
                                                std::cout << " sec" << std::endl; 
                                                fichier  << 0.0 << ";" << carmeriniParam<< ";" << lagrangianLambda_zero << ";" << nbIterationsWithoutImprovement << ";";
                                                fichier  << solver->getUB() << ";" << solver->getLB() << ";" << solver->getIteration() << ";" << solver->getStop()<< ";" << std::fixed  << LAGRANGIAN_OPTIMIZATION_TIME.getTimeInSecFromStart() << std::setprecision(9);
                                                fichier << std::endl;
                                                std::cout  << solver->getUB() << ";" << solver->getLB() << ";" << solver->getIteration() << ";" << solver->getStop()<< ";" << std::fixed  << LAGRANGIAN_OPTIMIZATION_TIME.getTimeInSecFromStart() << std::setprecision(9);
                                                std::cout << std::endl;
                                                nbIterationsWithoutImprovement= nbIterationsWithoutImprovement+5;
                                            }
                                            lagrangianLambda_zero = lagrangianLambda_zero+0.5;
                                        }
                                        carmeriniParam = carmeriniParam+0.5;
                                    }
                                }else{
                                    double lagrangianLambda_zero =0.5;
                                    while(lagrangianLambda_zero<=2.001){
                                        int nbIterationsWithoutImprovement = 5; 
                                        while(nbIterationsWithoutImprovement<=20){
                                            createFile(parameterFile,linkfile,demandfolders[i],numdemands[i],lagMethod,lagFormulation,heuristic,projection,warmstart,alternativeStop,directionMethod,0.0,0.0,lagrangianLambda_zero,nbIterationsWithoutImprovement,maxNbIterations);
                                            Input input(parameterFile);

                                            std::cout << "--- READING INSTANCE... --- " << std::endl;
                                            Instance instance(input);

                                            std::cout << "--- READING NEW ONLINE DEMANDS... --- " << std::endl;
                                            std::string nextFile = instance.getInput().getDemandToBeRoutedFilesFromIndex(0);
                                            instance.generateDemandsFromFile(nextFile);

                                            std::cout << "Solving with MIP-Cplex" << std::endl;
                                            ClockTime LAGRANGIAN_OPTIMIZATION_TIME(ClockTime::getTimeNow());
                                            lagSolverFactory factory;
                                            AbstractLagSolver *solver = factory.createSolver(instance);
                                            solver->run();

                                            std::cout << "Time taken by optimization is : ";
                                            std::cout << std::fixed  << LAGRANGIAN_OPTIMIZATION_TIME.getTimeInSecFromStart() << std::setprecision(9); 
                                            std::cout << " sec" << std::endl; 
                                            fichier  << 0.0 << ";" << 0.0<< ";" << lagrangianLambda_zero << ";" << nbIterationsWithoutImprovement << ";";
                                            fichier  << solver->getUB() << ";" << solver->getLB() << ";" << solver->getIteration() << ";" << solver->getStop()<< ";" << std::fixed  << LAGRANGIAN_OPTIMIZATION_TIME.getTimeInSecFromStart() << std::setprecision(9);
                                            fichier << std::endl;
                                            std::cout  << solver->getUB() << ";" << solver->getLB() << ";" << solver->getIteration() << ";" << solver->getStop()<< ";" << std::fixed  << LAGRANGIAN_OPTIMIZATION_TIME.getTimeInSecFromStart() << std::setprecision(9);
                                            std::cout << std::endl;
                                            
                                            nbIterationsWithoutImprovement= nbIterationsWithoutImprovement+5;

                                        }
                                        lagrangianLambda_zero=lagrangianLambda_zero+0.5;
                                    }
                                }
                                fichier.close();
                            }  
                        }
                    }
            }else{
                int warmstart=0;
                for(int projection = 0; projection<=2;projection++){ // Projection Method
                    for(int directionMethod = 0; directionMethod <= 3; directionMethod++){ // Direction Method
                        std::string nom_fichier = aux + dossier + "Instance" + std::to_string(i+1) +"/proj" + std::to_string(projection) + "_warmstart"+ std::to_string(warmstart) + "_altStop" +std::to_string(alternativeStop) + "_dirMethod" + std::to_string(directionMethod) +".csv";
                        std::cout << nom_fichier << std::endl;
                        std::ofstream fichier(nom_fichier);
                        fichier << "Max It: " << std::to_string(maxNbIterations) << "; Projection: " + std::to_string(projection) + "; Warmstart: "+ std::to_string(warmstart) + "; AlternativeStop: " +std::to_string(alternativeStop) + "; DirectionMethod: " + std::to_string(directionMethod) + "\n";
                        fichier << labels;
                            if(directionMethod == 1){
                                double crowderParam=0.3;
                                while(crowderParam<=0.901){
                                    double lagrangianLambda_zero =0.5;
                                    while(lagrangianLambda_zero<=2.001){
                                        int nbIterationsWithoutImprovement = 5;
                                        while(nbIterationsWithoutImprovement<=20){
                                            
                                            createFile(parameterFile,linkfile,demandfolders[i],numdemands[i],lagMethod,lagFormulation,heuristic,projection,warmstart,alternativeStop,directionMethod,crowderParam,0.0,lagrangianLambda_zero,nbIterationsWithoutImprovement,maxNbIterations);
                                            Input input(parameterFile);

                                            std::cout << "--- READING INSTANCE... --- " << std::endl;
                                            Instance instance(input);

                                            std::cout << "--- READING NEW ONLINE DEMANDS... --- " << std::endl;
                                            std::string nextFile = instance.getInput().getDemandToBeRoutedFilesFromIndex(0);
                                            instance.generateDemandsFromFile(nextFile);

                                            std::cout << "Solving with Lagrangian" << std::endl;
                                            ClockTime LAGRANGIAN_OPTIMIZATION_TIME(ClockTime::getTimeNow());
                                            lagSolverFactory factory;
			                                AbstractLagSolver *solver = factory.createSolver(instance);
			                                solver->run();

                                            std::cout << "Time taken by optimization is : ";
                                            std::cout << std::fixed  << LAGRANGIAN_OPTIMIZATION_TIME.getTimeInSecFromStart() << std::setprecision(9); 
                                            std::cout << " sec" << std::endl; 
                                            fichier  << crowderParam << ";" << 0.0 << ";" << lagrangianLambda_zero << ";" << nbIterationsWithoutImprovement << ";";
                                            fichier << solver->getUB() << ";" << solver->getLB() << ";" << solver->getIteration() << ";" << solver->getStop()<< ";" << std::fixed  << LAGRANGIAN_OPTIMIZATION_TIME.getTimeInSecFromStart() << std::setprecision(9);
                                            fichier << std::endl;
                                            std::cout  << solver->getUB() << ";" << solver->getLB() << ";" << solver->getIteration() << ";" << solver->getStop()<< ";" << std::fixed  << LAGRANGIAN_OPTIMIZATION_TIME.getTimeInSecFromStart() << std::setprecision(9);
                                            std::cout << std::endl;
                                            nbIterationsWithoutImprovement = nbIterationsWithoutImprovement+5;
                                        }
                                        lagrangianLambda_zero = lagrangianLambda_zero+0.5;
                                    }
                                    crowderParam = crowderParam+0.3;
                                }
                            }else if( directionMethod == 2){
                                double carmeriniParam=0.5;
                                while(carmeriniParam<=2.001){
                                    double lagrangianLambda_zero =0.5;
                                    while(lagrangianLambda_zero<=2.001){
                                        int nbIterationsWithoutImprovement = 5;
                                        while( nbIterationsWithoutImprovement<=20){
                                            
                                            createFile(parameterFile,linkfile,demandfolders[i],numdemands[i],lagMethod,lagFormulation,heuristic,projection,warmstart,alternativeStop,directionMethod,0.0,carmeriniParam,lagrangianLambda_zero,nbIterationsWithoutImprovement,maxNbIterations);
                                            Input input(parameterFile);

                                            std::cout << "--- READING INSTANCE... --- " << std::endl;
                                            Instance instance(input);

                                            std::cout << "--- READING NEW ONLINE DEMANDS... --- " << std::endl;
                                            std::string nextFile = instance.getInput().getDemandToBeRoutedFilesFromIndex(0);
                                            instance.generateDemandsFromFile(nextFile);

                                            std::cout << "Solving with MIP-Cplex" << std::endl;
                                            ClockTime LAGRANGIAN_OPTIMIZATION_TIME(ClockTime::getTimeNow());
                                            lagSolverFactory factory;
			                                AbstractLagSolver *solver = factory.createSolver(instance);
			                                solver->run();

                                            std::cout << "Time taken by optimization is : ";
                                            std::cout << std::fixed  << LAGRANGIAN_OPTIMIZATION_TIME.getTimeInSecFromStart() << std::setprecision(9); 
                                            std::cout << " sec" << std::endl; 
                                            fichier  << 0.0 << ";" << carmeriniParam<< ";" << lagrangianLambda_zero << ";" << nbIterationsWithoutImprovement << ";";
                                            fichier  << solver->getUB() << ";" << solver->getLB() << ";" << solver->getIteration() << ";" << solver->getStop()<< ";" << std::fixed  << LAGRANGIAN_OPTIMIZATION_TIME.getTimeInSecFromStart() << std::setprecision(9);
                                            fichier << std::endl;
                                            std::cout  << solver->getUB() << ";" << solver->getLB() << ";" << solver->getIteration() << ";" << solver->getStop()<< ";" << std::fixed  << LAGRANGIAN_OPTIMIZATION_TIME.getTimeInSecFromStart() << std::setprecision(9);
                                            std::cout << std::endl;
                                            nbIterationsWithoutImprovement= nbIterationsWithoutImprovement+5;
                                        }
                                        lagrangianLambda_zero = lagrangianLambda_zero+0.5;
                                    }
                                    carmeriniParam = carmeriniParam+0.5;
                                }
                            }else{
                                double lagrangianLambda_zero =0.5;
                                while(lagrangianLambda_zero<=2.001){
                                    int nbIterationsWithoutImprovement = 5; 
                                    while(nbIterationsWithoutImprovement<=20){
                                        createFile(parameterFile,linkfile,demandfolders[i],numdemands[i],lagMethod,lagFormulation,heuristic,projection,warmstart,alternativeStop,directionMethod,0.0,0.0,lagrangianLambda_zero,nbIterationsWithoutImprovement,maxNbIterations);
                                        Input input(parameterFile);

                                        std::cout << "--- READING INSTANCE... --- " << std::endl;
                                        Instance instance(input);

                                        std::cout << "--- READING NEW ONLINE DEMANDS... --- " << std::endl;
                                        std::string nextFile = instance.getInput().getDemandToBeRoutedFilesFromIndex(0);
                                        instance.generateDemandsFromFile(nextFile);

                                        std::cout << "Solving with MIP-Cplex" << std::endl;
                                        ClockTime LAGRANGIAN_OPTIMIZATION_TIME(ClockTime::getTimeNow());
                                        lagSolverFactory factory;
			                            AbstractLagSolver *solver = factory.createSolver(instance);
			                            solver->run();

                                        std::cout << "Time taken by optimization is : ";
                                        std::cout << std::fixed  << LAGRANGIAN_OPTIMIZATION_TIME.getTimeInSecFromStart() << std::setprecision(9); 
                                        std::cout << " sec" << std::endl; 
                                        fichier  << 0.0 << ";" << 0.0<< ";" << lagrangianLambda_zero << ";" << nbIterationsWithoutImprovement << ";";
                                        fichier  << solver->getUB() << ";" << solver->getLB() << ";" << solver->getIteration() << ";" << solver->getStop()<< ";" << std::fixed  << LAGRANGIAN_OPTIMIZATION_TIME.getTimeInSecFromStart() << std::setprecision(9);
                                        fichier << std::endl;
                                        std::cout  << solver->getUB() << ";" << solver->getLB() << ";" << solver->getIteration() << ";" << solver->getStop()<< ";" << std::fixed  << LAGRANGIAN_OPTIMIZATION_TIME.getTimeInSecFromStart() << std::setprecision(9);
                                        std::cout << std::endl;
                                        
                                        nbIterationsWithoutImprovement= nbIterationsWithoutImprovement+5;

                                    }
                                    lagrangianLambda_zero=lagrangianLambda_zero+0.5;
                                }
                            }
                            fichier.close();
                        }  
                    }
                }

            }
        }
    }
    return 0;
}