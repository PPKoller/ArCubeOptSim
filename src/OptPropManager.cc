#ifndef OPT_PROP_MANAGER_CC
#define OPT_PROP_MANAGER_CC

#include "OptPropManager.hh"
#include "DetConstr.hh"

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
	fVerbose = OptPropManager::kSilent;
	
	fDetConstr = NULL;
	
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
	
	
	json_proc_tab["setmatprop"] = &OptPropManager::setmatprop;
	json_proc_tab["setoptsurf"] = &OptPropManager::setoptsurf;
	json_proc_tab["setbordersurf"] = &OptPropManager::setbordersurf;
	json_proc_tab["setskinsurf"] = &OptPropManager::setskinsurf;
	json_proc_tab["buildoptsurf"] = &OptPropManager::buildoptsurf;
	json_proc_tab["buildbordersurface"] = &OptPropManager::buildbordersurface;
	json_proc_tab["buildskinsurf"] = &OptPropManager::buildskinsurf;
	
}



OptPropManager* OptPropManager::GetInstance()
{
	if(!gThis) gThis = new OptPropManager;
	return gThis;
}



void OptPropManager::ProcessJsonFile(const G4String& jsonfilename)
{
	json jsonObj;
	std::ifstream infile( jsonfilename.c_str() );
	if(!infile) return;
	
	infile >> jsonObj;
	
	
	for (json::iterator it = jsonObj.begin(); it != jsonObj.end(); ++it){
		if( it.value().is_object() ){
			if( json_proc_tab.find(it.key()) != json_proc_tab.end() ){
				json_proc_memfunc fncptr = json_proc_tab.at( it.key() );
				(this->*fncptr)( it.value() );
			}
		}
	}
}



G4LogicalBorderSurface* OptPropManager::FindBorderSurf(const G4String& logsurfname)
{
	const G4LogicalBorderSurfaceTable* surftab = G4LogicalBorderSurface::GetSurfaceTable();
	size_t nSurf = G4LogicalBorderSurface::GetNumberOfBorderSurfaces();
	
	if(!surftab) return NULL;
	
	for(size_t iSurf=0; iSurf<nSurf; iSurf++){
		if(surftab->at(iSurf)->GetName() == logsurfname){
			return surftab->at(iSurf);
		}
	}
	
	return NULL;
}


G4LogicalSkinSurface* OptPropManager::FindSkinSurf(const G4String& logsurfname)
{
	const G4LogicalSkinSurfaceTable* surftab = G4LogicalSkinSurface::GetSurfaceTable();
	size_t nSurf = G4LogicalSkinSurface::GetNumberOfSkinSurfaces();
	
	if(!surftab) return NULL;
	
	for(size_t iSurf=0; iSurf<nSurf; iSurf++){
		if(surftab->at(iSurf)->GetName() == logsurfname){
			return surftab->at(iSurf);
		}
	}
	
	return NULL;
}


G4OpticalSurface* OptPropManager::FindOptSurf(const G4String& optsurfname)
{
	G4SurfacePropertyTable* surftab = (G4SurfacePropertyTable*)G4OpticalSurface::GetSurfacePropertyTable();
	size_t nSurf = G4OpticalSurface::GetNumberOfSurfaceProperties();
		
	if( (!surftab) || (nSurf<=0) ) return NULL;
	
	for(size_t iSurf=0; iSurf<nSurf; iSurf++){
		if(surftab->at(iSurf)->GetName() == optsurfname){
			return dynamic_cast<G4OpticalSurface*> (surftab->at(iSurf));
		}
	}
	
	return NULL;
}


G4Material* OptPropManager::FindMaterial(const G4String& materialname)
{
	G4MaterialTable *pMatTable = G4Material::GetMaterialTable();
	size_t nMat = G4Material::GetNumberOfMaterials();
	
	if( (!pMatTable) || (nMat<=0) ) return NULL;
	
	for(size_t iMat=0; iMat<nMat; iMat++){
		if( (pMatTable->at(iMat)->GetName()) == materialname ){
			return pMatTable->at(iMat);
		}
	}
	
	return NULL;
}


G4VPhysicalVolume* OptPropManager::FindPhysVol(const G4String& physvolname)
{
	G4PhysicalVolumeStore *pPhysVolStore = G4PhysicalVolumeStore::GetInstance();
	
	G4VPhysicalVolume *vol=NULL;
	
	size_t nVols = pPhysVolStore->size();
	
	for(size_t iVol=0; iVol<nVols; iVol++){
		if( (pPhysVolStore->at(iVol)->GetName())==physvolname){
			if(vol){
				std::cout << "\nWARNING --> OptPropManager::FindPhysVol(...): There is more than one physical volume named " << physvolname << ". Using this volume might result in unexpected behaviour.\n" << std::endl;
			}
			vol = pPhysVolStore->at(iVol);
		}
	}
	
	return vol;
}


void OptPropManager::ReadValuesFromFile(const G4String& filename, std::vector<G4double>& ph_en, std::vector<G4double>& vals)
{
	ph_en.resize(0);
	vals.resize(0);
	
	std::ifstream infile(filename.c_str());
	
	if(!infile) return;
	
	std::string str;
	std::stringstream ss_tmp;
	
	
	while(getline(infile,str)){
		ss_tmp.clear(); ss_tmp.str("");
		ss_tmp << str;
		
		ss_tmp >> str;
		G4double ph_en_d = std::stod(str);
		
		if(ss_tmp){//There is only one value while the file format is defined with 2 columns
			ph_en.resize(0);
			vals.resize(0);
			return;
		}
		ss_tmp >> str;
		G4double val_d = std::stod(str);
		
		ph_en.push_back(ph_en_d);
		vals.push_back(val_d);
	}
	
}



void OptPropManager::setmatprop(const json keyval)
{
	if(fVerbose>=OptPropManager::kDebug){
		std::cout << "Debug --> OptPropManager::setmatprop(...): start of routine." << std::endl;
	}
	
	//Requirements are that the material must exist, otherwise return with an error
	if( !keyval.contains("matname") ){
		std::cout << "\nERROR --> OptPropManager::setmatprop(...): the json object doesn't contain the \"matname\" key. Cannot set material optical properties without knowing the material name!" << std::endl;
		return;
	}
	
	if( !keyval.at("matname").is_string() ){
		std::cout << "\nERROR --> OptPropManager::setmatprop(...): The \"matname\" keymust be a string!" << std::endl;
		return;
	}
	
	
	G4Material* mat = FindMaterial(keyval.at("matname").get<std::string>());
	
	if(!mat){
		std::cout << "\nERROR --> OptPropManager::setmatprop(...): Cannot find the material \""<< keyval.at("matname") <<"\" in table of the instanced materials!" << std::endl;
		return;
	}
	
	//The materials object have only the key "propfile", containing a list of keys that link a property to a specific text file
	
	if( keyval.contains("propfiles") && (keyval.at("propfile").is_object()) ){
		json propObj = keyval.at("propfiles");
		
		std::vector<G4double> en_vec(0), val_vec(0);
		
		for (json::iterator it = propObj.begin(); it != propObj.end(); ++it){
			if(!it.value().is_string()){
				std::cout << "\nERROR --> OptPropManager::setmatprop(...): The field corresponding to the property " << it.key() << " is not a string!" << std::endl;
				continue;
			}
			
			en_vec.clear(); en_vec.resize(0);
			val_vec.clear(); val_vec.resize(0);
			
			ReadValuesFromFile( it.value().get<std::string>(), en_vec, val_vec );
			
			if( (en_vec.size()==0) || (val_vec.size()==0) || (en_vec.size()!=val_vec.size()) ){
				continue;
			}
			
			if(fVerbose>=OptPropManager::kInfo){
				std::cout << "Info --> OptPropManager::setmatprop(...): Setting property " << it.key() << " for \"" << mat->GetName() << "\" from file \"" << it.value().get<std::string>() << "\"" << std::endl;
			}
			
			G4MaterialPropertiesTable* propTab = mat->GetMaterialPropertiesTable();
			
			if(!propTab){
				propTab = new G4MaterialPropertiesTable();
				mat->SetMaterialPropertiesTable(propTab);
			}
			
			if(propTab->GetProperty( it.key().c_str() )){
				propTab->RemoveProperty( it.key().c_str() );
			}
			propTab->AddProperty( it.key().c_str() ,(G4double*)&en_vec.at(0), (G4double*)&val_vec.at(0), en_vec.size() );
		}
	}
	
	if(fVerbose>=OptPropManager::kDebug){
		std::cout << "Debug --> OptPropManager::setmatprop(...): end of routine." << std::endl;
	}
}


