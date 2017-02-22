#!/bin/bash
#
# 
#run_analysis.sh
#
#       simple script to run RunPythia
# 

#
#prefix
prefix="R4"
#location to place .root file with pythia events
res_loc="/media/external1/packard/db/RChange/"
#the build location for the RunPythia program
pgm_loc="/home/packard/Dropbox/packard_SeniorProj/moschelli/p-pthesis/"
#the location to copy output (default: NULL)
out_loc="/media/scratch/packard/db/RChange/"
#final location from combiner
final_loc="/media/scratch/packard/db/"
#log location
log_loc="/home/packard/Dropbox/packard_SeniorProj/moschelli/p-pthesis/logs/ROOT/"
#the name of the .root file that stores final events
filename="pp10mEv_7TeV_ptHatMin0GeV_run"
#filename="test"
#number of events (comment out to default)
numevents=1000
#ipass limiter (Max number of passes)
ipass=1

#number of computer cores to run RunPythia on
#nCores=10

#cd $pgm_loc
#./bin/analysis $nCores $res_loc$prefix $filename $out_loc $numevents > $log_loc$filename'_'$prefix'analysis.log' &

nCores=9

cd $pgm_loc
for(( n=0; n<nCores; n++)) 
do
 
./bin/analysis $n $res_loc$prefix $filename $out_loc$prefix $numevents 1 $ipass > $log_loc$filename'_'$prefix'_run'$n'_ANALYZED.log' &
done

#./bin/analysis_combine $n $out_loc$prefix $filename $final_loc$prefix $numevents 4 $ipass > $log_loc$filename'_'$prefix'_COMBINED.log' &

#cp $res_loc$filename'_analysis.log' $pgm_loc/logs
#cp $res_loc$filename'_RESULTS.dat' $pgm_loc/data