//-------------------------------------------------------------------------------------------
//
//	GParticle.cxx
//
//	Stores final state particle information from PYTHIA events
//
//	Author : George Moschelli gmoschelli@gmail.com
//
//-------------------------------------------------------------------------------------------

#include "TMath.h"
#include "TDirectory.h"
#include "TProcessID.h"

#include "GParticle.h"

ClassImp(GParticle)

//-------------------------------------------------------------------------------------------
//	Default constructer
//-------------------------------------------------------------------------------------------
GParticle::GParticle()
{
	Reset();
}

//-------------------------------------------------------------------------------------------
//	Copy constructer
//-------------------------------------------------------------------------------------------
GParticle::GParticle(GParticle *p)
{
	CopyParticle(p);
}


//-------------------------------------------------------------------------------------------
//	Copy protected members
//-------------------------------------------------------------------------------------------
void GParticle::CopyParticle(GParticle *p)
{	
	Pid_PDG		= p->GetPID_PDG();
	status		= p->GetStatus();
	E		= p->GetE();
	Px		= p->GetPx();
	Py		= p->GetPy();
	Pz		= p->GetPz();
	Ptot		= p->GetPtot();
	Pt		= p->GetPt();
	Eta		= p->GetEta();
	Rap		= p->GetRap();
	Phi		= p->GetPhi();
	Mass		= p->GetMass();
	I3		= p->GetI3();
	Charge		= p->GetCharge();
	JetNumber	= p->GetJetNumber();
}

//------------------------------------------------------------------------------------------
//	Initialize Everything
//------------------------------------------------------------------------------------------
void GParticle::Reset()
{
	Pid_PDG		= 0;
	status		= 0;
	E		= 0.0;
	Px		= 0.0;
	Py		= 0.0;
	Pz		= 0.0;
	Ptot		= 0.0;
	Pt		= 0.0;
	Eta		= 0.0;
	Rap		= 0.0;
	Phi		= 0.0;
	Mass		= 0.0;
	I3		= 0;
	Charge		= 0;
	JetNumber	= 0;
}

//-------------------------------------------------------------------------------------------

void GParticle::Clear(Option_t * /*option*/)
{
	Reset();
}

//-------------------------------------------------------------------------------------------
//	azimuthal momentum angle
//-------------------------------------------------------------------------------------------
float GParticle::CalcPhi()
{
	float phi = 0;
	
	if(GetPx() != 0) phi = TMath::ATan(TMath::Abs(GetPy()/GetPx()));
	
	if(GetPx() > 0 && GetPy() >= 0)
	{
		return phi;
	}
	else if(GetPx() < 0 && GetPy() >= 0)
	{
		return TMath::Pi() - phi;
	}
	else if(GetPx() < 0 && GetPy() <  0)
	{
		return TMath::Pi() + phi;
	}
	else if(GetPx() >= 0 && GetPy() <  0)
	{
		return 2*TMath::Pi() - phi;
	}
	return phi;
}

//-------------------------------------------------------------------------------------------
//	pseudorapidity
//-------------------------------------------------------------------------------------------
float GParticle::CalcEta()
{
	float eta   = 0;
	float theta = 0;
	
	if(GetPz() >=0) theta = TMath::ASin(GetPt()/GetPtot());
	else theta = TMath::Pi() - TMath::ASin(GetPt()/GetPtot());
	
	eta = -TMath::Log(TMath::Tan((0.5*(theta))));	
	
	return eta;
}

//-------------------------------------------------------------------------------------------
//	rapidity
//-------------------------------------------------------------------------------------------
float GParticle::CalcRap()
{
//	float c = 299792458.0
	return 0.5*TMath::Log((GetE() + GetPz())/(GetE() - GetPz()));
}

//---------------------------------------------
/*
Int_t GParticle::BaryonNumber() {
	// all baryons have PIDs between 1 and 55,
	// the antibaryons between -55 and -1
	if (TMath::Abs(this->GetPID_UrQMD()) >=  1 &&
		TMath::Abs(this->GetPID_UrQMD()) <= 55) {
		return TMath::Sign(1,this->GetPID_UrQMD());
	}
	// everything else are mesons or leptons, i.e. baryon number 0
	return 0;
}

Int_t GParticle::StrangenessNumber() {
	// single strange baryons, e.g. Lambda
	if (TMath::Abs(this->GetPID_UrQMD()) >=  27 &&
	    TMath::Abs(this->GetPID_UrQMD()) <= 48) {
		return -1*TMath::Sign(1,this->GetPID_UrQMD());
	}
	// single strange mesons, e.g. Kaons
	if (TMath::Abs(this->GetPID_UrQMD()) == 106 ||
		TMath::Abs(this->GetPID_UrQMD()) == 108 ||
		TMath::Abs(this->GetPID_UrQMD()) == 110 ||
		TMath::Abs(this->GetPID_UrQMD()) == 113 ||
		TMath::Abs(this->GetPID_UrQMD()) == 121 ||
		TMath::Abs(this->GetPID_UrQMD()) == 117 ||
		TMath::Abs(this->GetPID_UrQMD()) == 125 ||
		TMath::Abs(this->GetPID_UrQMD()) == 129) {
		return 1*TMath::Sign(1,this->GetPID_UrQMD());
	}
	// double strange baryons, e.g. Xi
	if (TMath::Abs(this->GetPID_UrQMD()) >=  49 &&
		TMath::Abs(this->GetPID_UrQMD()) <= 54) {
		return -2*TMath::Sign(1,this->GetPID_UrQMD());
	}
	// triple strange baryons, e.g. Omega
	if (TMath::Abs(this->GetPID_UrQMD()) ==  55) {
		return -3*TMath::Sign(1,this->GetPID_UrQMD());
	}
	// not a strange particle
	return 0;
}
*/

