#include "genetic.h"

Routing::Routing(std::vector<std::vector<Fiber> > r, std::vector<int> l) : routes(r), loads(l), colors(), colored(false){
	for (int d = 0; d < routes.size(); ++d){
		colors.push_back(0) ;
	}
	metricVal = 0;
}

void Routing::computeMetric(int m){
	if(m == 1){
		for (int a = 0; a < this->routes.size(); a++){
			metricVal= metricVal+ this->routes[a].size()*loads[a];
		}
	}else{
		if(m == 2){
			metricVal= 1;
			for(int i = 0; i < colors.size(); ++i){
				if((colors[i]+loads[i]-1)>=metricVal){
					metricVal=(colors[i]+loads[i]-1);
				}
			}
		}
	}
}

void Routing::building(int metr, int edges, int slots) {
	setNbEdges(edges);
	setNbSlots(slots);
	//std::cout << " COLORING " << std::endl;
	feasible = tryColoring();
	if(feasible == true){
		colored = true;
		computeMetric(metr);
	}
}

void Routing::copying(Routing r) {
	setMetric(r.metricVal);
	setNbEdges(r.nbEdges);
	setNbSlots(r.nbSlots);
	copyColoring(r.colors);
	feasible = true;
	colored = true;
}

void Routing::copyColoring(std::vector<int> c){
	for (int d = 0; d < c.size(); ++d){
		colors[d] = c[d];
	}
}

Routing::~Routing() {
	for (int a = 0; a < this->routes.size(); a++){
		this->routes[a].clear();
	}
}

