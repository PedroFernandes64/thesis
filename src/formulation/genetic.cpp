#include "genetic.h"

Routing::Routing(std::vector<std::vector<Fiber> > r) : routes(r), colors(), colored(false){

}
bool Routing::tryColoring(){}

void Routing::setMetric(double m){
	metric = m;
}

void Routing::display(){
	std::cout << "DISPLAYING POPULATION " <<std::endl ;
	for (int a = 0; a < routes.size(); a++){
		std::cout << "demand "<< a + 1 << " || " ;
		for (int b = 0; b < routes[a].size(); b++){
			std::cout << routes[a][b].getSource()+1 << "-" << routes[a][b].getTarget()+1 << "|" ;
		}
		std::cout <<std::endl ;
	}
	if (isColored()==false){
		std::cout<< "no coloring" <<std::endl ;
	}else{
		std::cout<< "coloring" <<std::endl ;
	}    
	std::cout<< "metric "<< metric <<std::endl ;
}
/* Constructor. A graph associated to the initial mapping (instance) is built as well as an extended graph for each demand to be routed. */
Genetic::Genetic(const Instance &inst) : instance(inst){
    
    this->setToBeRouted(instance.getNextDemands());
    //displayToBeRouted();
	chosenK = 2;
	extraK = 2;
    GenerateShortestRoutes();
	nbInitialPop = 50;
	GenerateInitialPopulation(nbInitialPop);
	iterations = 3;
	for (int i = 0; i < iterations; ++i){
		doCrossing();
		doMutation();
		doSelection();
	}

}
void Genetic::GenerateInitialPopulation(int nbPop){

	displayShortestRoutes();

	for (int i = 0; i < nbPop; ++i){
		std::vector<std::vector<Fiber> > routes;
		//std::cout << "1" << std::endl;
		for (int d = 0; d < toBeRouted.size(); ++d){
			std::vector<Fiber> route;
			//std::cout << "2" << std::endl;
			int randomChoice;
			int upperlimit;
			if (shortestRoutesByDemand[d].size()<chosenK){
				upperlimit = shortestRoutesByDemand[d].size();
			}else{
				upperlimit = chosenK;
			}
			randomChoice = rand() % upperlimit;

			for (int l = 0; l < shortestRoutesByDemand[d][randomChoice].size(); ++l){
				route.push_back(shortestRoutesByDemand[d][randomChoice][l]);
				//std::cout << shortestRoutesByDemand[d][0][l].getSource()+1 << "-" << shortestRoutesByDemand[d][0][l].getTarget()+1 << "|" ;
            
			}
			routes.push_back(route);
		}
		
		Routing member(routes);

		initalPopulation.push_back(member);
	}
	for (int i = 0; i < initalPopulation.size(); i++){
		double metr = 0 ;
		for (int a = 0; a < initalPopulation[i].routes.size(); a++){
			metr = metr + initalPopulation[i].routes[a].size()*toBeRouted[a].getLoadC();
		}

		initalPopulation[i].setMetric(metr);
		initalPopulation[i].display();
	}
}

double Genetic::osnrPath(double noise, double pch){
    double osnr;
    double osnrdb;		
    osnr = pch/(noise);
    osnrdb = 10.0 * log10(osnr);
    return osnrdb;
}

