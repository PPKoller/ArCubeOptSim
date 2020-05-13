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
			if(fVerbose>=OptPropManager::kDebug){
				std::cout << "Debug --> OptPropManager::ProcessJsonFile(...): Key command to process: <" << it.key() << ">" << std::endl;
			}
			if( json_proc_tab.find(it.key()) != json_proc_tab.end() ){
				json_proc_memfunc fncptr = json_proc_tab.at( it.key() );
				(this->*fncptr)( it.value() );
			}else{
				std::cout << "ERROR --> OptPropManager::ProcessJsonFile(...): The key <" << it.key() << "> does not correspond to any callback function in the dictionary." << std::endl;
			}
		}
	}
}



std::set<G4LogicalSurface*>* OptPropManager::FindLogSurf(const G4String& logsurfname)
{
	std::map<G4String, std::set<G4LogicalSurface*> >::iterator it = fLogSurfMap.find(logsurfname);
	if(it == fLogSurfMap.end()) return  NULL;
	
	return &(it->second);
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


const std::vector<G4VPhysicalVolume* >* OptPropManager::FindPhysVol(const G4String& physvolname) const
{
	G4PhysicalVolumeStore *pPhysVolStore = G4PhysicalVolumeStore::GetInstance();
	
	if(!pPhysVolStore) return NULL; //This is a big problem as the static method above returns a static class member!!!
	
	if(!fDetConstr) return NULL;
	
	return fDetConstr->GetPvList(physvolname);
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
		G4cout << "\nERROR --> OptPropManager::setbordersurf(...): the json object doesn't contain the <surfname> key. Cannot change settings of a logical border surface without knowing its name!\n" << G4endl;
		return;
	}
	
	if( !keyval.at("surfname").is_string() ){
		G4cout << "\nERROR --> OptPropManager::setbordersurf(...): The <surfname> key must be a string!\n" << G4endl;
		return;
	}
	
	G4String logsurfname = keyval.at("surfname").get<std::string>();
	
	std::set<G4LogicalSurface* > *logsurflist = FindLogSurf(logsurfname);
	
	if(!logsurflist){
		G4cout << "\nERROR --> OptPropManager::setbordersurf(...): Cannot find the logical surface <"<< logsurfname <<"> in the table of the instanced logical border surfaces!\n" << G4endl;
		return;
	}
	
	
	if( keyval.contains("optsurf") ){
		if( keyval.at("optsurf").is_string() ){
			
			G4OpticalSurface *optsurf = FindOptSurf( keyval.at("optsurf").get<std::string>() );
			
			if(optsurf){
				std::set<G4LogicalSurface* >::iterator iT;
				for(iT=logsurflist->begin(); iT!=logsurflist->end(); ++iT){
					(*iT)->SetSurfaceProperty(optsurf);
				}
				
			}else{
				G4cout << "\nWARNING --> OptPropManager::setbordersurf(...): Could not find the optical surface <" << keyval.at("optsurf").get<std::string>() << ">. The optical surface of the <" << logsurfname << "> logical surface(s) will not be changed." << G4endl;
			}
			
		}else{
			G4cout << "\nERROR --> OptPropManager::setbordersurf(...): The <optsurf> key must be a string!\n" << G4endl;
		}
	}
	
	
	if(keyval.contains("model")){
		if( keyval.at("model").is_string() && (OptSurfModelMap.find(keyval.at("model").get<std::string>())!=OptSurfModelMap.end()) ){
			SetSurfModel( logsurfname, keyval.at("model").get<std::string>());
			//optsurf->SetModel( OptSurfModelMap.at(keyval.at("model").get<std::string>()) );
		}
	}
	
	if(keyval.contains("type")){
		if( keyval.at("type").is_string() && (OptSurfTypeMap.find(keyval.at("type").get<std::string>())!=OptSurfTypeMap.end()) ){
			SetSurfType( logsurfname, keyval.at("type").get<std::string>() );
			//optsurf->SetType( OptSurfTypeMap.at(keyval.at("type").get<std::string>()) );
		}
	}
	
	if(keyval.contains("finish")){
		if( keyval.at("finish").is_string() && (OptSurfFinishMap.find(keyval.at("finish").get<std::string>())!=OptSurfFinishMap.end()) ){
			SetSurfFinish( logsurfname, keyval.at("finish").get<std::string>() );
			//optsurf->SetFinish( OptSurfFinishMap.at(keyval.at("finish").get<std::string>()) );
		}
	}
	
	if(keyval.contains("sigma_alpha")){
		if( keyval.at("sigma_alpha").is_number_float() ){
			SetSurfSigmaAlpha( logsurfname, keyval.at("sigma_alpha").get<double>() );
			//optsurf->SetSigmaAlpha( keyval.at("sigma_alpha").get<double>() );
		}
	}
	
	
	if( keyval.contains("propfiles") && (keyval.at("propfile").is_object()) ){
		
		json propObj = keyval.at("propfiles");
		
		for (json::iterator it = propObj.begin(); it != propObj.end(); ++it){
			if(!it.value().is_string()){
				std::cout << "\nERROR --> OptPropManager::setbordersurf(...): The field corresponding to the property <" << it.key() << "> is not a string!" << std::endl;
				continue;
			}
			
			
			if(fVerbose>=OptPropManager::kDetails){
				G4cout << "Detail --> OptPropManager::setbordersurf(...): Setting property <" << it.key() << "> for logical surface(s) named <" << logsurfname << "> from file <" << it.value().get<std::string>() << ">" << G4endl;
			}
			
			SetSurfPropFromFile(logsurfname, it.value().get<std::string>(), it.key() );
		}
		
	}
	
	
	if(fVerbose>=OptPropManager::kDebug){
		std::cout << "Debug --> OptPropManager::setbordersurf(...): end of routine." << std::endl;
	}
}


