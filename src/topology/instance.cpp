#include "instance.h"
using namespace lemon;


/****************************************************************************************/
/*										Constructor										*/
/****************************************************************************************/

/* Constructor initializes the object with the information of an Input. */
Instance::Instance(const Input &i) : input(i){
	this->setNbNodes(0);
	this->setWasBlocked(false);
	createInitialMapping();
	this->setNextDemandToBeRoutedIndex(0);
	//PEDRO PEDRO PEDRO
	this->setPaseNodeC();
	this->setPaseNodeL();
	this->setPaseNodeS();
}

/* Copy constructor. */
Instance::Instance(const Instance &i) : input(i.getInput()){
	this->setNbNodes(i.getNbNodes());
	this->setWasBlocked(i.getWasBlocked());
	for(int j=0;j<i.tabEdge.size();j++){
		tabEdge.push_back(i.tabEdge[j]);
	}
	//this->setTabEdge(i.getTabEdge());
	this->setTabDemand(i.getTabDemand());
	this->setNextDemandToBeRoutedIndex(i.getNextDemandToBeRoutedIndex());
	//PEDRO PEDRO PEDRO
	this->setPaseNodeC();
	this->setPaseNodeL();
	this->setPaseNodeS();
}

/****************************************************************************************/
/*										Destructor										*/
/****************************************************************************************/

/* Destructor. Clears the vectors of demands and links. */
Instance::~Instance() {
	tabEdge.clear();
	tabDemand.clear();
}

/****************************************************************************************/
/*										Methods											*/
/****************************************************************************************/

/* Returns the number of demands already routed. */
int Instance::getNbRoutedDemands() const{
	int counter = 0;
	for(int i = 0; i < getNbDemands(); i++){
    	if (tabDemand[i].isRouted() == true){
			counter++;
		}
	}
	return counter;
}

/* Returns the vector of demands to be routed in the next optimization. */
std::vector<Demand> Instance::getNextDemands() const { 
	std::vector<Demand> toBeRouted;
	for(int i = getNextDemandToBeRoutedIndex(); i < getNbDemands(); i++){
		if ((int)toBeRouted.size() >= getInput().getNbDemandsAtOnce()){
			return toBeRouted;
		}
		if( tabDemand[i].isRouted() == false ){
			toBeRouted.push_back(tabDemand[i]);
		}
	}
	return toBeRouted;
}

void Instance::decreaseNbDemandsAtOnce(){
	int currentNbDemands = getInput().getNbDemandsAtOnce();
	this->input.setNbDemandsAtOnce(currentNbDemands - 1);
}

/* Returns the max used slice position throughout the whole network. */
int Instance::getMaxUsedSlicePosition() const{
	int maxSlice = -1;
	for (int i = 0; i < getNbEdges(); i++){
		int maxSliceFromEdge = tabEdge[i].getMaxUsedSlicePosition();
		if (maxSliceFromEdge > maxSlice){
			maxSlice = maxSliceFromEdge;
		}
	}
	return maxSlice;
}

/* Returns the max slice position (used or not) throughout the whole network. */
int Instance::getMaxSlice() const{
	int maxSlice = 0;
	for (int i = 0; i < getNbEdges(); i++){
		int maxSliceFromEdge = tabEdge[i].getNbSlices();
		if (maxSliceFromEdge > maxSlice){
			maxSlice = maxSliceFromEdge;
		}
	}
	return maxSlice;
}

/* Changes the attributes of the Fiber from the given index according to the attributes of the given link. */
void Instance::setEdgeFromId(int id, Fiber & edge){
	this->tabEdge[id].copyFiber(edge);
}

/* Changes the attributes of the Demand from the given index according to the attributes of the given demand. */
void Instance::setDemandFromId(int id, const Demand & demand){
	this->tabDemand[id].copyDemand(demand);
}

/* Builds the initial mapping based on the information retrived from the Input. */
void Instance::createInitialMapping(){
	std::cout << "--- CREATING INITIAL MAPPING... --- " << std::endl;
	if (!input.getTopologyFile().empty()){
		readTopology();
	}
	else{
		std::cerr << "A topology file MUST be declared in the input file.\n";
		exit(0);
	}
	if (!input.getInitialMappingDemandFile().empty()){
		readDemands();
		if (!input.getInitialMappingAssignmentFile().empty()){
			readDemandAssignment();
		}
		else{
			std::cout << "Starting with an empty mapping and demands from DemandFile are the first to be served. " << std::endl;
		}
	}
	else{
		std::cout << "Starting with an empty initial mapping. " << std::endl;
	}
	setNbInitialDemands(getNbRoutedDemands());
	std::cout << "Number of bands : " << input.getNbBands() << std::endl;
}

