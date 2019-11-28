#include "ParticleSource.hh"
#include "ParticleSourceMessenger.hh"
#include "PrimGenAction.hh"

#include <G4PrimaryParticle.hh>
#include <G4Event.hh>
#include <G4TransportationManager.hh>
#include <G4VPhysicalVolume.hh>
#include <G4PhysicalVolumeStore.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4IonTable.hh>
#include <G4Ions.hh>
#include <G4TrackingManager.hh>
#include <G4Track.hh>
#include <Randomize.hh>
#include "G4SystemOfUnits.hh"

#include <iostream>
#include <sstream>
#include <cmath>

using std::stringstream;



ParticleSourceOptPh::ParticleSourceOptPh(PrimaryGeneratorActionOptPh *primGenAct, G4int nPrim, G4int verb)
{
	fPrimGenAct = primGenAct;
	
	fPrimNb = nPrim;
	
	fParticleDefinition = G4ParticleTable::GetParticleTable()->FindParticle("opticalphoton");
	
	if(!fParticleDefinition){
		G4Exception("ParticleSourceOptPh::SetParticleDef(...)","Event0100", FatalException,"Cannot find the \"opticalphoton\" definition in the G4ParticleTable.");
	}
	
	fVerbosityLevel = verb;
	
	SetInitialValues();
	
	fMessenger = new ParticleSourceOptPhMessenger(this);
	fNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
	
	G4cout << G4endl << "Particle source initialized." << G4endl;
}


ParticleSourceOptPh::~ParticleSourceOptPh()
{
	delete fMessenger;
}


void ParticleSourceOptPh::SetInitialValues()
{
	fPartName = fParticleDefinition->GetParticleName();
	fCharge = fParticleDefinition->GetPDGCharge();
	fMass = fParticleDefinition->GetPDGMass();
	
	fMomentumDirection = G4ParticleMomentum(0., 0., 1.);
	fKinEnergy = 9.69*eV; //128 nm optical photons
	fTotEnergy = fMass + fKinEnergy;
	fMom = std::sqrt( fTotEnergy*fTotEnergy - fMass*fMass );
	
	fPosition=G4ThreeVector(0.,0.,0.);
	fTime=0.;
	fPolarization=G4ThreeVector(1.,0.,0.);
	
	
	fSourcePosType="Point";
	fShape="NULL";;
	fCenterCoords=G4ThreeVector(0.,0.,0.);
	
	fHalfx=0.;
	fHalfy=0.;
	fHalfz=0.;
	fRadius=0.;
	fConfine=false;
	fAngDistType="iso";
	fEnergyDisType="Mono";
	
	fVolumeNames.clear();
	
	fEnPrim.resize(fPrimNb,fTotEnergy);
	fPosPrim = G4ThreeVector(0.,0.,0.);
	fMomPrim.resize(fPrimNb);
	fPolPrim.resize(fPrimNb);
	
	fPhysVolPrim.resize(fPrimNb);
}


