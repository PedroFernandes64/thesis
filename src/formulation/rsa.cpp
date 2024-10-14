#include "rsa.h"

/* Constructor. A graph associated to the initial mapping (instance) is built as well as an extended graph for each demand to be routed. */
RSA::RSA(const Instance &inst) : instance(inst), compactEdgeId(compactGraph), compactEdgeLabel(compactGraph), 
                                compactEdgeLength(compactGraph), compactEdgeLineAmplifiers(compactGraph), compactEdgePnliC(compactGraph), compactEdgePaseLineC(compactGraph),compactEdgePnliL(compactGraph), 
                                compactEdgePaseLineL(compactGraph),compactEdgeNoiseC(compactGraph),compactEdgeNoiseL(compactGraph),compactEdgeDispersionC(compactGraph),compactEdgeDispersionL(compactGraph), compactNodeId(compactGraph), 
                                compactNodeLabel(compactGraph){
    setStatus(STATUS_UNKNOWN);
    /* Creates compact graph. */
    buildCompactGraph();
    ClockTime clock(ClockTime::getTimeNow());

    this->setNbBands(inst.getInput().getNbBands());
    /* Set demands to be routed. */
    this->setToBeRouted(instance.getNextDemands());
    //displayToBeRouted();
    
    /* Set loads to be routed. */
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        int demandLoad = 0;
        if(getToBeRouted_k(d).getLoadC() <= getToBeRouted_k(d).getLoadL()){
            demandLoad = getToBeRouted_k(d).getLoadL();
        }else{      
            demandLoad = getToBeRouted_k(d).getLoadC();
        }

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
        vecArcPnliC.emplace_back( std::make_shared<ArcCost>((*vecGraph[d])) );
        vecArcPaseLineC.emplace_back( std::make_shared<ArcCost>((*vecGraph[d])) );
        vecArcNoiseC.emplace_back( std::make_shared<ArcCost>((*vecGraph[d])) );
        vecArcPnliL.emplace_back( std::make_shared<ArcCost>((*vecGraph[d])) );
        vecArcPaseLineL.emplace_back( std::make_shared<ArcCost>((*vecGraph[d])) );
        vecArcNoiseL.emplace_back( std::make_shared<ArcCost>((*vecGraph[d])) );
        vecArcDispersionC.emplace_back( std::make_shared<ArcCost>((*vecGraph[d])) );
        vecArcDispersionL.emplace_back( std::make_shared<ArcCost>((*vecGraph[d])) );

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
            if(nbBands>1){
                sliceLimit = instance.getPhysicalLinkFromIndex(i).getNbSlices();
            }
            for (int s = 0; s < sliceLimit; s++){
                /* IF SLICE s IS NOT USED */
                if (instance.getPhysicalLinkFromIndex(i).getSlice_i(s).isUsed() == false){
                    /* CREATE NODES (u, s) AND (v, s) IF THEY DO NOT ALREADY EXIST AND ADD AN ARC BETWEEN THEM */
                    addArcs(d, linkSourceLabel, linkTargetLabel, i, s, instance.getPhysicalLinkFromIndex(i).getLength(), instance.getPhysicalLinkFromIndex(i).getLineAmplifiers(), instance.getPhysicalLinkFromIndex(i).getPnliC(), instance.getPhysicalLinkFromIndex(i).getPaseLineC(), instance.getPhysicalLinkFromIndex(i).getNoiseC(),instance.getPhysicalLinkFromIndex(i).getPnliL(), instance.getPhysicalLinkFromIndex(i).getPaseLineL(),instance.getPhysicalLinkFromIndex(i).getNoiseL(),instance.getPhysicalLinkFromIndex(i).getDispersionCoeffC()*instance.getPhysicalLinkFromIndex(i).getLength(),instance.getPhysicalLinkFromIndex(i).getDispersionCoeffL()*instance.getPhysicalLinkFromIndex(i).getLength());
                    addArcs(d, linkTargetLabel, linkSourceLabel, i, s, instance.getPhysicalLinkFromIndex(i).getLength(), instance.getPhysicalLinkFromIndex(i).getLineAmplifiers(), instance.getPhysicalLinkFromIndex(i).getPnliC(), instance.getPhysicalLinkFromIndex(i).getPaseLineC(), instance.getPhysicalLinkFromIndex(i).getNoiseC(),instance.getPhysicalLinkFromIndex(i).getPnliL(), instance.getPhysicalLinkFromIndex(i).getPaseLineL(),instance.getPhysicalLinkFromIndex(i).getNoiseL(),instance.getPhysicalLinkFromIndex(i).getDispersionCoeffC()*instance.getPhysicalLinkFromIndex(i).getLength(),instance.getPhysicalLinkFromIndex(i).getDispersionCoeffL()*instance.getPhysicalLinkFromIndex(i).getLength());
                }
            }
        }
    }
    setRSAGraphConstructionTime(clock.getTimeInSecFromStart());
    //std::cout <<  "Graph construction: " << clock.getTimeInSecFromStart() << std::endl;
    
    clock.setStart(ClockTime::getTimeNow());
    /* Calls preprocessing. */
    variablesSetTo0 = 0;
    preprocessingConstraints = 0;
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

    //PEDRO PEDRO PEDRO
    //ALL PATHS MODULE
    possiblePaths = 0;
    feasiblePathsC = 0;
    onlyOsnrFeasiblePathsC = 0;
    onlyReachFeasiblePathsC = 0;
    infeasiblePathsC = 0;
    feasiblePathsC = 0;
    onlyOsnrFeasiblePathsC = 0;
    onlyReachFeasiblePathsC = 0;
    infeasiblePathsC = 0;
    //if (this->getInstance().getInput().allPathsDataEnabled() == true){
    if(false){
        std::cout << "Computing statistics of all paths" << std::endl;
        AllPaths();
    }
    
    //PEDRO PEDRO PEDRO
}
//PEDRO PEDRO PEDRO
void RSA::AllPathsUtil(int u, int d, bool visited[], int path[], int& path_index)
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
                AllPathsUtil(*i, d, visited, path, path_index);
    }
 
    // Remove current vertex from path[] and mark it as unvisited
    path_index--;
    visited[u] = false;
}
/*
double RSA::osnrPathC(double paselinc, double pasenodec, double pnlic, double pchc){
    double osnr;
    double osnrdb;		
    osnr = pchc/(paselinc + pasenodec + pnlic);
    osnrdb = 10.0 * log10(osnr);
    return osnrdb;
}

double RSA::osnrPathL(double paselinl, double pasenodel, double pnlil, double pchl){
    double osnr;
    double osnrdb;		
    osnr = pchl/(paselinl + pasenodel + pnlil);
    osnrdb = 10.0 * log10(osnr);
    return osnrdb;
}
double RSA::osnrPathS(double paselins, double pasenodes, double pnlis, double pchs){
    double osnr;
    double osnrdb;		
    osnr = pchs/(paselins + pasenodes + pnlis);
    osnrdb = 10.0 * log10(osnr);
    return osnrdb;
}
*/
double RSA::osnrPath(double noise, double pch){
    double osnr;
    double osnrdb;		
    osnr = pch/(noise);
    osnrdb = 10.0 * log10(osnr);
    return osnrdb;
}

