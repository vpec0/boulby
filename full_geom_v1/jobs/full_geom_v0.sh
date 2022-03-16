#!/bin/bash

function EXECUTE() {
    echo $1
    eval $1
}

CHARGE=$1
ENERGY=$2
MATERIAL=$3
RUN_NO=$4
N_EVENTS=$5

LCG_REPO=/cvmfs/sft.cern.ch/lcg
LCG_VERSION=LCG_96b
BUILD_VERSION=x86_64-centos7-gcc8-opt
BUILD_VERSION_DBG=x86_64-centos7-gcc8-dbg
G4_VERSION=10.05.p01.1
ROOT_VERSION=6.18.04
CMAKE_VERSION=3.14.3
GCC_VERSION=8.3.0.1
GCC_BUILD=x86_64-centos7

GEANT4_HOME=${LCG_REPO}/releases/${LCG_VERSION}/Geant4/${G4_VERSION}/${BUILD_VERSION_DBG}
EXECUTE ". $GEANT4_HOME/Geant4-env.sh"
EXECUTE '. $GEANT4_HOME/bin/geant4.sh'

HOMEDIR=/home/vpec/work/boulby/dev/work
OUTDIR=/home/vpec/work/boulby/dev/data/env_vac/$MATERIAL/$CHARGE/$ENERGY/$RUN_NO
LOGDIR=$OUTDIR
#/home/vpec/work/boulby/dev/jobs/logs
LOGFILE=$LOGDIR/sim_$RUN_NO.log

install -d $OUTDIR
install -d $LOGDIR

[ $CHARGE == muminus ] && CHARGE_CODE=mu-
[ $CHARGE == muplus ] && CHARGE_CODE=mu+

MACRO_FILE=$OUTDIR/run_${CHARGE}_${ENERGY}GeV_${N_EVENTS}.mac
cat $HOMEDIR/run.mac |sed "s/energy 280/energy $ENERGY/g" | sed "s/beamOn 10000/beamOn $N_EVENTS/g" | \
    sed "s/particle mu+/particle $CHARGE_CODE/g" \
    > $MACRO_FILE

echo $MACRO_FILE ":" &> $LOGFILE
cat  $MACRO_FILE &>> $LOGFILE

date &>> $LOGFILE
$HOMEDIR/full_geom_v0 $MACRO_FILE $OUTDIR/sim.root $RUN_NO ${MATERIAL} &>> $LOGFILE
date &>> $LOGFILE
