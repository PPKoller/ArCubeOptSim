#include "AnalysisManager.hh"
#include "RunAction.hh"

#include "Randomize.hh"

#include <sys/time.h>



RunActionOptPh::RunActionOptPh(AnalysisManagerOptPh *pAnalysisManager)
{
	fAnalysisManager = pAnalysisManager;
	fRanSeed=0;
}

RunActionOptPh::~RunActionOptPh()
{

}

void RunActionOptPh::BeginOfRunAction(const G4Run *pRun)
{
	if(fAnalysisManager) fAnalysisManager->BeginOfRun(pRun);
}

void RunActionOptPh::EndOfRunAction(const G4Run *pRun)
{
	if(fAnalysisManager) fAnalysisManager->EndOfRun(pRun);
}

