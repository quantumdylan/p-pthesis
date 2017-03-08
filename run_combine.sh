#!/bin/bash
#
# 
#run_combine.sh
#
#       simple script to run combine
# 

#
#prefix
prefix="R6NOCHARGE"
#location to place .root file with pythia events
res_loc="/media/scratch/packard/db/RChange/"
#the build location for the RunPythia program
pgm_loc="/home/packard/Dropbox/packard_SeniorProj/moschelli/p-pthesis/"
#the location to copy output (default: NULL)
out_loc="/media/scratch/packard/db/"
#log location
log_loc="/home/packard/Dropbox/packard_SeniorProj/moschelli/p-pthesis/logs/Combinations/"
#the name of the .root file that stores final events
filename="pp10mEv_7TeV_ptHatMin0GeV"
#filename="test"
#ipass limiter (Max number of passes)
ipass=1

#number of computer cores analysis ran on
nCores=9

cd $out_loc
rm *.pdf

cd $pgm_loc

./bin/combine $res_loc $prefix $filename $out_loc $log_loc $ipass $nCores > $log_loc$prefix$filename'_ANALYZED.log' &


#cp $res_loc$filename'_analysis.log' $pgm_loc/logs

sleep 1

cp $out_loc$prefix$filename'_COMBINED.dat' $pgm_loc/discrete
rm $pgm_loc'data/*.pdf'
cp $out_loc*.pdf $pgm_loc'data'