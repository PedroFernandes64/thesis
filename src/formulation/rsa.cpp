#include "rsa.h"

/* Constructor. A graph associated to the initial mapping (instance) is built as well as an extended graph for each demand to be routed. */
RSA::RSA(const Instance &inst) : instance(inst), compactEdgeId(compactGraph), compactEdgeLabel(compactGraph), 
                                compactEdgeLength(compactGraph), compactEdgeLineAmplifiers(compactGraph), compactNodeId(compactGraph), 
                                compactNodeLabel(compactGraph){
    setStatus(STATUS_UNKNOWN);
    /* Creates compact graph. */
    buildCompactGraph();
    ClockTime clock(ClockTime::getTimeNow());

    /* Set demands to be routed. */
    this->setToBeRouted(instance.getNextDemands());
    //displayToBeRouted();
    
    /* Set loads to be routed. */
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        int demandLoad = getToBeRouted_k(d).getLoad();
        if(std::find(loadsToBeRouted.begin(), loadsToBeRouted.end(), demandLoad) == loadsToBeRouted.end()) {
            loadsToBeRouted.push_back(demandLoad);
        }
    }
    //displayLoadsToBeRouted();

    /* Creates an extended graph for each one of the demands to be routed. */
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        
        vecGraph.emplace_back( std::make_shared<ListDigraph>() );
        vecArcId.emplace_back( std::make_shared<ArcMap>((*vecGraph[d])) );
        vecArcLabel.emplace_back( std::make_shared<ArcMap>((*vecGraph[d])) );
        vecArcSlice.emplace_back( std::make_shared<ArcMap>((*vecGraph[d])) );
        vecArcLength.emplace_back( std::make_shared<ArcCost>((*vecGraph[d])) );
        vecArcLineAmplifiers.emplace_back( std::make_shared<ArcMap>((*vecGraph[d])) );
        vecArcLengthWithPenalty.emplace_back( std::make_shared<ArcCost>((*vecGraph[d])) );
        vecNodeId.emplace_back( std::make_shared<NodeMap>((*vecGraph[d])) );
        vecNodeLabel.emplace_back( std::make_shared<NodeMap>((*vecGraph[d])) );
        vecNodeSlice.emplace_back(std::make_shared<NodeMap>((*vecGraph[d])) );
        vecOnPath.emplace_back( std::make_shared<ArcMap>((*vecGraph[d])) );

        vecArcIndex.emplace_back(std::make_shared<ArcMap>((*vecGraph[d])));
        vecNodeIndex.emplace_back(std::make_shared<NodeMap>((*vecGraph[d])));
        vecArcVarId.emplace_back(std::make_shared<ArcMap>((*vecGraph[d])));
    
        for (int i = 0; i < instance.getNbEdges(); i++){
            int linkSourceLabel = instance.getPhysicalLinkFromIndex(i).getSource();
            int linkTargetLabel = instance.getPhysicalLinkFromIndex(i).getTarget();
            int sliceLimit = getNbSlicesLimitFromEdge(i);
            for (int s = 0; s < sliceLimit; s++){
                /* IF SLICE s IS NOT USED */
                if (instance.getPhysicalLinkFromIndex(i).getSlice_i(s).isUsed() == false){
                    
                    if (instance.getInput().getChosenPartitionPolicy() == Input::PARTITION_POLICY_HARD){
                        bool onLeftRegion = true;
                        if (getToBeRouted_k(d).getLoad() > instance.getInput().getPartitionLoad()){
                            onLeftRegion = false;
                        }
                        if ( (onLeftRegion) && (s < instance.getInput().getPartitionSlice()) ){
                            addArcs(d, linkSourceLabel, linkTargetLabel, i, s, instance.getPhysicalLinkFromIndex(i).getLength(), instance.getPhysicalLinkFromIndex(i).getLineAmplifiers());
                            //addArcs(d, linkSourceLabel, linkTargetLabel, i, s, instance.getPhysicalLinkFromIndex(i).getLength());
                            //addArcs(d, linkTargetLabel, linkSourceLabel, i, s, instance.getPhysicalLinkFromIndex(i).getLength());
                            addArcs(d, linkTargetLabel, linkSourceLabel, i, s, instance.getPhysicalLinkFromIndex(i).getLength(), instance.getPhysicalLinkFromIndex(i).getLineAmplifiers());
                        }
                        if ( (!onLeftRegion) && (s >= instance.getInput().getPartitionSlice()) ){
                            addArcs(d, linkSourceLabel, linkTargetLabel, i, s, instance.getPhysicalLinkFromIndex(i).getLength(), instance.getPhysicalLinkFromIndex(i).getLineAmplifiers());
                            //addArcs(d, linkSourceLabel, linkTargetLabel, i, s, instance.getPhysicalLinkFromIndex(i).getLength());
                            //addArcs(d, linkTargetLabel, linkSourceLabel, i, s, instance.getPhysicalLinkFromIndex(i).getLength());
                            addArcs(d, linkTargetLabel, linkSourceLabel, i, s, instance.getPhysicalLinkFromIndex(i).getLength(), instance.getPhysicalLinkFromIndex(i).getLineAmplifiers());
                        }
                    }
                    else{
                        /* CREATE NODES (u, s) AND (v, s) IF THEY DO NOT ALREADY EXIST AND ADD AN ARC BETWEEN THEM */
                        addArcs(d, linkSourceLabel, linkTargetLabel, i, s, instance.getPhysicalLinkFromIndex(i).getLength(), instance.getPhysicalLinkFromIndex(i).getLineAmplifiers());
                        //addArcs(d, linkSourceLabel, linkTargetLabel, i, s, instance.getPhysicalLinkFromIndex(i).getLength());
                        //addArcs(d, linkTargetLabel, linkSourceLabel, i, s, instance.getPhysicalLinkFromIndex(i).getLength());
                        addArcs(d, linkTargetLabel, linkSourceLabel, i, s, instance.getPhysicalLinkFromIndex(i).getLength(), instance.getPhysicalLinkFromIndex(i).getLineAmplifiers());
                    }
                }
            }
        }
    }
    setRSAGraphConstructionTime(clock.getTimeInSecFromStart());
    //std::cout <<  "Graph construction: " << clock.getTimeInSecFromStart() << std::endl;
    
    clock.setStart(ClockTime::getTimeNow());
    /* Calls preprocessing. */
    preprocessing();

    setPreprocessingTime(clock.getTimeInSecFromStart());
    //std::cout <<  "Preprocessing: " << clock.getTimeInSecFromStart() << std::endl;

    /* Sets arcs and nodes index. Sets arcs id's (variables id). */
    sourceNodeIndex.resize(getNbDemandsToBeRouted());
    targetNodeIndex.resize(getNbDemandsToBeRouted());
    int varId = 0;
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){ 
        int index=0;
        for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
            setArcIndex(a, d, index);
            index++;
            (*vecArcVarId[d])[a] = varId;
            varId++;
        }
        int nodeIndex = 0;
        for(ListDigraph::NodeIt v(*vecGraph[d]); v != INVALID; ++v){
            int label = getNodeLabel(v,d);
            if(getToBeRouted_k(d).getSource() == label){
                sourceNodeIndex[d] = nodeIndex;
            }
            if(getToBeRouted_k(d).getTarget() == label){
                targetNodeIndex[d] = nodeIndex;
            }
            setNodeIndex(v,d,nodeIndex);
            nodeIndex++;
        }
        /* Copy the node label map and the arc label map to iterable maps. */
        mapItNodeLabel.emplace_back(std::make_shared<IterableIntMap<ListDigraph, ListDigraph::Node>>((*vecGraph[d])) );
        mapItArcLabel.emplace_back(std::make_shared<IterableIntMap<ListDigraph, ListDigraph::Arc>>((*vecGraph[d])) );
        mapCopy<ListDigraph,NodeMap,IterableIntMap<ListDigraph, ListDigraph::Node>>((*vecGraph[d]),(*vecNodeLabel[d]),(*mapItNodeLabel[d]));
        mapCopy<ListDigraph,ArcMap,IterableIntMap<ListDigraph, ListDigraph::Arc>>((*vecGraph[d]),(*vecArcLabel[d]),(*mapItArcLabel[d]));
    }
    maxSliceOverallVarId = varId;

    auxNbSlicesLimitFromEdge.resize(instance.getNbEdges());
    for(int i=0; i<instance.getNbEdges(); i++){
        auxNbSlicesLimitFromEdge[i] = getNbSlicesLimitFromEdge(i);
    }
    auxNbSlicesGlobalLimit = getNbSlicesGlobalLimit();
   
    /*PEDRO PEDRO PEDRO*/ /*PEDRO PEDRO PEDRO*/ /*PEDRO PEDRO PEDRO*/
    //Modulo GN MODEL
    if (this->getInstance().getInput().isGNModelEnabled() == true){
        std::cout << "GN Model pre processing" << std::endl;
        gnModelAllPaths();
    }
    /*PEDRO PEDRO PEDRO*/ /*PEDRO PEDRO PEDRO*/ /*PEDRO PEDRO PEDRO*/
}