void OptPropManager::setoptsurf(const json keyval)
{

	if(fVerbose>=OptPropManager::kDebug){
		std::cout << "Debug --> OptPropManager::setoptsurf(...): start of routine." << std::endl;
	}
	
	//Requirements are that the optical surface exist, otherwise return with an error
	if( !keyval.contains("surfname") ){
		std::cout << "\nERROR --> OptPropManager::setoptsurf(...): the json object doesn't contain the \"surfname\" key. Cannot change settings of an optical surface without knowing its name!" << std::endl;
		return;
	}
	
	if( !keyval.at("surfname").is_string() ){
		std::cout << "\nERROR --> OptPropManager::setoptsurf(...): The \"surfname\" key must be a string!" << std::endl;
		return;
	}
	
	
	G4OpticalSurface* optsurf = FindOptSurf(keyval.at("surfname").get<std::string>());
	
	if(!optsurf){
		std::cout << "\nERROR --> OptPropManager::setoptsurf(...): Cannot find the optical surface \""<< keyval.at("surfname").get<std::string>() <<"\" in table of the instanced optical surfaces!" << std::endl;
		return;
	}
	
	
	if(keyval.contains("model")){
		if( keyval.at("model").is_string() && (OptSurfModelMap.find(keyval.at("model").get<std::string>())!=OptSurfModelMap.end()) ){
			optsurf->SetModel( OptSurfModelMap.at(keyval.at("model").get<std::string>()) );
		}
	}
	
	if(keyval.contains("type")){
		if( keyval.at("type").is_string() && (OptSurfTypeMap.find(keyval.at("type").get<std::string>())!=OptSurfTypeMap.end()) ){
			optsurf->SetType( OptSurfTypeMap.at(keyval.at("type").get<std::string>()) );
		}
	}
	
	if(keyval.contains("finish")){
		if( keyval.at("finish").is_string() && (OptSurfFinishMap.find(keyval.at("finish").get<std::string>())!=OptSurfFinishMap.end()) ){
			optsurf->SetFinish( OptSurfFinishMap.at(keyval.at("finish").get<std::string>()) );
		}
	}
	
	if(keyval.contains("sigma_alpha")){
		if( keyval.at("sigma_alpha").is_number_float() ){
			optsurf->SetSigmaAlpha( keyval.at("sigma_alpha").get<G4double>() );
		}
	}
	
	
	if( keyval.contains("propfiles") && (keyval.at("propfile").is_object()) ){
		
		json propObj = keyval.at("propfiles");
		
		std::vector<G4double> en_vec(0), val_vec(0);
		
		for (json::iterator it = propObj.begin(); it != propObj.end(); ++it){
			if(!it.value().is_string()){
				std::cout << "\nERROR --> OptPropManager::setoptsurf(...): The field corresponding to the property " << it.key() << " is not a string!" << std::endl;
				continue;
			}
			
			en_vec.clear(); en_vec.resize(0);
			val_vec.clear(); val_vec.resize(0);
			
			ReadValuesFromFile( it.value().get<std::string>(), en_vec, val_vec );
			
			if( (en_vec.size()==0) || (val_vec.size()==0) || (en_vec.size()!=val_vec.size()) ){
				continue;
			}
			
			if(fVerbose>=OptPropManager::kInfo){
				std::cout << "Info --> OptPropManager::setoptsurf(...): Setting property " << it.key() << " for \"" << optsurf->GetName() << "\" from file \"" << it.value().get<std::string>() << "\"" << std::endl;
			}
			
			
			G4MaterialPropertiesTable* propTab = optsurf->GetMaterialPropertiesTable();
			
			if(!propTab){
				propTab = new G4MaterialPropertiesTable();
				optsurf->SetMaterialPropertiesTable(propTab);
			}
			
			
			if(propTab->GetProperty( it.key().c_str() )){
				propTab->RemoveProperty( it.key().c_str() );
			}
			propTab->AddProperty( it.key().c_str() ,(G4double*)&en_vec.at(0), (G4double*)&val_vec.at(0), en_vec.size() );
		}
		
	}
	
	
	
	if(fVerbose>=OptPropManager::kDebug){
		std::cout << "Debug --> OptPropManager::setoptsurf(...): end of routine." << std::endl;
	}
}


