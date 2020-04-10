
using namespace std;

void plotdata()
{
	string filename="tpb_absorption_length.dat";
	
	
	ifstream infile(filename.c_str());
	
	if(!infile) return;
	
	
	stringstream line;
	string str;
	
	TGraph *gr = new TGraph;
	
	Int_t iPoint = 0;
	while( getline(infile,str) ){
		
		line.clear(); line.str("");
		
		line << str;
		
		line >> str;
		
		Double_t wl = 1e9*TMath::H()*TMath::C()/TMath::Qe()/std::stod(str);
		
		if(!line) continue;
		
		line >> str;
		
		Double_t absl = 1e6*std::stod(str);
		
		
		gr->SetPoint(iPoint, wl, absl);
		
		
		iPoint++;
	}
	
	
	TCanvas *c1 = new TCanvas("c1");
	gr->Draw("APL");
	
}