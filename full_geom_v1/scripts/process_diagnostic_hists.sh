#!/bin/bash

# TYPE="_polyhalite"
# SUFFIX=""
TYPE="_nacl"
SUFFIX="_4classes_sortbugfix"
RANGE=$(eval echo 3003{0..3}00)

[[ $# > 0 ]] && TYPE=$1
[[ $# > 1 ]] && SUFFIX=$2
[[ $# > 2 ]] && RANGE=$(eval echo $3)


OUTDIR=plots/full_geom_v1${TYPE}$SUFFIX
BASEDIR=data_v1/full_geom_v1${TYPE}$SUFFIX

install -d $OUTDIR

for BATCH in $RANGE ; do
    OUTPREF=$OUTDIR/batch_${BATCH}_
    LOGFILE=logs/process_diagnostics_v1${TYPE}${SUFFIX}_${BATCH}.log

    root -l -b -q "scripts_full_geom_v1/process_diagnostic_hists.C(\"\",\"$OUTPREF\", $BATCH, 100, \"$BASEDIR\")" \
	&> $LOGFILE&
done