bool Routing::tryColoring(){
	std::vector<std::vector<int> > demandsOnThisEdge;
	std::vector<std::vector<int> > matrixDd;
	for (int e = 0; e < nbEdges; ++e){
		std::vector<int> list;
		demandsOnThisEdge.push_back(list) ;
	}
	for (int d = 0; d < routes.size(); ++d){
		std::vector<int> list;
		for (int d2 = 0; d2 < routes.size(); ++d2){
			list.push_back(0) ;
		}
		matrixDd.push_back(list) ;
	}
	for(int d = 0; d < routes.size(); ++d){
		int currentDemand = d;
		for(int l = 0; l < routes[d].size(); ++l){
			demandsOnThisEdge[routes[d][l].getId()].push_back(d);
		}
	}
	/*
	std::cout<< "MATRIZ VAZIA" <<std::endl ;
	for (int d = 0; d < routes.size(); ++d){
		for (int d2 = 0; d2 < routes.size(); ++d2){
			std::cout << matrixDd[d][d2] << "|";
		}
		std::cout << std::endl;
	}
	std::cout<< "MATRIZ PREENCHIDA" <<std::endl ;
	*/
	for (int e = 0; e < nbEdges; ++e){
		for (int d = 0; d < demandsOnThisEdge[e].size(); ++d){
			for (int d2 = d+1; d2 < demandsOnThisEdge[e].size(); ++d2){			
				matrixDd[demandsOnThisEdge[e][d]][demandsOnThisEdge[e][d2]] = 1;
				matrixDd[demandsOnThisEdge[e][d2]][demandsOnThisEdge[e][d]] = 1;
			}
		}
	}
	/*
	for (int d = 0; d < routes.size(); ++d){
		for (int d2 = 0; d2 < routes.size(); ++d2){
			std::cout << matrixDd[d][d2] << "|";
		}
		std::cout << std::endl;
		
	}
	std::cout<< "TRYING TO COLOR" <<std::endl ;

	for (int d = 0; d < routes.size(); ++d){
		std::cout<< "demand " << d << " with "<< loads[d] << std::endl;
	}
	*/
	bool canColor = true;
	for (int d = 0; d < routes.size(); ++d){
		//std::cout<< "demand " << d << std::endl ;
		int feasible = true;
		int candidateColor = 1;
		int rightSlot = candidateColor + loads[d] - 1;
		int nextLowestColor = 1;
		int chosenColor = 0;
		bool ovDetected = true;
		while(ovDetected == true){
			ovDetected = false;
			//std::cout<< "Looking for overlapping between slots "<< candidateColor << "and " << rightSlot <<std::endl ;
			for (int d2 = 0; d2 < routes.size(); ++d2){
				if(matrixDd[d][d2] == 1){
					//std::cout<< "First neighboor "<< d2 << std::endl;
					int neighboorLeft =  colors[d2] ;
					if (neighboorLeft == 0){
						//std::cout<< "Not colored" <<std::endl;
					}else{
						int neighboorRight =  colors[d2] + loads[d2]-1;
						//std::cout<< "is between "<< neighboorLeft << " and "<< neighboorRight << std::endl;
						for (int s1 = candidateColor; s1 <= rightSlot; ++s1){
							for (int s2 = neighboorLeft; s2 <= neighboorRight; ++s2){
								if(s1==s2){
									//std::cout<< " overlaps " << std::endl;
									ovDetected = true;
									if (neighboorRight + 1 > nextLowestColor){
										nextLowestColor = neighboorRight + 1;
										//std::cout<< " we should look only after "<< nextLowestColor << std::endl;
									}
								}
							}
						}
					}
				}
			}
			if((nextLowestColor > nbSlots-loads[d] + 1)){
				feasible = false;
				ovDetected = false;
			}else{
				if(ovDetected == false){
					feasible = true;
					chosenColor = candidateColor;
					//std::cout<< "demand " << d << " gets " <<chosenColor << std::endl ;
					colors[d]=chosenColor;
				}else{
					candidateColor = nextLowestColor;
					rightSlot = candidateColor + loads[d] - 1;
				}
			}
		}
		if(feasible == false){
			canColor = false;
			break;
		}
	}
	if(canColor == true){
		for (int d = 0; d < routes.size(); ++d){
			int leftSlot1 = colors[d];
			int rightSlot1 = colors[d] + loads[d] -1;
			//std::cout<< " verificando demanda "<< d << " entre " << leftSlot1 <<"-"<< rightSlot1 << std::endl;	
			for (int d2 = 0; d2 < routes.size(); ++d2){
				int leftSlot2 = colors[d2];
				int rightSlot2 = colors[d2] + loads[d2] -1;
				//std::cout<< " contra "<< d2 << " entre " << leftSlot2 <<"-"<< rightSlot2 << std::endl;
				if((matrixDd[d][d2] == 1)||(matrixDd[d2][d])){
					//std::cout<< " ESSES PODEM OVERLAPAR " << std::endl;
					for (int s1 = leftSlot1; s1 <= rightSlot1; ++s1){
						for (int s2 = leftSlot2; s2 <= rightSlot2; ++s2){
							if(s1==s2){
								std::cout<< " AI FUDEU " << std::endl;
							}
						}
					}
				}
			}
		}
		return true;
	}else{
		return false;
	}
}

void Routing::setMetric(double m){
	this->metricVal = m;
}
void Routing::setNbEdges(int nb){
	this->nbEdges = nb;
}
void Routing::setNbSlots(int nb){
	this->nbSlots = nb;
}