void Genetic::GenerateShortestRoutes(){
    
	//DJIKISTRA MODULE     	
    int originDjikistra;
    int destinationDijikistra;
	std::vector<std::vector<Fiber> > routing;
	int countroutings=0;

    for (int i = 0; i < toBeRouted.size(); ++i){
        originDjikistra = toBeRouted[i].getSource();
        destinationDijikistra = toBeRouted[i].getTarget();
        //std::cout << "origin "<< originDjikistra<< std::endl;
        //std::cout << "dest "<< destinationDijikistra<< std::endl;
        //creating adj matrix
        std::vector<std::vector<int> > adjmatrix;
        std::vector<int> auxadj;
        for (int i = 0; i < getInstance().getNbNodes(); ++i){
            for (int j = 0; j < getInstance().getNbNodes(); ++j){
                auxadj.push_back(0);
            }
            adjmatrix.push_back(auxadj);
            auxadj.clear();
        }
        //filling adj matrix
        for (int i = 0; i < getInstance().getNbNodes(); ++i){
            int demandorigin = i;
            for (int j = 0; j < getInstance().getTabEdge().size(); ++j){
                int edgeorigin = getInstance().getTabEdge()[j].getSource();
                int edgedestination = getInstance().getTabEdge()[j].getTarget();
                //std::cout << "origin "<< edgeorigin<< std::endl;
                //std::cout << "dest "<< edgedestination<< std::endl;
                if (edgedestination == demandorigin){
                    adjmatrix[i][edgeorigin] = getInstance().getTabEdge()[j].getLength();
                    //adjmatrix[i][edgeorigin] = 1;
                }
                else{
                    if (edgeorigin == demandorigin){
                        adjmatrix[i][edgedestination] = getInstance().getTabEdge()[j].getLength();
                        //adjmatrix[i][edgedestination] = 1;
                    }
                }
            }
        }
        
        //std::cout <<std::endl<< "printing matrix" <<std::endl; 
        //for (int i = 0; i < adjmatrix.size(); i++){
        //    for (int j = 0; j < adjmatrix[i].size(); j++){
        //        std::cout << adjmatrix[i][j] << " ";
        //    }
        //    std::cout << std::endl;
        // }
        
        std::vector<std::vector<Fiber> > kpathsedges;
        
        std::vector<std::vector<Fiber> > kcandidates;
        //DEFINE K SHORTEST

        kpathsedges = kdijkstra(adjmatrix,originDjikistra, destinationDijikistra, chosenK+extraK);
        for (int k = 0; k < kpathsedges.size(); k++){
            double pathNoise = 0.0;
            double pathCD = 0.0;
            std::cout << std:: endl<< "Path from " << originDjikistra+1 << " to " << destinationDijikistra+1 << " is: " ; 
            for (int p = 0; p != kpathsedges[k].size(); ++p){
            	std::cout << kpathsedges[k][p].getSource()+1 << "-" << kpathsedges[k][p].getTarget()+1 << "|" ;
                pathNoise = pathNoise + kpathsedges[k][p].getNoiseC();
                pathCD = pathCD + (kpathsedges[k][p].getLength()* kpathsedges[k][p].getDispersionCoeffC());
            }

            double pch = toBeRouted[i].getPchC();
            double osnr;
            double osnrdb;
            double osnrLim = toBeRouted[i].getOsnrLimitC();		
            osnr = pch/(pathNoise);
            osnrdb = 10.0 * log10(osnr);
            double CDLim = toBeRouted[i].getmaxCDC();                  
            std::cout << "OSNR " << osnrdb << " CD " << pathCD <<"|" ;
            if ((pathCD <= CDLim) && (osnrdb>=osnrLim)){
                std::vector<Fiber> candidate;
                for (int p = 0; p != kpathsedges[k].size(); ++p){
                    candidate.push_back(kpathsedges[k][p]);
                }
                kcandidates.push_back(candidate);
            }else{
                std::cout << "REFUSED";
            }
                
        }
        shortestRoutesByDemand.push_back(kcandidates);
    }
}
  

