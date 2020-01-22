#ifndef OPT_PROP_MANAGER_CC
#define OPT_PROP_MANAGER_CC

#include "OptPropManager.hh"

#include "G4String.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"

#include <numeric>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cassert>



OptPropManager* OptPropManager::gThis = NULL;


//std::map<G4String, G4OpticalSurfaceModel>* OptPropManager::OptSurfModelMap = NULL;

//std::map<G4String, G4SurfaceType>* OptPropManager::OptSurfTypeMap = NULL;

//std::map<G4String, G4OpticalSurfaceFinish>* OptPropManager::OptSurfFinishMap = NULL;



OptPropManager::OptPropManager()
{
	//Map of the optical surfaces models
	(OptPropManager::OptSurfModelMap)["glisur"] = glisur;
	(OptPropManager::OptSurfModelMap)["unified"] = unified;
	(OptPropManager::OptSurfModelMap)["LUT"] = LUT;
	(OptPropManager::OptSurfModelMap)["DAVIS"] = DAVIS;
	(OptPropManager::OptSurfModelMap)["dichroic"] = dichroic;
	
	//Map of the optical surfaces types
	(OptPropManager::OptSurfTypeMap)["dielectric_metal"] = dielectric_metal;
	(OptPropManager::OptSurfTypeMap)["dielectric_dielectric"] = dielectric_dielectric;
	(OptPropManager::OptSurfTypeMap)["dielectric_LUT"] = dielectric_LUT;
	(OptPropManager::OptSurfTypeMap)["dielectric_LUTDAVIS"] = dielectric_LUTDAVIS;
	(OptPropManager::OptSurfTypeMap)["dielectric_dichroic"] = dielectric_dichroic;
	(OptPropManager::OptSurfTypeMap)["firsov"] = firsov;
	(OptPropManager::OptSurfTypeMap)["x_ray"] = x_ray;
	
	//Map of the optical surfaces finishes
	(OptPropManager::OptSurfFinishMap)["polished"] = polished;
	(OptPropManager::OptSurfFinishMap)["polishedfrontpainted"] = polishedfrontpainted;
	(OptPropManager::OptSurfFinishMap)["polishedbackpainted"] = polishedbackpainted;
	(OptPropManager::OptSurfFinishMap)["ground"] = ground;
	(OptPropManager::OptSurfFinishMap)["groundfrontpainted"] = groundfrontpainted;
	(OptPropManager::OptSurfFinishMap)["groundbackpainted"] = groundbackpainted;
	
	(OptPropManager::OptSurfFinishMap)["polishedlumirrorair"] = polishedlumirrorair;
	(OptPropManager::OptSurfFinishMap)["polishedlumirrorglue"] = polishedlumirrorglue;
	(OptPropManager::OptSurfFinishMap)["polishedair"] = polishedair;
	(OptPropManager::OptSurfFinishMap)["polishedteflonair"] = polishedteflonair;
	(OptPropManager::OptSurfFinishMap)["polishedtioair"] = polishedtioair;
	(OptPropManager::OptSurfFinishMap)["polishedtyvekair"] = polishedtyvekair;
	(OptPropManager::OptSurfFinishMap)["polishedvm2000air"] = polishedvm2000air;
	(OptPropManager::OptSurfFinishMap)["polishedvm2000glue"] = polishedvm2000glue;
	
	(OptPropManager::OptSurfFinishMap)["etchedlumirrorair"] = etchedlumirrorair;
	(OptPropManager::OptSurfFinishMap)["etchedlumirrorglue"] = etchedlumirrorglue;
	(OptPropManager::OptSurfFinishMap)["etchedair"] = etchedair;
	(OptPropManager::OptSurfFinishMap)["etchedteflonair"] = etchedteflonair;
	(OptPropManager::OptSurfFinishMap)["etchedtioair"] = etchedtioair;
	(OptPropManager::OptSurfFinishMap)["etchedtyvekair"] = etchedtyvekair;
	(OptPropManager::OptSurfFinishMap)["etchedvm2000air"] = etchedvm2000air;
	(OptPropManager::OptSurfFinishMap)["etchedvm2000glue"] = etchedvm2000glue;
	
	(OptPropManager::OptSurfFinishMap)["groundlumirrorair"] = groundlumirrorair;
	(OptPropManager::OptSurfFinishMap)["groundlumirrorglue"] = groundlumirrorglue;
	(OptPropManager::OptSurfFinishMap)["groundair"] = groundair;
	(OptPropManager::OptSurfFinishMap)["groundteflonair"] = groundteflonair;
	(OptPropManager::OptSurfFinishMap)["groundtioair"] = groundtioair;
	(OptPropManager::OptSurfFinishMap)["groundtyvekair"] = groundtyvekair;
	(OptPropManager::OptSurfFinishMap)["groundvm2000air"] = groundvm2000air;
	(OptPropManager::OptSurfFinishMap)["groundvm2000glue"] = groundvm2000glue;
	
	(OptPropManager::OptSurfFinishMap)["Rough_LUT"] = Rough_LUT;
	(OptPropManager::OptSurfFinishMap)["RoughTeflon_LUT"] = RoughTeflon_LUT;
	(OptPropManager::OptSurfFinishMap)["RoughESR_LUT"] = RoughESR_LUT;
	(OptPropManager::OptSurfFinishMap)["RoughESRGrease_LUT"] = RoughESRGrease_LUT;
	
	(OptPropManager::OptSurfFinishMap)["Polished_LUT"] = Polished_LUT;
	(OptPropManager::OptSurfFinishMap)["PolishedTeflon_LUT"] = PolishedTeflon_LUT;
	(OptPropManager::OptSurfFinishMap)["PolishedESR_LUT"] = PolishedESR_LUT;
	(OptPropManager::OptSurfFinishMap)["PolishedESRGrease_LUT"] = PolishedESRGrease_LUT;
	
	(OptPropManager::OptSurfFinishMap)["Detector_LUT"] = Detector_LUT;
}



