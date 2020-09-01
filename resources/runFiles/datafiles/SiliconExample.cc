  //TPB (Tetra-Phenyl Butadiene)
  //Density copied from PseudoCumene
  density=0.9*g/cm3;
  fTPB = new G4Material (name="TPB",density,ncomponents=2,kStateSolid);
  fTPB->AddElement(C,28);
  fTPB->AddElement(H,22);

  //ThreeMFoil (Polyethylene terephthalate)
  //http://multimedia.3m.com/mws/mediawebserver?mwsId=66666UuZjcFSLXTtM8T6oXTVEVuQEcuZgVs6EVs6E666666--
  //Density and composition from wiki
  density=1.4*g/cm3;
  fThreeMFoil = new G4Material(name="ThreeMFoil",density,ncomponents=3);
  fThreeMFoil->AddElement(C,10);
  fThreeMFoil->AddElement(H,8);
  fThreeMFoil->AddElement(O,4);

  //Fused Silica (same composition as quartz)
  //Density from Wikipedia
  density=2.203*g/cm3;
  fFusedSilica = new G4Material(name="FusedSilica",density,ncomponents=2,kStateSolid);
  fFusedSilica->AddElement(Si,1);
  fFusedSilica->AddElement(O,2);

  // Metal Silicon
  density = 2.33*g/cm3;
  fMetalSilicon =  new G4Material(name="MetalSilicon",density,ncomponents=1);
  fMetalSilicon->AddElement(Si,1.0);


  //---------------------------------------------------------------------------------
  // Metal Silicon
  //---------------------------------------------------------------------------------
  const int mySiliconRindDim = 46; //from https://refractiveindex.info/?shelf=main&book=Si&page=Aspnes
  double mySiliconRindEne[mySiliconRindDim] = {0.2066,0.2101,0.2138,0.2175,0.2214,0.2254,0.2296,0.2339,0.2384,0.2431,0.2480,0.2530,0.2583,0.2638,0.2695,0.2755,0.2818,0.2883,0.2952,0.3024,0.3100,0.3179,0.3263,0.3351,0.3444,0.3542,0.3647,0.3757,0.3875,0.3999,0.4133,0.4275,0.4428,0.4592,0.4769,0.4959,0.5166,0.5391,0.5636,0.5904,0.6199,0.6525,0.6888,0.7293,0.7749,0.8266}; //um
  double mySiliconRind[mySiliconRindDim] = {1.010,1.083,1.133,1.186,1.247,1.340,1.471,1.579,1.589,1.571,1.570,1.597,1.658,1.764,1.988,2.452,3.120,4.087,4.888,5.020,5.010,5.016,5.065,5.156,5.296,5.610,6.522,6.709,6.062,5.570,5.222,4.961,4.753,4.583,4.442,4.320,4.215,4.123,4.042,3.969,3.906,3.847,3.796,3.752,3.714,3.673}; //rindex
  for(int ij=0; ij<mySiliconRindDim; ij++) {
    mySiliconRindEne[ij] = h_Planck*c_light/(1000.*mySiliconRindEne[ij]*nm);
  }
  // same goal as bialkali
  G4MaterialPropertiesTable *myMetalSilicon= new G4MaterialPropertiesTable();
  //http://refractiveindex.info/?shelf=main&book=Si&page=Pierce
  //myMetalSilicon->AddProperty("IMAGINARYRINDEX",  mySiEnergy  ,mySiImRind ,   6);
  //myMetalSilicon->AddProperty("RINDEX",    myBialkaliEnergy, myBialkaliRI,  4);
  myMetalSilicon->AddProperty("RINDEX",    mySiliconRindEne, mySiliconRind,  mySiliconRindDim);
  myMetalSilicon->AddProperty("ABSLENGTH", myBialkaliEnergy, myBialkaliAbs, 4);
  fMetalSilicon->SetMaterialPropertiesTable(myMetalSilicon);
  fMetalCopper->SetMaterialPropertiesTable(myMetalSilicon);

  //---------------------------------------------------------------------------------
  // Liquid Argon
  //---------------------------------------------------------------------------------
  G4MaterialPropertiesTable *myLiquidArgon = new G4MaterialPropertiesTable();
  dim = 0;
  G4double LArRefIndex[1000], LArRefIndexEne[1000],LArRayLength[1000],LArAbsLength[1000] ;
  for(int ij=0;ij<55; ij++) {
    LArRefIndexEne[ij] = (1.0 + ij*0.2)*eV;
    const G4double lambda     = h_Planck*c_light/LArRefIndexEne[ij];
    LArRefIndex[ij]    = GetLArRefIndex(lambda/nm);
    LArRayLength[ij]   = GetLArRayLength(lambda/nm)*DSParameters::Get()->GetLArRayleighScale();
    // L700 why no to modify LAr absorption length for VUV light?
    if (lambda < 150*nm )  LArAbsLength[ij]   = DSParameters::Get()->GetLiquidArgonUVAbs()*m;
    if (lambda > 150*nm )  LArAbsLength[ij]   = DSParameters::Get()->GetLiquidArgonVisAbs()*m;
    dim = ij;
  }
  myLiquidArgon->AddProperty("RINDEX",   LArRefIndexEne , LArRefIndex,  dim+1);
  myLiquidArgon->AddProperty("RAYLEIGH", LArRefIndexEne , LArRayLength, dim+1);
  myLiquidArgon->AddProperty("ABSLENGTH",LArRefIndexEne , LArAbsLength, dim+1);

  //---------------------------------------------------------------------------------
  // TPB
  //---------------------------------------------------------------------------------

  G4MaterialPropertiesTable *myTPB = new G4MaterialPropertiesTable();

  G4double TPB_ENE[1000], TPB_EMISSION_VAL[1000], TPB_ABSORPTION_VAL[1000];
  dim = 0 ;
  G4double TPBNorma = 0 ;
  ifstream ftpb_emission("../data/detector/tpb_emission_spectrum.dat");
  if ( !ftpb_emission.is_open())
    DSLog(fatal) << "ERROR: Could not open TPB emission file" <<endlog;
  while(!ftpb_emission.eof()) {
    ftpb_emission >> myene >> myvalue ;
    if(ftpb_emission.eof()) break;
    TPB_ENE[dim] = myene*eV;
    if(hc/ (TPB_ENE[dim]/nm) > 600) {
      TPB_EMISSION_VAL[dim] = 0 ;
    } else  {
      TPB_EMISSION_VAL[dim] = myvalue;
      TPBNorma += myvalue ;
    }
    dim++;
  }
  ftpb_emission.close();

  G4double  TPB_WLS_ABSORPTION_VAL[1000], TPB_RINDEX[1000];
  dim = 0 ;
  ifstream ftpb_absorption("../data/detector/tpb_absorption_length.dat");
  if ( !ftpb_absorption.is_open())
    DSLog(fatal) << "ERROR: Could not open TPB absorption file" <<endlog;
  while(!ftpb_absorption.eof()) {
    ftpb_absorption >> myene >> myvalue ;
    if(ftpb_absorption.eof()) break;
    TPB_WLS_ABSORPTION_VAL[dim] = myvalue*m*DSParameters::Get()->GetWLSAbsorptionFactor();
    if (myene > 8.3)  {
      TPB_ABSORPTION_VAL[dim]  = DSParameters::Get()->GetTPBUVAbs()*m;
      TPB_RINDEX[dim]          = DSParameters::Get()->GetTPBUVRind();
     }
    else {
      TPB_ABSORPTION_VAL[dim]  = DSParameters::Get()->GetTPBVisAbs()*m;
      TPB_RINDEX[dim]          = DSParameters::Get()->GetTPBVisRind();
    }
    TPB_EMISSION_VAL[dim]       /= TPBNorma;

    dim++;
  }
  ftpb_absorption.close();

  myTPB->AddProperty("WLSCOMPONENT",TPB_ENE, TPB_EMISSION_VAL,       dim);
  myTPB->AddProperty("WLSABSLENGTH",TPB_ENE, TPB_WLS_ABSORPTION_VAL, dim);
  myTPB->AddProperty("ABSLENGTH",   TPB_ENE, TPB_ABSORPTION_VAL,     dim);
  //myTPB->AddProperty("RINDEX",      TPB_ENE, TPB_RINDEX,             dim);
  myTPB->AddProperty("RINDEX", LArRefIndexEne, LArRefIndex, LArRefIndexDim); //set TPB RINDEX to same as LAr
  myTPB->AddConstProperty("WLSMEANNUMBERPHOTONS",DSParameters::Get()->GetWLSMeanNumberPhotons());
  myTPB->AddConstProperty("WLSTIMECONSTANT",DSParameters::Get()->GetWLSTimeConstant_ns()*ns);
  myTPB->AddConstProperty("WLSEFFICIENCY",DSParameters::Get()->GetWLSEfficiency());
  fTPB->SetMaterialPropertiesTable(myTPB);



