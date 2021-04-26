#!/bin/bash

SCRIPT=scripts_full_geom_v1/print_signal_like_events_simple.C

for VERSION in v0_gsrock v{0,1}_nacl v1_polyhalite ; do
    echo $VERSION
    INFILE=$(ls plots/full_geom_$VERSION*/merged_proper_coinc_simple.root)
    root -l -b -q "$SCRIPT(\"$INFILE\")" > logs/proper_coinc_simple_${VERSION}.log &
done
