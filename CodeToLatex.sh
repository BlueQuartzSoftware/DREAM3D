#!/bin/bash

texFile=/tmp/AIMRepresentation_PublicAffairs_Release.tex
# cd "/Users/mjackson/Contracts/AFRL-TO90/AIMRepresentation"
DATE=`date +%Y.%m.%d`
git archive --format=tar HEAD | gzip > /tmp/AIMRepresentation_$DATE.tar.gz


echo "\documentclass[10pt,oneside]{book}" > $texFile
echo "\usepackage{geometry}" >> $texFile
echo "\usepackage{a4wide}" >> $texFile      
echo "\geometry{letterpaper}" >> $texFile    
echo "\usepackage[parfill]{parskip} " >> $texFile 
echo "\usepackage{graphicx}" >> $texFile
echo "\usepackage{amssymb}" >> $texFile
echo "\usepackage{epstopdf}" >> $texFile
echo "\usepackage{listings}" >> $texFile
echo "\usepackage{color}" >> $texFile
echo "\usepackage{times}" >> $texFile
echo "\RequirePackage{verbatim}" >> $texFile
echo "\textheight=9.0in" >> $texFile
echo "\textwidth=7.0in" >> $texFile
echo "\oddsidemargin=-0.25in" >> $texFile
echo "\evensidemargin=0.0in" >> $texFile
echo "\topmargin=-0.5in" >> $texFile
echo "" >> $texFile
echo "\usepackage[colorlinks=true, pdfstartview=FitV, linkcolor=blue, citecolor=blue, urlcolor=blue]{hyperref}" >> $texFile
echo "" >> $texFile
echo "\lstset{language=C++,captionpos=b,tabsize=3,frame=lines,keywordstyle=\color{magenta},commentstyle=\color{red},stringstyle=\color{blue},numbers=left,numberstyle=\tiny,numbersep=5pt,breaklines=true,showstringspaces=false,basicstyle=\footnotesize,emph={label}}" >> $texFile

echo "% Used by @code ... @endcode" >> $texFile
echo "\newenvironment{DoxyCode}{%" >> $texFile
echo "  \footnotesize%" >> $texFile
echo "  \verbatim%" >> $texFile
echo "}{%" >> $texFile
echo "  \endverbatim%" >> $texFile
echo "  \normalsize%" >> $texFile
echo "}" >> $texFile
echo "" >> $texFile
echo "\makeatletter" >> $texFile
echo "\renewcommand{\@makechapterhead}[1]{%" >> $texFile
echo "\vspace*{0 pt}%" >> $texFile
echo "{\setlength{\parindent}{0pt} \raggedright \normalfont" >> $texFile
echo "\bfseries\Large\thechapter.\ #1" >> $texFile
echo "\par\nobreak\vspace{2 pt}}}" >> $texFile
echo "\makeatother" >> $texFile
echo "\def\thechapter       {\arabic{chapter}}" >> $texFile
echo "% ------------------- Title and Author -----------------------------" >> $texFile
echo "\title{AIMRepresentation Software Tools Package}" >> $texFile
echo "\author{Michael Groeber (Air Force Research Laboratory) And Michael A. Jackson (BlueQuartz Software)}" >> $texFile
echo "\begin{document}" >> $texFile
echo "" >> $texFile
#echo "\frontmatter" >> $texFile
echo "\maketitle" >> $texFile
echo "" >> $texFile
echo "\tableofcontents" >> $texFile


files=`find ./License -type f -name "*.license"`
for file in $files;
do
  echo "$file"
  echo "\section{$file}" >> $texFile
  echo "\begin{lstlisting}" >> $texFile
  cat $texFile $file > /tmp/textemp.tex
  mv /tmp/textemp.tex $texFile 
  
  echo "\end{lstlisting}" >> $texFile
  echo "% ----------------------------------------------------------------------------------------" >> $texFile
done

files=`find ./Source/AIM -type f ! -name "itk*" -and \( -name "*.c*" -or -name "*.h*" -or -name "*.txx" \)`
for file in $files;
do
  echo "$file"
  echo "\section{$file}" >> $texFile
  echo "\begin{lstlisting}" >> $texFile
  cat $texFile $file > /tmp/textemp.tex
  mv /tmp/textemp.tex $texFile 
  
  echo "\end{lstlisting}" >> $texFile
  echo "% ----------------------------------------------------------------------------------------" >> $texFile
done

echo "\end{document}" >> $texFile
echo "\end" >> $texFile

echo "Done"