G4double DSMaterial::GetLArRefIndex(G4double lambda) {

  G4double epsilon;
  const G4double LArRho = 1.390;
  const G4double ArRho  = 0.001784;
  if (lambda <= 107.05) return 1.0e4; // lambda MUST be > 107.05 nm
  epsilon = lambda / 1000.0; // switch to micrometers
  epsilon = 1.0 / (epsilon * epsilon); // 1 / (lambda)^2
  epsilon = 1.2055e-2 * ( 0.2075 / (91.012 - epsilon) +
                          0.0415 / (87.892 - epsilon) +
                          4.3330 / (214.02 - epsilon) );
  epsilon *= (8./12.); // Bideau-Sellmeier -> Clausius-Mossotti
  epsilon *= (LArRho / ArRho); // density correction (Ar gas -> LAr liquid)
  if ((epsilon < 0.0) || (epsilon > 0.999999)) return 4.0e6;
  epsilon = (1.0 + 2.0 * epsilon) / (1.0 - epsilon); // solve Clausius-Mossotti

  return sqrt(epsilon);
}
//---------------------------------------------------------------------------------------

G4double DSMaterial::GetGArRefIndex(G4double lambda) {

  G4double epsilon;

  if (lambda <= 107.05) return 1.0e4; // lambda MUST be > 107.05 nm
  epsilon = lambda / 1000.0; // switch to micrometers
  epsilon = 1.0 / (epsilon * epsilon); // 1 / (lambda)^2
  epsilon = 1.2055e-2 * ( 0.2075 / (91.012 - epsilon) +
                          0.0415 / (87.892 - epsilon) +
                          4.3330 / (214.02 - epsilon) );
  epsilon *= (8./12.); // Bideau-Sellmeier -> Clausius-Mossotti
  if ((epsilon < 0.0) || (epsilon > 0.999999)) return 4.0e6;
  epsilon = (1.0 + 2.0 * epsilon) / (1.0 - epsilon); // solve Clausius-Mossotti

  return sqrt(epsilon);
}

