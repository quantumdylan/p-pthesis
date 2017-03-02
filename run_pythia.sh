#!/bin/bash
#
#	run_pythia.sh
#
#       simple script to run RunPythia
#	
#
#location to place .root file with pythia events
res_loc="/media/scratch/packard/db/"
#the build location for the RunPythia program
pgm_loc="/home/packard/Dropbox/packard_SeniorProj/moschelli/p-pthesis/"
#number of events
nEv=10000000
#center of mass collision energy in [GeV]
sqrts=7000
#kinimatic cut of hard scatterings at generation level in [GeV]
ptHatMin=0.0
ptHatMax=1000000.0
#the name of the .root file that stores final events
filename="pp10mEv_7TeV_ptHatMin0GeV_run"
#filename="test"
#number of computer cores to run RunPythia on
nCores=30

cd $pgm_loc
#./bin/RunPythia $nEv $sqrts $ptHatMin $ptHatMax $res_loc $filename > $res_loc$filename'.log' &
for(( n=0; n<nCores; n++)) 
do
	./bin/RunPythia $nEv $sqrts $ptHatMin $ptHatMax $res_loc $filename$n > $res_loc$filename$n'.log' &
	#need time to get a new random seed
	sleep 1
done