//-------------------------------------------------------------------------------------------
//
//	GEvent.h
//
//	Stores PYTHIA events as lists of final state particles
//
//	Author : George Moschelli gmoschelli@gmail.com
//
//-------------------------------------------------------------------------------------------

#include "TMath.h"
#include "TDirectory.h"
#include "TProcessID.h"

#include "GEvent.h"

ClassImp(GEvent)

//-------------------------------------------------------------------------------------------
//	Default constructer
//-------------------------------------------------------------------------------------------
GEvent::GEvent()
{
	Particles	= new TClonesArray("GParticle", 100000);
	HardPartons	= new TClonesArray("GParticle", 1000);
	Reset();
}

//-------------------------------------------------------------------------------------------
//	Copy constructer
//-------------------------------------------------------------------------------------------
GEvent::GEvent(GEvent *e)
{
	CopyEvent(e);
}

//-------------------------------------------------------------------------------------------
//	Copy protected members
//-------------------------------------------------------------------------------------------
void GEvent::CopyEvent(GEvent *e)
{
	NA		= e->GetNA();
	NB		= e->GetNB();
	EventNum	= e->GetEventNum();
	
	Mult		= e->GetMult();
	Ntot		= e->GetNtot();
	Nhard		= e->GetNhard();
	Nch		= e->GetNch();
	Nplus		= e->GetNplus();
	Nminus		= e->GetNminus();
	
	E_tot		= e->GetE_tot();
	Ech_tot		= e->GetEch_tot();
	ET_tot		= e->GetET_tot();
	ETch_tot	= e->GetETch_tot();
	
	PT_tot		= e->GetPT_tot();
	PTch_tot	= e->GetPTch_tot();
	
	RootS		= e->GetRootS();
	b		= e->Getb();
	
	nJets		= e->GetNumJets();
	nAcceptedJets = e->GetNAcceptedJets();
	
	Flag		= e->GetFlag();
	Particles	= e->GetParticles();
	HardPartons	= e->GetHardPartons();
}

//------------------------------------------------------------------------------------------
//	Initialize Everything
//------------------------------------------------------------------------------------------
void GEvent::Reset()
{
	NA		= 0;
	NB		= 0;
	EventNum	= 0;
	
	Mult		= 0;
	Ntot		= 0;
	Nhard		= 0;
	Nch		= 0;
	Nplus		= 0;
	Nminus		= 0;
	
	E_tot		= 0.0;
	Ech_tot		= 0.0;
	ET_tot		= 0.0;
	ETch_tot	= 0.0;
	
	PT_tot		= 0.0;
	PTch_tot	= 0.0;
	
	RootS		= 0.0;
	b		= 0.0;
	Flag		= 0;
	
	nJets		  = 0;
	nAcceptedJets = 0;
}

//-------------------------------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------------------------------
GEvent::~GEvent()
{
	Clear();
}

//-------------------------------------------------------------------------------------------

GParticle *GEvent::AddParticle(GParticle *p)
{
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,32,0)
	GParticle *particle = (GParticle*)Particles->ConstructedAt(Ntot++);
	particle->CopyParticle(p);
#else
	TClonesArray &particles = *Particles;
	GParticle *particle = new(particles[Ntot++]) Particles(p);
#endif

	return particle;
}

//-------------------------------------------------------------------------------------------

GParticle *GEvent::AddHardParton(GParticle *p)
{
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,32,0)
	GParticle *particle = (GParticle*)HardPartons->ConstructedAt(Nhard++);
	particle->CopyParticle(p);
#else
	TClonesArray &particles = *HardPartons;
	GParticle *particle = new(particles[Nhard++]) HardPartons(p);
#endif

	return particle;
}

//-------------------------------------------------------------------------------------------

void GEvent::Clear(Option_t * /*option*/)
{
	Particles->Clear("C");
	HardPartons->Clear("C");
	Reset();
}