void OptPropManager::setbordersurf(const json keyval)
{
	if(fVerbose>=OptPropManager::kDebug){
		std::cout << "Debug --> OptPropManager::setbordersurf(...): start of routine." << std::endl;
	}
	
	
	//Requirements are that the logical border surface exists, otherwise return with an error
	if( !keyval.contains("surfname") ){
		std::cout << "\nERROR --> OptPropManager::setbordersurf(...): the json object doesn't contain the \"surfname\" key. Cannot change settings of alogical border surface without knowing its name!\n" << std::endl;
		return;
	}
	
	if( !keyval.at("surfname").is_string() ){
		std::cout << "\nERROR --> OptPropManager::setbordersurf(...): The \"surfname\" key must be a string!\n" << std::endl;
		return;
	}
	
	
	G4LogicalBorderSurface* logsurf = FindBorderSurf(keyval.at("surfname").get<std::string>());
	
	if(!logsurf){
		std::cout << "\nERROR --> OptPropManager::setbordersurf(...): Cannot find the logical border surface \""<< keyval.at("surfname").get<std::string>() <<"\" in the table of the instanced logical border surfaces!\n" << std::endl;
		return;
	}
	
	
	bool setVol1=false, setVol2=false;
	
	
	
	if( keyval.contains("vol1") ){
		if( keyval.at("vol1").is_string() ){
			setVol1 = true;
		}else{
			std::cout << "\nERROR --> OptPropManager::setbordersurf(...): The \"vol1\" key must be a json string type!\n" << std::endl;
		}
	}
	
	if( keyval.contains("vol2") ){
		if( keyval.at("vol2").is_string() ){
			setVol2 = true;
		}else{
			std::cout << "\nERROR --> OptPropManager::setbordersurf(...): The \"vol2\" key must be a json string type!\n" << std::endl;
		}
	}
	
	if(setVol1 && setVol2){
		if(keyval.at("vol1").get<std::string>() == keyval.at("vol2").get<std::string>()){
			setVol1 = false;
			setVol2 = false;
			std::cout << "\nERROR --> OptPropManager::setbordersurf(...): The \"vol1\" and \"vol2\" are the same. A logical border surface must be defined with 2 different logical volumes!\n" << std::endl;
		}
	}
	
	if(setVol1){
		G4VPhysicalVolume *vol1 = FindPhysVol( keyval.at("vol1").get<std::string>() );
		if(vol1){
			logsurf->SetVolume1(vol1);
		}else{
			std::cout << "\nWARNING --> OptPropManager::setbordersurf(...):Could not find the physical volume \"" << keyval.at("vol1").get<std::string>() << "\". The vol1 of the \"" << logsurf->GetName() << "\" logical border surface will not be reset to any new volume." << std::endl;
		}
	}
	
	
	if(setVol2){
		G4VPhysicalVolume *vol2 = FindPhysVol( keyval.at("vol2").get<std::string>() );
		if(vol2){
			logsurf->SetVolume2(vol2);
		}else{
			std::cout << "\nWARNING --> OptPropManager::setbordersurf(...):Could not find the physical volume \"" << keyval.at("vol2").get<std::string>() << "\". The vol2 of the \"" << logsurf->GetName() << "\" logical border surface will not be reset to any new volume." << std::endl;
		}
	}
	
	if( keyval.contains("optsurf") ){
		if( keyval.at("optsurf").is_string() ){
			
			G4OpticalSurface *optsurf = FindOptSurf( keyval.at("optsurf").get<std::string>() );
			
			if(optsurf){
				logsurf->SetSurfaceProperty(optsurf);
			}else{
				std::cout << "\nWARNING --> OptPropManager::setbordersurf(...):Could not find the optical surface \"" << keyval.at("optsurf").get<std::string>() << "\". The optical surface of the \"" << logsurf->GetName() << "\" logical border surface will not be changed." << std::endl;
			}
			
		}else{
			std::cout << "\nERROR --> OptPropManager::setbordersurf(...): The \"optsurf\" key must be a string!\n" << std::endl;
		}
	}
	
	
	G4OpticalSurface *optsurf = dynamic_cast<G4OpticalSurface*> (logsurf->GetSurfaceProperty());
	
	if(!optsurf){
		std::cout << "\nWARNING --> OptPropManager::setbordersurf(...): The logical border surface \"" << logsurf->GetName() << "\" has not any optical surface set (null pointer)." << std::endl;
		return;
	}
	
	
	if(keyval.contains("model")){
		if( keyval.at("model").is_string() && (OptSurfModelMap.find(keyval.at("model").get<std::string>())!=OptSurfModelMap.end()) ){
			optsurf->SetModel( OptSurfModelMap.at(keyval.at("model").get<std::string>()) );
		}
	}
	
	if(keyval.contains("type")){
		if( keyval.at("type").is_string() && (OptSurfTypeMap.find(keyval.at("type").get<std::string>())!=OptSurfTypeMap.end()) ){
			optsurf->SetType( OptSurfTypeMap.at(keyval.at("type").get<std::string>()) );
		}
	}
	
	if(keyval.contains("finish")){
		if( keyval.at("finish").is_string() && (OptSurfFinishMap.find(keyval.at("finish").get<std::string>())!=OptSurfFinishMap.end()) ){
			optsurf->SetFinish( OptSurfFinishMap.at(keyval.at("finish").get<std::string>()) );
		}
	}
	
	if(keyval.contains("sigma_alpha")){
		if( keyval.at("sigma_alpha").is_number_float() ){
			optsurf->SetSigmaAlpha( keyval.at("sigma_alpha").get<double>() );
		}
	}
	
	
	if( keyval.contains("propfiles") && (keyval.at("propfile").is_object()) ){
		
		json propObj = keyval.at("propfiles");
		
		std::vector<G4double> en_vec(0), val_vec(0);
		
		for (json::iterator it = propObj.begin(); it != propObj.end(); ++it){
			if(!it.value().is_string()){
				std::cout << "\nERROR --> OptPropManager::setbordersurf(...): The field corresponding to the property " << it.key() << " is not a string!" << std::endl;
				continue;
			}
			
			en_vec.clear(); en_vec.resize(0);
			val_vec.clear(); val_vec.resize(0);
			
			ReadValuesFromFile( it.value().get<std::string>(), en_vec, val_vec );
			
			if( (en_vec.size()==0) || (val_vec.size()==0) || (en_vec.size()!=val_vec.size()) ){
				continue;
			}
			
			if(fVerbose>=OptPropManager::kInfo){
				std::cout << "Info --> OptPropManager::setbordersurf(...): Setting property " << it.key() << " for \"" << optsurf->GetName() << "\" from file \"" << it.value().get<std::string>() << "\"" << std::endl;
			}
			
			G4MaterialPropertiesTable* propTab = optsurf->GetMaterialPropertiesTable();
			
			if(!propTab){
				propTab = new G4MaterialPropertiesTable();
				optsurf->SetMaterialPropertiesTable(propTab);
			}
			
			if(propTab->GetProperty( it.key().c_str() )){
				propTab->RemoveProperty( it.key().c_str() );
			}
			propTab->AddProperty( it.key().c_str() ,(G4double*)&en_vec.at(0), (G4double*)&val_vec.at(0), en_vec.size() );
		}
		
	}
	
	
	if(fVerbose>=OptPropManager::kDebug){
		std::cout << "Debug --> OptPropManager::setbordersurf(...): end of routine." << std::endl;
	}
}


void OptPropManager::setskinsurf(const json keyval)
{}