void RSA::AllPaths(){
    
    //Building a structure with all possible paths for all possible demands

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
        AllPathsUtil(origin, destination, visited, path, path_index);
        allpaths.push_back(pathsdemand);
        pathsdemand.clear();
    }
    //manage paths
    std::vector<std::vector<double> > alldemandsdistances;
    std::vector<std::vector<double> > alldemandsPASEnodeC;
    std::vector<std::vector<double> > alldemandsPASElinC;
    std::vector<std::vector<double> > alldemandsPNLIC;
    std::vector<std::vector<double> > alldemandsPASEnodeL;
    std::vector<std::vector<double> > alldemandsPASElinL;
    std::vector<std::vector<double> > alldemandsPNLIL;
    std::vector<std::vector<double> > alldemandsPASEnodeS;
    std::vector<std::vector<double> > alldemandsPASElinS;
    std::vector<std::vector<double> > alldemandsPNLIS;
    std::vector<std::vector<double> > alldemandsNoiseC;
    std::vector<std::vector<double> > alldemandsNoiseL;
    std::vector<std::vector<double> > alldemandsNoiseS;
    std::vector<std::vector<double> > alldemandsDispersionC;
    std::vector<std::vector<double> > alldemandsDispersionL;
    std::vector<double> thisdemanddistances;
    std::vector<double> thisdemandPASEnodeC;
    std::vector<double> thisdemandPASElinC;
    std::vector<double> thisdemandPNLIC;
    std::vector<double> thisdemandPASEnodeL;
    std::vector<double> thisdemandPASElinL;
    std::vector<double> thisdemandPNLIL;
    std::vector<double> thisdemandPASEnodeS;
    std::vector<double> thisdemandPASElinS;
    std::vector<double> thisdemandPNLIS;
    std::vector<double> thisdemandNoiseC;
    std::vector<double> thisdemandNoiseL;
    std::vector<double> thisdemandNoiseS;
    std::vector<double> thisdemandDispersionC;
    std::vector<double> thisdemandDispersionL;

    int currentnode;
    int nextnode;
    int vertexamplis;
    double length;
    double pnliPathC;
    double paseLinPathC;
    double paseNodepathC;
    double pnliPathL;
    double paseLinPathL;
    double paseNodepathL;
    double pnliPathS;
    double paseLinPathS;
    double paseNodepathS;
    double noisePathC;
    double noisePathL;
    double noisePathS;
    double dispersionPathC;
    double dispersionPathL;
    int fibernumberinpath;

    for (int i = 0; i <allpaths.size(); ++i){
        for (int j = 0; j <allpaths[i].size(); ++j){
            vertexamplis = 0;
            pnliPathC = 0.0;
            paseLinPathC = 0.0;
            paseNodepathC = 0.0;
            pnliPathL = 0.0;
            paseLinPathL = 0.0;
            paseNodepathL = 0.0;
            pnliPathS = 0.0;
            paseLinPathS = 0.0;
            paseNodepathS = 0.0;
            noisePathC = 0.0;
            noisePathL = 0.0;
            noisePathS = 0.0;  
            dispersionPathC = 0.0;
            dispersionPathL = 0.0;  
            length = 0;          
            fibernumberinpath = 0;
            for (int k = 0; k <allpaths[i][j].size()-1; ++k){
                fibernumberinpath = fibernumberinpath + 1;
                currentnode = getCompactNodeLabel(allpaths[i][j][k]);
                nextnode = getCompactNodeLabel(allpaths[i][j][k+1]);
                vertexamplis++;
                length += instance.getPhysicalLinkBetween(currentnode,nextnode).getLength();
                //PNLI
                pnliPathC += instance.getPhysicalLinkBetween(currentnode,nextnode).getPnliC();
                paseLinPathC += instance.getPhysicalLinkBetween(currentnode,nextnode).getPaseLineC();

                pnliPathL += instance.getPhysicalLinkBetween(currentnode,nextnode).getPnliL();
                paseLinPathL += instance.getPhysicalLinkBetween(currentnode,nextnode).getPaseLineL();
            
                pnliPathS += instance.getPhysicalLinkBetween(currentnode,nextnode).getPnliS();
                paseLinPathS += instance.getPhysicalLinkBetween(currentnode,nextnode).getPaseLineS();

                noisePathC += instance.getPhysicalLinkBetween(currentnode,nextnode).getNoiseC();
                noisePathL += instance.getPhysicalLinkBetween(currentnode,nextnode).getNoiseL();                 
                noisePathS += instance.getPhysicalLinkBetween(currentnode,nextnode).getNoiseS();  

                dispersionPathC += (instance.getPhysicalLinkBetween(currentnode,nextnode).getDispersionCoeffC()*instance.getPhysicalLinkBetween(currentnode,nextnode).getLength());
                dispersionPathL += (instance.getPhysicalLinkBetween(currentnode,nextnode).getDispersionCoeffL()*instance.getPhysicalLinkBetween(currentnode,nextnode).getLength());
                
            }

            vertexamplis = vertexamplis +1;
            paseNodepathC = vertexamplis * instance.getPaseNodeC();
            paseNodepathL = vertexamplis * instance.getPaseNodeL();
            paseNodepathS = vertexamplis * instance.getPaseNodeS();
            //noisePathC += instance.getPaseNodeC(); //RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
            //noisePathL += instance.getPaseNodeL(); //RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK               
            //noisePathS += instance.getPaseNodeS(); //RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
            thisdemanddistances.push_back(length);
            thisdemandPASEnodeC.push_back(paseNodepathC);
            thisdemandPASElinC.push_back(paseLinPathC);
            thisdemandPNLIC.push_back(pnliPathC);
            thisdemandPASEnodeL.push_back(paseNodepathL);
            thisdemandPASElinL.push_back(paseLinPathL);
            thisdemandPNLIL.push_back(pnliPathL);
            thisdemandPASEnodeS.push_back(paseNodepathS);
            thisdemandPASElinS.push_back(paseLinPathS);
            thisdemandPNLIS.push_back(pnliPathS);
            thisdemandNoiseC.push_back(noisePathC);
            thisdemandNoiseL.push_back(noisePathL);
            thisdemandNoiseS.push_back(noisePathS);
            thisdemandDispersionC.push_back(dispersionPathC);
            thisdemandDispersionL.push_back(dispersionPathL);
        }
        alldemandsdistances.push_back(thisdemanddistances);
        alldemandsPASEnodeC.push_back(thisdemandPASEnodeC);
        alldemandsPASElinC.push_back(thisdemandPASElinC);
        alldemandsPNLIC.push_back(thisdemandPNLIC);
        alldemandsPASEnodeL.push_back(thisdemandPASEnodeL);
        alldemandsPASElinL.push_back(thisdemandPASElinL);
        alldemandsPNLIL.push_back(thisdemandPNLIL);
        alldemandsPASEnodeS.push_back(thisdemandPASEnodeS);
        alldemandsPASElinS.push_back(thisdemandPASElinS);
        alldemandsPNLIS.push_back(thisdemandPNLIS);
        alldemandsNoiseC.push_back(thisdemandNoiseC);
        alldemandsNoiseL.push_back(thisdemandNoiseL);
        alldemandsNoiseS.push_back(thisdemandNoiseS);
        alldemandsDispersionC.push_back(thisdemandDispersionC);
        alldemandsDispersionL.push_back(thisdemandDispersionL);
        thisdemanddistances.clear();
        thisdemandPASEnodeC.clear();
        thisdemandPNLIC.clear();
        thisdemandPASElinC.clear();
        thisdemandPASEnodeL.clear();
        thisdemandPNLIL.clear();
        thisdemandPASElinL.clear();
        thisdemandPASEnodeS.clear();
        thisdemandPNLIS.clear();
        thisdemandPASElinS.clear();
        thisdemandNoiseC.clear();
        thisdemandNoiseL.clear();
        thisdemandNoiseS.clear();
        thisdemandDispersionC.clear();
        thisdemandDispersionL.clear();
    }

    //Computing OSNR possible paths for all possible demands
    //PATH
    double distance;
    double dispersionC;
    double dispersionL;
    //OSNR
    double dbOsnrC;
    double dbOsnrL;
    //double dbOsnrS;
    std::ofstream outfile;
    bool printAllpaths = true;
    if(printAllpaths){
        std::cout << "Writing  OSNR's to file..." << std::endl;
        std::string outputOSNRName = "pathData.csv";
        outfile.open(outputOSNRName); 
        outfile << "Demand;"<<"TranspId;" <<"Path;" << "Distance;" << "dispersionC val;"<<"dispersionC lim;"<<"dispersionL val;"<<"dispersionL lim;"<< "osnrC val;"<<"osnrC lim;" << "osnrL val;"<<"osnrL lim"<< std::endl;
    }
    std::ofstream outfile2;
    bool printAuxDemand = false;
    if(printAuxDemand){
        std::cout << "Writing  paths as demands..." << std::endl;
        std::string outputDemandName = "demandAux.csv";
        outfile2.open(outputDemandName); 
        outfile2 << "index;origin;destination;slotsC;max_cdC;osnr_limitC;TranpCId;slotsL;max_cdL;osnr_limitL;TranspLId;Gbits/s;pchC;pchL;pchS"<< std::endl;
    }
    int total = 0;
    int bothC = 0;
    int osnrminC = 0;
    int maxCDC = 0;
    int noneC = 0;
    int bothL = 0;
    int maxCDL = 0;
    int osnrminL = 0;
    int noneL = 0;

    for (int i = 0 ; i <toBeRouted.size(); i++){	
        for (int j = 0; j< alldemandsdistances[i].size(); ++j){
            distance = alldemandsdistances[i][j];  
            dispersionC = alldemandsDispersionC[i][j];  
            dispersionL =  alldemandsDispersionL[i][j];  
            //dbOsnrC = osnrPathC(alldemandsPASElinC[i][j], alldemandsPASEnodeC[i][j], alldemandsPNLIC[i][j], toBeRouted[i].getPchC());
            //dbOsnrL = osnrPathL(alldemandsPASElinL[i][j], alldemandsPASEnodeL[i][j], alldemandsPNLIL[i][j], toBeRouted[i].getPchL());
            //dbOsnrS = osnrPathS(alldemandsPASElinS[i][j], alldemandsPASEnodeS[i][j], alldemandsPNLIS[i][j], toBeRouted[i].getPchS());
            dbOsnrC = osnrPath(alldemandsNoiseC[i][j], toBeRouted[i].getPchC());
            dbOsnrL = osnrPath(alldemandsNoiseL[i][j], toBeRouted[i].getPchL());
            //dbOsnrS = osnrPath(alldemandsNoiseS[i][j], toBeRouted[i].getPchS());
            if(printAllpaths){
                if(printAuxDemand){
                    outfile << total + 1;
                }
                else{
                    outfile << toBeRouted[i].getId() + 1;
                }
                outfile << ";";
                outfile << toBeRouted[i].getTranspIdC();
                outfile << ";";
                for (int k = 0; k <allpaths[i][j].size(); ++k)
                    outfile << getCompactNodeLabel(allpaths[i][j][k]) + 1 << "-";
                outfile << ";";
                outfile << distance;
                outfile << ";";
                outfile << dispersionC;
                outfile << ";";
                outfile << toBeRouted[i].getmaxCDC();
                outfile << ";";
                outfile << dispersionL;
                outfile << ";";
                outfile << toBeRouted[i].getmaxCDL();
                outfile << ";";
                outfile << dbOsnrC;
                outfile << ";";
                outfile << toBeRouted[i].getOsnrLimitC();
                outfile << ";";
                outfile << dbOsnrL;
                outfile << ";";
                outfile << toBeRouted[i].getOsnrLimitL();
                outfile << ";";
                outfile << "\n";
            }
            if(printAuxDemand){
                outfile2 << total + 1;
                outfile2 << ";";
                outfile2 << toBeRouted[i].getSource()+1;
                outfile2 << ";";
                outfile2 << toBeRouted[i].getTarget()+1;
                outfile2 << ";";
                outfile2 << toBeRouted[i].getLoadC();
                outfile2 << ";";
                outfile2 << toBeRouted[i].getmaxCDC();
                outfile2 << ";";
                outfile2 << toBeRouted[i].getOsnrLimitC();
                outfile2 << ";";
                outfile2 << toBeRouted[i].getTranspIdC();
                outfile2 << ";";
                outfile2 << toBeRouted[i].getLoadL();
                outfile2 << ";";
                outfile2 << toBeRouted[i].getmaxCDL();
                outfile2 << ";";
                outfile2 << toBeRouted[i].getOsnrLimitL();
                outfile2 << ";";
                outfile2 << toBeRouted[i].getTranspIdL();
                outfile2 << ";";
                outfile2 << toBeRouted[i].getGBits();
                outfile2 << ";";
                outfile2 << toBeRouted[i].getPchC();
                outfile2 << ";";
                outfile2 << toBeRouted[i].getPchL();
                outfile2 << ";";
                outfile2 << toBeRouted[i].getPchS();
                outfile2 << ";";
                outfile2 << "\n";
            }
            if (dispersionC <= (toBeRouted[i].getmaxCDC()) && dbOsnrC >= toBeRouted[i].getOsnrLimitC()){
                bothC = bothC +1;
            }
            if (dispersionC <= (toBeRouted[i].getmaxCDC()) && dbOsnrC < toBeRouted[i].getOsnrLimitC()){
                maxCDC = maxCDC +1;
            }
            if (dispersionC > (toBeRouted[i].getmaxCDC()) && dbOsnrC >= toBeRouted[i].getOsnrLimitC()){
                osnrminC = osnrminC +1;
            }
            if (dispersionC > (toBeRouted[i].getmaxCDC())&& dbOsnrC < toBeRouted[i].getOsnrLimitC()){
                noneC = noneC +1;
            }
            if (dispersionL <= (toBeRouted[i].getmaxCDL()) && dbOsnrL >= toBeRouted[i].getOsnrLimitL()){
                bothL = bothL +1;
            }
            if (dispersionL <= (toBeRouted[i].getmaxCDL()) && dbOsnrL < toBeRouted[i].getOsnrLimitL()){
                maxCDL = maxCDL +1;
            }
            if (dispersionL > (toBeRouted[i].getmaxCDL()) && dbOsnrL >= toBeRouted[i].getOsnrLimitL()){
                osnrminL = osnrminL +1;
            }
            if (dispersionL > (toBeRouted[i].getmaxCDL()) && dbOsnrL < toBeRouted[i].getOsnrLimitL()){
                noneL = noneL +1;
            }
            total = total + 1;
        }
    }
    possiblePaths = total;
    feasiblePathsC = bothC;
    onlyOsnrFeasiblePathsC = osnrminC;
    onlyReachFeasiblePathsC = maxCDC;
    infeasiblePathsC = noneC + osnrminC + maxCDC;
    feasiblePathsL = bothL;
    onlyOsnrFeasiblePathsL = osnrminL;
    onlyReachFeasiblePathsL = maxCDL;
    infeasiblePathsL = noneL + osnrminL + maxCDL;
           
}

