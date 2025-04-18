#include "genetic.h"

Routing::Routing(std::vector<const std::vector<Fiber>* > r, std::vector<int> lc,std::vector<int> ll,std::vector<double> s,int b) : routes(r), loadsC(lc), loadsL(ll),spectralEff(s), bands(b),colors(), colored(false){
	for (int d = 0; d < routes.size(); ++d){
		colors.push_back(0) ;
	}
	metricVal = 0.0;
}

void Routing::computeMetric(Input::ObjectiveMetric m){
	if(m == Input::OBJECTIVE_METRIC_TUS){
		for (int a = 0; a < this->routes.size(); a++){
			int slots = loadsC[a];
			if (bands==2){
				if (colors[a]>nbSlots/2){
					slots = loadsL[a];
				}
			}
			metricVal= metricVal + this->routes[a]->size()*slots;
		}
	}
	if(m == Input::OBJECTIVE_METRIC_NLUS){
		metricVal= 1;
		for(int i = 0; i < colors.size(); ++i){
			int slots = loadsC[i];
			if (bands==2){
				if (colors[i]>nbSlots/2){
					slots = loadsL[i];
				}
			}
			if((colors[i]+slots-1)>=metricVal){
				metricVal=(colors[i]+slots-1);
			}
		}
	}
	if(m == Input::OBJECTIVE_METRIC_TRL){
		for (int a = 0; a < this->routes.size(); a++){
			for (int b = 0; b < this->routes[a]->size(); b++){
				metricVal= metricVal+ (*this->routes[a])[b].getLength();
			}
		}
	}
	if(m == Input::OBJECTIVE_METRIC_TASE){
		for (int a = 0; a < this->routes.size(); a++){
			int amplis = 0;
			for (int b = 0; b < this->routes[a]->size(); b++){
				amplis = amplis + (*this->routes[a])[b].getLineAmplifiers();
			}
			amplis = amplis + this->routes[a]->size();
			metricVal= metricVal+ amplis*spectralEff[a];
		}
	}
	if(m == Input::OBJECTIVE_METRIC_ADS){
		for(int i = 0; i < colors.size(); ++i){
			if(colors[i] == 0){
				metricVal=metricVal+1;
			}
		}
	}
	if(m ==Input::OBJECTIVE_METRIC_LLB){
		std::vector<std::vector<int> > demandsOnThisEdge;
		for (int e = 0; e < nbEdges; ++e){
			std::vector<int> list;
			demandsOnThisEdge.push_back(list) ;
		}
		for(int d = 0; d < routes.size(); ++d){
			int currentDemand = d;
			for(int l = 0; l < routes[d]->size(); ++l){
				demandsOnThisEdge[(*routes[d])[l].getId()].push_back(d);
			}
		}
		for (int e = 0; e < demandsOnThisEdge.size(); ++e){
			bool lUsed = false;
			int charge =0;
			for(int d = 0; d < demandsOnThisEdge[e].size(); ++d){
				int demand = demandsOnThisEdge[e][d];
				if(colors[demand] > (nbSlots/2)){
					lUsed = true;
				}else{
					charge = charge+loadsC[demand];
				}
			}
			if ((lUsed==true) || (charge>ceil(0.8*(nbSlots/2)))){
				metricVal = metricVal + 1;
			}
		}
	}
	if(m ==Input::OBJECTIVE_METRIC_DCB){
		for(int i = 0; i < colors.size(); ++i){
			if(colors[i] > (nbSlots/2)){
				metricVal=metricVal+1;
			}
		}
	}

}

void Routing::building(Input::ObjectiveMetric metr, int edges, int slots) {
	setNbEdges(edges);
	setNbSlots(slots);	
	/*
	std::ofstream outfile;
	outfile.open("coloring.csv", std::ios_base::app);
	double cm0;
	double cm1;
	double cm2;
	double cm3;
	double cm4;
	feasible = tryColoring(0,metr);
	if(feasible == true){
		colored = true;
		computeMetric(metr);
	}
	cm0 = metricVal;
	metricVal =0;
	for (int d = 0; d < routes.size(); ++d){
		colors[d]=0 ;
	}
	feasible = tryColoring(1,metr);
	if(feasible == true){
		colored = true;
		computeMetric(metr);
	}
	cm1 = metricVal;
	metricVal =0;
	for (int d = 0; d < routes.size(); ++d){
		colors[d]=0 ;
	}
	feasible = tryColoring(2,metr);
	if(feasible == true){
		colored = true;
		computeMetric(metr);
	}
	cm2 = metricVal;
	metricVal =0;
	for (int d = 0; d < routes.size(); ++d){
		colors[d]=0 ;
	}
	feasible = tryColoring(3,metr);
	if(feasible == true){
		colored = true;
		computeMetric(metr);
	}
	cm3 = metricVal;
	metricVal =0;
	for (int d = 0; d < routes.size(); ++d){
		colors[d]=0 ;
	}
	feasible = tryColoring(4,metr);
	if(feasible == true){
		colored = true;
		computeMetric(metr);
	}
	cm4 = metricVal;
	metricVal =0;
	for (int d = 0; d < routes.size(); ++d){
		colors[d]=0 ;
	}
	outfile << "\n" << cm0 << ";" << cm1 << ";"<< cm2 << ";"<< cm3 << ";"<< cm4;
	//std::cout<<" m0: " << cm0<<" m1: " << cm1<<" m2: " << cm2<<" m3: " << cm3<<" m4: " << cm4 <<std::endl; 
	*/

	//if((metr ==Input::OBJECTIVE_METRIC_TUS) || (metr ==Input::OBJECTIVE_METRIC_NLUS) 
	//|| (metr ==Input::OBJECTIVE_METRIC_TRL) || (metr ==Input::OBJECTIVE_METRIC_TASE) 
	//|| (metr ==Input::OBJECTIVE_METRIC_ADS)){
	if(bands==1){
		feasible = tryColoring(1,metr);
		if(feasible == true){
			colored = true;
			computeMetric(metr);
		}else{
			//std::cout << "Trying ALTERNATIVE COLORING " << std::endl;
			for (int d = 0; d < routes.size(); ++d){
				colors[d]=0 ;
			}
			feasible = tryColoring(2,metr);

			if(feasible == true){
				colored = true;
				computeMetric(metr);
				//std::cout << "ALTERNATIVE COLORING ACCEPTED " << std::endl;
			}
		}
	}
	//if((metr ==Input::OBJECTIVE_METRIC_LLB) || (metr ==Input::OBJECTIVE_METRIC_DCB)){
	if(bands==2){
		//std::cout << "trying to color" << std::endl;
		feasible = tryColoring(3,metr);
		if(feasible == true){
			colored = true;
			computeMetric(metr);
		}else{
			//std::cout << "Trying ALTERNATIVE COLORING " << std::endl;
			for (int d = 0; d < routes.size(); ++d){
				colors[d]=0 ;
			}
			feasible = tryColoring(4,metr);

			if(feasible == true){
				colored = true;
				computeMetric(metr);
				//std::cout << "ALTERNATIVE COLORING ACCEPTED " << std::endl;
			}
		}
	}	
	//so fazer um if else guardando o resultado da primeira e depois comaprando com asegunda e substituindo
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
		this->routes.clear();
	}
}