void OptPropManager::buildoptsurf(const json keyval)
{
	if(fVerbose>=OptPropManager::kDebug){
		std::cout << "Debug --> OptPropManager::buildoptsurf(...): start of routine." << std::endl;
	}
	
	
	//Requirements are that the optical surface does not aready exist, otherwise return with an error
	if( !keyval.contains("surfname") ){
		std::cout << "\nERROR --> OptPropManager::buildoptsurf(...): the json object doesn't contain the \"surfname\" key. Cannot change settings of an optical surface without knowing its name!" << std::endl;
		return;
	}
	
	if( !keyval.at("surfname").is_string() ){
		std::cout << "\nERROR --> OptPropManager::buildoptsurf(...): The \"surfname\" key must be a string!" << std::endl;
		return;
	}
	
	G4OpticalSurface* optsurf = FindOptSurf(keyval.at("surfname").get<std::string>());
	
	if(optsurf){
		std::cout << "\nERROR --> OptPropManager::buildoptsurf(...): The optical surface \"" << keyval.at("surfname").get<std::string>() << "\" already exists!" << std::endl;
		return;
	}
	
	
	optsurf = new G4OpticalSurface( keyval.at("surfname").get<std::string>() );
	
	if(keyval.contains("model")){
		if( keyval.at("model").is_string() && (OptSurfModelMap.find(keyval.at("model").get<std::string>())!=OptSurfModelMap.end()) ){
			optsurf->SetModel( OptSurfModelMap.at(keyval.at("model").get<std::string>()) );
		}
	}
	
	if(keyval.contains("type")){
		if( keyval.at("type").is_string() && (OptSurfTypeMap.find(keyval.at("type").get<std::string>())!=OptSurfTypeMap.end()) ){
			optsurf->SetType( OptSurfTypeMap.at(keyval.at("type").get<std::string>()) );
		}
	}
	
	if(keyval.contains("finish")){
		if( keyval.at("finish").is_string() && (OptSurfFinishMap.find(keyval.at("finish").get<std::string>())!=OptSurfFinishMap.end()) ){
			optsurf->SetFinish( OptSurfFinishMap.at(keyval.at("finish").get<std::string>()) );
		}
	}
	
	if(keyval.contains("sigma_alpha")){
		if( keyval.at("sigma_alpha").is_number_float() ){
			optsurf->SetSigmaAlpha( keyval.at("sigma_alpha").get<double>() );
		}
	}
	
	
	if( keyval.contains("propfiles") && (keyval.at("propfile").is_object()) ){
		
		json propObj = keyval.at("propfiles");
		
		std::vector<G4double> en_vec(0), val_vec(0);
		
		for (json::iterator it = propObj.begin(); it != propObj.end(); ++it){
			if(!it.value().is_string()){
				std::cout << "\nERROR --> OptPropManager::buildoptsurf(...): The field corresponding to the property " << it.key() << " is not a string!" << std::endl;
				continue;
			}
			
			en_vec.clear(); en_vec.resize(0);
			val_vec.clear(); val_vec.resize(0);
			
			ReadValuesFromFile( it.value().get<std::string>(), en_vec, val_vec );
			
			if( (en_vec.size()==0) || (val_vec.size()==0) || (en_vec.size()!=val_vec.size()) ){
				continue;
			}
			
			if(fVerbose>=OptPropManager::kInfo){
				std::cout << "Info --> OptPropManager::buildoptsurf(...): Setting property " << it.key() << " for \"" << optsurf->GetName() << "\" from file \"" << it.value().get<std::string>() << "\"" << std::endl;
			}
			
			G4MaterialPropertiesTable* propTab = optsurf->GetMaterialPropertiesTable();
			
			if(!propTab){
				propTab = new G4MaterialPropertiesTable();
				optsurf->SetMaterialPropertiesTable(propTab);
			}
			
			if(propTab->GetProperty( it.key().c_str() )){
				propTab->RemoveProperty( it.key().c_str() );
			}
			propTab->AddProperty( it.key().c_str() ,(G4double*)&en_vec.at(0), (G4double*)&val_vec.at(0), en_vec.size() );
		}
		
	}
	
	
	
	if(fVerbose>=OptPropManager::kDebug){
		std::cout << "Debug --> OptPropManager::buildoptsurf(...): end of routine." << std::endl;
	}
}


void OptPropManager::buildbordersurface(const json keyval)
{
	if(fVerbose>=OptPropManager::kDebug){
		std::cout << "Debug --> OptPropManager::buildbordersurface(...): start of routine." << std::endl;
	}
	
	
	//Requirements are that the logical border surface does not exist, otherwise return with an error
	if( !keyval.contains("surfname") ){
		std::cout << "\nERROR --> OptPropManager::buildbordersurface(...): the json object doesn't contain the \"surfname\" key. Cannot change settings of alogical border surface without knowing its name!\n" << std::endl;
		return;
	}
	
	if( !keyval.at("surfname").is_string() ){
		std::cout << "\nERROR --> OptPropManager::buildbordersurface(...): The \"surfname\" key must be a string!\n" << std::endl;
		return;
	}
	
	
	if( FindBorderSurf( keyval.at("surfname").get<std::string>() ) ){
		std::cout << "\nERROR --> OptPropManager::buildbordersurface(...): The logical border surface \""<< keyval.at("surfname").get<std::string>() <<"\" already exists!\n" << std::endl;
		return;
	}
	
	
	if( !keyval.contains("vol1") ){
		std::cout << "\nERROR --> OptPropManager::buildbordersurface(...): The \"vol1\" key is mandatory to build a new logical border surface! The surface \"" << keyval.at("surfname").get<std::string>() << "\" will not be built!\n" << std::endl;
	}
	
	if( !keyval.at("vol1").is_string() ){
		std::cout << "\nERROR --> OptPropManager::buildbordersurface(...): The \"vol1\" key must be a json string type! The surface \"" << keyval.at("surfname").get<std::string>() << "\" will not be built!\n" << std::endl;
		return;
	}
	
	
	if( !keyval.contains("vol2") ){
		std::cout << "\nERROR --> OptPropManager::buildbordersurface(...): The \"vol2\" key is mandatory to build a new logical border surface! The surface \"" << keyval.at("surfname").get<std::string>() << "\" will not be built!\n" << std::endl;
		return;
	}
	
	if( !keyval.at("vol2").is_string() ){
		std::cout << "\nERROR --> OptPropManager::buildbordersurface(...): The \"vol2\" key must be a json string type! The surface \"" << keyval.at("surfname").get<std::string>() << "\" will not be built!\n" << std::endl;
		return;
	}
	
	
	if( keyval.at("vol1").get<std::string>() == keyval.at("vol2").get<std::string>() ){
		std::cout << "\nERROR --> OptPropManager::buildbordersurface(...): The \"vol1\" and \"vol2\" are the same. A logical border surface can be built only with 2 different physical volumes!\n" << std::endl;
		return;
	}
	
	G4VPhysicalVolume *vol1 = FindPhysVol(keyval.at("vol1").get<std::string>());
	
	if(!vol1){
		std::cout << "\nWARNING --> OptPropManager::buildbordersurface(...):Could not find the \"vol1\" physical volume with name \"" << keyval.at("vol1").get<std::string>() << "\". The logical border surface \"" << keyval.at("surfname").get<std::string>() << "\" will not be built!" << std::endl;
		return;
	}
	
	G4VPhysicalVolume *vol2 = FindPhysVol(keyval.at("vol2").get<std::string>());
	
	if(!vol2){
		std::cout << "\nWARNING --> OptPropManager::buildbordersurface(...):Could not find the \"vol2\" physical volume with name \"" << keyval.at("vol2").get<std::string>() << "\". The logical border surface \"" << keyval.at("surfname").get<std::string>() << "\" will not be built!" << std::endl;
		return;
	}
	
	
	
	if( !keyval.contains("optsurf") ){
		std::cout << "\nERROR --> OptPropManager::buildbordersurface(...): The \"optsurf\" key is mandatory! The logical border surface \"" << keyval.at("surfname").get<std::string>() << "\" will not be built!\n" << std::endl;
		return;
	}
	
	if( !keyval.at("optsurf").is_string() ){
		std::cout << "\nERROR --> OptPropManager::buildbordersurface(...): The \"optsurf\" key must be a string!\n" << std::endl;
		return;
	}
	
	
	G4OpticalSurface *optsurf = FindOptSurf( keyval.at("optsurf").get<std::string>() );
	
	if(!optsurf){
		std::cout << "\nWARNING --> OptPropManager::buildbordersurface(...): Could not find the optical surface \"" << keyval.at("optsurf").get<std::string>() << "\".  The logical border surface \"" << keyval.at("surfname").get<std::string>() << "\" will not be built!\n" << std::endl;
	}
	
	
	G4LogicalBorderSurface *logsurf = new G4LogicalBorderSurface( keyval.at("surfname").get<std::string>(), vol1, vol2, optsurf );
	
	
	if(keyval.contains("model")){
		if( keyval.at("model").is_string() && (OptSurfModelMap.find(keyval.at("model").get<std::string>())!=OptSurfModelMap.end()) ){
			optsurf->SetModel( OptSurfModelMap.at(keyval.at("model").get<std::string>()) );
		}
	}
	
	if(keyval.contains("type")){
		if( keyval.at("type").is_string() && (OptSurfTypeMap.find(keyval.at("type").get<std::string>())!=OptSurfTypeMap.end()) ){
			optsurf->SetType( OptSurfTypeMap.at(keyval.at("type").get<std::string>()) );
		}
	}
	
	if(keyval.contains("finish")){
		if( keyval.at("finish").is_string() && (OptSurfFinishMap.find(keyval.at("finish").get<std::string>())!=OptSurfFinishMap.end()) ){
			optsurf->SetFinish( OptSurfFinishMap.at(keyval.at("finish").get<std::string>()) );
		}
	}
	
	if(keyval.contains("sigma_alpha")){
		if( keyval.at("sigma_alpha").is_number_float() ){
			optsurf->SetSigmaAlpha( keyval.at("sigma_alpha").get<double>() );
		}
	}
	
	
	if( keyval.contains("propfiles") && (keyval.at("propfile").is_object()) ){
		
		json propObj = keyval.at("propfiles");
		
		std::vector<G4double> en_vec(0), val_vec(0);
		
		for (json::iterator it = propObj.begin(); it != propObj.end(); ++it){
			if(!it.value().is_string()){
				std::cout << "\nERROR --> OptPropManager::buildbordersurface(...): The field corresponding to the property " << it.key() << " is not a string!" << std::endl;
				continue;
			}
			
			en_vec.clear(); en_vec.resize(0);
			val_vec.clear(); val_vec.resize(0);
			
			ReadValuesFromFile( it.value().get<std::string>(), en_vec, val_vec );
			
			if( (en_vec.size()==0) || (val_vec.size()==0) || (en_vec.size()!=val_vec.size()) ){
				continue;
			}
			
			if(fVerbose>=OptPropManager::kInfo){
				std::cout << "Info --> OptPropManager::buildbordersurface(...): Setting property " << it.key() << " for \"" << optsurf->GetName() << "\" from file \"" << it.value().get<std::string>() << "\"" << std::endl;
			}
			
			G4MaterialPropertiesTable* propTab = optsurf->GetMaterialPropertiesTable();
			
			if(!propTab){
				propTab = new G4MaterialPropertiesTable();
				optsurf->SetMaterialPropertiesTable(propTab);
			}
			
			if(propTab->GetProperty( it.key().c_str() )){
				propTab->RemoveProperty( it.key().c_str() );
			}
			propTab->AddProperty( it.key().c_str() ,(G4double*)&en_vec.at(0), (G4double*)&val_vec.at(0), en_vec.size() );
		}
		
	}
	
	
	
	if(fVerbose>=OptPropManager::kDebug){
		std::cout << "Debug --> OptPropManager::buildbordersurface(...): end of routine." << std::endl;
	}
}