/** Returns the total number of loads to be routed. **/
int RSA::getTotalLoadsToBeRouted() const{ 
    int total = 0;
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        if(getToBeRouted_k(d).getLoadC() <= getToBeRouted_k(d).getLoadL()){
            total += getToBeRouted_k(d).getLoadL();
        }else{      
            total += getToBeRouted_k(d).getLoadC();
        }
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
            compactEdgePnliC[e] = edge.getPnliC();
            compactEdgePaseLineC[e] = edge.getPaseLineC();
            compactEdgePnliL[e] = edge.getPnliL();
            compactEdgePaseLineL[e] = edge.getPaseLineL();
            compactEdgeNoiseC[e] = edge.getNoiseC();
            compactEdgeNoiseL[e] = edge.getNoiseL();
            compactEdgeDispersionC[e] = edge.getDispersionCoeffC()*edge.getLength();
            compactEdgeDispersionL[e] = edge.getDispersionCoeffL()*edge.getLength();
            
        }
    }
}

/* Creates an arc -- and its nodes if necessary -- between nodes (source,slice) and (target,slice) on a graph. */
//void RSA::addArcs(int d, int linkSourceLabel, int linkTargetLabel, int linkLabel, int slice, double l){
void RSA::addArcs(int d, int linkSourceLabel, int linkTargetLabel, int linkLabel, int slice, double l, int la, double pnc, double pac, double nc, double pnl, double pal, double nl, double dc, double dl ){
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
    setArcPnliC(a, d, pnc);
    setArcPaseLineC(a, d, pac);
    //std::cout << "a: " << pnc+pac << "b: " <<  pnc+pac+instance.getPaseNodeC() <<std::endl;
    setArcNoiseC(a,d,nc);
    setArcPnliL(a, d, pnl);
    setArcPaseLineL(a, d, pal);
    setArcNoiseL(a,d,nl);
    setArcDispersionC(a,d,dc);
    setArcDispersionL(a,d,dl);
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
    int nbLow = 0;
    int nbInTarget = 0;
    int nbinSource= 0;
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

        if (uLabel == demandTarget){
            (*vecGraph[d]).erase(a);
            nb++;
            nbInTarget++;
            variablesSetTo0++;
        }
        else if (vLabel == demandSource){
            (*vecGraph[d]).erase(a);
            nb++;
            nbinSource++;
            variablesSetTo0++;
        }
        else if (instance.hasEnoughSpace(label, slice, getToBeRouted_k(d)) == false){
            (*vecGraph[d]).erase(a);
            nb++;
            nbLow++;
            variablesSetTo0++;
        }
        a = nextArc;
    }
    //std::cout << "> Number of non-routable arcs erased on graph #" << d <<  " by min load: " << nbLow << std::endl;
    //std::cout << "> Number of non-routable arcs erased on graph #" << d << " by in target: " << nbInTarget << std::endl; 
    //std::cout << "> Number of non-routable arcs erased on graph #" << d << " by in source: " << nbinSource << std::endl;  
    //std::cout << "> Number of non-routable arcs erased on graph #" << d << " : " << vtnc << std::endl; 
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
    std::cout << "> Non routable arcs: " << variablesSetTo0 << std::endl; 
    if (getInstance().getInput().getChosenPreprLvl() >= Input::PREPROCESSING_LVL_PARTIAL){
        // do partial preprocessing;
        pathExistencePreprocessing();
        
        //Tflow test
        preProcessingErasedArcs.resize(getNbDemandsToBeRouted());
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            preProcessingErasedArcs[d].resize(2*countEdges(compactGraph));
            for (int i=0; i<2*countEdges(compactGraph);i++){
                preProcessingErasedArcs[d][i].resize(2);
                preProcessingErasedArcs[d][i][0] = 0;
                preProcessingErasedArcs[d][i][1] = 0;
            }
        }
        //Tflow test
        bool keepPreprocessing;
        if (getInstance().getInput().isMaxCDEnabled() == true)
            keepPreprocessing = CDPreprocessing();
        if (getInstance().getInput().isMinOSNREnabled() == true)
            keepPreprocessing = OSNRPreprocessing();
            
        if (getInstance().getInput().getChosenPreprLvl() >= Input::PREPROCESSING_LVL_FULL){
            // do full preprocessing;
            if (getInstance().getInput().isMaxCDEnabled() == true){
                while (keepPreprocessing){
                    pathExistencePreprocessing(); 
                    keepPreprocessing = CDPreprocessing();
                }
            }
            if (getInstance().getInput().isMinOSNREnabled() == true){
                while (keepPreprocessing){
                    pathExistencePreprocessing(); 
                    keepPreprocessing = OSNRPreprocessing();
                }
            }           
        }
        /*
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            for (int i=0; i<2*countEdges(compactGraph);i++){
                std::cout << d+1 <<  ";"<< i <<  ";" <<  preProcessingErasedArcs[d][i][0] << ";" << preProcessingErasedArcs[d][i][1]<< std::endl;
            }
        }
        */
    }
    
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        contractNodesFromLabel(d, getToBeRouted()[d].getSource());
        contractNodesFromLabel(d, getToBeRouted()[d].getTarget());
    }

    //for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        //std::cout << "> Number of arcs in graph #" << d << " before preprocessing: " << nbArcsOld[d] << ". After: " << countArcs((*vecGraph[d])) << std::endl;
    //}
    std::cout << "> Variables set to 0: " << variablesSetTo0 << std::endl; 
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
bool RSA::CDPreprocessing(){
    std::cout << "Called CD preprocessing."<< std::endl;
    //Tflow test
    int nbEdges = countEdges(compactGraph);
    int totalNbTflow = 0;
    //Tflow test
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
            ListDigraph::ArcIt nextArc(*vecGraph[d], ++currentArc);
            //currentArc = a;
            int slice = getArcSlice(a, d);
            int limitCband = instance.getPhysicalLinkFromIndex(getArcLabel(a, d)).getNbSlicesC();
            double distanceToConsider = getToBeRouted_k(d).getmaxCDC();
            if (slice >= limitCband){
                distanceToConsider = getToBeRouted_k(d).getmaxCDL();
            }
            ListDigraph::Node source = getNode(d, getToBeRouted_k(d).getSource(), slice);
            ListDigraph::Node target = getNode(d, getToBeRouted_k(d).getTarget(), slice);
            if (source != INVALID && target != INVALID){
                if (shortestCD(d, source, a, target) > distanceToConsider + DBL_EPSILON){
                    //Tflow test
                    int arcTflowId = getArcLabel(a,d);
                    int u = getNodeLabel((*vecGraph[d]).source(a), d) + 1;
                    int v = getNodeLabel((*vecGraph[d]).target(a), d) + 1;
                    if (u < v){
                        if (slice >= limitCband){
                            if (preProcessingErasedArcs[d][arcTflowId][1]==0){
                                preProcessingErasedArcs[d][arcTflowId][1]++;
                                totalNbTflow++;
                                //std::cout << "Erasing (" << u << "," << v << ") in L band for demand: " << d << std::endl;
                                //std::cout << d+1 <<  ";"<< arcTflowId <<  ";2" << std::endl;
                            }
                        }else{
                            if (preProcessingErasedArcs[d][arcTflowId][0]==0){
                                preProcessingErasedArcs[d][arcTflowId][0]++;
                                totalNbTflow++;
                                //std::cout << d+1 <<  ";"<< arcTflowId <<  ";1" << std::endl;
                                //std::cout << "Erasing (" << u << "," << v << ") in C band for demand: " << d << std::endl;
                            }
                        }
                    }
                    else{
                        if (slice >= limitCband){
                            if (preProcessingErasedArcs[d][arcTflowId+nbEdges][1]==0){
                                preProcessingErasedArcs[d][arcTflowId+nbEdges][1]++;
                                totalNbTflow++;
                                //std::cout << "Erasing (" << u << "," << v << ") in L band for demand: " << d << std::endl;
                                //std::cout << d+1 <<  ";"<< arcTflowId+nbEdges <<  ";2" << std::endl;
                            }
                        }else{
                            if (preProcessingErasedArcs[d][arcTflowId+nbEdges][0]==0){
                                preProcessingErasedArcs[d][arcTflowId+nbEdges][0]++;
                                totalNbTflow++;
                                //std::cout << "Erasing (" << u << "," << v << ") in C band for demand: " << d << std::endl;
                                //std::cout << d+1 <<  ";"<< arcTflowId+nbEdges <<  ";1" << std::endl;
                            }
                        }                        
                    }
                    //Tflow test
                    //displayArc(d, a);
                    (*vecGraph[d]).erase(a);
                    nb++;
                    totalNb++;
                    variablesSetTo0++;
                }
            }
            else{
                (*vecGraph[d]).erase(a);
                nbElse++;
                totalNb++;
                variablesSetTo0++;
            }
            a = nextArc;
        }
        //std::cout << "> Number of erased arcs due to length in graph #" << d << ". If: " << nb << ". Else: " << nbElse << std::endl;
    }
    /*
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (int i=0; i<2*nbEdges;i++){
            std::cout << preProcessingErasedArcs[d][i] << " ";
        }
        std::cout << std::endl;
    }
    */
    if (totalNb >= 1){
        std::cout << "> Number of erased arcs due to CD in graph: "<< totalNb << std::endl;
        if (nbBands == 1){
            std::cout << "> Number of erased arcs in tflow: "<< totalNbTflow << std::endl;
        }
        return true;
    }
    return false;
}

