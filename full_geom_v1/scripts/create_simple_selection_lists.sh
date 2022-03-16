#!/bin/bash

SCRIPT=scripts_full_geom_v1/print_signal_like_events_simple.C

for VERSION in v0_gsrock v{0,1}_nacl v1_polyhalite ; do
    echo $VERSION
    INFILE=$(ls plots/full_geom_$VERSION*/merged_proper_coinc_simple.root)
    root -l -b -q "$SCRIPT(\"$INFILE\")" |\
     sed -n '/Full veto/{n; :a ; p; n; /No LS/!b a}' |\
     while read -a array ; do
	echo ${array[0]} ${array[1]} ${array[3]} ${array[4]} ${array[5]} ${array[6]} ${array[7]} ${array[8]} ${array[9]} ${array[2]}
     done > single_event_dumps/lists/proper_coinc_simple_${VERSION}_times_full &
done

for VERSION in v0_gsrock v{0,1}_nacl v1_polyhalite ; do
    echo $VERSION
    INFILE=$(ls plots/full_geom_$VERSION*/merged_proper_coinc_simple.root)
    root -l -b -q "$SCRIPT(\"$INFILE\")" |\
     sed -n '/No LS/{n; :a ; p; n; b a}' |\
     while read -a array ; do
	echo ${array[0]} ${array[1]} ${array[3]} ${array[4]} ${array[5]} ${array[6]} ${array[7]} ${array[8]} ${array[9]} ${array[2]}
     done > single_event_dumps/lists/proper_coinc_simple_${VERSION}_times_nols &
done