void OptPropManager::buildskinsurf(const json keyval)
{}




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
			if(OpticalSurface){
				OpticalSurface->SetSigmaAlpha(s_a);
			}else{
				G4cout << "WARNING --> OptPropManager::SetSurfSigmaAlpha(...): The G4LogicalBorder surface \"" << logsurfname << "\" returned null pointer to its G4OpticalSurface (maybe not yet set). The surface roughness cannot be set." << G4endl;
			}
		}else{
			G4cout << "WARNING --> OptPropManager::SetSurfSigmaAlpha(...): The G4LogicalBorder surface \"" << logsurfname << "\" has not yet been created. The surface roughness cannot be set." << G4endl;
		}
	}
}


void OptPropManager::SetSurfReflectivity(const G4String& logsurfname, const G4int Nentries, const G4double* photonenergies, const G4double* reflectivities)
{
	const G4LogicalBorderSurfaceTable* surftab = G4LogicalBorderSurface::GetSurfaceTable();
	
	if(surftab){
		G4LogicalSurface* Surface = NULL;
		for(size_t iSurf=0; iSurf<surftab->size(); iSurf++){
			G4String name = surftab->at(iSurf)->GetName();
			//std::cout << name << std::endl;
			
			if(name == logsurfname){
				Surface = surftab->at(iSurf);
			}
		}
		
		if(Surface){
			G4OpticalSurface* OpticalSurface = dynamic_cast <G4OpticalSurface*> (Surface->GetSurfaceProperty());
			
			if(OpticalSurface){
				G4MaterialPropertiesTable* propTab = OpticalSurface->GetMaterialPropertiesTable();
				
				if(!propTab){
					propTab = new G4MaterialPropertiesTable();
					OpticalSurface->SetMaterialPropertiesTable(propTab);
				}
				
				if(propTab->GetProperty("REFLECTIVITY")){
					propTab->RemoveProperty("REFLECTIVITY");
				}
				
				propTab->AddProperty("REFLECTIVITY",(G4double*)photonenergies,(G4double*)reflectivities,Nentries);
			}else{
				G4cout << "WARNING --> OptPropManager::SetSurfReflectivity(...): The G4LogicalBorder surface \"" << logsurfname << "\" has null pointer to its G4OpticalSurface (maybe not assigned yet). The REFLECTIVITY property cannot be applied." << G4endl;
			}
		}else{
			G4cout << "WARNING --> OptPropManager::SetSurfReflectivity(...): The G4LogicalBorder surface \"" << logsurfname << "\" has not yet been created. The REFLECTIVITY property cannot be applied." << G4endl;
		}
	}
}

void OptPropManager::SetSurfReflectivity(const G4String& logsurfname, const std::vector<G4double>& photonenergies, const std::vector<G4double>& reflectivities)
{
	if(photonenergies.size()==reflectivities.size()) OptPropManager::SetSurfReflectivity(logsurfname, photonenergies.size(), &photonenergies.at(0), &reflectivities.at(0));
}