/*PEDRO PEDRO PEDRO*/ /*PEDRO PEDRO PEDRO*/ /*PEDRO PEDRO PEDRO*/
void RSA::printAllPathsUtil(int u, int d, bool visited[], int path[], int& path_index)
{
    // Mark the current node and store it in path[]
    visited[u] = true;
    path[path_index] = u;
    path_index++;
 
    // If current vertex is same as destination, then print
    // current path[]
    if (u == d) {
        std::vector<ListGraph::Node> aux;
        for (int i = 0; i < path_index; i++){
            //std::cout << path[i] << " ";
            //std::cout << getCompactNodeLabel(getCompactNodeFromLabel(path[i])) << " "; ;
            aux.push_back(getCompactNodeFromLabel(path[i]));

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
                printAllPathsUtil(*i, d, visited, path, path_index);
    }
 
    // Remove current vertex from path[] and mark it as unvisited
    path_index--;
    visited[u] = false;
}

double RSA::paseLinFiber(double length, int spans){
    double h = 6.62 * pow(10,-34);                      //SI Joules second, J*s
    double lambd = 1545.0 * pow(10,-9);                 //SI meters, m                   #Usually nanometer (x nanometer)
    double c = 3.0 *pow(10,8);                          //SI meters by second, m 
    double nu = c/lambd;                                //SI hertz
    double NF = 5.0;                                    //SI dB
    double nsp = (1.0/2.0) * pow(10.0,NF/10.0);        
    double alpha = 0.2;                                 //NOT SI dB/kilometer 
    double a = log(10)*alpha/20 * pow(10,-3);           //SI 1/km
    double ls ;                                         //NOT SI kilometers
    double Gdb ;                                        //SI #dB
    double Glin ;                                       //LINEAR
    double Bn = 12.5 * pow(10,9);                       //SI Hertz                       #Usually gigahertz  (x ghz)
    double paseSpan ;
    double paseFiber;
    ls = length;                                             //NOT SI kilometers
    Gdb = alpha * ls;                                   //SI #dB
    Glin = pow(10,Gdb/10);                              //LINEAR
    paseSpan = 2.0* h * nu * nsp * (Glin-1.0) * Bn;
    paseFiber = paseSpan * spans;
    return paseFiber;
}

double RSA::pnliLinFiber(double length, int spans){
    double lambd = 1545.0 * pow(10,-9);                     //SI meters, m                   #Usually nanometer (x nanometer)
    double D = 16.5;                                            //#NOT SI ps/(nm km)
    double SI_D = D * pow (10,-6);                              //#SI s/m^2)
    double c = 3.0 *pow(10,8);                          //SI meters by second, m
    double beta2 = abs(SI_D) * pow(lambd,2)/(2*M_PIl*c);        //#SI s^2/m   
    double n2 = 2.7 * pow (10,-20);                             //#SI m^2/W               #Usually micrometer^2
    double aeff = 85 * pow (10,-12);                            //#SI m^2                 #Usually micrometer^2
    double gam = (n2/aeff) * (2*M_PIl/lambd);                   //#SI 1/W m
    double bwdm = 5000 * pow(10,9);                             //#SI #Hz                 #Usually gigahertz
    double Psat = 50 * pow(10,-3);                              //#SI #W                  #Usually mW
    double gwdm = Psat/bwdm;
    double alpha = 0.2;                                     //NOT SI dB/kilometer 
    double a = log(10)*alpha/20 * pow(10,-3);               //SI 1/km                                    //#SI #W/Hz
    double leff ;                                           //#SI #km
    double leff_a;                                          //#SI #km 
    double Ls = length * pow(10,3);                         //SI meters
    double Bn = 12.5 * pow(10,9);                       //SI Hertz                       #Usually gigahertz  (x ghz)
    double gnliSpan ;
    double gnliFiber;
    leff = (1.0 - exp(-2.0*a*Ls))/(2.0*a);                  //#SI #km
    leff_a = 1.0/(2.0 *a);                                  //#SI #km 
    gnliSpan = (8.0/27.0) * pow(gam,2) * pow(gwdm,3) * pow(leff,2) * (asinh(pow(M_PIl/2,2)*beta2*leff_a*pow(bwdm,2))/(M_PIl*beta2*leff_a)) * Bn;
    gnliFiber = gnliSpan * spans;
    return gnliFiber;
}
double RSA::paseNodePath(int nodes){
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
    double paseNode ;
    double paseNodeFiber;
    Gdb = 20;                                           //SI #dB
    Glin = pow(10,Gdb/10);                              //LINEAR
    paseNode = 2.0* h * nu * nsp * (Glin-1.0) * Bn;
    paseNodeFiber = paseNode * nodes;
    return paseNodeFiber;
}

double RSA::osnrPath(double paselin, double pasenode, double pnli, double slots){
    double Psat = 50 * pow(10,-3);
    double bwdm = 5000 * pow(10,9); 
    double gwdm = Psat/bwdm;
    double Bn = 12.5 * pow(10,9);                       //SI Hertz                       #Usually gigahertz  (x ghz)
    double pch;
    double osnr;
    double osnrdb;
    pch = slots * Bn * gwdm;			
    osnr = pch/(paselin + pasenode + pnli);
    osnrdb = 10.0 * log10(osnr);
    return osnrdb;
}

void RSA::gnModelAllPaths(){
    
    // ADJACENCY LIST
    std::vector<std::vector<int> > aux(instance.getNbNodes());
    adj_list = aux;
    //std::cout << "Edges:";
    for (ListGraph::EdgeIt i(compactGraph); i!=INVALID; ++i){
        //std::cout << " (" << compactGraph.id(compactGraph.u(i)) << "," << compactGraph.id(compactGraph.v(i)) << ")";
        adj_list[compactGraph.id(compactGraph.u(i))].push_back(compactGraph.id(compactGraph.v(i)));
        adj_list[compactGraph.id(compactGraph.v(i))].push_back(compactGraph.id(compactGraph.u(i)));
    }

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
        printAllPathsUtil(origin, destination, visited, path, path_index);
        allpaths.push_back(pathsdemand);
        pathsdemand.clear();
    }
    //manage paths
    std::vector<std::vector<double> > alldemandsdistances;
    std::vector<std::vector<double> > alldemandsPASEnode;
    std::vector<std::vector<double> > alldemandsPASElin;
    std::vector<std::vector<double> > alldemandsPNLI;
    std::vector<double> thisdemanddistances;
    std::vector<double> thisdemandPASEnode;
    std::vector<double> thisdemandPASElin;
    std::vector<double> thisdemandPNLI;
    int currentnode;
    int nextnode;
    double length;
    int spans;
    double lspan;
    int vertexamplis;
    double ASEfiber;
    double ASEpath;
    double PNLIfiber;
    double PNLIpath;
    double ASEnodepath;
    int fibernumberinpath;
    for (int i = 0; i <allpaths.size(); ++i){
        std::cout << "for demand " << i+1 <<std::endl;
        for (int j = 0; j <allpaths[i].size(); ++j){
            length = 0;
            vertexamplis = 0;
            ASEnodepath = 0;
            ASEpath = 0;
            PNLIpath = 0;
            std::cout << "path " << j+1 << " : "<< std::endl;
            fibernumberinpath = 0;
            for (int k = 0; k <allpaths[i][j].size()-1; ++k){
                fibernumberinpath = fibernumberinpath + 1;
                currentnode = getCompactNodeLabel(allpaths[i][j][k]);
                nextnode = getCompactNodeLabel(allpaths[i][j][k+1]);
                length += instance.getPhysicalLinkBetween(currentnode,nextnode).getLength();
                vertexamplis++;
                    //SPANS
                std::cout << "-Fiber: " << fibernumberinpath << " length: " << instance.getPhysicalLinkBetween(currentnode,nextnode).getLength() << std::endl;
                spans = ceil(instance.getPhysicalLinkBetween(currentnode,nextnode).getLength()/80.0);
                lspan = instance.getPhysicalLinkBetween(currentnode,nextnode).getLength()/spans;
                std::cout << "Has "  << spans << " spans of " << lspan << std::endl;
                //PASE
                ASEfiber = paseLinFiber(lspan,spans);
                std::cout << "Has "  << ASEfiber << " PASE for this fiber " << std::endl;
                ASEpath += ASEfiber;
                //GNLI
                PNLIfiber  = pnliLinFiber(lspan,spans);
                std::cout << "Has "  << PNLIfiber << " GNLI for this fiber " << std::endl;
                PNLIpath +=  PNLIfiber;
            }
            vertexamplis = vertexamplis +1;
            ASEnodepath = paseNodePath(vertexamplis);
            thisdemanddistances.push_back(length);
            thisdemandPASEnode.push_back(ASEnodepath);
            std::cout << "This demand has "  << ASEnodepath << " PASE node path "  << std::endl;
            thisdemandPASElin.push_back(ASEpath);
            std::cout << "This demand has "  << ASEpath << " PASE lin path "  << std::endl;
            thisdemandPNLI.push_back(PNLIpath);
            std::cout << "This demand has "  << PNLIpath << " PNLI path "  << std::endl << std::endl;
        }
        alldemandsdistances.push_back(thisdemanddistances);
        alldemandsPASEnode.push_back(thisdemandPASEnode);
        alldemandsPASElin.push_back(thisdemandPASElin);
        alldemandsPNLI.push_back(thisdemandPNLI);
        thisdemanddistances.clear();
        thisdemandPASEnode.clear();
        thisdemandPNLI.clear();
        thisdemandPASElin.clear();
    }

    //DEMANDE
    int slots;
    //PATH
    double distance;
    //OSNR
    double dbOsnr;
    double osnrlimdb = 23.5;
    std::cout << "Calculating OSNR " << std::endl;
    std::cout << "Writing  OSNR's to file..." << std::endl;
    std::ofstream fw("osnr.txt", std::ofstream::out);
    if (fw.is_open()){   
        for (int i = 0 ; i <toBeRouted.size(); i++){			
            fw << "OSNR demand: "  << i+1 << " : " << toBeRouted[i].getSource()+1 << " to " << toBeRouted[i].getTarget()+1 << std::endl;		
            for (int j = 0; j< alldemandsdistances[i].size(); ++j){
                distance = alldemandsdistances[i][j];
                slots = toBeRouted[i].getLoad();        
                dbOsnr = osnrPath(alldemandsPASElin[i][j], alldemandsPASEnode[i][j], alldemandsPNLI[i][j], slots);
                if((alldemandsdistances[i][j] <= toBeRouted[i].getMaxLength()) || (dbOsnr >= osnrlimdb) ){
                    fw << "=====Path=====: " << j+1 << std::endl;
                    fw << "PNLI " << alldemandsPNLI[i][j] << std::endl;
                    fw << "PASElin " << alldemandsPASElin[i][j] << std::endl;
                    fw << "PASEnode " << alldemandsPASEnode[i][j] << std::endl;
                    fw << "Nodes = ";
                    for (int k = 0; k <allpaths[i][j].size(); ++k)
                        fw << getCompactNodeLabel(allpaths[i][j][k]) + 1 << " ";
                    fw << std::endl;
                    fw << "Distance = " << distance;
                    fw << "---OSNR en db = " << dbOsnr << " ---" << std::endl;                     
                }
            }
            fw << "------------------------" << std::endl;
            }
        fw.close();
    }
    else{
        std::cout << "Problem with opening file";
    }       
}


