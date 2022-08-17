#ifndef ABSTRACT_LAG_FORMULATION_H
#define ABSTRACT_LAG_FORMULATION_H

#include "../../formulation/rsa.h"
//#include "../../formulation/flowForm.h"
#include "../../tools/clockTime.h"
#include "../tools/lagTools.h"

#include <lemon/bellman_ford.h>
#include <lemon/cost_scaling.h>
#include <lemon/capacity_scaling.h>

/** This class implements a general Lagrangian Formulation considering the Flow formulation **/

class AbstractLagFormulation: public FlowForm{

        protected:

                /******************************************* ITERATIONS ******************************************/
                double currentLagrCost;
                double currentRealCost;
                double currentPrimalCost;

                /********************************************** TIME *********************************************/
                ClockTime time;
                double constAuxGraphTime;
                double updateVariablesTime;
                double ShorstestPathTime;
                double substractMultipliersTime;
                double costTime;

                /****************************************** MULTIPLIERS ******************************************/

                /** A vector storing the value of the Lagrangian multipliers associated with Length Constraints. **/
                std::vector<double> lagrangianMultiplierLength;

                /** A 2-dimensional vector storing the value of the Lagrangian multipliers associated with each Source/Target constraint. **/
                std::vector<std::vector<double>> lagrangianMultiplierSourceTarget;

                /** A 2-dimensional vector storing the value of the Lagrangian multipliers associated with each Flow constraint. **/
                std::vector<std::vector<double>> lagrangianMultiplierFlow;

                /** A 2-dimensional vector storing the value of the Lagrangian multipliers associated with each Non-Overlapping constraint. **/
                std::vector<std::vector<double> > lagrangianMultiplierOverlap;

                /** A 2-dimensional vector storing the value of the Lagrangian multipliers associated with One Slice per Demand constraint. **/
                std::vector<std::vector<double > > lagrangianMultiplierOneSlicePerDemand;

                /** A 1-dimensional vector storing the value of the Lagrangian multipliers associated with Max used slice overall constrant. **/
                std::vector<double > lagrangianMultiplierMaxUsedSliceOverall;

                /** A 1-dimensional vector storing the value of the Lagrangian multipliers associated with Max used slice overall (auxiliary) constrant. **/
                std::vector<double > lagrangianMultiplierMaxUsedSliceOverallAux;

                /** A 2-dimensional vector storing the value of the Lagrangian multipliers associated with Max used slice overall 2 constrant. **/
                std::vector< std::vector<double> > lagrangianMultiplierMaxUsedSliceOverall2;

                /** A 2-dimensional vector storing the value of the Lagrangian multipliers associated with Max used slice overall 3 constrant. **/
                std::vector< std::vector<double> > lagrangianMultiplierMaxUsedSliceOverall3;

                /***************************************** STABILITY CENTER ********************************************/

                /** A vector storing the value of the Lagrangian stability center associated with Length Constraints. **/
                std::vector<double> lagrangianSCLength;

                /** A 2-dimensional vector storing the value of the Lagrangian stability center associated with source/target constraints **/
                std::vector<std::vector<double>> lagrangianSCSourceTarget;

                /** A 2-dimensional vector storing the value of the Lagrangian stability center associated with flow constraints **/
                std::vector<std::vector<double>> lagrangianSCFlow;

                /** A 2-dimensional vector storing the value of the Lagrangian stability center associated with each Non-Overlapping constraint. **/
                std::vector< std::vector<double> > lagrangianSCOverlap;

                /** A 2-dimensional vector storing the value of the Lagrangian stability center associates with One slice per demand. **/
                std::vector<std::vector<double > > lagrangianSCOneSlicePerDemand;

                 /** A 1-dimensional vector storing the value of the Lagrangian stability center associated with Max used slice overall constrant. **/
                std::vector<double > lagrangianSCMaxUsedSliceOverall;

                /** A 1-dimensional vector storing the value of the Lagrangian stability center associated with Max used slice overall (auxiliary) constrant. **/
                std::vector<double > lagrangianSCMaxUsedSliceOverallAux;

                /** A 2-dimensional vector storing the value of the Lagrangian stability center associated with Max used slice overall 2 constrant. **/
                std::vector< std::vector<double> > lagrangianSCMaxUsedSliceOverall2;

                /** A 2-dimensional vector storing the value of the Lagrangian stability center associated with Max used slice overall 3 constrant. **/
                std::vector< std::vector<double> > lagrangianSCMaxUsedSliceOverall3;

                /*********************************************** SLACK ******************************************************/

                /** Stores the value of the slack of lengths constraints (i.e., b - Dx). */
                std::vector<double> lengthSlack;

                /** Stores the value of the slack of Source/Target constraints (i.e., b - Dx). */
                std::vector<std::vector<double>> sourceTargetSlack;

                /** Stores the value of the slack of flow constraints (i.e., b - Dx). */
                std::vector<std::vector<double>> flowSlack;

                /** Stores the value of the slack of Non-Overlapping constraints (i.e., b - Dx). */
                std::vector< std::vector<double> > overlapSlack;

                /** Stores the value of the slack of one slice per demand constraints (i.e., b - Dx). */
                std::vector<std::vector<double> > oneSlicePerDemandSlack;

                /** Stores the value of the slack of Max used slice overall constraints (i.e., b - Dx). */
                std::vector<double> maxUsedSliceOverallSlack;

                /** Stores the value of the slack of Max used slice overall (auxiliary) constraints (i.e., b - Dx). */
                std::vector<double> maxUsedSliceOverallAuxSlack;

                /** Stores the value of the slack of Max used slice overall 2 constraints (i.e., b - Dx). */
                std::vector< std::vector<double> > maxUsedSliceOverallSlack2;

                /** Stores the value of the slack of Max used slice overall constraints (i.e., b - Dx). */
                std::vector< std::vector<double> > maxUsedSliceOverallSlack3;

                /************************************** SLACK CONSIDERING PRIMAL VECTOR ************************************/

                /** Stores the value of the slack (primal variables) of lengths constraints (i.e., b - Dx). */
                std::vector<double> lengthSlack_v2;

                /** Stores the value of the slack (primal variables) of Source/Target constraints (i.e., b - Dx). */
                std::vector<std::vector<double>> sourceTargetSlack_v2;

                /** Stores the value of the slack (primal variables) of flow constraints (i.e., b - Dx). */
                std::vector<std::vector<double>> flowSlack_v2;

                /** Stores the value of the slack (primal variables) of Non-Overlapping constraints (i.e., b - Dx). */
                std::vector< std::vector<double> > overlapSlack_v2;

                /** Stores the value of the slack (primal variables) of one slice per demand constraints (i.e., b - Dx). */
                std::vector<std::vector<double > > oneSlicePerDemandSlack_v2;

                /** Stores the value of the slack (primal variables) of Max used slice overall constraints (i.e., b - Dx). */
                std::vector<double> maxUsedSliceOverallSlack_v2;

                /** Stores the value of the slack (primal variables) of Max used slice overall (auxiliary) constraints (i.e., b - Dx). */
                std::vector<double> maxUsedSliceOverallAuxSlack_v2;

