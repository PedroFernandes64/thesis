#include "demand.h"

/* Constructor. */
Demand::Demand(int i, int s, int t, int l, double max, double osL, double p, bool a, int slice, double pathLen, int hops, std::string m, std::string space, std::string b){
	this->setId(i);
	this->setSource(s);
	this->setTarget(t);
	this->setLoad(l);
	this->setMaxLength(max);
	this->setOsnrLimit(osL);
	this->setPch(p);
	this->setRouted(a);
	this->setSliceAllocation(slice);
	this->setPathLength(pathLen);
	this->setNbHops(hops);
	this->setMode(m);
	this->setSpacing(space);
	this->setPathBandwidth(b);
}


/* Copies all information from a given demand. */
void Demand::copyDemand(const Demand & demand){
	this->setId(demand.getId());
	this->setSource(demand.getSource());
	this->setTarget(demand.getTarget());
	this->setLoad(demand.getLoad());
	this->setMaxLength(demand.getMaxLength());
	this->setOsnrLimit(demand.getOsnrLimit());
	this->setPch(demand.getPch());
	this->setRouted(demand.isRouted());
	this->setSliceAllocation(demand.getSliceAllocation());
	this->setPathLength(demand.getPathLength());
	this->setNbHops(demand.getNbHops());
	this->setMode(demand.getMode());
	this->setSpacing(demand.getSpacing());
	this->setPathBandwidth(demand.getPathBandwidth());
}

/* Displays demand information. */
void Demand::displayDemand(){
	std::string r;
	if (this->isRouted()){
		r = "YES";
	}
	else{
		r = "NO";
	}
	std::cout << "#" << this->getId()+1 << ". " << this->getSource()+1 << " -- " << this->getTarget()+1;
	std::cout << ". nbSlices: " << this->getLoad() << ", maxLength: " << this->getMaxLength()<< ", osnrLimit: " << this->getOsnrLimit()<< ", pch: " << this->getPch();
	std::cout << ", ROUTED: " << r << std::endl;
}

/* Verifies if the demand has exactly the given informations. */
void Demand::checkDemand(int i, int s, int t, int l){
	try {
		if (this->getId() != i || this->getSource() != s || this->getTarget() != t || this->getLoad() != l) {
			throw "Demands do not match. Verify files Demand.csv and Demand_edges_slices.csv\n";
		}
	}
	catch (const char* msg) {
		std::cerr << msg << std::endl;
	}
}

/* Returns a compact description of the demand in the form (source, target, load). */
std::string Demand::getString() const{
	std::string str = "(" + std::to_string(this->getSource() + 1) + ",";
	str += std::to_string(this->getTarget() + 1) + ",";
	str += std::to_string(this->getLoad()) + ")";
	return str;
}