void OptPropManager::setskinsurf(const json keyval)
{
	//Not implemented yet
}


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
		G4cout << "Debug --> OptPropManager::buildoptsurf(...): end of routine." << G4endl;
	}
}


void OptPropManager::buildbordersurface(const json keyval)
{
	if(fVerbose>=OptPropManager::kDebug){
		G4cout << "Debug --> OptPropManager::buildbordersurface(...): start of routine." << G4endl;
	}
	
	
	//Requirements are that the logical border surface does not exist, otherwise return with an error
	if( !keyval.contains("surfname") ){
		G4cout << "\nERROR --> OptPropManager::buildbordersurface(...): the json object doesn't contain the <surfname> key. Cannot change settings of alogical border surface without knowing its name!\n" << G4endl;
		return;
	}
	
	if( !keyval.at("surfname").is_string() ){
		G4cout << "\nERROR --> OptPropManager::buildbordersurface(...): The <surfname> key must be a string!\n" << G4endl;
		return;
	}
	
	
	if( !keyval.contains("optsurf") ){
		G4cout << "\nERROR --> OptPropManager::buildbordersurface(...): The <optsurf> key is mandatory! The logical border surface <" << keyval.at("surfname").get<std::string>() << "> will not be built!\n" << G4endl;
		return;
	}
	
	if( !keyval.at("optsurf").is_string() ){
		G4cout << "\nERROR --> OptPropManager::buildbordersurface(...): The <optsurf> key must be a string!\n" << G4endl;
		return;
	}
	
	
	G4OpticalSurface *optsurf = FindOptSurf( keyval.at("optsurf").get<std::string>() );
	
	if(!optsurf){
		G4cout << "\nERROR --> OptPropManager::buildbordersurface(...): Could not find the optical surface <" << keyval.at("optsurf").get<std::string>() << ">. The optical surface must be built before than the logical surface that uses it. The logical border surface <" << keyval.at("surfname").get<std::string>() << "> will not be built.\n" << G4endl;
	}
	
	
	if( !keyval.contains("vol1") ){
		G4cout << "\nERROR --> OptPropManager::buildbordersurface(...): The <vol1> key is mandatory to build a new logical border surface! The surface <" << keyval.at("surfname").get<std::string>() << "> will not be built!\n" << G4endl;
	}
	
	if( !keyval.at("vol1").is_string() ){
		G4cout << "\nERROR --> OptPropManager::buildbordersurface(...): The <vol1> key must be a json string type! The surface <" << keyval.at("surfname").get<std::string>() << "> will not be built!\n" << G4endl;
		return;
	}
	
	
	if( !keyval.contains("vol2") ){
		G4cout << "\nERROR --> OptPropManager::buildbordersurface(...): The <vol2> key is mandatory to build a new logical border surface! The surface <" << keyval.at("surfname").get<std::string>() << "> will not be built!\n" << G4endl;
		return;
	}
	
	if( !keyval.at("vol2").is_string() ){
		G4cout << "\nERROR --> OptPropManager::buildbordersurface(...): The <vol2> key must be a json string type! The surface <" << keyval.at("surfname").get<std::string>() << "> will not be built!\n" << G4endl;
		return;
	}
	
	
	if( keyval.at("vol1").get<std::string>() == keyval.at("vol2").get<std::string>() ){
		G4cout << "\nERROR --> OptPropManager::buildbordersurface(...): The <vol1> and <vol2> are the same. A logical border surface can be built only between 2 different physical volumes!\n" << G4endl;
		return;
	}
	
	
	const std::vector<G4VPhysicalVolume* > *vol_vec1 = FindPhysVol(keyval.at("vol1").get<std::string>());
	
	if(!vol_vec1){
		G4cout << "\nERROR --> OptPropManager::buildbordersurface(...):Could not find the <vol1> physical volume with name <" << keyval.at("vol1").get<std::string>() << ">. The logical border surface <" << keyval.at("surfname").get<std::string>() << "> will not be built!\n" << G4endl;
		return;
	}
	
	if( (!vol_vec1->size()) ){
		G4cout << "\nERROR --> OptPropManager::buildbordersurface(...):The <vol1> physical volume with name <" << keyval.at("vol1").get<std::string>() << "> corresponds to an empty list! This is an unexpected behaviour. The logical border surface <" << keyval.at("surfname").get<std::string>() << "> will not be built.\n" << G4endl;
		return;
	}
	
	
	const std::vector<G4VPhysicalVolume* > *vol_vec2 = FindPhysVol(keyval.at("vol2").get<std::string>());
	
	if(!vol_vec2){
		G4cout << "\nERROR --> OptPropManager::buildbordersurface(...):Could not find the <vol2> physical volume with name <" << keyval.at("vol2").get<std::string>() << ">. The logical border surface <" << keyval.at("surfname").get<std::string>() << "> will not be built!\n" << G4endl;
		return;
	}
	
	if( !(vol_vec2->size()) ){
		G4cout << "\nERROR --> OptPropManager::buildbordersurface(...): The <vol2> physical volume with name <" << keyval.at("vol2").get<std::string>() << "> corresponds to an empty list! This is an unexpected behaviour. The logical border surface <" << keyval.at("surfname").get<std::string>() << "> will not be built.\n" << G4endl;
		return;
	}
	
	
	
	//G4LogicalBorderSurface *logsurf = new G4LogicalBorderSurface( keyval.at("surfname").get<std::string>(), vol1, vol2, optsurf );
	G4int nLBS = BuildLogicalBorderSurface(keyval.at("surfname").get<std::string>(), keyval.at("vol1").get<std::string>(), keyval.at("vol2").get<std::string>(), optsurf->GetName());
	if(nLBS<=0){
		
		G4cout << "\nERROR --> OptPropManager::buildbordersurface(...): The logical surface <" << keyval.at("surfname").get<std::string>() << "> could not be made!\n" << G4endl;
		
	}else{
		if(fVerbose>=OptPropManager::kDetails){
			G4cout << "Detail --> OptPropManager::buildbordersurface(...): built " << nLBS << " instances of the logical surface <" << keyval.at("surfname").get<std::string>() << "> between volumes <" << keyval.at("vol1").get<std::string>() << "> and <" << keyval.at("vol2").get<std::string>() << ">, with optical surface <" << optsurf->GetName() << ">." << G4endl;
		}
		if(fVerbose>=OptPropManager::kInfo){
			G4cout << "Info --> OptPropManager::buildbordersurface(...): built logical surface <" << keyval.at("surfname").get<std::string>() << ">." << G4endl;
		}
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
				G4cout << "\nWARNING --> OptPropManager::buildbordersurface(...): The field corresponding to the property <" << it.key() << "> is not a string!\n" << G4endl;
				continue;
			}
			
			en_vec.resize(0);
			val_vec.resize(0);
			
			ReadValuesFromFile( it.value().get<std::string>(), en_vec, val_vec );
			
			if( (en_vec.size()==0) || (val_vec.size()==0) || (en_vec.size()!=val_vec.size()) ){
				G4cout << "\nWARNING --> OptPropManager::buildbordersurface(...): Wrong dimensions of the vectors built from the property file <" << it.value().get<std::string>() << ">. The field corresponding to the property <" << it.key() << "> is not a string!\n" << G4endl;
				continue;
			}
			
			if(fVerbose>=OptPropManager::kInfo){
				G4cout << "Info --> OptPropManager::buildbordersurface(...): Setting property <" << it.key() << "> for <" << optsurf->GetName() << "> from file <" << it.value().get<std::string>() << ">" << G4endl;
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
{
	//Not implemented yet
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


void OptPropManager::SetSurfModel(const G4String& logsurfname, const G4String& model)
{
	std::set<G4LogicalSurface* > *logsurflist = FindLogSurf(logsurfname);
	
	if(!logsurflist){
		G4cout << "\nERROR --> OptPropManager::SetSurfModel(...): Cannot find the logical surface(s) named <" << logsurfname << "> in the table of the instanced logical surfaces!\n" << G4endl;
		return;
	}
	
	if(!(logsurflist->size())){
		G4cout << "\nERROR --> OptPropManager::SetSurfModel(...): The list of logical surface(s) named <" << logsurfname << "> is empty!\n" << G4endl;
		return;
	}
	
	
	if(OptSurfModelMap.find(model)==OptSurfModelMap.end()){
		G4cout << "\nERROR --> OptPropManager::SetSurfModel(...): The model <" << model << "> is not in the list of known models and can not be applied to logical surface(s) <" << logsurfname << ">.\n" << G4endl;
		return;
	}
	
	
	G4int nFound=0;
	
	std::set<G4OpticalSurface*> oldoptsurf;
	
	std::set<G4LogicalSurface* >::iterator iT;
	for(iT=logsurflist->begin(); iT!=logsurflist->end(); ++iT){
		G4OpticalSurface *optsurf = dynamic_cast<G4OpticalSurface*>((*iT)->GetSurfaceProperty());
		
		if(optsurf){
			if(oldoptsurf.find( optsurf )==oldoptsurf.end()){
				oldoptsurf.insert( optsurf );
				optsurf->SetModel( OptSurfModelMap[model] );
			}
			nFound++;
		}else{
			G4cout << "WARNING --> OptPropManager::SetSurfModel(...): The logical surface <" << logsurfname << "> instance at <" << (*iT) << "> has no pointer (null) of the optical surface. The surface model <" << model << "> can not be applied to this surface instance." << G4endl;
		}
	}
	
	if((nFound>0) && fVerbose>=OptPropManager::kDetails){
		G4cout << "Detail --> OptPropManager::SetSurfModel(...): Model <" << model << "> applied to " << nFound << " out of " << logsurflist->size() << " instances of <" << logsurfname << "> logical surface." << G4endl;
	}else if(nFound==0){
		G4cout << "WARNING --> OptPropManager::SetSurfModel(...): The model <" << model << "> was not applied to any instance of <" << logsurfname << "> logical surface" << G4endl;
	}
}



void OptPropManager::SetSurfType(const G4String& logsurfname, const G4String& type)
{
	std::set<G4LogicalSurface* > *logsurflist = FindLogSurf(logsurfname);
	
	if(!logsurflist){
		G4cout << "\nERROR --> OptPropManager::SetSurfType(...): Cannot find the logical surface(s) named <" << logsurfname << "> in the table of the instanced logical surfaces!\n" << G4endl;
		return;
	}
	
	if(!(logsurflist->size())){
		G4cout << "\nERROR --> OptPropManager::SetSurfType(...): The list of logical surface(s) named <" << logsurfname << "> is empty!\n" << G4endl;
		return;
	}
	
	
	if(OptSurfTypeMap.find(type)==OptSurfTypeMap.end()){
		G4cout << "\nERROR --> OptPropManager::SetSurfType(...): The type <" << type << "> is not in the list of known surface types and can not be applied to the logical surface(s) <" << logsurfname << ">.\n" << G4endl;
		return;
	}
	
	
	G4int nFound = 0;
	
	std::set<G4OpticalSurface*> oldoptsurf;
	
	std::set<G4LogicalSurface* >::iterator iT;
	for(iT=logsurflist->begin(); iT!=logsurflist->end(); ++iT){
		G4OpticalSurface *optsurf = dynamic_cast<G4OpticalSurface*>((*iT)->GetSurfaceProperty());
		
		if(optsurf){
			if(oldoptsurf.find( optsurf )==oldoptsurf.end()){
				oldoptsurf.insert( optsurf );
				optsurf->SetType( OptSurfTypeMap[type] );
			}
			nFound++;
		}else{
			G4cout << "WARNING --> OptPropManager::SetSurfType(...): The logical surface <" << logsurfname << "> instance at <" << (*iT) << "> has no pointer (null) of the optical surface. The surface type <" << type << "> can not be applied to this surface instance." << G4endl;
		}
	}
	
	if((nFound>0) && fVerbose>=OptPropManager::kDetails){
		G4cout << "Detail --> OptPropManager::SetSurfType(...): Type <" << type << "> applied to " << nFound << " out of " << logsurflist->size() << " instances of <" << logsurfname << "> logical surface." << G4endl;
	}else if(nFound==0){
		G4cout << "WARNING --> OptPropManager::SetSurfType(...): The type <" << type << "> was not applied to any instance of <" << logsurfname << "> logical surface" << G4endl;
	}
}



void OptPropManager::SetSurfFinish(const G4String& logsurfname, const G4String& finish)
{
	std::set<G4LogicalSurface* > *logsurflist = FindLogSurf(logsurfname);
	
	if(!logsurflist){
		G4cout << "\nERROR --> OptPropManager::SetSurfFinish(...): Cannot find the logical surface(s) named <" << logsurfname << "> in the table of the instanced logical surfaces!\n" << G4endl;
		return;
	}
	
	if(!(logsurflist->size())){
		G4cout << "\nERROR --> OptPropManager::SetSurfFinish(...): The list of logical surface(s) named <" << logsurfname << "> is empty!\n" << G4endl;
		return;
	}
	
	
	if(OptSurfFinishMap.find(finish)==OptSurfFinishMap.end()){
		G4cout << "\nERROR --> OptPropManager::SetSurfFinish(...): The finish <" << finish << "> is not in the list of known surface finishes and can not be applied to the logical surface(s) <" << logsurfname << ">.\n" << G4endl;
		return;
	}
	
	
	G4int nFound = 0;
	
	std::set<G4OpticalSurface*> oldoptsurf;
	
	std::set<G4LogicalSurface* >::iterator iT;
	for(iT=logsurflist->begin(); iT!=logsurflist->end(); ++iT){
		G4OpticalSurface *optsurf = dynamic_cast<G4OpticalSurface*>((*iT)->GetSurfaceProperty());
		
		if(optsurf){
			if(oldoptsurf.find( optsurf )==oldoptsurf.end()){
				oldoptsurf.insert( optsurf );
				optsurf->SetFinish( OptSurfFinishMap[finish] );
			}
			nFound++;
		}else{
			G4cout << "WARNING --> OptPropManager::SetSurfFinish(...): The logical surface <" << logsurfname << "> instance at <" << (*iT) << "> has no pointer (null) of the optical surface. The surface finish <" << finish << "> can not be applied to this surface instance." << G4endl;
		}
	}
	
	if((nFound>0) && fVerbose>=OptPropManager::kDetails){
		G4cout << "Detail --> OptPropManager::SetSurfFinish(...): Finish <" << finish << "> applied to " << nFound << " out of " << logsurflist->size() << " instances of <" << logsurfname << "> logical surface." << G4endl;
	}else if(nFound==0){
		G4cout << "WARNING --> OptPropManager::SetSurfFinish(...): The finish <" << finish << "> was not applied to any instance of <" << logsurfname << "> logical surface" << G4endl;
	}
}


void OptPropManager::SetSurfPropFromFile(const G4String& logsurfname, const G4String& filename, const G4String& propertyname)
{
	std::vector<G4double> en_vec(0), val_vec(0);
	
	std::set<G4LogicalSurface* > *logsurflist = FindLogSurf(logsurfname);
	
	if(!logsurflist){
		G4cout << "\nERROR --> OptPropManager::SetSurfPropFromFile(...): Cannot find the logical surface(s) named <"<< logsurfname <<"> in the table of the instanced logical surfaces!\n" << G4endl;
		return;
	}
	
	ReadValuesFromFile( filename, en_vec, val_vec );
	
	
	if( (en_vec.size()==0) || (val_vec.size()==0) || (en_vec.size()!=val_vec.size()) ){
		G4cout << "\nERROR --> OptPropManager::SetSurfPropFromFile(...): Wrong size of the vectors read from file <"<< filename <<">. The property <" << propertyname << "> will not be set for the surface(s) named <" << logsurfname << ">!\n" << G4endl;
		return;
	}
	
	std::set<G4OpticalSurface*> oldsurfs;
	G4OpticalSurface* optsurf = NULL;
	
	std::set<G4LogicalSurface* >::iterator iT;
	for(iT=logsurflist->begin(); iT!=logsurflist->end(); ++iT){
		optsurf = dynamic_cast<G4OpticalSurface*>((*iT)->GetSurfaceProperty());
		
		if(optsurf){
			
			if(oldsurfs.find(optsurf)!=oldsurfs.end()){
				//The property for this optical surface has already been changed
				continue;
			}
			
			oldsurfs.insert(optsurf);
			
			G4MaterialPropertiesTable* propTab = optsurf->GetMaterialPropertiesTable();
			
			if(!propTab){
				propTab = new G4MaterialPropertiesTable();
				optsurf->SetMaterialPropertiesTable(propTab);
			}
			
			if(propTab->GetProperty( propertyname.c_str() )){
				propTab->RemoveProperty( propertyname.c_str() );
			}
			propTab->AddProperty( propertyname.c_str() ,(G4double*)&en_vec.at(0), (G4double*)&val_vec.at(0), en_vec.size() );
			
		}else{//The logical surface has no optical surface set which is very bad!!!
			
			G4cout << "\nERROR --> OptPropManager::SetSurfPropFromFile(...): The logical surface <" << logsurfname << "> instance at <" << (*iT) <<  "> has no optical surface pointer! Cannot apply the property <" << propertyname << "> for this logical surface instance\n" << G4endl;
			
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



G4int OptPropManager::BuildLogicalBorderSurface(const G4String& logsurfname, const G4String& physvol1, const G4String& physvol2, const G4String& optsurfname )
{
	//All the procedure should avoid that two logical surfaces with the same name can be created by mistake
	
	G4int iSurf=0;
	
	G4PhysicalVolumeStore *pPhysVolStore = G4PhysicalVolumeStore::GetInstance();
	
	if(!pPhysVolStore) return -1;
	
	
	if(!fDetConstr) return -2;
	
	DetConstrOptPh::PVmap *pv_map = (DetConstrOptPh::PVmap*)fDetConstr->GetVolsMap();
	
	if(!pv_map) return -3;
	
	std::vector<G4VPhysicalVolume*> vol1_vec, vol2_vec;
	
	
	
	if( pv_map->find(physvol1)==pv_map->end() ){
		G4cout << "\nWARNING --> OptPropManager::BuildLogicalBorderSurface(...): The physical volume <" << physvol1 << "> does not exists in the list of volumes. The logical surface <" << logsurfname << "> can not be created.\n" << G4endl;
		return -4;
	}else{
		//const std::string vname = physvol1.data();
		vol1_vec = (*pv_map)[ physvol1 ];
		//vol1_vec = (*pv_map)[ vname ];
	}
	
	if( pv_map->find(physvol2)==pv_map->end() ){
		G4cout << "\nWARNING --> OptPropManager::BuildLogicalBorderSurface(...): The physical volume <" << physvol2 << "> does not exists in the list of volumes. The logical surface <" << logsurfname << "> can not be created.\n" << G4endl;
		return -5;
	}else{
		//const std::string vname = physvol2.data();
		vol2_vec = (*pv_map)[ physvol2 ];
		//vol2_vec = (*pv_map)[ vname ];
	}
	
	size_t nVols1 = vol1_vec.size();
	if(nVols1==0){
		G4cout << "\nERROR --> OptPropManager::BuildLogicalBorderSurface(...): The list of physical volumes with name <" << physvol1 << "> is empty!!! The logical surface <" << logsurfname << "> will not be created.\n" << G4endl;
		return -6;
	}
	
	size_t nVols2 = vol2_vec.size();
	if(nVols2==0){
		G4cout << "\nERROR --> OptPropManager::BuildLogicalBorderSurface(...): The list of physical volumes with name <" << physvol2 << "> is empty!!! The logical surface <" << logsurfname << "> will not be created.\n" << G4endl;
		return -7;
	}
	
	
	G4SurfacePropertyTable *optsurftab = (G4SurfacePropertyTable*)G4OpticalSurface::GetSurfacePropertyTable();
	if( (!optsurftab) ) return -8; //This is a big problem as it is a static class member
	
	
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
	
	
	
	for(size_t iVol1 = 0; iVol1<nVols1; iVol1++){
		for(size_t iVol2 = 0; iVol2<nVols2; iVol2++){
			
			G4VPhysicalVolume *vol1 = vol1_vec.at(iVol1);
			G4VPhysicalVolume *vol2 = vol2_vec.at(iVol2);
			
			G4LogicalBorderSurface *logsurf = G4LogicalBorderSurface::GetSurface(vol1, vol2);
			
			if(logsurf){
				
				G4cout << "\nWARNING --> OptPropManager::BuildLogicalBorderSurface(...): The logical surface between volume <" << vol1->GetName() << "> at address" << vol1 << " and volume <" << vol2->GetName() << "> at address" << vol2 << " already exists with name <" << logsurf->GetName() << ">. Renaming it as <" << logsurfname << ">" << G4endl;
				if(optsurf){
					G4cout << " and assigning the surface <" << optsurf->GetName() << "> to it.\n" << G4endl;
				}else{
					G4cout << ".\n" << G4endl;
				}
				
				DeRegisterLogSurf(logsurf);
				
				logsurf->SetName(logsurfname);
				logsurf->SetSurfaceProperty(optsurf);
				
				RegisterLogSurf(logsurf);
				
				iSurf++;
				
			}else{
				
				if(fVerbose >= OptPropManager::kInfo){
					G4cout << "\Info --> OptPropManager::BuildLogicalBorderSurface(...): building logical surface <" << logsurfname << ">." << G4endl;
				}
				RegisterLogSurf( new G4LogicalBorderSurface(logsurfname, vol1, vol2, optsurf) );
				
				iSurf++;
			}
		}//Exiting from internal loop (jVol) of phys volumes
	}//Exiting from external loop (iVol) of phys volumes
	
	return iSurf;
}



void OptPropManager::SetOpticalSurface(const G4String& logsurfname, const G4String& optsurfname)
{
	G4OpticalSurface *optsurf = NULL;
	
	G4SurfacePropertyTable *optsurftab = (G4SurfacePropertyTable*)G4OpticalSurface::GetSurfacePropertyTable();
	
	if( !optsurftab ) return; //This is a big problem as they are static class members
	
	
	//size_t nLogSurfs = G4LogicalBorderSurface::GetNumberOfBorderSurfaces();
	size_t nOptSurfs = G4OpticalSurface::GetNumberOfSurfaceProperties();
	
	if( nOptSurfs==0 ) return;
	
	
	for(size_t iSurf=0; iSurf<nOptSurfs; iSurf++){
		if( (optsurftab->at(iSurf)->GetName())==optsurfname ) optsurf = static_cast<G4OpticalSurface*>(optsurftab->at(iSurf));
	}
	
	if(!optsurf) return;
	
	
	std::set< G4LogicalSurface* >* logsurflist = FindLogSurf(logsurfname);
	
	if(!logsurflist) return;
	
	if(logsurflist->size()) return;
	
	std::set< G4LogicalSurface* >::iterator iT;
	for(iT=logsurflist->begin(); iT!=logsurflist->end(); ++iT){
		(*iT)->SetSurfaceProperty(optsurf);
	}
}



void OptPropManager::RegisterLogSurf(G4LogicalSurface* logsurf)
{
	if(!logsurf) return;
	
	if(fLogSurfMap.find(logsurf->GetName())==fLogSurfMap.end()){
		std::set< G4LogicalSurface* > ls_set;
		ls_set.insert(logsurf);
		fLogSurfMap[logsurf->GetName()] = ls_set;
	}else{
		(fLogSurfMap[logsurf->GetName()]).insert(logsurf);
	}
}


void OptPropManager::DeRegisterLogSurf(G4LogicalSurface* logsurf)
{
	if(!logsurf) return;
	
	if(fLogSurfMap.find(logsurf->GetName())!=fLogSurfMap.end()){
		(fLogSurfMap[logsurf->GetName()]).erase(logsurf);
	}
}



#endif
