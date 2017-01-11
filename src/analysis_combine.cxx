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

Bool_t PythiaLibs() {
	if (gROOT) {
		cout << "-- Compile and Load the PythiaFluc Libraries --" << endl;
		// check if libraries are already loaded
		if (gClassTable->GetID("GParticle") < 0) {
			gROOT->LoadMacro("~/Dropbox/packard_SeniorProj/moschelli/24Sep2016/src/GParticle.cxx+");
		}
		if (gClassTable->GetID("GEvent") < 0) {
			gROOT->LoadMacro("~/Dropbox/packard_SeniorProj/moschelli/24Sep2016/src/GEvent.cxx+");
		}
		if (gClassTable->GetID("Results") < 0) {
			gROOT->LoadMacro("~/Dropbox/packard_SeniorProj/moschelli/24Sep2016/src/Results.cxx+");
		}
		cout << "-- PythiaFluc Libraries compiled and loaded --" << endl;
	}
	return kTRUE;
}

int main(int argc, const char** argv){
	PythiaLibs();

	int nCores = atoi(argv[1]);// set the number of .root files to read
	TString path = argv[2]; //location to place .root file with pythia events
	cout << path << endl;
	TString filename = argv[3]; // the name of the .root file that stores final events
	cout << filename << endl;


	// kinematic cuts
	float roots; // [GeV] center of mass collision energy
	float eta_min = -2.5, eta_max = 2.5;// min and max pseudo rapidity range
	float rap_min = -10, rap_max = 10;// min and max pseudo rapidity range
	float pt_min = 0.5, pt_max = 900000.0;// [GeV] min and max transverse momentum range


	int nbins = 10;
	int Nmax = 100;

	cout << path + filename + "0" + "_ANALYZED.root" << endl;
	TFile *initial_file = TFile::Open(path + filename + TString("0") + TString("_ANALYZED.root"), "RECREATE");
	initial_file->ls();
	

	// Load results classes from analysis
	Results *NoCuts_Res = new Results();
	NoCuts_Res = (Results*)initial_file->Get("NoCuts"); // not cuts of any kind
	cout << "Added to NoCuts" << endl;							

	Results *Everything_Res = new Results();
	Everything_Res = (Results*)initial_file->Get("Everything"); // no jet discrimination
	cout << "Added to Everything" << endl;

	Results *JetResults[4];// discriminates by jet content
	for (int i = 0; i < 4; i++) { // initialize jet results classes
		JetResults[i] = new Results();
		JetResults[i] = (Results*)initial_file->Get("NumJets" + TString(Form("%d", i * 2)));
		cout << "Added to NumJets" + TString(Form("%d", i * 2)) << endl;
	}

	initial_file->Close();

	int max_change = atoi(argv[6]);


	for (int i = 1; i < nCores + 1; i++) {
		cout << "Entered first loop" << endl;
		TFile *tmp_file = TFile::Open(path + filename + TString(Form("%d", i)) + TString("_ANALYZED.root"), "RECREATE");
		if (tmp_file->IsOpen()) cout << "hoo fuckin rah" << endl;
		cout << path + filename + TString(Form("%d", i)) + "_ANALYZED.root" << endl;
		tmp_file->ls();

		Results *tmpNoCut = (Results*)tmp_file->Get("NoCuts");
		if (tmpNoCut == NULL) cout << "well fuck me" << endl;
		cout << "fuck1" << endl;
		NoCuts_Res->Add(tmpNoCut);
		cout << "Added to NoCuts" << endl;

		Results *tmpEverything = (Results*)tmp_file->Get("Everything");
		Everything_Res->Add(tmpEverything);
		cout << "Added to Everything" << endl;


		for (int i = 0; i < 4; i++) { // initialize jet results classes
			Results *tempJet = (Results*)tmp_file->Get("NumJets" + TString(Form("%d", i * 2)));
			JetResults[i]->Add(tempJet);
			cout << "Added to NumJets" + TString(Form("%d", i * 2)) << endl;
		}
		tmp_file->Close();
	}

	

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


	int ipass = atoi(argv[7]);

	for (int i = 0; i < ipass; i++) {
		NoCuts_Res->Calculate(ipass);
		Everything_Res->Calculate(ipass);
		for (int i = 0; i < 4; i++) JetResults[i]->Calculate(ipass);
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	Print some results
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ofstream out;
	out.open(output + filename);

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

	for (int i = 0; i < 4; i++) {
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

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	Make Some Plots
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	TH1D *Nev_h[5];
	TH1D *avg_pt_h[5], *VarN_h[5], *ptVarN_h[5], *CovPtN_h[5], *D_h[5], *R_h[5];
	TH1D *avg_N_h[5], *avg_Nsq_h[5], *N_avg_sq_h[5];


	int xpix = 500, ypix = 500;



	//-------------------------------------------------------------------------------------------------------------
	//	Nev vs Nch
	//-------------------------------------------------------------------------------------------------------------
	TCanvas *cNev_Nch = new TCanvas("cNev_Nch", "cNev_Nch", 0, 0, xpix, ypix);
	cNev_Nch->cd();

	TLegend* leg_Nev = new TLegend(0.8, 0.8, 1.0, 1.0);

	// clone and plot histograms
	for (int i = 0; i < 5; i++) {
		if (i == 0) {
			Nev_h[i] = (TH1D*)Everything_Res->N_eve_hist->Clone();
			//Nev_h[i]->SetTitle("TEST");
			Nev_h[i]->SetLineColor(i + 1);
			Nev_h[i]->SetLineStyle(1);
			Nev_h[i]->SetMarkerColor(i + 1);
			Nev_h[i]->SetMarkerStyle(20);
			Nev_h[i]->SetMarkerSize(1);
			Nev_h[i]->GetXaxis()->SetTitle("N_{ch}");
			Nev_h[i]->GetYaxis()->SetTitle("N_{ev}");
			Nev_h[i]->GetYaxis()->SetRangeUser(1, 1e6);// adjust y-axis range
			gPad->SetLogy();
			Nev_h[i]->Draw("LP");

			leg_Nev->AddEntry(Nev_h[i], Everything_Res->Get_Name(), "LP");
		}
		else {
			Nev_h[i] = (TH1D*)JetResults[i - 1]->N_eve_hist->Clone();
			Nev_h[i]->SetLineColor(i + 1);
			Nev_h[i]->SetLineStyle(1);
			Nev_h[i]->SetMarkerColor(i + 1);
			Nev_h[i]->SetMarkerStyle(20);
			Nev_h[i]->SetMarkerSize(1);
			Nev_h[i]->Draw("LPsame");

			leg_Nev->AddEntry(Nev_h[i], TString(Form("%d", 2 * (i - 1))) + "Jets", "LP");
		}
	}

	leg_Nev->Draw();



	//-------------------------------------------------------------------------------------------------------------
	//	<pt> vs Nch
	//-------------------------------------------------------------------------------------------------------------
	TCanvas *cpt_Nch = new TCanvas("cpt_Nch", "cpt_Nch", 0, 0, xpix, ypix);
	cpt_Nch->cd();

	TLegend* leg_pt = new TLegend(0.8, 0.8, 1.0, 1.0);

	// clone and plot histograms
	for (int i = 0; i < 5; i++) {
		if (i == 0) {
			avg_pt_h[i] = (TH1D*)Everything_Res->pt_avg_hist->Clone();
			//avg_pt_h[i]->SetTitle("");
			avg_pt_h[i]->SetLineColor(i + 1);
			avg_pt_h[i]->SetLineStyle(1);
			avg_pt_h[i]->SetMarkerColor(i + 1);
			avg_pt_h[i]->SetMarkerStyle(20);
			avg_pt_h[i]->SetMarkerSize(1);
			avg_pt_h[i]->GetXaxis()->SetTitle("N_{ch}");
			avg_pt_h[i]->GetYaxis()->SetTitle("<p_{T}>");
			avg_pt_h[i]->GetYaxis()->SetRangeUser(0, 2);// adjust y-axis range
			avg_pt_h[i]->Draw("LP");

			leg_pt->AddEntry(avg_pt_h[i], Everything_Res->Get_Name(), "LP");
		}
		else {
			avg_pt_h[i] = (TH1D*)JetResults[i - 1]->pt_avg_hist->Clone();
			avg_pt_h[i]->SetLineColor(i + 1);
			avg_pt_h[i]->SetLineStyle(1);
			avg_pt_h[i]->SetMarkerColor(i + 1);
			avg_pt_h[i]->SetMarkerStyle(20);
			avg_pt_h[i]->SetMarkerSize(1);
			avg_pt_h[i]->Draw("LPsame");

			leg_pt->AddEntry(avg_pt_h[i], TString(Form("%d", 2 * (i - 1))) + "Jets", "LP");
		}
	}

	leg_pt->Draw();


	//-------------------------------------------------------------------------------------------------------------
	//	Var(N) vs Nch
	//-------------------------------------------------------------------------------------------------------------
	TCanvas *cVarN_Nch = new TCanvas("cVarN_Nch", "cVarN_Nch", 0, 0, xpix, ypix);
	cVarN_Nch->cd();

	TLegend* leg_VarN = new TLegend(0.8, 0.8, 1.0, 1.0);

	// clone and plot histograms
	for (int i = 0; i < 5; i++) {
		if (i == 0) {
			VarN_h[i] = (TH1D*)Everything_Res->VarN_hist->Clone();
			//VarN_h[i]->SetTitle("");
			VarN_h[i]->SetLineColor(i + 1);
			VarN_h[i]->SetLineStyle(1);
			VarN_h[i]->SetMarkerColor(i + 1);
			VarN_h[i]->SetMarkerStyle(20);
			VarN_h[i]->SetMarkerSize(1);
			VarN_h[i]->GetXaxis()->SetTitle("N_{ch}");
			VarN_h[i]->GetYaxis()->SetTitle("Var(N)");
			VarN_h[i]->GetYaxis()->SetRangeUser(0, 5);// adjust y-axis range
			VarN_h[i]->Draw("LP");

			leg_VarN->AddEntry(VarN_h[i], Everything_Res->Get_Name(), "LP");
		}
		else {
			VarN_h[i] = (TH1D*)JetResults[i - 1]->VarN_hist->Clone();
			VarN_h[i]->SetLineColor(i + 1);
			VarN_h[i]->SetLineStyle(1);
			VarN_h[i]->SetMarkerColor(i + 1);
			VarN_h[i]->SetMarkerStyle(20);
			VarN_h[i]->SetMarkerSize(1);
			VarN_h[i]->Draw("LPsame");

			leg_VarN->AddEntry(VarN_h[i], TString(Form("%d", 2 * (i - 1))) + "Jets", "LP");
		}
	}

	leg_VarN->Draw();



	//-------------------------------------------------------------------------------------------------------------
	//	<N> vs Nch
	//-------------------------------------------------------------------------------------------------------------
	TCanvas *cN_Nch = new TCanvas("cN_Nch", "cN_Nch", 0, 0, xpix, ypix);
	cN_Nch->cd();

	TLegend* leg_avgN = new TLegend(0.8, 0.8, 1.0, 1.0);

	// clone and plot histograms
	for (int i = 0; i < 5; i++) {
		if (i == 0) {
			avg_N_h[i] = (TH1D*)Everything_Res->N_avg_hist->Clone();
			//avg_N_h[i]->SetTitle("");
			avg_N_h[i]->SetLineColor(i + 1);
			avg_N_h[i]->SetLineStyle(1);
			avg_N_h[i]->SetMarkerColor(i + 1);
			avg_N_h[i]->SetMarkerStyle(20);
			avg_N_h[i]->SetMarkerSize(1);
			avg_N_h[i]->GetXaxis()->SetTitle("N_{ch}");
			avg_N_h[i]->GetYaxis()->SetTitle("<N>");
			avg_N_h[i]->GetYaxis()->SetRangeUser(0, Nmax);// adjust y-axis range
			avg_N_h[i]->Draw("LP");

			leg_avgN->AddEntry(avg_N_h[i], Everything_Res->Get_Name(), "LP");
		}
		else {
			avg_N_h[i] = (TH1D*)JetResults[i - 1]->N_avg_hist->Clone();
			//avg_N_h[i]->SetTitle("");
			avg_N_h[i]->SetLineColor(i + 1);
			avg_N_h[i]->SetLineStyle(1);
			avg_N_h[i]->SetMarkerColor(i + 1);
			avg_N_h[i]->SetMarkerStyle(20);
			avg_N_h[i]->SetMarkerSize(1);
			avg_N_h[i]->Draw("LPsame");

			leg_avgN->AddEntry(avg_N_h[i], TString(Form("%d", 2 * (i - 1))) + "Jets", "LP");
		}
	}

	leg_avgN->Draw();

	//-------------------------------------------------------------------------------------------------------------
	//	<N^2> and <N>^2 vs Nch
	//-------------------------------------------------------------------------------------------------------------
	TCanvas *cNsq_Nch = new TCanvas("cNsq_Nch", "cNsq_Nch", 0, 0, xpix, ypix);
	cNsq_Nch->cd();

	TLegend* leg_avgNsq = new TLegend(0.8, 0.8, 1.0, 1.0);

	// clone and plot histograms
	for (int i = 0; i < 5; i++) {
		if (i == 0) {
			avg_Nsq_h[i] = (TH1D*)Everything_Res->Nsq_avg_hist->Clone();
			//avg_Nsq_h[i]->SetTitle("");
			avg_Nsq_h[i]->SetLineColor(i + 1);
			avg_Nsq_h[i]->SetLineStyle(1);
			avg_Nsq_h[i]->SetMarkerColor(i + 1);
			avg_Nsq_h[i]->SetMarkerStyle(20);
			avg_Nsq_h[i]->SetMarkerSize(1);
			avg_Nsq_h[i]->GetXaxis()->SetTitle("N_{ch}");
			avg_Nsq_h[i]->GetYaxis()->SetTitle("<N^{2}> and <N>^{2}");
			avg_Nsq_h[i]->GetYaxis()->SetRangeUser(0, 4000);// adjust y-axis range
			avg_Nsq_h[i]->Draw("LP");

			leg_avgNsq->AddEntry(avg_Nsq_h[i], Everything_Res->Get_Name(), "LP");

			N_avg_sq_h[i] = (TH1D*)Everything_Res->N_avg_sq_hist->Clone();
			//N_avg_sq_h[i]->SetTitle("");
			N_avg_sq_h[i]->SetLineColor(i + 1);
			N_avg_sq_h[i]->SetLineStyle(7);
			N_avg_sq_h[i]->SetLineWidth(2);
			N_avg_sq_h[i]->SetMarkerColor(i + 1);
			N_avg_sq_h[i]->SetMarkerStyle(24);
			N_avg_sq_h[i]->SetMarkerSize(1);
			N_avg_sq_h[i]->Draw("LPsame");
		}
		else {
			avg_Nsq_h[i] = (TH1D*)JetResults[i - 1]->Nsq_avg_hist->Clone();
			//avg_Nsq_h[i]->SetTitle("");
			avg_Nsq_h[i]->SetLineColor(i + 1);
			avg_Nsq_h[i]->SetLineStyle(1);
			avg_Nsq_h[i]->SetMarkerColor(i + 1);
			avg_Nsq_h[i]->SetMarkerStyle(20);
			avg_Nsq_h[i]->SetMarkerSize(1);
			avg_Nsq_h[i]->Draw("LPsame");

			leg_avgNsq->AddEntry(avg_Nsq_h[i], TString(Form("%d", 2 * (i - 1))) + "Jets", "LP");

			N_avg_sq_h[i] = (TH1D*)JetResults[i - 1]->N_avg_sq_hist->Clone();
			//N_avg_sq_h[i]->SetTitle("");
			N_avg_sq_h[i]->SetLineColor(i + 1);
			N_avg_sq_h[i]->SetLineStyle(7);
			N_avg_sq_h[i]->SetLineWidth(2);
			N_avg_sq_h[i]->SetMarkerColor(i + 1);
			N_avg_sq_h[i]->SetMarkerStyle(24);
			N_avg_sq_h[i]->SetMarkerSize(1);
			N_avg_sq_h[i]->Draw("LPsame");
		}
	}

	leg_avgNsq->Draw();


	//-------------------------------------------------------------------------------------------------------------
	//	<pt>*Var(N) vs Nch
	//-------------------------------------------------------------------------------------------------------------
	TCanvas *cptVarN_Nch = new TCanvas("cptVarN_Nch", "cptVarN_Nch", 0, 0, xpix, ypix);
	cptVarN_Nch->cd();

	TLegend* leg_cptVarN = new TLegend(0.8, 0.8, 1.0, 1.0);

	// clone and plot histograms
	for (int i = 0; i < 5; i++) {
		if (i == 0) {
			ptVarN_h[i] = (TH1D*)Everything_Res->ptAvgVarN_hist->Clone();
			//ptVarN_h[i]->SetTitle("");
			ptVarN_h[i]->SetLineColor(i + 1);
			ptVarN_h[i]->SetLineStyle(1);
			ptVarN_h[i]->SetMarkerColor(i + 1);
			ptVarN_h[i]->SetMarkerStyle(20);
			ptVarN_h[i]->SetMarkerSize(1);
			ptVarN_h[i]->GetXaxis()->SetTitle("N_{ch}");
			ptVarN_h[i]->GetYaxis()->SetTitle("<p_{T}>Var(N)");
			ptVarN_h[i]->GetYaxis()->SetRangeUser(0, 7);// adjust y-axis range
			ptVarN_h[i]->Draw("LP");

			leg_cptVarN->AddEntry(ptVarN_h[i], Everything_Res->Get_Name(), "LP");
		}
		else {
			ptVarN_h[i] = (TH1D*)JetResults[i - 1]->ptAvgVarN_hist->Clone();
			ptVarN_h[i]->SetLineColor(i + 1);
			ptVarN_h[i]->SetLineStyle(1);
			ptVarN_h[i]->SetMarkerColor(i + 1);
			ptVarN_h[i]->SetMarkerStyle(20);
			ptVarN_h[i]->SetMarkerSize(1);
			ptVarN_h[i]->Draw("LPsame");

			leg_cptVarN->AddEntry(ptVarN_h[i], TString(Form("%d", 2 * (i - 1))) + "Jets", "LP");
		}
	}

	leg_cptVarN->Draw();



	//-------------------------------------------------------------------------------------------------------------
	//	Cov(Pt,N) vs Nch
	//-------------------------------------------------------------------------------------------------------------
	TCanvas *cCovPtN_Nch = new TCanvas("cCovPtN_Nch", "cCovPtN_Nch", 0, 0, xpix, ypix);
	cCovPtN_Nch->cd();

	TLegend* leg_CovPtN = new TLegend(0.8, 0.8, 1.0, 1.0);

	// clone and plot histograms
	for (int i = 0; i < 5; i++) {
		if (i == 0) {
			CovPtN_h[i] = (TH1D*)Everything_Res->CovPtN_hist->Clone();
			//CovPtN_h[i]->SetTitle("");
			CovPtN_h[i]->SetLineColor(i + 1);
			CovPtN_h[i]->SetLineStyle(1);
			CovPtN_h[i]->SetMarkerColor(i + 1);
			CovPtN_h[i]->SetMarkerStyle(20);
			CovPtN_h[i]->SetMarkerSize(1);
			CovPtN_h[i]->GetXaxis()->SetTitle("N_{ch}");
			CovPtN_h[i]->GetYaxis()->SetTitle("Cov(Pt,N)");
			CovPtN_h[i]->GetYaxis()->SetRangeUser(0, 6);// adjust y-axis range
			CovPtN_h[i]->Draw("LP");

			leg_CovPtN->AddEntry(CovPtN_h[i], Everything_Res->Get_Name(), "LP");
		}
		else {
			CovPtN_h[i] = (TH1D*)JetResults[i - 1]->CovPtN_hist->Clone();
			CovPtN_h[i]->SetLineColor(i + 1);
			CovPtN_h[i]->SetLineStyle(1);
			CovPtN_h[i]->SetMarkerColor(i + 1);
			CovPtN_h[i]->SetMarkerStyle(20);
			CovPtN_h[i]->SetMarkerSize(1);
			CovPtN_h[i]->Draw("LPsame");

			leg_CovPtN->AddEntry(CovPtN_h[i], TString(Form("%d", 2 * (i - 1))) + "Jets", "LP");
		}
	}

	leg_CovPtN->Draw();


	//-------------------------------------------------------------------------------------------------------------
	//	D vs Nch
	//-------------------------------------------------------------------------------------------------------------
	TCanvas *cD_Nch = new TCanvas("cD_Nch", "cD_Nch", 0, 0, xpix, ypix);
	cD_Nch->cd();

	TLegend* leg_D = new TLegend(0.8, 0.8, 1.0, 1.0);

	// clone and plot histograms
	for (int i = 0; i < 5; i++) {
		if (i == 0) {
			D_h[i] = (TH1D*)Everything_Res->D_hist->Clone();
			//D_h[i]->SetTitle("");
			D_h[i]->SetLineColor(i + 1);
			D_h[i]->SetLineStyle(1);
			D_h[i]->SetMarkerColor(i + 1);
			D_h[i]->SetMarkerStyle(20);
			D_h[i]->SetMarkerSize(1);
			D_h[i]->GetXaxis()->SetTitle("N_{ch}");
			D_h[i]->GetYaxis()->SetTitle("D");
			D_h[i]->GetYaxis()->SetRangeUser(-0.0015, 0.0085);// adjust y-axis range
			D_h[i]->Draw("LP");

			leg_D->AddEntry(D_h[i], Everything_Res->Get_Name(), "LP");
		}
		else {
			D_h[i] = (TH1D*)JetResults[i - 1]->D_hist->Clone();
			D_h[i]->SetLineColor(i + 1);
			D_h[i]->SetLineStyle(1);
			D_h[i]->SetMarkerColor(i + 1);
			D_h[i]->SetMarkerStyle(20);
			D_h[i]->SetMarkerSize(1);
			D_h[i]->Draw("LPsame");

			leg_D->AddEntry(D_h[i], TString(Form("%d", 2 * (i - 1))) + "Jets", "LP");
		}
	}

	leg_D->Draw();


	//-------------------------------------------------------------------------------------------------------------
	//	R vs Nch
	//-------------------------------------------------------------------------------------------------------------
	TCanvas *cR_Nch = new TCanvas("cR_Nch", "cR_Nch", 0, 0, xpix, ypix);
	cR_Nch->cd();

	TLegend* leg_R = new TLegend(0.8, 0.8, 1.0, 1.0);

	// clone and plot histograms
	for (int i = 0; i < 5; i++) {
		if (i == 0) {
			R_h[i] = (TH1D*)Everything_Res->R_hist->Clone();
			//R_h[i]->SetTitle("");
			R_h[i]->SetLineColor(i + 1);
			R_h[i]->SetLineStyle(1);
			R_h[i]->SetLineWidth(2);
			R_h[i]->SetMarkerColor(i + 1);
			R_h[i]->SetMarkerStyle(20);
			R_h[i]->SetMarkerSize(1);
			R_h[i]->GetXaxis()->SetTitle("N_{ch}");
			R_h[i]->GetYaxis()->SetTitle("R");
			R_h[i]->GetYaxis()->SetRangeUser(-0.05, 0.07);// adjust y-axis range
			R_h[i]->Draw("LP");

			leg_R->AddEntry(R_h[i], Everything_Res->Get_Name(), "LP");
		}
		else {
			R_h[i] = (TH1D*)JetResults[i - 1]->R_hist->Clone();
			R_h[i]->SetLineColor(i + 1);
			R_h[i]->SetLineStyle(1);
			R_h[i]->SetMarkerColor(i + 1);
			R_h[i]->SetMarkerStyle(20);
			R_h[i]->SetMarkerSize(1);
			R_h[i]->Draw("LPsame");

			leg_R->AddEntry(R_h[i], TString(Form("%d", 2 * (i - 1))) + "Jets", "LP");
		}
	}

	leg_R->Draw();






	// write root object to root file
	cNev_Nch->Write();
	cpt_Nch->Write();
	cVarN_Nch->Write();
	cN_Nch->Write();
	cNsq_Nch->Write();
	cptVarN_Nch->Write();
	cCovPtN_Nch->Write();
	cD_Nch->Write();
	cR_Nch->Write();

	return 0;
}