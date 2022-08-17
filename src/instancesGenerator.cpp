#include <bits/stdc++.h> 
#include <chrono> 

#include <lemon/list_graph.h>
#include <lemon/concepts/graph.h>

#include "generator.h"

#include "tools/clockTime.h"
#include "topology/instance.h"
#include "solver/solverFactory.h"

#include "lagrangian/solver/lagSolverFactory.h"


using namespace lemon;

int main(int argc, char *argv[]) {

    std::string topologyFile;
	if (argc < 2){
		std::cerr << "A topology file is required in the arguments. PLease run the program as \n./exec topologyFile.par demandsFile.par nbDemands(int)\n";
		throw std::invalid_argument( "did not receive an argument" );
	}
	else{
		topologyFile = argv[1];
	}
	std::cout << "TOPOLOGY FILE: " << topologyFile << std::endl;

    std::string demandFile;
	if (argc < 3){
		std::cerr << "A demands file is required in the arguments. PLease run the program as \n./exec topologyFile.par demandsFile.par nbDemands(int)\n";
		throw std::invalid_argument( "did not receive an argument" );
	}
	else{
		demandFile = argv[2];
	}
	std::cout << "DEMANDS FILE: " << demandFile << std::endl;
    int nbDemands;
	if (argc < 4){
		std::cerr << "A number of demands is required in the arguments. PLease run the program as \n./exec topologyFile.par demandsFile.par nbDemands(int)\n";
		throw std::invalid_argument( "did not receive an argument" );
	}
	else{
		nbDemands = std::atoi(argv[3]);
	}
	std::cout << "NB DEMANDS: " << nbDemands << std::endl;

    Generator generator(topologyFile,demandFile);
    generator.generateDemands(nbDemands);

    return 0;
}