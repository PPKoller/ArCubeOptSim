#include "EventData.hh"


EventDataOptPh::EventDataOptPh()
{
	m_iEventId = -1;
	m_iNbTotHits = -1;
	
	//m_iNbSteps = -1;
	
	fPx = new vector<double>;
	fPy = new vector<double>;
	fPz = new vector<double>;
	
	m_pTrackId = new vector<int>;
	
	//m_pParentId = new vector<int>;
	//m_pParentType = new vector<string>;
	
	//m_pCreatorProcess = new vector<string>;
	m_pDepositingProcess = new vector<string>;
	m_pPhysVolName = new vector<string>;
	
	m_pTime = new vector<double>;
	fX = new vector<double>;
	fY = new vector<double>;
	fZ = new vector<double>;
	
	//fHitVol = new vector<EventDataOptPh::LdID>;
	
	//m_pEnergyDeposited = new vector<double>;
	//m_pKineticEnergy = new vector<double>;
	
	//m_pPrimaryParticleType = new vector<string>;
	
	m_fPrimary_momX = new vector<double>;
	m_fPrimary_momY = new vector<double>;
	m_fPrimary_momZ = new vector<double>;
	
	m_fPrimary_polX = new vector<double>;
	m_fPrimary_polY = new vector<double>;
	m_fPrimary_polZ = new vector<double>;
	
	m_pPrimaryVolume = new vector<string>;
}


EventDataOptPh::~EventDataOptPh()
{
	delete m_pTrackId;
	
	delete fPx;
	delete fPy;
	delete fPz;
	
	//delete fHitVol;
	
	//delete m_pParentId;
	//delete m_pParentType;
	
	//delete m_pCreatorProcess;
	delete m_pDepositingProcess;
	delete m_pPhysVolName;
	
	delete m_pTime;
	delete fX;
	delete fY;
	delete fZ;
	
	delete m_fPrimary_momX;
	delete m_fPrimary_momY;
	delete m_fPrimary_momZ;
	
	delete m_fPrimary_polX;
	delete m_fPrimary_polY;
	delete m_fPrimary_polZ;
	
	delete m_pPrimaryVolume;
}


void EventDataOptPh::Reset()
{
	m_iEventId = 0;
	m_iNbTotHits = 0;
	
	fPx->clear();
	fPy->clear();
	fPz->clear();
	
	//fHitVol->clear();
	
	m_pTrackId->clear();
	
	//m_pParentId->clear();
	//m_pParentType->clear();
	
	//m_pCreatorProcess->clear();
	m_pDepositingProcess->clear();
	m_pPhysVolName->clear();
	
	m_pTime->clear();
	
	fX->clear();
	fY->clear();
	fZ->clear();
	
	m_fPrimary_momX->clear();
	m_fPrimary_momY->clear();
	m_fPrimary_momZ->clear();
	
	m_fPrimary_polX->clear();
	m_fPrimary_polY->clear();
	m_fPrimary_polZ->clear();
	
	m_pPrimaryVolume->clear();
}

