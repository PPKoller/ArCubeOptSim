#include "AnalysisMessenger.hh"
#include "AnalysisManager.hh"

#include <set>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>


AnalysisOptPhMessenger::AnalysisOptPhMessenger(AnalysisManagerOptPh *pAnManager)
{ 
	std::stringstream txt; txt.str("");
	
	fAnManager = pAnManager;
	
	fAnalysisDir = new G4UIdirectory("/argoncube/analysis/");
	fAnalysisDir->SetGuidance("ArgonCube analysis manager settings.");
	
	
	fVerboseCmd = new G4UIcmdWithAnInteger("/argoncube/analysis/verbose",this);
	fVerboseCmd->SetGuidance("Set verbosity of the analysis manager");
	fVerboseCmd->SetGuidance(" Default 1");
	fVerboseCmd->SetParameterName("Verb", false);
	fVerboseCmd->SetDefaultValue(1);
	txt << "Verb>=0 && Verb<=" << AnalysisManagerOptPh::kDebug;
	fVerboseCmd->SetRange( txt.str().c_str() );
	fVerboseCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	fPrintModuloCmd = new G4UIcmdWithAnInteger("/argoncube/analysis/PrintModulo",this);
	fPrintModuloCmd->SetGuidance("Prints the start of event every \"PrMod\"");
	fPrintModuloCmd->SetParameterName("PrMod", false);
	fPrintModuloCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fStepDebugCmd = new G4UIcmdWithABool("/argoncube/analysis/stepsDebug", this);
	fStepDebugCmd->SetGuidance("Activates debugging controls and messages at step level.");
	fStepDebugCmd->SetParameterName("DebugSteps", false);
	fStepDebugCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	fSaveDataCmd = new G4UIcmdWithAnInteger("/argoncube/analysis/SaveData", this);
	fSaveDataCmd->SetGuidance("Control for data tree saving: 0 to not save; 1 standard analysis variables (default); 2 extended tracking info for debugging (slow and heavy)");
	fSaveDataCmd->SetParameterName("Save", false);
	fSaveDataCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	fFileNameCmd = new G4UIcmdWithAString("/argoncube/analysis/FileName",this);
	fFileNameCmd->SetGuidance("Set the file name where data tree will be saved.");
	fFileNameCmd->SetParameterName("filename", false);
	fFileNameCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	// Define optical sensitive volumes
	fDefOptSDCmd = new G4UIcmdWithAString("/argoncube/analysis/DefOptSD", this);
	fDefOptSDCmd->SetGuidance("Defines a list of physical volume as sensitive (NULL to unset).");
	fDefOptSDCmd->SetGuidance("DefOptSD: sensdet VolName1 VolName2 ...");
	fDefOptSDCmd->SetParameterName("VolName", true, true);
	fDefOptSDCmd->SetDefaultValue("NULL");
	fDefOptSDCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	// Autoflush and autosave for data tree
	fAutoFlushCmd = new G4UIcmdWithAnInteger("/argoncube/analysis/SetAutoFlush",this);
	fAutoFlushCmd->SetGuidance("Autoflush settings of the data TTree (see ROOT reference guide for more).");
	fAutoFlushCmd->SetParameterName("AutoFlush", false, false);
	fAutoFlushCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	fAutoSaveCmd = new G4UIcmdWithAnInteger("/argoncube/analysis/SetAutoSave",this);
	fAutoSaveCmd->SetGuidance("Autosave settings of the data TTree (see ROOT reference guide for more).");
	fAutoSaveCmd->SetParameterName("AutoSave", false, false);
	fAutoSaveCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}


AnalysisOptPhMessenger::~AnalysisOptPhMessenger()
{
	delete fVerboseCmd;
	delete fPrintModuloCmd;
	delete fStepDebugCmd;
	delete fSaveDataCmd;
	delete fFileNameCmd;
	delete fDefOptSDCmd;
	
	delete fAnalysisDir;
	delete fAutoSaveCmd;
	delete fAutoFlushCmd;
}


void AnalysisOptPhMessenger::SetNewValue(G4UIcommand *pUIcommand, G4String hNewValue)
{
	
	if(pUIcommand == fVerboseCmd){
		G4cout << "Info --> AnalysisOptPhMessenger::SetNewValue(...): called command fVerboseCmd" << G4endl;
		fAnManager->SetVerbosity(fVerboseCmd->GetNewIntValue(hNewValue));
		return;
	}
	
	if(pUIcommand == fPrintModuloCmd){
		G4cout << "Info --> AnalysisOptPhMessenger::SetNewValue(...): called command fPrintModuloCmd" << G4endl;
		fAnManager->SetPrintModulo(fPrintModuloCmd->GetNewIntValue(hNewValue));
		return;
	}
	
	if(pUIcommand == fStepDebugCmd){
		G4cout << "Info --> AnalysisOptPhMessenger::SetNewValue(...): called command fVerboseCmd. Setting the analysis manager steps debug flag to: " << G4UIcommand::ConvertToString(fStepDebugCmd->GetNewBoolValue(hNewValue)) << G4endl;
		fAnManager->SetStepsDebug(fStepDebugCmd->GetNewBoolValue(hNewValue));
		return;
	}
	
	if(pUIcommand == fSaveDataCmd){
		G4cout << "Info --> AnalysisOptPhMessenger::SetNewValue(...): called command fSaveDataCmd. Setting the analysis manager saving flag to: " << fSaveDataCmd->ConvertToString(fSaveDataCmd->GetNewIntValue(hNewValue)) << G4endl;
		fAnManager->SetSaveData((AnalysisManagerOptPh::datasave)fSaveDataCmd->GetNewIntValue(hNewValue));
		return;
	}
	
	if(pUIcommand == fFileNameCmd){
		G4cout << "Info --> AnalysisOptPhMessenger::SetNewValue(...): called command fFileNameCmd. Setting the tree file name to: " << hNewValue << G4endl;
		if(fAnManager->GetSaveStatus()>AnalysisManagerOptPh::kOff){
			fAnManager->SetDataFilename(hNewValue);
		}
		return;
	}
	
	if(pUIcommand == fDefOptSDCmd){
		G4cout << "Info --> AnalysisOptPhMessenger::SetNewValue(...): called command fDefOptSDCmd. Setting the optical photons sensitive volumes file name to: " << hNewValue << G4endl;
		fAnManager->DefineOptPhSensDet(hNewValue);
		return;
	}
	
	if(pUIcommand == fAutoFlushCmd){
		G4cout << "Info --> AnalysisOptPhMessenger::SetNewValue(...): called command fAutoFlushCmd. Setting the data TTree autoflush to " << hNewValue << G4endl;
		fAnManager->SetAutoFlush( std::stoll(hNewValue) );
		return;
	}
	
	if(pUIcommand == fAutoSaveCmd){
		G4cout << "Info --> AnalysisOptPhMessenger::SetNewValue(...): called command fAutoSaveCmd. Setting the data TTree autosave to " << hNewValue << G4endl;
		fAnManager->SetAutoSave( std::stoll(hNewValue) );
		return;
	}
	
	
	G4cerr << "ERROR ---> AnalysisOptPhMessenger::SetNewValue(...): not recognized command!" << G4endl;

}