/** Returns the total number of loads to be routed. **/
int RSA::getTotalLoadsToBeRouted() const{ 
    int total = 0;
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        total += getToBeRouted_k(d).getLoad();
    }
    return total;
}

/* Builds the simple graph associated with the initial mapping. */
void RSA::buildCompactGraph(){
    for (int i = 0; i < instance.getNbNodes(); i++){
        ListGraph::Node n = compactGraph.addNode();
        compactNodeLabel[n] = i;
        compactNodeId[n] = compactGraph.id(n);
    }
    for (int i = 0; i < instance.getNbEdges(); i++){
        Fiber edge = instance.getPhysicalLinkFromIndex(i);
        int sourceLabel = edge.getSource();
        int targetLabel = edge.getTarget();
        ListGraph::Node sourceNode = INVALID;
        ListGraph::Node targetNode = INVALID;
        for (ListGraph::NodeIt v(compactGraph); v != INVALID; ++v){
            if(compactNodeLabel[v] == sourceLabel){
                sourceNode = v;
            }
            if(compactNodeLabel[v] == targetLabel){
                targetNode = v;
            }
        }
        if (targetNode != INVALID && sourceNode != INVALID){
            ListGraph::Edge e = compactGraph.addEdge(sourceNode, targetNode);
            compactEdgeId[e] = compactGraph.id(e);
            compactEdgeLabel[e] = edge.getIndex();
            compactEdgeLength[e] = edge.getLength();
            compactEdgeLineAmplifiers[e] = edge.getLineAmplifiers();
        }
    }
}

