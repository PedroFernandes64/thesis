#include "genericCallback.h"

GenericCallback::GenericCallback(const IloBoolVarMatrix &_x, Instance &i, ListGraph &g, std::vector<Demand> &toBeRouted,
         ListGraph::NodeMap<int> &nodeLabelMap, 
         ListGraph::NodeMap<int> &nodeIdMap, 
         ListGraph::EdgeMap<int> &edgeLabelMap, 
         ListGraph::EdgeMap<int> &edgeIdMap): x(_x){ 

    instance = &i;
    graph = & g;
    nodeLabel = & nodeLabelMap;
    nodeId = & nodeIdMap;
    edgeLabel = & edgeLabelMap;
    edgeId = & edgeIdMap;
    demands = & toBeRouted;
}

void GenericCallback::invoke (const IloCplex::Callback::Context &context){
    if ( context.inRelaxation() ) {
        addUserCuts(context);
    }

    if ( context.inCandidate() ){
        addLazyConstraints(context);
    }
}

void GenericCallback::addUserCuts(const IloCplex::Callback::Context &context) const{
    for(unsigned int d = 0; d < demands->size(); d++){
        int origin = getDemand_k(d).getSource();
        int destination = getDemand_k(d).getTarget();
        ListGraph::Node SOURCE = getNodeFromLabel(origin);
        ListGraph::Node TARGET = getNodeFromLabel(destination);
        
        //std::cout << "Checking path of demand " << d << ". From " << origin+1 << " to " << destination+1 << std::endl;
        //displaySolution_d(context, d);
        ListGraph::EdgeMap<double> capacityMap(*graph, 0.0);
        for (ListGraph::EdgeIt e(*graph); e != INVALID; ++e){
            int edge = getEdgeLabel(e);
            capacityMap[e] = context.getRelaxationPoint(x[edge][d]);
        }
        Preflow< ListGraph, ListGraph::EdgeMap<double> > maxFlow((*graph), capacityMap, SOURCE, TARGET);
        maxFlow.runMinCut();

        IloExpr expr(context.getEnv());
        for (ListGraph::EdgeIt e(*graph); e != INVALID; ++e){
            if (maxFlow.minCut(graph->u(e)) != maxFlow.minCut(graph->v(e))){
                int edge = getEdgeLabel(e);
                expr += x[edge][d];
            }  
        }

        if (context.getRelaxationValue(expr) <= 1 - EPS){
            //std::cout << "Adding user cut: " << expr << " >= 1" << std::endl;
            context.addUserCut( expr >= 1, IloCplex::UseCutPurge, IloFalse);
        }
    }
}

