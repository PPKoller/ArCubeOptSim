
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

DetConstrOptPh::DetConstrOptPh(G4String gdmlfilename):fWorld(NULL), fDetectorMessenger(NULL), fVerbose(0)
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
		BuildOpticalSurfaces();
		DefaultOptProperties();
	}
	
	G4cout<<"Finished construction "<<G4endl;
	//G4cout<<" Push any button to continue "<<G4endl;
	
	
	//G4cout<<"World returned"<<G4endl;
	return fWorld;
}



/////////////////////////////////////////////////////////////////////////////////////////
void DetConstrOptPh::DefaultOptProperties()
{
	SetLArRindex(1.369); //From Bordoni et al (2019), https://doi.org/10.1016/j.nima.2018.10.082
	SetLArRayleighScLen(91.0); //From Bordoni et al (2019), https://doi.org/10.1016/j.nima.2018.10.082
	
	SetLArAbsLen(10*m); //Depends on the purity
	
	SetResKaptonRefl(0); //By default all the photons are absorbed
	
	
	
	
	SetArCLightRefl(0); //Absorbs all the VUV photons
	
	
}


void BuildOpticalSurfaces()
{
	//volTPB_PV, volTPB -> volDC (volDC_PV) -> volWLS (volWLS_PV) -> volArCLight (volArCLight_PV) -> volOpticalDet (volOpticalDet_PV)
}


void DetConstrOptPh::SetLArRindex(G4double dRindex)
{
	G4Material *pLArMaterial = G4Material::GetMaterial(G4String("LAr"));
	
	if(!pLArMaterial){
		G4cout << "ERROR ---> DetConstrOptPh::SetLArRindex(...): LAr materials not found!" << G4endl;
		return;
	}
	
	G4MaterialPropertiesTable *pLArPropertiesTable = pLArMaterial->GetMaterialPropertiesTable();
	
	if(fVerbosity>1) G4cout << "Debug ---> DetConstrOptPh::SetLArRindex(...): Setting LAr refraction index to " << dRindex << G4endl;
	
	const G4int iNbEntries = 1;
	
	G4double LAr_PP[iNbEntries] = {9.69*eV}; //128 nm optical photons
	G4double LAr_RIND[iNbEntries] = {dRindex};
	pLArPropertiesTable->RemoveProperty("RINDEX");
	pLArPropertiesTable->AddProperty("RINDEX", LAr_PP, LAr_RIND, iNbEntries);
	
}


