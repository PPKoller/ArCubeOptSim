#include "G4RunManagerKernel.hh"
#include "G4Event.hh"
#include "Randomize.hh"

#include "PrimGenAction.hh"

PrimaryGeneratorActionOptPh::PrimaryGeneratorActionOptPh():
fParticleSource(NULL)
{
	fParticleSource = new ParticleSourceOptPh(this);
	
	fPrimNb = 1;
	
	fSeeds[0] = -1;
	fSeeds[1] = -1;
}

PrimaryGeneratorActionOptPh::~PrimaryGeneratorActionOptPh()
{
	if(fParticleSource) delete fParticleSource;
}

void PrimaryGeneratorActionOptPh::SetPrimaries(){
	if(fParticleSource){
		fPrimNb = fParticleSource->GetPrimNb();
	}
}

G4int PrimaryGeneratorActionOptPh::GetPrimNb(){
	SetPrimaries();
	return fPrimNb;
}

void PrimaryGeneratorActionOptPh::GeneratePrimaries(G4Event *pEvent)
{
	fSeeds[0] = *(CLHEP::HepRandom::getTheSeeds());
	fSeeds[1] = *(CLHEP::HepRandom::getTheSeeds()+1);
	
	/*
	G4cout << "------------------------------------------------------" << G4endl;
	G4cout << "\nXurich2PrimaryGeneratorActionOptPh::GeneratePrimaries:" << G4endl;
	fParticleSource->PrintParticle();
	fParticleSource->PrintDirection();
	fParticleSource->PrintPolar();
	G4cout << "\nNumber of primaries per event: " << fParticleSource->GetPrimNb() << G4endl;
	*/
	
	fParticleSource->GeneratePrimaryVertex(pEvent);
}