/* Creates an arc -- and its nodes if necessary -- between nodes (source,slice) and (target,slice) on a graph. */
//void RSA::addArcs(int d, int linkSourceLabel, int linkTargetLabel, int linkLabel, int slice, double l){
void RSA::addArcs(int d, int linkSourceLabel, int linkTargetLabel, int linkLabel, int slice, double l, int la){
    ListDigraph::Node arcSource = getNode(d, linkSourceLabel, slice);
    ListDigraph::Node arcTarget = getNode(d, linkTargetLabel, slice);

    if (arcSource == INVALID){
        arcSource = vecGraph[d]->addNode();
        int id = vecGraph[d]->id(arcSource);
        setNodeId(arcSource, d, id);
        setNodeLabel(arcSource, d, linkSourceLabel);
        setNodeSlice(arcSource, d, slice);
        //displayNode(edgeSource);
    }
    if (arcTarget == INVALID){
        arcTarget = vecGraph[d]->addNode();
        int id = vecGraph[d]->id(arcTarget);
        setNodeId(arcTarget, d, id);
        setNodeLabel(arcTarget, d, linkTargetLabel);
        setNodeSlice(arcTarget, d, slice);
        //displayNode(edgeSource);
    }
    
    // CREATE ARC BETWEEN NODES arcSource AND arcTarget
    ListDigraph::Arc a = vecGraph[d]->addArc(arcSource, arcTarget);
    int id = vecGraph[d]->id(a);
    setArcId(a, d, id);
    setArcLabel(a, d, linkLabel);
    setArcSlice(a, d, slice);
    setArcLength(a, d, l);
    setArcLineAmplifiers(a, d, la);
    int hop = instance.getInput().getHopPenalty();
    if (linkSourceLabel == getToBeRouted_k(d).getSource()){
        setArcLengthWithPenalty(a, d, l);
    }
    else{
        setArcLengthWithPenalty(a, d, l + hop);
    }
    (*vecOnPath[d])[a] = -1;
    //displayEdge(a);
}