/* Reads the topology information from file. */
void Instance::readTopology(){
	std::cout << "Reading " << input.getTopologyFile() << "."  << std::endl;
	CSVReader reader(input.getTopologyFile());
	/* dataList is a vector of vectors of strings. */
	/* dataList[0] corresponds to the first line of the document and dataList[0][i] to the i-th word.*/
	std::vector<std::vector<std::string> > dataList = reader.getData();
	int numberOfLines = (int)dataList.size();
	// The number of nodes is given by the max index of sources and targets
	int maxNode = 0;
	//skip the first line (headers)
	// edges and nodes id starts on 1 in the input files. In this program ids will be in the range [0,n-1]!
	for (int i = 1; i < numberOfLines; i++)	{
		int idEdge = std::stoi(dataList[i][0]) - 1;
		int edgeIndex = i - 1;
		int edgeSource = std::stoi(dataList[i][1]) - 1;
		int edgeTarget = std::stoi(dataList[i][2]) - 1;
		double edgeLength = std::stod(dataList[i][3]);
		int edgeNbSlicesC = std::stoi(dataList[i][4]);
		int edgeNbSlicesL = 0;
		int installedBands = 1;
		int availableBands = 1;
		if (getInput().getNbBands() == 2){
			edgeNbSlicesL = std::stoi(dataList[i][5]);
			availableBands = 2;
		}
		int edgeNbSlices = edgeNbSlicesC+edgeNbSlicesL;
		double edgeCost = 1.0;
		// Only read GNData if GNModel activated
		int edgeAmplis = 0;
		double edgePnliC = 0.0;
		double edgePaseC = 0.0;
		double edgePnliL = 0.0;
		double edgePaseL = 0.0;
		double edgePnliS = 0.0;
		double edgePaseS = 0.0;
		if (this->input.isOSNREnabled() == true ){
			edgeAmplis = std::stoi(dataList[i][6]);
			edgePnliC = std::stod(dataList[i][7]);
			edgePaseC = std::stod(dataList[i][8]);
			if (getInput().getNbBands() >= 2){
				edgePnliL = std::stod(dataList[i][9]);
				edgePaseL = std::stod(dataList[i][10]);
			}
			if (getInput().getNbBands() == 3){
				edgePnliS = std::stod(dataList[i][11]);
				edgePaseS = std::stod(dataList[i][12]);
			}
		}
		Fiber edge(idEdge, edgeIndex, edgeSource, edgeTarget, edgeLength, edgeNbSlices, edgeCost, edgeAmplis, edgePnliC, edgePnliL, edgePnliS, edgePaseC, edgePaseL, edgePaseS,edgeNbSlicesC,edgeNbSlicesL,availableBands,installedBands);
		this->tabEdge.push_back(edge);
		if (edgeSource > maxNode) {
			maxNode = edgeSource;
		}
		if (edgeTarget > maxNode) {
			maxNode = edgeTarget;
		}
		std::cout << "Creating edge ";
		edge.displayFiber();
	}
	this->setNbNodes(maxNode+1);
}

