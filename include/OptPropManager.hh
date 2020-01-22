#ifndef OPT_PROP_MANAGER_HH
#define OPT_PROP_MANAGER_HH


#include "G4OpBoundaryProcess.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"

#include <map>
#include <vector>
#include <fstream>
#include <string>

class G4String;

class OptPropManager
{
public:
	static OptPropManager* OptPropManagerInstance();
	
	
	
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
	
	
	
	static OptPropManager* gThis;
	
	std::map<G4String, G4SurfaceType> OptSurfTypeMap;
	std::map<G4String, G4OpticalSurfaceModel> OptSurfModelMap;
	std::map<G4String, G4OpticalSurfaceFinish> OptSurfFinishMap;
};

#endif