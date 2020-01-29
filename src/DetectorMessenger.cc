#include "DetectorMessenger.hh"
#include "DetConstr.hh"
#include "OptPropManager.hh"

#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>
#include <G4ParticleTable.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWith3Vector.hh>
#include <G4UIcmdWith3VectorAndUnit.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWithABool.hh>
#include <G4Tokenizer.hh>
#include <G4ios.hh>


#include <fstream>
#include <iomanip>


DetectorMessenger::DetectorMessenger(DetConstrOptPh *pDetector)
:fDetector(pDetector)
{ 
	fDetectorDir = new G4UIdirectory("/argoncube/detector/");
	fDetectorDir->SetGuidance("ArgonCube detector geometry and material properties control.");
	
	fDetectorOptDir = new G4UIdirectory("/argoncube/detector/optical/");
	fDetectorOptDir->SetGuidance("ArgonCube optical properties setup controls.");
	
	fPhysVolCoordCmd = new G4UIcmdWithAString("/argoncube/detector/PhysVolCoord", this);
	fPhysVolCoordCmd->SetParameterName("physvol", false);
	fPhysVolCoordCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	fLoadOpticalSettingsFile = new G4UIcmdWithAString("/argoncube/detector/optical/loadOptSett", this);
	fLoadOpticalSettingsFile->SetGuidance("Load the json file with all the optical settings used");
	fLoadOpticalSettingsFile->SetParameterName("SettFile",false);
	fLoadOpticalSettingsFile->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	
}

DetectorMessenger::~DetectorMessenger()
{
	//delete fLArAbsorbtionLengthCmd;
	//delete fLArRayScatterLengthCmd;

	delete fPhysVolCoordCmd;
	delete fLoadOpticalSettingsFile;
	delete fDetectorOptDir;
	delete fDetectorDir;
}

void DetectorMessenger::SetNewValue(G4UIcommand *pUIcommand, G4String hNewValue)
{
	if(pUIcommand == fPhysVolCoordCmd) fDetector->PrintVolumeCoordinates( hNewValue );
	
	if(pUIcommand == fLoadOpticalSettingsFile){
		OptPropManager::OptPropManagerInstance()->ProcessJsonFile( hNewValue );
	}
}


