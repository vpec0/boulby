#!/bin/bash

FNAME=grr_vs_nacl_comparison_plots

#FILE_LIST="$(ls compare_grr*_ratio.pdf)"

BASENAME=compare_grr_nacl_

DETNAMES="Tpc Rfr Skin Gdls Wt"
DEPOTYPES="Other Mu Em Xe"
VETO="noWt noGdlsWt noSkinGdlsWt noSkinWt"
LSSTATUS="on off"


# Create beamer tex header
cat > ${FNAME}.tex <<EOF
\input{beamerheader}
\usepackage{listings}
\title[Compiled Plots]{Compiled Plots}
\author{Viktor P\v e\v c}

\graphicspath{{}}
\begin{document}

EOF


## Create beamer frames

# Edep counts
for DETNAME in $DETNAMES ; do
	FBASENAME=${BASENAME}N$(echo $DETNAME|tr '[:upper:]' '[:lower:]')
	cat >> ${FNAME}.tex <<EOF
\begin{frame}{Deposition Counts -- GS Rock vs NaCl}
    \framesubtitle{$DETNAME}

  \begin{cvarblock}[12cm]{$DETNAME}
    \includegraphics[width=5.5cm]{$FBASENAME}
    \includegraphics[width=5.5cm]{${FBASENAME}_ratio}
  \end{cvarblock}

\end{frame}

EOF
done


# Edep plots
for DETNAME in $DETNAMES ; do
    for DEPOTYPE in "" $DEPOTYPES $( [ "$DETNAME" == Tpc ] && echo "Xeonly"); do
	FBASENAME=${BASENAME}Edep${DETNAME}
	[ -n "$DEPOTYPE" ] && FBASENAME=${FBASENAME}_
	FBASENAME=$FBASENAME$DEPOTYPE
	cat >> ${FNAME}.tex <<EOF
\begin{frame}{1st Energy Depositions -- GS Rock vs NaCl}
    \framesubtitle{$DETNAME, $DEPOTYPE}

  \begin{cvarblock}[12cm]{$DETNAME}
    \includegraphics[width=5.5cm]{$FBASENAME}
    \includegraphics[width=5.5cm]{${FBASENAME}_ratio}
  \end{cvarblock}

\end{frame}

EOF
    done
done



# Edep in TPC with veto - Scint ON and OFF
for STATUS in $LSSTATUS ; do
    cat >> ${FNAME}.tex <<EOF
\begin{frame}{}
  \centering
  \Large Depositions in TPC with VETO\\
  Scintillator $STATUS

\end{frame}

EOF

    for i in $VETO ; do
	for DEPOTYPE in "" $DEPOTYPES "Xeonly" ; do
	    FBASENAME=${BASENAME}EdepTpc${i}
	    [ -n "$DEPOTYPE" ] && FBASENAME=${FBASENAME}_$DEPOTYPE
	    cat >> ${FNAME}.tex <<EOF
\begin{frame}{1st Energy Deposition in TPC -- GS Rock vs NaCl}
    \framesubtitle{Scintillator $STATUS}

  \begin{cvarblock}[12cm]{Veto $i, Deposition by $DEPOTYPE}
    \includegraphics[width=5.5cm]{$FBASENAME}
    \includegraphics[width=5.5cm]{${FBASENAME}_ratio}
  \end{cvarblock}

\end{frame}

EOF
	done
    done
done




# close the tex file
cat >> ${FNAME}.tex <<EOF

\end{document}
EOF


# compile the file
pdflatex -interaction=nonstopmode ${FNAME}.tex

[ $? -eq 0 ] && . cleanup.sh