//THIS IS THE CENTRAL FUNCTION OF THIS CLASS
void ParticleSourceOptPh::GeneratePrimaryVertex(G4Event* evt)
{

	if(!fParticleDefinition)
	{
		G4cerr << "\nParticleSourceOptPh::GeneratePrimaryVertex --> ERROR: No particle is defined!" << G4endl;
		return;
	}

	// Position
	G4bool srcconf = false;
	
	
	if(fSourcePosType == "Point"){
		GeneratePointSource();
	}else if(fSourcePosType == "Volume"){
		
		if(fConfine == true){
			
			G4int LoopCount = 0;
			do{
				GeneratePointsInVolume();
				LoopCount++;
				
				if(LoopCount == 1000000)
				{
					G4cerr << "\nParticleSourceOptPh::GeneratePrimaryVertex(...) --> ERROR:" << G4endl;
					G4cerr << "*************************************" << G4endl;
					G4cerr << "LoopCount = 1000000" << G4endl;
					G4cerr << "Either the source distribution >> confinement" << G4endl;
					G4cerr << "or any confining volume may not overlap with" << G4endl;
					G4cerr << "the source distribution or any confining volumes" << G4endl;
					G4cerr << "may not exist" << G4endl;
					G4cerr << "If you have set confine then this will be ignored" << G4endl;
					G4cerr << "for this event." << G4endl;
					G4cerr << "*************************************" << G4endl;
					break; //Avoids an infinite loop
				}
				
			}while(!IsSourceConfined());
			
			
			
		}else{
			GeneratePointsInVolume();
		}
		
		
	}
	
	fPosPrim = fPosition;
	
	// Angular stuff
	if(fAngDistType == "direction"){
		SetDirection(fMomentumDirection);
	}else{
		if(fAngDistType != "iso"){
			G4cerr << "\nERROR --> ParticleSourceOptPh::GeneratePrimaryVertex(...): AngDistType has unusual value" << G4endl;
			return;
		}
	}
	
	// Energy stuff
	if(fEnergyDisType != "Mono"){
		G4cerr << "\nERROR --> ParticleSourceOptPh::GeneratePrimaryVertex: EnergyDisType has unusual value" << G4endl;
		return;
	}
	
	// create a new vertex
	G4PrimaryVertex *vertex = new G4PrimaryVertex(fPosition, fTime);
	
	G4String PhysVolName = fNavigator->LocateGlobalPointAndSetup(fPosition)->GetName();
	
	
	if(fVerbosityLevel > 2)
		G4cout << "Debug --> ParticleSourceOptPh::GeneratePrimaryVertex: Creating primaries and assigning to vertex" << G4endl;
	
	
	if(fVerbosityLevel > 0){
		G4cout << "\nInfo --> ParticleSourceOptPh::GeneratePrimaryVertex(...): " << G4endl;
		G4cout << " Number of prymaries per event: " << fPrimNb << G4endl;
		G4cout << "                 Particle name: " << fParticleDefinition->GetParticleName() << G4endl;
		G4cout << "                        Energy: " << fTotEnergy/keV << " keV" << G4endl;
		G4cout << "                          Mass: " << fMass/MeV << " MeV" << G4endl;
		G4cout << "                      Position: " << fPosition << G4endl;
		if(fAngDistType == "direction"){
		G4cout << "                     Direction: " << fMomentumDirection << G4endl;
		G4cout << "                  Polarization: " << fPolarization << G4endl;
		}else if(fAngDistType == "iso"){
		G4cout << "                     Direction: isotropic" << fMomentumDirection << G4endl;
		G4cout << "                  Polarization: random" << fPolarization << G4endl;
		}
	}
	
	
	if(fVerbosityLevel > 2) G4cout << "\nDebug --> ParticleSourceOptPh::GeneratePrimaryVertex(...): " << G4endl;
	
	for(G4int iPart = 0; iPart < fPrimNb; iPart++)
	{
		if(fAngDistType == "iso"){
			GenerateIsotropic();
			if(fVerbosityLevel > 2){
				G4cout << "    Direction("<<iPart<<"): " << fMomentumDirection.unit() << G4endl;
				G4cout << "    Polarization("<<iPart<<"): " << fPolarization << G4endl;
			}
		}
		
		G4PrimaryParticle *particle = new G4PrimaryParticle(fParticleDefinition);
		particle->SetMass(fMass);
		particle->SetMomentumDirection(fMomentumDirection.unit());
		particle->SetTotalEnergy(fTotEnergy);
		particle->SetCharge(fCharge);
		particle->SetPolarization(fPolarization.unit());
		vertex->SetPrimary(particle);
		
		fMomPrim.at(iPart) = particle->GetMomentum();
		fPolPrim.at(iPart) = particle->GetPolarization();
	}
	evt->AddPrimaryVertex(vertex);
	if(fVerbosityLevel > 1) G4cout << " Primary Vertex generated with " << fMomPrim.size() << " particles." << G4endl;
}


void ParticleSourceOptPh::SetPrimNb(G4int nprim)
{
	fPrimNb = nprim;
	
	if(fPrimGenAct) fPrimGenAct->SetPrimNb(fPrimNb);
	
	fEnPrim.resize(fPrimNb,fTotEnergy);
	fMomPrim.resize(fPrimNb);
	fPolPrim.resize(fPrimNb);
	fPhysVolPrim.resize(fPrimNb);
	
	return;
}


void ParticleSourceOptPh::SetParticleDef(G4ParticleDefinition *aParticleDefinition)
{
	if(!aParticleDefinition){
		G4Exception("ParticleSourceOptPh::SetParticleDef(...)","Event0101",FatalException,"Null pointer is given.");
	}
	fParticleDefinition = aParticleDefinition;
	fCharge = fParticleDefinition->GetPDGCharge();
	fMass = fParticleDefinition->GetPDGMass();
	fPartName = fParticleDefinition->GetParticleName();
}


