#!/bin/bash

FNAME=LS_on_off_comparison_plots

FILE_LIST_BASIC="EdepTpc_bwscaled EdepRfr_bwscaled EdepSkin_bwscaled EdepGdls_bwscaled EdepWt_bwscaled"
FILE_LIST_VETO="EdepTpc EdepTpcNoSkin EdepTpcNoSkinGdls EdepTpcNoSkinGdlsWt EdepTpcNoSkinWt"
#FILE_LIST_VETO="EdepTpc EdepTpcNoSkin EdepTpcNoSkinGdls EdepTpcNoSkinGdlsWt"
#EdepTpc_Xe EdepTpcNoSkin_Xe EdepTpcNoSkinGdls_Xe EdepTpcNoSkinGdlsWt_Xe  EdepTpc_Xeonly EdepTpcNoSkin_Xeonly EdepTpcNoSkinGdls_Xeonly EdepTpcNoSkinGdlsWt_Xeonly"


# Create beamer tex header
cat > ${FNAME}.tex <<EOF
\input{../beamerheader}
\usepackage{listings}
\title[Compiled Plots]{Compiled Plots}
\author{Viktor P\v e\v c}

\graphicspath{{}}
\begin{document}

EOF


## Create beamer frames

# simple energy depositions
# for fig_name in $FILE_LIST_BASIC ; do
#     cat >> ${FNAME}.tex <<EOF
# \begin{frame}{}

#   \begin{cvarblock}[12cm]{Scintillator On (left) vs Off (right)}
#     \includegraphics[width=4cm]{LSon/Edep/$fig_name}
#     \includegraphics[width=4cm]{LSoff/Edep/$fig_name}
#   \end{cvarblock}

# \end{frame}

# EOF
# done


# depositions in TPC with different veto
for dep_type in "" "_Xe" "_Xeonly" ; do
    for fig_name in $FILE_LIST_VETO ; do
	cat >> ${FNAME}.tex <<EOF
\begin{frame}{}

  \begin{cvarblock}[12cm]{Scintillator On (left) vs Off (right)}
    \includegraphics[width=4cm]{LSon/Edep/$fig_name$dep_type}
    \includegraphics[width=4cm]{LSoff/Edep/$fig_name$dep_type}
  \end{cvarblock}

\end{frame}

EOF
    done
done



# close the tex file
cat >> ${FNAME}.tex <<EOF

\end{document}
EOF


# compile the file
pdflatex ${FNAME}.tex

[ $? -eq 0 ] && . cleanup.sh