/* Reads the routed demand information from file. */
void Instance::readDemands(){
	std::cout << "Reading " << input.getInitialMappingDemandFile() << "." << std::endl;
	CSVReader reader(input.getInitialMappingDemandFile());
	/* dataList is a vector of vectors of strings. */
	/* dataList[0] corresponds to the first line of the document and dataList[0][i] to the i-th word.*/
	std::vector<std::vector<std::string> > dataList = reader.getData();
	int numberOfLines = (int)dataList.size();
	//skip the first line (headers)
	for (int i = 1; i < numberOfLines; i++) {
		int idDemand = std::stoi(dataList[i][0]) - 1;
		int demandSource = std::stoi(dataList[i][1]) - 1;
		int demandTarget = std::stoi(dataList[i][2]) - 1;
		int demandLoadC = std::stoi(dataList[i][3]);
		double demandMaxLengthC = std::stod(dataList[i][4]); // dataList[i][5])
		double demandOsnrLimitC=0.0;
		int demandLoadL = 0.0;
		double demandMaxLengthL = 0.0;
		double demandOsnrLimitL=0.0;
		double demandPchC = 0.0;
		double demandPchL = 0.0;
		double demandPchS = 0.0;
		if (this->input.isOSNREnabled () == true ){
			demandOsnrLimitC = std::stod(dataList[i][5]); 		// dataList[i][7])
			demandPchC = std::stod(dataList[i][6]);				// dataList[i][9])
			if (getInput().getNbBands() >= 2){
				demandOsnrLimitL = std::stod(dataList[i][5]); 	// dataList[i][8])
				demandPchL = std::stod(dataList[i][7]); 		// dataList[i][10])
			}
			if (getInput().getNbBands() == 3){
				demandPchS = std::stod(dataList[i][8]);			// dataList[i][11])
			}
		}
		if (getInput().getNbBands() == 2){
			demandLoadL = std::stoi(dataList[i][3]); // dataList[i][4])
			demandMaxLengthL = std::stod(dataList[i][4]); // dataList[i][6])
		}
		Demand demand(idDemand, demandSource, demandTarget, demandLoadC, demandLoadL, demandMaxLengthC, demandMaxLengthL, demandOsnrLimitC,demandOsnrLimitL, demandPchC, demandPchL, demandPchS, false);
		this->tabDemand.push_back(demand);
	}
}

/* Reads the assignment information from file. */
void Instance::readDemandAssignment(){
	CSVReader reader(input.getInitialMappingAssignmentFile());
	std::cout << "Reading " << input.getInitialMappingAssignmentFile() << "." << std::endl;

	/* dataList is a vector of vectors of strings. */
	/* dataList[0] corresponds to the first line of the document and dataList[0][0] to the first word.*/
	std::vector<std::vector<std::string> > dataList = reader.getData();
	int numberOfColumns = (int)dataList[0].size();
	int numberOfLines = (int)dataList.size();

	//check if the demands in this file are the same as the ones read in Demand.csv
	//skip the first word (headers) and the last one (empty)
	for (int i = 1; i < numberOfColumns-1; i++) {
		int demandId = stoi(getInBetweenString(dataList[0][i], "_", "=")) - 1;
		std::string demandStr = getInBetweenString(dataList[0][i], "(", ")");
		std::vector<std::string> demand = splitBy(demandStr, ",");
		int demandSource = std::stoi(demand[0]) - 1;
		int demandTarget = std::stoi(demand[1]) - 1;
		int demandLoad = std::stoi(demand[2]);
		this->tabDemand[demandId].checkDemand(demandId, demandSource, demandTarget, demandLoad);
	}
	std::cout << "Checking done." << std::endl;

	//search for slice allocation line
	for (int alloc = 0; alloc < numberOfLines; alloc++)	{
		if (dataList[alloc][0].find("slice allocation") != std::string::npos) {
			// for each demand
			for (int d = 0; d < this->getNbDemands(); d++) {
				int demandMaxSlice = std::stoi(dataList[alloc][d+1]) - 1;
				this->tabDemand[d].setRouted(true);
				this->tabDemand[d].setSliceAllocation(demandMaxSlice);
				// look for which edges the demand is routed
				double lengthOfPath = 0.0;
				int numberOfHops = 0;
				for (int i = 0; i < this->getNbEdges(); i++) {
					if (dataList[i+1][d+1] == "1") {
						this->tabEdge[i].assignSlices(this->tabDemand[d], demandMaxSlice);
						lengthOfPath += this->tabEdge[i].getLength();
						numberOfHops++;
					}
				}
				this->tabDemand[d].setPathLength(lengthOfPath);
				this->tabDemand[d].setNbHops(numberOfHops);
			}
		}
	}
}

/* Displays overall information about the current instance. */
void Instance::displayInstance() {
	std::cout << "**********************************" << std::endl;
	std::cout << "*      Constructed Instance      *" << std::endl;
	std::cout << "**********************************" << std::endl;
	std::cout << "Number of nodes : " << this->getNbNodes() << std::endl;
	std::cout << "Number of edges : " << this->getNbEdges() << std::endl;

	displayTopology();
	displaySlices();
	displayRoutedDemands();

}

