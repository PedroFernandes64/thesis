#include "physicalLink.h"
#include <iostream>

/****************************************************************************************/
/*										Constructor										*/
/****************************************************************************************/

/** Constructor. **/
Fiber::Fiber(int i, int ind, int s, int t, double l, int nb, double c, int la, double nC, double nL,double nS, double dC, double dL,  double dS, int sC, int sL, int ab, int ib) {
	this->setId(i);
	this->setIndex(ind);
	this->setSource(s);
	this->setTarget(t);
	this->setLength(l);
	this->setNbSlices(nb);
	for (int i = 0; i < nb; i++){
		this->spectrum.push_back(Slice());
	}
	this->setCost(c);
	this->setLineAmplifiers(la);
	this->setNbSlicesC(sC); 
	this->setNbSlicesL(sL); 
	this->setAvailableBands(ab); 
	this->setInstalledBands(ib); 
	this->setNoiseC(nC);
	this->setNoiseL(nL);
	this->setNoiseS(nS);
	this->setDispersionCoeffC(dC);
	this->setDispersionCoeffL(dL);
	this->setDispersionCoeffS(dS);
}

Fiber::Fiber(const Fiber & f){
	this->setId(f.id);
	this->setIndex(f.index);
	this->setSource(f.idSource);
	this->setTarget(f.idTarget);
	this->setNbSlices(f.nbSlices);
	this->setLength(f.length);
	for (int i = 0; i < f.nbSlices; i++){
		this->spectrum.push_back(f.spectrum[i]);
	}
	this->setCost(f.cost);
	this->setLineAmplifiers(f.lineAmplifiers);
	this->setNbSlicesC(f.nbSlicesCBand); 
	this->setNbSlicesL(f.nbSlicesLBand); 
	this->setAvailableBands(f.availableBands); 
	this->setInstalledBands(f.installedBands); 
	this->setNoiseC(f.noiseC);
	this->setNoiseL(f.noiseL);
	this->setNoiseS(f.noiseS);
	this->setDispersionCoeffC(f.dispersionC);
	this->setDispersionCoeffL(f.dispersionL);
	this->setDispersionCoeffS(f.dispersionS);
}

/****************************************************************************************/
/*										Destructor										*/
/****************************************************************************************/

/* Destructor. Clears the vector of slices. */
Fiber::~Fiber() {
	spectrum.clear();
}

/****************************************************************************************/
/*										Methods											*/
/****************************************************************************************/

/* Copies all information from a given fiber. */
void Fiber::copyFiber(Fiber & edge){
	this->setSource(edge.getSource());
	this->setTarget(edge.getTarget());
	this->setNbSlices(edge.getNbSlices());
	this->spectrum.resize(this->getNbSlices());
	for (int i = 0; i < this->getNbSlices(); i++){
		this->spectrum[i].setAssignment(edge.getSlice_i(i).getAssignment());
	}	
	this->setLength(edge.getLength());
	this->setCost(edge.getCost());
	this->setLineAmplifiers(edge.getLineAmplifiers());
	this->setNbSlicesC(edge.getNbSlicesC()); 
	this->setNbSlicesL(edge.getNbSlicesL()); 
	this->setAvailableBands(edge.getAvailableBands()); 
	this->setInstalledBands(edge.getInstalledBands());
	this->setNoiseC(edge.getNoiseC());
	this->setNoiseL(edge.getNoiseL());
	this->setNoiseS(edge.getNoiseS());
	this->setDispersionCoeffC(edge.getDispersionCoeffC());
	this->setDispersionCoeffL(edge.getDispersionCoeffL());
	this->setDispersionCoeffS(edge.getDispersionCoeffS());

}

/* Verifies if the fiber is routing a demand. */
bool Fiber::contains(const Demand &d) const{
	for (int i = 0; i < this->getNbSlices(); i++){
		if (getSlice_i(i).getAssignment() == d.getId()){
			return true;
		}
	}
	return false;
}

/* Assigns a demand to a given position in the spectrum. */
void Fiber::assignSlices(const Demand &d, int p){
	int demandLoad;
	if (p>=getNbSlicesC()){
		installedBands = 2;
		demandLoad = d.getLoadL();
	}else{
		demandLoad = d.getLoadC();
	}
	// assign demand d to this edge from position p - demandLoad + 1 to position p
	int first = p - demandLoad + 1;
	for (int i = first; i <= p; i++) {
		this->spectrum[i].setAssignment(d.getId());
	}
	if (p>=getNbSlicesC()){
		installedBands = 2;
	}
}

/* Returns the maximal slice position used in the frequency spectrum. Returns -1 if no slice is used. */
int Fiber::getMaxUsedSlicePosition() const {
	int max = -1;
	for (int i = 0; i < this->getNbSlices(); i++){
		if (getSlice_i(i).isUsed()){
			if (i >= max){
				max = i;
			}
		}
	}
	return max;
}

/* Returns the number of slices ocupied in the fiber's frequency spectrum. */
int Fiber::getNbUsedSlices() const {
	int value = 0;
	for (int i = 0; i < this->getNbSlices(); i++){
		if (getSlice_i(i).isUsed()){
			value++;
		}
	}
	return value;
}

/****************************************************************************************/
/*										Display											*/
/****************************************************************************************/

/* Displays summarized information about the fiber. */
void Fiber::displayFiber(){
	std::cout << "#" << this->getId()+1 << ". " << this->getSource()+1 << " -- " << this->getTarget()+1;
	std::cout << ". nb slices in C band: " << this->getNbSlicesC()<< ", nb slices in L band: " << this->getNbSlicesL()  << ", length: " << this->getLength() << ", amplis: " << this->getLineAmplifiers() << ", noiseC: " << this->getNoiseC() << ", noiseL: " << this->getNoiseL()<<std::endl;
}

/* Displays detailed information about state of the fiber. */
void Fiber::displayDetailedFiber(){
	std::cout << "#" << this->getId()+1 << ". " << this->getSource()+1 << " -- " << this->getTarget()+1;
	std::cout << ". length: " << this->getLength() << ", cost: " << this->getCost() << ", amplis: " << this->getLineAmplifiers() << std::endl;
	for (int i = 0; i < this->getNbSlices(); i++){
		std::cout << "\tSlice #" << i+1 << ". ";
		if (this->spectrum[i].isUsed()) {
			std::cout << this->spectrum[i].getAssignment()+1 << std::endl;
		}
		else{
			std::cout << "--" << std::endl;
		}
	}
}

/* Displays summarized information about slice occupation. */
void Fiber::displaySlices(){
	for (int i = 0; i < this->getNbSlicesC(); i++){
		if (this->spectrum[i].isUsed()) {
			std::cout << "*";
		}
		else {
			std::cout << "-";
		}
	}
	if(availableBands>1){
		std::cout << "||";
		for (int i = this->getNbSlicesC(); i < this->getNbSlicesC() + this->getNbSlicesL(); i++){
			if (this->spectrum[i].isUsed()) {
				std::cout << "*";
			}
			else {
				std::cout << "-";
			}
		}
	}
	std::cout << std::endl;
}
