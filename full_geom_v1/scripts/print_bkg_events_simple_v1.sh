#!/bin/bash


#for VERSION in v{0,1}_nacl v1_polyhalite ; do rootbq "scripts_full_geom_v1/print_signal_like_events_simple.C(\"$(ls plots/full_geom_$VERSION*/merged_proper_coinc_simple.root)\")" | sed -n '/No LS/{n; :a ; p; n; b a}' |while read -a array ; do echo ${array[0]} ${array[1]} ${array[3]} ; done > single_event_dumps/lists/proper_coinc_simple_${VERSION}_times_nols ; done


VERSION=v1_nacl
for BATCH in 30{0,3}{2,3}{0..3}00 ; do
    INFILE=$(ls plots/full_geom_$VERSION*/batch_${BATCH}_proper_coinc_simple_v1.root)
    OUTFILE=single_event_dumps/lists/proper_coinc_simple_v1_${VERSION}_${BATCH}.out
    root -b -q -l "scripts_full_geom_v1/print_signal_like_events_simple_v1.C(\"$INFILE\")" &> $OUTFILE &
    pids[$part]=$!
done

# wait for all parts to finish before moving on
# echo Waiting...
# for pid in ${pids[*]} ; do
#     wait $pid
# done
# echo Done.