/* Displays information about the physical topology. */
void Instance::displayTopology(){
	std::cout << std::endl << "--- The Physical Topology ---" << std::endl;
	for (int i = 0; i < this->getNbEdges(); i++) {
		tabEdge[i].displayFiber();
	}
	std::cout << std::endl;
}


/* Displays detailed information about state of the physical topology. */
void Instance::displayDetailedTopology(){
	std::cout << std::endl << "--- The Detailed Physical Topology ---" << std::endl;
	for (int i = 0; i < this->getNbEdges(); i++) {
		tabEdge[i].displayDetailedFiber();
	}
	std::cout << std::endl;

}

/* Displays summarized information about slice occupation of each Fiber. */
void Instance::displaySlices() {
	std::cout << std::endl << "--- Slice occupation ---" << std::endl;
	for (int i = 0; i < this->getNbEdges(); i++) {
		std::cout << "#" << i+1 << ". " << tabEdge[i].getSource()+1 << "-" << tabEdge[i].getTarget()+1 << ". ";
		tabEdge[i].displaySlices();
	}
	std::cout << std::endl;
}

/* Displays information about the routed demands. */
void Instance::displayRoutedDemands(){
	std::cout << std::endl << "--- The Routed Demands ---" << std::endl;
	for (int i = 0; i < this->getNbDemands(); i++) {
		if (tabDemand[i].isRouted()) {
			tabDemand[i].displayDemand();
		}
	}
	std::cout << std::endl;
}

/* Adds non-routed demands to the pool by reading the information from onlineDemands Input file. */
void Instance::generateDemandsFromFile(std::string filePath){
	std::cout << "Reading " << filePath << " ..." << std::endl;
	CSVReader reader(filePath);
	/* dataList is a vector of vectors of strings. */
	/* dataList[0] corresponds to the first line of the document and dataList[0][i] to the i-th word.*/
	std::vector<std::vector<std::string> > dataList = reader.getData();
	int numberOfLines = (int)dataList.size();
	int nbPreviousDemands = tabDemand.size();
	//skip the first line (headers)
	for (int i = 1; i < numberOfLines; i++) {
		int idDemand = std::stoi(dataList[i][0]) - 1 + nbPreviousDemands;
		int demandSource = std::stoi(dataList[i][1]) - 1;
		int demandTarget = std::stoi(dataList[i][2]) - 1;
		int demandLoadC = std::stoi(dataList[i][3]);
		double demandMaxLengthC = std::stod(dataList[i][4]); // dataList[i][5])
		double demandOsnrLimitC=0.0;
		int demandLoadL = 0.0;
		double demandMaxLengthL = 0.0;
		double demandOsnrLimitL=0.0;
		double demandPchC = 0.0;
		double demandPchL = 0.0;
		double demandPchS = 0.0;
		if (this->input.isOSNREnabled () == true ){
			demandOsnrLimitC = std::stod(dataList[i][5]); 		// dataList[i][7])
			demandPchC = std::stod(dataList[i][6]);				// dataList[i][9])
			if (getInput().getNbBands() >= 2){
				demandOsnrLimitL = std::stod(dataList[i][5]); 	// dataList[i][8])
				demandPchL = std::stod(dataList[i][7]); 		// dataList[i][10])
			}
			if (getInput().getNbBands() == 3){
				demandPchS = std::stod(dataList[i][8]);			// dataList[i][11])
			}
		}
		if (getInput().getNbBands() == 2){
			demandLoadL = std::stoi(dataList[i][3]); // dataList[i][4])
			demandMaxLengthL = std::stod(dataList[i][4]); // dataList[i][6])
		}
		std::string demandMode = "";
		std::string demandSpacing = "";
		std::string demandPathBandwidth = "";
		if (input.isGNPYEnabled()){
			demandMode = "mode_" + dataList[i][6];
			demandSpacing = dataList[i][7];
			demandPathBandwidth = dataList[i][8];
		}
		Demand demand(idDemand, demandSource, demandTarget, demandLoadC, demandLoadL, demandMaxLengthC, demandMaxLengthL, demandOsnrLimitC,demandOsnrLimitL, demandPchC, demandPchL, demandPchS, false, -1, 0, 0, demandMode, demandSpacing, demandPathBandwidth);
		this->tabDemand.push_back(demand);
	}
	//std::cout << "out" << std::endl;
}