/* Returns the distance of the shortest path from source to target passing through arc a. */
double RSA::shortestCD(int d, ListDigraph::Node &s, ListDigraph::Arc &a, ListDigraph::Node &t){
    double cd = 0.0;

    int slice = getArcSlice(a, d);
    int limitCband = instance.getPhysicalLinkFromIndex(getArcLabel(a, d)).getNbSlicesC();
    if (slice < limitCband){
        Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > s_u_path((*vecGraph[d]), (*vecArcDispersionC[d]));
        s_u_path.run(s,(*vecGraph[d]).source(a));
        if (s_u_path.reached((*vecGraph[d]).source(a))){
            cd += s_u_path.dist((*vecGraph[d]).source(a));
        }
        else{
            return DBL_MAX;
        }
        cd += getArcDispersionC(a, d);
        Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > v_t_path((*vecGraph[d]), (*vecArcDispersionC[d]));
        v_t_path.run((*vecGraph[d]).target(a), t);
        if (v_t_path.reached(t)){
            cd += v_t_path.dist(t);
        }
        else{
            return DBL_MAX;
        }
        return cd;
    }else{
        Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > s_u_path((*vecGraph[d]), (*vecArcDispersionL[d]));
        s_u_path.run(s,(*vecGraph[d]).source(a));
        if (s_u_path.reached((*vecGraph[d]).source(a))){
            cd += s_u_path.dist((*vecGraph[d]).source(a));
        }
        else{
            return DBL_MAX;
        }
        cd += getArcDispersionL(a, d);
        Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > v_t_path((*vecGraph[d]), (*vecArcDispersionL[d]));
        v_t_path.run((*vecGraph[d]).target(a), t);
        if (v_t_path.reached(t)){
            cd += v_t_path.dist(t);
        }
        else{
            return DBL_MAX;
        }
        return cd;
    }
    
}
/* Performs preprocessing based on the arc partial osnr and returns true if at least one arc is erased. */
bool RSA::OSNRPreprocessing(){
    std::cout << "Called OSNR preprocessing."<< std::endl;
    //Tflow test
    int nbEdges = countEdges(compactGraph);
    int totalNbTflow = 0;
    //Tflow test

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
            int limitCband = instance.getPhysicalLinkFromIndex(getArcLabel(a, d)).getNbSlicesC();
            double osnrRhs;
            double osnrLimdb = getToBeRouted_k(d).getOsnrLimitC();
            double pch = getToBeRouted_k(d).getPchC();
            //double lastAmpNoise = instance.getPaseNodeC();    //RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
            double shortestOSNR = shortestOSNRPartial(d, source, a, target);
            if (slice >= limitCband){
                osnrLimdb = getToBeRouted_k(d).getOsnrLimitL();
                pch = getToBeRouted_k(d).getPchL();
                //lastAmpNoise = instance.getPaseNodeL();       //RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
            }
            double osnrLim = pow(10,osnrLimdb/10);
            osnrRhs = pch/osnrLim; //- lastAmpNoise;               //RETIRANDO AMPLI. O AMPLI DO NO DE ORIGEM DO LINK JA ESTA INCLUINDO NO NOISE DO LINK
            if (source != INVALID && target != INVALID){
                //std::cout << "rhs: " << osnrRhs << " lhs:" << shortestOSNRCPartial(d, source, a, target) <<std::endl;
                if (shortestOSNR > osnrRhs + DBL_EPSILON){
                    //std::cout << "demand " <<  d << std::endl;
                    //std::cout <<"lhs " << shortestOSNRCPartial(d, source, a, target) <<std::endl;
                    //std::cout <<"rhs " << osnrRhs + DBL_EPSILON <<std:: endl;
                    //displayArc(d, a);
                    //std::cout << "length  " <<   getArcLength(a, d) << " penalties " << getArcLengthWithPenalties(a,d) << std::endl;
                    //Tflow test
                    int arcTflowId = getArcLabel(a,d);
                    int u = getNodeLabel((*vecGraph[d]).source(a), d) + 1;
                    int v = getNodeLabel((*vecGraph[d]).target(a), d) + 1;
                    if (u < v){
                        if (slice >= limitCband){
                            if (preProcessingErasedArcs[d][arcTflowId][1]==0){
                                preProcessingErasedArcs[d][arcTflowId][1]++;
                                totalNbTflow++;
                                //std::cout << "Erasing (" << u << "," << v << ") in L band for demand: " << d << std::endl;
                                //std::cout << d+1 <<  ";"<< arcTflowId <<  ";2" << std::endl;
                            }
                        }else{
                            if (preProcessingErasedArcs[d][arcTflowId][0]==0){
                                preProcessingErasedArcs[d][arcTflowId][0]++;
                                totalNbTflow++;
                                //std::cout << "Erasing (" << u << "," << v << ") in C band for demand: " << d << std::endl;
                                //std::cout << d+1 <<  ";"<< arcTflowId <<  ";1" << std::endl;
                            }
                        }
                    }
                    else{
                        if (slice >= limitCband){
                            if (preProcessingErasedArcs[d][arcTflowId+nbEdges][1]==0){
                                preProcessingErasedArcs[d][arcTflowId+nbEdges][1]++;
                                totalNbTflow++;
                                //std::cout << "Erasing (" << u << "," << v << ") in L band for demand: " << d << std::endl;
                                //std::cout << d+1 <<  ";"<< arcTflowId+nbEdges <<  ";2" << std::endl;
                            }
                        }else{
                            if (preProcessingErasedArcs[d][arcTflowId+nbEdges][0]==0){
                                preProcessingErasedArcs[d][arcTflowId+nbEdges][0]++;
                                totalNbTflow++;
                                //std::cout << "Erasing (" << u << "," << v << ") in C band for demand: " << d << std::endl;
                                //std::cout << d+1 <<  ";"<< arcTflowId+nbEdges <<  ";1" << std::endl;
                            }
                        }                
                    }
                    //Tflow test
                    (*vecGraph[d]).erase(a);
                    nb++;
                    totalNb++;
                    variablesSetTo0++;
                }
            }
            else{
                (*vecGraph[d]).erase(a);
                nbElse++;
                totalNb++;
                variablesSetTo0++;
            }
            a = nextArc;
            //std::cout << "next arc" << std::endl;
            //displayArc(d, a);
        }
        //std::cout << "> Number of erased arcs due to length in graph #" << d << ". If: " << nb << ". Else: " << nbElse << std::endl;
    }
    /*    
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        for (int i=0; i<2*nbEdges;i++){
            std::cout << preProcessingErasedArcs[d][i] << " ";
        }
        std::cout << std::endl;
    }*/
     
    if (totalNb >= 1){
        std::cout << "> Number of erased arcs due to OSNR in graph: "<< totalNb << std::endl;
        if (nbBands == 1){
            std::cout << "> Number of erased arcs in tflow: "<< totalNbTflow << std::endl;
        }
        return true;
    }
    return false;
    
}

