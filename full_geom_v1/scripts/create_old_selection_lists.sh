#!/bin/bash


if [ -n "$PRODUCELOGS" ] ; then
    SCRIPT="scripts_full_geom_v1/print_bkgevents.C"

    # for VERSION in v0_gsrock v{0,1}_nacl v1_polyhalite ; do
    for VERSION in v1_nacl ; do
	BASEDIR="$(ls -d data_v?/full_geom_${VERSION}{,_*sortbugfix} 2>/dev/null)"
	for BATCH in 300{2,3}{0..3}00; do
	    OUTLOG=single_event_dumps/lists/oldsel_${VERSION}_${BATCH}_print_bkg.log
	    root -l -b -q "$SCRIPT(\"\", $BATCH, 100, 0, \"LSon\", \"$BASEDIR\")" &> \
		$OUTLOG &
	done
    done
    exit
fi

if [ -n "$PRODUCELOGSNEW" ] ; then
    SCRIPT="scripts_full_geom_v1/print_bkgevents_new.C"

    # for VERSION in v0_gsrock v{0,1}_nacl v1_polyhalite ; do
    for VERSION in v1_nacl ; do
	BASEDIR="$(ls -d data_v?/full_geom_${VERSION}{,_*sortbugfix} 2>/dev/null)"
	for BATCH in 300{2,3}{0..3}00; do
	    OUTLOG=single_event_dumps/lists/oldsel_${VERSION}_${BATCH}_print_bkg_new.log
	    root -l -b -q "$SCRIPT(\"\", $BATCH, 100, 0, \"LSon\", \"$BASEDIR\")" &> \
		$OUTLOG &
	done
    done
    exit
fi

if [ -v FILTERLOGSNEW ] ; then

    for VERSION in v1_nacl ; do
	echo $VERSION
	LISTFILE=single_event_dumps/lists/oldsel_${VERSION}_new_times_nols

	rm $LISTFILE; touch $LISTFILE

	for BATCH in 300{2,3}{0..3}00; do
	    INFILE=single_event_dumps/lists/oldsel_${VERSION}_${BATCH}_print_bkg_new.log
	    cat $INFILE |\
        sed -n '/No LS/,${/Full\|No/!p}' |\
        awk '{if($3 > 1e-3) {print $1, $2, $4}}' >> $LISTFILE
            # while read -a array ; do
	    #     echo ${array[0]} ${array[1]} ${array[3]}
            # done
	done
    done
    exit
fi

#for VERSION in v0_gsrock v{0,1}_nacl v1_polyhalite ; do
for VERSION in v1_nacl ; do
    echo $VERSION
    LISTFILE=single_event_dumps/lists/oldsel_${VERSION}_times_full

    rm $LISTFILE; touch $LISTFILE

    for BATCH in 300{2,3}{0..3}00; do
	INFILE=single_event_dumps/lists/oldsel_${VERSION}_${BATCH}_print_bkg.log
	cat $INFILE |\
        sed -n '/Full veto/,/No LS/{/Full\|No/!p}' |\
# '/Full veto/{n; :a ; p; n; /No LS/!b a}' |\
	awk '{if($3 > 1e-3) {print $1, $2, $4}}' >> $LISTFILE
        # while read -a array ; do
	#     (( $(echo "${array[2]} > 1.e-3" | bc )) && echo ${array[0]} ${array[1]} ${array[3]}
	# done
    done
done

#for VERSION in v0_gsrock v{0,1}_nacl v1_polyhalite ; do
for VERSION in v1_nacl ; do
    echo $VERSION
    LISTFILE=single_event_dumps/lists/oldsel_${VERSION}_times_nols

    rm $LISTFILE; touch $LISTFILE

    for BATCH in 300{2,3}{0..3}00; do
	INFILE=single_event_dumps/lists/oldsel_${VERSION}_${BATCH}_print_bkg.log
	cat $INFILE |\
        sed -n '/No LS/,${/Full\|No/!p}' |\
        awk '{if($3 > 1e-3) {print $1, $2, $4}}' >> $LISTFILE
        # while read -a array ; do
	#     echo ${array[0]} ${array[1]} ${array[3]}
        # done
    done
done