                /** Stores the value of the slack (primal variables) of Max used slice overall 2 constraints (i.e., b - Dx). */
                std::vector< std::vector<double> > maxUsedSliceOverallSlack2_v2;

                /** Stores the value of the slack (primal variables) of Max used slice overall constraints (i.e., b - Dx). */
                std::vector< std::vector<double> > maxUsedSliceOverallSlack3_v2;

                /************************************************ DIRECTION ***************************************************/

                /** Stores the value of the direction of lengths constraints. */
                std::vector<double> lengthDirection;

                /** Stores the value of the direction of Source/Target constraints. */
                std::vector<std::vector<double>> sourceTargetDirection;

                /** Stores the value of the direction of flow constraints. */
                std::vector<std::vector<double>> flowDirection;

                /** Stores the value of the direction of overlap constraints. */
                std::vector< std::vector<double> > overlapDirection;

                /** Stores the value of the direction of one slice per demand constraints. */
                std::vector<std::vector<double > > oneSlicePerDemandDirection;

                /** Stores the value of the direction of Max used slice overall constraints. */
                std::vector<double> maxUsedSliceOverallDirection;

                /** Stores the value of the direction of Max used slice overall (auxiliary) constraints. */
                std::vector<double> maxUsedSliceOverallAuxDirection;

                /** Stores the value of the direction of Max used slice overall 2 constraints. */
                std::vector< std::vector<double> > maxUsedSliceOverall2Direction;

                /** Stores the value of the direction of Max used slice overall constraints. */
                std::vector< std::vector<double> > maxUsedSliceOverall3Direction;

                /******************************************* PRIMAL APPROXIMATION *********************************************/

                /** Vector with the lagrangian current primal approximation solution **/
                std::vector< std::vector<double> > primal_linear_solution;

                /** Primal approximation for maxUsedSliceOverall of the model **/
                double primalMaxUsedSliceOverall;

                /** Primal approximation for auxiliary constraint max used slice overall **/
                std::vector<bool> primalVarAuxZ;

                /******************************************** ASSIGNMENT MATRIX ***********************************************/

                /** Vector with the current lagrangian solution **/
                std::vector< std::vector<bool> > assignmentMatrix_d;

                /** Variable maxUsedSliceOverall of the model **/
                double maxUsedSliceOverall;
                
                /** Auxiliary variable for auxiliary constraint max used slice overall **/
                std::vector<bool> varAuxZ;

                /******************************************** BEST FEASIBLE SOLUTION ******************************************/
                
                /** Vector with the current best feasible solution. **/
                std::vector< std::vector<bool> > bestFeasibleSol;

                /** Best feasible value for the variable maxUsedSliceOverall of the model **/
                double bestMaxUsedSliceOverall;

                /************************************************** COEFF *****************************************************/

                /* Refers to the objective function coefficient of the arcs depending on the chosen objective function */
                std::vector< std::shared_ptr<ArcCost> > coeff; 

                /* Refers to the objective function coefficient of the arcs depending on the chosen objective function */
                std::vector< std::shared_ptr<ArcCost> > coeff8; 

                std::vector< std::shared_ptr<IterableIntMap<ListDigraph, ListDigraph::Node>>> mapItLabel;

                /* Refers to the upper bound of the arcs depending on the chosen objective function */
                std::vector< std::shared_ptr<ArcMap> > upperBound; 

                std::vector< std::shared_ptr<ArcMap> > lowerBound; 

                double realMaxUsedSliceOverall;

                double maxUsedSliceOverallUpperBound;

                double maxUsedSliceOverallLowerBound;

                std::vector<int> nbSlicesLimitFromEdge;

        public:
                /************************************************************************************************************/
                /*			                        CONSTRUCTORS	       	                                    */
                /************************************************************************************************************/
                /** Constructor. Builds the Formulation.  @param instance The instance to be solved. **/
                AbstractLagFormulation(const Instance &instance);

                /************************************************************************************************************/
                /*		                                   GETTERS 	     	                                    */
                /************************************************************************************************************/

                /********************************************* COSTS ITERATION **********************************************/
                double getLagrCurrentCost() const { return currentLagrCost; }
                double getRealCurrentCost() const { return currentRealCost; }
                double getPrimalCurrentCost() const { return currentPrimalCost; }

                /************************************************** TIME ****************************************************/
                double getConstAuxGraphTime() const { return constAuxGraphTime; }
                double getUpdateVariablesTime() const { return updateVariablesTime;}
                double getShorstestPathTime() const { return ShorstestPathTime;}
                double getSubstractMultipliersTime() const { return substractMultipliersTime;}
                double getCostTime() const {return costTime;}

                bool isInteger();

                /*********************************************** AUXILIARY **************************************************/

                void updateMaxUsedSliceOverallUpperBound(double value){ if(value < maxUsedSliceOverallUpperBound){maxUsedSliceOverallUpperBound = value;}}

                /**************************************** ****** MULTIPLIERS ************************************************/

                /** Returns the multiplier for the length constraint k **/
                double getLengthMultiplier_k(int k) const { return lagrangianMultiplierLength[k]; }

                /** Returns the multiplier for the source target constraint k,v **/
                double getSourceTargetMultiplier_k(int k, int v) const { return lagrangianMultiplierSourceTarget[k][v]; }

                /** Returns the multiplier for the flow constraint k,v **/
                double getFlowMultiplier_k(int k, int v) const { return lagrangianMultiplierFlow[k][v]; }

                /** Returns the multiplier for the overlap constraint e,s **/
                double getOverlapMultiplier_k(int e, int s) const { return lagrangianMultiplierOverlap[e][s]; }

                /** Returns the multiplier for the one slice per demand constraint e,k **/
                double getOneSlicePerDemandMultiplier_k(int e, int k) const { return lagrangianMultiplierOneSlicePerDemand[e][k]; }

                /** Returns the multiplier for the maximum used slice overall 1 constraint d **/
                double getMaxUsedSliceOverallMultiplier_k(int k) const { return lagrangianMultiplierMaxUsedSliceOverall[k]; }

                /** Returns the multiplier for the maximum used slice overall 1 (auxiliary) constraint d **/
                double getMaxUsedSliceOverallAuxMultiplier_k(int k) const { return lagrangianMultiplierMaxUsedSliceOverallAux[k]; }
                
                /** Returns the multiplier for the maximum used slice overall 2 constraint e,s **/
                double getMaxUsedSliceOverall2Multiplier_k(int e,int s) const { return lagrangianMultiplierMaxUsedSliceOverall2[e][s]; }
                
                /** Returns the multiplier for the maximum used slice overall 3 constraint v,s **/
                double getMaxUsedSliceOverall3Multiplier_k(int v, int s) const { return lagrangianMultiplierMaxUsedSliceOverall3[v][s]; }

                /******************************************** STABILITY CENTER *********************************************/

                /** Returns the stability center for the length constraint k **/
                double getLengthSC_k(int k) const { return lagrangianSCLength[k]; }

                /** Returns the stability center for the source target constraint k,v **/
                double getSourceTargetSC_k(int k, int v) const { return lagrangianSCSourceTarget[k][v]; }