void ParticleSourceOptPh::SetKinEnergy(G4double KinEnergy){
	fKinEnergy = KinEnergy;
	if(!fParticleDefinition){
		//Assuming zero mass
		fMass = 0;
		fTotEnergy = fKinEnergy;
		fMom = fTotEnergy;
		return;
	}
	fTotEnergy = fMass + fKinEnergy;
	fMom = std::sqrt( fTotEnergy*fTotEnergy - fMass*fMass );
}


void ParticleSourceOptPh::SetMomentum(G4double aMomentum)
{
	fMom = aMomentum;
	if(!fParticleDefinition){
		//Assuming zero mass
		fMass = 0;
		fTotEnergy = fMom;
		fKinEnergy = fTotEnergy;
	}
	
	fTotEnergy = std::sqrt( fMom*fMom + fMass*fMass );
	fKinEnergy = fTotEnergy - fMass;
}


void ParticleSourceOptPh::SetMomentum(G4ParticleMomentum aMomentum)
{
	fMomentumDirection = aMomentum.unit();
	SetMomentum(aMomentum.mag());
}


void ParticleSourceOptPh::PrintParticle(){ 
	G4cout << "\nSelected particle: " << fParticleDefinition->GetParticleName() << G4endl;
	//std::cout << "\nSelected particle: " << fParticleDefinition->GetParticleName() << std::endl;
}


void ParticleSourceOptPh::PrintDirection()
{
	if(GetAngDistrType()!=G4String("iso")){
		G4cout << "\nParticle direction: " << fMomentumDirection.unit() << G4endl;
	}else{
		G4cout << "\nParticle direction: isotropic" << G4endl;
	}
}


void ParticleSourceOptPh::PrintPolar()
{
	if(GetAngDistrType()!=G4String("iso")){
		G4cout << "\nParticle polarization: " << fPolarization.unit() << G4endl;
	}else{
		G4cout << "\nParticle polarization: random" << G4endl;
	}
}


void ParticleSourceOptPh::ConfineSourceToVolume(G4String hVolumeList)
{
	stringstream hStream;
	hStream.str(hVolumeList);
	G4String hVolumeName;
	
	// store all the volume names
	while(!hStream.eof())
	{
		hStream >> hVolumeName;
		fVolumeNames.insert(hVolumeName);
	}

	// checks if the selected volumes exist and store all volumes that match
	G4PhysicalVolumeStore *PVStore = G4PhysicalVolumeStore::GetInstance();
	G4bool bFoundAll = true;

	set<G4String> hActualVolumeNames;
	for(set<G4String>::iterator pIt = fVolumeNames.begin(); pIt != fVolumeNames.end(); pIt++){
		G4String hRequiredVolumeName = *pIt;
		G4bool bMatch = false;

		if(bMatch = (hRequiredVolumeName.last('*') != std::string::npos))
			hRequiredVolumeName = hRequiredVolumeName.strip(G4String::trailing, '*');

		G4bool bFoundOne = false;
		for(G4int iIndex = 0; iIndex < (G4int) PVStore->size(); iIndex++)
		{
			G4String hName = (*PVStore)[iIndex]->GetName();

			if((bMatch && (hName.substr(0, hRequiredVolumeName.size())) == hRequiredVolumeName) || hName == hRequiredVolumeName)
			{
				hActualVolumeNames.insert(hName);
				bFoundOne = true;
			}
		}

		bFoundAll = bFoundAll && bFoundOne;
	}

	if(bFoundAll)
	{
		fVolumeNames = hActualVolumeNames;
		fConfine = true;

		if(fVerbosityLevel >= 1)
			G4cout << "Source confined to volumes: " << hVolumeList << G4endl;

		if(fVerbosityLevel >= 2)
		{
			G4cout << "Volume list: " << G4endl;

			for(set<G4String>::iterator pIt = fVolumeNames.begin(); pIt != fVolumeNames.end(); pIt++)
				G4cout << *pIt << G4endl;
		}
	}
	else if(fVolumeNames.empty())
		fConfine = false;
	else
	{
		G4cout << "ParticleSourceOptPh::ConfineSourceToVolume(...) --> ERROR: One or more volumes do not exist! " << G4endl;
		G4cout << " Ignoring confine condition" << G4endl;
		fVolumeNames.clear();
		fConfine = false;
	}
}


