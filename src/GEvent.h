//-------------------------------------------------------------------------------------------
//
//	Event.h
//
//	Stores PYTHIA events as lists of final state particles
//
//	Author : George Moschelli gmoschelli@gmail.com
//
//-------------------------------------------------------------------------------------------

#ifndef GEvent_H
#define GEvent_H

#include "TObject.h"
#include "TClonesArray.h"
#include "GParticle.h"

class GEvent : public TObject
{
private:
	int		NA;		// atomic numver A
	int		NB;		// atomic numver B
	int		EventNum;	//the event number
	
	int		Mult;		//Multiplicity of all particles ever created in event (including parents and daughters)
	int		Ntot;		//total final state particles
	int		Nhard;		//number of hard scatterings
	int		Nch;		//charge particle multiplicity
	int		Nplus;		//positively charged particle multiplicity
	int		Nminus;		//negatively charge particle multiplicity
	
	int		nJets;		// the number of jets (not dijets) in the event
	int		nAcceptedJets;	// the number of accepted (passes kinematic cuts) jets from analysis
	
	double		E_tot;		// total energy of particles
	double		Ech_tot;	// total energy of charged particles
	double		ET_tot;		// total transverse energy of particles
	double		ETch_tot;	// total transverse energy of charged particles
	
	double		PT_tot;		// total transverse momentum
	double		PTch_tot;	// total transverse momentum of charged particles
	
	float		RootS;		//Center-of-Mass Energy (GeV)
	float		b;		//Impact Parameter in fm
	
	unsigned int	Flag;		//accepted event = 1 , rejected = 0
	TClonesArray	*Particles;	//array with all final tracks
	TClonesArray	*HardPartons;	//save the hardest partons at generation level

	virtual void	CopyEvent(GEvent *e);
	
public:
	GEvent();
	GEvent(GEvent *e); //copy constructor
	virtual ~GEvent();

	void		Reset();
	void		Clear(Option_t *option ="");

	void		SetNA(int n)			{ NA			= n;	}
	void		SetNB(int n)			{ NB			= n;	}
	void		SetEventNum(int n)		{ EventNum		= n;	}
	void		SetMult(int n)			{ Mult			= n;	}
	void		SetNtot(int n)			{ Ntot			= n;	}
	void		SetNhard(int n)			{ Nhard			= n;	}
	void		SetNch(int n)			{ Nch			= n;	}
	void		SetNplus(int n)			{ Nplus			= n;	}
	void		SetNminus(int n)		{ Nminus		= n;	}
	void		SetE_tot(double x)		{ E_tot			= x;	}
	void		SetEch_tot(double x)		{ Ech_tot		= x;	}
	void		SetET_tot(double x)		{ ET_tot		= x;	}
	void		SetETch_tot(double x)		{ ETch_tot		= x;	}
	void		SetPT_tot(double x)		{ PT_tot		= x;	}
	void		SetPTch_tot(double x)		{ PTch_tot		= x;	}
	void		SetRootS(float x)		{ RootS			= x;	}
	void		Setb(float x)			{ b			= x;	}
	void		SetFlag(unsigned int un)	{ Flag			= un;	}
	void		SetNumJets(int n)		{ nJets			= n;	}
	void		SetHardPartons(TClonesArray *hp){ HardPartons		= hp;	}
	void		SetNAcceptedJets(int n)	{ nAcceptedJets = n; }

	GParticle	*AddParticle(GParticle *p);
	GParticle	*AddHardParton(GParticle *p);

	int		GetNA()				{ return NA;			}
	int		GetNB()				{ return NB;			}
	int		GetEventNum()			{ return EventNum;		}
	int		GetMult()			{ return Mult;			}
	int		GetNtot()			{ return Ntot;			}
	int		GetNhard()			{ return Nhard;			}
	int		GetNch()			{ return Nch;			}
	int		GetNplus()			{ return Nplus;			}
	int		GetNminus()			{ return Nminus	;		}
	int		GetNAcceptedJets()	{ return nAcceptedJets; }
	double		GetE_tot()			{ return E_tot;			}
	double		GetEch_tot()			{ return Ech_tot;		}
	double		GetET_tot()			{ return ET_tot;		}
	double		GetETch_tot()			{ return ETch_tot;		}
	double		GetPT_tot()			{ return PT_tot;		}
	double		GetPTch_tot()			{ return PTch_tot;		}
	float		GetRootS()			{ return RootS;			}
	float		Getb()				{ return b;			}
	unsigned int	GetFlag()		const	{ return Flag;			}
	TClonesArray	*GetParticles()		const	{ return Particles;		}
	TClonesArray	*GetHardPartons()	const	{ return HardPartons;		}
	
	GParticle	*GetParticle(int n)		{ return (GParticle*)Particles->At(n);		}
	GParticle	*GetHardParton(int n)		{ return (GParticle*)HardPartons->At(n);	}
	
	int		GetNumJets()			{ return nJets;			}
	
	double		Get_pt_avg()			{ return PT_tot/Mult;		}
	double		Get_pt_ch_avg()			{ return PTch_tot/Nch;		}

	ClassDef(GEvent,2)
};
#endif