/* Returns the partial osnr of the shortest path from source to target passing through arc a in the C band. */
double RSA::shortestOSNRPartial(int d, ListDigraph::Node &s, ListDigraph::Arc &a, ListDigraph::Node &t){
    double OSNRPartial = 0.0;

    int limitCband = instance.getPhysicalLinkFromIndex(getArcLabel(a, d)).getNbSlicesC();
    int slice = getArcSlice(a, d);
    if (slice < limitCband){
        Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > s_u_path((*vecGraph[d]), (*vecArcNoiseC[d]));
        s_u_path.run(s,(*vecGraph[d]).source(a));
        if (s_u_path.reached((*vecGraph[d]).source(a))){
            OSNRPartial += s_u_path.dist((*vecGraph[d]).source(a));
        }else{
            return DBL_MAX;
        }
        OSNRPartial += getArcNoiseC(a, d);
        Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > v_t_path((*vecGraph[d]), (*vecArcNoiseC[d]));
        v_t_path.run((*vecGraph[d]).target(a), t);
        if (v_t_path.reached(t)){
            OSNRPartial += v_t_path.dist(t);
        }else{
            return DBL_MAX;
        }
    }else{
        Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > s_u_path((*vecGraph[d]), (*vecArcNoiseL[d]));
        s_u_path.run(s,(*vecGraph[d]).source(a));
        if (s_u_path.reached((*vecGraph[d]).source(a))){
            OSNRPartial += s_u_path.dist((*vecGraph[d]).source(a));
        }else{
            return DBL_MAX;
        }       
        OSNRPartial += getArcNoiseL(a, d); 
        Dijkstra< ListDigraph, ListDigraph::ArcMap<double> > v_t_path((*vecGraph[d]), (*vecArcNoiseL[d]));
        v_t_path.run((*vecGraph[d]).target(a), t);
        if (v_t_path.reached(t)){
            OSNRPartial += v_t_path.dist(t);
        }
        else{
            return DBL_MAX;
        }
    }
    return OSNRPartial;
}

