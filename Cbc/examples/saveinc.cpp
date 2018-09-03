#include <cstdio>
#include <cstdlib>
#include <OsiCbcSolverInterface.hpp>
#include <CbcModel.hpp>
#include <CglPreProcess.hpp>
#include <CbcSolver.hpp>
#include <Cbc_C_Interface.h>
#include "CbcEventHandler.hpp"

static int callBack(CbcModel * model, int whereFrom)
{/*
    OsiSolverInterface *solver = model->solver();

    printf("called code %d\n", whereFrom); fflush(stdout);

    if (whereFrom==6)
    {
    char str[256] = "original";
    if (solver->getPreProcessor())
    {
    strcpy(str, "preprocessed");
    }
    printf("\n< SOLUTION FOUND model is %s >\n", str);
    fflush( stdout );
    }*/

    return 0;
}

class SolHandler : public CbcEventHandler {
    public:
        virtual CbcAction event(CbcEvent whichEvent);
        SolHandler();
        SolHandler(CbcModel * model);
        virtual ~SolHandler();
        SolHandler(const SolHandler & rhs);
        SolHandler& operator=(const SolHandler& rhs);
        virtual CbcEventHandler *clone() const ;
};

SolHandler::SolHandler () 
    : CbcEventHandler()
{
}

SolHandler::SolHandler (const SolHandler & rhs) 
    : CbcEventHandler(rhs)
{  
}

SolHandler::SolHandler(CbcModel * model)
    : CbcEventHandler(model)
{
}

SolHandler::~SolHandler ()
{
}

SolHandler &SolHandler::operator=(const SolHandler& rhs)
{
    if (this != &rhs) {
        CbcEventHandler::operator=(rhs);
    }
    return *this;
}

CbcEventHandler * SolHandler::clone() const
{
    return new SolHandler(*this);
}

CbcEventHandler::CbcAction SolHandler::event(CbcEvent whichEvent)
{
    // If in sub tree carry on
    if (!model_->parentModel()) 
    {
        if (whichEvent==solution||whichEvent==heuristicSolution) 
        {
            char str[256] = " NOT preproc";
            OsiSolverInterface *solver = model_->solver();
            if (solver->getPreProcessor())
            {
                CglPreProcess *cgp = (CglPreProcess *)solver->getPreProcessor();
                OsiSolverInterface *orig = cgp->originalModel();
                strcpy( str, " PREPROC ");
                char strp[256] = "";
                sprintf(strp, " vars from %d to %d", orig->getNumCols(), solver->getNumCols());
                strcpy( str, strp );
            }


            printf("EVH : %s\n", str); fflush(stdout);
        }
   }

    return noAction;
}



int main( int argc, char **argv )
{
    if (argc<2)
    {
        fprintf( stderr, "enter instance name\n" );
        exit( EXIT_FAILURE );
    }

    CbcSolverUsefulData cbcData;
    cbcData.noPrinting_ = false;
    OsiClpSolverInterface lp;

    lp.readMps(argv[1]);

    CbcModel model(lp);

    SolHandler sh;
    model.passInEventHandler( &sh );

    CbcMain0( model, cbcData );
    CbcMain1(argc-1, (const char **)(argv+1), model, callBack, cbcData );



    return 0;
}