                /** Returns the stability center for the flow constraint k,v **/
                double getFlowSC_k(int k, int v) const { return lagrangianSCFlow[k][v]; }

                /** Returns the stability center for the overlap constraint e,s **/
                double getOverlapSC_k( int e, int s) const { return lagrangianSCOverlap[e][s]; }

                /** Returns the stability center for the one slice per demand constraint e,k **/
                double getOneSlicePerDemandSC_k( int e, int k) const { return lagrangianSCOneSlicePerDemand[e][k]; }

                /** Returns the stability center for the maximum used slice overall 1 constraint k **/
                double getMaxUsedSliceOverallSC_k(int k) const { return lagrangianSCMaxUsedSliceOverall[k]; }

                /** Returns the stability center for the maximum used slice overall 1 (auxiliary) constraint k **/
                double getMaxUsedSliceOverallAuxSC_k(int k) const { return lagrangianSCMaxUsedSliceOverallAux[k]; }

                /** Returns the stability center for the maximum used slice overall 2 constraint e,s **/
                double getMaxUsedSliceOverall2SC_k(int e, int s) const { return lagrangianSCMaxUsedSliceOverall2[e][s]; }

                /** Returns the stability center for the maximum used slice overall 3 constraint v,s **/
                double getMaxUsedSliceOverall3SC_k(int v, int s) const { return lagrangianSCMaxUsedSliceOverall3[v][s]; }


                /************************************************** SLACK ****************************************************/

                /** Return the slack for the the length constraint k **/
                double getLengthSlack_k(int k) const { return lengthSlack[k];}

                /** Return the slack for the the source target constraint k,v **/
                double getSourceTargetSlack_k(int k, int v) const { return sourceTargetSlack[k][v];}

                /** Return the slack for the the flow constraint k,v **/
                double getFlowSlack_k(int k,int v) const { return flowSlack[k][v];}

                /** Return the slack for the the overlap constraint e,s **/
                double getOverlapSlack_k(int e, int s) const { return overlapSlack[e][s]; }

                /** Return the slack for the the one slice per demand constraint e,k **/
                double getOneSlicePerDemandSlack_k(int e, int k) const { return oneSlicePerDemandSlack[e][k]; }

                /** Return the slack for the the maximum used slice overall 1 constraint k **/
                double getMaxUsedSliceOverallSlack_k(int k) const { return maxUsedSliceOverallSlack[k]; }

                /** Return the slack for the the maximum used slice overall 1 (auxiliary) constraint k **/
                double getMaxUsedSliceOverallAuxSlack_k(int k) const { return maxUsedSliceOverallAuxSlack[k]; }

                /** Return the slack for the the maximum used slice overall 2 constraint e,s **/
                double getMaxUsedSliceOverall2Slack_k(int e, int s) const { return maxUsedSliceOverallSlack2[e][s]; }

                /** Return the slack for the the maximum used slice overall 3 constraint v,s **/
                double getMaxUsedSliceOverall3Slack_k(int v, int s) const { return maxUsedSliceOverallSlack3[v][s]; }

                /************************************** SLACK PRIMAL VARIABLES *******************************************/
                
                /** Return the slack with primal variables for the the length constraint k **/
                double getLengthSlack_v2_k(int k) const { return lengthSlack_v2[k];}

                /** Return the slack with primal variables for the the source/target constraint k,v **/
                double getSourceTargetSlack_v2_k(int k, int v) const { return sourceTargetSlack_v2[k][v];}

                /** Return the slack with primal variables for the the flow constraint k,v **/
                double getFlowSlack_v2_k(int k,int v) const { return flowSlack_v2[k][v];}

                /** Return the slack with primal variables for the the non-overlap constraint e,s **/
                double getOverlapSlack_v2_k(int e, int s) const { return overlapSlack_v2[e][s]; }

                /** Return the slack with primal variables for the one slice per demand constraint e,k **/
                double getOneSlicePerDemandSlack_v2_k(int e, int k) const { return oneSlicePerDemandSlack_v2[e][k]; }

                /** Return the slack with primal variables for the the maximum used slice overall 1 constraint k **/
                double getMaxUsedSliceOverallSlack_v2_k(int k) const { return maxUsedSliceOverallSlack_v2[k]; }

                /** Return the slack with primal variables for the the maximum used slice overall 1 constraint k **/
                double getMaxUsedSliceOverallAuxSlack_v2_k(int k) const { return maxUsedSliceOverallAuxSlack_v2[k]; }

                /** Return the slack with primal variables for the the maximum used slice overall 2 constraint e,s **/
                double getMaxUsedSliceOverall2Slack_v2_k(int e, int s) const { return maxUsedSliceOverallSlack2_v2[e][s]; }

                /** Return the slack with primal variables for the the maximum used slice overall 3 constraint v,s **/
                double getMaxUsedSliceOverall3Slack_v2_k(int v, int s) const { return maxUsedSliceOverallSlack3_v2[v][s]; }

                /************************************************ DIRECTION **************************************************/

                /** Return the slack with primal variables for the the length constraint k **/
                double getLengthDirection_k(int k) const { return lengthDirection[k];}

                /** Return the slack with primal variables for the the source/target constraint k,v **/
                double getSourceTargetDirection_k(int k, int v) const { return sourceTargetDirection[k][v];}

                /** Return the slack with primal variables for the the flow constraint k,v **/
                double getFlowDirection_k(int k,int v) const { return flowDirection[k][v];}

                /** Return the slack with primal variables for the the non-overlap constraint e,s **/
                double getOverlapDirection_k(int e,int s) const { return overlapDirection[e][s]; }

                /** Return the slack with primal variables for the the one slice per demand constraint e,k **/
                double getOneSlicePerDemandDirection_k(int e, int k) const { return oneSlicePerDemandDirection[e][k]; }

                /** Return the slack with primal variables for the the maximum used slice overall constraint k **/
                double getMaxUsedSliceOverallDirection_k(int k) const { return maxUsedSliceOverallDirection[k]; }

                /** Return the slack with primal variables for the the maximum used slice overall (auxiliary) constraint k **/
                double getMaxUsedSliceOverallAuxDirection_k(int k) const { return maxUsedSliceOverallAuxDirection[k]; }

                /** Return the slack with primal variables for the the maximum used slice overall 2 constraint e,s **/
                double getMaxUsedSliceOverall2Direction_k(int e, int s) const { return maxUsedSliceOverall2Direction[e][s]; }

                /** Return the slack with primal variables for the the maximum used slice overall 3 constraint v,s **/
                double getMaxUsedSliceOverall3Direction_k(int v, int s) const { return maxUsedSliceOverall3Direction[v][s]; }

                /************************************************ MODULES ****************************************************/

                /* Returns |slack|^2 */
                virtual double getSlackModule(double = -1.0) = 0;

                /* Returns |slack_v2|^2 , slack considering primal variables*/
                virtual double getSlackModule_v2(double = -1.0) = 0;

                /* Returns slack*slack_v2 , slack considering primal variables*/
                virtual double getSlackPrimalSlackProd(double = -1.0) = 0;