void GenericCallback::addLazyConstraints(const IloCplex::Callback::Context &context) const{
    std::cout << "Entering callback." << std::endl;
    if ( !context.isCandidatePoint() ){
        throw IloCplex::Exception(-1, "Unbounded solution");
    }
    for(unsigned int d = 0; d < demands->size(); d++){
        int origin = getDemand_k(d).getSource();
        int destination = getDemand_k(d).getTarget();
        ListGraph::Node SOURCE = getNodeFromLabel(origin);
        ListGraph::Node TARGET = getNodeFromLabel(destination);
        //std::cout << "Checking path of demand " << d << ". From " << origin+1 << " to " << destination+1 << std::endl;
        
        //displaySolution_d(context, d);
        if (TARGET == INVALID || SOURCE == INVALID){
            throw IloCplex::Exception(-1, "Could not find source or target from demand inside generic callback.");
        }
        
        ListGraph::Node currentNode = SOURCE;
        std::vector<int> setOfNodes;
        //setOfNodes.push_back(origin);
        bool cutFound = false;
        int previousNodeLabel = -1;
        while (currentNode != TARGET && !cutFound){
            //std::cout << "Add Node: " << (*nodeLabel)[currentNode]+1 << std::endl;
            setOfNodes.push_back(getNodeLabel(currentNode));
            ListGraph::Edge nextEdge = INVALID;
            for (ListGraph::IncEdgeIt e(*graph, currentNode); e != INVALID; ++e){
                int edge = getEdgeLabel(e);
                if (getNodeLabel(graph->u(e)) != previousNodeLabel && getNodeLabel(graph->v(e)) != previousNodeLabel){
                    if (context.getCandidatePoint(x[edge][d]) >= 1 - EPS){
                        nextEdge = e;
                    }
                }
            }
            
            if (nextEdge == INVALID){
                // create cut
                displaySet(setOfNodes);
                IloExpr exp(context.getEnv());
                for (ListGraph::EdgeIt e(*graph); e != INVALID; ++e){
                    int u = getNodeLabel(graph->u(e));
                    int v = getNodeLabel(graph->v(e));
                    if (std::find(setOfNodes.begin(), setOfNodes.end(), u) != setOfNodes.end()){
                        if (std::find(setOfNodes.begin(), setOfNodes.end(), v) == setOfNodes.end()){
                            // add edge to cut
                            int edge = getEdgeLabel(e);
                            exp += x[edge][d];
                        }
                    }
                    else{
                        if (std::find(setOfNodes.begin(), setOfNodes.end(), v) != setOfNodes.end()){
                            // add edge to cut
                            int edge = getEdgeLabel(e);
                            exp += x[edge][d];
                        }
                    }
                }
                
                //std::cout << "Adding lazy constraint: " << exp << " >= 1" << std::endl;
                //std::cout << "Candidate has: " << context.getCandidateValue(exp) << " > 1" << std::endl;
                context.rejectCandidate(exp >= 1);
                exp.end();
                cutFound = true;
            }
            else{
                //displayEdge(nextEdge);
                previousNodeLabel = getNodeLabel(currentNode);
                if(getNodeLabel(currentNode) == getNodeLabel(graph->u(nextEdge))){
                    currentNode = (*graph).v(nextEdge);
                }
                else{
                    currentNode = (*graph).u(nextEdge);
                }
            }
        }
    }
    /*
    IloNum isUsed = context.getCandidatePoint(opened[j]);
    IloNum served = 0.0; // Number of clients currently served from j
    for (IloInt c = 0; c < nbClients; ++c)
        served += context.getCandidatePoint(supply[c][j]);
    if ( served > (nbClients - 1.0) * isUsed + EPS ) {
        IloNumExpr sum = IloExpr(context.getEnv());
        for (IloInt c = 0; c < nbClients; ++c)
            sum += supply[c][j];
        sum -= (nbClients - 1) * opened[j];
        cout << "Adding lazy capacity constraint " << sum << " <= 0" << endl;
        context.rejectCandidate(sum <= 0.0);
        sum.end();
    }*/
}


void GenericCallback::displayEdge(const ListGraph::Edge &e) const{
    std::cout << "(" << (*nodeLabel)[(*graph).u(e)] + 1<< ", " << (*nodeLabel)[(*graph).v(e)] + 1 << ")" << std::endl;               
}

void GenericCallback::displaySet(const std::vector<int> set) const{
    std::cout << "A cut has been found with set {";
    for (unsigned int i = 0; i < set.size() - 1; i++){
        std::cout << set[i]+1 << ", ";
    }
    std::cout << set.back()+1 << "}" << std::endl;
}

void GenericCallback::displaySolution_d(const IloCplex::Callback::Context &context, const int d) const{
    for (ListGraph::EdgeIt e(*graph); e != INVALID; ++e){
        int edge = (*edgeLabel)[e];
        int u = (*nodeLabel)[(*graph).u(e)]+1;
        int v = (*nodeLabel)[(*graph).v(e)]+1;
        if(context.inCandidate()){
            if (context.getCandidatePoint(x[edge][d]) >= 1 - EPS){
                std::cout << x[edge][d] << ": (" << u << "," << v << ")" << context.getCandidatePoint(x[edge][d]) << std::endl;
            }
        }
        if (context.inRelaxation()){
            if (context.getRelaxationPoint(x[edge][d]) >= EPS){
                std::cout << x[edge][d] << ": (" << u << "," << v << ")" << context.getRelaxationPoint(x[edge][d]) << std::endl;
            }
        }
    }


}
// Destructor
GenericCallback::~GenericCallback(){}