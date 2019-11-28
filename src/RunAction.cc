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
	if(fRanSeed > 0){
	    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
	    G4cout << "RunActionOptPh::BeginOfRunAction Initialize random numbers with seed = "<<fRanSeed<<G4endl;
	    CLHEP::HepRandom::setTheSeed(fRanSeed);
	}else{
	    // initialize with time.....
	    struct timeval hTimeValue;
	    gettimeofday(&hTimeValue, NULL);
	    G4cout << "RunActionOptPh::BeginOfRunAction Initialize random numbers with seed = "<<hTimeValue.tv_usec<<G4endl;
	    CLHEP::HepRandom::setTheSeed(hTimeValue.tv_usec);
	}
	
	if(fAnalysisManager) fAnalysisManager->BeginOfRun(pRun);
}

void RunActionOptPh::EndOfRunAction(const G4Run *pRun)
{
	if(fAnalysisManager) fAnalysisManager->EndOfRun(pRun);
}