                /* Returns |direction|^2 */
                virtual double getDirectionModule() = 0 ;

                /* Returns slack*direction */
                virtual double getSlackDirectionProd() = 0;

                /* Returns slack*direction  considering the normal direction*/
                virtual double getSlackDirectionProdNormal() = 0;

                /* Returns |slack|/m, where m is the number of relaxed constraints */
                virtual double getMeanSlackModule_v2() =0;

                /* Return the value to update the direction as a scalar of the subgradient */
                double getDirectionMult();

                /***************************************** PRIMAL SOLUTION ***********************************************/

                /** Returns the vector with the primal approximation values. **/
                std::vector< std::vector<double> > getPrimalVariables() const { return primal_linear_solution;}

                /***************************************** CURRENT SOLUTION **********************************************/

                /** Returns the vector with the sub problem variables values **/
                std::vector< std::vector<bool> > getVariables() const { return assignmentMatrix_d;}

                /********************************************* GRAPH D ***********************************************/

                /** Returns the Graph of demand d. **/
                std::shared_ptr<ListDigraph> getVecGraphD(int d) const { return vecGraph[d];}

                /*********************************************** COEFF *************************************************/

                /** Returns the map cost of the objective function coefficients. **/
                std::shared_ptr<ArcCost> getCoeffMap(int d) const {return coeff[d];}

                std::shared_ptr<ArcCost> getCoeffMapObj8(int d) const { return coeff8[d]; } 

                /** Returns the map index of the atcs index. **/
                std::shared_ptr<ArcMap> getIndexMap(int d) const {return vecArcIndex[d];}

                std::shared_ptr<ArcMap> getVarIdMap(int d) const {return vecArcVarId[d];}

                std::shared_ptr<ArcMap> getArcSliceMap(int d) const {return vecArcSlice[d];}

                std::shared_ptr<ArcMap> getArcLabelMap(int d) const {return vecArcLabel[d];}

                std::shared_ptr<ArcMap> getArcLowerMap(int d) const {return lowerBound[d];}

                std::shared_ptr<ArcMap> getArcUpperMap(int d) const {return upperBound[d];}

                /** Returns the lower bound of an arc in a graph. @param a The arc. @param d The graph index. **/
                int getArcLower(const ListDigraph::Arc &a, int d) const { return (*lowerBound[d])[a]; }

                void getPrimalSolution(double *);

                void getPrimalAppSolution(double *);

                virtual void getDualSolution(double *) =0;

                void getBestFeasibleSolution(double *);

                /*******************************************************************************************************/
                /*		                               SETTERS 	    	    	                               */
                /*******************************************************************************************************/

                /**************************************** COSTS ITERATION **********************************************/
                void setCurrentLagrCost(double val){ currentLagrCost = val; }
                void incCurrentLagrCost(double val){ currentLagrCost += val; }
                void setCurrentRealCost(double val){ currentRealCost = val; }
                void incCurrentRealCost(double val){ currentRealCost += val; }
                void setCurrentPrimalCost(double val){ currentPrimalCost = val; }
                void incCurrentPrimalCost(double val){ currentPrimalCost += val; }

                /********************************************** TIME ***************************************************/
                void setConstAuxGraphTime(double value) { constAuxGraphTime = value;}
                void setUpdateVariablesTime(double value) { updateVariablesTime = value; }
                void setShorstestPathTime(double value) { ShorstestPathTime = value; }
                void setSubstractMultipliersTime(double value) { substractMultipliersTime = value;}
                void incUpdateVariablesTime(double value) { updateVariablesTime += value; }
                void incShorstestPathTime(double value) { ShorstestPathTime += value; }
                void incSubstractMultipliersTime(double value) { substractMultipliersTime+=value;}
                void setCostTime(double value) { costTime = value;}
                void incCostTime(double value) { costTime += value;}

                /******************************************* MULTIPLIERS ***********************************************/

                /** Sets the multiplier for the length constraint k **/
                void setLengthMultiplier_k (int k, double val) { lagrangianMultiplierLength[k] = val; }

                /** Sets the multiplier for the source/target constraint k,v **/
                void setSourceTargetMultiplier_k (int k, int v, double val) { lagrangianMultiplierSourceTarget[k][v] = val; }

                /** Sets the multiplier for the flow constraint k,v **/
                void setFlowMultiplier_k (int k, int v, double val) { lagrangianMultiplierFlow[k][v] = val; }

                /** Sets the multiplier for the non-overlap constraint e,s **/
                void setOverlapMultiplier_k ( int e, int s, double val) { lagrangianMultiplierOverlap[e][s] = val; }

                /** Sets the multiplier for the one slice per demand constraint e,k **/
                void setOneSlicePerDemandMultiplier_k(int e, int k, double val) { lagrangianMultiplierOneSlicePerDemand[e][k]=val; }

                /** Sets the multiplier for the maximum slice used overall 1 constraint k **/
                void setMaxUsedSliceOverallMultiplier_k(int k, double val) { lagrangianMultiplierMaxUsedSliceOverall[k] = val;}

                /** Sets the multiplier for the maximum slice used overall 1 (auxiliary) constraint k **/
                void setMaxUsedSliceOverallAuxMultiplier_k(int k, double val) { lagrangianMultiplierMaxUsedSliceOverallAux[k] = val;}

                /** Sets the multiplier for the maximum slice used overall 2 constraint e,s **/
                void setMaxUsedSliceOverall2Multiplier_k(int e,int s, double val) { lagrangianMultiplierMaxUsedSliceOverall2[e][s] = val;}

                /** Sets the multiplier for the maximum slice used overall 3 constraint v,s **/
                void setMaxUsedSliceOverall3Multiplier_k(int v,int s, double val) { lagrangianMultiplierMaxUsedSliceOverall3[v][s] = val;}

                /********************************** STABILITY CENTER ***************************************/

                /** Sets the stability center for the length constraint k **/
                void setLengthSC_k (int k, double val) { lagrangianSCLength[k] = val; }

                /** Sets the stability center for the source/target constraint k,v **/
                void setSourceTargetSC_k (int k, int v, double val) { lagrangianSCSourceTarget[k][v] = val; }

                /** Sets the stability center for the flow constraint k,v **/
                void setFlowSC_k (int k, int v, double val) { lagrangianSCFlow[k][v] = val; }

                /** Sets the stability center for the non-overlap constraint e,s **/
                void setOverlapSC_k ( int e, int s, double val) { lagrangianSCOverlap[e][s] = val; }

                /** Sets the stability center for the one slice per demand constraint e,k **/
                void setOneSlicePerDemandSC_k(int e, int k, double val) { lagrangianSCOneSlicePerDemand[e][k] = val; }

                /** Sets the stability center for the maximum used slice 1 constraint k **/
                void setMaxUsedSliceOverallSC_k(int k, double val) { lagrangianSCMaxUsedSliceOverall[k] = val;}

                /** Sets the stability center for the maximum used slice 1 (auxiliary) constraint k **/
                void setMaxUsedSliceOverallAuxSC_k(int k, double val) { lagrangianSCMaxUsedSliceOverallAux[k] = val;}

