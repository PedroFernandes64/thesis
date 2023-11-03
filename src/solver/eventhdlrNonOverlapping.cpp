#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>

#include "objscip/objscip.h"
#include "eventhdlrNonOverlapping.h"

/** solving process initialization method of event handler (called when branch and bound process is about to begin)
 *
 *  This method is called when the presolving was finished and the branch and bound process is about to begin.
 *  The event handler may use this call to initialize its branch and bound specific data.
 *
 */
SCIP_DECL_EVENTINITSOL(EventhdlrNonOverlapping::scip_initsol)
{
   SCIP_CALL( SCIPcatchEvent( scip, SCIP_EVENTTYPE_SOLFOUND, eventhdlr, NULL, NULL) );

   return SCIP_OKAY;
}

/** solving process deinitialization method of event handler (called before branch and bound process data is freed)
 *
 *  This method is called before the branch and bound process is freed.
 *  The event handler should use this call to clean up its branch and bound data.
 */
SCIP_DECL_EVENTEXITSOL(EventhdlrNonOverlapping::scip_exitsol)
{
   SCIP_CALL( SCIPdropEvent( scip, SCIP_EVENTTYPE_SOLFOUND, eventhdlr, NULL, -1) );

   return SCIP_OKAY;
}

SCIP_DECL_EVENTEXEC(EventhdlrNonOverlapping::scip_exec){
   /*std::cout << std::endl << std::endl << std::endl << "HEY HEY HEY HEY HEY HEY HEY HEY HEY HEY HEY HEY HEY HEY HEY HEY HEY" << std::endl << std::endl << std::endl << std::endl;
   std::cout << SCIPgetNConss	(	scip	)	;

   formulation->updatePath(solver->getIntermediateSolution());
   //formulation->displayPaths();
   if(formulation->hasOverlapping()){
      int nVars = formulation->getOverlappingIds().size();
      std::cout << std::endl << nVars << std::endl;
      std::cout << "overlapinho" << std::endl;
      formulation->displayPaths();
      std::string consName = "NonOverlapping";
      VarArray v = formulation->getVariables();

      SCIP_CONS * cons;
      std::cout << "TAR" << std::endl;
      SCIPcreateConsLinear(scip, & cons, consName.c_str(), 
                           0, NULL, NULL, 0, nVars - 1, 
                           TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE);
      std::cout << "TUR" << std::endl;
      for(int i = 0; i < nVars; i++){
         for(unsigned int k = 0; k < v.size(); k++){
            if(v[k].getId() == formulation->getOverlappingIds()[i]){
               SCIPaddCoefLinear(scip, cons, solver->getVars()[k], 1); 
               std::cout << std::endl << "var name" << SCIPvarGetName(solver->getVars()[k]) << std::endl;
            }  
         }
      }
      SCIPaddCons(scip, cons);
      SCIPmarkConsPropagate(scip, cons);
      SCIPrepropagateNode(scip, SCIPgetRootNode(scip));
      std::cout << SCIPgetNConss	(	scip	)	;
   }
   */
   return SCIP_OKAY;
}