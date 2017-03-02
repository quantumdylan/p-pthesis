#!/bin/bash
#
#	run_fastjet.sh
#
#       simple script to run RunFastjet
#	
#
#location to place .root file with pythia events
res_loc="/media/scratch/packard/db/"
#the build location for the RunPythia program
pgm_loc="/home/packard/Dropbox/packard_SeniorProj/moschelli/p-pthesis/"
#the root file output location
out_loc="/media/external1/packard/db/RChange/"
#prefix for output
prefix="R4testing"
#log location
log_loc="/home/packard/Dropbox/packard_SeniorProj/moschelli/p-pthesis/logs/FastJet/"
#the name of the .root file that stores final events
filename="pp10mEv_7TeV_ptHatMin0GeV_run"

#FASTJET VARIABLE OVERRIDES
#R value for jet cuts (0.4 default, comment out to default)
#R=0.1
#R=0.3
#R=0.6
R=0.4
#Opening Angle (0.4 default, comment out to default)
OA=0.4
#<pt> minimum cut for jet acceptance (5.0 default, comment out to default)
JP=5.0
#Leading Pt minimum (8.0 default, comment out to default)
LJ=8.0
#Subleading Pt minimum (5.0 default, comment out to default)
SLJ=5.0
#Maximum track rapidity (200.0 default, comment out to default)
MTR=200.0
#Pt low (0.0 default, comment out to default)
PL=0.0
#Pt high (1.0 default, comment out to default)
PH=1.0
#Number of events (default from file, comment out to default)
NE=1000

#number of computer cores to run RunPythia on
nCores=9

cd $pgm_loc
for(( n=0; n<nCores; n++)) 
do
	./bin/RunFastJet $res_loc $filename$n $out_loc$prefix $R $OA $JP $LJ $SLJ $MTR $PL $PH $NE > $log_loc$filename$n'_'$prefix'_fastjet.log' &
done