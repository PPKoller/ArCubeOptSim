#include "DetConstr.hh"
#include "PrimGenAction.hh"
#include "AnalysisManager.hh"
#include "AnalysisMessenger.hh"
//#include "OptPhHit.hh"
#include "EventData.hh"

#include "G4SDManager.hh"
#include "G4Run.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4TrackStatus.hh"
#include "G4StepStatus.hh"
#include "G4VProcess.hh"
#include "G4PhysicalVolumeStore.hh"

#include <numeric>
#include <fstream>
#include <sstream>


using std::vector;
using std::stringstream;
using std::set;
using std::ofstream;


AnalysisManagerOptPh::AnalysisManagerOptPh(PrimaryGeneratorActionOptPh *pPrimaryGeneratorAction)
{
	fMessenger = new AnalysisOptPhMessenger(this);
	
	fVerbose=0;
	fPrintModulo=0;
	
	fPrimaryGeneratorAction = pPrimaryGeneratorAction;
	
	fSave=0;
	
	//fPmtHitsCollectionID = -1;
	
	fDataFilename = "events.root";
	
	fEventData = new EventDataOptPh();
	
	fTreeFile=NULL;
	fTree=NULL;
	
	fAutoSaveEvs = 10000;
	fAutoFlushEvs = 10000;
}

AnalysisManagerOptPh::~AnalysisManagerOptPh()
{
	if(fEventData) delete fEventData;
	delete fMessenger;
}



