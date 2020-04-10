#include "EventData.hh"


EventDataOptPh::EventDataOptPh()
{
	fEventId = -1;
	//fPrimaryVolume = new vector<int>;
	
	fPrimEn = new vector<double>;
	
	fPrimaryVolumeIndex=-1;
	fPrimaryVolumeCopyNum=-1;
	
	fPrimary_Xmom = new vector<Double_t>;
	fPrimary_Ymom = new vector<Double_t>;
	fPrimary_Zmom = new vector<Double_t>;
	
	fPrimary_Xpol = new vector<Double_t>;
	fPrimary_Ypol = new vector<Double_t>;
	fPrimary_Zpol = new vector<Double_t>;
	
	
	fNbTotHits = -1;
	
	fVolIndex = new vector<Long64_t>;
	fHitVolId = new vector<Long64_t>;
	fTime = new vector<Double_t>;
	
	fTrackId = new vector<Long64_t>;
	fPartGener = new vector<Long64_t>;
	
	fParentId = new vector<Long64_t>;
	
	fXpos = new vector<Double_t>;
	fYpos = new vector<Double_t>;
	fZpos = new vector<Double_t>;
	
	fXmom = new vector<Double_t>;
	fYmom = new vector<Double_t>;
	fZmom = new vector<Double_t>;
	
	fXpol = new vector<Double_t>;
	fYpol = new vector<Double_t>;
	fZpol = new vector<Double_t>;
	
	fCreatProc = new vector<Int_t>;
	fFirstParentId = new vector<Long64_t>;
	fDepProc = new vector<Int_t>;
}


EventDataOptPh::~EventDataOptPh()
{
	//delete fPrimaryVolume;
	
	delete fPrimEn;
	
	delete fPrimary_Xmom;
	delete fPrimary_Ymom;
	delete fPrimary_Zmom;
	
	delete fPrimary_Xpol;
	delete fPrimary_Ypol;
	delete fPrimary_Zpol;
	
	delete fVolIndex;
	delete fHitVolId;
	delete fTime;
	
	delete fTrackId;
	delete fPartGener;
	
	delete fParentId;
	
	delete fXpos;
	delete fYpos;
	delete fZpos;
	
	delete fXmom;
	delete fYmom;
	delete fZmom;
	
	delete fXpol;
	delete fYpol;
	delete fZpol;
	
	delete fCreatProc;
	delete fFirstParentId;
	delete fDepProc;
}


void EventDataOptPh::Reset(int primNum)
{
	fEventId = -1;
	
	fPrimEn->assign(primNum,0);
	
	fPrimaryVolumeIndex=-1;
	fPrimaryVolumeCopyNum=-1;
	
	fPrimary_Xmom->assign(primNum,0);
	fPrimary_Ymom->assign(primNum,0);
	fPrimary_Zmom->assign(primNum,0);
	
	fPrimary_Xpol->assign(primNum,0);
	fPrimary_Ypol->assign(primNum,0);
	fPrimary_Zpol->assign(primNum,0);
	
	fNbTotHits = 0;
	
	fVolIndex->resize(0);
	fHitVolId->resize(0);
	fTime->resize(0);
	
	fTrackId->resize(0);
	
	fPartGener->resize(0);
	fParentId->resize(0);
	
	fXpos->resize(0);
	fYpos->resize(0);
	fZpos->resize(0);
	
	fXmom->resize(0);
	fYmom->resize(0);
	fZmom->resize(0);
	
	fXpol->resize(0);
	fYpol->resize(0);
	fZpol->resize(0);
	
	fCreatProc->resize(0);
	fFirstParentId->resize(0);
	fDepProc->resize(0);
}