/* Adds non-routed demands to the pool by generating N random demands. */
void Instance::generateRandomDemands(const int N){
	srand (1234567890);
	for (int i = 0; i < N; i++){
		int idDemand =  i + getNbRoutedDemands();
		int demandSource = rand() % getNbNodes();
		int demandTarget = rand() % getNbNodes();
		while (demandTarget == demandSource){
			demandTarget = rand() % getNbNodes();
		} 
		int demandLoad = 3;
		double demandMaxLength = 3000;
		double demandOsnrLimit = 13;
		double demandPch = 0.000375;
		Demand demand(idDemand, demandSource, demandTarget, demandLoad, demandMaxLength, demandOsnrLimit, false);
		this->tabDemand.push_back(demand);
	}
}

/* Verifies if there is enough place for a given demand to be routed through link i on last slice position s. */
bool Instance::hasEnoughSpace(const int i, const int s, const Demand &demand){
	// std::cout << "Calling hasEnoughSpace..." << std::endl;
	int LOAD;
	if (s >= getPhysicalLinkFromIndex(i).getNbSlicesC() ){
		LOAD = demand.getLoadL();
	}
	else{
		LOAD = demand.getLoadC();
	}
	int firstPosition = s - LOAD + 1;
	if (firstPosition < 0){
		return false;
	}
	for (int pos = firstPosition; pos <= s; pos++){
		if (getPhysicalLinkFromIndex(i).getSlice_i(pos).isUsed() ==  true){
			return false;
		}
	}
	// std::cout << "Called hasEnoughSpace." << std::endl;
	return true;
}

/* Assigns the given demand to the j-th slice of the i-th link. */
void Instance::assignSlicesOfLink(int linkLabel, int slice, const Demand &demand){
	this->tabEdge[linkLabel].assignSlices(demand, slice);
	this->tabDemand[demand.getId()].setRouted(true);
	this->tabDemand[demand.getId()].setSliceAllocation(slice);
	
	int currentNbHops = this->tabDemand[demand.getId()].getNbHops();
	this->tabDemand[demand.getId()].setNbHops(currentNbHops+1);

	double currentPathLength = this->tabDemand[demand.getId()].getPathLength();
	double linkLength = this->tabEdge[linkLabel].getLength();
	this->tabDemand[demand.getId()].setPathLength(currentPathLength + linkLength);
}


/* Displays information about the non-routed demands. */
void Instance::displayNonRoutedDemands(){
	std::cout << std::endl << "--- The Non Routed Demands ---" << std::endl;
	for (int i = 0; i < this->getNbDemands(); i++) {
		if (tabDemand[i].isRouted() == false) {
			tabDemand[i].displayDemand();
		}
	}
	std::cout << std::endl;

}

/* Displays information about the non-routed demands. */
void Instance::displayAllDemands(){
	std::cout << std::endl << "--- The Demands ---" << std::endl;
	for (int i = 0; i < this->getNbDemands(); i++) {
		tabDemand[i].displayDemand();
	}
	std::cout << std::endl;

}

/* Call the methods allowing the build of output files. */
void Instance::output(std::string i){
	std::cout << "Output " << i << std::endl;
	outputEdgeSliceHols(i);
	outputDemands(i);
	outputDemandEdgeSlices(i);
	outputMetrics(i);
}

/* Builds file Demand_edges_slices.csv containing information about the assignment of routed demands. */
void Instance::outputDemandEdgeSlices(std::string counter){
	std::string delimiter = ";";
	std::string filePath = this->input.getOutputPath() + "Demand_edges_slices_" + counter + ".csv";
	std::ofstream myfile(filePath.c_str(), std::ios::out | std::ios::trunc);
	if (myfile.is_open()){
		myfile << "edge_slice_demand" << delimiter;
		for (int i = 0; i < getNbDemands(); i++){
			if (getDemandFromIndex(i).isRouted()){
				myfile << "k_" << getDemandFromIndex(i).getId()+1 << "= " << getDemandFromIndex(i).getString() << delimiter;
			}
		}
		myfile << "\n";
		for (int e = 0; e < getNbEdges(); e++){
			myfile << getPhysicalLinkFromIndex(e).getString() << delimiter;
			for (int i = 0; i < getNbDemands(); i++){
				if (getDemandFromIndex(i).isRouted()){
					// if demand is routed through edge: 1
					if (getPhysicalLinkFromIndex(e).contains(getDemandFromIndex(i)) == true){
						myfile << "1" << delimiter;
					}
					else{
						myfile << " " << delimiter;
					}
				}
			}
			myfile << "\n";
		}
		myfile << " slice allocation " << delimiter;
		for (int i = 0; i < getNbDemands(); i++){
			if (getDemandFromIndex(i).isRouted()){
				myfile << getDemandFromIndex(i).getSliceAllocation()+1 << delimiter;
			}
		}
		myfile << "\n";
		myfile.close();
	}
	else{
		std::cerr << "Unable to open file " << filePath << "\n";
	}
}

