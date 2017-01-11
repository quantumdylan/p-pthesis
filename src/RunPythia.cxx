//-----------------------------------------------------------------------------------------------------------------
// Make a tree of events using George's Custom Event and Particles Classes
// run it with    : ./bin/RunPythia
//
//	to view in root:
//	.L /home/moschelli/Dropbox/Research/pythia/RunPythiaFastJet/src/GParticle.cxx+
//	.L /home/moschelli/Dropbox/Research/pythia/RunPythiaFastJet/src/GEvent.cxx+
//	TFile *_file0 = TFile::Open("/media/scratch/moschelli/pythia_results/FileName.root")
//-----------------------------------------------------------------------------------------------------------------

#include "Pythia.h"

#include <iostream>
#include <string>
#include <sstream>

#include "TROOT.h"
#include "TMath.h"
#include "TString.h"
#include "TObjString.h"
#include "TObject.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TF1.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"

//George's Custom Classes
#include "GParticle.h"
#include "GEvent.h"

using namespace std;
using namespace Pythia8;

Int_t main(int argc, char **argv) 
{
	int nEvent = atoi(argv[1]);// set the number of events to generate
	double roots = atof(argv[2]);// [GeV] per nucleon center of mass energy
	
	// kinimatic cut of hard scatterings at generation level [GeV]
	float ptHatMin = atof(argv[3]);
	float ptHatMax = atof(argv[4]);
	
	TString path = argv[5]; //location to place .root file with pythia events
	TString filename = argv[6]; // the name of the .root file that stores final events
	
	//***************************************************************************************************
	//	CONFIGURE PYTHIA
	//***************************************************************************************************
//	Pythia pythia("/usr/share/pythia8-data/xmldoc");
        Pythia pythia("/media/scratch/pythia/share/Pythia8/xmldoc");
	Event& event = pythia.event;

	// Process selection.
	pythia.readString("HardQCD:all = on");  
	pythia.readString("HadronLevel:Decay = on");
	pythia.readString("PartonLevel:ISR = on");
	pythia.readString("PartonLevel:FSR = on");

	stringstream numbf(stringstream::app|stringstream::in|stringstream::out);
	numbf.setf(ios::fixed, ios::floatfield);
	numbf.setf(ios::showpoint);
	numbf.precision(1);
	numbf.str("PhaseSpace:pTHatMin = "); numbf << ptHatMin;
	string pmin=numbf.str();
	numbf.str("PhaseSpace:pTHatMax = "); numbf << ptHatMax;
	string pmax=numbf.str();

	cout << pmin << endl;
	cout << pmax << endl;

	pythia.readString(pmin);  
	pythia.readString(pmax);

	// No event record printout.
	pythia.readString("Next:numberShowInfo = 0"); 
	pythia.readString("Next:numberShowProcess = 0"); 
	pythia.readString("Next:numberShowEvent = 0"); 

	// for pPb
	//pythia.readString("Tune:pp = 5");
	//pythia.readString("SigmaProcess:Kfactor = 0.7");

	//random seed
	// Pick new random number seed for each run, based on clock.
	pythia.readString("Random:setSeed = on");
	pythia.readString("Random:seed = 0");

	// LHC initialization.
	//pythia.init(2212, 2212,5020.);
	// pythia.init(2212, 2212,2760.);

	// RHIC initialization.
	pythia.readString("Beams:idA = 2212");// proton = 2212
	pythia.readString("Beams:idB = 2212");// proton = 2212 
	pythia.readString("Beams:eCM = " + to_string(roots));
	pythia.init();

	// ----------------------------------------------------------------------------------------
	// Set up tree
	// ----------------------------------------------------------------------------------------  
	// George's Custom Classes
	GEvent *g_event = new GEvent();
	GParticle *g_particle = new GParticle();
	
	// Set up the ROOT TFile and TTree.
	TFile *fout = TFile::Open(path+filename+TString(".root"),"recreate");
	TTree *event_tree = new TTree("Events","A Tree with Events");
	event_tree->Branch("event",&g_event);// each event is a branch

	//****************************************************************************************
	//	EVENT LOOP
	//****************************************************************************************
	int nEv = 0;
	do {
		if (!pythia.next()) continue;// skip unsuccessfull events
		
		// set some event information
		g_event->SetNA(1);
		g_event->SetNB(1);
		g_event->SetEventNum(nEv);
		g_event->SetMult(event.size());//Multiplicity of all particles ever created in event (including parents and daughters)
		g_event->SetRootS(roots);
		g_event->Setb(0);
		g_event->SetFlag(1);
		
		Int_t Nch = 0;		//number of charged particles in event nEv
		Int_t Nplus = 0;	//number of positively charged particles in event nEv
		Int_t Nminus = 0;	//number of negatively charged particles in event nEv
		Int_t Ntot = 0;		//total number of particles in event nEv
		Int_t Nhard = 0;	// number of hard partons in event nEv
		Double_t E_tot = 0.0;	// total energy in event nEv
		Double_t Ech_tot = 0.0;	// total energy of charged particles in event nEv
		Double_t ET_tot = 0.0;	// total transverse energy in event nEv
		Double_t ETch_tot = 0.0;// total transverse energy of charged particles in event nEv
		Double_t PT_tot = 0.0;	// total transverse momentum in event nEv
		Double_t PTch_tot = 0.0;// total transverse momentum of charged particles in event nEv
    
//		cout << "event_" << nEv << "mult " << event.size() << endl;
		
		//********************************************************************************************************
		//	PARTICLE LOOP
		//********************************************************************************************************
		for (int nP = 0; nP<event.size() ; ++nP )
		{
			if ( event.size() < 3 )continue;      // if less than three particles there is only scattered beams so skip
      
			Particle& part = event[nP];// grab a particle using the pythia particle and event classes
	 
			g_particle->Clear();// make sure particle class container is empty before filling stuff
			
			// set some particle information
			g_particle->SetPID_PDG(part.id());
			g_particle->SetStatus(part.status());// indicates final particle or initial scattering
			g_particle->SetPx(part.px());
			g_particle->SetPy(part.py());
			g_particle->SetPz(part.pz());
			g_particle->SetPtot(sqrt(part.px()*part.px() + part.py()*part.py() + part.pz()*part.pz()));
			g_particle->SetPt(part.pT());
			g_particle->SetE(part.e());
			g_particle->SetRap(part.y());
			g_particle->SetPhi(part.phi());
			g_particle->SetEta(part.eta());
			g_particle->SetMass(part.m());
			g_particle->SetCharge(part.charge());
			
			
			// Save hardest partons
			if (part.status()==-23){ // status -23 means result of initial hard scattering
				g_event->AddHardParton(g_particle);
				continue; // skip the other event uptate stuff
			}
			
			if ( !part.isFinal() ) continue;// if particle is not final state particle then skip
			if ( !part.name().compare(0, 3, "nu_") ) continue; // skip neutrinos
			
			
			// add pirticle to evnet
			g_event->AddParticle(g_particle);
			
			// calculate some total event information
			Ntot++; // note Ntot also gets incremented by the AddParticle function
			E_tot += part.e();
			ET_tot += sqrt(part.m()*part.m() + part.pT()*part.pT());
			PT_tot += part.pT();
			if(part.charge() > 0.) {
				Nch++;
				Nplus++;
				Ech_tot += part.e();
				ETch_tot += sqrt(part.m()*part.m() + part.pT()*part.pT());
				PTch_tot += part.pT();
			}
			if(part.charge() < 0.) {
				Nch++;
				Nminus++;
				Ech_tot += part.e();
				ETch_tot += sqrt(part.m()*part.m() + part.pT()*part.pT());
				PTch_tot += part.pT();
			}
		}// end particle loop
    
    
		// set calculated event information
		g_event->SetNtot(Ntot);// Ntot member gets overwritten
		g_event->SetNch(Nch);
		g_event->SetNplus(Nplus);
		g_event->SetNminus(Nminus);
		g_event->SetE_tot(E_tot);
		g_event->SetEch_tot(Ech_tot);
		g_event->SetET_tot(ET_tot);
		g_event->SetETch_tot(ETch_tot);
		g_event->SetPT_tot(PT_tot);
		g_event->SetPTch_tot(PTch_tot);
		

		// Fill the pythia event into the TTree.  
		// Warning: the files will rapidly become large if all events 
		// are saved. In some cases it may be convenient to do some 
		// processing of events and only save those that appear 
		// interesting for future analyses.
		event_tree->Fill();
		// then clear the event class container
		g_event->Clear();
    
		++nEv;
	} while (nEv < nEvent);// end event loop

    
	//  Write tree.
//	event_tree->Print();
	event_tree->Write();
	// delete pointers
	delete fout;
//	delete part;
	delete g_event;
	delete g_particle;
  
  return 0;
}
