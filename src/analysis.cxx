#include <string>
#include "stdio.h"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "TROOT.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TProfile.h"
#include "TF1.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TChain.h"
#include "TTree.h"
#include "TVector3.h"
#include "TMath.h"
#include "TLatex.h"
#include "TPad.h"
#include "TCutG.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TLine.h"
#include "TSystem.h"
#include "TDirectory.h"
#include "TPaletteAxis.h"
#include "TColor.h"
#include "TObjectTable.h"
#include "TKey.h"
#include "TRandom.h"
#include "TApplication.h"
#include "TStopwatch.h"

//George's Custom Classes
#include "GParticle.h"
#include "GEvent.h"
#include "Results.h"

using namespace std;


// -----------------------------------------------------------------------------

int main ( int argc, const char** argv ) 
{
	int nCores = atoi(argv[1]);// set the number of .root files to read
	TString path = argv[2]; //location to place .root file with pythia events
	TString filename = argv[3]; // the name of the .root file that stores final events
	int ipass_limiter = atoi(argv[7]);
	
	TChain* InputChain = new TChain("Events");
	if (argv[6] != NULL) {
		cout << "hi " << nCores << endl;
		cout << path + filename + TString(Form("%d", nCores)) + "_FoundJets.root" << endl;
		InputChain->Add(path + filename + TString(Form("%d", nCores)) + TString("_FoundJets.root"));
	}
	else {
		// add all event trees called "Events" from all files with name path/filename*_FoundJets.root 
		InputChain->Add(path + filename + TString("*_FoundJets.root"));// adds all event trees from all files 
	}
	
	
	// George's Custom Classes
	// these are stored in the tree "Events" and are needed to read it
	GEvent *event = new GEvent();
	GParticle *part1 = new GParticle(); // pointer to a particle object used to read particles from the event object
	
	// each event is a branch
	InputChain->GetBranch("event")->SetAutoDelete(kFALSE);
	InputChain->SetBranchAddress("event", &event);
	
	int NumEvents;
	if (argv[5] != NULL && atoi(argv[5]) <= InputChain->GetEntries()){
		cout << "Input overriding default for NumEvents!" << endl;
		NumEvents = atoi(argv[5]);
	}
	else{
		NumEvents = InputChain->GetEntries();// get the number of events
	}
	
	
	cout << "There are " << NumEvents << " events!" << endl;
	
	
	
	// kinematic cuts
	float roots; // [GeV] center of mass collision energy
	float eta_min = -25, eta_max = 25;// min and max pseudo rapidity range
	float rap_min = -100, rap_max = 100;// min and max pseudo rapidity range
	float pt_min = 0.5, pt_max = 900000.0;// [GeV] min and max transverse momentum range
	
	
	int nbins = 30;
	int Nmax = 100;

	int numJetClasses = 7;
	
	// results class objects
	Results *NoCuts_Res = new Results("NoCuts_", nbins, Nmax); // not cuts of any kind
	// used cuts specified in particle loop
	Results *Everything_Res = new Results("Everything_", nbins, Nmax); // no jet discrimination
	Results *JetResults[numJetClasses];// discriminates by jet content
	for(int i = 0; i < numJetClasses; i++){ // initialize jet results classes
		JetResults[i] = new Results("NumJets"+TString(Form("%d",i))+"_", nbins, Nmax);
		JetResults[i]->Set_NumJets(i); // set jet content label
	}
	
	Everything_Res->Set_NumJetClasses(numJetClasses);
	
	
	TStopwatch timer;
	timer.Start();
	
	
	//***********************************************************************
	//	MULTIPLE PASSES THROUGH EVENTS LOOP
	//***********************************************************************
	for (int ipass = 0; ipass < ipass_limiter; ipass++) { // make two passes through the event loop
		if (ipass == 0) cout << "*********************** FIRST PASS ***********************" << endl;
		if (ipass == 1) cout << "*********************** SECOND PASS ***********************" << endl;

		//***********************************************************************
		//	EVENT LOOP
		//***********************************************************************
		for (int iev = 0; iev < NumEvents; iev++)
		{
			int Nacc_event = 0, N_nocut_event = 0;
			double pt_event = 0.0, pt_nocut_event = 0.0;

			//read branch event
			InputChain->GetEntry(iev);// grabs event ev from the tree

			if (iev == 0) roots = event->GetRootS();

			GParticle *particle1; // pointer to a particle object used to read particles from the event object
			TObjArray *Particles; // partices are stored in the event object in an array TObjArray
			Particles = (TObjArray *)event->GetParticles();// fill the array from the event object
			int nentries = Particles->GetEntries();// get the number of particles in this event
			int jetnum = 0;// counter for analyzed jets

			//***********************************************************************
			//	PARTICLE LOOP
			//***********************************************************************
			for (int ip = 0; ip < nentries; ++ip)
			{
				particle1 = (GParticle *)Particles->At(ip);// get particle1

				float charge1 = particle1->GetCharge();
				double pt1 = particle1->GetPt(); // get transverse momentum
				double eta1 = particle1->GetEta();// get pseudorapidity
				double y1 = particle1->GetRap(); // get rapidity
				// if GetJetNumber() = 0 particle is not in a jet
				// for GetJetNumber() = positve int the number represents which jet the particle is in
				// int jetnum = particle1->GetJetNumber();
				bool leading = particle1->GetIsLeading();


				N_nocut_event++;
				pt_nocut_event += pt1;

				//
				//	conditions for accepted particles
				//
				if (charge1 != 0) { // choose only charged particles
					if (pt1 > pt_min && pt1 <= pt_max) { // make pt cut selection
						if (eta1 > eta_min && eta1 <= eta_max) { // make pseudo rapidity cut selection
							if (y1 > rap_min && y1 <= rap_max) { // make rapidity cut selection
								if (leading){
									jetnum++;
								}

								Nacc_event++;
								pt_event += pt1;

							}// end rapidity cut condition
						}// end pseudo rapidity cut condition
					}// end pt cut condition
				}// end charge condition

			}// END PARTICLE LOOP
			event->SetNAcceptedJets(jetnum); // set our rapidity-based jet number

			// update Event ensemble results
			NoCuts_Res->Update(ipass, N_nocut_event, pt_nocut_event); // always update
			Everything_Res->Update(ipass, Nacc_event, pt_event); // always update
			// only update if jet content condition is met
			for (int i = 0; i < numJetClasses; i++)
				if (jetnum == i) JetResults[i]->Update(ipass, Nacc_event, pt_event);



			//		  if(NoCuts_Res->Get_Nsq_tot() < 0) out << "Event " << iev << "Nnocut " << N_nocut_event << "NsqTot " << NoCuts_Res->Get_Nsq_tot() << endl;


					// delete pointers	
			//		delete particle1;
			//		delete Particles;

			event->Clear();
			if (iev % 100000 == 0) cout << iev << " events analysed." << endl;

		}// END EVENT LOOP
		cout << NumEvents << " events analysed." << endl;
	}


	// probably should set sqrt(s) somewhere
	NoCuts_Res->Set_SqrtS(roots);
	Everything_Res->Set_SqrtS(roots);
	for (int i = 0; i < numJetClasses; i++) JetResults[i]->Set_SqrtS(roots);

	if (NoCuts_Res == NULL || Everything_Res == NULL) cout << "We are well and truly fucked" << endl;



	//OUTPUT BLOCK
	TString output;
	if (argv[4] != NULL) {
		output = argv[4];
		cout << endl << "External input " << output << " received for output path! Overriding default..." << endl;
	}
	else {
		output = path;
		cout << endl << "No output path specified, setting to input path..." << endl;
	}

	cout << endl << "Output path set as " << output << ". Will write contents to this directory!" << endl;

	TFile *outRoot = new TFile(output + filename + TString(Form("%d", nCores)) + TString("_ANALYZED.root"), "RECREATE");


	NoCuts_Res->Write("NoCuts");
	Everything_Res->Write("Everything");
	for (int i = 0; i < numJetClasses; i++) {
		JetResults[i]->Write("NumJets" + TString(Form("%d", i)));
	}
	
	
	outRoot->Write();
	outRoot->Close();
	
	
	
	
	
	
	
	timer.Stop();
	cout << endl << "Run Time: " << endl;
	timer.Print();
	cout << endl;
	

	
	
	// delete pointers
//	delete event;
//	delete NoCuts_Res;
//	delete Everything_Res;
//	delete [] JetResults;
	  
	return 0;
}
  