                /** Sets the stability center for the maximum used slice 2 constraint e,s **/
                void setMaxUsedSliceOverall2SC_k(int e,int s, double val) { lagrangianSCMaxUsedSliceOverall2[e][s] = val;}

                /** Sets the stability center for the maximum used slice 3 constraint v,s **/
                void setMaxUsedSliceOverall3SC_k(int v,int s, double val) { lagrangianSCMaxUsedSliceOverall3[v][s] = val;}

                /*********************************************** SLACK ****************************************************/

                /** Sets length slack - considering sub problem solution **/
                void setLengthSlack_k (int k, double val) { lengthSlack[k] = val; }

                /** Sets source/target slack - considering sub problem solution **/
                void setSourceTargetSlack_k (int k, int v, double val) { sourceTargetSlack[k][v] = val; }

                /** Sets flow slack - considering sub problem solution **/
                void setFlowSlack_k (int k, int v, double val) { flowSlack[k][v] = val; }

                /** Sets non-overlap slack - considering sub problem solution **/
                void setOverlapSlack_k( int e, int s, double val){ overlapSlack[e][s] = val; }

                /** Sets one slice per demand slack - considering sub problem solution **/
                void setOneSlicePerDemandSlack_k(int e, int k, double val) { oneSlicePerDemandSlack_v2[e][k] = val; }

                /** Sets maximum used slice overall slack - considering sub problem solution **/
                void setMaxUsedSliceOverallSlack_k(int k, double val) { maxUsedSliceOverallSlack[k] = val; }

                /** Sets maximum used slice overall slack (auxiliary) - considering sub problem solution **/
                void setMaxUsedSliceOverallAuxSlack_k(int k, double val) { maxUsedSliceOverallAuxSlack[k] = val; }

                /** Sets maximum used slice overall 2 slack - considering sub problem solution **/
                void setMaxUsedSliceOverall2Slack_k(int e,int s, double val) { maxUsedSliceOverallSlack2[e][s] = val; }

                /** Sets maximum used slice overall 3 slack - considering sub problem solution **/
                void setMaxUsedSliceOverall3Slack_k(int v,int s, double val) { maxUsedSliceOverallSlack3[v][s] = val; }

                /********************************** SLACK PRIMAL VARIABLES ***************************************/
                
                /** Sets length slack - considering primal approximation **/
                void setLengthSlack_v2_k (int k, double val) { lengthSlack_v2[k] = val; }

                /** Sets source/target slack - considering primal approximation **/
                void setSourceTargetSlack_v2_k (int k, int v, double val) { sourceTargetSlack_v2[k][v] = val; }

                /** Sets flow slack - considering primal approximation **/
                void setFlowSlack_v2_k (int k, int v, double val) { flowSlack_v2[k][v] = val; }

                /** Sets non-overlap slack - considering primal approximation **/
                void setOverlapSlack_v2_k( int e, int s, double val){ overlapSlack_v2[e][s] = val; }

                /** Sets one slice per demand slack - considering primal approximation **/
                void setOneSlicePerDemandSlack_v2_k(int e, int k, double val) { oneSlicePerDemandSlack_v2[e][k] = val;}

                /** Sets maximum used slice overall 1 slack - considering primal approximation **/
                void setMaxUsedSliceOverallSlack_v2_k(int k, double val) { maxUsedSliceOverallSlack_v2[k] = val;}

                /** Sets maximum used slice overall 1 (auxiliary) slack - considering primal approximation **/
                void setMaxUsedSliceOverallAuxSlack_v2_k(int k, double val) { maxUsedSliceOverallAuxSlack_v2[k] = val;}

                /** Sets maximum used slice overall 2 slack - considering primal approximation **/
                void setMaxUsedSliceOverall2Slack_v2_k(int e,int s, double val) { maxUsedSliceOverallSlack2_v2[e][s] = val;}

                /** Sets maximum used slice overall 3 slack - considering primal approximation **/
                void setMaxUsedSliceOverall3Slack_v2_k(int v,int s, double val) { maxUsedSliceOverallSlack3_v2[v][s] = val;}

                /****************************************** DIRECTION ************************************************/

                /** Sets length direction **/
                void setLengthDirection_k (int k, double val) { lengthDirection[k] = val; }

                /** Sets source/target slack**/
                void setSourceTargetDirection_k (int k, int v, double val) { sourceTargetDirection[k][v] = val; }

                /** Sets flow direction **/
                void setFlowDirection_k (int k, int v, double val) { flowDirection[k][v] = val; }

                /** Sets non-overlap direction **/
                void setOverlapDirection_k( int e, int s, double val){ overlapDirection[e][s] = val; }

                /** Sets one slice per demand direction **/
                void setOneSlicePerDemandDirection_k(int e, int k, double val ) { oneSlicePerDemandDirection[e][k] =val;}

                /** Sets maximum used slice overall direction **/
                void setMaxUsedSliceOverallDirection_k(int k, double val) { maxUsedSliceOverallDirection[k] = val;}

                /** Sets maximum used slice overall (auxiliary) direction **/
                void setMaxUsedSliceOverallAuxDirection_k(int k, double val) { maxUsedSliceOverallAuxDirection[k] = val;}

                /** Sets maximum used slice overall 2 direction **/
                void setMaxUsedSliceOverall2Direction_k(int e,int s, double val) { maxUsedSliceOverall2Direction[e][s] = val;}

                /** Sets maximum used slice overall 3 direction **/
                void setMaxUsedSliceOverall3Direction_k(int v,int s, double val) { maxUsedSliceOverall3Direction[v][s] = val;}

                /* ************************************************************************************************* /
                *                                       INITIALIZATION METHODS                              
                *************************************************************************************************** */
                
                /* Initiliaze the upper bound according to each objective function */
                double initialUBValue();

                double initialUBValueObj1();

                double initialUBValueObj2();

                double initialUBValueObj2p();

                double initialUBValueObj4();

                double initialUBValueObj8();

                /** Sets all initial parameters **/
                virtual void init(bool = true) = 0;

                /** Sets the lower and upper bound  of the variables. **/
                void updateLowerUpperBound(double*,double*);

                void verifyLowerUpperBound();

                /******************************************** MULTIPLIERS *******************************************/

                virtual void startMultipliers(double *,int,int) = 0;

                /** Sets the initial lagrangian multipliers values for the subgradient to run. **/
                virtual void initMultipliers() = 0;

                /** Sets the initial lagrangian multipliers associated with length constraints. **/
                void initializeLengthMultipliers(double);

                /** Sets the initial lagrangian multipliers associated with Source/Target constraints **/
                void initializeSourceTargetMultipliers(double);
                
                /** Sets the initial lagrangian multipliers associated with flow constraints **/
                void initializeFlowMultipliers(double);

                /** Sets the initial lagrangian multipliers associated with non-overlapping constraints. **/
                void initializeOverlapMultipliers(double);

                /** Sets the initial lagrangian multipliers associated with one slice per demand constraints. **/
                void initializeOneSlicePerDemandMultipliers(double);

