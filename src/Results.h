//-------------------------------------------------------------------------------------------
//
//	Results.h
//
//	Results from analysis of PYTHIA events with jets found by FastJet
//
//	Author : George Moschelli gmoschelli@gmail.com
//
//-------------------------------------------------------------------------------------------

#ifndef Results_H
#define Results_H

#include "TObject.h"
#include "TMath.h"
#include "TH1.h"
#include "TH1D.h"
#include <iostream>
using namespace std;

class Results : public TObject
{
private:
	TString name;	// name that will be appended to histograms
	
	int	nbins;		// number of bins for histograms
	int	Nmax;		// max number of accepted particles for histogram range
	int	NumJets;	// flag indicating the number of jets in this event class
				// -1 = no jet selection
	double	SqrtS;		// [GeV] center of mass collision energy

	virtual void	CopyResults(Results *r);
	
public:
	//Move these into public to make addition of Results easier, not dealing with protected shit
	int	N_eve;		// accepted number of events in this calculation
	int	N_tot;		// total number of particles for all accepted events

	long long int	Nsq_tot;	// sum over events of squares of particles per event



	double	Pt_tot;		// total transverse momentum of all particle for all accepted events


	double	PtN_tot;	// sum over events of Pt_event*N_event
	double	avg_N;		// <N>
	double	avg_Nsq;	// <N^2>
	double	var_N;		// <N^2> - <N>^2
	double	avg_Pt_tot;	// <PT> = <total pt>
	double	avg_pt;		// <pt> = <PT>/<N>
	double	avg_PtN;	// <PtN> = Sum_eve (Pt_event*N_event) / N_eve
	double	cov_PtN;	// <PtN> - <Pt><N>
	double	D;		// ( cov_PtN - <pt>*var_N )/<N>^2
	double	R;		// ( Var(N) - <N> )/<N>^2


	double	VarN_2pass;	// < (N - <N>)^2 >
	double	CovPtN_2pass;	// < (Pt - <Pt>)(N - <N>) >

	double	D_2pass;
	double	R_2pass;
	//END DISCRETE VARS

	Results();
	Results(TString str, int nbin, int N_max);
	Results(Results *r); //copy constructor
	virtual ~Results();

	void	Reset();
	void	SetHistograms();
	
	void	Update(int pass, int N, double PT); // updates information from an event
	void	Calculate(int pass); // calculates observables. Call only after event loop

	void	Add(Results *r);

	
	void	Set_Name(TString str)	{ name		= str;	}
	void	Set_nbins(int n)	{ nbins		= n;	}
	void	Set_NhistMax(int n)	{ Nmax		= n;	}
	void	Set_NumJets(int n)	{ NumJets	= n;	}
	void	Set_SqrtS(double x)	{ SqrtS		= x;	}
	
	
	TString Get_Name()		{ return name;		}
	int	Get_nbins()		{ return nbins;		}
	int	Get_NhistMax()		{ return Nmax;		}
	int	Get_NumJets()		{ return NumJets;	}
	int	Get_Nevents()		{ return N_eve;		}
	int	Get_N_tot()		{ return N_tot;		}
	int	Get_Nsq_tot()		{ return Nsq_tot;	}
	double	Get_Pt_tot()		{ return Pt_tot;	}
	double	Get_PtN_tot()		{ return PtN_tot;	}
	double	Get_SqrtS()		{ return SqrtS;		}
	double	Get_AvgN()		{ return avg_N;		}
	double	Get_AvgNsq()		{ return avg_Nsq;	}
	
	double	Get_VarN()		{ return var_N;		}
	double	Get_VarN2P()		{ return VarN_2pass;	}
	
	double	Get_AvgTotPt()		{ return avg_Pt_tot;	}
	double	Get_AvgPt()		{ return avg_pt;	}
	double	Get_AvgPtN()		{ return avg_PtN;	}
	
	double	Get_CovPtN()		{ return cov_PtN;	}
	double	Get_CovPtN2P()		{ return CovPtN_2pass;	}
	
	double	Get_D()			{ return D;		}
	double	Get_R()			{ return R;		}
	double	Get_D2P()		{ return D_2pass;	}
	double	Get_R2P()		{ return R_2pass;	}
	
	TH1D	*N_eve_hist;// Nev vs Nacc
	
	TH1D	*N_avg_hist; // <N> vs Nacc
	TH1D	*N_avg_sq_hist; // <N>^2 vs Nacc
	TH1D	*Nsq_avg_hist;// <N^2> vs Nacc
	TH1D	*VarN_hist;//  Var(N) vs Nacc
	
	TH1D	*Pt_tot_avg_hist; //  <PT> vs Nacc
	TH1D	*PtAvg_NAvg_hist;// <PT><N> vs Nacc
	TH1D	*pt_avg_hist;// <pt> = <PT>/<N>
	TH1D	*PtN_avg_hist;// <PtN> vs Nacc
	
	TH1D	*CovPtN_hist;// Cov(PtN) vs Nacc
	TH1D	*ptAvgVarN_hist;// <pt>Var(N) vs Nacc
	
	TH1D	*D_hist;// ( Cov(PtN) - <pt>Var(N) )/<N>^2
	
	TH1D	*R_hist;// (Var(N) - <N>)/<N>^2

	ClassDef(Results,2)
};
#endif