/* Builds file Demand.csv containing information about the routed demands. */
void Instance::outputDemands(std::string counter){
	std::string delimiter = ";";
	std::string filePath = this->input.getOutputPath() + "ServedDemands" + counter + ".csv";
	std::ofstream myfile(filePath.c_str(), std::ios::out | std::ios::trunc);
	if (myfile.is_open()){
		int nbServedSlices = 0;
		int nbBlockedSlices = 0;
		myfile << "index" << delimiter;
		myfile << "origin" << delimiter;
		myfile << "destination" << delimiter;
		myfile << "slots" << delimiter;
		myfile << "max_length" << delimiter;
		myfile << "osnr_limit" << delimiter;
		myfile << "pchC" << delimiter;
		myfile << "pchL" << delimiter;
		myfile << "pchS" << delimiter;
		myfile << "Routed" << "\n";
		for (int i = 0; i < getNbDemands(); i++){
			myfile << std::to_string(getDemandFromIndex(i).getId()+1) << delimiter;
			myfile << std::to_string(getDemandFromIndex(i).getSource()+1) << delimiter;
			myfile << std::to_string(getDemandFromIndex(i).getTarget()+1) << delimiter;
			myfile << std::to_string(getDemandFromIndex(i).getLoadC()) << delimiter;
			myfile << std::to_string(getDemandFromIndex(i).getMaxLengthC()) << delimiter;
			myfile << std::to_string(getDemandFromIndex(i).getOsnrLimitC()) << delimiter;
			myfile << std::to_string(getDemandFromIndex(i).getPchC()) << delimiter;
			myfile << std::to_string(getDemandFromIndex(i).getPchL()) << delimiter;
			myfile << std::to_string(getDemandFromIndex(i).getPchS()) << delimiter;
			if (getDemandFromIndex(i).isRouted()){
				myfile << "1" << "\n";
				nbServedSlices += getDemandFromIndex(i).getLoadC();
			}
			else{
				myfile << "0" << "\n";
				nbBlockedSlices += getDemandFromIndex(i).getLoadC();
			}
		}

		myfile << "\n";
		myfile << "Nb_Served_Demands" << delimiter << getNbRoutedDemands() << "\n";
		myfile << "Nb_Blocked_Demands" << delimiter << getNbNonRoutedDemands() << "\n"; 
		myfile << "Nb_Served_Slices" << delimiter << nbServedSlices << "\n";
		myfile << "Nb_Blocked_Slices" << delimiter << nbBlockedSlices << "\n"; 
		myfile.close();
	}
	else{
		std::cerr << "Unable to open output file " << filePath << "\n";
	}
}

int Instance::getNumberOfOccupiedSlices() const{
	int value = 0;
	for (int e = 0; e < getNbEdges(); e++){
		value += getPhysicalLinkFromIndex(e).getNbUsedSlices();
	}
	return value;
}

