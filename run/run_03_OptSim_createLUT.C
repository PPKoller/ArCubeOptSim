#include<math.h>
#include<vector>
#include<stdio.h>
#include<stdlib.h>

void run_03_OptSim_createLUT(){

  //read environment variables
  char temp = (getenv("USRG"))[0];
  int usrg = atoi(&temp);

  int i = 0, j = 0; //iterator
  int n_evt = 0; //number of events
  int n_ph = 0; //number of photons per event
  char buffer[100]; //read buffer to skip lines

  //read input tree
  TFile * in_file = new TFile("/output/root_files/OptSim_00000000.root", "READ");
  TChain * in_tree = new TChain("t1");
  in_tree->Add("/output/root_files/*.root");

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
  FILE * format;

  if(usrg){
    format = fopen("/input/OptSim_LUT_voxel_table.txt", "r");
  }else{
    format = fopen("OptSim_LUT_voxel_table.txt", "r");
  }
  
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
  TFile * out_file = new TFile("/output/OptSim_LUT_ArgonCube2x2.root", "RECREATE");
  TTree * out_tree = new TTree("PhotonLibraryData","ArgonCube 2x2 LUT data");

  //define branch variables
  int Voxel = -1;
  int OpChannel = -1;
  float Visibility = -1.;
  int VolID = -1;
  int VolIdx = -1;
  float YPos = -1.;
  float Time = -1.;

  //create branches
  out_tree->Branch("Voxel", &Voxel);
  out_tree->Branch("OpChannel", &OpChannel);
  out_tree->Branch("Visibility", &Visibility);
  out_tree->Branch("VolID", &VolID);
  out_tree->Branch("VolIdx", &VolIdx);
  out_tree->Branch("YPos", &YPos);
  out_tree->Branch("Time", &Time);
  
  int voXYZ[3] = {0, 0, 0};
  int voxelID = -1;
  int channelID = -1;
  int sipmID = -1;
  int hitVolID = -1;
  int hitVolIdx = -1;
  int n = 0;
  int nChannel = 48;
  int hits[nChannel];
  int volID[nChannel];
  int volIdx[nChannel];
  float ypos[nChannel];

  for(i=0; i<nChannel; i++){
    hits[i] = 0;
    volID[i] = -1;
    volIdx[i] = -1;
    ypos[i] = -999.;
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
          VolID = volID[j];
          VolIdx = volIdx[j];
          YPos = ypos[j];
          if(Visibility>0) out_tree->Fill();
          hits[j] = 0;
          volID[j] = -1;
          volIdx[j] = -1;
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
      hitVolID = hit_vol_id->at(j);
      
      hitVolIdx = hit_vol_index->at(j);
      if(hitVolIdx==19 or hitVolIdx==25) hitVolIdx += 1;

      switch(hitVolIdx){
        case 20: //LCM
          channelID = 0;
          sipmID = 0;

          switch((hitVolID%1000)/100){
            case 0: //LCM1
              sipmID += 0;
              break;
            case 1: //LCM2
              sipmID += 2;
              break;
            case 2: //LCM3
              sipmID += 4;
              break;
          }
          
          switch(hitVolID%10){
            case 0: //SiPM0
              sipmID += 0;
              break;
            case 1: //SiPM1
              sipmID += 1;
              break;
          }

          switch(hitVolID/10000){
            case 0: //DetR
              channelID += sipmID;
              
              switch((hitVolID%10000)/1000){
                case 0: //Plane0
                  channelID += 0;
                  break;
                case 1: //Plane1
                  channelID += 12;
                  break;
              }
              
              break;//DetR
            
            case 1: //DetL
              channelID += 24;
              channelID += 5-sipmID;
              
              switch((hitVolID%10000)/1000){
                case 0: //Plane0
                  channelID += 12;
                  break;
                case 1: //Plane1
                  channelID += 0;
                  break;
              }
              
              break;//DetL
          }

          break;//case 11
        
        case 26: //ArCLight
          channelID = 0;
          sipmID = 0;
      
          switch(hitVolID%10){
            case 0: //SiPM0
              sipmID += 0;
              break;
            case 1: //SiPM1
              sipmID += 1;
              break;
            case 2: //SiPM2
              sipmID += 2;
              break;
            case 3: //SiPM3
              sipmID += 3;
              break;
            case 4: //SiPM4
              sipmID += 4;
              break;
            case 5: //SiPM5
              sipmID += 5;
              break;
          }

          switch(hitVolID/1000){
            case 0: //DetR
              channelID += sipmID;
              
              switch((hitVolID%1000)/100){
                case 0: //ArCLight0
                  channelID += 6;
                  break;
                case 1: //ArCLight1
                  channelID += 18;
                  break;
              }

              break;//DetR

            case 1: //DetL
              channelID += 24;
              channelID += 5-sipmID;
              
              switch((hitVolID%1000)/100){
                case 0: //ArCLight0
                  channelID += 18;
                  break;
                case 1: //ArCLight1
                  channelID += 6;
                  break;
              }

              break;//DetL
          }

          break;//case 17
      
      }//switch(hit_vol_index)
      
      hits[channelID] += 1;
      volID[channelID] = hitVolID;
      volIdx[channelID] = hitVolIdx;
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
