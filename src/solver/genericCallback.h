#ifndef __genericCallback__h
#define __genericCallback__h


#include <ilcplex/ilocplex.h>
#include <lemon/preflow.h>
#include "RSA.h"

#define EPS 1e-4 /**< Epsilon used for violation of cuts. **/


typedef IloArray<IloBoolVarArray> IloBoolVarMatrix;
/************************************************************************************
 * This is the class implementing the generic callback interface. It has two main 
 * functions: addUserCuts and addLazyConstraints.												
 ************************************************************************************/
class GenericCallback: public IloCplex::Callback::Function {
   private:
      // Variables for opening facilities.
      IloBoolVarMatrix x;
      Instance *instance;
      std::vector<Demand> *demands;
      ListGraph *graph;
      ListGraph::NodeMap<int> *nodeLabel;
      ListGraph::NodeMap<int> *nodeId;
      ListGraph::EdgeMap<int> *edgeLabel;
      ListGraph::EdgeMap<int> *edgeId;



   public:
      // Constructor with data.
      GenericCallback(const IloBoolVarMatrix &_x, Instance &i, ListGraph &g, std::vector<Demand> &toBeRouted,
         ListGraph::NodeMap<int> &nodeLabelMap, 
         ListGraph::NodeMap<int> &nodeIdMap, 
         ListGraph::EdgeMap<int> &edgeLabelMap, 
         ListGraph::EdgeMap<int> &edgeIdMap);

      // Separate the disaggregated capacity constraints.
      //
      // In the model we have for each location j the constraint
      //    sum(c in clients) supply[c][j] <= (nbClients-1) * opened[j]
      // Clearly, a client can only be serviced from a location that is opened,
      // so we also have a constraint
      //    supply[c][j] <= opened[j]
      // that must be satisfied by every feasible solution. These constraints
      // tend to be violated in LP relaxation. In this callback we separate
      // them.
      void addUserCuts (const IloCplex::Callback::Context &context) const; 
          /*
         IloInt const nbLocations = opened.getSize();
         IloInt const nbClients = supply.getSize();

         // For each j and c check whether in the current solution (obtained by
         // calls to getValue()) we have supply[c][j] > opened[j]. If so, then we have
         // found a violated constraint and add it as a cut.
         for (IloInt j = 0; j < nbLocations; ++j) {
            for (IloInt c = 0; c < nbClients; ++c) {
               IloNum const s = context.getRelaxationPoint(supply[c][j]);
               IloNum const o = context.getRelaxationPoint(opened[j]);
               if ( s > o + EPS) {
                  cout << "Adding: " << supply[c][j].getName() << " <= "
                       << opened[j].getName() << " [" << s << " > " << o << "]" << endl;
                  context.addUserCut( supply[c][j] - opened[j] <= 0,
                                     IloCplex::UseCutPurge, IloFalse);
               }
            }
         }*/


      // Lazy constraint callback to enforce the capacity constraints.
      //
      // If used then the callback is invoked for every integer feasible
      // solution CPLEX finds. For each location j it checks whether
      // constraint
      //    sum(c in C) supply[c][j] <= (|C| - 1) * opened[j]
      // is satisfied. If not then it adds the violated constraint as lazy
      // constraint.
      void addLazyConstraints(const IloCplex::Callback::Context &context) const;


      inline Demand getDemand_k(int k) const { return (*demands)[k]; }
      inline int getNodeLabel(ListGraph::Node v) const { return (*nodeLabel)[v]; }
      inline int getNodeId(ListGraph::Node v) const { return (*nodeId)[v]; }
      inline int getEdgeLabel(ListGraph::Edge e) const { return (*edgeLabel)[e]; }
      inline int getEdgeId(ListGraph::Edge e) const { return (*edgeId)[e]; }
      inline ListGraph::Node getNodeFromLabel(int label) const {
         for (ListGraph::NodeIt v(*graph); v != INVALID; ++v){
            if (getNodeLabel(v) == label){
                return v;
            }
         }
         return INVALID;
      }

      void displayEdge(const ListGraph::Edge &e) const ;
      void displaySet(const std::vector<int> set) const ;
      void displaySolution_d(const IloCplex::Callback::Context &context, const int d) const;
      // This is the function that we have to implement and that CPLEX will call
      // during the solution process at the places that we asked for.
      virtual void invoke (const IloCplex::Callback::Context &context);

      /// Destructor
      virtual ~GenericCallback();
};
#endif