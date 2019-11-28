#ifndef __EVENT_ACTION_OPT_PH_HH__
#define __EVENT_ACTION_OPT_PH_HH__

#include <G4UserEventAction.hh>

#include "AnalysisManager.hh"

class G4Event;

class EventActionOptPh : public G4UserEventAction
{
public:
	EventActionOptPh(AnalysisManagerOptPh *pAnalysisManager = NULL);
	virtual ~EventActionOptPh();

public:
	void BeginOfEventAction(const G4Event *pEvent);
	void EndOfEventAction(const G4Event *pEvent);

private:
	AnalysisManagerOptPh *fAnalysisManager;
	G4int fPmtHitsCollectionID;
};

#endif