void AnalysisManagerOptPh::BeginOfRun(const G4Run *pRun)
{
	//G4cout << "\nEntering in AnalysisManagerOptPh::BeginOfRun(...)" << G4endl;
	
	if(fSave==0){
		G4cout << "\nWARNING AnalysisManagerOptPh::BeginOfRun(...) ---> AnalysisManagerOptPh::BeginOfRun(...): Data will not be saved." << G4endl;
		return;
	}
	
	fTreeFile = new TFile(fDataFilename.c_str(), "RECREATE", "File containing event data of optical photon simulations of ArgonCube");
	
	if(!fTreeFile){//Here there is a problem
		fSave = 0; //Do this to save from application crashing
		return;
	}
	
	TParameter<int>* ptTParNbEventsToSimulate = new TParameter<int>("EventsNb", fNbEventsToSimulate);
	
	fNbPrim = fPrimaryGeneratorAction->GetPrimNb();
	TParameter<int>* ptParNbPrimaryPhotPerEvent = new TParameter<int>("PrimNb", fNbPrim);
	
	if(fTreeFile) fTreeFile->WriteTObject(ptTParNbEventsToSimulate, 0, "overwrite");
	if(fTreeFile) fTreeFile->WriteTObject(ptParNbPrimaryPhotPerEvent, 0, "overwrite");
	
	delete ptTParNbEventsToSimulate;
	delete ptParNbPrimaryPhotPerEvent;
	
	
	
	fTree = new TTree("t1", "Tree containing event data for ArgonCube optical photon simulations.");

	gROOT->ProcessLine("#include <vector>");
	gROOT->ProcessLine("#include <string>");

	fTree->Branch("eventid", &fEventData->m_iEventId, "eventid/I");
	
	//Primary particle savings
	//if(!fLite) fTree->Branch("type_pri", "vector<string>", &fEventData->m_pPrimaryParticleType);
	fTree->Branch("prim_vol", "vector<string>", &fEventData->m_pPrimaryVolume);
	fTree->Branch("prim_xpos", &fEventData->m_fPrimary_posX, "prim_xpos/D");
	fTree->Branch("prim_ypos", &fEventData->m_fPrimary_posY, "prim_ypos/D");
	fTree->Branch("prim_zpos", &fEventData->m_fPrimary_posZ, "prim_zpos/D");
	if(fSave>1) fTree->Branch("prim_xmom", "vector<double>", &fEventData->m_fPrimary_momX);
	if(fSave>1) fTree->Branch("prim_ymom", "vector<double>", &fEventData->m_fPrimary_momY);
	if(fSave>1) fTree->Branch("prim_zmom", "vector<double>", &fEventData->m_fPrimary_momZ);
	if(fSave>1) fTree->Branch("prim_xpol", "vector<double>", &fEventData->m_fPrimary_polX);
	if(fSave>1) fTree->Branch("prim_ypol", "vector<double>", &fEventData->m_fPrimary_polY);
	if(fSave>1) fTree->Branch("prim_zpol", "vector<double>", &fEventData->m_fPrimary_polZ);
	
	
	//Hits realated savings (steps)
	//fTree->Branch("ntpmthits", &fEventData->m_iNbTopPmtHits, "ntpmthits/I");
	//fTree->Branch("nbpmthits", &fEventData->m_iNbBottomPmtHits, "nbpmthits/I");
	fTree->Branch("totalhits", &fEventData->m_iNbTotHits, "totalhits/I");
	//fTree->Branch("etot", &fEventData->m_dTotalDepEn, "etot/D");
	
	fTree->Branch("Px", "vector<double>", &fEventData->fPx);
	fTree->Branch("Py", "vector<double>", &fEventData->fPy);
	fTree->Branch("Pz", "vector<double>", &fEventData->fPz);
	
	//fTree->Branch("HitVol", "vector<int>", &fEventData->fHitVol);
	
	fTree->Branch("trackid", "vector<int>", &fEventData->m_pTrackId);
	//if(fSave>1) fTree->Branch("parttype", "vector<string>", &fEventData->m_pParticleType);
	//if(fSave>1) fTree->Branch("parentid", "vector<int>", &fEventData->m_pParentId);
	//if(fSave>1) fTree->Branch("parenttype", "vector<string>", &fEventData->m_pParentType);
	//if(fSave>1) fTree->Branch("creaproc", "vector<string>", &fEventData->m_pCreatorProcess);
	if(fSave>1) fTree->Branch("deposproc", "vector<string>", &fEventData->m_pDepositingProcess);
	//if(fSave>1) fTree->Branch("physvol", "vector<string>", &fEventData->m_pPhysVolName);
	if(fSave>1) fTree->Branch("time", "vector<double>", &fEventData->m_pTime);
	if(fSave>1) fTree->Branch("xpos", "vector<double>", &fEventData->fX);
	if(fSave>1) fTree->Branch("ypos", "vector<double>", &fEventData->fY);
	if(fSave>1) fTree->Branch("zpos", "vector<double>", &fEventData->fZ);
	//if(fSave>1) fTree->Branch("edep", "vector<double>", &fEventData->m_pEnergyDeposited);
	
	
	//fTree->SetMaxTreeSize((int)1e6);
	fTree->SetAutoFlush(fAutoFlushEvs);
	fTree->SetAutoSave(fAutoSaveEvs);
	
	
	
	
	
	
	//G4cout << "\nExiting from AnalysisManagerOptPh::BeginOfRun(...)" << G4endl;
}


void AnalysisManagerOptPh::EndOfRun(const G4Run *pRun)
{
	if(fSave==0) return;
	
	if(fTreeFile){
		if(fTree){
			fTreeFile->WriteTObject(fTree, 0, "overwrite");
			delete fTreeFile;
			fTree=NULL;
		}
		fTreeFile=NULL;
	}
}


void AnalysisManagerOptPh::BeginOfEvent(const G4Event *pEvent)
{
	
	// grab event ID
	G4int event_id = pEvent->GetEventID();
	
	// print this information event by event (modulo n)  	
	if( (fVerbose>0) && (fPrintModulo>0) && (event_id%fPrintModulo == 0) ){
		G4cout << "\nInfo --> AnalysisManagerOptPh::BeginOfEvent(...): Begin of event: " << event_id  << G4endl;
	}
	
	/*
	if(fHitsCollectionID == -1){
		G4SDManager *pSDManager = G4SDManager::GetSDMpointer();
		fHitsCollectionID = pSDManager->GetCollectionID("OptPhHitsCollection");
	}
	*/
	
	fEventData->Reset();
}


