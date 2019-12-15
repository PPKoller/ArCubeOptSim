
/////////////////////////////////////////////////////////////////////////////////////////
// Geometry of Xurich TPC
// March 29, 2016

// must load g4.9.4 first

// updates
// updated header files below and associated .cc files for XurichDet
// check materials are called out and consistent: ok except SS316 is used, not 304 
// commented out meshes, April 25, 2016
// changed cryostat volume from vac to LXe to avoid potential boundary problems

// note on prefixes: 
//   d is for dimensions
//   m_ is for member class
//   pos is for position (nothing to do with p for pointer!) to more easily distiguish from the dimensions

// namimg notes:
//   "Name" describes the component
//   NameVol  (volume defined by shape and dimensions)
//   NameLogVol (logical volume)
//   NamePhysVol (physical volume)


/////////////////////////////////////////////////////////////////////////////////////////


#include "DetConstr.hh"
#include "DetectorMessenger.hh"
//#include "SensitiveDetectors.hh"

#include "globals.hh"
#include "G4SystemOfUnits.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4SDManager.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4EllipticalCone.hh"
#include "G4Trd.hh"
#include "G4Sphere.hh"
#include "G4Torus.hh"
#include "G4Polyhedra.hh"
#include "G4Polycone.hh"
#include "G4Ellipsoid.hh"
#include "G4ExtrudedSolid.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"

#include "G4OpBoundaryProcess.hh"



#include <vector>
#include <numeric>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cassert>

using std::vector;
using std::stringstream;
using std::max;
using std::ifstream;
using std::ofstream;


//map<G4String, G4double> DetConstrOptPh::fGeometryParams;


//using namespace CLHEP;

/////////////////////////////////////////////////////////////////////////////////////////
// constructor

DetConstrOptPh::DetConstrOptPh(G4String gdmlfilename):fWorld(NULL), fDetectorMessenger(NULL)
{
	fGDMLParser = new G4GDMLParser;
	
	{
		ifstream gdmlfile(gdmlfilename.c_str());
		if(gdmlfile){
			fGDMLParser->Read(gdmlfilename);
			fWorld = fGDMLParser->GetWorldVolume();
		}
	}
	
	if(fWorld) fDetectorMessenger = new DetectorMessenger(this);
	
	
}


/////////////////////////////////////////////////////////////////////////////////////////
// destructor

DetConstrOptPh::~DetConstrOptPh()
{
	if(fDetectorMessenger) delete fDetectorMessenger;
	delete fGDMLParser;
}


/////////////////////////////////////////////////////////////////////////////////////////

G4VPhysicalVolume *DetConstrOptPh::Construct()
{
	if(fWorld){
		DefineOptProperties();
	}
	
	G4cout<<"Finished construction "<<G4endl;
	//G4cout<<" Push any button to continue "<<G4endl;
	
	
	//G4cout<<"World returned"<<G4endl;
	return fWorld;
}



/////////////////////////////////////////////////////////////////////////////////////////
void DetConstrOptPh::DefineOptProperties()
{
	
}


void SetLArRindex(G4double dRindex)
{
	
}


/////////////////////////////////////////////////////////////////////////////////////////
void DetConstrOptPh::SetLArAbsLen(G4double dLength)
{
    G4Material *pLArMaterial = G4Material::GetMaterial(G4String("LiquidArgon"));
    
    if(pLArMaterial)
    {
        G4cout << "----> Setting LAr absorbtion length to " << dLength/cm << " cm" << G4endl;
        
        G4MaterialPropertiesTable *pLArPropertiesTable = pLArMaterial->GetMaterialPropertiesTable();
        
        const G4int iNbEntries = 1;
        
        G4double LAr_PP[iNbEntries] = {9.69*eV}; //128 nm optical photons
        G4double LAr_ABSL[iNbEntries] = {dLength};
        pLArPropertiesTable->RemoveProperty("ABSLENGTH");
        pLArPropertiesTable->AddProperty("ABSLENGTH", LAr_PP, LAr_ABSL, iNbEntries);
    }
    else
    {
        G4cout << "ls!> LAr materials not found!" << G4endl;
        //exit(-1);
    }
}


/////////////////////////////////////////////////////////////////////////////////////////
void DetConstrOptPh::SetLArRayleighScLen(G4double dLength)
{
    G4Material *pLArMaterial = G4Material::GetMaterial(G4String("LiquidArgon"));
    
    if(pLArMaterial){
        
        G4cout << "----> Setting LAr scattering length to " << dLength/cm << " cm" << G4endl;
        
        G4MaterialPropertiesTable *pLArPropertiesTable = pLArMaterial->GetMaterialPropertiesTable();
        
        const G4int iNbEntries = 1;
        
        G4double LAr_PP[iNbEntries] = {6.91*eV};
        G4double LAr_RAYLSC[iNbEntries] = {dLength};
        pLArPropertiesTable->RemoveProperty("RAYLEIGH");
        pLArPropertiesTable->AddProperty("RAYLEIGH", LAr_PP, LAr_RAYLSC, iNbEntries);
    }else{
        G4cout << "ls!> LAr materials not found!" << G4endl;
        //exit(-1);
    }
}


/////////////////////////////////////////////////////////////////////////////////////////
void DetConstrOptPh::SetG10Refl(G4double dReflectivity){
	
}


