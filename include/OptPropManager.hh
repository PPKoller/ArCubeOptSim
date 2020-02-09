#ifndef OPT_PROP_MANAGER_HH
#define OPT_PROP_MANAGER_HH


#include "G4OpBoundaryProcess.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"

#include "nlohmann/json.hpp"

#include <map>
#include <vector>
#include <fstream>
#include <string>



class G4String;


// for convenience
using json = nlohmann::json;

//Definition of the functins that take a "processing unit" from the main json file structure
class OptPropManager;
typedef void (OptPropManager::*json_proc_memfunc)(const json keyval);


class OptPropManager
{
public:
	static OptPropManager* GetInstance();
	
	void ProcessJsonFile(const G4String& jsonfilename);
	
	
	
	//loads the refractive index from a 2 column ascii file and gives it to a material if this exists
	//The first column should be the energy photon (not the wavelenght) and the second column the refraction index
	void SetMaterialRindex(const G4String& materialname, const G4String& asciifilename);
	
	//loads the absorption lenght from a 2 column ascii file and gives it to a material if this exists
	//The first column should be the energy photon (not the wavelenght) and the second column the lenght (in m)
	void SetMaterialAbsLenght(const G4String& materialname, const G4String& asciifilename);
	
	//loads the Rayleigh scattering lenght from a 2 column ascii file and gives it to a material if this exists
	//The first column should be the energy photon (not the wavelenght) and the second column the lenght (in m)
	void SetMaterialRayleighLenght(const G4String& materialname, const G4String& asciifilename);
	
	
	void SetMaterialRindex(const G4String& materialname, const G4int Nentries, const G4double* wavelenghts, const G4double* rindexes);
	void SetMaterialRindex(const G4String& materialname, const std::vector<G4double>& wavelenghts, const std::vector<G4double>& rindexes);
	
	
	void SetMaterialAbsLenght(const G4String& materialname, const G4int Nentries, const G4double* wavelenghts, const G4double* abslenghts);
	void SetMaterialAbsLenght(const G4String& materialname, const std::vector<G4double>& wavelenghts, const std::vector<G4double>& abslenghts);
	
	
	void SetMaterialRayleighLenght(const G4String& materialname, const G4int Nentries, const G4double* wavelenghts, const G4double* rayleighlenghts);
	void SetMaterialRayleighLenght(const G4String& materialname, const std::vector<G4double>& wavelenghts, const std::vector<G4double>& rayleighlenghts);
	
	
	//To use these methods the "logical surface" should already have an optical surface assigned otherwise it doesn't apply these things
	void SetOpticalSurfaceModel(const G4String& logsurfname, const G4String& model);
	void SetOpticalSurfaceType(const G4String& logsurfname, const G4String& type);
	void SetOpticalSurfaceFinish(const G4String& logsurfname, const G4String& finish);
	
	void SetSurfSigmaAlpha(const G4String& logsurfname, const G4double& s_a);
	
	
	void BuildOpticalSurface(const G4String& optsurfname, const G4String& model, const G4String& type, const G4String& finish );
	
	//Defaults to glisur, dielectric_dielectric, polished
	void BuildOpticalSurface(const G4String& optsurfname)
	{
		BuildOpticalSurface(optsurfname, "glisur", "dielectric_dielectric", "polished");
	};
	
	
	void BuildLogicalBorderSurface(const G4String& logsurfname, const G4String& optsurfname, const G4String& physvol1, const G4String& physvol2);
	
	
	void SetOpticalSurface(const G4String& logsurfname, const G4String& optsurfname);
	
	
private:
	OptPropManager();
	~OptPropManager(){;};
	
	
	G4int fVerbosity;
	
	static OptPropManager* gThis;
	
	std::map<G4String, G4SurfaceType> OptSurfTypeMap;
	std::map<G4String, G4OpticalSurfaceModel> OptSurfModelMap;
	std::map<G4String, G4OpticalSurfaceFinish> OptSurfFinishMap;
	
	
	
	//The following functions process the info of the "setmatprop" key word at the upper level of the jsonfile structure
	
	//Modifies the optical properties of an already existing material
	void setmatprop(const json keyval);
	
	//Modifies the properties of an already existing G4OpticalSurface
	void setoptsurf(const json keyval);
	
	//Modifies the properties of an already existing G4LogicalBorderSurface. If necessary accesses to its G4OpticalSurface to apply the properties
	void setbordersurf(const json keyval);
	
	//Modifies the properties of an already existing G4LogicalSkinSurface. If necessary accesses to its G4OpticalSurface to apply the properties
	void setskinsurf(const json keyval);
	
	//Builds a new G4OpticalSurface if it doesn't already exist (check by name). If it exists doesn't do anything
	void buildoptsurf(const json keyval);
	
	//Builds a new G4LogicalBorderSurface if it doesn't already exist (check by name). If it exists doesn't do anything. In the process an already instanced G4OpticalSurface must be given to the surface, otherwise this object won't be created.
	void buildbordersurface(const json keyval);
	
	//Builds a new G4LogicalSkinSurface if it doesn't already exist (check by name). If it exists doesn't do anything. In the process an already instanced G4OpticalSurface must be given to the surface, otherwise this object won't be created.
	void buildskinsurf(const json keyval);
	
	
	
	//Map from the key (a string obj) to the corresponding function
	std::map<G4String, json_proc_memfunc> json_proc_tab;
	
	
	//Service functions used by the stuff here above
	G4VPhysicalVolume* FindPhysVol(const G4String& physvolname);
	
	G4LogicalBorderSurface* FindBorderSurf(const G4String& logsurfname);
	
	G4LogicalSkinSurface* FindSkinSurf(const G4String& logsurfname);
	
	G4OpticalSurface* FindOptSurf(const G4String& optsurfname);
	
	G4Material* FindMaterial(const G4String& materialname);
	
	void ReadValuesFromFile(const G4String& filename, std::vector<G4double>& ph_en, std::vector<G4double>& vals);
};

#endif