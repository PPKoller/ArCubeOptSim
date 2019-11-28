#ifndef __ANALYSIS_MANAGER_OPT_PH_HH__
#define __ANALYSIS_MANAGER_OPT_PH_HH__

#include "globals.hh"

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TParameter.h"
#include "TDirectory.h"


#include <set>
#include <vector>


class G4Run;
class G4Event;
class G4Step;

class EventDataOptPh;
class PrimaryGeneratorActionOptPh;
class AnalysisOptPhMessenger;


using std::vector;
using std::stringstream;
using std::set;

class AnalysisManagerOptPh
{
public:
	AnalysisManagerOptPh(PrimaryGeneratorActionOptPh *pPrimaryGeneratorAction);
	virtual ~AnalysisManagerOptPh();

public:
	void BeginOfRun(const G4Run *pRun); 
	void EndOfRun(const G4Run *pRun); 
	void BeginOfEvent(const G4Event *pEvent); 
	void EndOfEvent(const G4Event *pEvent); 
	void Step(const G4Step *pStep);	
	
	void DefineOptPhSensDet(G4String volList);
	void SetSaveData(G4int save=1){ fSave=save; };
	void SetStepsDebug(G4bool dbg=true){ fStepsDebug=dbg; };
	void SetDataFilename(const G4String &hFilename) { fDataFilename = hFilename; };
	void SetNbEventsToSimulate(G4int iNbEventsToSimulate) { fNbEventsToSimulate = iNbEventsToSimulate; };
	void SetNbOfPrimariesPerEvent(G4int nPrim){ fNbPrim = nPrim; };
	virtual void SetVerbosity(G4int verb){fVerbose=verb;};
	virtual void SetPrintModulo(G4int modulo){fPrintModulo=modulo;};
	
	G4String GetDataFilename() const { return fDataFilename; };
	G4int GetNbEventsToSimulate() const { return fNbEventsToSimulate; };
	G4int GetNbOfPrimariesPerEvent() const { return fNbPrim; };

private:
	G4int fSave;
	G4bool fStepsDebug;
	G4int fVerbose;
	G4int fPrintModulo;

	G4int fHitsCollectionID;

	G4String fDataFilename;
	G4int fNbEventsToSimulate, fNbPrim;
	
	TFile *fTreeFile;
	TTree *fTree;
	
	G4int fAutoSaveEvs, fAutoFlushEvs;
	
	PrimaryGeneratorActionOptPh *fPrimaryGeneratorAction;
	AnalysisOptPhMessenger *fMessenger;
	EventDataOptPh *fEventData;
	
	set<G4String> fOptPhSenDetVolNames;
};

#endif