/* Returns the first node identified by (label, slice) on graph #d. */
ListDigraph::Node RSA::getNode(int d, int label, int slice){
    for (ListDigraph::NodeIt n(*vecGraph[d]); n != INVALID; ++n){
        if(getNodeLabel(n, d) == label && getNodeSlice(n, d) == slice){
            return n;
        }
    }
    return INVALID;
}

/* Updates the mapping stored in the given instance with the results obtained from RSA solution (i.e., vecOnPath).*/
void RSA::updateInstance(Instance &i){
    //instance.displaySlices();
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
            if ((*vecOnPath[d])[a] != -1){
                int id = (*vecOnPath[d])[a];
                Demand demand = i.getTabDemand()[id];
                i.assignSlicesOfLink(getArcLabel(a, d), getArcSlice(a, d), demand);
            }
        }
    }
    i.displaySlices();
    
    //update next demand to be routed index.
    int nextDemandToBeRouted = instance.getNextDemandToBeRoutedIndex() + getNbDemandsToBeRouted();
	if (instance.getInput().isBlockingAllowed()){
		if (instance.getWasBlocked() == true){
			nextDemandToBeRouted++;
		}
		i.setNbDemandsAtOnce(instance.getInput().getNbDemandsAtOnce());
	}
	else{
		if (instance.getWasBlocked() == true){
			i.setNbDemandsAtOnce(0);
		}
	}
    i.setNextDemandToBeRoutedIndex(nextDemandToBeRouted);
}

/* Returns the first node with a given label from the graph associated with the d-th demand to be routed. If such node does not exist, return INVALID. */
ListDigraph::Node RSA::getFirstNodeFromLabel(int d, int label){
    for (ListDigraph::NodeIt v(*vecGraph[d]); v != INVALID; ++v){
        if (getNodeLabel(v, d) == label){
            return v;
        }
    }
    return INVALID;
}

/* Contract nodes with the same given label from the graph associated with the d-th demand to be routed. */
void RSA::contractNodesFromLabel(int d, int label){
    int nb = 0;
    ListDigraph::NodeIt previousNode(*vecGraph[d]);
    ListDigraph::Node n = getFirstNodeFromLabel(d, label);
    ListDigraph::NodeIt v(*vecGraph[d]);
    ListDigraph::NodeIt currentNode(*vecGraph[d], v);
    if (n != INVALID){
        (*vecNodeSlice[d])[n] = -1;
        while (v != INVALID){
            currentNode = v;
            ListDigraph::NodeIt nextNode(*vecGraph[d], ++currentNode);
            currentNode = v;
            if ( (getNodeLabel(v, d) == label) && ((*vecGraph[d]).id(n) != (*vecGraph[d]).id(v)) ){
                (*vecGraph[d]).contract(n, v);
                nb++;
            }
            v = nextNode;
        }
    }
    //std::cout << "> Number of nodes with label " << label << " contracted: " << nb << std::endl; 
}

/* Delete arcs that are known 'a priori' to be unable to route on graph #d. */
void RSA::eraseNonRoutableArcs(int d){
    int nb = 0;
    ListDigraph::ArcIt previousArc(*vecGraph[d]);
    ListDigraph::ArcIt a(*vecGraph[d]);
    ListDigraph::ArcIt currentArc(*vecGraph[d], a);
    int demandSource = getToBeRouted_k(d).getSource();
    int demandTarget = getToBeRouted_k(d).getTarget();
    while (a != INVALID){
        currentArc = a;
        ListDigraph::ArcIt nextArc(*vecGraph[d], ++currentArc);
        currentArc = a;
        int label = getArcLabel(a, d);
        int slice = getArcSlice(a, d);
        int uLabel = getNodeLabel((*vecGraph[d]).source(a), d);
        int vLabel = getNodeLabel((*vecGraph[d]).target(a), d);
        if ( (instance.hasEnoughSpace(label, slice, getToBeRouted_k(d)) == false) || (uLabel == demandTarget) || (vLabel == demandSource) ){
            (*vecGraph[d]).erase(a);
            nb++;
        }
        a = nextArc;
    }
    //std::cout << "> Number of non-routable arcs erased on graph #" << d << ": " << nb << std::endl; 
}

