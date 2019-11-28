#include "DetectorMessenger.hh"
#include "DetConstr.hh"


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
	fDetectorDir = new G4UIdirectory("/arcube/detector/");
	fDetectorDir->SetGuidance("ArgonCube detector geometry and material properties control.");
	
	fDetectorOptDir = new G4UIdirectory("/arcube/detector/optical/");
	fDetectorOptDir->SetGuidance("ArgonCube optical properties setup controls.");
	
	fLArAbsorbtionLengthCmd = new G4UIcmdWithADoubleAndUnit("/arcube/detector/optical/setLArAbsorbtionLength", this);
	fLArAbsorbtionLengthCmd->SetGuidance("Define LXe absorbtion length.");
	fLArAbsorbtionLengthCmd->SetParameterName("AbsL", false);
	fLArAbsorbtionLengthCmd->SetRange("AbsL >= 0.");
	fLArAbsorbtionLengthCmd->SetUnitCategory("Length");
	fLArAbsorbtionLengthCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fLArRayScatterLengthCmd = new G4UIcmdWithADoubleAndUnit("/arcube/detector/optical/setLArRayScatterLength", this);
	fLArRayScatterLengthCmd->SetGuidance("Define LAr Rayleigh Scattering length.");
	fLArRayScatterLengthCmd->SetParameterName("ScatL", false);
	fLArRayScatterLengthCmd->SetRange("ScatL >= 0.");
	fLArRayScatterLengthCmd->SetUnitCategory("Length");
	fLArRayScatterLengthCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	
	fPhysVolCoordCmd = new G4UIcmdWithAString("/arcube/detector/optical/PhysVolCoord", this);
	fPhysVolCoordCmd->SetParameterName("physvol", false);
	fPhysVolCoordCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

DetectorMessenger::~DetectorMessenger()
{
	delete fLArAbsorbtionLengthCmd;
	delete fLArRayScatterLengthCmd;

	delete fPhysVolCoordCmd;

	delete fDetectorDir;
}

void DetectorMessenger::SetNewValue(G4UIcommand *pUIcommand, G4String hNewValue)
{
	//if(pUIcommand == fTeflonReflectivityCmd) fDetector->SetTeflonReflectivity( fTeflonReflectivityCmd->GetNewDoubleValue(hNewValue) );
	
	if(pUIcommand == fLArAbsorbtionLengthCmd) fDetector->SetLArAbsLen( fLArAbsorbtionLengthCmd->GetNewDoubleValue(hNewValue) );
	
	if(pUIcommand == fLArRayScatterLengthCmd) fDetector->SetLArRayleighScLen( fLArRayScatterLengthCmd->GetNewDoubleValue(hNewValue) );
	
	if(pUIcommand == fPhysVolCoordCmd) fDetector->PrintVolumeCoordinates( hNewValue );
	
	//if(pUIcommand == fLArScintillationCmd) fDetector->SetLXeScintillation(fLXeScintillationCmd->GetNewBoolValue(hNewValue));
	
	
}