//---------------------------------------------------------------------------------------

G4double DSMaterial::GetLArRayLength(G4double lambda) {
  const G4double LArT   = 87.0;                 // the actual temperature of LAr in detector
  const G4double LArKT  = 2.18e-10 * cm*cm/(1.e-5*newton); // LAr isothermal compressibility
  const G4double k      = 1.380658e-23;            // the Boltzmann constant

  G4double h = GetLArEpsilon(lambda);
  if (h < 1.00000001) h = 1.00000001; // just a precaution
  h = (h - 1.0) * (h + 2.0); // the "dielectric constant" dependance
  h *= h; // take the square
  h *= LArKT * LArT *  k  ; // compressibility * temperature * Boltzmann constant
  h /= lambda * lambda * lambda * lambda * 1.0e-36; // (lambda)^4
  h *= 9.18704494231105429; // (2 * Pi / 3)^3
//   if ( h < (1.0 / (10.0 * km)) ) h = 1.0 / (10.0 * km); // just a precaution
//   if ( h > (1.0 / (0.1 * nanometer)) ) h = 1.0 / (0.1 * nanometer); // just a precaution
  //  return ( 100.0 / h );
  return ( 100./h )*um;
}

//---------------------------------------------------------------------------------------

G4double DSMaterial::GetGArRayLength(G4double lambda) {
  const G4double ArT   = 87.0;                 // the actual temperature of LAr in detector
  const G4double ArKT  = 2.18e-5*cm*cm/(1.e-5*newton); // LAr isothermal compressibility
  const G4double k     = 1.380658e-23;            // the Boltzmann constant

  G4double h = GetGArEpsilon(lambda);
  if (h < 1.00000001) h = 1.00000001; // just a precaution
  h = (h - 1.0) * (h + 2.0); // the "dielectric constant" dependance
  h *= h; // take the square
  h *= ArKT * ArT * k; // compressibility * temperature * Boltzmann constant
  h /= lambda * lambda * lambda * lambda * 1.0e-36; // (lambda)^4
  h *= 9.18704494231105429; // (2 * Pi / 3)^3
//   if ( h < (1.0 / (10.0 * km)) ) h = 1.0 / (10.0 * km); // just a precaution
//   if ( h > (1.0 / (0.1 * nanometer)) ) h = 1.0 / (0.1 * nanometer); // just a precaution
  return ( 100.0 / h )*um;
}


