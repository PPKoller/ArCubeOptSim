#ifndef __PARTICLE_SOURCE_OPT_PH_HH__
#define __PARTICLE_SOURCE_OPT_PH_HH__


#include "globals.hh"
#include "G4VPrimaryGenerator.hh"
#include "G4Navigator.hh"
#include "G4ParticleMomentum.hh"
#include "G4ParticleDefinition.hh"
#include "G4Track.hh"


#include <set>
#include <vector>

using std::set;
using std::vector;

class PrimaryGeneratorActionOptPh;
class ParticleSourceOptPhMessenger;

class ParticleSourceOptPh: public G4VPrimaryGenerator
{
public:
	ParticleSourceOptPh(PrimaryGeneratorActionOptPh *primGenAct, G4int nPrim=1, G4int verb=0);
	virtual ~ParticleSourceOptPh();
	
	
	//THIS IS THE CENTRAL FUNCTION OF ALL THE CLASS
	void GeneratePrimaryVertex(G4Event *pEvent);
	
	//This method is eventually used to switch to geantino (for debugging) otherwise the standard "opticalphoton" particle is defined already in the constructor
	void SetParticleDef(G4ParticleDefinition* aParticleDefinition);
	void SetKinEnergy(G4double KinEnergy);
	void SetDirection(G4ThreeVector aDirection){fMomentumDirection = aDirection.unit();};
	void SetMomentum(G4double aMomentum);
	void SetMomentum(G4ParticleMomentum aMomentum);
	
	void SetPosDisType(G4String hSourcePosType) { fSourcePosType = hSourcePosType; }
	void SetPosDisShape(G4String hShape) { fShape = hShape; }
	void SetCenterCoords(G4ThreeVector hCenterCoords) { fCenterCoords = hCenterCoords; }

	void SetHalfX(G4double dHalfx) { fHalfx = dHalfx; }
	void SetHalfY(G4double dHalfy) { fHalfy = dHalfy; }
	void SetHalfZ(G4double dHalfz) { fHalfz = dHalfz; }
	void SetRadius(G4double dRadius) { fRadius = dRadius; }
	
	
	inline void SetVerbosity(G4int iVerbosityLevel) { fVerbosityLevel = iVerbosityLevel; };
	
	inline void SetAngDistType(G4String hAngDistType) { fAngDistType = hAngDistType; }
	
	void SetPhotonPolar(G4ThreeVector hPol) { fPolarization = hPol.unit(); }
	void SetEnergyDisType(G4String type){fEnergyDisType=type;};
	
	void SetPrimNb(G4int nprim);
	G4int GetPrimNb()const{return fPrimNb;};
	
	const G4ThreeVector& GetPrimPos()const{return fPosPrim;};
	const vector<G4ParticleMomentum>& GetPrimMom()const{return fMomPrim;};
	const vector<G4ThreeVector>& GetPrimPol()const{return fPolPrim;};
	
	
	
	const G4String &GetParticleType()const{ return fParticleDefinition->GetParticleName(); };
	G4double GetTotalEnergy()const{ return fTotEnergy; }
	G4double GetKinEnergy()const{ return fKinEnergy; }
	G4double GetMomentum()const{ return fMom; }
	const G4ThreeVector &GetDirection()const{ return fMomentumDirection; }
	const G4ThreeVector &GetPosition()const{ return fPosition; }
	const G4ThreeVector &GetPolarization()const{ return fPolarization; }
	const G4String& GetPosDisType()const{ return fSourcePosType; };
	const G4String& GetAngDistrType()const{return fAngDistType;};
	
	
	void PrintParticle();
	void PrintDirection();
	void PrintPolar();

	void GeneratePointSource();
	void GeneratePointsInVolume();
	G4bool IsSourceConfined();
	void ConfineSourceToVolume(G4String);

	void GenerateIsotropic();

protected:
	virtual void SetInitialValues();
	
	G4int fPrimNb;
	G4ParticleDefinition *fParticleDefinition;
	
	G4String fPartName;
	G4double fMass, fCharge;
	G4double fTotEnergy, fKinEnergy, fMom;
	G4ParticleMomentum fMomentumDirection;
	G4ThreeVector fPolarization;
	
	G4ThreeVector fPosition;
	G4double fTime;
	
	
	G4String fSourcePosType;
	G4String fShape;
	G4ThreeVector fCenterCoords;
	G4double fHalfx;
	G4double fHalfy;
	G4double fHalfz;
	G4double fRadius;
	G4bool fConfine;
	set<G4String> fVolumeNames;
	G4String fAngDistType;
	G4String fEnergyDisType;
	
	vector<G4double> fEnPrim;
	G4ThreeVector fPosPrim;
	vector<G4ThreeVector> fPolPrim;
	vector<G4ParticleMomentum> fMomPrim;
	vector<G4String> fPhysVolPrim;
	
	G4int fVerbosityLevel;
	
private:
	PrimaryGeneratorActionOptPh *fPrimGenAct;
	ParticleSourceOptPhMessenger *fMessenger;
	G4Navigator *fNavigator;
};


#endif

