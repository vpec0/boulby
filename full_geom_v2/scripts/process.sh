#!/bin/bash

#TYPE="_polyhalite"
#SUFFIX=""
TYPE="_nacl"
SUFFIX="_4classes_sortbugfix"

OUTDIR=plots/full_geom_v1${TYPE}$SUFFIX
BASEDIR=data_v1/full_geom_v1${TYPE}$SUFFIX

install -d $OUTDIR

for LS in "LSon" "LSoff" ; do
    for BATCH in 3003{0..3}00 ; do
	OUTPREF=$OUTDIR/batch_${BATCH}_${LS}_
	LOGFILE=logs/process_v1${TYPE}${SUFFIX}_${BATCH}_${LS}.log

	root -l -b -q "scripts_full_geom_v1/process.C(\"\",\"$OUTPREF\", $BATCH, 100, \"${LS}\", \"$BASEDIR\")" \
	    &> $LOGFILE&
    done
done