std::vector<std::vector<Fiber> > Genetic::kdijkstra(std::vector<std::vector<int> > graph, int src, int dest, int K){
	std::vector<std::vector<int> > modifiablegraph = graph;
	std::vector<std::vector<int> > A; //chosen paths
	std::vector<std::vector<int> > B; //candidate paths 
	std::vector<int> djikistraSolution;
	int spurNode;
	std::vector<int> spurPath;
	double dist;
	djikistraSolution = dijkstra(graph,src,dest,dist);
	//std::cout << "1 djikitra: ";
	//for (int zz = 0; zz < djikistraSolution.size(); zz++){
	//		std::cout << djikistraSolution[zz] << " ";
	//}
	//std::cout << std::endl;
	A.push_back(djikistraSolution);
	for (int k = 1; k<K; k++){
		for (int i = 0; i < A[k-1].size()-1; i++ ){
			//std::cout << "olha o i " << i << std::endl;
			spurNode = A[k-1][i];
			//std::cout << "spur node " << spurNode << std::endl;
			std::vector<int> rootpath;
			for (int j = 0; j <= i; j++){
				rootpath.push_back(A[k-1][j]);
			}
			//std::cout << "root path: ";
			//for (int w = 0; w < rootpath.size(); w++){
			//	std::cout << rootpath[w] << " ";
			//}
			//std::cout << std::endl;
			for (int path = 0; path < A.size() ; path ++){
				std::vector<int> auxiliarpath;
				//definindo se vai ate o i ou ate o fim do path ai
				int stop = i;
				if (A[path].size()<i){
					//std::cout << "A nao é grande o suficiente pro i" << std::endl;
					stop = A[path].size() -1;
				}
				for (int node = 0; node <= stop; node++){
					auxiliarpath.push_back(A[path][node]);
				}
				//std::cout << "auxiliar path from A: ";
				//	for (int w = 0; w < auxiliarpath.size(); w++){
				//		std::cout << auxiliarpath[w] << " ";
				//	}
				//std::cout << std::endl;
				//verificar se o roothpath é igual
				bool auxigualroot = true;
				for (int v = 0; v < auxiliarpath.size(); v++){
					if (auxiliarpath[v]!=rootpath[v]){
						auxigualroot = false;
					}
				}
				if (auxigualroot == true){
					//std::cout << "remover " << A[path][i] << " e "  << A[path][i+1] << std::endl;
					modifiablegraph[A[path][i]-1][A[path][i+1]-1] = 0;
					modifiablegraph[A[path][i+1]-1][A[path][i]-1] = 0;
				}
			}
			//std::cout << "removendo do spur path" << std::endl;
			if (rootpath.size() > 1){
				for (int w = 0; w < rootpath.size()-1; w++){
					//std::cout << "remover " << rootpath[w] << " e "  << rootpath[w+1] << std::endl;
					modifiablegraph[rootpath[w]-1][rootpath[w+1]-1] = 0;
					modifiablegraph[rootpath[w+1]-1][rootpath[w]-1] = 0;
				}
			}
			//std::cout << "removendo causa de ciclos path" << std::endl;
			for (int w = 0; w < rootpath.size()-1; w++){
				for (int adj = 0; adj < modifiablegraph[rootpath[w]-1].size(); adj++){
					//std::cout << "remover " << rootpath[w] << " e "  <<  adj+1<< std::endl;
					modifiablegraph[rootpath[w]-1][adj] = 0;
					modifiablegraph[adj][rootpath[w]-1] = 0;
				}
			}
			//std::cout << "printing adjmatrix" <<std::endl; 
			//for (int i = 0; i < modifiablegraph.size(); i++){
			//	for (int j = 0; j < modifiablegraph[i].size(); j++){
			//		std::cout << modifiablegraph[i][j] << " ";
			//	}
			//	std::cout << std::endl;
			//}
			spurPath = dijkstra(modifiablegraph,spurNode-1,dest,dist);
			if (spurPath.size() == 1){
				modifiablegraph = graph;
				//std::cout << "breakou o pau comeu " << std::endl;
			}
			else{
			//std::cout << "pedaço do novo candidato: ";
			//for (int zz = 0; zz < spurPath.size(); zz++){
			//	std::cout << spurPath[zz] << " ";
			//}
			//std::cout << std::endl;
			std::vector<int> totalpath;
			//std::cout << "adicionando root"<< std::endl;
			for (int nn = 0; nn < rootpath.size()-1; nn++){
				totalpath.push_back(rootpath[nn]);
			}
			//std::cout << "adicionando pedaço"<< std::endl;
			for (int nn = 0; nn < spurPath.size(); nn++){
				totalpath.push_back(spurPath[nn]);
			}
			//std::cout << "novo candidato: ";
			//for (int zz = 0; zz < totalpath.size(); zz++){
			//	std::cout << totalpath[zz] << " ";
			//}
			bool ehigual = false;
			for (int bzin=0; bzin < B.size(); bzin++){
				if (B[bzin].size() == totalpath.size()){
					bool auxtotaligualbzin = true;
					for (int v = 0; v < totalpath.size(); v++){
						if (B[bzin][v]!=totalpath[v]){
							//std::cout << "elemento diferente caraio"<<std::endl;
							auxtotaligualbzin = false;
						}
					}
					if (auxtotaligualbzin == true){
						//std::cout << "essa porra é ingual adiciona n"<<std::endl;
						ehigual = true;
						break;
					}
				}
			}
			if (ehigual == false){
				//std::cout << "adicione ";
				//for (int zz = 0; zz < totalpath.size(); zz++){
				//	std::cout << totalpath[zz] << " ";
				//}
				//std::cout << std::endl;
				B.push_back(totalpath);
			}
			modifiablegraph = graph;
			}
		}
		if (B.size()==0){
			break;
		}
		//std::cout << "paths em b: " << B.size()<<  std::endl;
		//for (int bzin = 0; bzin < B.size(); bzin++){
		//	std::cout << "paths " << bzin + 1<< " : ";
		//	for (int bzin2 = 0; bzin2 < B[bzin].size(); bzin2++){
		//		std::cout << B[bzin][bzin2] << " ";
		//	}
		//	std::cout << std::endl;
		//}
		//FALTA O SORT
		std::vector<double> distB;
		for (int bzin = 0; bzin < B.size(); bzin++){
			double distatual = 0;
			for (int bzin2 = 0; bzin2 < B[bzin].size()-1; bzin2++){
				//std::cout << "Somando edge " << B[bzin][bzin2] <<" " << B[bzin][bzin2+1] << " dist: " << graph[B[bzin][bzin2]-1][B[bzin][bzin2+1]-1] <<std::endl;
				distatual = distatual + graph[B[bzin][bzin2]-1][B[bzin][bzin2+1]-1];
			}
			distB.push_back(distatual);
			//std::cout << std::endl;
		}
		//for (int bzin = 0; bzin < distB.size(); bzin++){
		//	std::cout << "b " << bzin + 1  << " dist: " << distB[bzin] <<std::endl;
		//
		//}
		double minelement = *min_element(distB.begin(),distB.end());
		//std::cout << "el minimo: " << minelement <<std::endl;
		int index;
		for (int bzin = 0; bzin < distB.size(); bzin++){
			if(distB[bzin]==minelement){
				index = bzin;
				break;
			}
		}
		//std::cout << "index del minimo: " << index <<std::endl;
		//FAZER SORT
		//std::cout << "paths em A: " << A.size()<<  std::endl;
		//for (int bzin = 0; bzin < A.size(); bzin++){
		//	std::cout << "paths " << bzin + 1<< " : ";
		//	for (int bzin2 = 0; bzin2 < A[bzin].size(); bzin2++){
		//		std::cout << A[bzin][bzin2] << " ";
		//	}
		//	std::cout << std::endl;
		//}

		//std::cout << "adicionando ";
		//for (int zz = 0; zz < B[index].size(); zz++){
		//	std::cout << B[index][zz] << " ";
		//}
		A.push_back(B[index]);
		//aqui fazer swap de quem ta no index com o begin
		//std::cout << "trocando ";
		//for (int zz = 0; zz < B[0].size(); zz++){
		//	std::cout << B[0][zz] << " ";
		//}
		//std::cout << "por ";
		//for (int zz = 0; zz < B[index].size(); zz++){
		//	std::cout << B[index][zz] << " ";
		//}
		B[0].swap(B[index]);
		B.erase(B.begin());
		//std::cout << "paths em b: " << B.size()<<  std::endl;
		//for (int bzin = 0; bzin < B.size(); bzin++){
		//	std::cout << "paths " << bzin + 1<< " : ";
		//	for (int bzin2 = 0; bzin2 < B[bzin].size(); bzin2++){
		//		std::cout << B[bzin][bzin2] << " ";
		//	}
		//	std::cout << std::endl;
		//}
	}

	std::vector<std::vector<Fiber> > solution;
	std::vector<Fiber> auxsolution;
	for (int a = 0; a <A.size(); a++){
		for (int i = 0; i < A[a].size() -1 ; i++){
			int origindege = A[a][i]  ;
			int destinationedge = A[a][i+1] ;  
			for (int j = 0; j < getInstance().getTabEdge().size(); j++){
				if (getInstance().getTabEdge()[j].getSource() == origindege-1 && getInstance().getTabEdge()[j].getTarget() == destinationedge-1){
					auxsolution.push_back(getInstance().getTabEdge()[j]);
				}
				if (getInstance().getTabEdge()[j].getTarget() == origindege-1 && getInstance().getTabEdge()[j].getSource() == destinationedge-1){
					auxsolution.push_back(getInstance().getTabEdge()[j]);
				}
			}
		}
		solution.push_back(auxsolution);
		auxsolution.clear();
	}
	return solution;

}