void OptPropManager::SetMaterialRindex(const G4String& materialname, const G4int Nentries, const G4double* photonenergies, const G4double* rindexes)
{
	G4MaterialTable *pMatTable = G4Material::GetMaterialTable();
	size_t nMat = G4Material::GetNumberOfMaterials();
	
	if(pMatTable && (nMat>0)){
		for(size_t iMat=0; iMat<nMat;iMat++){
			G4Material *mat = pMatTable->at(iMat);
			
			if( mat->GetName() == materialname ){
				
				G4MaterialPropertiesTable* propTab = mat->GetMaterialPropertiesTable();
				
				
				if(!propTab){
					propTab = new G4MaterialPropertiesTable();
					mat->SetMaterialPropertiesTable(propTab);
				}
				
				
				if(propTab->GetProperty("RINDEX")){
					propTab->RemoveProperty("RINDEX");
				}
				propTab->AddProperty("RINDEX",(G4double*)photonenergies,(G4double*)rindexes,Nentries);
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
			G4Material *mat = pMatTable->at(iMat);
			if( mat->GetName() == materialname ){
				
				G4MaterialPropertiesTable* propTab = mat->GetMaterialPropertiesTable();
				
				if(!propTab){
					propTab = new G4MaterialPropertiesTable();
					mat->SetMaterialPropertiesTable(propTab);
				}
				
				if(propTab->GetProperty("ABSLENGTH")){
					propTab->RemoveProperty("ABSLENGTH");
				}
				propTab->AddProperty("ABSLENGTH",(G4double*)photonenergies,(G4double*)abslenghts,Nentries);
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
			G4Material *mat = pMatTable->at(iMat);
			
			if( mat->GetName() == materialname ){
				
				G4MaterialPropertiesTable* propTab = mat->GetMaterialPropertiesTable();
				
				if(!propTab){
					propTab = new G4MaterialPropertiesTable();
					mat->SetMaterialPropertiesTable(propTab);
				}
				
				if(propTab->GetProperty("RAYLEIGH")){
					propTab->RemoveProperty("RAYLEIGH");
				}
				propTab->AddProperty("RAYLEIGH",(G4double*)photonenergies,(G4double*)rayleighlenghts,Nentries);
			}
		}
	}
}

void OptPropManager::SetMaterialRayleighLenght(const G4String& materialname, const std::vector<G4double>& photonenergies, const std::vector<G4double>& rayleighlenghts)
{
	if(photonenergies.size()==rayleighlenghts.size()) OptPropManager::SetMaterialRayleighLenght(materialname, photonenergies.size(), &photonenergies.at(0), &rayleighlenghts.at(0));
}



void OptPropManager::SetMaterialEfficiency(const G4String& materialname, const G4int Nentries, const G4double* photonenergies, const G4double* efficiencies)
{
	G4MaterialTable *pMatTable = G4Material::GetMaterialTable();
	size_t nMat = G4Material::GetNumberOfMaterials();
	
	if(pMatTable && (nMat>0)){
		for(size_t iMat=0; iMat<nMat;iMat++){
			G4Material *mat = pMatTable->at(iMat);
			if( mat->GetName() == materialname ){
				
				G4MaterialPropertiesTable* propTab = mat->GetMaterialPropertiesTable();
				
				if(!propTab){
					propTab = new G4MaterialPropertiesTable();
					mat->SetMaterialPropertiesTable(propTab);
				}
				
				if(propTab->GetProperty("EFFICIENCY")){
					propTab->RemoveProperty("EFFICIENCY");
				}
				propTab->AddProperty("EFFICIENCY",(G4double*)photonenergies,(G4double*)efficiencies,Nentries);
			}
		}
	}
}

void OptPropManager::SetMaterialEfficiency(const G4String& materialname, const std::vector<G4double>& photonenergies, const std::vector<G4double>& efficiencies)
{
	if(photonenergies.size()==efficiencies.size()) OptPropManager::SetMaterialEfficiency(materialname, photonenergies.size(), &photonenergies.at(0), &efficiencies.at(0));
}



void OptPropManager::SetMaterialWLSAbsLenght(const G4String& materialname, const G4int Nentries, const G4double* photonenergies, const G4double* wlsabslenghts)
{
	G4MaterialTable *pMatTable = G4Material::GetMaterialTable();
	size_t nMat = G4Material::GetNumberOfMaterials();
	
	if(pMatTable && (nMat>0)){
		for(size_t iMat=0; iMat<nMat;iMat++){
			G4Material *mat = pMatTable->at(iMat);
			if( mat->GetName() == materialname ){
				
				G4MaterialPropertiesTable* propTab = mat->GetMaterialPropertiesTable();
				
				if(!propTab){
					propTab = new G4MaterialPropertiesTable();
					mat->SetMaterialPropertiesTable(propTab);
				}
				
				if(propTab->GetProperty("WLSABSLENGTH")){
					propTab->RemoveProperty("WLSABSLENGTH");
				}
				propTab->AddProperty("WLSABSLENGTH",(G4double*)photonenergies,(G4double*)wlsabslenghts,Nentries);
			}
		}
	}
}

void OptPropManager::SetMaterialWLSAbsLenght(const G4String& materialname, const std::vector<G4double>& photonenergies, const std::vector<G4double>& wlsabslenghts)
{
	if(photonenergies.size()==wlsabslenghts.size()) OptPropManager::SetMaterialWLSAbsLenght(materialname, photonenergies.size(), &photonenergies.at(0), &wlsabslenghts.at(0));
}



void OptPropManager::SetMaterialWLSEmission(const G4String& materialname, const G4int Nentries, const G4double* photonenergies, const G4double* wlsemissions)
{
	G4MaterialTable *pMatTable = G4Material::GetMaterialTable();
	size_t nMat = G4Material::GetNumberOfMaterials();
	
	if(pMatTable && (nMat>0)){
		for(size_t iMat=0; iMat<nMat;iMat++){
			G4Material *mat = pMatTable->at(iMat);
			if( mat->GetName() == materialname ){
				
				G4MaterialPropertiesTable* propTab = mat->GetMaterialPropertiesTable();
				
				if(!propTab){
					propTab = new G4MaterialPropertiesTable();
					mat->SetMaterialPropertiesTable(propTab);
				}
				
				if(propTab->GetProperty("WLSCOMPONENT")){
					propTab->RemoveProperty("WLSCOMPONENT");
				}
				propTab->AddProperty("WLSCOMPONENT",(G4double*)photonenergies,(G4double*)wlsemissions,Nentries);
			}
		}
	}
}

void OptPropManager::SetMaterialWLSEmission(const G4String& materialname, const std::vector<G4double>& photonenergies, const std::vector<G4double>& wlsemissions)
{
	if(photonenergies.size()==wlsemissions.size()) OptPropManager::SetMaterialWLSEmission(materialname, photonenergies.size(), &photonenergies.at(0), &wlsemissions.at(0));
}



void OptPropManager::SetMaterialWLSDelay(const G4String& materialname, const G4double* delay)
{
	G4MaterialTable *pMatTable = G4Material::GetMaterialTable();
	size_t nMat = G4Material::GetNumberOfMaterials();
	
	if(pMatTable && (nMat>0)){
		for(size_t iMat=0; iMat<nMat;iMat++){
			G4Material *mat = pMatTable->at(iMat);
			if( mat->GetName() == materialname ){
				
				G4MaterialPropertiesTable* propTab = mat->GetMaterialPropertiesTable();
				
				if(!propTab){
					propTab = new G4MaterialPropertiesTable();
					mat->SetMaterialPropertiesTable(propTab);
				}
				
				if(propTab->GetProperty("WLSTIMECONSTANT")){
					propTab->RemoveProperty("WLSTIMECONSTANT");
				}
				propTab->AddConstProperty("WLSTIMECONSTANT",(G4double)*delay);
			}
		}
	}
}

void OptPropManager::SetMaterialWLSDelay(const G4String& materialname, const std::vector<G4double>& delay)
{
	OptPropManager::SetMaterialWLSDelay(materialname, &delay.at(0));
}



void OptPropManager::SetOpticalSurfaceModel(const G4String& logsurfname, const G4String& model)
{
	G4LogicalBorderSurfaceTable* surftab = (G4LogicalBorderSurfaceTable*)G4LogicalBorderSurface::GetSurfaceTable();
	size_t nLogSurf = G4LogicalBorderSurface::GetNumberOfBorderSurfaces();
	
	if( (!surftab) || (nLogSurf==0) ) return;
	
	if(OptSurfModelMap.find(model)==OptSurfModelMap.end()) return;
	
	G4int nFound = 0;
	
	for(size_t iSurf=0; iSurf<nLogSurf; iSurf++){
		if( (surftab->at(iSurf)->GetName())==logsurfname ){
			nFound+=1;
			G4OpticalSurface *optsurf = dynamic_cast<G4OpticalSurface*>(surftab->at(iSurf)->GetSurfaceProperty());
			if(optsurf){
				optsurf->SetModel( OptSurfModelMap[model] );
			}else{
				G4cout << "WARNING --> OptPropManager::SetOpticalSurfaceModel(...): The G4LogicalBorder surface \"" << logsurfname << "\", instance number " << nFound << " returns null pointer to its G4OpticalSurface (maybe not assigned yet). The surface model cannot be set for this surface." << G4endl;
			}
		}
	}
	if((nFound>0) && fVerbose>=OptPropManager::kDetails){
		G4cout << "Detail --> OptPropManager::SetOpticalSurfaceModel(...): Applied model to all the " << nFound << " G4VLogicalSurfaces with name \"" << logsurfname << "\"." << G4endl;
	}else if(nFound==0){
		G4cout << "WARNING --> OptPropManager::SetOpticalSurfaceModel(...): Could not find any G4VLogicalSurfaces with name \"" << logsurfname << "\""  << G4endl;
	}
}



void OptPropManager::SetOpticalSurfaceType(const G4String& logsurfname, const G4String& type)
{
	G4LogicalBorderSurfaceTable* surftab = (G4LogicalBorderSurfaceTable*)G4LogicalBorderSurface::GetSurfaceTable();
	size_t nLogSurf = G4LogicalBorderSurface::GetNumberOfBorderSurfaces();
	if( (!surftab) || (nLogSurf==0) ) return;
	
	if(OptSurfTypeMap.find(type)==OptSurfTypeMap.end()) return;
	
	G4int nFound = 0;
	
	for(size_t iSurf=0; iSurf<nLogSurf; iSurf++){
		if( (surftab->at(iSurf)->GetName())==logsurfname ){
			nFound += 1;
			G4OpticalSurface *optsurf = dynamic_cast<G4OpticalSurface*>(surftab->at(iSurf)->GetSurfaceProperty());
			if(optsurf){
				optsurf->SetType( OptSurfTypeMap[type] );
			}else{
				G4cout << "WARNING --> OptPropManager::SetOpticalSurfaceType(...): The G4LogicalBorder surface \"" << logsurfname << "\", instance number " << nFound << " returns null pointer to its G4OpticalSurface (maybe not assigned yet). The surface type cannot be set for this surface." << G4endl;
			}
		}
	}
	if((nFound>0) && fVerbose>=OptPropManager::kDetails){
		G4cout << "Detail --> OptPropManager::SetOpticalSurfaceType(...): Applied the surface type to all the " << nFound << " G4VLogicalSurfaces with name \"" << logsurfname << "\"." << G4endl;
	}else if(nFound==0){
		G4cout << "WARNING --> OptPropManager::SetOpticalSurfaceType(...): Could not find any G4VLogicalSurfaces with name \"" << logsurfname << "\""  << G4endl;
	}
}



void OptPropManager::SetOpticalSurfaceFinish(const G4String& logsurfname, const G4String& finish)
{
	G4LogicalBorderSurfaceTable* surftab = (G4LogicalBorderSurfaceTable*)G4LogicalBorderSurface::GetSurfaceTable();
	size_t nLogSurf = G4LogicalBorderSurface::GetNumberOfBorderSurfaces();
	if( (!surftab) || (nLogSurf==0) ) return;
	
	if(OptSurfFinishMap.find(finish)==OptSurfFinishMap.end()) return;
	
	G4int nFound = 0;
	
	for(size_t iSurf=0; iSurf<nLogSurf; iSurf++){
		if( (surftab->at(iSurf)->GetName())==logsurfname ){
			nFound += 1;
			G4OpticalSurface *optsurf = dynamic_cast<G4OpticalSurface*>(surftab->at(iSurf)->GetSurfaceProperty());
			if(optsurf){
				optsurf->SetFinish( OptSurfFinishMap[finish] );
			}else{
				G4cout << "WARNING --> OptPropManager::SetOpticalSurfaceFinish(...): The G4LogicalBorder surface <" << logsurfname << ">, instance number " << nFound << " returns null pointer to its G4OpticalSurface (maybe not assigned yet). The surface finish cannot be set for this surface." << G4endl;
			}
		}
	}
	if((nFound>0) && fVerbose>=OptPropManager::kDetails){
		G4cout << "Detail --> OptPropManager::SetOpticalSurfaceFinish(...): Applied the surface finish to all the " << nFound << " G4VLogicalSurfaces with name <" << logsurfname << ">." << G4endl;
	}else if(nFound==0){
		G4cout << "\nWARNING --> OptPropManager::SetOpticalSurfaceFinish(...): Could not find any G4VLogicalSurfaces with name <" << logsurfname << ">.\n"  << G4endl;
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



void OptPropManager::BuildLogicalBorderSurface(const G4String& logsurfname, const G4String& physvol1, const G4String& physvol2, const G4String& optsurfname )
{
	//All the procedure should avoid that two logical surfaces with the same name can be created by mistake
	
	G4PhysicalVolumeStore *pPhysVolStore = G4PhysicalVolumeStore::GetInstance();
	
	if(!pPhysVolStore) return;
	
	
	if(!fDetConstr) return;
	
	DetConstrOptPh::PVmap *pv_map = (DetConstrOptPh::PVmap*)fDetConstr->GetVolsMap();
	
	if(!pv_map) return;
	
	std::vector<G4VPhysicalVolume*> vol1_vec, vol2_vec;
	
	
	
	if( pv_map->find(physvol1)==pv_map->end() ){
		G4cout << "\nWARNING --> OptPropManager::BuildLogicalBorderSurface(...): The physical volume <" << physvol1 << "> does not exists in the list of volumes. The logical surface <" << logsurfname << "> will not be created.\n" << G4endl;
		return;
	}else{
		//const std::string vname = physvol1.data();
		vol1_vec = (*pv_map)[ physvol1 ];
		//vol1_vec = (*pv_map)[ vname ];
	}
	
	if( pv_map->find(physvol2)==pv_map->end() ){
		G4cout << "\nWARNING --> OptPropManager::BuildLogicalBorderSurface(...): The physical volume <" << physvol2 << "> does not exists in the list of volumes. The logical surface <" << logsurfname << "> will not be created.\n" << G4endl;
		return;
	}else{
		//const std::string vname = physvol2.data();
		vol2_vec = (*pv_map)[ physvol2 ];
		//vol2_vec = (*pv_map)[ vname ];
	}
	
	size_t nVols1 = vol1_vec.size();
	if(nVols1==0){
		G4cout << "\nERROR --> OptPropManager::BuildLogicalBorderSurface(...): The list of physical volumes with name <" << physvol2 << "> is empty!!! The logical surface <" << logsurfname << "> will not be created.\n" << G4endl;
		return;
	}
	
	size_t nVols2 = vol2_vec.size();
	if(nVols2==0){
		G4cout << "\nERROR --> OptPropManager::BuildLogicalBorderSurface(...): The list of physical volumes with name <" << physvol2 << "> is empty!!! The logical surface <" << logsurfname << "> will not be created.\n" << G4endl;
		return;
	}
	
	
	G4SurfacePropertyTable *optsurftab = (G4SurfacePropertyTable*)G4OpticalSurface::GetSurfacePropertyTable();
	if( (!optsurftab) ) return; //This is a big problem as it is a static class member
	
	
	
	G4VPhysicalVolume *vol1, *vol2;
	
	bool singleinstances = false;
	
	if( (nVols1==1) && (nVols2==1) ) singleinstances = true;
	
	
	//Check if the optical surface already exists
	G4OpticalSurface *optsurf = NULL;
	size_t nOptSurf = G4OpticalSurface::GetNumberOfSurfaceProperties();
	if(optsurfname != G4String("")){
		for(size_t iSurf=0; iSurf<nOptSurf; iSurf++){
			if( (optsurftab->at(iSurf)->GetName())==optsurfname ) optsurf = dynamic_cast<G4OpticalSurface*>(optsurftab->at(iSurf));
		}
		
		if(!optsurf){
			G4cout << "\nWARNING --> OptPropManager::BuildLogicalBorderSurface(...): The optical surface <" << optsurfname << "> could not be found in the table of all optical surfaces. A dummy surface with the given name will be created for building the logical surface <" << logsurfname << ">.\n" << G4endl;
		}
		
		optsurf = new G4OpticalSurface(optsurfname);
		
	}
	
	
	if(!optsurf){
		G4cout << "\nWARNING --> OptPropManager::BuildLogicalBorderSurface(...): The logical surface <" << logsurfname << "> will be built without an optical surface (null pointer). This might produce run-time crashes and/or wrong behaviour of the simulation!\n" << G4endl;
	}
	
	
	if( singleinstances ){
		//Check if it already exists
		vol1 = vol1_vec.at(0);
		vol2 = vol2_vec.at(0);
		
		G4LogicalBorderSurface *logsurf = G4LogicalBorderSurface::GetSurface(vol1, vol2);
		
		if(logsurf){
			
			G4cout << "\nWARNING --> OptPropManager::BuildLogicalBorderSurface(...): The optical surface between volume <" << vol1->GetName() << "> at address" << vol1 << " and volume <" << vol2->GetName() << "> at address" << vol2 << " already exists with name <" << logsurf->GetName() << ">. Renaming it as <" << logsurfname << ">" << G4endl;
			if(optsurf){
				G4cout << " and assigning the surface <" << optsurf->GetName() << "> to it.\n" << G4endl;
			}else{
				G4cout << ".\n" << G4endl;
			}
			
			logsurf->SetName(logsurfname);
			logsurf->SetSurfaceProperty(optsurf);
			
		}else{
			
			//Check if there a surface with this name
			G4LogicalBorderSurfaceTable* logsurftab = (G4LogicalBorderSurfaceTable*)G4LogicalBorderSurface::GetSurfaceTable();
			size_t nLogSurf = G4LogicalBorderSurface::GetNumberOfBorderSurfaces();
			
			for(size_t iSurf=0; iSurf<nLogSurf; iSurf++){
				if( (logsurftab->at(iSurf)->GetName())==logsurfname ){
					logsurf = logsurftab->at(iSurf);
				}
			}
			
			if(!logsurf){
				
				if(fVerbose >= OptPropManager::kInfo){
					G4cout << "Info --> OptPropManager::BuildLogicalBorderSurface(...): creating logical surface <" << logsurfname << ">." << G4endl;
				}
				new G4LogicalBorderSurface(logsurfname,vol1,vol2,optsurf);
				
			}else{
				G4cout << "\nERROR --> OptPropManager::BuildLogicalBorderSurface(...): The logical surface with name <" << logsurfname << "> already exists. Cannot create another one with same name!\n" << G4endl;
			}
		}
	}else{//There are multiple instances of the two volumes
		
		size_t iSurf = 0;
		std::stringstream ss_tmp; 
		for(size_t iVol1 = 0; iVol1<nVols1; iVol1++){
			for(size_t iVol2 = 0; iVol2<nVols2; iVol2++){
				iSurf++;
				
				vol1 = vol1_vec.at(iVol1);
				vol2 = vol2_vec.at(iVol2);
				ss_tmp.str("");
				ss_tmp << logsurfname << "_" << iSurf;
				
				G4LogicalBorderSurface *logsurf = G4LogicalBorderSurface::GetSurface(vol1, vol2);
				
				if(logsurf){
					
					G4cout << "\nWARNING --> OptPropManager::BuildLogicalBorderSurface(...): The optical surface between volume <" << vol1->GetName() << "> at address" << vol1 << " and volume <" << vol2->GetName() << "> at address" << vol2 << " already exists with name <" << logsurf->GetName() << ">. Renaming it as <" << ss_tmp.str().c_str() << "> and assigning the ." << G4endl;
					if(optsurf){
						G4cout << " and assigning the surface <" << optsurf->GetName() << "> to it.\n" << G4endl;
					}else{
						G4cout << ".\n" << G4endl;
					}
					
					logsurf->SetName(ss_tmp.str().c_str());
					logsurf->SetSurfaceProperty(optsurf);
					
				}else{
					
					//Check if there a surface with this name
					G4LogicalBorderSurfaceTable* logsurftab = (G4LogicalBorderSurfaceTable*)G4LogicalBorderSurface::GetSurfaceTable();
					size_t nLogSurf = G4LogicalBorderSurface::GetNumberOfBorderSurfaces();
					
					for(size_t pSurf=0; pSurf<nLogSurf; pSurf++){
						if( (logsurftab->at(pSurf)->GetName())==logsurfname ){
							logsurf = logsurftab->at(pSurf);
						}
					}
					
					if(!logsurf){
						if(fVerbose >= OptPropManager::kInfo){
							G4cout << "\Info --> OptPropManager::BuildLogicalBorderSurface(...): creating logical surface <" << ss_tmp.str().c_str() << ">." << G4endl;
						}
						new G4LogicalBorderSurface(ss_tmp.str().c_str(),vol1,vol2,optsurf);
					}else{
						G4cout << "\nERROR --> OptPropManager::BuildLogicalBorderSurface(...): The logical surface with name <" << ss_tmp.str().c_str() << "> already exists. Cannot create another one with same name!\n" << G4endl;
					}
				}
			}//Exiting from internal loop (jVol) of phys volumes
		}//Exiting from external loop (iVol) of phys volumes
	}
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
