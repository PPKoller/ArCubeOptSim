#ifndef __PRIMARY_GENERATOR_ACTION_OPT_PH_HH__
#define __PRIMARY_GENERATOR_ACTION_OPT_PH_HH__

#include "ParticleSource.hh"

#include "globals.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"



class G4Event;

class PrimaryGeneratorActionOptPh: public G4VUserPrimaryGeneratorAction
{
public:
	PrimaryGeneratorActionOptPh();
	virtual ~PrimaryGeneratorActionOptPh();

public:
	
	ParticleSourceOptPh* GetParticleSource() const {return fParticleSource;};
	
	const long *GetEventSeeds() const { return fSeeds; };
	const G4String &GetParticleTypeOfPrimary() const { return fParticleTypeOfPrimary; };
	G4double GetEnergyOfPrimary() const { return fEnergyOfPrimary; };
	G4ThreeVector GetPositionOfPrimary() const { return fPositionOfPrimary; };
	
	
	//This is not a simple getter
	G4int GetPrimNb();
	
	//This controls the Particle source settings to match the fPrimNb
	void SetPrimaries();
	
	//This should be used only by the Particle source (otherwise it is very unsafe)
	void SetPrimNb(G4int primNb){fPrimNb=primNb;};
	
	
	
	const G4ThreeVector& GetPrimPos()const{return fParticleSource->GetPrimPos();};
	const vector<G4ParticleMomentum>& GetPrimMom()const{return fParticleSource->GetPrimMom();};
	const vector<G4ThreeVector>& GetPrimPol()const{return fParticleSource->GetPrimPol();};
	
	
	
	virtual void GeneratePrimaries(G4Event *pEvent);

private:
	long fSeeds[2];
	G4int fPrimNb;
	G4String fParticleTypeOfPrimary;
	G4double fEnergyOfPrimary;
	G4ThreeVector fPositionOfPrimary;

	ParticleSourceOptPh *fParticleSource;
	//PrimGenActOptPhMessanger *fMessenger;
};

#endif