OptPropManager* OptPropManager::OptPropManagerInstance()
{
	if(!gThis) gThis = new OptPropManager;
	return gThis;
}



void OptPropManager::SetSurfSigmaAlpha(const G4String& logsurfname, const G4double& s_a)
{
	const G4LogicalBorderSurfaceTable* surftab = G4LogicalBorderSurface::GetSurfaceTable();
	
	if(surftab){
		G4LogicalSurface* Surface = NULL;
		
		for(size_t iSurf=0; iSurf<surftab->size(); iSurf++){
			G4String name = surftab->at(iSurf)->GetName();
			if(name == logsurfname){
				Surface = surftab->at(iSurf);
			}
		}
		
		if(Surface){
			G4OpticalSurface* OpticalSurface = dynamic_cast <G4OpticalSurface*> (Surface->GetSurfaceProperty());
			if(OpticalSurface) OpticalSurface->SetSigmaAlpha(s_a);
		}
	}
}



void OptPropManager::SetMaterialRindex(const G4String& materialname, const G4int Nentries, const G4double* photonenergies, const G4double* rindexes)
{
	G4MaterialTable *pMatTable = G4Material::GetMaterialTable();
	size_t nMat = G4Material::GetNumberOfMaterials();
	
	if(pMatTable && (nMat>0)){
		for(size_t iMat=0; iMat<nMat;iMat++){
			if( (pMatTable->at(iMat)->GetName()) == materialname ){
				if(pMatTable->at(iMat)->GetMaterialPropertiesTable()->GetProperty("RINDEX")) pMatTable->at(iMat)->GetMaterialPropertiesTable()->RemoveProperty("RINDEX");
					pMatTable->at(iMat)->GetMaterialPropertiesTable()->AddProperty("RINDEX",(G4double*)photonenergies,(G4double*)rindexes,Nentries);
			}
		}
	}
}

