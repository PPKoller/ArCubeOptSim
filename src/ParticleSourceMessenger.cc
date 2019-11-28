#include "ParticleSource.hh"
#include "ParticleSourceMessenger.hh"

#include "G4UIcommand.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"
#include "G4Tokenizer.hh"

#include "G4Geantino.hh"
#include "G4ParticleTable.hh"
#include "G4OpticalPhoton.hh"

#include "G4ios.hh"

#include <fstream>
#include <iomanip>


ParticleSourceOptPhMessenger::ParticleSourceOptPhMessenger(ParticleSourceOptPh *pParticleSource)
{
	fParticleSource = pParticleSource;
	
	fParticleTable = G4ParticleTable::GetParticleTable();

	// create directory
	fDirectory = new G4UIdirectory("/argoncube/optphot/gun/");
	fDirectory->SetGuidance("Particle source control commands for optical photons in ArgonCube.");

	// list available particles
	/*
	fListCmd = new G4UIcmdWithoutParameter("/xurich2/gun/List", this);
	fListCmd->SetGuidance("List available particles.");
	fListCmd->SetGuidance(" Invoke G4ParticleTable.");
	*/
	
	// set particle  
	fParticleCmd = new G4UIcmdWithAString("/argoncube/optphot/gun/particle", this);
	fParticleCmd->SetGuidance("Set particle to be generated.");
	fParticleCmd->SetGuidance(" (geantino is default)");
	fParticleCmd->SetGuidance(" (opticalphoton can be specified for shooting)");
	fParticleCmd->SetParameterName("particleName", true);
	fParticleCmd->SetDefaultValue("geantino");
	G4String candidateList;
	G4int nPtcl = fParticleTable->entries();

	for(G4int iPart = 0; iPart < nPtcl; iPart++)
	{
		candidateList += fParticleTable->GetParticleName(iPart);
		candidateList += " ";
	}
	fParticleCmd->SetCandidates(candidateList);
	fParticleCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	// number of primaries
	fPrimNbCmd = new G4UIcmdWithAnInteger("/argoncube/optphot/gun/primaryNb", this);
	fPrimNbCmd->SetGuidance("Set number of primary particles");
	fPrimNbCmd->SetGuidance(" Default 1");
	fPrimNbCmd->SetParameterName("PrimNb", true);
	fPrimNbCmd->SetDefaultValue(1);
	fPrimNbCmd->SetRange("PrimNb>=0");
	fPrimNbCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	// particle direction
	fDirectionCmd = new G4UIcmdWith3Vector("/argoncube/optphot/gun/direction", this);
	fDirectionCmd->SetGuidance("Set momentum direction.");
	fDirectionCmd->SetGuidance("Direction needs not to be a unit vector.");
	fDirectionCmd->SetParameterName("Px", "Py", "Pz", true, true);
	fDirectionCmd->SetRange("Px != 0 || Py != 0 || Pz != 0");
	fDirectionCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	// set polarization
	fPolarCmd = new G4UIcmdWith3Vector("/argoncube/optphot/gun/polarization", this);
	fPolarCmd->SetGuidance("Set polarization vector.");
	fPolarCmd->SetGuidance("Direction needs not to be a unit vector.");
	fPolarCmd->SetParameterName("ex", "ey", "ez", true, true);
	fPolarCmd->SetRange("ex != 0 || ey != 0 || ez != 0");
	fPolarCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	// particle energy
	fEnergyCmd = new G4UIcmdWithADoubleAndUnit("/argoncube/optphot/gun/energy", this);
	fEnergyCmd->SetGuidance("Set optical photon energy.");
	fEnergyCmd->SetParameterName("Energy", true, true);
	fEnergyCmd->SetDefaultUnit("eV");
	fEnergyCmd->SetUnitCategory("Energy");
	fEnergyCmd->SetUnitCandidates("eV keV MeV GeV TeV");
	fEnergyCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fPositionCmd = new G4UIcmdWith3VectorAndUnit("/argoncube/optphot/gun/position", this);
	fPositionCmd->SetGuidance("Set starting position of the particle.");
	fPositionCmd->SetParameterName("X", "Y", "Z", true, true);
	fPositionCmd->SetDefaultUnit("cm");
	fPositionCmd->SetUnitCategory("Length");
	fPositionCmd->SetUnitCandidates("nm mum mm cm m km");
	fPositionCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	// source distribution type
	fTypeCmd = new G4UIcmdWithAString("/argoncube/optphot/gun/sourceType", this);
	fTypeCmd->SetGuidance("Sets source distribution type.");
	fTypeCmd->SetGuidance("Either Point or Volume");
	fTypeCmd->SetParameterName("DisType", true, true);
	fTypeCmd->SetDefaultValue("Point");
	fTypeCmd->SetCandidates("Point Volume");
	fTypeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	// source shape
	fShapeCmd = new G4UIcmdWithAString("/argoncube/optphot/gun/shape", this);
	fShapeCmd->SetGuidance("Sets source shape type.");
	fShapeCmd->SetParameterName("Shape", true, true);
	fShapeCmd->SetDefaultValue("NULL");
	fShapeCmd->SetCandidates("Sphere Cylinder Box");
	fShapeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	// center coordinates
	fCenterCmd = new G4UIcmdWith3VectorAndUnit("/argoncube/optphot/gun/center", this);
	fCenterCmd->SetGuidance("Set center coordinates of source.");
	fCenterCmd->SetParameterName("X", "Y", "Z", true, true);
	fCenterCmd->SetDefaultUnit("cm");
	fCenterCmd->SetUnitCategory("Length");
	fCenterCmd->SetUnitCandidates("nm mum mm cm m km");
	fCenterCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	// half x of source(if source shape is Box)
	fHalfxCmd = new G4UIcmdWithADoubleAndUnit("/argoncube/optphot/gun/halfx", this);
	fHalfxCmd->SetGuidance("Set x half length of source.");
	fHalfxCmd->SetParameterName("Halfx", true, true);
	fHalfxCmd->SetDefaultUnit("cm");
	fHalfxCmd->SetUnitCategory("Length");
	fHalfxCmd->SetUnitCandidates("nm mum mm cm m km");
	fHalfxCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	// half y of source (if source shape is Box)
	fHalfyCmd = new G4UIcmdWithADoubleAndUnit("/argoncube/optphot/gun/halfy", this);
	fHalfyCmd->SetGuidance("Set y half length of source.");
	fHalfyCmd->SetParameterName("Halfy", true, true);
	fHalfyCmd->SetDefaultUnit("cm");
	fHalfyCmd->SetUnitCategory("Length");
	fHalfyCmd->SetUnitCandidates("nm mum mm cm m km");
	fHalfyCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	// half height of source
	fHalfzCmd = new G4UIcmdWithADoubleAndUnit("/argoncube/optphot/gun/halfz", this);
	fHalfzCmd->SetGuidance("Set z half length of source.");
	fHalfzCmd->SetParameterName("Halfz", true, true);
	fHalfzCmd->SetDefaultUnit("cm");
	fHalfzCmd->SetUnitCategory("Length");
	fHalfzCmd->SetUnitCandidates("nm mum mm cm m km");
	fHalfzCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	// radius of source  
	fRadiusCmd = new G4UIcmdWithADoubleAndUnit("/argoncube/optphot/gun/radius", this);
	fRadiusCmd->SetGuidance("Set radius of source.");
	fRadiusCmd->SetParameterName("Radius", true, true);
	fRadiusCmd->SetDefaultUnit("cm");
	fRadiusCmd->SetUnitCandidates("nm mum mm cm m km");
	fRadiusCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	// confine to volume(s)
	fConfineCmd = new G4UIcmdWithAString("/argoncube/optphot/gun/confine", this);
	fConfineCmd->SetGuidance("Confine source to volume(s) (NULL to unset).");
	fConfineCmd->SetGuidance("usage: confine VolName1 VolName2 ...");
	fConfineCmd->SetParameterName("VolName", true, true);
	fConfineCmd->SetDefaultValue("NULL");
	fConfineCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	// angular distribution
	fAngTypeCmd = new G4UIcmdWithAString("/argoncube/optphot/gun/angtype", this);
	fAngTypeCmd->SetGuidance("Sets angular source distribution type");
	fAngTypeCmd->SetGuidance("Possible variables are: iso direction");
	fAngTypeCmd->SetParameterName("AngDis", true, true);
	fAngTypeCmd->SetDefaultValue("iso");
	fAngTypeCmd->SetCandidates("iso direction");
	fAngTypeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	// verbosity
	fVerbosityCmd = new G4UIcmdWithAnInteger("/argoncube/optphot/gun/verbose", this);
	fVerbosityCmd->SetGuidance("Set Verbose level for gun");
	fVerbosityCmd->SetGuidance(" 0 : Silent");
	fVerbosityCmd->SetGuidance(" 1 : Limited information");
	fVerbosityCmd->SetGuidance(" 2 : Detailed information");
	fVerbosityCmd->SetParameterName("level", false);
	fVerbosityCmd->SetRange("level>=0 && level <=2");
	fVerbosityCmd->AvailableForStates(G4State_PreInit, G4State_PreInit, G4State_Idle);
	
	// Get the particle type
	fGetPartCmd = new G4UIcmdWithoutParameter("/argoncube/optphot/gun/getPartType", this);
	fGetPartCmd->SetGuidance("Prints the particle type selected");
	fGetPartCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	// Get particle direction
	fGetDirectCmd = new G4UIcmdWithoutParameter("/argoncube/optphot/gun/getDirection", this);
	fGetDirectCmd->SetGuidance("Prints the particle direction set");
	fGetDirectCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	// Get particle polarization
	fGetPolarCmd = new G4UIcmdWithoutParameter("/argoncube/optphot/gun/getPolar", this);
	fGetPolarCmd->SetGuidance("Prints the particle polarization set");
	fGetPolarCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

ParticleSourceOptPhMessenger::~ParticleSourceOptPhMessenger()
{
	delete fDirectory;
	
	delete fPrimNbCmd;
	delete fTypeCmd;
	delete fShapeCmd;
	delete fCenterCmd;
	delete fHalfxCmd;
	delete fHalfyCmd;
	delete fHalfzCmd;
	delete fRadiusCmd;
	delete fConfineCmd;
	delete fAngTypeCmd;
	delete fVerbosityCmd;
	delete fParticleCmd;
	delete fPositionCmd;
	delete fDirectionCmd;
	delete fPolarCmd;
	delete fEnergyCmd;
	delete fGetPartCmd;
	delete fGetDirectCmd;
	delete fGetPolarCmd;
}

void ParticleSourceOptPhMessenger::SetNewValue(G4UIcommand* command, G4String newValues)
{
	if(command == fPrimNbCmd){
		G4int newPrimNb = fPrimNbCmd->GetNewIntValue(newValues);
		G4cout << "\nSetting number of primaries to be generated to: " << newPrimNb << G4endl;
		fParticleSource->SetPrimNb( newPrimNb );
		return;
	}
	
	if(command == fParticleCmd){
		G4ParticleDefinition * pd = fParticleTable->FindParticle(newValues);
		if(pd){
			G4cout << "\nSetting primary particle to: <" << pd->GetParticleName() << ">" << G4endl;
			fParticleSource->SetParticleDef(pd);
		}
		return;
	}
	
	if(command == fTypeCmd){
		G4cout << "\nSetting position distribution type to: \"" << newValues << "\"" << G4endl;
		fParticleSource->SetPosDisType(newValues);
		return;
	}
	
	if(command == fShapeCmd){
		G4cout << "\nSetting position source shape to: \"" << newValues << "\"" << G4endl;
		fParticleSource->SetPosDisShape(newValues);
		return;
	}
	
	if(command == fCenterCmd){
		G4cout << "\nSetting source center coordinates to: " << fCenterCmd->GetNew3VectorValue(newValues) << G4endl;
		fParticleSource->SetCenterCoords(fCenterCmd->GetNew3VectorValue(newValues));
		return;
	}
	
	if(command == fHalfxCmd){
		G4cout << "\nSetting source half X dimension to: " << fHalfxCmd->GetNewDoubleValue(newValues) << G4endl;
		fParticleSource->SetHalfX(fHalfxCmd->GetNewDoubleValue(newValues));
		return;
	}
	
	if(command == fHalfyCmd){
		G4cout << "\nSetting source half Y dimension to: " << fHalfyCmd->GetNewDoubleValue(newValues) << G4endl;
		fParticleSource->SetHalfY(fHalfyCmd->GetNewDoubleValue(newValues));
		return;
	}
	
	if(command == fHalfzCmd){
		G4cout << "\nSetting source half Z dimension to: " << fHalfzCmd->GetNewDoubleValue(newValues) << G4endl;
		fParticleSource->SetHalfZ(fHalfzCmd->GetNewDoubleValue(newValues));
		return;
	}
	
	if(command == fRadiusCmd){
		G4cout << "\nSetting source radius to: " << fRadiusCmd->GetNewDoubleValue(newValues) << G4endl;
		fParticleSource->SetRadius(fRadiusCmd->GetNewDoubleValue(newValues));
		return;
	}
	
	if(command == fAngTypeCmd){
		G4cout << "\nSetting angular distribution type to: \"" << newValues << "\"" << G4endl;
		fParticleSource->SetAngDistType(newValues);
		return;
	}
	
	if(command == fConfineCmd){
		G4cout << "\nConfining primary particle to volume: \"" << newValues << "\"" << G4endl;
		fParticleSource->ConfineSourceToVolume(newValues);
		return;
	}
	
	if(command == fVerbosityCmd){
		G4cout << "\nSetting particle generator verbosity level to: " << fVerbosityCmd->GetNewIntValue(newValues) << G4endl;
		fParticleSource->SetVerbosity(fVerbosityCmd->GetNewIntValue(newValues));
		return;
	}
	
	//else if(command == fListCmd) fParticleSource->DumpTable();return;
	
	if(command == fDirectionCmd)
	{
		G4cout << "\nSetting primary particle direction to: " << fDirectionCmd->GetNew3VectorValue(newValues) << G4endl;
		G4cout << "\nSetting angular distribution type to: \"direction\"" << G4endl;
		fParticleSource->SetAngDistType("direction");
		fParticleSource->SetDirection(fDirectionCmd->GetNew3VectorValue(newValues));
		return;
	}
	
	if(command==fPolarCmd){
		G4cout << "\nSetting primary particle polarization to: " << fPolarCmd->GetNew3VectorValue(newValues) << G4endl;
		fParticleSource->SetPhotonPolar(fPolarCmd->GetNew3VectorValue(newValues));
		return;
	}
	
	if(command == fEnergyCmd)
	{
		G4cout << "\nSetting primary particle energy to: " << newValues << G4endl;
		G4cout << "\nSetting energy distribution type to: \"Mono\"" << G4endl;
		fParticleSource->SetEnergyDisType("Mono");
		fParticleSource->SetKinEnergy(fEnergyCmd->GetNewDoubleValue(newValues));
		return;
	}
	
	if(command == fPositionCmd)
	{
		G4cout << "\nSetting primary particle position to: " << fPositionCmd->GetNew3VectorValue(newValues) << G4endl;
		G4cout << "\nSetting position distribution type to: \"Point\"" << G4endl;
		fParticleSource->SetPosDisType("Point");
		fParticleSource->SetCenterCoords(fPositionCmd->GetNew3VectorValue(newValues));
		return;
	}
	
	if(command == fGetPartCmd)
	{
		fParticleSource->PrintParticle();
		return;
	}
	
	if(command == fGetDirectCmd)
	{
		
		fParticleSource->PrintDirection();
		return;
	}
	
	if(command == fGetPolarCmd)
	{
		fParticleSource->PrintPolar();
		return;
	}
	
	G4cerr << "ParticleSourceOptPhMessenger::SetNewValue(...) --> ERROR: not recognized command" << G4endl;
}

