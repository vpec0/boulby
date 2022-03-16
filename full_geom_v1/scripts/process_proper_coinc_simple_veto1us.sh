#!/bin/bash

VERSION_DIR=full_geom_v1_nacl_4classes_sortbugfix
DATA_DIR=/data/vpec/boulby/full_geom_v1/data/$VERSION_DIR
OUTPUT_DIR=plots/$VERSION_DIR
mkdir -p $OUTPUT_DIR
mkdir -p logs/$VERSION_DIR


[[ "$VERSION_DIR" == *"v1_nacl"* ]] && BATCH_LIST=$(echo 30{0,3}{2,3}{0..3}00)
[[ "$VERSION_DIR" == *"v1_polyhalite"* ]] && BATCH_LIST=$(echo 30{1,2}{2,3}{0..3}00)
[[ "$VERSION_DIR" == *"v0_gsrock"* ]] && BATCH_LIST=$(echo 2000{0..3}00)
[[ "$VERSION_DIR" == *"v0_nacl"* ]] && BATCH_LIST=$(echo 200{1,2}{0..3}00)


for i in $BATCH_LIST ; do
    nice time root -l -b -q "scripts_full_geom_v1/process_proper_coinc_simple_veto1us.C(\"$DATA_DIR\", $i, 100, \"$OUTPUT_DIR/batch_${i}_proper_coinc_simple_veto1us.root\")" \
&> logs/$VERSION_DIR/process_proper_coinc_simple_veto1us_${i}.log &
done
