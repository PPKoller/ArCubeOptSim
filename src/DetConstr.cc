#include "DetConstr.hh"
#include "DetectorMessenger.hh"
#include "OptPropManager.hh"
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
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"


#include <vector>
#include <map>
#include <numeric>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cassert>

using std::vector;
using std::map;
using std::stringstream;
using std::max;
using std::ifstream;
using std::ofstream;


//map<G4String, G4double> DetConstrOptPh::fGeometryParams;


/////////////////////////////////////////////////////////////////////////////////////////
// constructor

DetConstrOptPh::DetConstrOptPh(G4String gdmlfilename)
{
	fWorld = NULL;
	fDetectorMessenger = NULL;
	
	fGDMLParser = new G4GDMLParser;
	
	{
		ifstream gdmlfile(gdmlfilename.c_str());
		if(gdmlfile){
			fGDMLParser->Read(gdmlfilename);
			fWorld = fGDMLParser->GetWorldVolume();
		}
	}
	
	if(fWorld){
		fDetectorMessenger = new DetectorMessenger(this);
	}
	
	fOptPropManager = OptPropManager::GetInstance();
	
	if(!fOptPropManager){
		G4Exception("DetConstrOptPh::DetConstrOptPh(...)","Geom.001", FatalException,"Cannot get \"OptPropManager\" pointer.");
	}
	
	fVerbose = 0;
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
		BuildDefaultOpticalSurfaces();
		DefaultOptProperties();
	}
	
	if(fVerbose>0) G4cout << "Info --> DetConstrOptPh::Construct(): Finished construction "<<G4endl;
	
	
	//G4cout<<"World returned"<<G4endl;
	return fWorld;
}



/////////////////////////////////////////////////////////////////////////////////////////
void DetConstrOptPh::DefaultOptProperties()
{
	G4double opt_ph_en[1] = {9.69*eV};
	G4double lar_rindex[1] = {1.369}; //From Bordoni et al (2019), https://doi.org/10.1016/j.nima.2018.10.082
	G4double lar_rayleigh_len[1] = {0.91*m}; //From Bordoni et al (2019), https://doi.org/10.1016/j.nima.2018.10.082
	G4double lar_abs_len[1] = {10*m}; //Depends on the purity
	
	if(!fOptPropManager){
		G4Exception("DetConstrOptPh::DefaultOptProperties()","Geom.002", FatalException,"\"OptPropManager\" pointer is null.");
	}
	
	fOptPropManager->SetMaterialRindex("LAr", 1, opt_ph_en, lar_rindex ); 
	fOptPropManager->SetMaterialAbsLenght("LAr", 1, opt_ph_en, lar_abs_len );
	fOptPropManager->SetMaterialRayleighLenght("LAr", 1, opt_ph_en, lar_rayleigh_len );
	
	
	G4double tpb_rindex[1] = {1.67}; //From Benson et al (2018), https://doi.org/10.1140/epjc/s10052-018-5807-z
	//G4double tpb_rayleigh_len[1] = {};
	G4double tpb_qe[1] = {0.58}; //Quantum efficiency of VUV WLS. From Benson et al (2018), https://doi.org/10.1140/epjc/s10052-018-5807-z
	G4double tpb_abs_len[1] = {400/(1.-tpb_qe[0])*nm}; //From Benson et al (2018), https://doi.org/10.1140/epjc/s10052-018-5807-z
  G4double tpb_wls_abs_len[1] = {400/tpb_qe[0]*nm};
  G4double tpb_wls_emission[1] = {425*nm};
  G4double tpb_wls_delay[1] = {0.5*ns};

	fOptPropManager->SetMaterialRindex("TPB", 1, opt_ph_en, tpb_rindex );
	fOptPropManager->SetMaterialAbsLenght("TPB", 1, opt_ph_en, tpb_abs_len );
	fOptPropManager->SetMaterialWLSAbsLenght("TPB", 1, opt_ph_en, tpb_wls_abs_len );
  fOptPropManager->SetMaterialWLSEmission("TPB", 1, opt_ph_en, tpb_wls_emission );
  fOptPropManager->SetMaterialWLSDelay("TPB", tpb_wls_delay);
	//SetQE("TPB", 1, G4double opt_ph_en[] = {9.69*eV}, tpb_qe );

	fOptPropManager->SetSurfSigmaAlpha("LAr2TPB_logsurf", 0.1);
	fOptPropManager->SetSurfSigmaAlpha("TPB2LAr_logsurf", 0.1);
	
	
}


