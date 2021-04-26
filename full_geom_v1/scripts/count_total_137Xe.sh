#!/bin/bash



for type in nacl polyhalite ; do
    [ $type == nacl ] && batches=$(echo 30{0,3}{2,3}) && suffix=_nacl_4classes_sortbugfix
    [ $type == polyhalite ] && batches=$(echo 30{1,2}{2,3}) && suffix=_polyhalite
    for prebatch in $batches ; do
	for batch in ${prebatch}{0..3}00 ; do
	    root -b -l -q "scripts_full_geom_v1/count_total_137Xe.C(\"\", $batch, 100, \"data_v1/full_geom_v1$suffix\")"
	done | grep "137Xe: " > 137xe_counts_${type}_${prebatch}xxx &
    done
done
