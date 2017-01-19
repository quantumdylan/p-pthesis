#include <string>
#include <sstream>
#include "stdio.h"
#include <iostream>
#include <vector>

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

//George's custom classes
#include "GParticle.h"
#include "GEvent.h"

using namespace std;

int main(int argc, const char** argv) {
	string prefixes[5] = { "Everything_", "NumJets0_", "NumJets2_", "NumJets4_", "NumJets6_" }; //useful for not having to constantly type things out
	string hist_topic[4] = { "N_eve_hists", "pt_avg_hists", "D_hists", "R_hists" };
	string y_axis_labels[4] = { "N_{ev}", "p_{T}", "D", "R" };
	
	//-----------------------------------------------------------------------------
	//	Take input arguments from the script
	//-----------------------------------------------------------------------------
	//find the paths to the input and output file(s)
	TString location = argv[1]; //input the input path
	cout << "File path: " + location << endl;
	TString output = argv[2]; //input the output path
	cout << "Output path: " + output << endl;

	//prefix for histograms that will be created later
	string title = argv[3]; //for naming the hists
	cout << "Title prefix: " + title << endl << endl;

	int i = 4; //offset for the previous argvs
	int total_files = 0; //counter for the total number of files
	vector<TString> file_locations; //vector storing the file locations

	//loop through the remaining argv values, which will give us the filenames we want to load
	while (argv[i] != NULL) {
		total_files = i - 3; //set total files to last checked i, so we don't go one over
		file_locations.push_back(argv[i]);
		cout << "Index: " << total_files << "   " << "File: " + file_locations[i - 4] << endl; //god I'm dumb, can't access random memory data lol
		i++; //increment the counter
	}

	cout << "Total files: " << total_files << endl << endl << endl;
	cout << "-----------------------------------------------------------------------------------" << endl << endl << endl;


	//-----------------------------------------------------------------------------
	//	Declare the histograms
	//-----------------------------------------------------------------------------
	vector<TH1D*> Everything_N_eve_hists; //individual jet content hist vectors
	vector<TH1D*> NumJets0_N_eve_hists;
	vector<TH1D*> NumJets2_N_eve_hists;
	vector<TH1D*> NumJets4_N_eve_hists;
	vector<TH1D*> NumJets6_N_eve_hists;

	vector<vector<TH1D*>> N_eve_hists; //full all file all content hist vector vectors

	vector<TH1D*> Everything_pt_avg_hists;
	vector<TH1D*> NumJets0_pt_avg_hists;
	vector<TH1D*> NumJets2_pt_avg_hists;
	vector<TH1D*> NumJets4_pt_avg_hists;
	vector<TH1D*> NumJets6_pt_avg_hists;

	vector<vector<TH1D*>> pt_avg_hists;

	vector<TH1D*> Everything_D_hists;
	vector<TH1D*> NumJets0_D_hists;
	vector<TH1D*> NumJets2_D_hists;
	vector<TH1D*> NumJets4_D_hists;
	vector<TH1D*> NumJets6_D_hists;

	vector<vector<TH1D*>> D_hists;

	vector<TH1D*> Everything_R_hists;
	vector<TH1D*> NumJets0_R_hists;
	vector<TH1D*> NumJets2_R_hists;
	vector<TH1D*> NumJets4_R_hists;
	vector<TH1D*> NumJets6_R_hists;

	vector<vector<TH1D*>> R_hists;

	vector<vector<vector<TH1D*>>> all_hists;

	//-----------------------------------------------------------------------------
	//	Declare the histograms
	//-----------------------------------------------------------------------------
	vector<TFile*> files; //vector containing files to open
	for (int i = 0; i < file_locations.size(); i++) {
		TFile *tmp_file = TFile::Open(location + file_locations[i] + ".root"); //open a temporary file

		//as long as the file opens correctly, add the file to our TFile stack
		if (tmp_file->IsOpen()) {
			cout << "File " + location + file_locations[i] + ".root is opened, yaaaaay" << endl << endl;
			cout << endl << "Performing test for histograms..." << endl;
			tmp_file->ls();
			cout << endl << endl << "End test for histograms." << endl << endl << endl;
		}

		files.push_back(tmp_file); //pushback the temp file
	}


	//-----------------------------------------------------------------------------
	//	Fetch the histograms
	//-----------------------------------------------------------------------------
	for (int i = 0; i < file_locations.size(); i++) {
		//Generate the temp hists to be added to the stack
		TH1D *temp_Everything_N_hist = (TH1D*)files[i]->Get("Everything_N_eve_hist");
		TH1D *temp_NumJets0_N_hist = (TH1D*)files[i]->Get("NumJets0_N_eve_hist");
		TH1D *temp_NumJets2_N_hist = (TH1D*)files[i]->Get("NumJets2_N_eve_hist");
		TH1D *temp_NumJets4_N_hist = (TH1D*)files[i]->Get("NumJets4_N_eve_hist");
		TH1D *temp_NumJets6_N_hist = (TH1D*)files[i]->Get("NumJets6_N_eve_hist");

		TH1D *temp_Everything_pt_hist = (TH1D*)files[i]->Get("Everything_pt_avg_hist");
		TH1D *temp_NumJets0_pt_hist = (TH1D*)files[i]->Get("NumJets0_pt_avg_hist");
		TH1D *temp_NumJets2_pt_hist = (TH1D*)files[i]->Get("NumJets2_pt_avg_hist");
		TH1D *temp_NumJets4_pt_hist = (TH1D*)files[i]->Get("NumJets4_pt_avg_hist");
		TH1D *temp_NumJets6_pt_hist = (TH1D*)files[i]->Get("NumJets6_pt_avg_hist");

		TH1D *temp_Everything_D_hist = (TH1D*)files[i]->Get("Everything_D_hist");
		TH1D *temp_NumJets0_D_hist = (TH1D*)files[i]->Get("NumJets0_D_hist");
		TH1D *temp_NumJets2_D_hist = (TH1D*)files[i]->Get("NumJets2_D_hist");
		TH1D *temp_NumJets4_D_hist = (TH1D*)files[i]->Get("NumJets4_D_hist");
		TH1D *temp_NumJets6_D_hist = (TH1D*)files[i]->Get("NumJets6_D_hist");

		TH1D *temp_Everything_R_hist = (TH1D*)files[i]->Get("Everything_R_hist");
		TH1D *temp_NumJets0_R_hist = (TH1D*)files[i]->Get("NumJets0_R_hist");
		TH1D *temp_NumJets2_R_hist = (TH1D*)files[i]->Get("NumJets2_R_hist");
		TH1D *temp_NumJets4_R_hist = (TH1D*)files[i]->Get("NumJets4_R_hist");
		TH1D *temp_NumJets6_R_hist = (TH1D*)files[i]->Get("NumJets6_R_hist");


		//Add temp hists to the stack
		Everything_N_eve_hists.push_back(temp_Everything_N_hist);
		NumJets0_N_eve_hists.push_back(temp_NumJets0_N_hist);
		NumJets2_N_eve_hists.push_back(temp_NumJets2_N_hist);
		NumJets4_N_eve_hists.push_back(temp_NumJets4_N_hist);
		NumJets6_N_eve_hists.push_back(temp_NumJets6_N_hist);

		Everything_pt_avg_hists.push_back(temp_Everything_pt_hist);
		NumJets0_pt_avg_hists.push_back(temp_NumJets0_pt_hist);
		NumJets2_pt_avg_hists.push_back(temp_NumJets2_pt_hist);
		NumJets4_pt_avg_hists.push_back(temp_NumJets4_pt_hist);
		NumJets6_pt_avg_hists.push_back(temp_NumJets6_pt_hist);

		Everything_D_hists.push_back(temp_Everything_D_hist);
		NumJets0_D_hists.push_back(temp_NumJets0_D_hist);
		NumJets2_D_hists.push_back(temp_NumJets2_D_hist);
		NumJets4_D_hists.push_back(temp_NumJets4_D_hist);
		NumJets6_D_hists.push_back(temp_NumJets6_D_hist);

		Everything_R_hists.push_back(temp_Everything_R_hist);
		NumJets0_R_hists.push_back(temp_NumJets0_R_hist);
		NumJets2_R_hists.push_back(temp_NumJets2_R_hist);
		NumJets4_R_hists.push_back(temp_NumJets4_R_hist);
		NumJets6_R_hists.push_back(temp_NumJets6_R_hist);
	}

	cout << "Passed initial hist fetch" << endl;
	for (int i = 0; i < 5; i++) {
		switch (i) {
		case 0: N_eve_hists.push_back(Everything_N_eve_hists); pt_avg_hists.push_back(Everything_pt_avg_hists); D_hists.push_back(Everything_D_hists); R_hists.push_back(Everything_R_hists); break;
		case 1: N_eve_hists.push_back(NumJets0_N_eve_hists); pt_avg_hists.push_back(NumJets0_pt_avg_hists); D_hists.push_back(NumJets0_D_hists); R_hists.push_back(NumJets0_R_hists); break;
		case 2: N_eve_hists.push_back(NumJets2_N_eve_hists); pt_avg_hists.push_back(NumJets2_pt_avg_hists); D_hists.push_back(NumJets2_D_hists); R_hists.push_back(NumJets2_R_hists); break;
		case 3: N_eve_hists.push_back(NumJets4_N_eve_hists); pt_avg_hists.push_back(NumJets4_pt_avg_hists); D_hists.push_back(NumJets4_D_hists); R_hists.push_back(NumJets4_R_hists); break;
		case 4: N_eve_hists.push_back(NumJets6_N_eve_hists); pt_avg_hists.push_back(NumJets6_pt_avg_hists); D_hists.push_back(NumJets6_D_hists); R_hists.push_back(NumJets6_R_hists); break;
		default: cout << "Error creating congregate hist vector arrays";
		}
		cout << "Current loop for adding hists to stack: " << i << endl;
	}
	cout << endl << endl;

	all_hists.push_back(N_eve_hists);
	all_hists.push_back(pt_avg_hists);
	all_hists.push_back(D_hists);
	all_hists.push_back(R_hists);
	//histogram vectors are now in this format: temp_hist[x][y] where x is the jet content id and y is the file id
	//all_hists contains all histograms in this format: all_hists[x][y][z] where x is the histogram type (N, pt, D, etc.) y is the jet content and z is the file id

	//-----------------------------------------------------------------------------
	//	Create root file to store plots and histograms
	//-----------------------------------------------------------------------------
	cout << "Output file opening as: " + output + title + "_Comparison.root" << endl;
	TFile outRoot(output + title + "_Comparison.root", "RECREATE");


	//set the standard for our histogram plots
	int xpix = 500, ypix = 500;


	//-----------------------------------------------------------------------------
	//	Create the empty plots and their legends
	//-----------------------------------------------------------------------------
	//start with total plots and legends
	TCanvas *cNev_TOT_Nch = new TCanvas("cNev_TOT_Nch", "cNev_TOT_Nch", 0, 0, xpix, ypix);// create the N_eve canvas
	TCanvas *cpt_TOT_Nch = new TCanvas("cpt_TOT_Nch", "cpt_TOT_Nch", 0, 0, xpix, ypix);// create the pt canvas
	TCanvas *cD_TOT_Nch = new TCanvas("cD_TOT_Nch", "cD_TOT_Nch", 0, 0, xpix, ypix);// create the D canvas
	TCanvas *cR_TOT_Nch = new TCanvas("cR_TOT_Nch", "cR_TOT_Nch", 0, 0, xpix, ypix);// create the R canvas

	TLegend* leg_Nev_TOT = new TLegend(0.8, 0.8, 1.0, 1.0);
	TLegend* leg_pt_TOT = new TLegend(0.8, 0.8, 1.0, 1.0);
	TLegend* leg_D_TOT = new TLegend(0.8, 0.8, 1.0, 1.0);
	TLegend* leg_R_TOT = new TLegend(0.8, 0.8, 1.0, 1.0);

	vector<TCanvas*> total_plots;
	vector<TLegend*> total_leg;

	total_plots.push_back(cNev_TOT_Nch);
	total_plots.push_back(cpt_TOT_Nch);
	total_plots.push_back(cD_TOT_Nch);
	total_plots.push_back(cR_TOT_Nch);

	total_leg.push_back(leg_Nev_TOT);
	total_leg.push_back(leg_pt_TOT);
	total_leg.push_back(leg_D_TOT);
	total_leg.push_back(leg_R_TOT);
	cout << "Added total plots and legends" << endl;

	//next the vectors for the file dependent combination plots
	vector<TCanvas*> cNev_FILE_Nch;
	vector<TCanvas*> cpt_FILE_Nch;
	vector<TCanvas*> cD_FILE_Nch;
	vector<TCanvas*> cR_FILE_Nch;

	vector<TLegend*> leg_Nev_FILE;
	vector<TLegend*> leg_pt_FILE;
	vector<TLegend*> leg_D_FILE;
	vector<TLegend*> leg_R_FILE;

	//and finally the jet content dependent combination plots
	vector<TCanvas*> cNev_INDJET_Nch;
	vector<TCanvas*> cpt_INDJET_Nch;
	vector<TCanvas*> cD_INDJET_Nch;
	vector<TCanvas*> cR_INDJET_Nch;

	vector<TLegend*> leg_Nev_INDJET;
	vector<TLegend*> leg_pt_INDJET;
	vector<TLegend*> leg_D_INDJET;
	vector<TLegend*> leg_R_INDJET;

	//fill the combination plot/legend containers
	for (int i = 0; i < file_locations.size(); i++) {
		//clunky as fuck but it works
		stringstream convert;
		string filename;
		convert << i + 1;
		filename = convert.str();
		convert.clear();

		string NevT = "cNev_FILE_" + filename + "_Nch";
		string ptT = "cpt_FILE_" + filename + "_Nch";
		string DT = "cD_FILE_" + filename + "_Nch";
		string RT = "cR_FILE_" + filename + "_Nch";

		TCanvas *NevTemp = new TCanvas(NevT.c_str(), NevT.c_str(), 0, 0, xpix, ypix);
		TCanvas *ptTemp = new TCanvas(ptT.c_str(), ptT.c_str(), 0, 0, xpix, ypix);
		TCanvas *DTemp = new TCanvas(DT.c_str(), DT.c_str(), 0, 0, xpix, ypix);
		TCanvas *RTemp = new TCanvas(RT.c_str(), RT.c_str(), 0, 0, xpix, ypix);

		TLegend* leg_Nev_temp = new TLegend(0.8, 0.8, 1.0, 1.0);
		TLegend* leg_pt_temp = new TLegend(0.8, 0.8, 1.0, 1.0);
		TLegend* leg_D_temp = new TLegend(0.8, 0.8, 1.0, 1.0);
		TLegend* leg_R_temp = new TLegend(0.8, 0.8, 1.0, 1.0);

		cNev_FILE_Nch.push_back(NevTemp);
		cpt_FILE_Nch.push_back(ptTemp);
		cD_FILE_Nch.push_back(DTemp);
		cR_FILE_Nch.push_back(RTemp);

		leg_Nev_FILE.push_back(leg_Nev_temp);
		leg_pt_FILE.push_back(leg_pt_temp);
		leg_D_FILE.push_back(leg_D_temp);
		leg_R_FILE.push_back(leg_R_temp);
	}
	cout << "Completed FILE plot decs" << endl;
	//0 - everything, 1 - 0-dijet, 2 - 1-dijet, 3 - 2-dijet, 4 - 3-dijet
	for (int i = 0; i < 5; i++) {
		string title;
		switch (i) {
		case 0: title = "_Everything"; break;
		case 1: title = "_0-Dijets"; break;
		case 2: title = "_1-Dijet"; break;
		case 3: title = "_2-Dijet"; break;
		case 4: title = "_3-Dijet"; break;
		default: title = "I FUCKED UP";
		}

		string NevT = "cNev_INDJET_" + title + "_Nch";
		string ptT = "cpt_INDJET_" + title + "_Nch";
		string DT = "cD_INDJET_" + title + "_Nch";
		string RT = "cR_INDJET_" + title + "_Nch";

		TCanvas *NevTemp = new TCanvas(NevT.c_str(), NevT.c_str(), 0, 0, xpix, ypix);
		TCanvas *ptTemp = new TCanvas(ptT.c_str(), ptT.c_str(), 0, 0, xpix, ypix);
		TCanvas *DTemp = new TCanvas(DT.c_str(), DT.c_str(), 0, 0, xpix, ypix);
		TCanvas *RTemp = new TCanvas(RT.c_str(), RT.c_str(), 0, 0, xpix, ypix);

		TLegend* leg_Nev_temp = new TLegend(0.8, 0.8, 1.0, 1.0);
		TLegend* leg_pt_temp = new TLegend(0.8, 0.8, 1.0, 1.0);
		TLegend* leg_D_temp = new TLegend(0.8, 0.8, 1.0, 1.0);
		TLegend* leg_R_temp = new TLegend(0.8, 0.8, 1.0, 1.0);

		cNev_INDJET_Nch.push_back(NevTemp);
		cpt_INDJET_Nch.push_back(ptTemp);
		cD_INDJET_Nch.push_back(DTemp);
		cR_INDJET_Nch.push_back(RTemp);

		leg_Nev_INDJET.push_back(leg_Nev_temp);
		leg_pt_INDJET.push_back(leg_pt_temp);
		leg_D_INDJET.push_back(leg_D_temp);
		leg_R_INDJET.push_back(leg_R_temp);
	}
	cout << "Completed INDJET plot decs" << endl;
	//combine plots for ease of automation
	vector<vector<TCanvas*>> file_plots;
	vector<vector<TCanvas*>> indjet_plots;
	vector<vector<vector<TCanvas*>>> all_plots;

	file_plots.push_back(cNev_FILE_Nch);
	file_plots.push_back(cpt_FILE_Nch);
	file_plots.push_back(cD_FILE_Nch);
	file_plots.push_back(cR_FILE_Nch);
	indjet_plots.push_back(cNev_INDJET_Nch);
	indjet_plots.push_back(cpt_INDJET_Nch);
	indjet_plots.push_back(cD_INDJET_Nch);
	indjet_plots.push_back(cR_INDJET_Nch);

	all_plots.push_back(file_plots);
	all_plots.push_back(indjet_plots);

	vector<vector<TLegend*>> file_leg;
	vector<vector<TLegend*>> indjet_leg;
	vector<vector<vector<TLegend*>>> all_leg;

	file_leg.push_back(leg_Nev_FILE);
	file_leg.push_back(leg_pt_FILE);
	file_leg.push_back(leg_D_FILE);
	file_leg.push_back(leg_R_FILE);
	indjet_leg.push_back(leg_Nev_INDJET);
	indjet_leg.push_back(leg_pt_INDJET);
	indjet_leg.push_back(leg_D_INDJET);
	indjet_leg.push_back(leg_R_INDJET);

	//all_leg[x][y]: x is file and indjet discrimination, y is plot type;
	all_leg.push_back(file_leg);
	all_leg.push_back(indjet_leg);
	cout << "Finished with all_plots and all_leg" << endl;

	//-----------------------------------------------------------------------------
	//	Prepare histograms
	//-----------------------------------------------------------------------------
	for (int i = 0; i < file_locations.size(); i++) { //we loop through all files
		for (int j = 0; j < 5; j++) { //and also through all jet contents
			//global rules for line/marker styling
			int LC = j + 2; //LINE COLOR: offset of 2 to prevent invisible lines (hopefully)

			int LS = ((j + 1) + i * 5) % 11; //LINE STYLE: offset of 1, rolls around at 11

			int LW = 2; //LINE WIDTH: constant of 2 for now

			int MC = ((j + 1) + i * 5) % 40; //MARKER COLOR: offset of 1, rolls around at 40

			int MS = ((j + 1) + i * 5) % 20; //MARKER STYLE: offset of 1, rolls around at 20

			int MSZ = 1; //MARKER SIZE: constant of 1 for now

			//all_hists contains all histograms in this format: all_hists[x][y][z] where x is the histogram type (N, pt, D, etc.) y is the jet content and z is the file id
			//setting histogram style settings for all plots
			for (int h = 0; h < 4; h++) {
				all_hists[h][j][i]->SetLineColor(LC);
				all_hists[h][j][i]->SetLineColor(LC);
				all_hists[h][j][i]->SetLineWidth(LW);
				all_hists[h][j][i]->SetMarkerColor(MC);
				all_hists[h][j][i]->SetMarkerStyle(MS);
				all_hists[h][j][i]->SetMarkerSize(MSZ);

				//cout << "Made it through hist setting" << endl;
				string title = prefixes[j];
				title.append(hist_topic[h]);
				all_hists[h][j][i]->SetTitle(title.c_str());

				if (i == 0) { //initial conditions for plots
					all_hists[h][j][i]->GetXaxis()->SetTitle("N_{ch}");
					all_hists[h][j][i]->GetYaxis()->SetTitle(y_axis_labels[h].c_str());
					if (h == 0) { //range for N_eve
						all_hists[h][j][i]->GetYaxis()->SetRangeUser(1, 1e8);
						gPad->SetLogy();
					}
					else if (h == 1) //range for pt
						all_hists[h][j][i]->GetYaxis()->SetRangeUser(0, 2.2);
					else //range for D and R
						all_hists[h][j][i]->GetYaxis()->SetRangeUser(-0.25, 0.25);
				}
			}
		}
	}

	cout << "Made it through hist prep" << endl;
	//-----------------------------------------------------------------------------
	//	Draw histograms to plots
	//-----------------------------------------------------------------------------
	//plot histograms to their respective vector positions
	//all_leg[x][y]: x is plot type (INDJET, etc), y is hist type (Nev, pt, etc)
	//all_plot[x][y]: same
	//all_hists[x][y][z]: x is hist type, y is jet content, z is file id
	//p is plot type, h is hist type
	//p: TOT, FILE, INDJET
	//h: Neve, pt, D
	for (int p = 0; p < 3; p++) {
		for (int h = 0; h < 4; h++) {
			if (p == 0) { //total loop
				//cout << "Entered total loop for " << h << " time" << endl;
				total_plots[h]->cd(); //select the plot to do (dependent on only histogram type)

				for (int f = 0; f < file_locations.size(); f++) {
					for (int j = 0; j < 5; j++) { //loops through everything and plots everything to one plot
						if (h == 0) //first loop initial draw (dependent on entire histogram)
							all_hists[h][j][f]->Draw("LP");
						else
							all_hists[h][j][f]->Draw("LPsame"); //draw all other hists in this plot with the same settings

						string title = prefixes[j];
						title.append(hist_topic[h]);
						
						total_leg[h]->AddEntry(all_hists[h][j][f], title.c_str(), "LP"); //add this legend entry
					}
				}
				//end of TOTAL plots
			}
			else if (p == 1) { //file loop
				for (int f = 0; f < file_locations.size(); f++) { //loops through jet content while file id is constant
					//cout << "This next part will probably reaaaaally fuck up" << endl;
					all_plots[p-1][h][f]->cd(); //select the plot (dependent on file id and histogram type)
					for (int j = 0; j < 5; j++) {
						if (j == 0) //first loop initial draw (dependent on jet content)
							all_hists[h][j][f]->Draw("LP");
						else
							all_hists[h][j][f]->Draw("LPsame"); //draw all other hists in this plot with the same settings

						string title = prefixes[j]; //create a title for the legend
						title.append(hist_topic[h]);
						
						all_leg[p-1][h][f]->AddEntry(all_hists[h][j][f], title.c_str(), "LP"); //add this legend entry
					}
				}
			}
			else if (p == 2) {  //indjet loop
				for (int j = 0; j < 5; j++) { //loops through files while jet content is constant
					//cout << "This next part will probably reaaaaally reaaaaaaaaaaaaaally fuck up" << endl;
					all_plots[p - 1][h][j]->cd(); //select the plot (dependent on jet content and histogram type)
					for (int f = 0; f < file_locations.size(); f++) {
						if (f == 0) //first loop initial draw (dependent on file id)
							all_hists[h][j][f]->Draw("LP");
						else
							all_hists[h][j][f]->Draw("LPsame"); //draw all other hists in this plot with the same settings

						string title = prefixes[j]; //create a title for the legend
						title.append(hist_topic[h]);

						all_leg[p - 1][h][j]->AddEntry(all_hists[h][j][f], title.c_str(), "LP"); //add this legend entry
					}
				}
			}
		}
	}


	//-----------------------------------------------------------------------------
	//	Write plots and close file
	//-----------------------------------------------------------------------------
	cout << "Writing plots to file" << endl;
	cout << "Writing total plots..." << endl;
	for (int t = 0; t < 4; t++) {
		total_plots[t]->cd();
		total_leg[t]->Draw();
		total_plots[t]->Write();
		TString filepath(output + "TOTAL_PLOT_" + hist_topic[t] + ".pdf");
		total_plots[t]->Print(filepath, "pdf");
	}
	cout << "Finished writing total plots." << endl << endl;

	cout << "Writing file plots..." << endl;
	for (int f = 0; f < file_locations.size(); f++) {
		for (int h = 0; h < 4; h++) {
			all_plots[0][h][f]->cd();
			all_leg[0][h][f]->Draw();
			all_plots[0][h][f]->Write();
			TString filepath(output + "FILEid_" + TString(Form("%d", f)) + "_PLOT_" + hist_topic[h] + ".pdf");
			all_plots[0][h][f]->Print(filepath, "pdf");
		}
	}
	cout << "Finished writing file plots." << endl << endl;

	cout << "Writing indjet plots..." << endl;
	for (int j = 0; j < 5; j++) {
		for (int h = 0; h < 4; h++) {
			all_plots[1][h][j]->cd();
			all_leg[1][h][j]->Draw();
			all_plots[1][h][j]->Write();
			TString filepath(output + "INDJET_" + prefixes[j] + "_PLOT_" + hist_topic[h] + ".pdf");
			all_plots[1][h][j]->Print(filepath, "pdf");
		}
	}
	cout << "Finished writing indjet plots." << endl;

	outRoot.Write();

	outRoot.Close();

	//to print to pdf: take canvas->Print(filepath+filename+".pdf", "pdf")

	return 0;
}