void Routing::display(){
	for (int a = 0; a < routes.size(); a++){
		std::cout << "Demand "<< a + 1 << " || Slots : "<<colors[a]<<"-"<< colors[a] + loads[a] -1 << " |||";
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
	std::cout<< "metric "<< metricVal <<std::endl ;
}
/* Constructor. A graph associated to the initial mapping (instance) is built as well as an extended graph for each demand to be routed. */
Genetic::Genetic(const Instance &inst) : instance(inst){
    
    this->setToBeRouted(instance.getNextDemands());
	setLoadVector();
    //displayToBeRouted();
	chosenK = 3;
	extraK = 3;
	metric = 1;
	ClockTime clock(ClockTime::getTimeNow());
	clock.setStart(ClockTime::getTimeNow());
    GenerateShortestRoutes();
	std::cout<< "shortest CLOCK: "<< clock.getTimeInSecFromStart() <<std::endl ;
	nbInitialPop = 1000;

	clock.setStart(ClockTime::getTimeNow());
	GenerateInitialPopulation(nbInitialPop);
	//for (int i = 0; i < population.size(); i++){
	//	population[i].display();
	//}
	int iterations = 20;
	for (int i = 1; i <= iterations; ++i){
		doCrossing();
		//for (int i = 0; i < thisIterationCrossing.size(); i++){
		////	thisIterationCrossing[i].display();
		//}
		doMutation();
		//for (int i = 0; i < thisIterationMutation.size(); i++){
		//	thisIterationMutation[i].display();
		//}
		doSelection();
		std::cout<<"FIN IT: "<< i<<std::endl;
		std::cout<<"First place: "<< population[0].metricVal<<std::endl;
		std::cout<<"Last place: "<< population[population.size()-1].metricVal<<std::endl;
		//for (int i = 0; i < population.size(); i++){
		//	population[i].display();
		//}
	}
	std::cout<< "genetic CLOCK: "<< clock.getTimeInSecFromStart() <<std::endl ;

}

void Genetic::setLoadVector(){
	for (int i = 0; i < toBeRouted.size(); ++i){
		loads.push_back(toBeRouted[i].getLoadC());
	}
}

void Genetic::doSelection(){
	std::cout << "ADDING OLD POP: " << population.size() << std::endl;
	for (int i = 0; i < population.size(); ++i){
		//population[i].display();
		std::vector<std::vector<Fiber> > routes;
		for (int d = 0; d < population[i].routes.size(); ++d){
			std::vector<Fiber> route;
			for (int l = 0; l < population[i].routes[d].size(); ++l){
				route.push_back(population[i].routes[d][l]);
			}
			routes.push_back(route);
		}
		Routing member(routes,loads);
		
		member.copying(population[i]);
		thisIterationSelected.push_back(member);
	}
	std::cout << "ADDING CROSSED: " << thisIterationCrossing.size() << std::endl;
	for (int i = 0; i < thisIterationCrossing.size(); ++i){
		std::vector<std::vector<Fiber> > routes;
		//std::cout << "1" << std::endl;
		for (int d = 0; d < thisIterationCrossing[i].routes.size(); ++d){
			std::vector<Fiber> route;
			for (int l = 0; l < thisIterationCrossing[i].routes[d].size(); ++l){
				route.push_back(thisIterationCrossing[i].routes[d][l]);
			}
			routes.push_back(route);
		}
		Routing member(routes,loads);	
		member.copying(thisIterationCrossing[i]);
		thisIterationSelected.push_back(member);
	}
	std::cout << "ADDING MUTANTS: " << thisIterationMutation.size() << std::endl;
	for (int i = 0; i < thisIterationMutation.size(); ++i){
		std::vector<std::vector<Fiber> > routes;
		//std::cout << "1" << std::endl;
		for (int d = 0; d < thisIterationMutation[i].routes.size(); ++d){
			std::vector<Fiber> route;
			for (int l = 0; l < thisIterationMutation[i].routes[d].size(); ++l){
				route.push_back(thisIterationMutation[i].routes[d][l]);
			}
			routes.push_back(route);
		}
		Routing member(routes,loads);
		member.copying(thisIterationMutation[i]);
		thisIterationSelected.push_back(member);
	}
	std::cout << "TOTAL POP : " << thisIterationSelected.size() << std::endl;
	/*
	for (int i = 0; i < thisIterationSelected.size(); i++){
		thisIterationSelected[i].display();
	}
	*/
	std::cout << "SORTING POP" << std::endl;
	sortRouting sorter;
	std::sort (thisIterationSelected.begin(),thisIterationSelected.end(),sorter);
	/*
	for (int i = 0; i < thisIterationSelected.size(); i++){
		thisIterationSelected[i].display();
	}
	*/
	std::cout << "SELECTING POP" << std::endl;
	int toDelete = thisIterationSelected.size() - nbInitialPop;
	std::cout << "DELETED: " << toDelete << std::endl;
	for (int i = 0; i < toDelete; i++){
		thisIterationSelected.pop_back();
	}
	/*
	for (int i = 0; i < thisIterationSelected.size(); i++){
		thisIterationSelected[i].display();
	}*/

	//std::cout << "CLEANING OLD POPS" << std::endl;
	while(population.size()>0){
		population.pop_back();
	}
	while(thisIterationCrossing.size()>0){
		thisIterationCrossing.pop_back();
	}
	while(thisIterationMutation.size()>0){
		thisIterationMutation.pop_back();
	}
	//std::cout <<  population.size() << std::endl;
	//std::cout << "ADDING SELECTED TO POP" << std::endl;
	for (int i = 0; i < thisIterationSelected.size(); ++i){
		std::vector<std::vector<Fiber> > routes;
		//std::cout << "1" << std::endl;
		for (int d = 0; d < thisIterationSelected[i].routes.size(); ++d){
			std::vector<Fiber> route;
			for (int l = 0; l < thisIterationSelected[i].routes[d].size(); ++l){
				route.push_back(thisIterationSelected[i].routes[d][l]);
			}
			routes.push_back(route);
		}
		Routing member(routes,loads);
		member.copying(thisIterationSelected[i]);
		population.push_back(member);
	}
	/*
	std::cout << "NEW POP: " << population.size() << std::endl;
	for (int i = 0; i < population.size(); i++){
		population[i].display();
	}*/
	std::cout << "CLEANING SELECTED VECTOR" << std::endl;
	while(thisIterationSelected.size()>0){
		thisIterationSelected.pop_back();
	}
}
void Genetic::doMutation(){
	std::cout << "MUTATION OPERATION" << std::endl;
	//int nbMutations = floor(population.size()/5);
	int nbMutations = floor(population.size()/2);  
	std::vector<int> canMutate;
		
	for (int d = 0; d < toBeRouted.size(); ++d){
		if (shortestRoutesByDemand[d].size()>chosenK){
			canMutate.push_back(d);
		}
	}
	for (int i = 0; i < nbMutations; ++i){
		int mutant = rand() % population.size();
		std::vector<std::vector<Fiber> > routes;
		//std::cout << "mutating " << mutant + 1 << std::endl;
		for (int d = 0; d < toBeRouted.size(); ++d){
			std::vector<Fiber> route;
			bool mutantGene = std::find(std::begin(canMutate),std::end(canMutate), d) != std::end(canMutate);
			if (mutantGene  == true){
				//std::cout << "NEW gene  " << d + 1 << std::endl;
				int options = shortestRoutesByDemand[d].size() - chosenK;
				int newGene = rand() % options;
				for (int l = 0; l < shortestRoutesByDemand[d][newGene+chosenK].size(); ++l){
					route.push_back(shortestRoutesByDemand[d][newGene+chosenK][l]);
					//std::cout << shortestRoutesByDemand[d][0][l].getSource()+1 << "-" << shortestRoutesByDemand[d][0][l].getTarget()+1 << "|" ;
				}
				routes.push_back(route);
			}else{
				//std::cout << "gene  " << d + 1 << std::endl;
				for (int l = 0; l < population[mutant].routes[d].size(); ++l){
					route.push_back(population[mutant].routes[d][l]);

				}
				routes.push_back(route);
			}
		}
		Routing member(routes,loads);
		member.building(metric,instance.getTabEdge().size(),instance.getTabEdge()[0].getNbSlicesC());
		if(member.feasible == true){
			//std::cout << "Member accepted "<< i << std::endl;
			thisIterationMutation.push_back(member);
		}else{
			//std::cout << "Member refused "<< i << std::endl;
			//member.display();
		}
	}

}


void Genetic::doCrossing(){
	std::cout << "CROSSING OPERATION" << std::endl;
	//int nbCrossings = floor(population.size()/2); 
	int nbCrossings = floor(population.size()); 
	for (int i = 0; i < nbCrossings; ++i){
		int progenitor1;
		int progenitor2;
		progenitor1 = rand() % population.size();
		progenitor2 = rand() % population.size();
		std::vector<std::vector<Fiber> > routes;
		//std::cout << "crossing " << progenitor1 + 1<< "and " << progenitor2 + 1<< std::endl;
		for (int d = 0; d < toBeRouted.size(); ++d){
			std::vector<Fiber> route;
			if (d%2 == 0){
				for (int l = 0; l < population[progenitor1].routes[d].size(); ++l){
					route.push_back(population[progenitor1].routes[d][l]);

				}
				routes.push_back(route);
			}else{
				for (int l = 0; l < population[progenitor2].routes[d].size(); ++l){
					route.push_back(population[progenitor2].routes[d][l]);

				}
				routes.push_back(route);
			}
		}
		
		Routing member(routes,loads);
		member.building(metric,instance.getTabEdge().size(),instance.getTabEdge()[0].getNbSlicesC());
		if(member.feasible == true){
			//std::cout << "Member accepted "<< i << std::endl;
			thisIterationCrossing.push_back(member);
		}else{
			//std::cout << "Member refused " << i << std::endl;
			//member.display();
		}
	}
}

void Genetic::GenerateInitialPopulation(int nbPop){

	//displayShortestRoutes();
	std::cout << "BUILDING INITIAL POP" << std::endl;
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
		
		Routing member(routes,loads);
		member.building(metric,instance.getTabEdge().size(),instance.getTabEdge()[0].getNbSlicesC());
		if(member.feasible == true){
			//std::cout << "Member accepted "<< i << std::endl;
			population.push_back(member);
		}else{
			//std::cout << "Member refused "<< i << std::endl;
			//member.display();
		}
		
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
    std::cout << "GENERATING SHORTEST ROUTES" << std::endl;
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
                    //adjmatrix[i][edgeorigin] = getInstance().getTabEdge()[j].getLength();
                    adjmatrix[i][edgeorigin] = 1;
                }
                else{
                    if (edgeorigin == demandorigin){
                        //adjmatrix[i][edgedestination] = getInstance().getTabEdge()[j].getLength();
                        adjmatrix[i][edgedestination] = 1;
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
		//std::cout << "DISPLAYING SHORTEST " <<std::endl ;
        kpathsedges = kdijkstra(adjmatrix,originDjikistra, destinationDijikistra, chosenK+extraK);
        for (int k = 0; k < kpathsedges.size(); k++){
            double pathNoise = 0.0;
            double pathCD = 0.0;
            //std::cout << "Demand: "<< i+1 <<" -> Path from " << originDjikistra+1 << " to " << destinationDijikistra+1 << " is: " ; 
            for (int p = 0; p != kpathsedges[k].size(); ++p){
            	//std::cout << kpathsedges[k][p].getSource()+1 << "-" << kpathsedges[k][p].getTarget()+1 << "|" ;
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
            //std::cout << " OSNR " << osnrdb << " CD " << pathCD <<"|" ;
            if ((pathCD <= CDLim) && (osnrdb>=osnrLim)){
                std::vector<Fiber> candidate;
                for (int p = 0; p != kpathsedges[k].size(); ++p){
                    candidate.push_back(kpathsedges[k][p]);
                }
                kcandidates.push_back(candidate);
				//std::cout << " ACCEPTED" <<std::endl;
            }else{
                //std::cout << " REFUSED" <<std::endl;
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
	std::cout << "PRESENTING ROUTE OPTIONS "<<std::endl ;
    for (int a = 0; a < shortestRoutesByDemand.size(); a++){
        std::cout << "Demand "<< a + 1 <<std::endl ;
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