//---------------------------------------------------------------------------------------
G4double DSMaterial::GetLArEpsilon(G4double lambda) {
  const G4double LArRho = 1.390;
  const G4double ArRho  = 0.001784;
  G4double epsilon;

  if (lambda <= 107.05) return 1.0e4; // lambda MUST be > 107.05 nm
  epsilon = lambda / 1000.0; // switch to micrometers
  epsilon = 1.0 / (epsilon * epsilon); // 1 / (lambda)^2
  epsilon = 1.2055e-2 * ( 0.2075 / (91.012 - epsilon) +
                          0.0415 / (87.892 - epsilon) +
                          4.3330 / (214.02 - epsilon) );
  epsilon *= (8./12.); // Bideau-Sellmeier -> Clausius-Mossotti
  epsilon *= (LArRho / ArRho); // density correction (Ar gas -> LAr liquid)
  if ((epsilon < 0.0) || (epsilon > 0.999999)) return 4.0e6;
  epsilon = (1.0 + 2.0 * epsilon) / (1.0 - epsilon); // solve Clausius-Mossotti

  return epsilon;
}
//---------------------------------------------------------------------------------------
G4double DSMaterial::GetGArEpsilon(G4double lambda) {

  G4double epsilon;

  if (lambda <= 107.05) return 1.0e4; // lambda MUST be > 107.05 nm
  epsilon = lambda / 1000.0; // switch to micrometers
  epsilon = 1.0 / (epsilon * epsilon); // 1 / (lambda)^2
  epsilon = 1.2055e-2 * ( 0.2075 / (91.012 - epsilon) +
                          0.0415 / (87.892 - epsilon) +
                          4.3330 / (214.02 - epsilon) );
  epsilon *= (8./12.); // Bideau-Sellmeier -> Clausius-Mossotti
  if ((epsilon < 0.0) || (epsilon > 0.999999)) return 4.0e6;
  epsilon = (1.0 + 2.0 * epsilon) / (1.0 - epsilon); // solve Clausius-Mossotti

  return epsilon;
}

