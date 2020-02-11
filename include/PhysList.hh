#ifndef __PHYS_LIST_HH__
#define __PHYS_LIST_HH__

#include <G4VUserPhysicsList.hh>
#include <globals.hh>

//Forward declarations


class G4Scintillation;
class G4OpAbsorption;
class G4OpRayleigh;
class G4OpWLS;
class G4OpMieHG;
class G4OpBoundaryProcess;

class PhysListOptPhMessenger;


class PhysListOptPh: public G4VUserPhysicsList
{
public:
	PhysListOptPh();
	virtual ~PhysListOptPh();
	
	void SetCuts();
	
	virtual void ConstructParticle();
	virtual void ConstructProcess();
	
	//for the Messenger
	void SetVerbose(G4int verb){fVerboseLevel=verb;};
	

protected:
	
	

private:
	
	void ConstructMyBosons();
	void ConstructOptical();
	
	
	PhysListOptPhMessenger* fMessenger;
	
	//All the static declaration here are made for speed and low memory consumption in addition they are thread safe using the G4 internal functionalities
	//Note: All this static members are not shared between different threads but shared between different instances of the class in the same thread
	static G4ThreadLocal G4int fVerboseLevel;
	//static G4ThreadLocal G4int fOpVerbLevel;
	
	static G4ThreadLocal G4OpAbsorption* fAbsorptionProcess;
	static G4ThreadLocal G4OpRayleigh* fRayleighScatteringProcess;
	static G4ThreadLocal G4OpWLS* fWLSProcess;
	//static G4ThreadLocal G4OpMieHG* fMieHGScatteringProcess;
	static G4ThreadLocal G4OpBoundaryProcess* fBoundaryProcess;
	
	static const G4double fDefaultCutValue;
};

#endif