/* Runs preprocessing on every extended graph. */
void RSA::preprocessing(){
    std::vector<int> nbArcsOld(getNbDemandsToBeRouted(), 0);
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        nbArcsOld[d] = countArcs((*vecGraph[d]));
    }

    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        eraseNonRoutableArcs(d);
    }
    if (getInstance().getInput().getChosenPreprLvl() >= Input::PREPROCESSING_LVL_PARTIAL){
        // do partial preprocessing;
        pathExistencePreprocessing();
        bool keepPreprocessing = lengthPreprocessing();

        if (getInstance().getInput().getChosenPreprLvl() >= Input::PREPROCESSING_LVL_FULL){
            // do full preprocessing;
            while (keepPreprocessing){
                pathExistencePreprocessing();
                keepPreprocessing = lengthPreprocessing();
            }
        }
    }
    
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        contractNodesFromLabel(d, getToBeRouted()[d].getSource());
        contractNodesFromLabel(d, getToBeRouted()[d].getTarget());
    }

    //for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        //std::cout << "> Number of arcs in graph #" << d << " before preprocessing: " << nbArcsOld[d] << ". After: " << countArcs((*vecGraph[d])) << std::endl;
    //}
}

/* Erases every arc from graph #d having the given slice and returns the number of arcs removed. */
void RSA::pathExistencePreprocessing(){
    std::cout << "Called Path Existence preprocessing."<< std::endl;
    int totalNb = 0;
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        bool STOP = false;
        while (!STOP){
            int nb = 0;
            ListDigraph::ArcIt previousArc(*vecGraph[d]);
            ListDigraph::ArcIt a(*vecGraph[d]);
            ListDigraph::ArcIt currentArc(*vecGraph[d], a);

            while (a != INVALID){
                currentArc = a;
                ListDigraph::ArcIt nextArc(*vecGraph[d], ++currentArc);
                currentArc = a;
                int slice = getArcSlice(a, d);
                ListDigraph::Node source = getNode(d, getToBeRouted_k(d).getSource(), slice);
                ListDigraph::Node target = getNode(d, getToBeRouted_k(d).getTarget(), slice);
                if (source == INVALID || target == INVALID){
                    (*vecGraph[d]).erase(a);
                    nb++;
                    totalNb++;
                }
                a = nextArc;
            }

            if (nb == 0){
                STOP = true;
            }
        }
    }
    std::cout << "> Number of erased arcs due to Path Existence: " << totalNb << std::endl;
}

/* Performs preprocessing based on the arc lengths and returns true if at least one arc is erased. */
bool RSA::lengthPreprocessing(){
    std::cout << "Called Length preprocessing."<< std::endl;
    int totalNb = 0;
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        //displayGraph(d);
        int nb = 0;
        int nbElse = 0;
        ListDigraph::ArcIt previousArc(*vecGraph[d]);
        ListDigraph::ArcIt a(*vecGraph[d]);
        ListDigraph::ArcIt currentArc(*vecGraph[d], a);
        while (a != INVALID){
            currentArc = a;
            //displayArc(d, a);
            ListDigraph::ArcIt nextArc(*vecGraph[d], ++currentArc);
            //currentArc = a;
            int slice = getArcSlice(a, d);
            ListDigraph::Node source = getNode(d, getToBeRouted_k(d).getSource(), slice);
            ListDigraph::Node target = getNode(d, getToBeRouted_k(d).getTarget(), slice);
            if (source != INVALID && target != INVALID){
                if (shortestDistance(d, source, a, target) >= getToBeRouted_k(d).getMaxLength() + DBL_EPSILON){
                    //std::cout << "demand " <<  d << std::endl;
                    //std::cout << shortestDistance(d, source, a, target) <<std:: endl;
                    //std::cout << getToBeRouted_k(d).getMaxLength() + DBL_EPSILON <<std:: endl;
                    //displayArc(d, a);
                    //std::cout << "length  " <<   getArcLength(a, d) << " penalties " << getArcLengthWithPenalties(a,d) << std::endl;
                    (*vecGraph[d]).erase(a);
                    nb++;
                    totalNb++;
                }
            }
            else{
                (*vecGraph[d]).erase(a);
                nbElse++;
                totalNb++;
            }
            a = nextArc;
            //std::cout << "next arc" << std::endl;
            //displayArc(d, a);
        }
        //std::cout << "> Number of erased arcs due to length in graph #" << d << ". If: " << nb << ". Else: " << nbElse << std::endl;
    }
    if (totalNb >= 1){
        std::cout << "> Number of erased arcs due to length in graph: "<< totalNb << std::endl;
        return true;
    }
    return false;
}