void DetConstrOptPh::SetG10Rindex(G4double dReflectivity){
	
}


void DetConstrOptPh::SetResKaptonRefl(G4double dReflectivity){
	
}


void DetConstrOptPh::PrintVolumeCoordinates(G4String hVolName)
{
	if(hVolName==G4String("")){
		G4cout << "Physical volume name not set!!!" << G4endl;
		return;
	}
	
	G4PhysicalVolumeStore* pPhysVolStore = G4PhysicalVolumeStore::GetInstance();
	
	G4int Nvolumes = pPhysVolStore->size();
	
	//Take the physical volume pointer you want
	std::vector<G4VPhysicalVolume *> vPhysVols;
	for(G4int i=0; i<Nvolumes; i++){
		G4String tmp_name = pPhysVolStore->at(i)->GetName();
		if(tmp_name==hVolName){
			vPhysVols.push_back(pPhysVolStore->at(i));
		}
	}
	
	if(vPhysVols.size()==0){
		G4cout << "Physical Volume \"" << hVolName << "\" not found!!!" << G4endl;
		return(0);
	}else{
		G4cout << "There are " << vPhysVols.size() << " instances of the physical volume \"" << hVolName << "\"" << G4endl;
	}
	
	G4cout << G4endl << G4endl;
	
	G4double mass = (pPhysVol->GetLogicalVolume()->GetMass(false,false))/kg;
	G4double density = (pPhysVol->GetLogicalVolume()->GetMaterial()->GetDensity())/(kg/m3);
	G4double volume = mass/density;
	G4cout << "Mass of physical volume \"" << pPhysVol->GetName() << "\" = " << mass << " kg" << G4endl;
	G4cout << "Volume of physical volume \"" << pPhysVol->GetName() << "\" = " << volume << " m^3" << G4endl;
	G4cout << G4endl << G4endl;
	
	
	for(G4int iVol=0; iVol<vPhysVols.size(); iVol++){
		G4cout << "\n\nInstance " << iVol << ":" << G4endl;
		
		G4VPhysicalVolume *pPhysVol = vPhysVols.at(iVol);
		
		G4ThreeVector ShiftGlob = pPhysVol->GetTranslation();
		
		while( pPhysVol->GetMotherLogical() ){
			//Find the mother phys volume
			G4VPhysicalVolume *pAncPhysVol;
			
			for(G4int i=0; i<Nvolumes; i++){
				if(pPhysVolStore->at(i)->GetLogicalVolume()->IsDaughter(pPhysVol)){
					pAncPhysVol = pPhysVolStore->at(i);
				}
			}
			G4cout << "Shift of physical volume \"" << pPhysVol->GetName() << "\" with respect to \"" << pAncPhysVol->GetName() << "\" =  " << pPhysVol->GetTranslation()/mm << " mm" << G4endl;
			pPhysVol = pAncPhysVol;
			ShiftGlob = ShiftGlob + pPhysVol->GetTranslation();
		}
		
		G4cout << "\nGlobal shift of physical volume \"" << hVolName << "\" (inst. " << iVol << ") =  " << ShiftGlob/mm << " mm" << G4endl;
		
	}
	
	return;
}


void DetConstrOptPh::PrintListOfPhysVols()
{
	if(!fWorld) return;
	
	G4PhysicalVolumeStore* pPhysVolStore = G4PhysicalVolumeStore::GetInstance();
	
	if(!pPhysVolStore) return;
	
	G4int nvols = pPhysVolStore->size();
	
	
	G4cout << "\nList of Physical Volumes (PV, LV, MV)" << G4endl;
	
	for(G4int ivol=0; ivol<nvols; ivol++){
		G4LogicalVolume *pMotherVol = pPhysVolStore->at(ivol)->GetMotherLogical();
		if(pMotherVol){
			G4cout << "PV: " << pPhysVolStore->at(ivol)->GetName() << "\tLV: " << pPhysVolStore->at(ivol)->GetLogicalVolume()->GetName() << "\tMV: " << pPhysVolStore->at(ivol)->GetMotherLogical()->GetName() << G4endl;
		}else{
			G4cout << "PV: " << pPhysVolStore->at(ivol)->GetName() << "\tLV: " << pPhysVolStore->at(ivol)->GetLogicalVolume()->GetName() << "\tMV: None" << G4endl;
		}
	}
	
	G4cout << "\nTotal number of physical volumes registered: " << nvols << G4endl;
	
	
}


void DetConstrOptPh::PrintListOfLogVols()
{
	if(!fWorld) return;
	
	G4LogicalVolumeStore* pLogVolStore = G4LogicalVolumeStore::GetInstance();
	
	if(!pLogVolStore) return;
	
	G4int nvols = pLogVolStore->size();
	
	
	G4cout << "\nList of Logical Volumes (LV, Material)" << G4endl;
	
	for(G4int ivol=0; ivol<nvols; ivol++){
		G4cout << "LG: " << pLogVolStore->at(ivol)->GetName() << "\tMaterial: " << pLogVolStore->at(ivol)->GetMaterial()->GetName() << G4endl;
	}
	
	G4cout << "\nTotal number of logical volumes registered: " << nvols << G4endl;
	
	
}
