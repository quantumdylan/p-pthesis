#include "TROOT.h"
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TFile.h>

#include <TLorentzVector.h>
#include <TClonesArray.h>
#include <TString.h>
#include <TChain.h>
#include <TBranch.h>
#include <TMath.h>
#include <TRandom.h>
#include <TSystem.h>
#include "TObject.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TStopwatch.h"

//#include "FastJet3.h"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/ClusterSequencePassiveArea.hh"
#include "fastjet/ClusterSequenceActiveArea.hh"
#include "fastjet/ClusterSequenceActiveAreaExplicitGhosts.hh"
#include "fastjet/Selector.hh"

#include "fastjet/tools/JetMedianBackgroundEstimator.hh"
#include "fastjet/tools/Subtractor.hh"
#include "fastjet/tools/Filter.hh"
#include "fastjet/FunctionOfPseudoJet.hh"

//George's Custom Classes
#include "GParticle.h"
#include "GEvent.h"

using namespace std;
using namespace fastjet;


// -----------------------------------------------------------------------------
//  Di-jet Assymetry Aj
// -----------------------------------------------------------------------------
double CalcAj (fastjet::PseudoJet Jet1, fastjet::PseudoJet Jet2)
{
	return fabs (( Jet1.pt() - Jet2.pt() ) / ( Jet1.pt() + Jet2.pt() )); 
}


// -----------------------------------------------------------------------------

