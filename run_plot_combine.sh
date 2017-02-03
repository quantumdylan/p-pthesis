#!/bin/bash
#
#	run_analysis.sh
#
#       simple script to run RunPythia
#
#
#location to place .root file with pythia events
res_loc="/media/scratch/packard/db/TEST"
#the build location for the RunPythia program
pgm_loc="/home/packard/Dropbox/packard_SeniorProj/moschelli/p-pthesis/"
#the location to copy output (default: NULL)
out_loc="/media/scratch/packard/db/"
#pdf location
pdf_loc="/home/packard/Dropbox/packard_SeniorProj/moschelli/p-pthesis/data/"
#log location
log_loc="/home/packard/Dropbox/packard_SeniorProj/moschelli/p-pthesis/logs/Combinations/"
#the name of the .root file that stores final events
filename="MOREBINS_Combination_R4"
#prefix title for hists
title="MOREBINS_R1_R4_R6_R8"
#FILES (add more as needed):
file1="R1pp10mEv_7TeV_ptHatMin0GeV_COMBINED"
file2="R2pp10mEv_7TeV_ptHatMin0GeV_COMBINED"
file3="R3pp10mEv_7TeV_ptHatMin0GeV_COMBINED"
file4="R4pp10mEv_7TeV_ptHatMin0GeV_COMBINED"
file5="testR6pp10mEv_7TeV_ptHatMin0GeV_COMBINED"
file6="testR8pp10mEv_7TeV_ptHatMin0GeV_COMBINED"

cd $pgm_loc
./bin/PlotStuff $res_loc $out_loc $title $file4 > $log_loc$title'_Comparison.log'

cd $pdf_loc
#rm *.pdf

cd $out_loc
cp *.pdf $pdf_loc
rm *.pdf