                /** Sets the initial lagrangian multipliers associated with max used slice overall constraints. **/
                void initializeMaxUsedSliceOverallMultipliers(double);

                /** Sets the initial lagrangian multipliers associated with max used slice overall (auxiliary) constraints. **/
                void initializeMaxUsedSliceOverallAuxMultipliers(double);

                /** Sets the initial lagrangian multipliers associated with max used slice overall 2 constraints. **/
                void initializeMaxUsedSliceOverall2Multipliers(double);

                /** Sets the initial lagrangian multipliers associated with max used slice overall 3 constraints. **/
                void initializeMaxUsedSliceOverall3Multipliers(double);

                /******************************************** STABILITY CENTER *******************************************/

                /** Initializes the slack of relaxed constraints. **/
                virtual void initStabilityCenter() = 0;

                /** Sets the initial lagrangian stability center associated with length constraints. **/
                void initializeLengthSC();

                /** Sets the initial lagrangian stability center associated with Source/Target constraints **/
                void initializeSourceTargetSC();
                
                /** Sets the initial lagrangian stability center associated with flow constraints **/
                void initializeFlowSC();

                /** Sets the initial lagrangian stability center associated with overlap constraints **/
                void initializeOverlapSC();

                /** Sets the initial lagrangian stability center associated with one slice per demand constraints **/
                void initializeOneSlicePerDemandSC();

                /** Sets the initial lagrangian stability center associated with max used slice overall constraints **/
                void initializeMaxUsedSliceOverallSC();

                /** Sets the initial lagrangian stability center associated with max used slice overall (auxiliary) constraints **/
                void initializeMaxUsedSliceOverallAuxSC();

                /** Sets the initial lagrangian stability center associated with max used slice overall 2 constraints **/
                void initializeMaxUsedSliceOverall2SC();

                /** Sets the initial lagrangian stability center associated with max used slice overall 3 constraints **/
                void initializeMaxUsedSliceOverall3SC();

                /************************************************ SLACK **************************************************/

                /** Initializes the slack of relaxed constraints. **/
                virtual void initSlacks() = 0;

                /** Initializes the slack of length constraints. **/
                void initializeLengthSlacks();
                
                /** Initializes the slack of Source/Target constraints. **/
                void initializeSourceTargetSlacks();

                /** Initializes the slack of Flow constraints. **/
                void initializeFlowSlacks();

                /** Initializes the slack of non-overlap constraints. **/
                void initializeOverlapSlacks();

                /** Initializes the slack of one slice per demand constraints. **/
                void initializeOneSlicePerDemandSlacks();

                /** Sets the initial lagrangian slack associated with max used slice overall constraints **/
                void initializeMaxUsedSliceOverallSlacks();

                /** Sets the initial lagrangian slack associated with max used slice overall (auxiliary) constraints **/
                void initializeMaxUsedSliceOverallAuxSlacks();

                /** Sets the initial lagrangian slack associated with max used slice overall 2 constraints **/
                void initializeMaxUsedSliceOverall2Slacks();

                /** Sets the initial lagrangian slack associated with max used slice overall 3 constraints **/
                void initializeMaxUsedSliceOverall3Slacks();


                /** Reset the slack of relaxed constraints. **/
                virtual void resetSlacks() = 0;

                /** Reset the slack of length constraints. **/
                void resetLengthSlacks();
                
                /** Reset the slack of Source/Target constraints. **/
                void resetSourceTargetSlacks();

                /** Reset the slack of Flow constraints. **/
                void resetFlowSlacks();

                /** Reset the slack of non-overlap constraints. **/
                void resetOverlapSlacks();

                /** Reset the slack of one slice per demand constraints. **/
                void resetOneSlicePerDemandSlacks();

                /** Reset lagrangian slack associated with max used slice overall constraints **/
                void resetMaxUsedSliceOverallSlacks();

                /** Reset lagrangian slack associated with max used slice overall (auxiliary) constraints **/
                void resetMaxUsedSliceOverallAuxSlacks();

                /** Reset lagrangian slack associated with max used slice overall 2 constraints **/
                void resetMaxUsedSliceOverall2Slacks();

                /** Reset lagrangian slack associated with max used slice overall 3 constraints **/
                void resetMaxUsedSliceOverall3Slacks();

                /************************************* PRIMAL APPROXIMATION SLACK ******************************************/

                /** Initializes the slack (considering primal approximation) of relaxed constraints. **/
                virtual void initPrimalSlacks() = 0;

                /** Initializes the slack of length constraints (considering primal approximation). **/
                void initializeLengthPrimalSlacks();

                /** Initializes the slack of Source/Target constraints (considering primal approximation). **/
                void initializeSourceTargetPrimalSlacks();

                void initializeFlowPrimalSlacks();

                void initializeOverlapPrimalSlacks();

                void initializeOneSlicePerDemandPrimalSlacks();

                void initializeMaxUsedSliceOverallPrimalSlacks();

                void initializeMaxUsedSliceOverallAuxPrimalSlacks();

                void initializeMaxUsedSliceOverall2PrimalSlacks();

                void initializeMaxUsedSliceOverall3PrimalSlacks();

                /*********************************************** DIRECTION ************************************************/

                /** Initializes the direction of relaxed constraints. **/
                virtual void initDirection() = 0;

                /** Sets the initial direction of length constraints **/
                void initializeLengthDirection();

                /** Sets the initial direction of source/target constraints **/
                void initializeSourceTargetDirection();

                /** Sets the initial direction of flow constraints **/
                void initializeFlowDirection();

                /** Sets the initial direction of overlap constraints **/
                void initializeOverlapDirection();

                /** Sets the initial direction of overlap constraints **/
                void initializeOneSlicePerDemandDirection();

                /** Sets the initial direction of maximum used slice overall 1 constraints **/
                void initializeMaxUsedSliceOverallDirection();

                /** Sets the initial direction of maximum used slice overall 1 (auxiliary) constraints **/
                void initializeMaxUsedSliceOverallAuxDirection();

                /** Sets the initial direction of maximum used slice overall 2 constraints **/
                void initializeMaxUsedSliceOverall2Direction();

                /** Sets the initial direction of maximum used slice overall 3 constraints **/
                void initializeMaxUsedSliceOverall3Direction();

                /*********************************************** ASSIGNMENT MATRIX ********************************************/

                /** Initializes the variables values */
                void initAssignmentMatrix();

                /*********************************************** PRIMAL SOLUTION **********************************************/

                /** Initializes the primal approximation given by volume algorithm **/
                void initPrimalSolution();

                /******************************************* BEST FEASIBLE SOLUTION *******************************************/

                void initBestFeasibleSolution();

                /*********************************************** PRIMAL APPROXIMATION **********************************************/

                void initPrimalApproximation();

                /************************************************ COEFFICIENTS ************************************************/

                /** Inializes the objective function coefficients **/
                void initCoeff();

                void setCoeffMapObj8();

                /* *************************************************************************************************************
                *                                                RUNING METHODS
                ************************************************************************************************************** */
                
                /** Solves the RSA lagrangian sub problem according to the chosen formulation **/
                virtual void run(bool=false) = 0;

                virtual void subtractConstantValuesFromLagrCost() = 0;