/* Returns the distance of the shortest path from source to target passing through arc a. */
double RSA::shortestDistance(int d, ListDigraph::Node &s, ListDigraph::Arc &a, ListDigraph::Node &t){
    double distance = 0.0;
    Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > s_u_path((*vecGraph[d]), (*vecArcLengthWithPenalty[d]));

    s_u_path.run(s,(*vecGraph[d]).source(a));
    if (s_u_path.reached((*vecGraph[d]).source(a))){
        distance += s_u_path.dist((*vecGraph[d]).source(a));
    }
    else{
        return DBL_MAX;
    }
    //std::cout << distance << " " << std::endl;
    distance += getArcLengthWithPenalties(a, d);
    //std::cout << distance << " " << std::endl;
    Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > v_t_path((*vecGraph[d]), (*vecArcLengthWithPenalty[d]));
    v_t_path.run((*vecGraph[d]).target(a), t);
    if (v_t_path.reached(t)){
        distance += v_t_path.dist(t);
    }
    else{
        return DBL_MAX;
    }
    //std::cout << distance << " " << std::endl;
    return distance;
}

/* Returns the coefficient of an arc according to metric 1 on graph #d. */
double RSA::getCoeffObj1(const ListDigraph::Arc &a, int d){
    double coeff = 0.0;
    int NB_EDGES = instance.getNbEdges();
    ListDigraph::Node u = (*vecGraph[d]).source(a);
    int uLabel = getNodeLabel(u, d);
    int arcSlice = getArcSlice(a, d);
    int arcLabel = getArcLabel(a, d);
    if(uLabel == getToBeRouted_k(d).getSource()){
        switch (instance.getInput().getChosenPartitionPolicy() ){
            case Input::PARTITION_POLICY_NO:
                coeff = (arcSlice + 1);
            break;
            case Input::PARTITION_POLICY_SOFT:
                if(getToBeRouted_k(d).getLoad() <= instance.getInput().getPartitionLoad()){
                    coeff = arcSlice + 1; 
                }
                else{
                    coeff = (instance.getPhysicalLinkFromIndex(arcLabel).getNbSlices() - arcSlice);
                }
            break;
            case Input::PARTITION_POLICY_HARD:
                if(getToBeRouted_k(d).getLoad() <= instance.getInput().getPartitionLoad()){
                    coeff = (arcSlice + 1); 
                }
                else{
                    coeff = (instance.getPhysicalLinkFromIndex(arcLabel).getNbSlices() - arcSlice);
                }
            break;
            default:
                std::cout << "ERROR: Partition policy not recognized." << std::endl;
                exit(0);
            break;
        }
    }
    else{
        coeff = 0; 
    }
    return coeff;
}

/* Returns the coefficient of an arc according to metric 1p on graph #d. */
double RSA::getCoeffObj1p(const ListDigraph::Arc &a, int d){
    double coeff = 0.0;
    int arcLabel = getArcLabel(a, d);
    int arcSlice = getArcSlice(a, d);
    int maxSliceUsedOnLink = instance.getPhysicalLinkFromIndex(arcLabel).getMaxUsedSlicePosition();
    if(arcSlice <= maxSliceUsedOnLink){
        coeff = maxSliceUsedOnLink; 
    }
    else{
        coeff = arcSlice; 
    }
    return coeff;
}

/* Returns the coefficient of an arc according to metric 2 on graph #d. */
double RSA::getCoeffObj2(const ListDigraph::Arc &a, int d){
    double coeff = 1.0;
    return coeff;
}

/* Returns the coefficient of an arc according to metric 2p on graph #d. */
double RSA::getCoeffObj2p(const ListDigraph::Arc &a, int d){
    double coeff = getToBeRouted_k(d).getLoad();
    return coeff;
}

/* Returns the coefficient of an arc according to metric 4 on graph #d. */
double RSA::getCoeffObj4(const ListDigraph::Arc &a, int d){
    return getArcLength(a, d);
}

/* Returns the coefficient of an arc according to metric 8 on graph #d. */
double RSA::getCoeffObj8(const ListDigraph::Arc &a, int d){
    double coeff = 0.0;
    int maxSliceUsed = instance.getMaxUsedSlicePosition();
    int arcSlice = getArcSlice(a, d);
    ListDigraph::Node u = (*vecGraph[d]).source(a);
    int uLabel = getNodeLabel(u, d);
    if(uLabel == getToBeRouted_k(d).getSource()){
        if(arcSlice <= maxSliceUsed){
            coeff = maxSliceUsed; 
        }
        else{
            coeff = arcSlice; 
        }
    }
    else{
        coeff = 0;
    }
    return coeff;
}

/* Returns the coefficient of an arc (according to the chosen metric) on graph #d. */
double RSA::getCoeff(const ListDigraph::Arc &a, int d){
    double coeff = 0.0;
    switch (getInstance().getInput().getChosenObj_k(0)){
        case Input::OBJECTIVE_METRIC_0:
        {
            coeff = 0;
            break;
        }
        case Input::OBJECTIVE_METRIC_1:
        {
            coeff = getCoeffObj1(a, d);
            break;
        }
        case Input::OBJECTIVE_METRIC_1p:
        {
            coeff = getCoeffObj1p(a, d);
            break;
        }
        case Input::OBJECTIVE_METRIC_2:
        {
            coeff = getCoeffObj2(a, d);
            break;
        }
        case Input::OBJECTIVE_METRIC_2p:
        {
            coeff = getCoeffObj2p(a, d);
            break;
        }
        case Input::OBJECTIVE_METRIC_4:
        {
            coeff = getCoeffObj4(a, d);
            break;
        }
        case Input::OBJECTIVE_METRIC_8:
        {
            coeff = getCoeffObj8(a, d);
            break;
        }
        default:
        {
            std::cerr << "Objective metric out of range.\n";
            exit(0);
            break;
        }
    }
    
    return coeff;
}


