#ifndef __DET_CONSTR_HH__
#define __DET_CONSTR_HH__

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"

#include "G4GDMLParser.hh"

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
	
	void SetLArRindex(G4double dRindex);
	void SetLArAbsLen(G4double dLength);
	void SetLArRayleighScLen(G4double dLength);
	
	void SetG10Refl(G4double dReflectivity);
	void SetG10Rindex(G4double dRindex);
	
	void SetResKaptonRefl(G4double dReflectivity);//Reflectivity of the G10 laminated with the kapton resistive foil
	
	void SetArCLightRefl(G4double dReflectivity);//This is actually the reflectivity of the TPB coated on the ArCLight
	void SetArCLightSurfRough(G4double dAlpha);
	
	
	void SetVerbosity(G4int _verb){fVerbosity=_verb;};
	
	
	// functions to communicate with DetectorMessenger
	//void SetCheckOverlap(G4bool dCheckOverlap){pCheckOverlap = dCheckOverlap;};
	
	//static bool GetGeometryParameter(G4String szParameter, G4double& outval);
	
	void PrintVolumeCoordinates(G4String VolName);
	
	void PrintListOfPhysVols();
	void PrintListOfLogVols();
	
private:
	//Here also all the optical surfaces are defined
	void DefaultOptProperties();
	
	
private:
	
	//G4bool   pCheckOverlap;

	//static map<G4String, G4double> fGeometryParams;
	//map<G4String, G4LogicalVolume*> fLogVolumes;
	
	G4int fVerbose;
	
	G4GDMLParser *fGDMLParser;
	G4VPhysicalVolume *fWorld;
	DetectorMessenger *fDetectorMessenger;
	
	//
};


#endif