void DetConstrOptPh::BuildDefaultOpticalSurfaces()
{
	//By default the EJ28 WLS does't have optical properties.
	//They can be defined later
	
	G4VPhysicalVolume *vol1, *vol2;
	
	
	// --------------------------------------//
	//  Interface between LAr and TPB layer  //
	// --------------------------------------//
	vol1 = G4PhysicalVolumeStore::GetInstance()->GetVolume("volTPCActive_PV");
	vol2 = G4PhysicalVolumeStore::GetInstance()->GetVolume("volTPB_PV");
	if( vol1 && vol2){
		
		//By the default the surfaces are with ground finish
		//The reflectivity is calculated by the Fresnel law
		//No reflection properties are defined ==> lambertian reflection is selected in this default
		
		G4OpticalSurface* LAr2TPB_optsurf = new G4OpticalSurface("LAr2TPB_optsurf", unified, ground, dielectric_dielectric);
		
		G4LogicalBorderSurface* LAr2TPB_logsurf = new G4LogicalBorderSurface("LAr2TPB_logsurf",vol1,vol2,LAr2TPB_optsurf);
		
		LAr2TPB_optsurf -> SetMaterialPropertiesTable( new G4MaterialPropertiesTable() );
		
		
		//Make the optical surface from TPB to LAr
		
		G4OpticalSurface* TPB2LAr_optsurf = new G4OpticalSurface("TPB2LAr_optsurf", unified, ground, dielectric_dielectric);
		
		G4LogicalBorderSurface* TPB2LAr_logsurf = new G4LogicalBorderSurface("TPB2LAr_logsurf",vol2,vol1,TPB2LAr_optsurf);
		
		TPB2LAr_optsurf -> SetMaterialPropertiesTable( new G4MaterialPropertiesTable() );
		
		
	}//End of interface between LAr and ArCLight TPB coating
	
	
	
	// --------------------------------------//
	//  Interface between TPB and EJ280 WLS  //
	// --------------------------------------//
	vol1 = G4PhysicalVolumeStore::GetInstance()->GetVolume("volTPB_PV");
	vol2 = G4PhysicalVolumeStore::GetInstance()->GetVolume("volWLS_PV");
	if( vol1 && vol2){
		
		//As a default the surface is defined as polished and front painted.
		//This allow for some reflection while the non reflected photons are simply absorbed by the painting.
		//Ideally this surface should be defined as dielectric_dichroic whic can be implemented later by the user.
		
		G4OpticalSurface* TPB2EJ280_optsurf = new G4OpticalSurface("TPB2EJ280_optsurf", unified, polishedfrontpainted, dielectric_dielectric);
		
		G4LogicalBorderSurface* TPB2EJ280_logsurf = new G4LogicalBorderSurface("TPB2EJ280_logsurf",vol1,vol2,TPB2EJ280_optsurf);


    //Make the optical surface from EJ280 WLS to TPB
		
		G4OpticalSurface* EJ2802TPB_optsurf = new G4OpticalSurface("EJ2802TPB_optsurf", unified, polished, dielectric_metal);
		
		G4LogicalBorderSurface* EJ2802TPB_logsurf = new G4LogicalBorderSurface("EJ2802TPB_logsurf",vol2,vol1,EJ2802TPB_optsurf);

		
	}//End of interface between EJ280 WLS and ArCLight TPB coating
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
		return;
	}else{
		G4cout << "There are " << vPhysVols.size() << " instances of the physical volume \"" << hVolName << "\"" << G4endl;
	}
	
	G4cout << G4endl << G4endl;
	
	/*
	G4double mass = (vPhysVols..at(0)->GetLogicalVolume()->GetMass(false,false))/kg;
	G4double density = (pPhysVol->GetLogicalVolume()->GetMaterial()->GetDensity())/(kg/m3);
	G4double volume = mass/density;
	G4cout << "Mass of physical volume \"" << pPhysVol->GetName() << "\" = " << mass << " kg" << G4endl;
	G4cout << "Volume of physical volume \"" << pPhysVol->GetName() << "\" = " << volume << " m^3" << G4endl;
	G4cout << G4endl << G4endl;
	*/
	
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