void OptPropManager::SetMaterialRindex(const G4String& materialname, const std::vector<G4double>& photonenergies, const std::vector<G4double>& rindexes)
{
	if(photonenergies.size()==rindexes.size()) OptPropManager::SetMaterialRindex(materialname, photonenergies.size(), &photonenergies.at(0), &rindexes.at(0));
}



void OptPropManager::SetMaterialAbsLenght(const G4String& materialname, const G4int Nentries, const G4double* photonenergies, const G4double* abslenghts)
{
	G4MaterialTable *pMatTable = G4Material::GetMaterialTable();
	size_t nMat = G4Material::GetNumberOfMaterials();
	
	if(pMatTable && (nMat>0)){
		for(size_t iMat=0; iMat<nMat;iMat++){
			if( (pMatTable->at(iMat)->GetName()) == materialname ){
				if(pMatTable->at(iMat)->GetMaterialPropertiesTable()->GetProperty("ABSLENGTH")) pMatTable->at(iMat)->GetMaterialPropertiesTable()->RemoveProperty("ABSLENGTH");
					pMatTable->at(iMat)->GetMaterialPropertiesTable()->AddProperty("ABSLENGTH",(G4double*)photonenergies,(G4double*)abslenghts,Nentries);
			}
		}
	}
}

void OptPropManager::SetMaterialAbsLenght(const G4String& materialname, const std::vector<G4double>& photonenergies, const std::vector<G4double>& abslenghts)
{
	if(photonenergies.size()==abslenghts.size()) OptPropManager::SetMaterialAbsLenght(materialname, photonenergies.size(), &photonenergies.at(0), &abslenghts.at(0));
}



void OptPropManager::SetMaterialRayleighLenght(const G4String& materialname, const G4int Nentries, const G4double* photonenergies, const G4double* rayleighlenghts)
{
	G4MaterialTable *pMatTable = G4Material::GetMaterialTable();
	size_t nMat = G4Material::GetNumberOfMaterials();
	
	if(pMatTable && (nMat>0)){
		for(size_t iMat=0; iMat<nMat;iMat++){
			if( (pMatTable->at(iMat)->GetName()) == materialname ){
				if(pMatTable->at(iMat)->GetMaterialPropertiesTable()->GetProperty("RAYLEIGH")) pMatTable->at(iMat)->GetMaterialPropertiesTable()->RemoveProperty("RAYLEIGH");
					pMatTable->at(iMat)->GetMaterialPropertiesTable()->AddProperty("RAYLEIGH",(G4double*)photonenergies,(G4double*)rayleighlenghts,Nentries);
			}
		}
	}
}

void OptPropManager::SetMaterialRayleighLenght(const G4String& materialname, const std::vector<G4double>& photonenergies, const std::vector<G4double>& rayleighlenghts)
{
	if(photonenergies.size()==rayleighlenghts.size()) OptPropManager::SetMaterialRayleighLenght(materialname, photonenergies.size(), &photonenergies.at(0), &rayleighlenghts.at(0));
}



void OptPropManager::SetOpticalSurfaceModel(const G4String& logsurfname, const G4String& model)
{
	G4LogicalBorderSurfaceTable* surftab = (G4LogicalBorderSurfaceTable*)G4LogicalBorderSurface::GetSurfaceTable();
	size_t nLogSurf = G4LogicalBorderSurface::GetNumberOfBorderSurfaces();
	if( (!surftab) || (nLogSurf==0) ) return;
	
	if(OptSurfModelMap.find(model)==OptSurfModelMap.end()) return;
	
	for(size_t iSurf=0; iSurf<nLogSurf; iSurf++){
		if( (surftab->at(iSurf)->GetName())==logsurfname ){
			G4OpticalSurface *optsurf = dynamic_cast<G4OpticalSurface*>(surftab->at(iSurf)->GetSurfaceProperty());
			if(optsurf){
				optsurf->SetModel( OptSurfModelMap[model] );
			}
		}
	}
}



