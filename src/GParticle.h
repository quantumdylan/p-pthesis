//-------------------------------------------------------------------------------------------
//
//	Particle.h
//
//	Stores final state particle information from PYTHIA events
//
//	Author : George Moschelli gmoschelli@gmail.com
//
//-------------------------------------------------------------------------------------------

#ifndef GParticle_H
#define GParticle_H

#include "TObject.h"
#include "TClonesArray.h"
#include "TMath.h"

class GParticle : public TObject
{

 protected:
	int	Pid_PDG;	//particle-ID PDG
	int	status;		//particle status in evnent (final or initial)
	float	E;		//Energy
	float	Px;		//x momentum component in GeV
	float	Py;		//y momentum component in GeV
	float	Pz;		//z momentum component in GeV
	float	Ptot;		//total momentum in GeV
	float	Pt;		//transverse momentum in GeV
	float	Eta;		// pseudorapidity
	float	Rap;		// rapidity
	float	Phi;		// azimuthal momentum angle
	float	Mass;		//mass of particle in GeV
	float	I3;		//2*I3: isospin z-projection (doubled)
	float	Charge;		//charge of particle
	
	int	JetNumber;	// index indicating which jet this particle belongs to
				// if JetNumber == 0 this particle is not in a jet

				
 public:
	GParticle();
	GParticle(GParticle *p); //copy constructor
	virtual ~GParticle() {Clear();}
	
	virtual void	CopyParticle(GParticle *p);

	void	SetPID_PDG(int n)	{ Pid_PDG	= n; }
	void	SetStatus(int n)	{ status	= n; }
	void	SetE(float x)		{ E		= x; }
	void	SetPx(float x)		{ Px		= x; }
	void	SetPy(float x)		{ Py		= x; }
	void	SetPz(float x)		{ Pz		= x; }
	void	SetPtot(float x)	{ Ptot		= x; }
	void	SetPt(float x)		{ Pt		= x; }
	void	SetEta(float x)		{ Eta		= x; }
	void	SetRap(float x)		{ Rap		= x; }
	void	SetPhi(float x)		{ Phi		= x; }
	void	SetMass(float x)	{ Mass		= x; }
	void	SetI3(int n)		{ I3		= n; }
	void	SetCharge(int n)	{ Charge	= n; }
	void	SetJetNumber(int n)	{ JetNumber	= n; }
	
	void	Reset();
	void	Clear(Option_t *option="");

	int	GetPID_PDG()		{ return Pid_PDG;	}
	int	GetStatus()		{ return status;	}
	float	GetE()			{ return E;		}
	float	GetPx()			{ return Px;		}
	float	GetPy()			{ return Py;		}
	float	GetPz()			{ return Pz;		}
	float	GetEta()		{ return Eta;		}
	float	GetRap()		{ return Rap;		}
	float	GetPtot()		{ return Ptot;		}
	float	GetPt()			{ return Pt;		}
	float	GetPhi()		{ return Phi;		}
	float	GetMass()		{ return Mass;		}
	int	GetI3()			{ return I3;		}
	int	GetCharge()		{ return Charge;	}
	int	GetJetNumber()		{ return JetNumber;	}

	float	CalcPhi();	// azimuthal momentum angle
	float	CalcEta();	// pseudorapidity
	float	CalcRap();	// rapidity
//	int	BaryonNumber();	// Baryon number of the particle
//	int	StrangenessNumber();	// Strangeness number of the particle

	ClassDef(GParticle,2)
};
#endif
