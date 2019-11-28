#include "AnalysisMessenger.hh"
#include "AnalysisManager.hh"

#include <set>
#include <vector>
#include <fstream>
#include <iomanip>


AnalysisOptPhMessenger::AnalysisOptPhMessenger(AnalysisManagerOptPh *pAnManager)
{ 
	fAnManager = pAnManager;
	
	fAnalysisDir = new G4UIdirectory("/arcube/analysis/");
	fAnalysisDir->SetGuidance("ArgonCube analysis manager settings.");
	
	
	fVerboseCmd = new G4UIcmdWithAnInteger("/arcube/analysis/verbose",this);
	fVerboseCmd->SetGuidance("Set verbosity of the analysis manager");
	fVerboseCmd->SetGuidance(" Default 1");
	fVerboseCmd->SetParameterName("Verb", true);
	fVerboseCmd->SetDefaultValue(1);
	fVerboseCmd->SetRange("Verb>=0 && Verb<=2");
	fVerboseCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	fPrintModuloCmd = new G4UIcmdWithAnInteger("/arcube/analysis/PrintModulo",this);
	fPrintModuloCmd->SetGuidance("Prints the start of event every \"PrMod\"");
	fPrintModuloCmd->SetParameterName("PrMod", false);
	fPrintModuloCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fStepDebugCmd = new G4UIcmdWithABool("/arcube/analysis/stepsDebug", this);
	fStepDebugCmd->SetGuidance("Activates debugging controls and messages at step level.");
	fStepDebugCmd->SetParameterName("DebugSteps", false);
	fStepDebugCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	fSaveDataCmd = new G4UIcmdWithAnInteger("/arcube/analysis/SaveData", this);
	fSaveDataCmd->SetGuidance("Control for data tree saving: 0 to not save; 1 standard analysis variables (default); 2 extended tracking info for debugging (slow and heavy)");
	fSaveDataCmd->SetParameterName("Save", true);
	fSaveDataCmd->SetDefaultValue(1);
	fSaveDataCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	fFileNameCmd = new G4UIcmdWithAString("/arcube/analysis/FileName",this);
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
}


void AnalysisOptPhMessenger::SetNewValue(G4UIcommand *pUIcommand, G4String hNewValue)
{
	
	if(pUIcommand == fVerboseCmd){
		fAnManager->SetVerbosity(fVerboseCmd->GetNewIntValue(hNewValue));
		return;
	}
	
	if(pUIcommand == fPrintModuloCmd){
		fAnManager->SetPrintModulo(fPrintModuloCmd->GetNewIntValue(hNewValue));
		return;
	}
	
	if(pUIcommand == fStepDebugCmd){
		G4cout << "\nSetting the analysis manager steps debug flag to: " << G4UIcommand::ConvertToString(fStepDebugCmd->GetNewBoolValue(hNewValue)) << G4endl;
		fAnManager->SetStepsDebug(fStepDebugCmd->GetNewBoolValue(hNewValue));
		return;
	}
	
	if(pUIcommand == fSaveDataCmd){
		G4cout << "\nSetting the analysis manager saving flag to: " << fSaveDataCmd->ConvertToString(fSaveDataCmd->GetNewIntValue(hNewValue)) << G4endl;
		fAnManager->SetSaveData(fSaveDataCmd->GetNewIntValue(hNewValue));
		return;
	}
	
	if(pUIcommand == fFileNameCmd){
		G4cout << "\nSetting the tree file name to: " << hNewValue << G4endl;
		fAnManager->SetSaveData(true);
		fAnManager->SetDataFilename(hNewValue);
		return;
	}
	
	if(pUIcommand == fDefOptSDCmd){
		G4cout << "\nSetting the optical photons sensitive volumes file name to: " << hNewValue << G4endl;
		fAnManager->DefineOptPhSensDet(hNewValue);
	}
	
	
	G4cerr << "ERROR ---> AnalysisOptPhMessenger::SetNewValue(...): not recognized command!" << G4endl;

}


