#include<math.h>
#include<vector>

void createLUT(){

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
  vector<Long64_t> * hit_vol_id = 0;
  vector<double_t> * hit_time = 0;

  in_tree->SetBranchAddress("prim_Xpos", &prim_pos[0]);
  in_tree->SetBranchAddress("prim_Ypos", &prim_pos[1]);
  in_tree->SetBranchAddress("prim_Zpos", &prim_pos[2]);
  in_tree->SetBranchAddress("totalhits", &totalhits);
  in_tree->SetBranchAddress("hit_vol_id", &hit_vol_id);
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
  int Voxel = 0;
  int OpChannel = 0;
  float Visibility = 0;
  float Time = 0;

  //create branches
  out_tree->Branch("Voxel", &Voxel);
  out_tree->Branch("OpChannel", &OpChannel);
  out_tree->Branch("Visibility", &Visibility);
  out_tree->Branch("Time", &Time);
  
  int voXYZ[3] = {0, 0, 0};
  int voxelID = -1;
  int n = 0;
  int hits = 0;

  //write branches
  //loop over events
  for(i=0; i<n_evt; i++){
    in_tree->GetEntry(i);
  
    //calculate voxel ID based on primary position
    voXYZ[0] = int(floor((prim_pos[0]-xyz_min[0])/dim_vox[0]));
    voxelID = voXYZ[0]+voXYZ[1]*n_vox[0]+voXYZ[2]*n_vox[0]*n_vox[1];

    //fill tree and reset values
    if(Voxel!=voxelID){
      if(voxelID<0){
        Voxel = voxelID;
        std::cout << "processing voxel no. " << Voxel << " ..." << std::endl;
      }else{
        Visibility = (float)totalhits/(float)(n*n_ph);
        out_tree->Fill();

        //reset values
        Voxel = voxelID;
        std::cout << "processing voxel no. " << Voxel << " ..." << std::endl;
        n = 0;
        hits = 0;
      }
    }

    n += 1;
    hits += totalhits;
  }

  //create vectors
  TVectorT<double> Min = TVectorT<double>(3);
  TVectorT<double> Max = TVectorT<double>(3);
  TVectorT<double> NDivisions= TVectorT<double>(3);

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