int Genetic::minDistance(std::vector<int> dist, std::vector<bool> sptSet)
{
  	// Initialize min value
    int min = INT_MAX, min_index;

    for (int v = 0; v < getInstance().getNbNodes(); v++)
        if (sptSet[v] == false && dist[v] <= min)
            min = dist[v], min_index = v;

    return min_index;
}
  
std::vector<int> Genetic::dijkstra(std::vector<std::vector<int> > graph, int src, int dest, double & pathdistance)
{
    std::vector<int> dist; 	// The output array.  dist[i] will hold the shortest distance from src to i
	
	std::vector<std::vector<int> > pathNodes;
  
    std::vector<bool> sptSet; // sptSet[i] will be true if vertex i is included in shortest path tree or shortest distance from src to i is finalized
  
    // Initialize all distances as INFINITE and stpSet[] as
    // false
    for (int i = 0; i < getInstance().getNbNodes(); i++){
        dist.push_back(INT_MAX);
		sptSet.push_back(false);
	}

	std::vector<int> auxpathNodes;
	for (int i = 0; i < getInstance().getNbNodes(); i++){
        pathNodes.push_back(auxpathNodes);
	}
	  
    // Distance of source vertex from itself is always 0
    dist[src] = 0;
    // Find shortest path for all vertices
    for (int count = 0; count < getInstance().getNbNodes(); count++) {
        // Pick the minimum distance vertex from the set of
        // vertices not yet processed. u is always equal to
        // src in the first iteration.
        int u = minDistance(dist, sptSet);
        // Mark the picked vertex as processed
        sptSet[u] = true;
  
        // Update dist value of the adjacent vertices of the
        // picked vertex.
        for (int v = 0; v < getInstance().getNbNodes(); v++){
  
            // Update dist[v] only if is not in sptSet,
            // there is an edge from u to v, and total
            // weight of path from src to  v through u is
            // smaller than current value of dist[v]
			
            if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v]){
				//std::cout << "avaliando " << v+1 <<std::endl; ;
				if (pathNodes[v].size() > 0){
					//std::cout << "path existente a ser substituido" << std::endl;
					pathNodes[v].clear();
				}
                dist[v] = dist[u] + graph[u][v];
				pathNodes[v].push_back(u);
				//std::cout << "no path do " << v+1 << " adicionar " << u+1 ;
				//std::cout << " e tambem ";
				for(int i = 0; i < pathNodes[u].size(); i++){
					pathNodes[v].push_back(pathNodes[u][i]);
					//std::cout << pathNodes[u][i] + 1 << " "; 
				}
			}
		}
    }
	/*
	std::cout << "Vertex Path from Source" << endl;
	for (int i = 0; i < pathNodes.size(); i++){
		std::cout << i+1 << " ";
		for (int j = 0; j < pathNodes[i].size(); j++){
			std::cout <<  pathNodes[i][j] + 1<< " "; 
		}
		std::cout<< std::endl;
	}
	*/
	/*
	std::cout << "Chosen path" << endl;
	for (int i = pathNodes[dest].size() -1 ; i >= 0; i--){
		std::cout <<  pathNodes[dest][i] + 1 << " "; 
	}
	std::cout << dest + 1;
	std::cout<< std::endl;
	*/
	
	double chosendistance = dist[dest];
    /*
	std::cout << "Chosen distance " << chosendistance << endl;
    // print the constructed distance array
    std::cout << "Vertex Distance from Source" << endl;
    for (int i = 0; i < RSA_Input_.getNodes().size(); i++){
        std::cout << i+1 << " " << dist[i] << std::endl;
	}
	*/
    
	std::vector<int> sol;
	for (int i = pathNodes[dest].size() -1 ; i >= 0; i--){
		sol.push_back(pathNodes[dest][i] + 1); 
	}
	sol.push_back(dest+ 1);
	
	return sol;
}