void AnalysisManagerOptPh::EndOfEvent(const G4Event *pEvent)
{
	//G4HCofThisEvent* pHCofThisEvent = pEvent->GetHCofThisEvent();
	//OptPhHitsCollection* pHitsCollection = NULL;
	
	/*
	//Primary particles information
	G4ThreeVector posVec = fPrimaryGeneratorAction->GetPrimPos();
	vector<G4ParticleMomentum> momVec = fPrimaryGeneratorAction->GetPrimMom();
	vector<G4ThreeVector> polVec = fPrimaryGeneratorAction->GetPrimPol();
	
	
	G4int iNbHits = 0;
	
	if(pHCofThisEvent){
		if(fPmtHitsCollectionID != -1){
			pPmtHitsCollection = (HitsCollection *)(pHCofThisEvent->GetHC(fHitsCollectionID));
			if(pPmtHitsCollection){
				iNbHits = pPmtHitsCollection->entries();
			}
		}
	}
	
	
	if(fVerbose>1){
		if(fPrintModulo>0){
			if (event_id%fPrintModulo == 0){
				G4cout << "----> Number of primaries: " << posVec.size() << G4endl;
			}
		}else{
			G4cout << "---> Event " << event_id << ". Number of primaries: " << posVec.size() << G4endl;
		}
	}
	
	
	if(fVerbose>0){
		if(!pHitsCollection){
			G4cout << "\nWARNING ---> AnalysisManagerOptPh::EndOfEvent(...): the pointer <pPmtHitsCollection> is 0!" << endl;
			G4cout << "Value of <fPmtHitsCollectionID>: " << fPmtHitsCollectionID << G4endl;
		}
		
		if(fPrintModulo>0){
			if (event_id%fPrintModulo == 0){
				G4cout << "Info ---> AnalysisManagerOptPh::EndOfEvent(...): Total number of hits: " << iNbPmtHits << G4endl;
			}
		}else{
			G4cout << "Info ---> AnalysisManagerOptPh::EndOfEvent(...): Event " << event_id << ". Total number of hits: " << iNbHits << G4endl;
		}
	}
	
	
	
	if(fSave==0) return;
	
	fEventData->m_fPrimary_posX = posVec.x();
	fEventData->m_fPrimary_posY = posVec.y();
	fEventData->m_fPrimary_posZ = posVec.z();
	
	if(fSave>1){
		for(unsigned iEnt=0; iEnt<momVec.size(); iEnt++){
			fEventData->m_fPrimary_momX->push_back( momVec.at(iEnt).x() );
			fEventData->m_fPrimary_momY->push_back( momVec.at(iEnt).y() );
			fEventData->m_fPrimary_momZ->push_back( momVec.at(iEnt).z() );
		}
	
		for(unsigned iEnt=0; iEnt<polVec.size(); iEnt++){
			fEventData->m_fPrimary_polX->push_back( polVec.at(iEnt).x() );
			fEventData->m_fPrimary_polY->push_back( polVec.at(iEnt).y() );
			fEventData->m_fPrimary_polZ->push_back( polVec.at(iEnt).z() );
		}
	}
	
	
	//Hits information
	fEventData->m_iEventId = event_id;
	fEventData->m_iNbTotHits = iNbHits;
	
	fEventData->m_pTrackId->resize(iNbHits);
	
	if(fSave>1){
		fEventData->m_pParticleType->resize(iNbHits);
		fEventData->m_pParticleType->resize(iNbHits);
		fEventData->m_pParentId->resize(iNbHits);
		fEventData->m_pParentType->resize(iNbHits);
		
		fEventData->m_pPhysVolName->resize(iNbHits);
		
		fEventData->m_pTime->resize(iNbHits);
		
		fEventData->fX->resize(iNbHits);
		fEventData->fY->resize(iNbHits);
		fEventData->fZ->resize(iNbHits);
		
		fEventData->m_pEnergyDeposited->resize(iNbHits);
	}
	
	fEventData->fPx->resize(iNbHits);
	fEventData->fPy->resize(iNbHits);
	fEventData->fPz->resize(iNbHits);
	
	fEventData->fHitVol->resize(iNbHits);
	
	//This container is just to check whether a track has more than 1 hit (bad!)
	std::set<G4int> tracksContainer;
	
	//Calculate how many hits are in the top PMT and how many in the bottom
	for(G4int iH=0; iH<iNbPmtHits; iH++){
		OptPhHit *pHit = (OptPhHit*)pHitsCollection->GetHit(iH);
		
		G4int trID = pHit->GetTrackID();
		if(tracksContainer.find(trID)==tracksContainer.end()){
			tracksContainer.insert(trID);
		}else{
			G4cout << "WARNING ---> AnalysisManagerOptPh::EndOfEvent(...):" << " Event " << event_id << ". The track " << trID << " already had an hit!" << endl;
		}
		
		if(fSave>1) fEventData->m_pTrackId->at(iH) = trID;
		//if(fSave>1) fEventData->m_pParticleType->at(iH) = pHit->GetPartType();
		
		//if(fSave>1) fEventData->m_pParentId->at(iH) = pHit->GetParentTrackID();
		//if(fSave>1) fEventData->m_pParentType->at(iH) = pHit->GetParentType();
		
		//if(fSave>1) fEventData->m_pCreatorProcess->at(iH) = pHit->GetCreatProc();
		if(fSave>1) fEventData->m_pDepositingProcess->at(iH) = pHit->GetDepProc();
		
		G4String VolName = "";
		
		if(pHit->GetPhysVol()){
			VolName = pHit->GetPhysVol()->GetName();
		}
		if(fSave>1) fEventData->m_pPhysVolName->at(iH)=VolName;
		
		
		fEventData->m_pTime->at(iH) = pHit->GetTime();
		
		if(fSave>1) fEventData->fX->at(iH) = (pHit->GetHitPos()).x();
		if(fSave>1) fEventData->fY->at(iH) = (pHit->GetHitPos()).y();
		if(fSave>1) fEventData->fZ->at(iH) = (pHit->GetHitPos()).z();
		
		if(fSave>1) fEventData->m_pEnergyDeposited->at(iH) = pHit->GetEdep();
		fEventData->m_dTotalDepEn += pHit->GetEdep();
		
		fEventData->fPx->at(iH) = (pHit->GetHitMom()).x();
		fEventData->fPy->at(iH) = (pHit->GetHitMom()).y();
		fEventData->fPz->at(iH) = (pHit->GetHitMom()).z();
		
		if(fVerbose>2){
			if(fPrintModulo>0){
				if (event_id%fPrintModulo == 0){
					G4cout << "Debug ---> AnalysisManagerOptPh::EndOfEvent(...): Saved Hit " << iH << G4endl;
				}
			}else{
				G4cout << "Debug ---> AnalysisManagerOptPh::EndOfEvent(...): Event " << event_id << ". Saved Hit " << iH << G4endl;
			}
		}
	}//End of the loop over the hits
	
	
	if(fTree) fTree->Fill();
	*/
}


