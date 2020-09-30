#include<math.h>
#include<vector>
#include<stdio.h>
#include<stdlib.h>

void run_03_OptSim_createLUT(){

  //read environment variables
  char temp = (getenv("USRG"))[0];
  int usrg = atoi(&temp);

  int i = 0, j = 0; //iterator
  int n_ph = 0; //number of photons per event
  int n_evt = 0; //number of events
  int n_entries = 0; //number of events total
  char buffer[100]; //read buffer to skip lines

  //read input tree
  TChain * in_tree = new TChain("t1");
  in_tree->Add("/output/root_files/*.root");

  n_entries = in_tree->GetEntries();

  //set branch addresses
  Long64_t totalhits = 0;
  vector<Int_t> * hit_vol_index = 0;
  vector<Int_t> * hit_vol_copy = 0;
  vector<Long64_t> * hit_vol_id = 0;
  vector<Double_t> * hit_time = 0;
  vector<Double_t> * hit_ekin = 0;

  in_tree->SetBranchAddress("totalhits", &totalhits);
  in_tree->SetBranchAddress("hit_vol_index", &hit_vol_index);
  in_tree->SetBranchAddress("hit_vol_copy", &hit_vol_copy);
  in_tree->SetBranchAddress("hit_vol_id", &hit_vol_id);
  in_tree->SetBranchAddress("hit_time", &hit_time);
  in_tree->SetBranchAddress("hit_ekin", &hit_ekin);

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
  fscanf(format,"%d %d",&n_evt,&n_ph);
  printf("number of events per voxel: %d \n", n_evt);
  printf("number of photons per event: %d \n", n_ph);
  
  fclose(format);

  //write LUT tree
  TFile * out_file = new TFile("/output/OptSim_LUT_ArgonCube2x2.root", "RECREATE");
  TTree * out_tree = new TTree("PhotonLibraryData","ArgonCube 2x2 LUT data");

  //define branch variables
  int Voxel = -1;
  int Voxel_temp = -1;
  int OpChannel = -1;
  float Visibility = -1.;
  float T4 = -1.;
  TH1F * Time = NULL;
  //float Time = -1.;

  //create branches
  out_tree->Branch("Voxel", &Voxel);
  out_tree->Branch("OpChannel", &OpChannel);
  out_tree->Branch("Visibility", &Visibility);
  out_tree->Branch("T4",&T4);
  out_tree->Branch("Time", "TH1F", &Time);
  
  //variable declaration
  int voxelID = -1;
  int channelID = -1;
  int sipmID = -1;
  int hitVolID = 0;
  int hitVolIdx = -1;
  int nChannel = 48;
  int nVox = n_vox[2]*n_vox[0]*n_vox[1];
  int hits[nChannel];
  vector<TH1F*> timeVec;

  // SiPM efficiency for shifted spectrum
  float effSiPM = 0.4;
  
  //vector initialization
  for(i=0; i<nChannel; i++){
    hits[i] = 0;
    timeVec.push_back(new TH1F(Form("Voxel%06d_OpChannel%02d",voxelID+1,i),"Hit time distribution",100,0.,100.));
  }

  //write branches
  //loop over events (<= to make sure last voxel will be filled!)
  for(i=0; i<=n_entries; i++){
    in_tree->GetEntry(i);

    //new voxel / last entry
    if(!(i%n_evt)){
      voxelID += 1;

      //accept voxelID in case this is the first voxel
      if(voxelID==0){
        Voxel = voxelID;
        std::cout << "processing voxel no. " << Voxel << " of " << nVox << " ..." << std::endl;
      }
      
      //fill tree and reset values in case this is NOT the first voxel
      else{
        
        //create mean for LCM SiPM pairs
        for(j=0; j<18; j+=2){
          if(j==6) j += 6;
          hits[j] = (hits[j]+hits[j+1])/2;
          hits[j+1] = hits[j];
        }
        
        //loop over optical channels
        for(j=0; j<nChannel; j++){
          OpChannel = j;
          Visibility = effSiPM*(float)hits[j]/(float)(n_evt*n_ph);
          Time = timeVec[j];
          T4 = Time->GetBinCenter(Time->FindFirstBinAbove(4));

          //only fill if vixibility > 0
          if(Visibility>0){
            out_tree->Fill();

            //Filly symmetry pair
            Voxel_temp = Voxel;
            Voxel += 2*nVox;
            Voxel -= (2*(Voxel_temp/(n_vox[0]*n_vox[1]))+1)*(n_vox[0]*n_vox[1]);
            OpChannel = (OpChannel+nChannel/2)%nChannel;
            timeVec[j]->SetName(Form("Voxel%06d_OpChannel%02d",Voxel,OpChannel));
            out_tree->Fill();
            Voxel = Voxel_temp;
          }/*else{
            std::cout << "Voxel: " << Voxel << ", OpChannel: " << j << " is has 0 visibility..." << std::endl;
          }*/

          //reset hits
          hits[j] = 0;

        }

        if(i==n_entries) break;

        //accept new voxelID
        Voxel = voxelID;
        if(!(Voxel%10)) std::cout << "processing voxel no. " << Voxel << " of " << nVox << " ..." << std::endl;

        //reset histos
        timeVec.clear();
        for(j=0; j<nChannel; j++){
          timeVec.push_back(new TH1F(Form("Voxel%06d_OpChannel%02d",Voxel,j),"Hit time distribution",100,0.,100.));
        }
      }
    }

    if(i==n_entries) break;

    //get optical channel ID
    for(j=0; j<totalhits; j++){
      
      //only count shifted photons
      if(hit_ekin->at(j) > 9E-6) continue;

      hitVolID = hit_vol_id->at(j);
      
      hitVolIdx = hit_vol_index->at(j);
      if(hitVolIdx==10 or hitVolIdx==15) hitVolIdx += 1;

      switch(hitVolIdx){
        case 11: //LCM
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
        
        case 16: //ArCLight
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
      timeVec[channelID]->Fill(hit_time->at(j));
      hits[channelID] += 1;
    }
  }

  //create vectors
  TVectorT<Double_t> Min = TVectorT<Double_t>(3);
  TVectorT<Double_t> Max = TVectorT<Double_t>(3);
  TVectorT<Double_t> NDivisions= TVectorT<Double_t>(3);

  //fill vectors
  n_vox[2] *= 2;
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
  std::cout << "\nTree (" << n_entries << " evts, " << n_ph << " ph/evt) PhotonLibraryData is as follows\n" << std::endl;
  out_tree->Print();
  Min.Print("Min");
  Max.Print("Max");
  NDivisions.Print("NDivisions");

  //free memory
  delete out_tree;
  delete out_file;

}