double Instance::getMetricValue(Metric metric) const{
	
	double value = 0;
	switch (metric){
        case METRIC_ONE:
        {
			for (int i = 0; i < getNbDemands(); i++){
				if (getDemandFromIndex(i).isRouted()){
					value += getDemandFromIndex(i).getSliceAllocation()+1;
				}
			}
            break;
        }
        case METRIC_ONE_P:
        {
            for (int e = 0; e < getNbEdges(); e++){
				value += getPhysicalLinkFromIndex(e).getMaxUsedSlicePosition()+1;
			}
			break;
		}
        case METRIC_TWO:
        {
			for (int i = 0; i < getNbDemands(); i++){
				if (getDemandFromIndex(i).isRouted()){
					value += getDemandFromIndex(i).getNbHops();
				}
			}
            break;
        }
        case METRIC_FOUR:
        {
			for (int i = 0; i < getNbDemands(); i++){
				if (getDemandFromIndex(i).isRouted()){
					value += getDemandFromIndex(i).getPathLength();
				}
			}
            break;
        }
        case METRIC_EIGHT:
        {
            for (int e = 0; e < getNbEdges(); e++){
				double lastSlice = getPhysicalLinkFromIndex(e).getMaxUsedSlicePosition()+1;
				if (lastSlice > value){
					value = lastSlice; 
				}
			}
            break;
        }
        default:
        {
            std::cerr << "The chosen metric to be evaluated is out of range.\n";
            exit(0);
            break;
        }
    }
	return value;
}
/* Builds file Metrics.csv containing information about the obtained metric values. */
void Instance::outputMetrics(std::string counter){
	std::string delimiter = ";";
	std::string filePath = this->input.getOutputPath() + "Metrics" + counter + ".csv";
	std::ofstream myfile(filePath.c_str(), std::ios::out | std::ios::trunc);
	if (myfile.is_open()){
		myfile << "met1" << delimiter << getMetricValue(METRIC_ONE) << "\n";
		myfile << "met1p" << delimiter << getMetricValue(METRIC_ONE_P) << "\n";
		myfile << "met2" << delimiter << getMetricValue(METRIC_TWO) << "\n";
		myfile << "met4" << delimiter << getMetricValue(METRIC_FOUR) << "\n";
		myfile << "met8" << delimiter << getMetricValue(METRIC_EIGHT) << "\n";
		myfile << "BlackPixels" << delimiter << getNumberOfOccupiedSlices() << "\n";
		myfile.close();
	}
	else{
		std::cerr << "Unable to open output file " << filePath << "\n";
	}
}


/* Builds file Edge_Slice_Holes_i.csv containing information about the mapping after n optimizations. */
void Instance::outputEdgeSliceHols(std::string counter){
	std::cout << "Output EdgeSliceHols: " << counter << std::endl;
	std::string delimiter = ";";
	std::string filePath = this->input.getOutputPath() + "Edge_Slice_Holes_" + counter + ".csv";
	std::ofstream myfile(filePath.c_str(), std::ios::out | std::ios::trunc);
	if (myfile.is_open()){
		myfile << " Slice-Edge " << delimiter;
		for (int i = 0; i < getNbEdges(); i++){
			std::string edge = "e_" + std::to_string(i+1);
			myfile << edge << delimiter;
		}
		myfile << "\n";
		for (int s = 0; s < input.getnbSlicesInOutputFile(); s++){
			std::string slice = "s_" + std::to_string(s+1);
			myfile << slice << delimiter;
			for (int i = 0; i < getNbEdges(); i++){
				if (s < getPhysicalLinkFromIndex(i).getNbSlices() && getPhysicalLinkFromIndex(i).getSlice_i(s).isUsed() == true){
					myfile << "1" << delimiter;
				}
				else{
					myfile << "0" << delimiter;
				}
			}
			myfile << "\n";
		}
		myfile << "Nb_New_Demands:" << delimiter << getNbRoutedDemands() - getNbInitialDemands() << "\n";
  		myfile.close();
	}
	else{
		std::cerr << "Unable to open file " << filePath << "\n";
	}
}


/* Builds file results.csv containing information about the main obtained results. */
void Instance::outputLogResults(std::string fileName, double time){
	std::string delimiter = ";";
	std::string filePath = this->input.getOutputPath() + "results.csv";
	std::ofstream myfile(filePath.c_str(), std::ios_base::app);
	if (myfile.is_open()){
		myfile << fileName << delimiter;
		int nbRouted = getNbRoutedDemands();
		myfile << nbRouted - getNbInitialDemands() << delimiter;
		myfile << nbRouted << "\n";
		myfile.close();
	}
	else{
		std::cerr << "Unable to open file " << filePath << "\n";
	}
}

/* Verifies if there exists a link between nodes of id u and v. */
bool Instance::hasLink(int u, int v){
	for (unsigned int e = 0; e < tabEdge.size(); e++){
		if ((tabEdge[e].getSource() == u) && (tabEdge[e].getTarget() == v)){
			return true;
		}
		if ((tabEdge[e].getSource() == v) && (tabEdge[e].getTarget() == u)){
			return true;
		}
	}
	return false;
}