void AnalysisManagerOptPh::Step(const G4Step *pStep)
{
	//G4cout << "\nEntering in AnalysisManagerOptPh::Step\n" << G4endl;
	
	if( (fVerbose<3) && (fOptPhSenDetVolNames.empty()) )return;
	//if(!pStep) return; //This just avoids problems
	
	G4Track *track = pStep->GetTrack();
	//if(!track) return; //This just avoids problems
	
	G4String partname = track->GetDefinition()->GetParticleName();
	if(partname!=G4String("opticalphoton")) return;
	
	G4StepPoint *point1 = pStep->GetPreStepPoint();
	G4StepPoint *point2 = pStep->GetPostStepPoint();
	if(!(point1 && point2)) return;
	
	/*
	G4TouchableHandle touch1 = point1->GetTouchableHandle();
	G4TouchableHandle touch2 = point2->GetTouchableHandle();
	
	G4String Vol1 = "None";
	G4String Vol2 = "None";
	
	if(touch1->GetVolume()){
		Vol1 = touch1->GetVolume()->GetName(); 
	}
	if(touch2->GetVolume()){
		Vol2 = touch2->GetVolume()->GetName(); 
	}
	
	G4String TrackStat;
	
	//Check whether the particle is in one of the sensitive volumes
	if((Vol2==G4String("TopPMTPhotocathode"))||(Vol2==G4String("BottomPMTPhotocathode"))){
		
		if(fVerbose>2){
			if(point2->GetStepStatus()==fGeomBoundary){
				G4cout << "\nDebug ---> AnalysisManagerOptPh::Step(...): " << "Optical photon at volumes boundary. Entering into volume \"" << Vol2 << "\" from volume \"" << Vol1 << "." << G4endl;
			}else{
				G4cout << "\nStep Debug ---> " << "Optical photon post step point in volume \"" << Vol2 << "\"" << G4endl;
			}
			
			G4TrackStatus trstatus = track->GetTrackStatus();
			if(trstatus==fAlive) TrackStat = "Alive";
			if(trstatus==fStopButAlive) TrackStat = "StopButAlive";
			if(trstatus==fStopAndKill) TrackStat = "StopAndKill";
			if(trstatus==fKillTrackAndSecondaries) TrackStat = "KillTrackAndSecondaries";
			if(trstatus==fSuspend) TrackStat = "Suspend";
			if(trstatus==fPostponeToNextEvent) TrackStat = "PostponeToNextEvent";
		
			G4cout << "Debug ---> AnalysisManagerOptPh::Step(...): " << "Status of optical photon particle: " << TrackStat << G4endl;
			G4cout << "Debug ---> AnalysisManagerOptPh::Step(...): " << "Process selected in this step: " << point2->GetProcessDefinedStep()->GetProcessName() << G4endl;
		}
		
		
		
	}
	
	if((Vol1==G4String("TopPMTPhotocathode"))||(Vol1==G4String("BottomPMTPhotocathode"))){
		
		if(fVerbose>2){
			if(point1->GetStepStatus()==fGeomBoundary){
				G4cout << "\nStep Debug ---> " << "Optical photon at volumes surfaces. First step into volume \"" << Vol1 << G4endl;
			}else{
				G4cout << "\nStep Debug ---> " << "Optical photon post step point in volume \"" << Vol2 << "\"" << G4endl;
			}
			
			G4TrackStatus trstatus = track->GetTrackStatus();
			
			if(trstatus==fAlive) TrackStat = "Alive";
			if(trstatus==fStopButAlive) TrackStat = "StopButAlive";
			if(trstatus==fStopAndKill) TrackStat = "StopAndKill";
			if(trstatus==fKillTrackAndSecondaries) TrackStat = "KillTrackAndSecondaries";
			if(trstatus==fSuspend) TrackStat = "Suspend";
			if(trstatus==fPostponeToNextEvent) TrackStat = "PostponeToNextEvent";
		
			G4cout << "Step Debug ---> " << "Status of optical photon particle: " << TrackStat << G4endl;
			G4cout << "Step Debug ---> " << "Process selected in this step: " << point2->GetProcessDefinedStep()->GetProcessName() << G4endl;
		}
		
	}
	*/
	
	
	
	//G4cout << "\nExiting from AnalysisManagerOptPh::Step\n" << G4endl;
	
}