/* Displays the demands to be routed in the next optimization. */
void Genetic::displayToBeRouted(){
    std::cout << "--- ROUTING DEMANDS --- " << std::endl;
    for (int i = 0; i < getNbDemandsToBeRouted(); i++){
        std::cout << "#" << toBeRouted[i].getId()+1;
        std::cout << " (" << toBeRouted[i].getSource()+1 << ", " << toBeRouted[i].getTarget()+1 << "), ";
        std::cout << "requiring " << toBeRouted[i].getLoadC() << " slices and having Max CD ";
        std::cout << toBeRouted[i].getmaxCDC() << std::endl;
    }
}

void Genetic::displayShortestRoutes(){
    for (int a = 0; a < shortestRoutesByDemand.size(); a++){
        std::cout << "demand "<< a + 1 <<std::endl ;
        for (int b = 0; b < shortestRoutesByDemand[a].size(); b++){
            std::cout << "option "<< b + 1 << " || " ;
            for (int c = 0; c < shortestRoutesByDemand[a][b].size(); c++){
                std::cout << shortestRoutesByDemand[a][b][c].getSource()+1 << "-" << shortestRoutesByDemand[a][b][c].getTarget()+1 << "|" ;
            }
        std::cout <<std::endl ;
        }
    }    
}

/****************************************************************************************/
/*										Destructor										*/
/****************************************************************************************/

/* Destructor. Clears the vectors of demands and links. */
Genetic::~Genetic() {
	toBeRouted.clear();
	
}
