//-------------------------------------------------------------------------------------------
//
//	Results.cxx
//
//	Results from analysis of PYTHIA events with jets found by FastJet
//
//	Author : George Moschelli gmoschelli@gmail.com
//
//-------------------------------------------------------------------------------------------


#include "Results.h"

ClassImp(Results)

//-------------------------------------------------------------------------------------------
//	Default constructer
//-------------------------------------------------------------------------------------------

Results::Results()
{
	name = "results_";
	Reset();
	// default values for histogram parameters
	nbins = 10;
	Nmax = 100;
	SetHistograms();
}


//-------------------------------------------------------------------------------------------
//	constructer with unique name
//-------------------------------------------------------------------------------------------
Results::Results(TString str, int nbin, int N_max)
{
	name = str;
	Reset();
	// default values for histogram parameters
	nbins = nbin;
	Nmax = N_max;
	//SetHistograms();
}

//-------------------------------------------------------------------------------------------
//	Copy constructer
//-------------------------------------------------------------------------------------------
Results::Results(Results *r)
{
	CopyResults(r);
}

//-------------------------------------------------------------------------------------------
//	Add two results together
//-------------------------------------------------------------------------------------------
void Results::Add(Results *r)
{
	N_eve = N_eve + r->N_eve;
	N_tot = N_tot + r->N_tot;
	Nsq_tot = Nsq_tot + r->Nsq_tot;
	Pt_tot = Pt_tot + r->Pt_tot;
	PtN_tot = PtN_tot + r->PtN_tot;

	//Take this Results and add the histograms/values with another
	/*
	N_eve_hist->Sumw2(); 
	N_eve_hist->Add(r->N_eve_hist);
	

	N_avg_hist->Sumw2(); 
	N_avg_hist->Add(r->N_avg_hist);
	

	Nsq_avg_hist->Sumw2(); 
	Nsq_avg_hist->Add(r->Nsq_avg_hist);
	
	//VarN_hist->Add(r->VarN_hist);

	Pt_tot_avg_hist->Sumw2(); 
	Pt_tot_avg_hist->Add(r->Pt_tot_avg_hist);
	

	//PtAvg_NAvg_hist->Add(r->PtAvg_NAvg_hist);
	//pt_avg_hist->Add(r->pt_avg_hist);

	PtN_avg_hist->Sumw2(); 
	PtN_avg_hist->Add(r->PtN_avg_hist);
	

	//CovPtN_hist->Add(r->CovPtN_hist);
	//ptAvgVarN_hist->Add(r->ptAvgVarN_hist);

	//D_hist->Add(r->D_hist);

	//R_hist->Add(r->R_hist);
	*/
}

//-------------------------------------------------------------------------------------------
//	Copy protected members
//-------------------------------------------------------------------------------------------
void Results::CopyResults(Results *r)
{
	name		= r->Get_Name();
	
	SqrtS		= r->Get_SqrtS();
	NumJets		= r->Get_NumJets();
	AcceptedNumJets		= r->Get_AcceptedNumJets();
	numJetClasses  = r->Get_NumJetClasses();
	
	nbins		= r->Get_nbins();
	Nmax		= r->Get_NhistMax();
	
	N_eve		= r->Get_Nevents();
	N_tot		= r->Get_N_tot();
	Nsq_tot		= r->Get_Nsq_tot();
	
	Pt_tot		= r->Get_Pt_tot();
	PtN_tot		= r->Get_PtN_tot();
	
	avg_N		= r->Get_AvgN();
	avg_Nsq		= r->Get_AvgNsq();
	var_N		= r->Get_VarN();
	
	avg_Pt_tot	= r->Get_AvgTotPt();
	avg_pt		= r->Get_AvgPt();
	
	avg_PtN		= r->Get_AvgPtN();
	
	cov_PtN		= r->Get_CovPtN();
	
	D		= r->Get_D();
	R		= r->Get_R();
	
	VarN_2pass	= r->Get_VarN2P();
	CovPtN_2pass	= r->Get_CovPtN2P();
	
	D_2pass	= 	r->Get_D2P();
	R_2pass	= 	r->Get_R2P();
}

