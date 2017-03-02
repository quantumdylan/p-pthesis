//George's Custom Classes
#include "GParticle.h"
#include "GEvent.h"
#include "Results.h"

//include ROOT headers
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
#include "TArray.h"
#include "TArrayD.h"

#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

int main(int argc, const char** argv) {
	//TString inpath, TString prefix, TString filename, TString outpath, TString logpath, int ipass, int nCores
	//initialize the 3 histogram results objects
	TString inpath = argv[1];
	TString prefix = argv[2];
	TString filename = argv[3];
	TString outpath = argv[4];
	TString logpath = argv[5];
	int ipass = atoi(argv[6]);
	int nCores = atoi(argv[7]);

	Results *NoCuts_Res;
	Results *Everything_Res;

	//int numJetClasses = Everything_Res->Get_NumJetClasses();
	int numJetClasses = 7;
	cout << "Hello?" << endl;
	cout << "Total number of jet classes from file: " << numJetClasses << endl;

	Results *JetResults[numJetClasses];

	//**CHANGES MADE FROM INITIAL CODE:
	//**All commented code removed from this block, added more verbose documentation to save headaches

	//**no_hist BRANCH CHANGES:
	//**Removed all code pertaining to histograms. Working on discrete plots.

	//-------------------------------------------------------------------------------------------------------
	//	Simple loop to load all result objects from files and write to above containers
	//-------------------------------------------------------------------------------------------------------
	for (int i = 0; i < nCores + 1; i++) {
		if (i < nCores) {
			TFile *tmp = TFile::Open(inpath + prefix + filename + TString("_run") + TString(Form("%d", i)) + TString("_ANALYZED.root"));

			//print some information for logging purposes (current loop index, file being read, list contents of file, then 3 carriage returns
			//in this case, index correponds to which file is being opened
			cout << "Index:   " << TString(Form("%d", i)) << "    File: " << inpath + prefix + filename + TString("_run") + TString(Form("%d", i)) + TString("_ANALYZED.root") << endl << endl;
			cout << "Contents of file: " << endl;
			tmp->ls();
			cout << endl << endl << endl;

			//start parsing information
			//small condition: if we're on the first loop then we initialize the empty containers with the first file
			if (i == 0) {
				NoCuts_Res = (Results*)tmp->Get("NoCuts"); //Get the first, then add to this
				Everything_Res = (Results*)tmp->Get("Everything");
				for (int i = 0; i < numJetClasses; i++) { // initialize jet results classes
					JetResults[i] = (Results*)tmp->Get("NumJets" + TString(Form("%d", i)));
				}

				cout << "Got past initial conditions" << endl;
			}
			//all other results objects are added to the current running totals using Results->Add()
			else {
				NoCuts_Res->Add((Results*)tmp->Get("NoCuts")); //Add this set of data to our master set
				Everything_Res->Add((Results*)tmp->Get("Everything"));
				for (int i = 0; i < numJetClasses; i++) { // initialize jet results classes
					JetResults[i]->Add((Results*)tmp->Get("NumJets" + TString(Form("%d", i))));
				}
			}
			cout << "Got past add loops" << endl;
		}
		//once all files have been read, the loop proceeds for one additional pass to calculate all variables
		//by calling Results->Add()
		else {
			for (int j = 0; j < ipass; j++) {
				NoCuts_Res->Calculate(j);
				Everything_Res->Calculate(j);
				for (int k = 0; k < numJetClasses; k++) JetResults[k]->Calculate(j);
			}
		}
		cout << "Got past calculations" << endl;
	}
	//end of file parsing loop

	//-------------------------------------------------------------------------------------------------------
	//	START OF DISPLAY SECTION OF COMBINE
	//	WILL DISPLAY INTEGRAL AND HISTOGRAM RESULTS
	//-------------------------------------------------------------------------------------------------------

	// kinematic cuts
	float roots; // [GeV] center of mass collision energy
	float eta_min = -2.5, eta_max = 2.5;// min and max pseudo rapidity range
	float rap_min = -10, rap_max = 10;// min and max pseudo rapidity range
	float pt_min = 0.5, pt_max = 900000.0;// [GeV] min and max transverse momentum range


	//int nbins = 30;
	//int Nmax = 100;

	//-------------------------------------------------------------------------------------------------------
	//	Open file for writing info
	//-------------------------------------------------------------------------------------------------------
	TFile *outRoot = new TFile(outpath + prefix + filename + TString("_COMBINED.root"), "RECREATE");


	//-------------------------------------------------------------------------------------------------------
	//	Print some results
	//-------------------------------------------------------------------------------------------------------
	//load integral output file
	//data will be stored in tablature format, openable in excel natively
	ofstream out;
	out.open(outpath + prefix + filename + "_COMBINED.dat");

	out << "eta_min = " << eta_min << endl;
	out << "eta_max = " << eta_max << endl;
	out << "rap_min = " << rap_min << endl;
	out << "rap_max = " << rap_max << endl;
	out << "pt_min = " << pt_min << endl;
	out << "pt_max = " << pt_max << endl;

	out << "all charged particles" << endl << endl;


	out << "NumJet\tsqrtS\tNev\t<N>\t<N^2>\tVarN\tVarN2p\t<Pt>\t<pt>\t<Pt*N>\tCovPtN\tCovPtN2p\tD\tR\tD2p\tR2p" << endl;

	out << NoCuts_Res->Get_NumJets() << "\t" << NoCuts_Res->Get_SqrtS() << "\t";
	out << NoCuts_Res->Get_Nevents() << "\t" << NoCuts_Res->Get_AvgN() << "\t";
	out << NoCuts_Res->Get_AvgNsq() << "\t" << NoCuts_Res->Get_VarN() << "\t" << NoCuts_Res->Get_VarN2P() << "\t";
	out << NoCuts_Res->Get_AvgTotPt() << "\t" << NoCuts_Res->Get_AvgPt() << "\t";
	out << NoCuts_Res->Get_AvgPtN() << "\t" << NoCuts_Res->Get_CovPtN() << "\t" << NoCuts_Res->Get_CovPtN2P() << "\t";
	out << NoCuts_Res->Get_D() << "\t" << NoCuts_Res->Get_R() << "\t";
	out << NoCuts_Res->Get_D2P() << "\t" << NoCuts_Res->Get_R2P();
	out << endl;


	out << Everything_Res->Get_NumJets() << "\t" << Everything_Res->Get_SqrtS() << "\t";
	out << Everything_Res->Get_Nevents() << "\t" << Everything_Res->Get_AvgN() << "\t";
	out << Everything_Res->Get_AvgNsq() << "\t" << Everything_Res->Get_VarN() << "\t" << Everything_Res->Get_VarN2P() << "\t";
	out << Everything_Res->Get_AvgTotPt() << "\t" << Everything_Res->Get_AvgPt() << "\t";
	out << Everything_Res->Get_AvgPtN() << "\t" << Everything_Res->Get_CovPtN() << "\t" << Everything_Res->Get_CovPtN2P() << "\t";
	out << Everything_Res->Get_D() << "\t" << Everything_Res->Get_R() << "\t";
	out << Everything_Res->Get_D2P() << "\t" << Everything_Res->Get_R2P();
	out << endl;

	for (int i = 0; i < numJetClasses; i++) {
		out << JetResults[i]->Get_NumJets() << "\t" << JetResults[i]->Get_SqrtS() << "\t";
		out << JetResults[i]->Get_Nevents() << "\t" << JetResults[i]->Get_AvgN() << "\t";
		out << JetResults[i]->Get_AvgNsq() << "\t" << JetResults[i]->Get_VarN() << "\t" << JetResults[i]->Get_VarN2P() << "\t";
		out << JetResults[i]->Get_AvgTotPt() << "\t" << JetResults[i]->Get_AvgPt() << "\t";
		out << JetResults[i]->Get_AvgPtN() << "\t" << JetResults[i]->Get_CovPtN() << "\t" << JetResults[i]->Get_CovPtN2P() << "\t";
		out << JetResults[i]->Get_D() << "\t" << JetResults[i]->Get_R() << "\t";
		out << JetResults[i]->Get_D2P() << "\t" << JetResults[i]->Get_R2P();
		out << endl;
	}

	out.close();

	cout << "Finished writing dat results" << endl;


	vector<TArrayD*> content_array;

	for (int i = 0; i < 10; i++){
		TArrayD* temp = new TArrayD(numJetClasses + 1); // need one extra slot for the "everything" objects

		content_array.push_back(temp); // add the generic array to the stack.
	}

	cout << "Generated empty arrays" << endl;

	for (int j = 0; j < numJetClasses + 1; j++){
		Results* current_results;

		switch (j){
		case 0: current_results = Everything_Res; break;
		default: current_results = JetResults[j - 1]; // if it's not the Everything_Res special case then it's definitely JetResults
		}

		for (int i = 0; i < 10; i++){
			double current_element;

			switch (i){
			case 0: 
				current_element = current_results->Get_Nevents(); // this is the basic template, but change the Get() function for each to their respective things.
				break;
			case 1:
				current_element = current_results->Get_AvgN();
				break;
			case 2:
				current_element = current_results->Get_AvgNsq();
				break;
			case 3:
				current_element = current_results->Get_VarN();
				break;
			case 4:
				current_element = current_results->Get_AvgTotPt();
				break;
			case 5:
				current_element = current_results->Get_AvgPt();
				break;
			case 6:
				current_element = current_results->Get_PtN_tot();
				break;
			case 7:
				current_element = current_results->Get_CovPtN();
				break;
			case 8:
				current_element = current_results->Get_D();
				break;
			case 9:
				current_element = current_results->Get_R();
				break;
			default: cout << "FUCK" << endl;
			}

			content_array.at(i)->SetAt(current_element, j); // set the element for the TArrayD
		}
	}

	cout << "Got results from file" << endl;

	int nbins = numJetClasses;
	int xpix = 500, ypix = 500;
	
	vector<TH1D*> all_jet_hists;
	vector<TCanvas*> all_jet_canvas;
	vector<TLegend*> all_jet_leg;

	// filling jet histograms
	TString jet_titles[10] = { "N_eve_hist", "N_avg_hist", "N_avg_sq_hist", "Var_N_hist", "Pt_tot_avg_hist", "pt_avg_hist", "PtN_tot_hist", "Cov_PtN_hist", "D_hist", "R_hist" };
	TString can_titles[10] = { "cNev_JetC", "cNav_JetC", "cNsq_JetC", "cVarN_JetC", "cPt_JetC", "cpt_JetC", "cPtN_JetC", "cCov_PtN_JetC", "cD_JetC", "cR_JetC" };
	TString y_axis_titles[10] = { "N_{ev}", "N_{avg}", "N_{avg}^2", "Var(N)", "P_{T}", "p_{T}", "P_{T}N", "Cov(P_{T}N)", "D", "R"};
	TString x_axis_title = "Jet Content";

	for (int i = 0; i < 10; i++){
		TCanvas* current_canvas = new TCanvas(can_titles[i], can_titles[i], 0, 0, xpix, ypix); // create empty canvas with specified name
		TLegend* current_leg = new TLegend(0.8, 0.8, 1.0, 1.0);
		TH1D* current_jet_hist_vector = new TH1D(jet_titles[i], jet_titles[i], nbins, 0.0, nbins); // create an empty histogram with the specified name

		for (int j = 1; j < numJetClasses + 1; j++){
			current_jet_hist_vector->SetBinContent(j - 1, content_array.at(i)->GetAt(j)); // fill the histogram with information from our data
		}

		cout << "Set histogram content from array" << endl;

		all_jet_hists.push_back(current_jet_hist_vector); // add the histogram group to the stack
		all_jet_canvas.push_back(current_canvas);
		all_jet_leg.push_back(current_leg);
	}

	cout << "Created canvases and legends" << endl;

	for (int i = 0; i < 10; i++){
		all_jet_canvas.at(i)->cd();

		all_jet_hists.at(i)->SetLineColor(3);
		all_jet_hists.at(i)->SetLineStyle(1);
		all_jet_hists.at(i)->SetMarkerColor(3);
		all_jet_hists.at(i)->SetMarkerStyle(20);
		all_jet_hists.at(i)->SetMarkerSize(1);
		all_jet_hists.at(i)->GetXaxis()->SetTitle(x_axis_title);
		all_jet_hists.at(i)->GetYaxis()->SetTitle(y_axis_titles[i]);

		//"N_eve_hist", "N_avg_hist", "N_avg_sq_hist", "Var_N_hist", "Pt_avg_hist", "pt_avg_hist", "PtN_tot_hist", "Cov_PtN_hist", "D_hist", "R_hist"

		if ((i >= 0) && (i < 3)){
			all_jet_hists.at(i)->GetYaxis()->SetRangeUser(1, 1e8);
			gPad->SetLogy();
		}
		else{
			double range_max = 0;
			double range_min = 0;
			switch (i){
			case 3: range_max = 900; range_min = 0; break;
			case 4: range_max = 200; range_min = 0; break;
			case 5: range_max = 2; range_min = 0; break;
			case 6: range_max = 5e8; range_min = 0; break;
			case 7: range_max = 1e3; range_min = 0; break;
			case 8: range_max = 0.01; range_min = -0.02; break;
			case 9: range_max = 0.15; range_min = 0; break;
			default: cout << "motherfucker" << endl;
			}

			all_jet_hists.at(i)->GetYaxis()->SetRangeUser(range_min, range_max);
		}

		all_jet_hists.at(i)->Draw("LP hist");

		all_jet_leg.at(i)->AddEntry(all_jet_hists.at(i), "Jet Content", "LP hist");
		all_jet_leg.at(i)->Draw();

		TString pdfpath(outpath + jet_titles[i] + "_Cone_" + prefix + "_JetContent.pdf");

		all_jet_canvas.at(i)->Write();
		all_jet_canvas.at(i)->Print(pdfpath, "pdf");
	}

	cout << "Set all histograms and drawn" << endl;


	//close the file to prevent issues with file read/write
	outRoot->Close();
	return 0;
}