                /*********************************************** CHECK FEASIBILITY ********************************************/

                /** Checks if the sub problem solution is feasible **/
                virtual bool checkFeasibility() = 0;

                /** Check if length constraints are feasible **/
                bool checkLengthFeasibility();

                /** Check if source/target constraints are feasible **/
                bool checkSourceTargetFeasibility();

                /** Check if flow constraints are feasible **/
                bool checkFlowFeasibility();

                /** Check if non-overlap constraints are feasible **/
                bool checkOverlapFeasibility();

                /** Check if one slice per demand are feasible **/

                /** Check if maximum used slice overall are feasible **/
                bool checkMaxUsedSliceOverallFeasibility();

                /** Check if maximum used slice overall (auxiliary) are feasible **/

                /** Checks if the primal approximation solution is feasible **/
                virtual bool checkFeasibility_v2() = 0;

                /** Check if length constraints are feasible - primal approximation **/
                bool checkLengthFeasibility_v2();

                /** Check if source/target constraints are feasible - primal approximation **/
                bool checkSourceTargetFeasibility_v2();

                /** Check if flow constraints are feasible - primal approximation **/
                bool checkFlowFeasibility_v2();

                /** Check if non-overlap constraints are feasible - primal approximation **/
                bool checkOverlapFeasibility_v2();
                
                /** Check if max used slice overall constraints are feasible - primal approximation **/
                bool checkMaxUsedSliceOverallFeasibility_v2();

                /*************************************** CHECK SLACKNESS CONDITION ********************************************/

                virtual bool checkSlacknessCondition() = 0;

                bool checkLengthSlacknessCondition();

                bool checkSourceTargetSlacknessCondition();

                bool checkFlowSlacknessCondition();

                bool checkOverlapSlacknessCondition();

                bool checkMaxUsedSliceOverallSlacknessCondition();

                /*************************************************************************************************************/
                /*				                  UPDATE		      				     */
                /*************************************************************************************************************/

                /********************************************** MULTIPLIERS **************************************************/

                /* Updates lagrangian multipliers with the rule: u[k+1] = u[k] + t[k]*violation */
                virtual void updateMultiplier(double) = 0;

                /* Updates length lagrangian multipliers  */
                void updateLengthMultiplier(double);

                /* Updates source/target lagrangian multipliers  */
                void updateSourceTargetMultiplier(double);

                /* Updates flow lagrangian multipliers  */
                void updateFlowMultiplier(double);

                /* Updates non-overlap lagrangian multipliers  */
                void updateOverlapMultiplier(double);

                /* Updates one slice per demand lagrangian multipliers  */
                void updateOneSlicePerDemandMultiplier(double);

                /* Updates maximum used slice overall lagrangian multipliers  */
                void updateMaxUsedSliceOverallMultiplier(double);

                /* Updates maximum used slice overall (auxiliary) lagrangian multipliers  */
                void updateMaxUsedSliceOverallAuxMultiplier(double);

                /* Updates maximum used slice overall 2 lagrangian multipliers  */
                void updateMaxUsedSliceOverall2Multiplier(double);

                /* Updates maximum used slice overall 3 lagrangian multipliers  */
                void updateMaxUsedSliceOverall3Multiplier(double);

                /*********************************** MULTIPLIER CONSIDERING THE STABILITY CENTER *****************************/

                /* Updates lagrangian multipliers with the rule: u[k+1] = stability center[k] + t[k]*violation */
                virtual void updateMultiplier_v2(double) = 0;

                /* Updates length lagrangian multipliers  */
                void updateLengthMultiplier_v2(double);

                /* Updates source/target lagrangian multipliers  */
                void updateSourceTargetMultiplier_v2(double);

                /* Updates flow lagrangian multipliers  */
                void updateFlowMultiplier_v2(double);

                /* Updates non-overlap lagrangian multipliers  */
                void updateOverlapMultiplier_v2(double);

                /* Updates one slice per demand lagrangian multipliers  */
                void updateOneSlicePerDemandMultiplier_v2(double);

                /* Updates maximum used slice overall 1 lagrangian multipliers  */
                void updateMaxUsedSliceOverallMultiplier_v2(double);

                /* Updates maximum used slice overall 1 (auxiliary) lagrangian multipliers  */
                void updateMaxUsedSliceOverallAuxMultiplier_v2(double);

                /* Updates maximum used slice overall 2 lagrangian multipliers  */
                void updateMaxUsedSliceOverall2Multiplier_v2(double);

                /* Updates maximum used slice overall 3 lagrangian multipliers  */
                void updateMaxUsedSliceOverall3Multiplier_v2(double);

                /********************************************* STABILITY CENTER *********************************************/

                /** Updates lagrangian stabitity center **/
                virtual void updateStabilityCenter() = 0;

                /** Updates length stability center **/
                void updateLengthSC();

                /** Updates source/target stability center **/
                void updateSourceTargetSC();

                /** Updates flow stability center **/
                void updateFlowSC();

                /** Updates overlap stability center **/
                void updateOverlapSC();

                /** Updates one slice per demand stability center **/
                void updateOneSlicePerDemandSC();

                /** Updates maximum used slice overall stability center **/
                void updateMaxUsedSliceOverallSC();

                /** Updates maximum used slice overall (auxiliary) stability center **/
                void updateMaxUsedSliceOverallAuxSC();

                /** Updates maximum used slice overall 2 stability center **/
                void updateMaxUsedSliceOverall2SC();

                /** Updates maximum used slice overall 3 stability center **/
                void updateMaxUsedSliceOverall3SC();

                /************************************************** SLACK ****************************************************/

                /** Updates the slack of a lehgth constraint using the assigment matrix. **/
                void updateLengthSlack(int, const ListDigraph::Arc &);

                /** Updates the slack of a source/target constraint using the assigment matrix. **/
                void updateSourceTargetSlack(int, const ListDigraph::Arc &);

                /** Updates the slack of a flow constraint using the assigment matrix. **/
                void updateFlowSlack(int, const ListDigraph::Arc &);

                /** Updates the slack of a non-overlap constraint using the assigment matrix. **/
                void updateOverlapSlack(int, const ListDigraph::Arc &);

                /** Updates the slack of one slice constraints using the assignment matrix. **/
                void updateOneSlicePerDemandSlack(int, const ListDigraph::Arc &);

                /** Updates the slack of a maximum used slice overall constraint using the assigment matrix. **/
                void updateMaxUsedSliceOverallSlack(int, const ListDigraph::Arc &);

                void updateMaxUsedSliceOverallSlack_aux();
                void updateMaxUsedSliceOverallAuxSlack_aux();

                /** Updates the slack of a maximum used slice overall (auxiliary) constraint using the assigment matrix. **/
                void updateMaxUsedSliceOverallAuxSlack(int, const ListDigraph::Arc &);

                /** Updates the slack of a maximum used slice overall 2 constraint using the assigment matrix. **/
                void updateMaxUsedSliceOverall2Slack(int, const ListDigraph::Arc &);

                /** Updates the slack of a maximum used slice overall 3 constraint using the assigment matrix. **/
                void updateMaxUsedSliceOverall3Slack(int, const ListDigraph::Arc &);