void OptPropManager::SetOpticalSurfaceType(const G4String& logsurfname, const G4String& type)
{
	G4LogicalBorderSurfaceTable* surftab = (G4LogicalBorderSurfaceTable*)G4LogicalBorderSurface::GetSurfaceTable();
	size_t nLogSurf = G4LogicalBorderSurface::GetNumberOfBorderSurfaces();
	if( (!surftab) || (nLogSurf==0) ) return;
	
	if(OptSurfTypeMap.find(type)==OptSurfTypeMap.end()) return;
	
	for(size_t iSurf=0; iSurf<nLogSurf; iSurf++){
		if( (surftab->at(iSurf)->GetName())==logsurfname ){
			G4OpticalSurface *optsurf = dynamic_cast<G4OpticalSurface*>(surftab->at(iSurf)->GetSurfaceProperty());
			if(optsurf){
				optsurf->SetType( OptSurfTypeMap[type] );
			}
		}
	}
}



void OptPropManager::SetOpticalSurfaceFinish(const G4String& logsurfname, const G4String& finish)
{
	G4LogicalBorderSurfaceTable* surftab = (G4LogicalBorderSurfaceTable*)G4LogicalBorderSurface::GetSurfaceTable();
	size_t nLogSurf = G4LogicalBorderSurface::GetNumberOfBorderSurfaces();
	if( (!surftab) || (nLogSurf==0) ) return;
	
	if(OptSurfFinishMap.find(finish)==OptSurfFinishMap.end()) return;
	
	for(size_t iSurf=0; iSurf<nLogSurf; iSurf++){
		if( (surftab->at(iSurf)->GetName())==logsurfname ){
			G4OpticalSurface *optsurf = dynamic_cast<G4OpticalSurface*>(surftab->at(iSurf)->GetSurfaceProperty());
			if(optsurf){
				optsurf->SetFinish( OptSurfFinishMap[finish] );
			}
		}
	}
}



void OptPropManager::BuildOpticalSurface(const G4String& optsurfname, const G4String& model, const G4String& type, const G4String& finish )
{
	G4SurfacePropertyTable *surftab = (G4SurfacePropertyTable*)G4OpticalSurface::GetSurfacePropertyTable();
	size_t nOptSurf = G4OpticalSurface::GetNumberOfSurfaceProperties();
	
	if( (!surftab) ) return; //This is a big problem as it is a static class member!!!
	
	if(OptSurfModelMap.find(model)==OptSurfModelMap.end()) return;
	
	if(OptSurfTypeMap.find(type)==OptSurfTypeMap.end()) return;
	
	if(OptSurfFinishMap.find(finish)==OptSurfFinishMap.end()) return;
	
	//Check if it alredy exists. If it does do not proceed to the creation of this surface as this might overwrite another optical surface used by some logical surface
	for(size_t iSurf=0; iSurf<nOptSurf; iSurf++){
		if( (surftab->at(iSurf)->GetName())==optsurfname ) return;
	}
	
	G4OpticalSurface *optsurf = new G4OpticalSurface(optsurfname, OptSurfModelMap[model], OptSurfFinishMap[finish], OptSurfTypeMap[type]);
}