//Service methods
void AnalysisManagerOptPh::DefineOptPhSensDet(G4String volList)
{
	if(volList == G4String("NULL")){
		fOptPhSenDetVolNames.clear();
		return;
	}
	
	stringstream hStream;
	hStream.str(volList);
	G4String hVolumeName;
	
	
	// store all the volume names
	set<G4String> candidatevolnames;
	while(!hStream.eof()){
		hStream >> hVolumeName;
		candidatevolnames.insert(hVolumeName);
	}
	
	
	// checks if the selected volumes exist and store all volumes that match
	G4PhysicalVolumeStore *PVStore = G4PhysicalVolumeStore::GetInstance();
	
	for(set<G4String>::iterator pIt = candidatevolnames.begin(); pIt != candidatevolnames.end(); pIt++){
		G4String hRequiredVolumeName = *pIt;
		G4bool bMatch = false;
		
		if(bMatch = (hRequiredVolumeName.last('*') != std::string::npos)) hRequiredVolumeName = hRequiredVolumeName.strip(G4String::trailing, '*');
		
		for(G4int iIndex = 0; iIndex < (G4int) PVStore->size(); iIndex++){
			G4String hName = (*PVStore)[iIndex]->GetName();
			
			if( (hName == hRequiredVolumeName) || (bMatch && (hName.substr(0, hRequiredVolumeName.size())) == hRequiredVolumeName) ){
				fOptPhSenDetVolNames.insert(hName);
			}
		}
	}
	
	
	
}