//------------------------------------------------------------------------------------------
//	Initialize Everything
//------------------------------------------------------------------------------------------
void Results::Reset()
{
	SqrtS = 0.0;		// center of mass collision energy
	
	nbins = 10;
	Nmax = 100;
	
	NumJets = -1; 		// flag indicating the number of jets in this event class
	AcceptedNumJets = -1; 		// flag indicating the number of jets in this event class
	// -1 = no jet selection
	numJetClasses = 0; 		// flag indicating the number of jets in this event class
	
	N_eve = 0;		// accepted number of events in this calculation
	N_tot = 0;		// total number of particles for all accepted events
	Nsq_tot = 0;		// sum over events of squares of particles per event
	
	Pt_tot = 0.0;		// total transverse momentum of all particle for all accepted events
	PtN_tot = 0.0;		// sum over events of Pt_event*N_event
	
	avg_N = 0.0;		// <N>
	avg_Nsq = 0.0;		// <N^2>
	var_N = 0.0;		// <N^2> - <N>^2
	
	avg_Pt_tot = 0.0;	// <PT> = <total pt>
	avg_pt = 0.0;		// <pt> = <PT>/<N>
	
	avg_PtN = 0.0;		// <PtN> = Sum_eve (Pt_event*N_event) / N_eve
	
	cov_PtN = 0.0;		// <PtN> - <Pt><N>
	
	D = 0.0;		// ( cov_PTN - <pt>*var_N )/<N>^2
	R = 0.0;		// ( Var(N) - <N> )/<N>^2
	
	VarN_2pass = 0.0;	// < (N - <N>)^2 >
	CovPtN_2pass = 0.0;	// < (Pt - <Pt>)(N - <N>) >
	
	D_2pass = 0.0;
	R_2pass = 0.0;
}


//------------------------------------------------------------------------------------------
//	Update private members for an event with N paricles with total transverse momentum PT
//
//	int pass updates variable based on which cycle though the data
//		 Calculate(int pass = 0) should have been called before Update(pass = 2, N, PT)
//------------------------------------------------------------------------------------------
void Results::Update(int pass, int N, double PT)
{
	if(pass == 0){// first pass through events
		if(N > 0) // there must be particles to count
		{
			N_eve++;
			N_tot += N;
			Nsq_tot += N*N;
			if(Nsq_tot < 0) exit (EXIT_FAILURE);
			Pt_tot += PT;
			PtN_tot += PT*N;
			
			// histograms
			/*
			N_eve_hist->Fill(double(N));
			N_avg_hist->Fill(double(N), double(N));
			Nsq_avg_hist->Fill(double(N), double(N)*double(N));
			
			Pt_tot_avg_hist->Fill(double(N), PT);
			PtN_avg_hist->Fill(double(N), PT*double(N));
			*/

			//cout << "N: " << N << endl;
			//cout << "PT: " << PT << endl;
			//cout << "Ntot: " << N_tot << endl;
			//cout << "PTtot: " << Pt_tot << endl;
		}
	}// end first pass condidtion
	else if(pass == 1){// second pass through events
		double deltaN = double(N) - avg_N; // N - <N>  // N = number of accepted particles in one evnet
		double deltaPt = PT - avg_Pt_tot;// Pt - <Pt>  // Pt = total transverse momentum in one evnet
		
		if(N > 0) // there must be particles to count
		{
			VarN_2pass += deltaN*deltaN/N_eve;
			CovPtN_2pass += deltaPt*deltaN/N_eve;
		}
	}// end second pass though events
	else{ 
		cout << "*** ERROR *** in Results::Update() with number of passes though events" << endl;
		exit(EXIT_FAILURE);
	}
}

