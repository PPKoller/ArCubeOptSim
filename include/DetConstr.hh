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


class DetConstrOptPh: public G4VUserDetectorConstruction
{
public:
	DetConstrOptPh(G4String gdmlfilename);
	virtual ~DetConstrOptPh();
	
	G4VPhysicalVolume* Construct();
	
	
	const G4GDMLParser* GetGdmlParser() const {return fGDMLParser;};
	const G4VPhysicalVolume* GetWorldVolume() const {return fWorld;};
	
	
	inline void SetVerbosity(G4int _verb){fVerbose=_verb;};
	
	
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
	virtual void BuildDefaultOpticalSurfaces();
	virtual void DefaultOptProperties();
	
	
private:
	
	DetectorMessenger *fDetectorMessenger;
	
	G4GDMLParser *fGDMLParser;
	G4VPhysicalVolume *fWorld;
	
	G4int fVerbose;
};





#endif