/////////////////////////////////////////////////////////////////////////////////////////
void DetConstrOptPh::SetLArAbsLen(G4double dLength)
{
	G4Material *pLArMaterial = G4Material::GetMaterial(G4String("LAr"));
	
	if(pLArMaterial){
		if(fVerbosity>1) G4cout << "Debug ---> DetConstrOptPh::SetLArAbsLen(...): Setting LAr absorbtion length to " << dLength/cm << " cm" << G4endl;
		
		G4MaterialPropertiesTable *pLArPropertiesTable = pLArMaterial->GetMaterialPropertiesTable();
		
		const G4int iNbEntries = 1;
		
		G4double LAr_PP[iNbEntries] = {9.69*eV}; //128 nm optical photons
		G4double LAr_ABSL[iNbEntries] = {dLength};
		pLArPropertiesTable->RemoveProperty("ABSLENGTH");
		pLArPropertiesTable->AddProperty("ABSLENGTH", LAr_PP, LAr_ABSL, iNbEntries);
	}else{
		G4cout << "ls!> LAr materials not found!" << G4endl;
		//exit(-1);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////
void DetConstrOptPh::SetLArRayleighScLen(G4double dLength)
{
    G4Material *pLArMaterial = G4Material::GetMaterial(G4String("LAr"));
    
    if(pLArMaterial){
        
        if(fVerbosity>1) G4cout << "Debug ---> DetConstrOptPh::SetLArRayleighScLen(...): Setting LAr scattering length to " << dLength/cm << " cm" << G4endl;
        
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
void DetConstrOptPh::SetG10Refl(G4double dReflectivity)
{
	G4Material *pMaterial = G4Material::GetMaterial(G4String("G10"));
	
	if(!pMaterial){
		G4cout << "ERROR ---> DetConstrOptPh::SetG10Refl(...): G10 material not found!" << G4endl;
		return;
	}
	
	G4MaterialPropertiesTable *pPropertiesTable = pMaterial->GetMaterialPropertiesTable();
	
	if(fVerbosity>1) G4cout << "Debug ---> DetConstrOptPh::SetG10Refl(...): Setting G10 reflectivity index to " << dRindex << G4endl;
	
	const G4int iNbEntries = 1;
	
	G4double PP[iNbEntries] = {9.69*eV}; //128 nm optical photons
	G4double REFL[iNbEntries] = {dRindex};
	pPropertiesTable->RemoveProperty("REFLECTIVITY");
	pPropertiesTable->AddProperty("REFLECTIVITY", PP, REFL, iNbEntries);
}


void DetConstrOptPh::SetG10Rindex(G4double dReflectivity)
{
	G4Material *pMaterial = G4Material::GetMaterial(G4String("G10"));
	
	if(!pMaterial){
		G4cout << "ERROR ---> DetConstrOptPh::SetG10Rindex(...): G10 materials not found!" << G4endl;
		return;
	}
	
	G4MaterialPropertiesTable *pPropertiesTable = pMaterial->GetMaterialPropertiesTable();
	
	if(fVerbosity>1) G4cout << "Debug ---> DetConstrOptPh::SetG10Rindex(...): Setting G10 refraction index to " << dRindex << G4endl;
	
	const G4int iNbEntries = 1;
	
	G4double PP[iNbEntries] = {9.69*eV}; //128 nm optical photons
	G4double RIND[iNbEntries] = {dRindex};
	pPropertiesTable->RemoveProperty("RINDEX");
	pPropertiesTable->AddProperty("RINDEX", PP, RIND, iNbEntries);
}


void DetConstrOptPh::SetResKaptonRefl(G4double dReflectivity)
{
	G4Material *pMaterial = G4Material::GetMaterial(G4String("G10kapton"));
	
	if(!pMaterial){
		G4cout << "ERROR ---> DetConstrOptPh::SetResKaptonRefl(...): G10kapton material not found!" << G4endl;
		return;
	}
	
	G4MaterialPropertiesTable *pPropertiesTable = pMaterial->GetMaterialPropertiesTable();
	
	if(fVerbosity>1) G4cout << "Debug ---> DetConstrOptPh::SetResKaptonRefl(...): Setting G10kapton reflectivity index to " << dRindex << G4endl;
	
	const G4int iNbEntries = 1;
	
	G4double PP[iNbEntries] = {9.69*eV}; //128 nm optical photons
	G4double REFL[iNbEntries] = {dRindex};
	pPropertiesTable->RemoveProperty("REFLECTIVITY");
	pPropertiesTable->AddProperty("REFLECTIVITY", PP, REFL, iNbEntries);
}


void DetConstrOptPh::SetArCLightSurfRough(G4double dAlpha)
{
	
}


void DetConstrOptPh::SetFF4Refl(G4double dReflectivity)
{
	G4Material *pMaterial = G4Material::GetMaterial(G4String("FF4"));
	
	if(!pMaterial){
		G4cout << "ERROR ---> DetConstrOptPh::SetFF4Refl(...): FF4 material not found!" << G4endl;
		return;
	}
	
	G4MaterialPropertiesTable *pPropertiesTable = pMaterial->GetMaterialPropertiesTable();
	
	if(fVerbosity>1) G4cout << "Debug ---> DetConstrOptPh::SetFF4Refl(...): Setting FF4 reflectivity index to " << dRindex << G4endl;
	
	const G4int iNbEntries = 1;
	
	G4double PP[iNbEntries] = {9.69*eV}; //128 nm optical photons
	G4double REFL[iNbEntries] = {dRindex};
	pPropertiesTable->RemoveProperty("REFLECTIVITY");
	pPropertiesTable->AddProperty("REFLECTIVITY", PP, REFL, iNbEntries);
}


void DetConstrOptPh::SetFF4Rindex(G4double dReflectivity)
{
	G4Material *pMaterial = G4Material::GetMaterial(G4String("FF4"));
	
	if(!pMaterial){
		G4cout << "ERROR ---> DetConstrOptPh::SetFF4Rindex(...): FF4 materials not found!" << G4endl;
		return;
	}
	
	G4MaterialPropertiesTable *pPropertiesTable = pMaterial->GetMaterialPropertiesTable();
	
	if(fVerbosity>1) G4cout << "Debug ---> DetConstrOptPh::SetFF4Rindex(...): Setting FF4 refraction index to " << dRindex << G4endl;
	
	const G4int iNbEntries = 1;
	
	G4double PP[iNbEntries] = {9.69*eV}; //128 nm optical photons
	G4double RIND[iNbEntries] = {dRindex};
	pPropertiesTable->RemoveProperty("RINDEX");
	pPropertiesTable->AddProperty("RINDEX", PP, RIND, iNbEntries);
}


void DetConstrOptPh::PrintVolumeCoordinates(G4String hVolName)
{
	if(hVolName==G4String("")){
		G4cout << "Physical volume name not set!!!" << G4endl;
		return;
	}
	
	G4PhysicalVolumeStore* pPhysVolStore = G4PhysicalVolumeStore::GetInstance();
	
	G4int Nvolumes = pPhysVolStore->size();
	
	G4VPhysicalVolume *pPhysVol = NULL;
	for(G4int i=0; i<Nvolumes; i++){
		G4String tmp_name = pPhysVolStore->at(i)->GetName();
		if(tmp_name==hVolName){
			pPhysVol = pPhysVolStore->at(i);
		}
	}
	
	if(!pPhysVol){
		G4cout << "Physical Volume \"" << hVolName << "\" not found!!!" << G4endl;
		return;
	}
	
	G4cout << G4endl << G4endl;
	
	G4ThreeVector ShiftGlob = pPhysVol->GetTranslation();

	while(pPhysVol->GetMotherLogical()){
		//Find the mother phys volume
		G4VPhysicalVolume *pAncPhysVol;
	
		for(G4int i=0; i<Nvolumes; i++){
			if(pPhysVolStore->at(i)->GetLogicalVolume()->IsDaughter(pPhysVol)){
				pAncPhysVol = pPhysVolStore->at(i);
			}
		}
		G4cout << "Shift of physical volume \"" << pPhysVol->GetName() << "\" respect to \"" << pAncPhysVol->GetName() << "\" =  " << pPhysVol->GetTranslation()/mm << " mm" << G4endl;
		pPhysVol = pAncPhysVol;
		ShiftGlob = ShiftGlob + pPhysVol->GetTranslation();
	}

	G4cout << G4endl << "Global shift of physical volume \"" << hVolName << "\" =  " << ShiftGlob/mm << " mm" << G4endl;
	
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