                /**************************************** SLACK PRIMAL APPROXIMATION ******************************************/
                
                /** Updates the slack considering the primal approximation. **/
                virtual void updatePrimalSlack(double) = 0;

                /** Updates the length slack considering the primal approximation. Rule: slack_v2 = alpha*slack + (1-alpha)*slack_v2 **/
                void updateLengthPrimalSlack(double);

                /** Updates the source/target slack considering the primal approximation. Rule: slack_v2 = alpha*slack + (1-alpha)*slack_v2 **/
                void updateSourceTargetPrimalSlack(double);

                /** Updates the flow slack considering the primal approximation. Rule: slack_v2 = alpha*slack + (1-alpha)*slack_v2 **/
                void updateFlowPrimalSlack(double);

                /** Updates the overlap slack considering the primal approximation. Rule: slack_v2 = alpha*slack + (1-alpha)*slack_v2 **/
                void updateOverlapPrimalSlack(double);

                /** Updates the one slice per demand slack considering the primal approximation. Rule: slack_v2 = alpha*slack + (1-alpha)*slack_v2 **/
                void updateOneSlicePerDemandPrimalSlack(double);

                /** Updates the max used slice overall slack considering the primal approximation. Rule: slack_v2 = alpha*slack + (1-alpha)*slack_v2 **/
                void updateMaxUsedSliceOverallPrimalSlack(double);

                /** Updates the max used slice overall auxiliary slack considering the primal approximation. Rule: slack_v2 = alpha*slack + (1-alpha)*slack_v2 **/
                void updateMaxUsedSliceOverallAuxPrimalSlack(double);

                /** Updates the max used slice overall 2 slack considering the primal approximation. Rule: slack_v2 = alpha*slack + (1-alpha)*slack_v2 **/
                void updateMaxUsedSliceOverall2PrimalSlack(double);

                /** Updates the max used slice overall 3 slack considering the primal approximation. Rule: slack_v2 = alpha*slack + (1-alpha)*slack_v2 **/
                void updateMaxUsedSliceOverall3PrimalSlack(double);

                /************************************************* DIRECTION *************************************************/

                /** Updates the directions. Rule:direction[k+1] = slack[k+1] + theta*direction[k]. Theta is computed in the function
                 * according to given direction method. **/
                virtual void updateDirection() = 0;

                /** Updates the length direction **/
                void updateLengthDirection(double);

                /** Updates the source/target direction **/
                void updateSourceTargetDirection(double);

                /** Updates the flow direction **/
                void updateFlowDirection(double);

                /** Updates the non-overlap direction **/
                void updateOverlapDirection(double);

                /** Updates the one slice per demand direction **/
                void updateOneSlicePerDemandDirection(double);

                /** Updates the maximum used slice overall direction **/
                void updateMaxUsedSliceOverallDirection(double);

                /** Updates the maximum used slice overall (auxiliary) direction **/
                void updateMaxUsedSliceOverallAuxDirection(double);

                /** Updates the maximum used slice overall 2 direction **/
                void updateMaxUsedSliceOverall2Direction(double);

                /** Updates the maximum used slice overall 3 direction **/
                void updateMaxUsedSliceOverall3Direction(double);

                virtual void clearSlacks() = 0;
        
                /******************************************** PRIMAL SOLUTION *********************************************/

                /** Updates the primal approximation according to given alpha (parameter)**/
                void updatePrimalSolution(double); 

                void clearAssignmentMatrix();

                void clearBestFeasibleSolution();

                /*************************************** PRIMAL APPROXIMATION *********************************************/

                void updatePrimalApproximation(double);

                void clearPrimalApproximationMatrix();

                void changePrimalApproximation();

                void changePrimalApproximationToBestFeasibleSol();

                /*************************************** PRIMAL APPROXIMATION *********************************************/

                void changeBestSolution(std::vector<std::vector<bool>>,double);

                void changeBestSolutionWithPrimalSolution();

                /***************************************** ASSIGNMENT MATRIX **********************************************/

                /** Updates the assignment matrix to false **/
                void updateAssignment();

                /**********************************************************************************************************/
                /*					        Display						          */
                /**********************************************************************************************************/

                /** Displays the slack values **/
                virtual void displaySlack(std::ostream & = std::cout) = 0;

                /**Displays the multipliers values **/
                virtual void displayMultiplier(std::ostream & = std::cout) = 0;

                /* **********************************************************************************************************
                *                                                  DESTRUCTOR  
                ********************************************************************************************************** */
                
                virtual ~AbstractLagFormulation(){

                        lagrangianMultiplierLength.clear();
                        lagrangianMultiplierSourceTarget.clear();
                        lagrangianMultiplierFlow.clear();
                        lagrangianMultiplierOverlap.clear();
                        lagrangianMultiplierOneSlicePerDemand.clear();
                        lagrangianMultiplierMaxUsedSliceOverall.clear();
                        lagrangianMultiplierMaxUsedSliceOverallAux.clear();
                        lagrangianMultiplierMaxUsedSliceOverall2.clear();
                        lagrangianMultiplierMaxUsedSliceOverall3.clear();

                        lengthSlack.clear();
                        sourceTargetSlack.clear();
                        flowSlack.clear();
                        overlapSlack.clear();
                        oneSlicePerDemandSlack.clear();
                        maxUsedSliceOverallSlack.clear();
                        maxUsedSliceOverallAuxSlack.clear();
                        maxUsedSliceOverallSlack2.clear();
                        maxUsedSliceOverallSlack3.clear();

                        lengthSlack_v2.clear();
                        sourceTargetSlack_v2.clear();
                        flowSlack_v2.clear();
                        overlapSlack_v2.clear();
                        oneSlicePerDemandSlack_v2.clear();
                        maxUsedSliceOverallSlack_v2.clear();
                        maxUsedSliceOverallAuxSlack_v2.clear();
                        maxUsedSliceOverallSlack2_v2.clear();
                        maxUsedSliceOverallSlack3_v2.clear();

                        lagrangianSCLength.clear();
                        lagrangianSCSourceTarget.clear();
                        lagrangianSCFlow.clear();
                        lagrangianSCOverlap.clear();
                        lagrangianSCOneSlicePerDemand.clear();
                        lagrangianSCMaxUsedSliceOverall.clear();
                        lagrangianSCMaxUsedSliceOverallAux.clear();
                        lagrangianSCMaxUsedSliceOverall2.clear();
                        lagrangianSCMaxUsedSliceOverall3.clear();

                        lengthDirection.clear();
                        sourceTargetDirection.clear();
                        flowDirection.clear();
                        overlapDirection.clear();
                        oneSlicePerDemandDirection.clear();
                        maxUsedSliceOverallDirection.clear();
                        maxUsedSliceOverallAuxDirection.clear();
                        maxUsedSliceOverall2Direction.clear();
                        maxUsedSliceOverall3Direction.clear();

                        assignmentMatrix_d.clear();
                        primal_linear_solution.clear();

                        coeff.clear();
                        mapItLabel.clear();
                }
};

#endif