void ParticleSourceOptPh::GeneratePointSource()
{
	// Generates Points given the point source.
	if(fSourcePosType == "Point")
		fPosition = fCenterCoords;
	else if(fVerbosityLevel >= 1)
		G4cout << "ParticleSourceOptPh::GeneratePointSource() --> ERROR SourcePosType is not set to Point" << G4endl;
}


void ParticleSourceOptPh::GeneratePointsInVolume()
{
	
	G4double x = 0., y = 0., z = 0.;

	if((fSourcePosType != G4String("Volume")) && (fVerbosityLevel >= 1))
		G4cerr << "\nParticleSourceOptPh::GeneratePointsInVolume() --> ERROR: SourcePosType not Volume" << G4endl;

	if(fShape == "Sphere")
	{
		x = fRadius * 2.;
		y = fRadius * 2.;
		z = fRadius * 2.;
		
		do{
			x = G4UniformRand();
			y = G4UniformRand();
			z = G4UniformRand();

			x = (2*x-1)*fRadius;
			y = (2*y-1)*fRadius;
			z = (2*z-1)*fRadius;
		}while(((x * x) + (y * y) + (z * z)) > (fRadius * fRadius));
		
	}

	else if(fShape == "Cylinder")
	{
		x = fRadius * 2.;
		y = fRadius * 2.;
		
		z = G4UniformRand();
		z = (2*z - 1)*fHalfz;
		
		
		do{
			x = G4UniformRand();
			y = G4UniformRand();
			
			x = (2*x-1)*fRadius;
			y = (2*y-1)*fRadius;
		}while(((x * x) + (y * y)) > (fRadius * fRadius));
	}

	else if(fShape == "Box")
	{
		x = 2*(G4UniformRand()-0.5)*fHalfx;
		y = 2*(G4UniformRand()-0.5)*fHalfy;
		z = 2*(G4UniformRand()-0.5)*fHalfz;
	}

	else
		G4cerr << "\nParticleSourceOptPh::GeneratePointsInVolume() --> ERROR: Volume Shape Does Not Exist" << G4endl;
	
	fPosition = fCenterCoords + G4ThreeVector(x,y,z);
}


G4bool ParticleSourceOptPh::IsSourceConfined()
{
	// Method to check point is within the volume specified
	if(fConfine == false)
		G4cerr << "\nParticleSourceOptPh::IsSourceConfined() --> ERROR: Confine is false" << G4endl;
	
	G4ThreeVector null(0., 0., 0.);
	G4ThreeVector *ptr = &null;;

	// Check fParticlePosition is within a volume in our list
	G4VPhysicalVolume *theVolume;

	theVolume = fNavigator->LocateGlobalPointAndSetup(fPosition, ptr, true);
	G4String theVolName = theVolume->GetName();

	set<G4String>::iterator pIt;
	if((pIt = fVolumeNames.find(theVolName)) != fVolumeNames.end())
	{
		if(fVerbosityLevel >= 1)
			G4cout << "Particle is in volume " << *pIt << G4endl;
		return (true);
	}
	else
		return (false);
}


void ParticleSourceOptPh::GenerateIsotropic()
{
	G4double px, py, pz;

	G4double sintheta, sinphi, costheta, cosphi;

	costheta = 1 - 2*G4UniformRand();
	sintheta = std::sqrt(1. - std::pow(costheta,2));

	G4double phi = CLHEP::twopi * G4UniformRand();
	sinphi = std::sin(phi);
	cosphi = std::cos(phi);

	px = sintheta * cosphi;
	py = sintheta * sinphi;
	pz = costheta;

	fMomentumDirection= G4ThreeVector(px,py,pz).unit();
	
	if( (fPartName=="gamma") || (fPartName=="opticalphoton") ){
		
		//Generate the random polarization as a 2pi around the momentum direction
		G4double alpha = CLHEP::twopi * G4UniformRand();
	
		G4double sinalpha = std::sin(alpha);
		G4double cosalpha = std::cos(alpha);
	
		//This is always orthogonal to the momentum direction (try to make the scalar product)
		px = cosalpha*costheta*cosphi - sinalpha*sinphi;
		py = cosalpha*costheta*sinphi + sinalpha*cosphi;
		pz = -cosalpha*sintheta;
	
		fPolarization= G4ThreeVector(px,py,pz).unit();
		
	}
	
	// m_hParticleMomentumDirection now holds unit momentum vector.
	if(fVerbosityLevel >= 2) G4cout << "Generating isotropic vector: " << fMomentumDirection << G4endl;
}