void OptPropManager::BuildLogicalBorderSurface(const G4String& logsurfname, const G4String& optsurfname, const G4String& physvol1, const G4String& physvol2)
{
	//All the procedure should avoid that two logical surfaces with the same name can be created by mistake
	
	G4PhysicalVolumeStore *pPhysVolStore = G4PhysicalVolumeStore::GetInstance();
	
	if(!pPhysVolStore) return;
	
	G4VPhysicalVolume *vol1=NULL, *vol2=NULL;
	
	size_t nVols = pPhysVolStore->size();
	
	for(size_t iVol=0; iVol<nVols; iVol++){
		if( (pPhysVolStore->at(iVol)->GetName())==physvol1) vol1 = pPhysVolStore->at(iVol);
		if( (pPhysVolStore->at(iVol)->GetName())==physvol2) vol2 = pPhysVolStore->at(iVol);;
	}
	
	if( (!vol1) || (!vol2) || (vol1==vol2)) return;
	
	
	//Check if the optical surface already exists
	G4SurfacePropertyTable *optsurftab = (G4SurfacePropertyTable*)G4OpticalSurface::GetSurfacePropertyTable();
	size_t nOptSurf = G4OpticalSurface::GetNumberOfSurfaceProperties();
	
	if( (!optsurftab) ) return; //This is a big problem as it is a static class member
	
	G4OpticalSurface *optsurf = NULL;
	for(size_t iSurf=0; iSurf<nOptSurf; iSurf++){
		if( (optsurftab->at(iSurf)->GetName())==optsurfname ) optsurf = dynamic_cast<G4OpticalSurface*>(optsurftab->at(iSurf));
	}
	
	//If the G4OpticalSurface doesn't exists do not go beyond in building the G4LogicalBorderSurface
	if(!optsurf){
		return;
	}
	
	
	//Check if it already exists
	G4LogicalBorderSurface *logsurf = G4LogicalBorderSurface::GetSurface(vol1, vol2);
	
	if(logsurf){
		logsurf->SetName(logsurfname);
		logsurf->SetSurfaceProperty(optsurf);
		return;
	}
	
	
	//Check if there a surface with this name and in case just return
	G4LogicalBorderSurfaceTable* logsurftab = (G4LogicalBorderSurfaceTable*)G4LogicalBorderSurface::GetSurfaceTable();
	size_t nLogSurf = G4LogicalBorderSurface::GetNumberOfBorderSurfaces();
	
	for(size_t iSurf=0; iSurf<nLogSurf; iSurf++){
		if( (logsurftab->at(iSurf)->GetName())==logsurfname ) return;
	}
	
	//Here the logsurf is still NULL so a new object can be instanced safely
	logsurf = new G4LogicalBorderSurface(logsurfname, vol1, vol2, optsurf);
}


void OptPropManager::SetOpticalSurface(const G4String& logsurfname, const G4String& optsurfname)
{
	G4LogicalBorderSurface *logsurf = NULL;
	G4OpticalSurface *optsurf = NULL;
	
	G4LogicalBorderSurfaceTable *logsurftab = (G4LogicalBorderSurfaceTable*)G4LogicalBorderSurface::GetSurfaceTable();
	G4SurfacePropertyTable *optsurftab = (G4SurfacePropertyTable*)G4OpticalSurface::GetSurfacePropertyTable();
	
	if( (!logsurftab) || (!optsurftab) ) return; //This is a big problem as they are static class members
	
	
	size_t nLogSurfs = G4LogicalBorderSurface::GetNumberOfBorderSurfaces();
	size_t nOptSurfs = G4OpticalSurface::GetNumberOfSurfaceProperties();
	
	if( (nLogSurfs==0) || (nOptSurfs==0) ) return;
	
	
	for(size_t iSurf=0; iSurf<nOptSurfs; iSurf++){
		if( (optsurftab->at(iSurf)->GetName())==optsurfname ) optsurf = static_cast<G4OpticalSurface*>(optsurftab->at(iSurf));
	}
	
	if(!optsurf) return;
	
	
	for(size_t iSurf=0; iSurf<nLogSurfs; iSurf++){
		if( (logsurftab->at(iSurf)->GetName())==logsurfname ) logsurf = logsurftab->at(iSurf);
	}
	
	if(!logsurf) return;
	
	logsurf->SetSurfaceProperty(optsurf);
}


#endif