//------------------------------------------------------------------------------------------
//	Calculate avreage quantites and histograms
//
//	int pass updates variable based on which cycle though the data
//		 Calculate(int pass = 0) should have been called before Calculate(pass = 2)
//------------------------------------------------------------------------------------------
void Results::Calculate(int pass)
{
	if(pass == 0){// first pass through events
		if(N_eve > 0) // there must be events
		{
			avg_N = double(N_tot)/N_eve;
			avg_Nsq = double(Nsq_tot)/N_eve;
			var_N = avg_Nsq - (avg_N*avg_N);
			
			avg_Pt_tot = Pt_tot/N_eve;
			avg_pt = avg_Pt_tot/avg_N;
			
			avg_PtN = PtN_tot/N_eve;
			cov_PtN = avg_PtN - (avg_Pt_tot*avg_N);
			
			D = (cov_PtN - (avg_pt*var_N))/(avg_N*avg_N);
			R = (var_N - avg_N)/(avg_N*avg_N);
			
			// histograms
			
			/*
			N_avg_hist->Sumw2(); 
			N_avg_hist->Divide(N_eve_hist);
			
			N_avg_sq_hist->Sumw2(); 
			N_avg_sq_hist->Multiply(N_avg_hist, N_avg_hist, 1.0, 1.0);
			
			
			Nsq_avg_hist->Sumw2(); 
			Nsq_avg_hist->Divide(N_eve_hist);
			
			VarN_hist->Sumw2(); 
			VarN_hist->Add(Nsq_avg_hist, N_avg_sq_hist, 1.0, -1.0);
			
			
			Pt_tot_avg_hist->Sumw2(); 
			Pt_tot_avg_hist->Divide(N_eve_hist);
			
			pt_avg_hist->Sumw2(); 
			pt_avg_hist->Divide(Pt_tot_avg_hist, N_avg_hist, 1.0, 1.0);
			
			
			PtN_avg_hist->Sumw2(); 
			PtN_avg_hist->Divide(N_eve_hist);
			
			
			PtAvg_NAvg_hist->Sumw2(); 
			PtAvg_NAvg_hist->Multiply(Pt_tot_avg_hist, N_avg_hist, 1.0, 1.0);
			
			
			CovPtN_hist->Sumw2(); 
			CovPtN_hist->Add(PtN_avg_hist, PtAvg_NAvg_hist, 1.0, -1.0);
			
			ptAvgVarN_hist->Sumw2(); 
			ptAvgVarN_hist->Multiply(pt_avg_hist, VarN_hist, 1.0, 1.0);
			
			
			
			D_hist->Sumw2(); 
			D_hist->Add(CovPtN_hist, ptAvgVarN_hist, 1.0, -1.0);
			
			D_hist->Sumw2(); 
			D_hist->Divide(N_avg_sq_hist);
			
			
			R_hist->Sumw2(); 
			R_hist->Add(VarN_hist, N_avg_hist, 1.0, -1.0);
			
			R_hist->Sumw2();
			R_hist->Divide(N_avg_sq_hist);

			VarN_hist->Divide(N_avg_sq_hist);
			CovPtN_hist->Divide(N_avg_sq_hist);
			*/
		}
	}// end first pass through events condidtion
	else if(pass == 1){// second pass through events
	  	if(N_eve > 0) // there must be events
		{
//			VarN_2pass = VarN_2pass/N_eve;
//			CovPtN_2pass = CovPtN_2pass/N_eve;
			
			D_2pass = (CovPtN_2pass - (avg_pt*VarN_2pass))/(avg_N*avg_N);
			R_2pass = (VarN_2pass - avg_N)/(avg_N*avg_N);
		}
	}// end second pass through events condition
	else{ 
		cout << "*** ERROR *** in Results::Calculate() with number of passes though events" << endl;
		exit(EXIT_FAILURE);
	}
}



//------------------------------------------------------------------------------------------
//	Initialize histograms
//------------------------------------------------------------------------------------------
void Results::SetHistograms()
{
	TString title;
	
	/*
	title = name + "N_eve_hist";
	N_eve_hist = new TH1D(title,title, nbins, 0.0, Nmax);
	
	title = name + "N_avg_hist";
	N_avg_hist = new TH1D(title,title, nbins, 0.0, Nmax);
	
	title = name + "N_avg_sq_hist";
	N_avg_sq_hist = new TH1D(title,title, nbins, 0.0, Nmax);
	
	title = name + "Nsq_avg_hist";
	Nsq_avg_hist = new TH1D(title,title, nbins, 0.0, Nmax);
	
	title = name + "VarN_hist";
	VarN_hist = new TH1D(title,title, nbins, 0.0, Nmax);
	
	title = name + "Pt_tot_avg_hist";
	Pt_tot_avg_hist = new TH1D(title,title, nbins, 0.0, Nmax);
	
	title = name + "pt_avg_hist";
	pt_avg_hist = new TH1D(title,title, nbins, 0.0, Nmax);
	
	title = name + "PtN_avg_hist";
	PtN_avg_hist = new TH1D(title,title, nbins, 0.0, Nmax);
	
	title = name + "PtAvg_NAvg_hist";
	PtAvg_NAvg_hist = new TH1D(title,title, nbins, 0.0, Nmax);
	
	title = name + "CovPtN_hist";
	CovPtN_hist = new TH1D(title,title, nbins, 0.0, Nmax);
	
	title = name + "ptAvgVarN_hist";
	ptAvgVarN_hist = new TH1D(title,title, nbins, 0.0, Nmax);
	
	title = name + "D_hist";
	D_hist = new TH1D(title,title, nbins, 0.0, Nmax);
	
	title = name + "R_hist";
	R_hist = new TH1D(title,title, nbins, 0.0, Nmax);
	*/
}

//-------------------------------------------------------------------------------------------
//	Destructor
//-------------------------------------------------------------------------------------------
Results::~Results()
{
	// delete pointers
	/*
	delete N_eve_hist;
	delete N_avg_hist;
	delete N_avg_sq_hist;
	delete Nsq_avg_hist;
	delete VarN_hist;
	delete Pt_tot_avg_hist;
	delete pt_avg_hist;
	delete PtN_avg_hist;
	delete PtAvg_NAvg_hist;
	delete CovPtN_hist;
	delete ptAvgVarN_hist;
	delete D_hist;
	delete R_hist;
	*/
}
