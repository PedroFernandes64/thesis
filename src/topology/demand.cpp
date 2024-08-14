#include "demand.h"

/* Constructor. */
Demand::Demand(int i, int s, int t, int lC, int lL, double maxC, double maxL, double osC, double osL, int tC, int tL, double pc, double pl, double ps,double gb, bool a, int slice, double pathLen, int hops, std::string m, std::string space, std::string b){
	this->setId(i);
	this->setSource(s);
	this->setTarget(t);
	this->setLoadC(lC);
	this->setmaxCDC(maxC);
	this->setOsnrLimitC(osC);
	this->setLoadL(lL);
	this->setmaxCDL(maxL);
	this->setOsnrLimitL(osL);
	this->setTranspIdC(tC);
	this->setTranspIdL(tL);
	this->setGBits(gb);
	this->setPchC(pc);
	this->setPchL(pl);
	this->setPchS(ps);
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
	this->setLoadC(demand.getLoadC());
	this->setmaxCDC(demand.getmaxCDC());
	this->setOsnrLimitC(demand.getOsnrLimitC());
	this->setLoadL(demand.getLoadL());
	this->setmaxCDL(demand.getmaxCDL());
	this->setOsnrLimitL(demand.getOsnrLimitL());
	this->setTranspIdC(demand.getTranspIdC());
	this->setTranspIdL(demand.getTranspIdL());
	this->setGBits(demand.getGBits());
	this->setPchC(demand.getPchC());
	this->setPchL(demand.getPchL());
	this->setPchS(demand.getPchS());
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
	std::cout << ". nbSlices C: " << this->getLoadC() << ", maxCD C: " << this->getmaxCDC()<< ", osnrLimit C: " << this->getOsnrLimitC()<< ", pchC: " << this->getPchC()<< ", transpIdC: " << this->getTranspIdC();
	std::cout << ". nbSlices L: " << this->getLoadL() << ", maxCD L: " << this->getmaxCDL()<< ", osnrLimit L: " << this->getOsnrLimitL()<< ", pchL: " << this->getPchL()<< ", transpIdL: " << this->getTranspIdL();
	std::cout << ". Gbits/s: " << this->getGBits();
	std::cout << ", ROUTED: " << r << std::endl;
}

/* Verifies if the demand has exactly the given informations. */
void Demand::checkDemand(int i, int s, int t, int l){
	try {
		if (this->getId() != i || this->getSource() != s || this->getTarget() != t || this->getLoadC() != l) {
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
	str += std::to_string(this->getLoadC()) + ")";
	return str;
}
