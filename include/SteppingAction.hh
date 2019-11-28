#ifndef __STEPPING_ACTION_OPT_PH_HH__
#define __STEPPING_ACTION_OPT_PH_HH__

#include <globals.hh>
#include <G4UserSteppingAction.hh>


class AnalysisManagerOptPh;

class SteppingActionOptPh: public G4UserSteppingAction
{
public:
	SteppingActionOptPh(AnalysisManagerOptPh *pAnalysisManager=0);
	virtual ~SteppingActionOptPh();
	
	void UserSteppingAction(const G4Step* aStep);

private:
	AnalysisManagerOptPh *fAnalysisManager;
};

#endif

