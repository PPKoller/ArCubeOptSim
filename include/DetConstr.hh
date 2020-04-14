#ifndef __DET_CONSTR_HH__
#define __DET_CONSTR_HH__

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"

#include "G4GDMLParser.hh"
#include "G4Material.hh"

#include <vector>
#include <map>


using std::vector;
using std::map;


class G4String;
class G4Colour;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4SubtractionSolid;
class DetectorMessenger;
class OptPropManager;


class DetConstrOptPh: public G4VUserDetectorConstruction
{
public:
	
	enum verbosity{
		kSilent,
		kInfo,
		kDetails,
		kDebug
	};
	
	DetConstrOptPh(G4String gdmlfilename);
	virtual ~DetConstrOptPh();
	
	G4VPhysicalVolume* Construct();
	
	
	const G4GDMLParser* GetGdmlParser() const {return fGDMLParser;};
	const G4VPhysicalVolume* GetWorldVolume() const {return fWorld;};
	
	
	inline void SetVerbosity(DetConstrOptPh::verbosity verb){fVerbose=verb;};
	inline DetConstrOptPh::verbosity GetVerbosity(){return fVerbose;};
	
	inline void SetTpbThickness(G4double thick){fTpbThick = thick;};
	inline G4double GetTpbThickness(){return fTpbThick;}
	
	// functions used moslty by the DetectorMessenger
	//void SetCheckOverlap(G4bool dCheckOverlap){pCheckOverlap = dCheckOverlap;};
	
	//static bool GetGeometryParameter(G4String szParameter, G4double& outval);
	
	void PrintVolumeCoordinates(G4String VolName);
	
	void PrintListOfPhysVols();
	void PrintListOfLogVols();
	
protected:
	//These methods are only used at the startup as default (and to debug)
	//They should go away when the full user interface for optical setting
	//Here also all the optical surfaces are defined
	virtual void BuildTPBlayer();
	virtual void BuildDefaultOpticalSurfaces();
	virtual void DefaultOptProperties();
	virtual G4Material* FindMaterial(G4String matname);
	
private:
	
	DetectorMessenger *fDetectorMessenger;
	
	OptPropManager *fOptPropManager;
	
	G4GDMLParser *fGDMLParser;
	G4VPhysicalVolume *fWorld;
	
	DetConstrOptPh::verbosity fVerbose;
	
	
	G4double fTpbThick;
};





#endif