/* Returns the coefficient of an arc according to metric 1 on graph #d. */
double RSA::getCoeffObj1(const ListDigraph::Arc &a, int d){
    double coeff = 0.0;

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
                if(getToBeRouted_k(d).getLoadC() <= instance.getInput().getPartitionLoad()){
                    coeff = arcSlice + 1; 
                }
                else{
                    coeff = (instance.getPhysicalLinkFromIndex(arcLabel).getNbSlices() - arcSlice);
                }
            break;
            case Input::PARTITION_POLICY_HARD:
                if(getToBeRouted_k(d).getLoadC() <= instance.getInput().getPartitionLoad()){
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
double RSA::getCoeffObjSLUS(const ListDigraph::Arc &a, int d){
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
double RSA::getCoeffObjSULD(const ListDigraph::Arc &a, int d){
    double coeff = 1.0;
    return coeff;
}

/* Returns the coefficient of an arc according to metric 2p on graph #d. */
double RSA::getCoeffObjTUS(const ListDigraph::Arc &a, int d){
    double coeff = getToBeRouted_k(d).getLoadC();
    int limitCband = instance.getPhysicalLinkFromIndex(getArcLabel(a, d)).getNbSlicesC();
    int slice = getArcSlice(a, d);
    if (slice >= limitCband){
        coeff = getToBeRouted_k(d).getLoadL();
    }
    return coeff;
}

/* Returns the coefficient of an arc according to metric 4 on graph #d. */
double RSA::getCoeffObjTRL(const ListDigraph::Arc &a, int d){
    return getArcLength(a, d);
}

/* Returns the coefficient of an arc according to metric 4p on graph #d. */
double RSA::getCoeffObjTASE(const ListDigraph::Arc &a, int d){
    return  (getArcLineAmplifiers(a, d)+1) * (getToBeRouted_k(d).getGBits()/getToBeRouted_k(d).getLoadC());
}

/* Returns the coefficient of an arc according to metric 8 on graph #d. */
double RSA::getCoeffObjNLUS(const ListDigraph::Arc &a, int d){
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

/* Returns the coefficient of an arc according to metric 10 on graph #d. */
double RSA::getCoeffObjTOS(const ListDigraph::Arc &a, int d){
    int limitCband = instance.getPhysicalLinkFromIndex(getArcLabel(a, d)).getNbSlicesC();
    int slice = getArcSlice(a, d);
    if (slice < limitCband){
        double pnli = ceil(getArcPnliC(a,d)* pow(10,8)*100)/100; //ROUNDING
        double paseLine = ceil(getArcPaseLineC(a,d)* pow(10,8)*100)/100; //ROUNDING
        double paseNode = ceil(instance.getPaseNodeC() * pow(10,8)*100)/100; //ROUNDING
        return -((pnli+paseLine+paseNode)/getToBeRouted_k(d).getLoadC());
    }else{
        double pnli = ceil(getArcPnliL(a,d)* pow(10,8)*100)/100; //ROUNDING
        double paseLine = ceil(getArcPaseLineL(a,d)* pow(10,8)*100)/100; //ROUNDING
        double paseNode = ceil(instance.getPaseNodeL() * pow(10,8)*100)/100; //ROUNDING
        return -((pnli+paseLine+paseNode)/getToBeRouted_k(d).getLoadL());        
    }
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
        case Input::OBJECTIVE_METRIC_SLUS:
        {
            coeff = getCoeffObjSLUS(a, d);
            break;
        }
        case Input::OBJECTIVE_METRIC_SULD:
        {
            coeff = getCoeffObjSULD(a, d);
            break;
        }
        case Input::OBJECTIVE_METRIC_TUS:
        {
            coeff = getCoeffObjTUS(a, d);
            break;
        }
        case Input::OBJECTIVE_METRIC_TRL:
        {
            coeff = getCoeffObjTRL(a, d);
            break;
        }
        case Input::OBJECTIVE_METRIC_TASE:
        {
            coeff = getCoeffObjTASE(a, d);
            break;
        }
        case Input::OBJECTIVE_METRIC_NLUS:
        {
            coeff = getCoeffObjNLUS(a, d);
            break;
        }
        case Input::OBJECTIVE_METRIC_TOS:
        {
            coeff = getCoeffObjTOS(a, d);
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
        std::cout << "requiring " << toBeRouted[i].getLoadC() << " slices and having Max CD ";
        std::cout << toBeRouted[i].getmaxCDC() << std::endl;
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

/* Displays the paths found for each of the new routed demands. */
void RSA::displayOSNR(Instance &i){
    if(this->getInstance().getInput().isMinOSNREnabled() == true){
        double osnrSurplus = 0.0;
        std::cout << "Displaying OSNR " << std::endl;
        for (int d = 0; d < getNbDemandsToBeRouted(); d++){
            if(i.getTabDemand()[d].isRouted()){
                double osnrDenominator = 0.0;
                double osnrNumerator = 0.0;
                double osnr = 0.0;
                double osnrDb = 0.0;
                bool auxCband = true;
                for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                    if ((*vecOnPath[d])[a] != -1){
                        //std::cout <<"edge: "<<  instance.getPhysicalLinkFromIndex(getArcLabel(a,d)).getSource()+1 << "-"<<instance.getPhysicalLinkFromIndex(getArcLabel(a,d)).getTarget()+1 << " slot: "<< getArcSlice(a,d)+1 << "demand: "<< d+1<<  std::endl;
                        int limitCband = instance.getPhysicalLinkFromIndex(getArcLabel(a, d)).getNbSlicesC();
                        int slice = getArcSlice(a, d);
                        double noise = 0.0;

                        if (slice < limitCband){
                            noise = ceil(getArcNoiseC(a,d)* pow(10,8)*100)/100; //ROUNDING
                        }else{
                            noise = ceil(getArcNoiseL(a,d)* pow(10,8)*100)/100 ; //ROUNDING 
                            auxCband = false;                    
                        }
                        osnrDenominator += noise;
                    }
                }
                //std::cout <<"NOISE: "<< osnrDenominator << std::endl;
                if (auxCband == true){
                    osnrDenominator += ceil(instance.getPaseNodeC()* pow(10,8)*100)/100;
                    osnrNumerator += ceil(getToBeRouted_k(d).getPchC()* pow(10,8)*100)/100;
                    osnr = osnrNumerator/osnrDenominator;
                    osnrDb = 10*log10(osnr);
                    if (osnrDb<getToBeRouted_k(d).getOsnrLimitC()){
                        std::cout << "ERROR: OSNR below limit" << std::endl;
                    }
                    std::cout << "For demand " << getToBeRouted_k(d).getId() + 1 << " : " << osnrDb << " x " << getToBeRouted_k(d).getOsnrLimitC() << " limit" << std::endl;
                    osnrSurplus += osnrDb - getToBeRouted_k(d).getOsnrLimitC();
                }else{
                    osnrDenominator += ceil(instance.getPaseNodeL()* pow(10,8)*100)/100;
                    osnrNumerator += ceil(getToBeRouted_k(d).getPchL()* pow(10,8)*100)/100;
                    osnr = osnrNumerator/osnrDenominator;
                    osnrDb = 10*log10(osnr);
                    if (osnrDb<getToBeRouted_k(d).getOsnrLimitL()){
                        std::cout << "ERROR: OSNR below limit" << std::endl;
                    }
                    std::cout << "For demand " << getToBeRouted_k(d).getId() + 1 << " : " << osnrDb << " x " << getToBeRouted_k(d).getOsnrLimitL() << " limit" << std::endl;
                    osnrSurplus += osnrDb - getToBeRouted_k(d).getOsnrLimitL();                    
                }
            }
        }
        std::cout << "Extra OSNR in routing: " << osnrSurplus << std::endl;
    }
}

/* Displays the paths found for each of the new routed demands. */
void RSA::displayOFData(Instance &i){
    std::cout << "Displaying data for multiple metrics " << std::endl;
    int OBJECTIVE_METRIC_SLUS = 0;	/**< Minimize the sum of (max used slice positions) over edges. **/                     //OK
    int OBJECTIVE_METRIC_SULD = 0;		/**< Minimize the sum of (number of hops in paths) over demands. **/                //OK
    int OBJECTIVE_METRIC_TUS = 0;	/**< Minimize the sum of occupied slices. **/                                           //OK
    double OBJECTIVE_METRIC_TRL = 0;		/**< Minimize the path lengths. **/                                                 //OK
    double OBJECTIVE_METRIC_TASE = 0;	/**< Minimize the amplifiers. **/                                                       //OK 
    int OBJECTIVE_METRIC_NLUS = 0;		/**< Minimize the max used slice position overall. **/                              //OK 
    double OBJECTIVE_METRIC_TOS = 0.0; 	/**< Minimize the sum of differences between the OSNR of a path ant the OSNR limit **/
    int OBJECTIVE_METRIC_ADS = 0; 	/**< Minimize the weighted sum of rejected demands **/                                  //OK
    int OBJECTIVE_METRIC_LLB = 0; 	/**< Minimize the number of links with installed L band **/                             //OK
    int OBJECTIVE_METRIC_DCB = 0; 	/**< Maximize the number of demands in the c band **/                                   //OK
    for (int d = 0; d < getNbDemandsToBeRouted(); d++){
        if(i.getTabDemand()[d].isRouted()){
            OBJECTIVE_METRIC_ADS = OBJECTIVE_METRIC_ADS +1;
            bool bandComputed = false;
            for (ListDigraph::ArcIt a(*vecGraph[d]); a != INVALID; ++a){
                if ((*vecOnPath[d])[a] != -1){
                    OBJECTIVE_METRIC_SULD = OBJECTIVE_METRIC_SULD +1;
                    OBJECTIVE_METRIC_TRL = OBJECTIVE_METRIC_TRL + getArcLength(a,d);
                    OBJECTIVE_METRIC_TASE = OBJECTIVE_METRIC_TASE + getCoeffObjTASE(a, d);
                    int slice = getArcSlice(a, d);
                    int limitCband = instance.getPhysicalLinkFromIndex(getArcLabel(a, d)).getNbSlicesC();
                    if (slice < limitCband){
                        OBJECTIVE_METRIC_TUS = OBJECTIVE_METRIC_TUS + getToBeRouted_k(d).getLoadC();
                        if (bandComputed == false){
                            OBJECTIVE_METRIC_DCB = OBJECTIVE_METRIC_DCB + 1;
                            bandComputed = true;
                        }
                    }else{
                        OBJECTIVE_METRIC_TUS = OBJECTIVE_METRIC_TUS + getToBeRouted_k(d).getLoadL();                   
                    }
                }
            }
        }
    }
    for (int e = 0; e < i.getNbEdges(); e++){
        
        if(i.getPhysicalLinkFromIndex(e).getInstalledBands() == 2){
            OBJECTIVE_METRIC_LLB = OBJECTIVE_METRIC_LLB +1;
        }
        if(i.getPhysicalLinkFromIndex(e).getMaxUsedSlicePosition()+1 > OBJECTIVE_METRIC_NLUS){
            OBJECTIVE_METRIC_NLUS = i.getPhysicalLinkFromIndex(e).getMaxUsedSlicePosition()+1;
        }
        OBJECTIVE_METRIC_SLUS = OBJECTIVE_METRIC_SLUS+i.getPhysicalLinkFromIndex(e).getMaxUsedSlicePosition()+1;
    }
    std::cout << "Accepted demand set: " << OBJECTIVE_METRIC_ADS  << std::endl;
    std::cout << "Demands in C band: " << OBJECTIVE_METRIC_DCB  << std::endl;
    std::cout << "Links with L band: " << OBJECTIVE_METRIC_LLB  << std::endl;  
    std::cout << "Network last used slot: " << OBJECTIVE_METRIC_NLUS  << std::endl;
    std::cout << "Sum of last used slots: " << OBJECTIVE_METRIC_SLUS << std::endl;
    std::cout << "Sum of used links by demands: " << OBJECTIVE_METRIC_SULD << std::endl;
    std::cout << "Total route length: " << OBJECTIVE_METRIC_TRL << std::endl;
    std::cout << "Total used slots: " << OBJECTIVE_METRIC_TUS << std::endl;
    std::cout << "Total used amplifiers: " << OBJECTIVE_METRIC_TASE << std::endl;

    ADS = OBJECTIVE_METRIC_ADS;
    DCB = OBJECTIVE_METRIC_DCB ;
    LLB = OBJECTIVE_METRIC_LLB ;  
    NLUS = OBJECTIVE_METRIC_NLUS ;
    SLUS = OBJECTIVE_METRIC_SLUS;
    SULD = OBJECTIVE_METRIC_SULD;
    TRL = OBJECTIVE_METRIC_TRL;
    TUS = OBJECTIVE_METRIC_TUS;
    TASE = OBJECTIVE_METRIC_TASE;

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
    vecArcPnliC.clear();
    vecArcPaseLineC.clear();
    vecArcNoiseC.clear();
    vecArcPnliL.clear();
    vecArcPaseLineL.clear();
    vecArcNoiseL.clear();
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
