#ifndef __EventhdlrNonOverlapping__h
#define __EventhdlrNonOverlapping__h

#include "objscip/objscip.h"
#include "abstractSolver.h"
#include "solverSCIP.h"
#include "../formulation/tFlowForm.h"

/** C++ wrapper object for event handlers */
class EventhdlrNonOverlapping : public scip::ObjEventhdlr{

    private:
    AbstractFormulation *formulation;
    SolverSCIP *solver;

    public:

    /** default constructor */
    EventhdlrNonOverlapping(SCIP* scip, AbstractFormulation* &_formulation, SolverSCIP* _solver) : ObjEventhdlr(scip, "nonoverlapping","event handler for non overlapping constraint in T-flow formulation"), formulation(_formulation), solver(_solver){}

    /** destructor */
    virtual ~EventhdlrNonOverlapping(){}

    virtual SCIP_DECL_EVENTINITSOL(scip_initsol);

    virtual SCIP_DECL_EVENTEXITSOL(scip_exitsol);

    virtual SCIP_DECL_EVENTEXEC(scip_exec);


};

#endif