/* Returns the first Fiber with source s and target t. */
Fiber Instance::getPhysicalLinkBetween(int u, int v){
	for (unsigned int e = 0; e < tabEdge.size(); e++){
		if ((tabEdge[e].getSource() == u) && (tabEdge[e].getTarget() == v)){
			return tabEdge[e];
		}
		if ((tabEdge[e].getSource() == v) && (tabEdge[e].getTarget() == u)){
			return tabEdge[e];
		}
	}
	std::cerr << "Did not found a link between " << u << " and " << v << "!!\n";
	exit(0);
	Fiber link(-1,-1,-1,-1);
	return link;
}

/*PEDRO PEDRO PEDRO*/
void Instance::setPaseNodeC() { 
	double h = 6.62 * pow(10,-34);                      //SI Joules second, J*s
    double lambd = 1545.0 * pow(10,-9);                 //SI meters, m                   #Usually nanometer (x nanometer)
    double c = 3.0 *pow(10,8);                          //SI meters by second, m 
    double nu = c/lambd;                                //SI hertz
    double NF = 5.0;                                    //SI dB
    double nsp = (1.0/2.0) * pow(10.0,NF/10.0);        
    double alpha = 0.2;                                 //NOT SI dB/kilometer 
    double a = log(10)*alpha/20 * pow(10,-3);           //SI 1/km
    double Gdb ;                                        //SI #dB
    double Glin ;                                       //LINEAR
    double Bn = 12.5 * pow(10,9);                       //SI Hertz                       #Usually gigahertz  (x ghz)
    double paseNod ;
    Gdb = 20.0;                                           //SI #dB
    Glin = pow(10.0,Gdb/10.0);                              //LINEAR
    paseNod = 2.0* h * nu * nsp * (Glin-1.0) * Bn;
	this->paseNodeC = paseNod; 
}

void Instance::setPaseNodeL() { 
	double h = 6.62 * pow(10,-34);                      //SI Joules second, J*s
    double lambd = 1580.0 * pow(10,-9);                 //SI meters, m                   #Usually nanometer (x nanometer)
    double c = 3.0 *pow(10,8);                          //SI meters by second, m 
    double nu = c/lambd;                                //SI hertz
    double NF = 5.5;                                    //SI dB
    double nsp = (1.0/2.0) * pow(10.0,NF/10.0);        
    double alpha = 0.22;                                 //NOT SI dB/kilometer 
    double a = log(10)*alpha/20 * pow(10,-3);           //SI 1/km
    double Gdb ;                                        //SI #dB
    double Glin ;                                       //LINEAR
    double Bn = 12.5 * pow(10,9);                       //SI Hertz                       #Usually gigahertz  (x ghz)
    double paseNod ;
    Gdb = 20;                                           //SI #dB
    Glin = pow(10,Gdb/10);                              //LINEAR
    paseNod = 2.0* h * nu * nsp * (Glin-1.0) * Bn;
	this->paseNodeL = paseNod; 
}

void Instance::setPaseNodeS() { 
	double h = 6.62 * pow(10,-34);                      //SI Joules second, J*s
    double lambd = 1490.0 * pow(10,-9);                 //SI meters, m                   #Usually nanometer (x nanometer)
    double c = 3.0 *pow(10,8);                          //SI meters by second, m 
    double nu = c/lambd;                                //SI hertz
    double NF = 7.0;                                    //SI dB
    double nsp = (1.0/2.0) * pow(10.0,NF/10.0);        
    double alpha = 0.22;                                 //NOT SI dB/kilometer 
    double a = log(10)*alpha/20 * pow(10,-3);           //SI 1/km
    double Gdb ;                                        //SI #dB
    double Glin ;                                       //LINEAR
    double Bn = 12.5 * pow(10,9);                       //SI Hertz                       #Usually gigahertz  (x ghz)
    double paseNod ;
    Gdb = 20;                                           //SI #dB
    Glin = pow(10,Gdb/10);                              //LINEAR
    paseNod = 2.0* h * nu * nsp * (Glin-1.0) * Bn;
	this->paseNodeS = paseNod; 
}