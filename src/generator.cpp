#include "generator.h"

Generator::Generator(std::string topology, std::string demand):topologyFile(topology),demandsFile(demand),nbNodes(0),compactArcId(compactGraph), compactArcLabel(compactGraph), 
                                compactArcLength(compactGraph), compactNodeId(compactGraph), 
                                compactNodeLabel(compactGraph){
    readTopology();
    buildCompactGraph();
}

/* Reads the topology information from file. */
void Generator::readTopology(){
	std::cout << "Reading " << topologyFile << "."  << std::endl;
	CSVReader reader(topologyFile);
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
		int edgeNbSlices = std::stoi(dataList[i][4]);
		double edgeCost = std::stod(dataList[i][5]);
		Fiber edge(idEdge, edgeIndex, edgeSource, edgeTarget, edgeLength, edgeNbSlices, edgeCost);
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

/* Builds the simple graph associated with the initial mapping. */
void Generator::buildCompactGraph(){
    for (int i = 0; i < getNbNodes(); i++){
        ListDigraph::Node n = compactGraph.addNode();
        compactNodeLabel[n] = i;
        compactNodeId[n] = compactGraph.id(n);
    }
    for (int i = 0; i < getNbEdges(); i++){
        Fiber edge = getPhysicalLinkFromIndex(i);
        int sourceLabel = edge.getSource();
        int targetLabel = edge.getTarget();
        ListDigraph::Node sourceNode = INVALID;
        ListDigraph::Node targetNode = INVALID;
        for (ListDigraph::NodeIt v(compactGraph); v != INVALID; ++v){
            if(compactNodeLabel[v] == sourceLabel){
                sourceNode = v;
            }
            if(compactNodeLabel[v] == targetLabel){
                targetNode = v;
            }
        }
        if (targetNode != INVALID && sourceNode != INVALID){
            ListDigraph::Arc a = compactGraph.addArc(sourceNode, targetNode);
            compactArcId[a] = compactGraph.id(a);
            compactArcLabel[a] = edge.getIndex();
            compactArcLength[a] = edge.getLength();

            ListDigraph::Arc a2 = compactGraph.addArc(targetNode, sourceNode);
            compactArcId[a2] = compactGraph.id(a);
            compactArcLabel[a2] = edge.getIndex();
            compactArcLength[a2] = edge.getLength();
        }
    }
}

void Generator::generateDemands(int nbDemands){

    /* initialize random seed: */
    srand (time(NULL));

    int sourceLabel, targetLabel, slice;
    double length;

    std::ofstream fichier(demandsFile.c_str());
    if (!fichier.fail()) {
        fichier << "index;origin;destination;slots;max_reach" << std::endl;
        std::string delimiter = ";";
        for(int i = 0; i < nbDemands;i++){
            bool STOP = false;
            while(STOP == false){
                sourceLabel = -1;
                targetLabel = -1;
                while(sourceLabel == targetLabel){
                    sourceLabel = rand() % getNbNodes();
                    targetLabel = rand() % getNbNodes();
                }

                slice = rand() % 4 + 3; 

                if(slice == 3){
                    length = 3000.0;
                }else if(slice == 4 || slice == 5){
                    length = 1500.0;
                }else if(slice == 6){
                    length = 600.0;
                }

                const ListDigraph::Node SOURCE = getFirstNodeFromLabel(sourceLabel);
                const ListDigraph::Node TARGET = getFirstNodeFromLabel(targetLabel);

                Dijkstra<ListDigraph,ArcCost> shortestPath(compactGraph,compactArcLength);
                shortestPath.run(SOURCE, TARGET);

                if(shortestPath.dist(TARGET)<=length){
                    STOP = true;
                }
            }
            fichier << i+1 << delimiter;
            fichier << sourceLabel + 1 << delimiter;
            fichier << targetLabel + 1 << delimiter;
            fichier << slice << delimiter;
            fichier << length << delimiter;
            fichier << std::endl;
        }
    }
    fichier.close();
}

ListDigraph::Node Generator::getFirstNodeFromLabel(int label) const{
    for(ListDigraph::NodeIt node(compactGraph); node != INVALID; ++node){
        if(compactNodeLabel[node]==label){
            return node;
        }
    }
}