int main(int argc, const char** argv)
{
	TStopwatch timer;
	timer.Start();

	TString path = argv[1]; //location to read .root file with pythia events
	TString filename = argv[2]; // the name of the .root file that stores final events

	cout << "Input path set as: " << path << "." << endl;
	cout << endl << "Filename set as: " << filename << "." << endl;


	// Load input
	cout << endl << "opening:   " << path + filename + TString(".root") << endl << endl;

	TChain* InputChain = new TChain("Events");
	InputChain->Add(path + filename + TString(".root"));
	assert(InputChain->GetEntries() > 0 && "Something went wrong loading input.");

	// George's Custom Classes
	// these are stored in the tree "Events" and are needed to read it
	GEvent *event = new GEvent();
	GParticle *part1 = new GParticle(); // pointer to a particle object used to read particles from the event object

	// each event is a branch
	InputChain->GetBranch("event")->SetAutoDelete(kFALSE);
	InputChain->SetBranchAddress("event", &event);
	
	TString output; //location to place .root file with found jets
	if (argv[3] != NULL){
		output = argv[3]; //if a location is supplied, write to that location
		cout << endl << "External input: " << output << " received for output path! Overriding default..." << endl;
	}
	else{
		output = path; //if a location isn't specified, default to path (FOR BACKWARDS COMPATABILITY)
		cout << endl << "No output path specified, setting to input path..." << endl;
	}

	cout << endl << "Output path set as " << output << ". Will write contents to this directory!" << endl;

	// Output
	TFile* fout = new TFile(output + filename + TString("_FoundJets.root"), "RECREATE"); //made change to allow output to be written to separate location
	TTree *event_tree = new TTree("Events", "A Tree with Events");
	event_tree->Branch("event", &event); //each event is a branch



	//---------------------------------------- Parameters ----------------------------------------

	float R = 0.4;// jet resolution parameter
	float OpeningAngle = 0.4; // back to back di-jet opening angle

	// vector of particles in an event after transformation of TLorentzVector to PseudoJet
	vector<PseudoJet> particles;
	vector<PseudoJet> ch_particles;


	// [GeV] total jet pt
	double jet_ptmin = 5.0;    // [GeV] Min jet pT
	double jet_ptmax = 10000.0;   // [GeV] DEBUG

	double LeadPtMin = 8.0;      // [GeV] Min leading jet pT 
	double SubLeadPtMin = 5.0;   // [GeV] Min sub-leading jet pT 

	double max_track_rap = 200.0; // Constituent eta; acceptance
	double PtConsLo = 0.0;	//  [GeV] Low constituent pT cut 
	double PtConsHi = 1.0;	//  [GeV] High constituent pT cut  

	// Checking for argv values for jet restrictions
	// and setting values if argv exists
	// ---------------------------------------------
	if (argv[4] != NULL){
		R = atof(argv[4]); //input for R
		cout << endl << "External input " << R << " received for R! Overriding default..." << endl;
	}
	if (argv[5] != NULL){
		OpeningAngle = atof(argv[5]); //input for opening angle
		cout << endl << "External input " << OpeningAngle << " received for OpeningAngle! Overriding default..." << endl;
	}
	if (argv[6] != NULL){
		jet_ptmin = atof(argv[6]); //input for pt minimum for accepted jets
		cout << endl << "External input " << jet_ptmin << " received for jet_ptmin! Overriding default..." << endl;
	}
	if (argv[7] != NULL){
		LeadPtMin = atof(argv[7]); //input for leading jet Pt minimum
		cout << endl << "External input " << LeadPtMin << " received for LeadPtMin! Overriding default..." << endl;
	}
	if (argv[8] != NULL){
		SubLeadPtMin = atof(argv[8]); //input for subleading minimum
		cout << endl << "External input " << SubLeadPtMin << " received for SubLeadPtMin! Overriding default..." << endl;
	}
	if (argv[9] != NULL){
		max_track_rap = atof(argv[9]); //maximum track rapidity
		cout << endl << "External input " << max_track_rap << " received for max_track_rap! Overriding default..." << endl;
	}
	if (argv[10] != NULL){
		PtConsLo = atof(argv[10]); //Pt low
		cout << endl << "External input " << PtConsLo << " receieved for PtConsLo! Overriding default..." << endl;
	}
	if (argv[11] != NULL){
		PtConsHi = atof(argv[11]); //Pt high
		cout << endl << "External input " << PtConsHi << " received for PtConsHi! Overriding default..." << endl;
	}

	// output parameters
	// -----------------
	cout << endl << "R: " << R << endl;
	cout << endl << "OpeningAngle: " << OpeningAngle << endl;
	cout << endl << "jet_ptmin: " << jet_ptmin << endl;
	cout << endl << "LeadPtMin: " << LeadPtMin << endl;
	cout << endl << "SubLeadPtMin: " << SubLeadPtMin << endl;
	cout << endl << "max_track_rap: " << max_track_rap << endl;
	cout << endl << "PtConsLo: " << PtConsLo << endl;
	cout << endl << "PtConsHi: " << PtConsHi << endl;

	// derived rapidity cuts
	// ---------------------
	double max_rap = max_track_rap-R;

	// Constituent selectors
	// ---------------------
	Selector select_track_rap = fastjet::SelectorAbsRapMax(max_track_rap);
	Selector select_lopt      = fastjet::SelectorPtMin( PtConsLo );
	Selector select_hipt      = fastjet::SelectorPtMin( PtConsHi );
	
	Selector slo = select_track_rap * select_lopt;
	Selector shi = select_track_rap * select_hipt;

	// Jet candidate selectors
	// -----------------------
	Selector select_jet_rap     = fastjet::SelectorAbsRapMax(max_rap);
	Selector select_jet_pt_min  = fastjet::SelectorPtMin( jet_ptmin );
	Selector select_jet_pt_max  = fastjet::SelectorPtMax( jet_ptmax );
	Selector sjet = select_jet_rap && select_jet_pt_min && select_jet_pt_max;
	
	// Choose a jet and area definition
	// --------------------------------
	JetDefinition jet_def = fastjet::JetDefinition(fastjet::antikt_algorithm, R);

	
	// Get number of events
	// --------------------
	int nEvents=InputChain->GetEntries();// get the number of events
	if (argv[12] != NULL){
		if (atoi(argv[12]) <= nEvents){
			nEvents = atoi(argv[12]);
			cout << endl << "External input " << nEvents << " received for number of events! Overriding default..." << endl;
		}
		else{
			cout << endl << "WARNING! External event number input exceeds nEvents from PYTHIA file, reverting to nEvents from file..." << endl;
		}
	}
	else{
		cout << endl << "Number of events: " << nEvents << ".";
	}

	int JetEvent_counter = 0;
//**********************************************************************************************************************************
//	EVENT LOOP
//**********************************************************************************************************************************
	for (int ev = 0; ev < nEvents; ++ev ){
	    if ( !(ev%10000) ) cout << "Event " << ev << " / " << nEvents << endl;
	  
		particles.clear();// clear vector of pseudojets
		ch_particles.clear();// clear vector of pseudojets
		InputChain->GetEntry(ev);// grabs event ev from the tree		
		
		// partices are stored in the event object in an array TObjArray
		TObjArray *G_Particles = (TObjArray *) event->GetParticles();// fill the array from the event object
	    
		// Transform G_Particles array into (FastJet) PseudoJets
		for ( int i=0; i < G_Particles->GetEntries() ; ++i )
		{
			part1 = (GParticle *) G_Particles->At(i);// get part1
			//FastJet is keeping photons as neutral particles (PID 22)
			//what the fuck
			if (part1->GetPID_PDG() <= 100) continue; // skip photons and general 2-digit PIDs
			if (part1->GetCharge() == 0) continue; // skip all neutral particles
			PseudoJet tempjet(part1->GetPx(), part1->GetPy(), part1->GetPz(), part1->GetE());
			tempjet.set_user_index(i);
			particles.push_back(tempjet); // pseudojet with all particles including neutrals
			if(part1->GetCharge() != 0) ch_particles.push_back(tempjet); // pseudojet with charged particles including neutrals
		}// end transform particle loop
		
		
		// Apply selector to the full particle set
		// defines a subset of PsuedoJets defindes by selectors slo and shi
		
		// only charged
//		vector<PseudoJet> pLo = slo( ch_particles );//contains particles with pt > PtConsLo in rapidity range +/- max_track_rap
//		vector<PseudoJet> pHi = shi( ch_particles );//contains particles with pt > PtConsHi in rapidity range +/- max_track_rap
		// all particles
		vector<PseudoJet> pLo = slo( particles );//contains particles with pt > PtConsLo in rapidity range +/- max_track_rap
		vector<PseudoJet> pHi = shi( particles );//contains particles with pt > PtConsHi in rapidity range +/- max_track_rap
		
// call fastjet and do clustering (NO background subtraction)
// ------------------------------------------------------------------------------------------------------------------------------------------
		// find jets containing all particles
		ClusterSequence csAllPart_temp ( particles, jet_def);
		vector<PseudoJet> csAllPart = fastjet::sorted_by_pt(csAllPart_temp.inclusive_jets() );
		// find high constituent pT jets using only charged particles 
		ClusterSequence csaHi ( pHi, jet_def);
		
		// sjet selector does rapidity cuts of clustered jets
		// use sjet to determine if there are any jets that meet our criteria
		// sorted_by_pt sorts vector of psuedojets by total jet pt
		vector<PseudoJet> HiResult = fastjet::sorted_by_pt( sjet ( csaHi.inclusive_jets() ) );
		
		
//**********************************************************************************************************************************************
//	accepted jet conditions
//**********************************************************************************************************************************************
		int dijet_counter = 0;// the number of dijets found in this event
		int jet_counter = 0;// the number of dijets found in this event
		vector<int> jet_history;
		
		
		// require at least two jets to be found
		// reqire the highest pt jet (which is a leading jet) to have total pt > LeadPtMin
		if(HiResult.size() > 1 && HiResult.at(0).pt() >= LeadPtMin)
		{
			// loo through all paris of jets
			for(int n = 0; n < HiResult.size(); n++)
			  for(int m = n+1; m < HiResult.size(); m++)
			  {
				float dphi = fabs( HiResult.at(n).delta_phi_to ( HiResult.at(m) ));
				double Aj = CalcAj(HiResult.at(n), HiResult.at(m));
				
				if (HiResult.at(n).pt() >= LeadPtMin && HiResult.at(m).pt() >= SubLeadPtMin // require leading and sub-leading jets in correct pt ranges
			        && ( dphi > TMath::Pi() - OpeningAngle ) && ( dphi < TMath::Pi() + OpeningAngle ))// check back to back within OpeningAngle
				{ // accepted jet
					
					// check if jet has already been used in another dijet pair
					int jflag = 0;
					for(int nn = 0; nn < jet_history.size(); nn++)
					  if(n == jet_history[nn] || m == jet_history[nn]) jflag++;
					  
						if(jflag == 0)// jet has been accepted and not repeatedly used
						{
				  
							dijet_counter++;
							jet_history.push_back(n);
							jet_history.push_back(m);
							
//							cout << "Event " << ev << " DiJets " << dijet_counter;
//							cout << " jet" << n << " jet" << m << " dphi " << dphi << " Aj " << Aj;
//							cout << " jet" << n << " pt " << HiResult.at(n).pt() << " ang " << HiResult.at(n).phi() << " rap " << HiResult.at(n).rap();
//							cout << " jet" << m << " pt " << HiResult.at(m).pt() << " ang " << HiResult.at(m).phi() << " rap " << HiResult.at(m).rap();
//							cout << endl;
							
							for (int i=0; i< HiResult.size(); i++) // loop through all jet candidates  
							{
								if(i == n || i == m)// make sure jet is one of the accepted jets
								{
									jet_counter++;
									int leadingID = HiResult.at(i).constituents().at(0).user_index();
									
									for(int j=0; j< HiResult.at(i).constituents().size(); j++) // loop through constituents of jet candidate
									{
										
										int index = HiResult.at(i).constituents().at(j).user_index();
										
										// check if the particles added to the event_tree are the same as the ones in this jet
										if(event->GetParticle(index)->GetPx() != HiResult.at(i).constituents().at(j).px())
										  cout << "***** Event " << ev << " PseudoJet with index " << index << " px= " 
										  << HiResult.at(i).constituents().at(j).px()
										  << " doesn't match event->GetParticle(index) px= " << event->GetParticle(index)->GetPx() << "*****" << endl;
										
										if(event->GetParticle(index)->GetPy() != HiResult.at(i).constituents().at(j).py())
										  cout << "***** Event " << ev << " PseudoJet with index " << index << " py= " 
										  << HiResult.at(i).constituents().at(j).py()
										  << " doesn't match event->GetParticle(index) py= " << event->GetParticle(index)->GetPy() << "*****" << endl;
										
										if(event->GetParticle(index)->GetPz() != HiResult.at(i).constituents().at(j).pz())
										  cout << "***** Event " << ev << " PseudoJet with index " << index << " pz= " 
										  << HiResult.at(i).constituents().at(j).pz()
										  << " doesn't match event->GetParticle(index) pz= " << event->GetParticle(index)->GetPz() << "*****" << endl;
										
										if(event->GetParticle(index)->GetE() != HiResult.at(i).constituents().at(j).e())
										  cout << "***** Event " << ev << " PseudoJet with index " << index << " E= " 
										  << HiResult.at(i).constituents().at(j).e()
										  << " doesn't match event->GetParticle(index) E= " << event->GetParticle(index)->GetE() << "*****" << endl;
										
										  event->GetParticle(index)->SetJetNumber(jet_counter);
										  event->SetNumJets(jet_counter);// this will only be correct if csAllPart is sorted by jet pt

										  if (event->GetParticle(index)->GetPt() >= event->GetParticle(leadingID)->GetPt()){
											  leadingID = index;
										  }
									}// end loop through jet constituents

									event->GetParticle(leadingID)->SetIsLeading(true);
									/*
									//verbose logging for debugging purposes
									if (event->GetParticle(leadingID)->GetIsLeading()){
										cout << "Set jet " << leadingID << " as leading in event " << ev << "!" << endl;
										cout << "Jet " << leadingID << " has eta: " << event->GetParticle(leadingID)->GetEta() << " and rap: " << event->GetParticle(leadingID)->GetRap() << endl;
										cout << "Jet " << leadingID << " has charge: " << event->GetParticle(leadingID)->GetCharge() << " and pt: " << event->GetParticle(leadingID)->GetPt() << endl;
										cout << "Jet " << leadingID << " has PID: " << event->GetParticle(leadingID)->GetPID_PDG() << endl;
									}
									*/

								}// end jet is accepted jet
							}// end loop through jets
						}// end repeated jet condition
				}// end accpeted jet condition two
			  }// end jet pair loop
		}// end accpeted jet condition one
		
		if(dijet_counter > 0) {
		  JetEvent_counter++;// if any dijets were found increase JetEvent_counter
		  cout << "Event " << ev << " Number of Jets accepted = " << jet_counter <<  " Number of DiJets found = " << dijet_counter << endl;
		}
		jet_history.clear();
		

		
		// Fill the pythia event into the TTree.  
		// Warning: the files will rapidly become large if all events 
		// are saved. In some cases it may be convenient to do some 
		// processing of events and only save those that appear 
		// interesting for future analyses.
		event_tree->Fill();
		// then clear the event class container
		event->Clear();
		
	} // end event loop

	timer.Stop();
	
	cout << endl << "Found " << JetEvent_counter << " events with jets." << endl;
	cout << endl << "Time elapsed: ";
	timer.Print(); 

	
	// Close up shop
	//  Write tree.
//	event_tree->Print();
	event_tree->Write();
	
	delete fout;
	delete event;
	delete part1;
	  
	  
	return 0;
}
  
