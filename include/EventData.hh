#ifndef __EVENT_DATA_OPT_PH_HH__
#define __EVENT_DATA_OPT_PH_HH__

#include <string>
#include <vector>

using std::string;
using std::vector;


class EventDataOptPh
{
public:
	
	/*
	enum LdID{ //Light detector ID
		kTop = 1,
		kOther = 0,
		kBot = -1
	};
	*/
	
	EventDataOptPh();
	virtual ~EventDataOptPh();
	
	void Reset();
	
	
	
public:
	int m_iEventId;								// the event ID
	int m_iNbTotHits;						// number of all the hits for the event
	
	vector<double> *fPx, *fPy, *fPz; //I need this to determine the angle hitting the photocatode when the absorption takes place
	
	//vector<EventDataOptPh::LdID> *fHitVol; //This is to distinguish the PMT of the hit without using the name
	
	vector<int> *m_pTrackId;					// id of the particle
	
	//vector<int> *m_pParentId;					// id of the parent particle
	//vector<string> *m_pParentType;				// type of particle
	
	//vector<string> *m_pCreatorProcess;			// interaction
	vector<string> *m_pDepositingProcess;		// energy depositing process
	vector<string> *m_pPhysVolName;				// Name of the physical volume of the step (not the primary)
	
	vector<double> *m_pTime;					// time of the step
	vector<double> *fX, *fY, *fZ;						// position of the step
	//vector<double> *m_pEnergyDeposited; 		// energy deposited in the step
	//vector<double> *m_pKineticEnergy;			// particle kinetic energy after the step
	
	double m_fPrimary_posX;						// position of the primary particle
	double m_fPrimary_posY;
	double m_fPrimary_posZ;
	
	vector<double> *m_fPrimary_momX;			// Momentum of the primary
	vector<double> *m_fPrimary_momY;
	vector<double> *m_fPrimary_momZ;
	
	vector<double> *m_fPrimary_polX;			// Polarization of the primary particle
	vector<double> *m_fPrimary_polY;
	vector<double> *m_fPrimary_polZ;
	
	vector<string> *m_pPrimaryVolume;
};

#endif

