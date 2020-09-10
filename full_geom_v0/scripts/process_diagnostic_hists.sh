#!/bin/bash

# TYPE="_polyhalite"
# SUFFIX=""
TYPE="_nacl"
SUFFIX="_4classes_sortbugfix"
RANGE=$(eval echo 2001{0..3}00 2002{0..3}00)

[[ $# > 0 ]] && TYPE=$1
[[ $# > 1 ]] && SUFFIX=$2
[[ $# > 2 ]] && RANGE=$(eval echo $3)


OUTDIR=plots/full_geom_v0${TYPE}$SUFFIX
BASEDIR=data_v0/full_geom_v0${TYPE}$SUFFIX
LOGDIR=logs/v0/diagnostics

install -d $OUTDIR
install -d $LOGDIR

for BATCH in $RANGE ; do
    OUTPREF=$OUTDIR/batch_${BATCH}_
    LOGFILE=$LOGDIR/process_diagnostics_v0${TYPE}${SUFFIX}_${BATCH}.log

    root -l -b -q "scripts_full_geom_v0/process_diagnostic_hists.C(\"\",\"$OUTPREF\", $BATCH, 100, \"$BASEDIR\")" \
	&> $LOGFILE&
done