ListGraph::Node RSA::getCompactNodeFromLabel(int label) const {
    for (ListGraph::NodeIt v(compactGraph); v != INVALID; ++v){
        if (getCompactNodeLabel(v) == label){
            return v;
        }
    }
    return INVALID;
}

/* Displays the demands to be routed in the next optimization. */
void RSA::displayToBeRouted(){
    std::cout << "--- ROUTING DEMANDS --- " << std::endl;
    for (int i = 0; i < getNbDemandsToBeRouted(); i++){
        std::cout << "#" << toBeRouted[i].getId()+1;
        std::cout << " (" << toBeRouted[i].getSource()+1 << ", " << toBeRouted[i].getTarget()+1 << "), ";
        std::cout << "requiring " << toBeRouted[i].getLoad() << " slices and having Max length ";
        std::cout << toBeRouted[i].getMaxLength() << std::endl;
    }
}
/* Displays the loads to be routed in the next optimization. */
void RSA::displayLoadsToBeRouted(){
    std::cout << "--> THE DIFFERENT ROUTING LOADS: ";
    if (getNbLoadsToBeRouted() <= 0){
        std::cout << "\nERROR: Loads have not been computed." << std::endl;
        exit(0);
    }
    std::cout << loadsToBeRouted[0];
    for (int i = 1; i < getNbLoadsToBeRouted(); i++){
        std::cout << ", " << loadsToBeRouted[i] ;
    }
    std::cout << "." << std::endl;
}

/* Displays the nodes of graph #d that are incident to the node identified by (label,slice). */
void RSA::displayNodesIncidentTo(int d, int label, int slice){
    ListDigraph::Node target = getNode(d, label, slice);
    std::cout << "Nodes incident to (" << label << ", " << slice << "): " << std::endl;
    for (ListDigraph::InArcIt a(*vecGraph[d], target); a != INVALID; ++a){
        ListDigraph::Node incident = (*vecGraph[d]).source(a);
        std::cout << "(" << getNodeLabel(incident, d) << ", " << getNodeSlice(incident, d) << ")" << std::endl;
    }
    for (ListDigraph::OutArcIt a(*vecGraph[d], target); a != INVALID; ++a){
        ListDigraph::Node incident = (*vecGraph[d]).source(a);
        std::cout << "(" <<  getNodeLabel(incident, d) << ", " << getNodeSlice(incident, d) << ")" << std::endl;
    }
}

/* Displays the nodes of graph #d that have a given label. */
void RSA::displayNodesFromLabel(int d, int label){
    std::cout << "Nodes with label " << label << ": " << std::endl;
    for (ListDigraph::NodeIt n(*vecGraph[d]); n != INVALID; ++n){
        if(getNodeLabel(n, d) == label){
            std::cout << "(" << getNodeLabel(n, d) << ", " << getNodeSlice(n, d) << ")" << std::endl;
        }
    }
}

/* Displays the paths found for each of the new routed demands. */
void RSA::displayPaths(){
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        std::cout << "For demand " << getToBeRouted_k(d).getId() + 1 << " : " << std::endl;
        for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
            if ((*vecOnPath[d])[a] != -1){
                displayArc(d, a);
            }
        }
        std::cout << std::endl;
    }
}

/* Displays an arc from the graph #d. */
void RSA::displayArc(int d, const ListDigraph::Arc &a){
    std::cout << "(" << getNodeLabel((*vecGraph[d]).source(a), d) + 1;
    std::cout << "--";
    std::cout <<  getNodeLabel((*vecGraph[d]).target(a), d) + 1 << ", " << getArcSlice(a, d) + 1 << ")" << std::endl;
}


/* Display all arcs from the graph #d. */
void RSA::displayGraph(int d){
    for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
        displayArc(d, a);
    }
}

/* Displays a node from the graph #d. */
void RSA::displayNode(int d, const ListDigraph::Node &n){
    std::cout << "(" << getNodeLabel(n, d)+1 << "," << getNodeSlice(n, d)+1 << ")";
}


int RSA::getDegree(const ListGraph::Node& node) const{
    int degree = 0;
    for (ListGraph::IncEdgeIt a(compactGraph, node); a != INVALID; ++a){
        degree++;
    }
    return degree;
}

int RSA::getCutCardinality(const std::vector<int> & cutSet) const{
    int cardinality = 0;
    for (ListGraph::EdgeIt e(compactGraph); e != INVALID; ++e){
        int labelU = getCompactNodeLabel(compactGraph.u(e));
        int labelV = getCompactNodeLabel(compactGraph.v(e));
        bool uInCut = (std::find(cutSet.begin(), cutSet.end(), labelU) != cutSet.end());
        bool vInCut = (std::find(cutSet.begin(), cutSet.end(), labelV) != cutSet.end());
        if (vInCut != uInCut){
            cardinality++;
        }
    }
    return cardinality;
}
/****************************************************************************************/
/*										Destructor										*/
/****************************************************************************************/

/* Destructor. Clears the vectors of demands and links. */
RSA::~RSA() {
	toBeRouted.clear();
	loadsToBeRouted.clear();
    vecArcId.clear();
    vecArcLabel.clear();
    vecArcSlice.clear();
    vecArcLength.clear();
    vecArcLineAmplifiers.clear();
    vecNodeId.clear();
    vecNodeLabel.clear();
    vecNodeSlice.clear();
    vecOnPath.clear();
    vecArcIndex.clear();
    vecNodeIndex.clear();
    vecArcVarId.clear();
    sourceNodeIndex.clear();
    targetNodeIndex.clear();
    vecGraph.clear();
}