bool Routing::tryColoring(int prio, Input::ObjectiveMetric metric){
	std::vector<std::vector<int> > demandsOnThisEdge;
	std::vector<std::vector<int> > matrixDd;
	std::vector<int> priorityList;
	std::vector<sequenciator> sequenceVector;
	int priorityParam;

	priorityParam = prio;

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
		for(int l = 0; l < routes[d]->size(); ++l){
			demandsOnThisEdge[(*routes[d])[l].getId()].push_back(d);
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
	//arbirtrary priority
	if(priorityParam==0){
		for (int d = 0; d < matrixDd.size(); ++d){
			sequenciator demand;
			demand.id = d;
			demand.criteria = 1;
			sequenceVector.push_back(demand);
		}
	}
	//std::cout << "more neighboors priority " << std::endl;
	if(priorityParam==1){
		for (int d = 0; d < matrixDd.size(); ++d){
			int counter = 0;
			//std::cout << "Demand " << d << " has ";
			for (int d2 = 0; d2 < matrixDd[d].size(); ++d2){	
				if(matrixDd[d][d2] == 1){
					counter = counter + 1;
				}
			}
			//std::cout << counter << " neighboors "<<std::endl;
			sequenciator demand;
			demand.id = d;
			demand.criteria = counter;
			sequenceVector.push_back(demand);
		}
		/*
		std::cout << "Presort " << std::endl;
		for (int d = 0; d < sequenceVector.size(); ++d){
			std::cout << "Demand " << sequenceVector[d].id << " has " << sequenceVector[d].criteria << std::endl;
		}*/
		sortSequenciatorHigher sorter;
		std::sort (sequenceVector.begin(),sequenceVector.end(),sorter);
		/*std::cout << "Postsort " << std::endl;
		for (int d = 0; d < sequenceVector.size(); ++d){
			std::cout << "Demand " << sequenceVector[d].id << " has " << sequenceVector[d].criteria << std::endl;
		}*/
	}
	//std::cout << "used slots priority " << std::endl;
	if(priorityParam==2){
		for (int d = 0; d < routes.size(); ++d){
			int counter = routes[d]->size();
			//std::cout << "Demand " << d << " has " << counter << " links";
			//std::cout << " and uses " << counter*loads[d] << " slots " << std::endl;
			sequenciator demand;
			demand.id = d;
			demand.criteria = counter*loadsC[d];
			sequenceVector.push_back(demand);
		}/*
		std::cout << "Presort " << std::endl;
		for (int d = 0; d < sequenceVector.size(); ++d){
			std::cout << "Demand " << sequenceVector[d].id << " has " << sequenceVector[d].criteria << std::endl;
		}*/
		sortSequenciatorHigher sorter;
		std::sort (sequenceVector.begin(),sequenceVector.end(),sorter);
		/*std::cout << "Postsort " << std::endl;
		for (int d = 0; d < sequenceVector.size(); ++d){
			std::cout << "Demand " << sequenceVector[d].id << " has " << sequenceVector[d].criteria << std::endl;
		}*/
	}
	//priorityParam = 3;
	//std::cout << "used links priority " << std::endl;
	if(priorityParam==3){
		std::vector<sequenciator> forcedMonoband;
		std::vector<sequenciator> otherDemands;
		for (int d = 0; d < routes.size(); ++d){
			int counter = loadsC[d];
			sequenciator demand;
			demand.id = d;
			demand.criteria = counter;
			if ((loadsL[d]==0)&&(bands==2)){
				forcedMonoband.push_back(demand);
			}			
		}
		sortSequenciatorHigher sorterA;
		std::sort (forcedMonoband.begin(),forcedMonoband.end(),sorterA);
		/*std::cout << "Postsort " << std::endl;
		for (int d = 0; d < forcedMonoband.size(); ++d){
			std::cout << "Demand " << forcedMonoband[d].id << " has " << forcedMonoband[d].criteria << std::endl;
		}*/
		for (int d = 0; d < routes.size(); ++d){
			int counter = routes[d]->size();
			//std::cout << "Demand " << d << " has " << counter << " links";
			//std::cout << " and uses " << counter*loads[d] << " slots " << std::endl;
			sequenciator demand;
			demand.id = d;
			demand.criteria = counter;
			otherDemands.push_back(demand);
		}
		/*std::cout << "Presort " << std::endl;
		for (int d = 0; d < sequenceVector.size(); ++d){
			std::cout << "Demand " << sequenceVector[d].id << " has " << sequenceVector[d].criteria << std::endl;
		}*/
		sortSequenciatorHigher sorterB;
		std::sort (otherDemands.begin(),otherDemands.end(),sorterB);
		/*std::cout << "Postsort " << std::endl;
		for (int d = 0; d < otherDemands.size(); ++d){
			std::cout << "Demand " << otherDemands[d].id << " has " << otherDemands[d].criteria << std::endl;
		}*/
		std::vector<int> alreadyPriorized;
		for (int d = 0; d < loadsC.size(); ++d){
			alreadyPriorized.push_back(0);
		}
		for (int d = 0; d < forcedMonoband.size(); ++d){
			if (alreadyPriorized[forcedMonoband[d].id]==0){
				sequenciator demand;
				demand.id =forcedMonoband[d].id;
				demand.criteria = forcedMonoband[d].criteria;
				sequenceVector.push_back(demand);
				alreadyPriorized[forcedMonoband[d].id]=1;
			}else{
				//std::cout << "already added " << std::endl;
			}
		}
		for (int d = 0; d < otherDemands.size(); ++d){
			if (alreadyPriorized[otherDemands[d].id]==0){
				sequenciator demand;
				demand.id =otherDemands[d].id;
				demand.criteria = otherDemands[d].criteria;
				sequenceVector.push_back(demand);
				alreadyPriorized[otherDemands[d].id]=1;
			}else{
				//std::cout << "already added " << std::endl;
			}
		}
		/*std::cout << "Postsort " << std::endl;
		for (int d = 0; d < sequenceVector.size(); ++d){
			std::cout << "Demand " << sequenceVector[d].id << " has " << sequenceVector[d].criteria << std::endl;
		}*/
	}
	//std::cout << "most charged links priority " << std::endl;
	if(priorityParam==4){
		std::vector<int> charges;
		std::vector<sequenciator> monobandChanceEdgeOrder;
		std::vector<sequenciator> bibandEdgeOrder;
		std::vector<sequenciator> forcedMonoband;
		for (int d = 0; d < routes.size(); ++d){
			int counter = loadsC[d];
			sequenciator demand;
			demand.id = d;
			demand.criteria = counter;
			if ((loadsL[d]==0)&&(bands==2)){
				forcedMonoband.push_back(demand);
			}			
		}
		sortSequenciatorHigher sorterA;
		std::sort (forcedMonoband.begin(),forcedMonoband.end(),sorterA);
		for (int e = 0; e < nbEdges; ++e){
			int charge = 0;
			for (int d = 0; d < demandsOnThisEdge[e].size(); ++d){
				int demandId = demandsOnThisEdge[e][d];
				charge = charge + loadsC[demandId];
			}
			sequenciator edge;
			edge.criteria=charge;
			edge.id=e;
			if(bands==2){
				if (charge>(ceil(0.8*(nbSlots/2)))){
					bibandEdgeOrder.push_back(edge);
					//std::cout << "Edge " << e << " is biband "<<std::endl;
				}else{
					monobandChanceEdgeOrder.push_back(edge);
				}	
			}else{
				monobandChanceEdgeOrder.push_back(edge);
			}
		}

		/*std::cout << "Presort mono chance" << std::endl;
		for (int e = 0; e < monobandChanceEdgeOrder.size(); ++e){
			std::cout << "Edge " << monobandChanceEdgeOrder[e].id << " has " << monobandChanceEdgeOrder[e].criteria << std::endl;
		}*/
		sortSequenciatorHigher esorter;
		std::sort (monobandChanceEdgeOrder.begin(),monobandChanceEdgeOrder.end(),esorter);
		/*std::cout << "Postsort mono chance" << std::endl;
		for (int e = 0; e < monobandChanceEdgeOrder.size(); ++e){
			std::cout << "Edge " << monobandChanceEdgeOrder[e].id << " has " << monobandChanceEdgeOrder[e].criteria << std::endl;
		}
		std::cout << "Presort dual sure" << std::endl;
		for (int e = 0; e < bibandEdgeOrder.size(); ++e){
			std::cout << "Edge " << bibandEdgeOrder[e].id << " has " << bibandEdgeOrder[e].criteria << std::endl;
		}*/
		sortSequenciatorHigher esorter2;
		std::sort (bibandEdgeOrder.begin(),bibandEdgeOrder.end(),esorter2);
		/*std::cout << "Postsort dual sure" << std::endl;
		for (int e = 0; e < bibandEdgeOrder.size(); ++e){
			std::cout << "Edge " << bibandEdgeOrder[e].id << " has " << bibandEdgeOrder[e].criteria << std::endl;
		}*/
		std::vector<int> alreadyPriorized;
		for (int d = 0; d < loadsC.size(); ++d){
			alreadyPriorized.push_back(0);
		}
		for (int d = 0; d < forcedMonoband.size(); ++d){
			if (alreadyPriorized[forcedMonoband[d].id]==0){
				sequenciator demand;
				demand.id =forcedMonoband[d].id;
				demand.criteria = forcedMonoband[d].criteria;
				sequenceVector.push_back(demand);
				alreadyPriorized[forcedMonoband[d].id]=1;
			}else{
				//std::cout << "already added " << std::endl;
			}
		}
		for (int e = 0; e < monobandChanceEdgeOrder.size(); ++e){
			int edgeToSort = monobandChanceEdgeOrder[e].id;
			for (int d = 0; d < demandsOnThisEdge[edgeToSort].size(); ++d){
				int demandId = demandsOnThisEdge[edgeToSort][d];
				//std::cout << "On edge " << edgeToSort << " passes " << demandId << std::endl;
				if (alreadyPriorized[demandId]==0){
					sequenciator demand;
					demand.id = demandId;
					demand.criteria = edgeToSort;
					sequenceVector.push_back(demand);
					alreadyPriorized[demandId]=1;
				}else{
					//std::cout << "already added " << std::endl;
				}
			}
		}
		for (int e = 0; e < bibandEdgeOrder.size(); ++e){
			int edgeToSort = bibandEdgeOrder[e].id;
			for (int d = 0; d < demandsOnThisEdge[edgeToSort].size(); ++d){
				int demandId = demandsOnThisEdge[edgeToSort][d];
				//std::cout << "On edge " << edgeToSort << " passes " << demandId << std::endl;
				if (alreadyPriorized[demandId]==0){
					sequenciator demand;
					demand.id = demandId;
					demand.criteria = edgeToSort;
					sequenceVector.push_back(demand);
					alreadyPriorized[demandId]=1;
				}
				else{
					//std::cout << "already added " << std::endl;
				}
			}
		}
		/*
		std::cout << "Postsort " << std::endl;
		for (int d = 0; d < sequenceVector.size(); ++d){
			std::cout << "Demand " << sequenceVector[d].id << " has " << sequenceVector[d].criteria << std::endl;
		}*/
	}
	/*
	for (int a = 0; a < routes.size(); a++){
		std::cout << "Demand "<< a + 1 << " || Slots : "<<colors[a]<<"-"<< colors[a] + loads[a] -1 << " |||";
		for (int b = 0; b < routes[a]->size(); b++){
			std::cout << (*routes[a])[b].getSource()+1 << "-" << (*routes[a])[b].getTarget()+1 << "|" ;
		}
		std::cout <<std::endl ;
	}
	std::cout << "SORTED" << std::endl;
		for (int d = 0; d < sequenceVector.size(); ++d){
			std::cout << "Demand " << sequenceVector[d].id +1<< " has " << sequenceVector[d].criteria << std::endl;
	}

	for (int d = 0; d < routes.size(); ++d){
		for (int d2 = 0; d2 < routes.size(); ++d2){
			std::cout << matrixDd[d][d2] << "|";
		}
		std::cout << std::endl;
		
	
	std::cout<< "TRYING TO COLOR" <<std::endl ;

	for (int d = 0; d < routes.size(); ++d){
		std::cout<< "demand " << d << " with "<< loads[d] << std::endl;
	}
	}*/
	bool canColor = true;
	for (int d = 0; d < routes.size(); ++d){

		int coloringTarget = sequenceVector[d].id;
		int feasible = true;
		int candidateColor = 1;
		int rightSlot = candidateColor + loadsC[coloringTarget] - 1;
		int nextLowestColor = 1;
		int chosenColor = 0;
		bool ovDetected = true;
		int slotLimit = nbSlots-loadsC[coloringTarget] + 1;
		int bandLimit = slotLimit;
		if(bands==2){
			bandLimit= (nbSlots/2)-loadsC[coloringTarget] + 1;
			slotLimit = nbSlots-loadsL[coloringTarget] + 1;
			if (loadsL[coloringTarget]==0){
				slotLimit = nbSlots-loadsC[coloringTarget] + 1;
				bandLimit = slotLimit;
			}
		}
		//std::cout<< "demand " << coloringTarget << std::endl ;
		while(ovDetected == true){
			ovDetected = false;
			//std::cout<< "Looking for overlapping between slots "<< candidateColor << "and " << rightSlot <<std::endl ;
			for (int d2 = 0; d2 < routes.size(); ++d2){
				if(matrixDd[coloringTarget][d2] == 1){
					//std::cout<< "First neighboor "<< d2 << std::endl;
					int neighboorLeft =  colors[d2] ;
					if (neighboorLeft == 0){
						//std::cout<< "Not colored" <<std::endl;
					}else{
						int neighboorSlots = loadsC[d2];
						if (bands==2){
							if (colors[d2]>nbSlots/2){
								neighboorSlots = loadsL[d2];
							}
						}
						int neighboorRight =  colors[d2] + neighboorSlots-1;
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
			if((nextLowestColor > slotLimit)){
				feasible = false;
				ovDetected = false;
				if(metric ==Input::OBJECTIVE_METRIC_ADS){
					feasible = true;
					chosenColor = 0;
					colors[coloringTarget]=chosenColor;
				}
			}else{
				if(ovDetected == false){
					feasible = true;
					chosenColor = candidateColor;
					//std::cout<< "demand " << coloringTarget << " gets " <<chosenColor << std::endl ;
					colors[coloringTarget]=chosenColor;
				}else{
					candidateColor = nextLowestColor;
					int slotsToUse = loadsC[coloringTarget];
					if(bands==2){
						if (candidateColor > bandLimit){
							if(candidateColor <= bandLimit+loadsC[coloringTarget] - 1){
								candidateColor = bandLimit+loadsC[coloringTarget];
								//std::cout << "cant be between bands, next candidate = " <<candidateColor <<std::endl;
							}
						}
						if (candidateColor >nbSlots/2){
							slotsToUse = loadsL[coloringTarget];
						}
					}
					rightSlot = candidateColor + slotsToUse - 1;
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
			int rightSlot1 = colors[d] + loadsC[d] -1;
			if (bands==2){
				if (colors[d]>nbSlots/2){
					rightSlot1 = colors[d] + loadsL[d] -1;
				}
			}
			if (leftSlot1 != 0){
				//std::cout<< " verificando demanda "<< d << " entre " << leftSlot1 <<"-"<< rightSlot1 << std::endl;	
				for (int d2 = 0; d2 < routes.size(); ++d2){
					int leftSlot2 = colors[d2];
					int rightSlot2 = colors[d2] + loadsC[d2] -1;
					if (bands==2){
						if (colors[d2]>nbSlots/2){
							rightSlot2 = colors[d2] + loadsL[d2] -1;
						}
					}
					if (leftSlot2 != 0){
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
		int slots = loadsC[a];
		if (bands==2){
			if (colors[a]>nbSlots/2){
				slots = loadsL[a];
			}
		}
		if (colors[a]!=0){
			std::cout << "Demand "<< a + 1 << " || Slots : "<<colors[a]<<"-"<< colors[a] + slots -1 << " |||";
		}else{
			std::cout << "Demand "<< a + 1 << " || Slots : "<<colors[a]<<"-"<< colors[a] << " |||";
		}
		for (int b = 0; b < routes[a]->size(); b++){
			std::cout << (*routes[a])[b].getSource()+1 << "-" << (*routes[a])[b].getTarget()+1 << "|" ;
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

void Genetic::run(){
	std::cout<<"Genetic metric: "<< instance.getInput().geneticAlgorithmMetric()<<std::endl;
	std::cout<<"Genetic iterations: "<< instance.getInput().geneticAlgorithmIterations()<<std::endl;
	std::cout<<"Genetic population "<< instance.getInput().geneticAlgorithmPopulation()<<std::endl;
	std::cout<<"Genetic crossing tax "<< instance.getInput().geneticAlgorithmCrossing()<<std::endl;
	std::cout<<"Genetic mutation tax "<< instance.getInput().geneticAlgorithmMutation()<<std::endl;
	std::cout<<"Genetic chosenK "<< instance.getInput().geneticGetChosenK()<<std::endl;
	std::cout<<"Genetic extraK "<<  instance.getInput().geneticGetExtraK()<<std::endl;
	bool stop = false;
	coloringTime =0.0;
	initialPopTime = 0.0;
	crossingTime = 0.0;
    mutationTime= 0.0;
    consolidatingSelectedTime= 0.0;
    selectionTime= 0.0;
	ClockTime clockHeuristc(ClockTime::getTimeNow());
	clockHeuristc.setStart(ClockTime::getTimeNow());
	GenerateShortestRoutes2();
	//displayShortestRoutes();
    //GenerateShortestRoutes();
	//displayShortestRoutes();

	nbInitialPop =  instance.getInput().geneticAlgorithmPopulation();
	
	std::ofstream outfile;
	//outfile.open("coloring.csv", std::ios_base::app);
	//outfile << "\ncm0;cm1;cm2;cm3;cm4;";
	//std::ofstream outfile;
	//outfile.open("genetic.csv");
	//outfile << "\nfirst;last";
	GenerateInitialPopulation(nbInitialPop);
	if (population.size() == 0){
		throw std::runtime_error("Not found feasible member for the population");
	}
	//for (int i = 0; i < population.size(); i++){
	//	population[i].display();
	//}
	currentBest = INT_MAX;
	int inputIterations = instance.getInput().geneticAlgorithmIterations();
	int currentIt = 0;
	int itWithoutImprovement=0;
	bool improvementPhase = false;

	while(stop == false){
		currentIt = currentIt +1;
		std::cout<<"==========BEGIN IT: "<< currentIt<<std::endl;
		doCrossing();
		//for (int i = 0; i < thisIterationCrossing.size(); i++){
		////	thisIterationCrossing[i].display();
		//}
		doMutation();
		//for (int i = 0; i < thisIterationMutation.size(); i++){
		//	thisIterationMutation[i].display();
		//}
		doSelection();
		//for (int i = 0; i < population.size(); i++){
		//	population[i].display();
		//}
		std::cout<<"First place: "<< population[0].metricVal<<std::endl;
		std::cout<<"Last place: "<< population[population.size()-1].metricVal<<std::endl;
		std::string	f = std::to_string(population[0].metricVal);
		std::string	l = std::to_string(population[population.size()-1].metricVal);
		//outfile << "\n" + f + ";" << l;

		std::cout<<"==========FIN IT: "<< currentIt<<std::endl;
		if(improvementPhase ==false){
			if (currentBest > population[0].metricVal) {
				std::cout<<"UPDATE BEST SOL"<<std::endl;
				currentBest = population[0].metricVal;
				itToBest = currentIt;
				timeToBest =  clockHeuristc.getTimeInSecFromStart();
			}
			if(currentIt >=inputIterations) {
				std::cout<<"NUMBER OF ITERATIONS ATTAINED"<<std::endl;
				std::cout<<"Continuing until 10 iterations without improvement"<<std::endl;
				improvementPhase = true;	
			}
		}else{
			if (currentBest > population[0].metricVal) {
				std::cout<<"UPDATE BEST SOL"<<std::endl;
				currentBest = population[0].metricVal;
				itToBest = currentIt;
				timeToBest =  clockHeuristc.getTimeInSecFromStart();
				itWithoutImprovement=0;
			}else{
				itWithoutImprovement = itWithoutImprovement +1;
				std::cout<<"No improvement for " << itWithoutImprovement << " it"<<std::endl;
			}
			if (itWithoutImprovement >= 10) {
				stop = true;
				for (int i = 0; i < 1; i++){
					population[i].display();
				}
			}
		}
	}

	heuristicTime = clockHeuristc.getTimeInSecFromStart();
	std::cout<<"Best SOL: "<< currentBest<<std::endl;
	std::cout<<"Found at it: "<< itToBest<<" after " << timeToBest <<std::endl;
	std::cout<<"Total time: "<< heuristicTime<<std::endl;
	std::cout<<"K-shortest: "<< kShortestTime<<std::endl;
	std::cout<<"Generating initial "<< initialPopTime<<std::endl;
	std::cout<<"Crossing "<< crossingTime<<std::endl;
	std::cout<<"Mutation "<< mutationTime<<std::endl;
	std::cout<<"Consolidating "<< consolidatingSelectedTime<<std::endl;
	std::cout<<"Sorting "<< selectionTime<<std::endl;
	std::cout<<"Coloring "<< coloringTime<<std::endl;
	std::cout<<"Others "<< heuristicTime-kShortestTime-initialPopTime-crossingTime-mutationTime-consolidatingSelectedTime-selectionTime-coloringTime<<std::endl;
}

Genetic::Genetic(const Instance &inst) : instance(inst),rng(std::random_device{}()){
	this->setToBeRouted(instance.getNextDemands());
	setLoadCVector();
	setLoadLVector();
	setSpectralEffVector();
    //displayToBeRouted();
	chosenK = instance.getInput().geneticGetChosenK();
	extraK = instance.getInput().geneticGetExtraK();
	metric = instance.getInput().geneticAlgorithmMetric();
	nbBands = instance.getInput().getNbBands();
	nbSlots = nbBands*instance.getTabEdge()[0].getNbSlicesC();
}
	
std::vector<std::vector<int>> Genetic::buildMatrixKsol(int k){
	for (int i = 0; i < instance.getTabEdge().size(); ++i){	
		std::vector<int> thisEdgeSlots;
		for (int j = 0; j < nbSlots; ++j){	
			thisEdgeSlots.push_back(0);
		}
		edgeSlotMap.push_back(thisEdgeSlots);
	}
	/*
	for (int i = 0; i < edgeSlotMap.size(); ++i){	
		std::vector<int> thisEdgeSlots;
		for (int j = 0; j < edgeSlotMap[i].size(); ++j){	
			std::cout << edgeSlotMap[i][j] << " ";
		}
		std::cout << std::endl; 
	}
	*/
	//population[k].display();
	for (int a = 0; a < population[k].routes.size(); a++){
		int demand = a +1;
		int slots = population[k].loadsC[a];
		if (nbBands=2){
			if (population[k].colors[a]>nbSlots/2){
				slots = population[k].loadsL[a];
			}
		}
		int lastSlot = population[k].colors[a] + slots -1;
		for (int b = 0; b < population[k].routes[a]->size(); b++){
			int edge = (*population[k].routes[a])[b].getIndex();
			if (population[k].colors[a] != 0){
				edgeSlotMap[edge][lastSlot-1] = demand;
			}
		}
	}
	/*std::cout <<  "Edge slot map "<< std::endl; 
	for (int i = 0; i < edgeSlotMap.size(); ++i){	
		std::vector<int> thisEdgeSlots;
		std::cout << i + 1 <<  "|  ";
		for (int j = 0; j < edgeSlotMap[i].size(); ++j){	
			std::cout << edgeSlotMap[i][j] << " ";
		}
		std::cout << std::endl; 
	}*/
	return edgeSlotMap;
}

std::vector<int> Genetic::buildLastSlotByDemand(int k){
	std::vector<int> slotsByDemand;
	//population[k].display();
	for (int a = 0; a < population[k].colors.size(); a++){
		int slots = population[k].loadsC[a];
		if (nbBands=2){
			if (population[k].colors[a]>nbSlots/2){
				slots = population[k].loadsL[a];
			}
		}
		if (population[k].colors[a] != 0){
			slotsByDemand.push_back(population[k].colors[a] + slots -1);
		}else{
			slotsByDemand.push_back(0);
		}
		
	}
	/*std::cout <<  "Last slot by demand "<< std::endl; 
	for (int a = 0; a < slotsByDemand.size(); a++){
		std::cout << "Demand " << a+1 << " s: " << slotsByDemand[a] << std::endl;
	}*/
	
	lastSlotDemand = slotsByDemand;
	return lastSlotDemand;
}

std::vector<std::vector<int>> Genetic::buildPathNodesByDemand(int k){
	std::vector<std::vector<int> > nodesListDemand;
	//population[k].display();
	for (int a = 0; a < population[k].routes.size(); a++){
		//std::cout  << "Demand " << a+1<< std::endl;
		std::vector<int> nodesThisDemand;
		if (population[k].colors[a] != 0){
			int currentNode = toBeRouted[a].getSource()+1;
			//std::cout  << "First Node " << currentNode<< std::endl;
			nodesThisDemand.push_back(currentNode);
			for (int b = 0; b < (*population[k].routes[a]).size(); b++){
				int u = (*population[k].routes[a])[b].getSource()+1;
				int v = (*population[k].routes[a])[b].getTarget()+1;
				//std::cout  << "looking edge " <<  u  << " " << v << std::endl;
				//std::cout  << "for" <<  currentNode << std::endl;
				if (u != currentNode){
					nodesThisDemand.push_back(u);
					currentNode = u;
				}else{
					if ( v != currentNode){
						nodesThisDemand.push_back(v);
						currentNode = v;
					}
				}
			}
		}
		nodesListDemand.push_back(nodesThisDemand);
	}
	/*std::cout <<  "Node List by demand "<< std::endl; 
	
	for (int i = 0; i < nodesListDemand.size(); ++i){	
		std::cout << "Demand " << i+1 << " : ";
		for (int j = 0; j < nodesListDemand[i].size(); ++j){	
			std::cout << nodesListDemand[i][j] << " ";
		}
		std::cout << std::endl; 
	}*/
	nodesByDemand=nodesListDemand;
	return nodesByDemand;
}
void Genetic::setLoadCVector(){
	for (int i = 0; i < toBeRouted.size(); ++i){
		loadsC.push_back(toBeRouted[i].getLoadC());
	}
}
void Genetic::setLoadLVector(){
	for (int i = 0; i < toBeRouted.size(); ++i){
		loadsL.push_back(toBeRouted[i].getLoadL());
	}
}
void Genetic::setSpectralEffVector(){
	for (int i = 0; i < toBeRouted.size(); ++i){
		spectralEffC.push_back(toBeRouted[i].getGBits()/toBeRouted[i].getLoadC());
	}
}

void Genetic::doSelection(){
	ClockTime clockConsolidate(ClockTime::getTimeNow());
	clockConsolidate.setStart(ClockTime::getTimeNow());

	std::vector<sequenciator> sequenceVector;
	int posMaxOldPop = population.size()-1;
	int posMaxCrossingPop = population.size() + thisIterationCrossing.size()-1;
	int posMaxMutationPop = population.size() + thisIterationCrossing.size() + thisIterationMutation.size()-1;
	int pos = 0;
	int totalPop = population.size() + thisIterationCrossing.size() + thisIterationMutation.size();
	//std::cout << "pos 1: " << posMaxOldPop << std::endl;
	//std::cout << "pos 2: " << posMaxCrossingPop << std::endl;
	//std::cout << "pos 3: " << posMaxMutationPop << std::endl;
	//std::cout << "OLD POP: " << population.size();
	for (int i = 0; i < population.size(); ++i){
		sequenciator popMember;
		popMember.id = pos;
		popMember.criteria = population[i].metricVal;
		sequenceVector.push_back(popMember);
		pos = pos +1;
	}
	//std::cout << ", ADDING CROSSED: " << thisIterationCrossing.size() ;
	for (int i = 0; i < thisIterationCrossing.size(); ++i){
		sequenciator popMember;
		popMember.id = pos;
		popMember.criteria = thisIterationCrossing[i].metricVal;
		sequenceVector.push_back(popMember);
		pos = pos +1;
	}
	//std::cout << ", ADDING MUTANTS: " << thisIterationMutation.size();
	for (int i = 0; i < thisIterationMutation.size(); ++i){
		sequenciator popMember;
		popMember.id = pos;
		popMember.criteria = thisIterationMutation[i].metricVal;
		sequenceVector.push_back(popMember);
		pos = pos +1;
	}
	//std::cout << " = TOTAL POP : " << totalPop << std::endl;
	consolidatingSelectedTime = consolidatingSelectedTime+ clockConsolidate.getTimeInSecFromStart();
	//for (int d = 0; d < sequenceVector.size(); ++d){
	//	std::cout << "id : " << sequenceVector[d].id  << "metric : " << sequenceVector[d].criteria << std::endl;
	//}


	ClockTime clockSelect(ClockTime::getTimeNow());
	clockSelect.setStart(ClockTime::getTimeNow());
	//std::cout << "SELECTING POP";
	
	// SORTING
	sortSequenciatorLower sorter2;
	std::sort (sequenceVector.begin(),sequenceVector.end(),sorter2);
	selectionTime = selectionTime+ clockSelect.getTimeInSecFromStart();
	//std::cout << "CONSOLIDATING DATA" << std::endl;

	clockConsolidate.setStart(ClockTime::getTimeNow());
	int toDelete = totalPop - nbInitialPop;
	if (toDelete < 0){
		toDelete = 0;
	}
	//std::cout << ", TO DELETE: " << toDelete << std::endl;
	//std::cout << "BUILDING NEW POP" << std::endl;
	int limit = nbInitialPop;
	if (limit > totalPop){
		limit = totalPop;
	}
	//std::cout << "limit "<<limit << std::endl;
	for (int i = 0; i < limit; ++i){
		std::vector<const std::vector<Fiber>* > routes;
		//std::cout << "looking for: "<< sequenceVector[i].id << std::endl;
		if(sequenceVector[i].id <= posMaxOldPop){
			int element = sequenceVector[i].id;
			//std::cout << "in pos: "<< element << std::endl;
			for (int d = 0; d < population[element].routes.size(); ++d){
				routes.push_back(population[element].routes[d]);
			}
			Routing member(routes,loadsC,loadsL,spectralEffC,nbBands);
			member.copying(population[element]);
			thisIterationTotalPop.push_back(member);
		}else{
			if(sequenceVector[i].id <= posMaxCrossingPop){
				int element = sequenceVector[i].id-population.size();
				//std::cout << "in pos: "<< element << std::endl;
				for (int d = 0; d < thisIterationCrossing[element].routes.size(); ++d){
					routes.push_back(thisIterationCrossing[element].routes[d]);
				}

				Routing member(routes,loadsC,loadsL,spectralEffC,nbBands);
				member.copying(thisIterationCrossing[element]);
				thisIterationTotalPop.push_back(member);
			}else{
				if(sequenceVector[i].id <= posMaxMutationPop){
					int element = sequenceVector[i].id-thisIterationCrossing.size()-population.size();
					//std::cout << "in pos: "<< element << std::endl;
					for (int d = 0; d < thisIterationMutation[element].routes.size(); ++d){
						routes.push_back(thisIterationMutation[element].routes[d]);
					}
					Routing member(routes,loadsC,loadsL,spectralEffC,nbBands);
					member.copying(thisIterationMutation[element]);
					thisIterationTotalPop.push_back(member);
				}else{
					std::cout << "ERROR"<<std::endl;
				}
			}
		}
	}

	while(population.size()>0){
		population.pop_back();
	}
	while(thisIterationCrossing.size()>0){
		thisIterationCrossing.pop_back();
	}
	while(thisIterationMutation.size()>0){
		thisIterationMutation.pop_back();
	}
	population = thisIterationTotalPop;
	//std::cout << "CLEANING SELECTED VECTOR" << std::endl;
	while(thisIterationTotalPop.size()>0){
		thisIterationTotalPop.pop_back();
	}
	consolidatingSelectedTime = consolidatingSelectedTime+ clockConsolidate.getTimeInSecFromStart();
}

void Genetic::doMutation(){
	//std::cout << "MUTATION OPERATION" << std::endl;

	int nbMutations = floor(population.size()* ((double)instance.getInput().geneticAlgorithmMutation()/100.0));
	//int nbMutations = floor(population.size());  
	std::vector<int> canMutate;
		
	for (int d = 0; d < toBeRouted.size(); ++d){
		if (shortestRoutesByDemand[d].size()>chosenK){
			canMutate.push_back(d);
		}
	}
	/*
	std::cout << "DEMANDES AVEC MUTATION POSSIBLE " << canMutate.size() << std::endl;
	for (int d = 0; d < canMutate.size(); ++d){
		std::cout <<  canMutate[d]+1 << std::endl;
	}*/
	int mutationsByMember = ceil(canMutate.size()/10.0);
	//std::cout << "MUTATION AUTORISES " << mutationsByMember << std::endl;
	for (int i = 0; i < nbMutations; ++i){
		ClockTime clockMutation(ClockTime::getTimeNow());
		clockMutation.setStart(ClockTime::getTimeNow());
		int mutant = getRandomNumber(0, population.size()-1);
		//std::cout << "MUTANT " << mutant + 1 << std::endl;
		std::vector<int> refusedInThisMutant;
		std::vector<int> priorityMutateList;
		if (metric == Input::OBJECTIVE_METRIC_ADS){
			for (int d = 0; d < toBeRouted.size(); ++d){
				if (population[mutant].colors[d] == 0){
					refusedInThisMutant.push_back(d);
				}
			}
			/*
			std::cout << "DEMANDES REFUSES IN MUTANT " << refusedInThisMutant.size() << std::endl;
			for (int d = 0; d < refusedInThisMutant.size(); ++d){
				std::cout <<  refusedInThisMutant[d]+1 << std::endl;
			}
			*/
			for (int d = 0; d < refusedInThisMutant.size(); ++d){
				bool refusedAndCanMutate = std::find(std::begin(canMutate),std::end(canMutate), refusedInThisMutant[d]) != std::end(canMutate);		
				if (refusedAndCanMutate == true){
					priorityMutateList.push_back(refusedInThisMutant[d]);
				}
			}
			/*
			std::cout << "PRIORITY TO MUTATE " << priorityMutateList.size() << std::endl;
			for (int d = 0; d < priorityMutateList.size(); ++d){
				std::cout <<  priorityMutateList[d]+1 << std::endl;
			}
			*/
		}
		
		std::vector<const std::vector<Fiber>* > routes;
		
		int genesMutated = 0;
		std::vector<int> thisMutationList;
		while(genesMutated < mutationsByMember){
			if (priorityMutateList.size()>0){
				int positionInAuxiliary = getRandomNumber(0, priorityMutateList.size()-1);
				int toMutateNow = priorityMutateList[positionInAuxiliary];
				thisMutationList.push_back(toMutateNow);
				priorityMutateList.erase(priorityMutateList.begin()+positionInAuxiliary);
				genesMutated = genesMutated +1;
				//std::cout << "ADD " << toMutateNow+1 << std::endl;
			}else{
				//Aqui eu posso melhorar para pegar e deletar de um vetor auxiliar ao inves de insistir no random
				int positionInAuxiliary = getRandomNumber(0, canMutate.size()-1);
				int toMutateNow = canMutate[positionInAuxiliary];
				//std::cout << "CHOICE " << toMutateNow << std::endl;
				bool alreadyChosen = std::find(std::begin(thisMutationList),std::end(thisMutationList), toMutateNow) != std::end(thisMutationList);
				if(alreadyChosen == false){
					thisMutationList.push_back(toMutateNow);
					genesMutated = genesMutated +1;
					//std::cout << "ADD " << toMutateNow << std::endl;
				}
			}
		}
		for (int d = 0; d < toBeRouted.size(); ++d){
			bool mutantGene = std::find(std::begin(thisMutationList),std::end(thisMutationList), d) != std::end(thisMutationList);
			if (mutantGene  == true){
				//std::cout << "NEW gene  " << d + 1 << std::endl;
				int options = shortestRoutesByDemand[d].size() - chosenK;
				int newGene = getRandomNumber(0, options-1);		
				routes.push_back(&shortestRoutesByDemand[d][newGene+chosenK]);
				//std::cout << shortestRoutesByDemand[d][newGene+chosenK][l].getSource()+1 << "-" << shortestRoutesByDemand[d][newGene+chosenK][l].getTarget()+1 << "|" ;

			}else{
				//std::cout << "gene  " << d + 1 << std::endl;
				routes.push_back(population[mutant].routes[d]);
			}
		}
		Routing member(routes,loadsC,loadsL,spectralEffC,nbBands);
		mutationTime = mutationTime + clockMutation.getTimeInSecFromStart();
		ClockTime clockColoring(ClockTime::getTimeNow());
		clockColoring.setStart(ClockTime::getTimeNow());
		member.building(metric,instance.getTabEdge().size(),nbSlots);
		coloringTime =coloringTime+ clockColoring.getTimeInSecFromStart();
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
	//std::cout << "CROSSING OPERATION" << std::endl;

	int nbCrossings = floor(population.size()* ((double)instance.getInput().geneticAlgorithmCrossing()/100.0)); 
	//int nbCrossings = floor(population.size()); 
	for (int i = 0; i < nbCrossings; ++i){
		ClockTime clockCrossing(ClockTime::getTimeNow());
		clockCrossing.setStart(ClockTime::getTimeNow());
		int progenitor1;
		int progenitor2;
		progenitor1 = getRandomNumber(0, population.size()-1);
		progenitor2 = getRandomNumber(0, population.size()-1);
		std::vector<const std::vector<Fiber>* > routes;
		//std::cout << "crossing " << progenitor1 + 1<< "and " << progenitor2 + 1<< std::endl;
		for (int d = 0; d < toBeRouted.size(); ++d){
			int randProgenitor = getRandomNumber(1, 2); //random between 1 and 2
			if (randProgenitor == 1){
				routes.push_back(population[progenitor1].routes[d]);
			}else{
				routes.push_back(population[progenitor2].routes[d]);
			}
		}
		
		Routing member(routes,loadsC,loadsL,spectralEffC,nbBands);
		crossingTime = crossingTime + clockCrossing.getTimeInSecFromStart();
		ClockTime clockColoring(ClockTime::getTimeNow());
		clockColoring.setStart(ClockTime::getTimeNow());
		member.building(metric,instance.getTabEdge().size(),nbSlots);
		coloringTime =coloringTime+ clockColoring.getTimeInSecFromStart();
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
	std::cout << "BUILDING INITIAL POP" << std::endl;
	for (int i = 0; i < nbPop; ++i){
		
		ClockTime clockInitial(ClockTime::getTimeNow());
		clockInitial.setStart(ClockTime::getTimeNow());
		std::vector<const std::vector<Fiber>* > routes;
		for (int d = 0; d < toBeRouted.size(); ++d){
			int randomChoice;
			int upperlimit;
			if (shortestRoutesByDemand[d].size()<chosenK){
				upperlimit = shortestRoutesByDemand[d].size();
			}else{
				upperlimit = chosenK;
			}
			randomChoice = getRandomNumber(0, upperlimit-1); 
			routes.push_back(&shortestRoutesByDemand[d][randomChoice]);
			//std::cout << shortestRoutesByDemand[d][0][l].getSource()+1 << "-" << shortestRoutesByDemand[d][0][l].getTarget()+1 << "|" ;
		}
            
		Routing member(routes,loadsC,loadsL,spectralEffC,nbBands);
		initialPopTime = initialPopTime + clockInitial.getTimeInSecFromStart();
		ClockTime clockColoring(ClockTime::getTimeNow());
		clockColoring.setStart(ClockTime::getTimeNow());
		member.building(metric,instance.getTabEdge().size(),nbSlots);
		coloringTime =coloringTime+ clockColoring.getTimeInSecFromStart();
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

void Genetic::GenerateShortestRoutes2(){
    std::cout << "GENERATING SHORTEST ROUTES" << std::endl;
	ClockTime clockShortest(ClockTime::getTimeNow());
	clockShortest.setStart(ClockTime::getTimeNow());
	//Building a structure with all possible paths for all possible demands

    // ADJACENCY LIST
    std::vector<std::vector<int> > aux(instance.getNbNodes());
    adj_list = aux;
    //std::cout << "Edges:";
    for (int i=0; i< instance.getTabEdge().size(); ++i){
		int origin = instance.getTabEdge()[i].getSource();
		int dest = instance.getTabEdge()[i].getTarget();
        adj_list[origin].push_back(dest);
        adj_list[dest].push_back(origin);
    }
	/*
	for (int i=0; i< adj_list.size(); ++i){
		for (int j=0; j< adj_list[i].size(); ++j){
			std::cout<< adj_list[i][j]+1 << " ";
		}
		std::cout<<std::endl;
	}
	*/
    int origin;
    int destination;
    for (int i = 0; i < toBeRouted.size(); ++i){
        origin = toBeRouted[i].getSource();
        destination =  toBeRouted[i].getTarget();
        // Mark all the vertices as not visited
        bool* visited = new bool[instance.getNbNodes()];
        // Create an array to store paths
        int* path = new int[instance.getNbNodes()];
        int path_index = 0; // Initialize path[] as empty
        
        // Initialize all vertices as not visited
        for (int i = 0; i < instance.getNbNodes(); i++)
            visited[i] = false;
        
        // Call the recursive helper function to print all paths
        AllPathsUtil(origin, destination, visited, path, path_index);
        allpaths.push_back(pathsdemand);
        pathsdemand.clear();
    }
	std::vector<std::vector<double> > alldemandsdistances;
	std::vector<std::vector<double> > alldemandsDispersionC;
	std::vector<std::vector<double> > alldemandsNoiseC;
	std::vector<std::vector<double> > alldemandsDispersionL;
	std::vector<std::vector<double> > alldemandsNoiseL;
	std::vector<std::vector<int> > alldemandsNbEdges;
	std::vector<std::vector<int> > alldemandsAmplis;
	std::vector<double> thisdemanddistances;
	std::vector<double> thisdemandDispersionC;
	std::vector<double> thisdemandNoiseC;
	std::vector<double> thisdemandDispersionL;
	std::vector<double> thisdemandNoiseL;
	std::vector<int> thisdemandNbEdges;
	std::vector<int> thisdemandAmplis;
	int currentnode;
    int nextnode;
    double length;
	double dispersionPathC;
    double noisePathC;
	double dispersionPathL;
    double noisePathL;
	int nbEdges;
	int amplis;

    for (int i = 0; i <allpaths.size(); ++i){
        for (int j = 0; j <allpaths[i].size(); ++j){
            noisePathC = 0.0;
            length = 0.0;
			nbEdges= 0;
			dispersionPathC =0.0;
			amplis = 0;
			noisePathL = 0.0;
			dispersionPathL =0.0;
            for (int k = 0; k <allpaths[i][j].size()-1; ++k){
                currentnode = allpaths[i][j][k];
                nextnode = allpaths[i][j][k+1];
                length += instance.getPhysicalLinkBetween(currentnode,nextnode).getLength();
				dispersionPathC += (instance.getPhysicalLinkBetween(currentnode,nextnode).getDispersionCoeffC()*instance.getPhysicalLinkBetween(currentnode,nextnode).getLength());
                noisePathC += instance.getPhysicalLinkBetween(currentnode,nextnode).getNoiseC();
				dispersionPathL += (instance.getPhysicalLinkBetween(currentnode,nextnode).getDispersionCoeffL()*instance.getPhysicalLinkBetween(currentnode,nextnode).getLength());
                noisePathL += instance.getPhysicalLinkBetween(currentnode,nextnode).getNoiseL();
				nbEdges = nbEdges+1;
				amplis += instance.getPhysicalLinkBetween(currentnode,nextnode).getLineAmplifiers();
			}			
			thisdemanddistances.push_back(length);
			thisdemandDispersionC.push_back(dispersionPathC);
            thisdemandNoiseC.push_back(noisePathC);
			thisdemandDispersionL.push_back(dispersionPathL);
            thisdemandNoiseL.push_back(noisePathL);
            thisdemandNbEdges.push_back(nbEdges);
			thisdemandAmplis.push_back(amplis+nbEdges); //summing line amplifiers + node amplifiers

        }
        alldemandsdistances.push_back(thisdemanddistances);
		alldemandsDispersionC.push_back(thisdemandDispersionC);
        alldemandsNoiseC.push_back(thisdemandNoiseC);
		alldemandsDispersionL.push_back(thisdemandDispersionL);
        alldemandsNoiseL.push_back(thisdemandNoiseL);
        alldemandsNbEdges.push_back(thisdemandNbEdges);
		alldemandsAmplis.push_back(thisdemandAmplis);
        thisdemanddistances.clear();
		thisdemandDispersionC.clear();
        thisdemandNoiseC.clear();
		thisdemandDispersionL.clear();
        thisdemandNoiseL.clear();
		thisdemandNbEdges.clear();
		thisdemandAmplis.clear();
    }
	/*
	for (int i=0; i< allpaths.size(); ++i){
		std::cout<< " demand " << i <<std::endl;
		for (int j=0; j< allpaths[i].size(); ++j){
			std::cout<< " option " << j << " : ";
			for (int k=0; k< allpaths[i][j].size(); ++k){
				std::cout<< allpaths[i][j][k]+1 << " ";
			}
			std::cout <<  " length" << alldemandsdistances[i][j]<< "|" ;
			std::cout <<  " noise" << alldemandsNoiseC[i][j] << "|" ;
			std::cout <<  " nb edges" << alldemandsNbEdges[i][j] << "|" <<std::endl;
			std::cout <<  " amplis" << alldemandsAmplis[i][j] << "|" <<std::endl;
			std::cout<<std::endl;
		}
		std::cout<<std::endl;
	}
	*/
	for (int i = 0; i <allpaths.size(); ++i){
	//for each demand we will build all auxiliars
		//std::cout << "for demand " << i +1 << " limits OSNR " << toBeRouted[i].getOsnrLimitC() <<
		//" limits cd " << toBeRouted[i].getmaxCDC() << std::endl;
		std::vector<prePath> qualifiedForThisDemand;
		for (int j = 0; j <allpaths[i].size(); ++j){
		//for route we will evaluate QoT
			double dbOsnrC = osnrPath(alldemandsNoiseC[i][j], toBeRouted[i].getPchC());
			double dispersionC = alldemandsDispersionC[i][j];
			double dbOsnrL = osnrPath(alldemandsNoiseL[i][j], toBeRouted[i].getPchL());
			double dispersionL = alldemandsDispersionL[i][j];
			if((nbBands==1)||(toBeRouted[i].getLoadL()==0)){
				if (dispersionC <= (toBeRouted[i].getmaxCDC()) && dbOsnrC >= toBeRouted[i].getOsnrLimitC()){
					//if qot ok we add it to set
					prePath aux;
					aux.length = alldemandsdistances[i][j];
					aux.noise = alldemandsNoiseC[i][j];
					aux.nbEdges = alldemandsNbEdges[i][j];
					aux.amplis = alldemandsAmplis[i][j];
					std::vector<Fiber> auxin;
					for (int k = 0; k <allpaths[i][j].size()-1; ++k){
						auxin.push_back(instance.getPhysicalLinkBetween(allpaths[i][j][k],allpaths[i][j][k+1]));
					}	
					aux.links = auxin;
					qualifiedForThisDemand.push_back(aux);
					/*
					for (int i = 0; i <aux.links.size(); ++i){
						std::cout << aux.links[i].getSource()+1 << "-" << aux.links[i].getTarget()+1 << "|" ;
					}
					std::cout <<  " dispersion " << aux.length*17 << "|" ;
					std::cout <<  " noise " << aux.noise << "|" ;
					std::cout <<  " nb edges " << aux.nbEdges << "|";
					std::cout <<  " amplis " << aux.amplis << "|";
					std::cout <<  " osnr " << dbOsnrC << "|" <<std::endl;
					*/
				}
			}else{
			//if(nbBands==2){
				if ((dispersionC <= (toBeRouted[i].getmaxCDC())) && (dbOsnrC >= toBeRouted[i].getOsnrLimitC()) &&  (dispersionL <= (toBeRouted[i].getmaxCDL())) && (dbOsnrL >= toBeRouted[i].getOsnrLimitL())){
					//if qot ok we add it to set
					prePath aux;
					aux.length = alldemandsdistances[i][j];
					aux.noise = alldemandsNoiseC[i][j];
					aux.nbEdges = alldemandsNbEdges[i][j];
					aux.amplis = alldemandsAmplis[i][j];
					std::vector<Fiber> auxin;
					for (int k = 0; k <allpaths[i][j].size()-1; ++k){
						auxin.push_back(instance.getPhysicalLinkBetween(allpaths[i][j][k],allpaths[i][j][k+1]));
					}	
					aux.links = auxin;
					qualifiedForThisDemand.push_back(aux);
					/*
					for (int i = 0; i <aux.links.size(); ++i){
						std::cout << aux.links[i].getSource()+1 << "-" << aux.links[i].getTarget()+1 << "|" ;
					}
					std::cout <<  " dispersion " << aux.length*17 << "|" ;
					std::cout <<  " noise " << aux.noise << "|" ;
					std::cout <<  " nb edges " << aux.nbEdges << "|";
					std::cout <<  " amplis " << aux.amplis << "|";
					std::cout <<  " osnr " << dbOsnrC << "|" <<std::endl;
					*/
				}
			}
		}
		/*std::cout << "for demand " << i +1 <<std::endl;
		for (int i = 0; i <qualifiedForThisDemand.size(); ++i){
			for (int j = 0; j <qualifiedForThisDemand[i].links.size(); ++j){
				std::cout << qualifiedForThisDemand[i].links[j].getSource()+1 << "-" << qualifiedForThisDemand[i].links[j].getTarget()+1 << "|" ;
			}
			std::cout <<  " length " << qualifiedForThisDemand[i].length*17 << "|" ;
			std::cout <<  " noise " << qualifiedForThisDemand[i].noise << "|" ;
			std::cout <<  " nb edges " << qualifiedForThisDemand[i].nbEdges << "|"<<std::endl;
		}
		std::cout << "SORTED for demand " << i +1 <<std::endl;
		*/
		
		sortPrePathByEdges sorterA;
		sortPrePathByAmps sorterB;
		sortPrePathByLength sorterC;
		//ClockTime bilula(ClockTime::getTimeNow());
		//bilula.setStart(ClockTime::getTimeNow());
		if((metric==Input::OBJECTIVE_METRIC_TUS)||(metric ==Input::OBJECTIVE_METRIC_LLB)){
			//std::cout << "Sorting by nb edges"<< std::endl;
			std::sort(qualifiedForThisDemand.begin(),qualifiedForThisDemand.end(),sorterA);
		}
		if((metric==Input::OBJECTIVE_METRIC_NLUS)||(metric==Input::OBJECTIVE_METRIC_TASE)||(metric==Input::OBJECTIVE_METRIC_ADS)||(metric ==Input::OBJECTIVE_METRIC_DCB)){
			//std::cout << "Sorting by amplis"<< std::endl;
			std::sort(qualifiedForThisDemand.begin(),qualifiedForThisDemand.end(),sorterB);
		}
		if(metric==Input::OBJECTIVE_METRIC_TRL){
			//std::cout << "Sorting by length"<< std::endl;
			std::sort(qualifiedForThisDemand.begin(),qualifiedForThisDemand.end(),sorterC);
		}
		//std::cout<< "TIME SORTING SHORTEST "<< bilula.getTimeInSecFromStart()<<std::endl;
		/*std::cout << "demand " <<i+1 << std::endl;
		
		for (int i = 0; i <qualifiedForThisDemand.size(); ++i){
			for (int j = 0; j <qualifiedForThisDemand[i].links.size(); ++j){
				std::cout << qualifiedForThisDemand[i].links[j].getSource()+1 << "-" << qualifiedForThisDemand[i].links[j].getTarget()+1 << "|" ;
			}
			std::cout <<  " nb edges " << qualifiedForThisDemand[i].nbEdges << "|";
			std::cout <<  " length " << qualifiedForThisDemand[i].length << "|" <<std::endl;
			//std::cout <<  " amplis " << qualifiedForThisDemand[i].amplis << "|";
			//std::cout <<  " cd " << qualifiedForThisDemand[i].length*17 << "|" ;
			//std::cout <<  " noise " << qualifiedForThisDemand[i].noise << "|" <<std::endl;
			
		}*/
		std::vector<std::vector<Fiber> > kcandidates;
		int limit = chosenK+extraK;

		
		if(qualifiedForThisDemand.size()<limit){
			limit = qualifiedForThisDemand.size();
		}
		for (int i = 0; i <limit; ++i){
			std::vector<Fiber> candidate;
			for (int j = 0; j <qualifiedForThisDemand[i].links.size(); ++j){
     			candidate.push_back(qualifiedForThisDemand[i].links[j]);
			}
        kcandidates.push_back(candidate); 
        }
        shortestRoutesByDemand.push_back(kcandidates);
	}
	kShortestTime = clockShortest.getTimeInSecFromStart();
}

void Genetic::AllPathsUtil(int u, int d, bool visited[], int path[], int& path_index)
{
    // Mark the current node and store it in path[]
    visited[u] = true;
    path[path_index] = u;
    path_index++;
 
    // If current vertex is same as destination, then print
    // current path[]
    if (u == d) {
        std::vector<int> aux;
        for (int i = 0; i < path_index; i++){
            //std::cout << path[i] << " ";
            //std::cout << getCompactNodeLabel(getCompactNodeFromLabel(path[i])) << " "; ;
            aux.push_back(path[i]);

        }
        pathsdemand.push_back(aux);
        //std::cout << std::endl;
    }
    else // If current vertex is not destination
    {
        // Recur for all the vertices adjacent to current vertex
        std::vector<int>::iterator i;
        for (i = adj_list[u].begin(); i != adj_list[u].end(); ++i)
            if (!visited[*i])
                AllPathsUtil(*i, d, visited, path, path_index);
    }
 
    // Remove current vertex from path[] and mark it as unvisited
    path_index--;
    visited[u] = false;
}

void Genetic::GenerateShortestRoutes(){
    std::cout << "GENERATING SHORTEST ROUTES" << std::endl;
	ClockTime clockShortest(ClockTime::getTimeNow());
	clockShortest.setStart(ClockTime::getTimeNow());
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
	kShortestTime = clockShortest.getTimeInSecFromStart();
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
	pathdistance =chosendistance;
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

void Genetic::computeLB()
{
	double totalMinimumMetric=0.0;

	//DJIKISTRA MODULE     	
    int originDjikistra;
    int destinationDijikistra;
	double thisDemandMinimumMetric = 0.0;
	double minimumSlots = 0.0;
	int nbEdges = getInstance().getTabEdge().size();
	
	std::vector<double> linkCharge(nbEdges, 0.0);
	

    for (int i = 0; i < toBeRouted.size(); ++i){
        originDjikistra = toBeRouted[i].getSource();
        destinationDijikistra = toBeRouted[i].getTarget();
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
					if((metric==Input::OBJECTIVE_METRIC_TUS)||(metric==Input::OBJECTIVE_METRIC_NLUS)){
                    	adjmatrix[i][edgeorigin] = 1;
					}
					if(metric==Input::OBJECTIVE_METRIC_TRL){
                    	adjmatrix[i][edgeorigin] = getInstance().getTabEdge()[j].getLength();
					}
					if(metric==Input::OBJECTIVE_METRIC_TASE){
                    	adjmatrix[i][edgeorigin] = getInstance().getTabEdge()[j].getLineAmplifiers()+1;
					}
                }
                else{
                    if (edgeorigin == demandorigin){
						if((metric==Input::OBJECTIVE_METRIC_TUS)||(metric==Input::OBJECTIVE_METRIC_NLUS)){
							adjmatrix[i][edgedestination] = 1;
						}
						if(metric==Input::OBJECTIVE_METRIC_TRL){
							adjmatrix[i][edgedestination] = getInstance().getTabEdge()[j].getLength();
						}
						if(metric==Input::OBJECTIVE_METRIC_TASE){
							adjmatrix[i][edgedestination] = getInstance().getTabEdge()[j].getLineAmplifiers()+1;
						}
					}
                }
            }
        }
		std::vector<int> djikistraSolution;

		double dist;
		djikistraSolution = dijkstra(adjmatrix,originDjikistra,destinationDijikistra,dist);
		if((metric==Input::OBJECTIVE_METRIC_TUS)||(metric==Input::OBJECTIVE_METRIC_NLUS)){
			thisDemandMinimumMetric = (djikistraSolution.size()-1)*toBeRouted[i].getLoadC();
		}
		if(metric==Input::OBJECTIVE_METRIC_TRL){
			thisDemandMinimumMetric = dist;
		}
		if(metric==Input::OBJECTIVE_METRIC_TASE){
			thisDemandMinimumMetric = dist*(toBeRouted[i].getGBits()/toBeRouted[i].getLoadC());
		}
		//std::cout << "Demand  " << i +1 <<  " metric " << thisDemandMinimumMetric << std::endl;
		
		totalMinimumMetric = totalMinimumMetric + thisDemandMinimumMetric;
		//std::cout << "Demand  " << i +1 <<  " used at least " << djikistraSolution.size()-1 <<  " edges so total s " <<thisDemandMinimumSlots<<std::endl;
		//std::cout << "total  " << totalMinimumSlots << std::endl;
	}
	/*
	double maxLinkCharge =0;
	if (prepro == true){
    	maxLinkCharge = *std::max_element(linkCharge.begin(), linkCharge.end());
	}*/


	if(metric==Input::OBJECTIVE_METRIC_TUS){
		computedLB = totalMinimumMetric;
		//std::cout << "LB  " << computedLB << std::endl;
	}
	if(metric==Input::OBJECTIVE_METRIC_NLUS){
		computedLB = ceil(totalMinimumMetric/instance.getNbEdges());
		//std::cout << "LB non round  " << totalMinimumSlots/instance.getNbEdges() << std::endl;
		//std::cout << "LB dividing charge by all links  " << computedLB << std::endl;
		//std::cout << "LB dividing charge by preprocessed links  " << ceil(maxLinkCharge) << std::endl;
	}
	if(metric==Input::OBJECTIVE_METRIC_TRL){
		computedLB = totalMinimumMetric;
		//std::cout << "LB  " << computedLB << std::endl;
	}
	if(metric==Input::OBJECTIVE_METRIC_TASE){
		computedLB = totalMinimumMetric;
		//std::cout << "LB  " << computedLB << std::endl;
	}

	for (int i = 0; i < toBeRouted.size(); ++i){
		originDjikistra = toBeRouted[i].getSource();
        destinationDijikistra = toBeRouted[i].getTarget();
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
                if (edgedestination == demandorigin){
                    adjmatrix[i][edgeorigin] = 1;
                }
                else{
                    if (edgeorigin == demandorigin){
						adjmatrix[i][edgedestination] = 1;
					}
                }
            }
        }
		std::vector<int> djikistraSolution;
		double dist;
		djikistraSolution = dijkstra(adjmatrix,originDjikistra,destinationDijikistra,dist);
		minimumSlots = minimumSlots+ ((djikistraSolution.size()-1)*toBeRouted[i].getLoadC());
	}

	lowerBoundUsedSlot = ceil(minimumSlots/instance.getNbEdges());	
}


int Genetic::getRandomNumber(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);  // Create distribution per call
    return dist(rng);
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
