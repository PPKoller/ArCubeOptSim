#include<math.h>
#include<vector>

void O3_OptSim_createLUT(){

  int i = 0, j = 0; //iterator
  int n_evt = 0; //number of events
  int n_ph = 0; //number of photons per event
  char buffer[100]; //read buffer to skip lines

  //read input tree
  TFile * in_file = new TFile("out/OptSim_00000000.root", "READ");
  TChain * in_tree = new TChain("t1");
  in_tree->Add("out/*.root");

  n_evt = in_tree->GetEntries();
  n_ph = ((TParameter<int>*)in_file->Get("PrimNb"))->GetVal();

  //set branch addresses
  double prim_pos[3] = {0., 0., 0.};
  Long64_t totalhits = 0;
  vector<Int_t> * hit_vol_index = 0;
  vector<Int_t> * hit_vol_copy = 0;
  vector<Long64_t> * hit_vol_id = 0;
  vector<Double_t> * hit_time = 0;
  vector<Double_t> * hit_ypos = 0;

  in_tree->SetBranchAddress("prim_Xpos", &prim_pos[0]);
  in_tree->SetBranchAddress("prim_Ypos", &prim_pos[1]);
  in_tree->SetBranchAddress("prim_Zpos", &prim_pos[2]);
  in_tree->SetBranchAddress("totalhits", &totalhits);
  in_tree->SetBranchAddress("hit_vol_index", &hit_vol_index);
  in_tree->SetBranchAddress("hit_vol_copy", &hit_vol_copy);
  in_tree->SetBranchAddress("hit_vol_id", &hit_vol_id);
  in_tree->SetBranchAddress("hit_ypos", &hit_ypos);
  in_tree->SetBranchAddress("hit_time", &hit_time);

  //read format file
  FILE * format = fopen("OptSim_LUT_voxel_table.txt", "r");
  
  double xyz_min[3] = {0,0,0}; //minimum coordinates
  double xyz_max[3] = {0,0,0}; //maximum coordinates
  double dim_vox[3] = {0,0,0}; //voxel dimensions
  int n_vox[3] = {0,0,0}; //number of voxels

  fscanf(format,"%lf %lf %lf",&xyz_min[0],&xyz_min[1],&xyz_min[2]);
  printf("minimum coordinates (x,y,z) [mm]: %.3lf x %.3lf x %.3lf\n",xyz_min[0],xyz_min[1],xyz_min[2]);
  fscanf(format,"%lf %lf %lf",&xyz_max[0],&xyz_max[1],&xyz_max[2]);
  printf("maximum coordinates (x,y,z) [mm]: %.3lf x %.3lf x %.3lf\n",xyz_max[0],xyz_max[1],xyz_max[2]);
  fscanf(format,"%lf %lf %lf",&dim_vox[0],&dim_vox[1],&dim_vox[2]);
  printf("voxel dimensions (x,y,z) [mm]: %.3lf x %.3lf x %.3lf\n",dim_vox[0],dim_vox[1],dim_vox[2]);
  fscanf(format,"%d %d %d",&n_vox[0],&n_vox[1],&n_vox[2]);
  printf("number of voxels (x,y,z): %d x %d x %d\n",n_vox[0],n_vox[1],n_vox[2]);
  //fgets(buffer, 100, format); //fscanf(format,"%d %d",&n_evt,&n_ph);
  printf("number of events: %d \n", n_evt);
  //printf("number of photons per event: %d \n", n_ph);
  
  fclose(format);

  //write LUT tree
  TFile * out_file = new TFile("OptSim_LUT_ArgonCube2x2.root", "RECREATE");
  TTree * out_tree = new TTree("PhotonLibraryData","ArgonCube 2x2 LUT data");

  //define branch variables
  int Voxel = -1;
  int OpChannel = -1;
  float Visibility = -1.;
  int HitID = -1;
  float YPos = -1.;
  float Time = -1.;

  //create branches
  out_tree->Branch("Voxel", &Voxel);
  out_tree->Branch("OpChannel", &OpChannel);
  out_tree->Branch("Visibility", &Visibility);
  out_tree->Branch("HitID", &HitID);
  out_tree->Branch("YPos", &YPos);
  out_tree->Branch("Time", &Time);
  
  int voXYZ[3] = {0, 0, 0};
  int voxelID = -1;
  int volIdx = -1;
  int channelID = -1;
  int volID = -1;
  int n = 0;
  int nChannel = 48;
  int hits[nChannel];
  int hitID[nChannel];
  float ypos[nChannel];

  for(i=0; i<nChannel; i++){
    hits[i] = 0;
    ypos[i] = 0;
  }

  //write branches
  //loop over events
  for(i=0; i<n_evt; i++){
    in_tree->GetEntry(i);
  
    //calculate voxel ID based on primary position
    voXYZ[0] = int(floor((prim_pos[0]-xyz_min[0])/dim_vox[0]));
    voXYZ[1] = int(floor((prim_pos[1]-xyz_min[1])/dim_vox[1]));
    voXYZ[2] = int(floor((prim_pos[2]-xyz_min[2])/dim_vox[2]));
    voxelID = voXYZ[0]+voXYZ[1]*n_vox[0]+voXYZ[2]*n_vox[0]*n_vox[1];
    
    //fill tree and reset values
    if(Voxel!=voxelID){
      if(voxelID==0){
        Voxel = voxelID;
        std::cout << "processing voxel no. " << Voxel << " of " << n_vox[0]*n_vox[1]*n_vox[2] << " ..." << std::endl;
      }else{
        for(j=0; j<nChannel; j++){
          OpChannel = j;
          Visibility = (float)hits[j]/(float)(n*n_ph);
          YPos = ypos[j];
          HitID = hitID[j];
          if(Visibility>0) out_tree->Fill();
          hits[j] = 0;
          hitID[j] = -1;
          ypos[j] = -999.;
        }

        //reset values
        Voxel = voxelID;
        if(!(Voxel%10)) std::cout << "processing voxel no. " << Voxel << " of " << n_vox[0]*n_vox[1]*n_vox[2] << " ..." << std::endl;
        n = 0;
      }
    }
  
    //get optical channel ID
    for(j=0; j<totalhits; j++){
      volID = hit_vol_id->at(j);
      
      volIdx = hit_vol_index->at(j);
      if(volIdx==11 or volIdx==17) volIdx += 1;

      switch(volIdx){
        
        case 12: //LCM
          channelID = 0;

          switch(volID/10000){
            case 0: //DetR
              //std::cout << "DetR" << std::endl;
              break;
            case 1: //DetL
              //std::cout << "DetL" << std::endl;
              channelID += 24;
              break;
          }
          
          switch((volID%10000)/1000){
            case 0: //Plane0
              //std::cout << "  Plane0" << std::endl;
              if(channelID==24){
                channelID += 12;
              }else{
                channelID += 0;
              }
              break;
            case 1: //Plane1
              //std::cout << "  Plane1" << std::endl;
              if(channelID==24){
                channelID += 0;
              }else{
                channelID += 12;
              }
              break;
          }

          switch((volID%1000)/100){
            case 0: //LCM1
              //std::cout << "    LCM0" << std::endl;
              break;
            case 1: //LCM2
              //std::cout << "    LCM1" << std::endl;
              channelID += 2;
              break;
            case 2: //LCM3
              //std::cout << "    LCM2" << std::endl;
              channelID += 4;
              break;
          }
          
          switch(volID%10){
            case 0: //SiPM0
              //std::cout << "      SiPM0" << std::endl;
              break;
            case 1: //SiPM1
              //std::cout << "      SiPM1" << std::endl;
              channelID += 1;
              break;
          }

          break;//case 11
        
        case 18: //ArCLight
          channelID = 0;

          switch(volID/1000){
            case 0: //DetR
              //std::cout << "DetR" << std::endl;
              break;
            case 1: //DetL
              //std::cout << "DetL" << std::endl;
              channelID += 24;
              break;
          }
      
          switch((volID%1000)/100){
            case 0: //ArCLight0
              //std::cout << "  ArCLight0" << std::endl;
              if(channelID==24){
                channelID += 18;
              }else{
                channelID += 6;
              }
              break;
            case 1: //ArCLight1
              //std::cout << "  ArCLight1" << std::endl;
              if(channelID==24){
                channelID += 6;
              }else{
                channelID += 18;
              }
              break;
          }

          switch(volID%10){
            case 0: //SiPM0
              //std::cout << "    SiPM0" << std::endl;
              break;
            case 1: //SiPM1
              //std::cout << "    SiPM1" << std::endl;
              channelID += 1;
              break;
            case 2: //SiPM2
              //std::cout << "    SiPM2" << std::endl;
              channelID += 2;
              break;
            case 3: //SiPM3
              //std::cout << "    SiPM3" << std::endl;
              channelID += 3;
              break;
            case 4: //SiPM4
              //std::cout << "    SiPM4" << std::endl;
              channelID += 4;
              break;
            case 5: //SiPM5
              //std::cout << "    SiPM5" << std::endl;
              channelID += 5;
              break;
          }

          break; //case 17
      
      }//switch(hit_vol_index)
      
      //if(channelID==22) std::cout << " ==> channelID = " << channelID << ", YPos = " << hit_ypos->at(j) << ", voxID: " << voxelID << std::endl;
      hits[channelID] += 1;
      hitID[channelID] = volID;
      ypos[channelID] = hit_ypos->at(j);
    }

    n += 1;
  }

  //create vectors
  TVectorT<Double_t> Min = TVectorT<Double_t>(3);
  TVectorT<Double_t> Max = TVectorT<Double_t>(3);
  TVectorT<Double_t> NDivisions= TVectorT<Double_t>(3);

  //fill vectors
  for(i=0; i<3; i++){
    Min(i) = xyz_min[i];
    Max(i) = xyz_max[i];
    NDivisions(i) = n_vox[i];
  }

  //write tree
  out_tree->Write();
  Min.Write("Min");
  Max.Write("Max");
  NDivisions.Write("NDivisions");

  //print tree
  std::cout << "\nTree (" << n_evt << " evts, " << n_ph << " ph/evt) PhotonLibraryData is as follows\n" << std::endl;
  out_tree->Print();
  Min.Print("Min");
  Max.Print("Max");
  NDivisions.Print("NDivisions");

  //free memory
  delete in_file;
  delete out_file;

}
