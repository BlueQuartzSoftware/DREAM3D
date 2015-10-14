#!/bin/bash 

#  adapted from DOS batch file supplied by Sven Vogel, LANL
#  edited to handle multiple Pole figures, ADR  x 04
#  switched from sh to bash
#  last edit:  2 Jan 07, ADR
#  last edit:  2 Jul 08, ADR; discrete PFs
#  added DL option, 30 i 10, ADR; plot PFs with labels
#  added IX option, iv 12, ADR; 
#    plots pairs of arrows on IVPs from ExtractExptlFFTComparison
#  added SP option, vii 12, ADR;
#    plots sets of pole figures on top of one another
#  added DA option for overlaid stereo projections
#   PSTEXT now uses "-h1" instead of "-H1" for header line
#  added "gmt " in front of each command to adapt to the OSX app version
#  added Windows support through the MSys prompt. Sanity checking to make sure we have all the executables.

DS_VERSION=2
DS_LAST_REVISION="3 Feb 2014"
echo "Draw_stereograms: Last Revised ${DS_LAST_REVISION}"

# Sanity check the user's environment first
# Check for GMT Version 5
echo "Checking for the GMT executable...."
GIT=`type -P gmt`
if [[ $GIT == "" ]];
  then
  echo "GMT is needed for this script. Please install it on your system and be sure it is on your path."
  exit 1
fi

# Get the GMT Version we are using
GMT_MIN_VERSION="5"
GMT_VERSION=`gmt --version`
if (( ${#GMT_VERSION[0]} < ${GMT_MIN_VERSION} ))
then
        echo "Version Check Failed: GMT Version found '${GMT_VERSION}', Needed Version '${GMT_MIN_VERSION}'"
        exit 1
fi

# Check for the 'convert' program which is part of the ImageMagick installation
echo "Checking for the 'convert' executable..."
CONVERT=`type -P convert`
if [[ $CONVERT == "" ]];
        then
        echo "Missing 'convert' program from the ImageMagick installation. Please install ImageMagick and make sure the executables are available on your path"
        exit 1
fi

# In order to convert postscript to an image format Ghostscript is used (Linux) or Native OS X functions
# but for Windows we need to make sure Ghostscript is installed
if [ ${OSTYPE} = "msys" ]
then
        echo "Checking for 32bit Ghostscript executables...."
        GSWIN32=`type -P gswin32c`
        if [[ $GSWIN32 == "" ]];
        then
                echo "Missing 'gswin32c' program from the Ghostscript package. Please install ImageMagick and make sure the executables are available on your path"
                exit 1
        fi
fi


#----------------------------------------------------------------------------------
# This next section deals with getting the necessary commands to generate a continuous plot versus a discrete plot.
# We are going to default to attempting a continuous plot. If you explicitly want a Discrete plot then set the variable
# DO_DISCRETE=1
DO_DISCRETE=0

# Check for the 'bc' command. if it is not found then we can not do a discrete plot
BC=`type -P bc`
if [[ $BC == "" ]];
        then
        echo "Missing 'bc' program which allows you to do a continuous plot. Turning off the continuous plot and doing a discrete instead"
        DO_DISCRETE=1
fi


 
STRING="
 0  90
   0.0000       45.000    
   2.1208       44.980    
   4.2388       44.921    
   6.3509       44.824    
   8.4545       44.687    
   10.547       44.512    
   12.625       44.299    
   14.687       44.048    
   16.731       43.761    
   18.754       43.438    
   20.754       43.079    
   22.729       42.687    
   24.679       42.260    
   26.602       41.801    
   28.496       41.311    
   30.361       40.789    
   32.197       40.239    
   34.001       39.659    
   35.776       39.053    
   37.519       38.420    
   39.232       37.761    
   40.913       37.078    
   42.564       36.372    
   44.185       35.644 
 45 35.3
 0 90"

#  for clipping, drawing boxes around cubic Inv PF
#  echo $STRING

HEXSTRING="
 0.0  90.0
 0.0   0.0
 2.0   0.0
 4.0   0.0
 6.0   0.0
 8.0   0.0
10.0   0.0
12.0   0.0
14.0   0.0
16.0   0.0
18.0   0.0
20.0   0.0
22.0   0.0
24.0   0.0
26.0   0.0
28.0   0.0
30.0  0.0
 0.0  90.0"
#  for clipping, drawing boxes around hex Inv PF

#  don't really want to proceed unless we have at least 3 arguments
if [ $# -le 2 ]; then
   echo " number of args = " $#
   echo " usage= ./Draw_stereograms number_of_files Stereo_file_name [2d/5d/PF/IF] [wysiwyg/gray/polar/...] [Lo] [Hi] [step] [stereo/equal/ortho]  [CUBIC/HEX/ORT]"
   echo " If #3='2d' then it looks for only one file (with extension .dat )"
   echo " If #3='5d' then it looks for several stereogram files (with extension .dat )"
   echo " If #3='PF' it looks for pole figure files (with extension .gpf )"
   echo " If #3='IF' it looks for inverse pole figure files (with extension .gpf )"
   echo " If #3='DP' it looks for discrete pole figure files (with extension .dpf )"
   echo " If #3='DA' it looks for pairs of discrete PF files to co-plot (with extension .daN, .dbN )"
   echo "     N must start at 1 and increase sequentially)"
   echo " If #3='DI' it looks for discrete inv. pole figure files (with extension .dip )"
   echo " If #3='DL' it looks for discrete PFs with PSTEXT lines (with extension .txt )"
   echo " If #3='IX'discrete inv. pole figure files for pairs of (with extensions .ipx1+.ipx2 )"
   echo " If #3='PX'discrete pole figure files for vectors between pairs of points (with extension .dpx1 )"
   echo " If #3='P2'discrete pole figure files for pairs of points, 2 files (with extension .dpx, .dpx2 )"
   echo " If #9='HEX' then it draws lines for hexagonal symmetry instead of cubic "
   echo " If #9='ORT' then it draws lines for orthorhombic symmetry instead of cubic "
   exit

else

   echo " Script for drawing stereograms from 5-parameter GB analysis, or pole figures"
   echo " Note that the input files (data) must have longitude, latitude, intensity"
   echo " Tony Rollett, CMU, with thanks to Sven Vogel, Los Alamos "

   if [ $# -ge 3 ]; then
      if [ $3 != 2d ] && [ $3 != 5d ] && [ $3 != PF ] && [ $3 != IF ] && [ $3 != DP ] \
          && [ $3 != DI ]  &&  [ $3 != IX ] && [ $3 != PX ] \
          && [ $3 != P2 ]  && [ $3 != DL ]  && [ $3 != DA ]; then
         echo "data can only be 2d, 5d, PF, IF, DP, DA, DI, PX, P2, IX or DL"
         exit 1
      fi
   fi

   if [ $# -ge 8 ]; then
      if [ $8 != ortho ] && [ $8 != stereo ] && [ $8 != equal ]; then
         echo "projections can only be stereo, equal_area or ortho "
         exit 1
      fi
   fi

fi

# argument 1 = number of pole figures
# argument 2 = name of popLA polefigures file, assuming there is a suffix, e.g. .EPF
# argument 3 = 2d if there is only file to plot
# argument 3 = 5d if there are several files to plot
# argument 3 = PF if there are pole figure files to plot
# argument 3 = IF if there are inverse pole figure files to plot
# argument 3 = DP if there are discrete pole figure files to plot
# argument 3 = DI if there are discrete inverse pole figure files to plot
# arg 4 = color scheme
# arg 5 = minimum contour value
# arg 6 = max contour
# arg 7 = interval
# arg 8 = projection type

# argument __ = [nospot]

#  ./pf2GMT $2
#   the conversion is not needed for 5param output,  iv 06

var1="$2"
#  echo "var1 = $2"
# t=${var1%*.*}
# t=${var1%.*}
t="$2"
#  echo "var1 (with everything from the last . on stripped out) = "$t""
echo "filename base: " "$t"
#  "$t" now contains the basename
#  exit 0

if [ -e "${t}".ps ] ; then
    rm "${t}".ps
fi
#  get rid of any PS file left over

if [ $# -lt 7 ]; then
   TEMP="1"   # default for later use
else
   TEMP=$7
fi

BORDER="-Bwesn"
PROJ="-JS-90/90/5c"
# REGION="-Rg"
#  let's try using Global consistently
#  PROJ="-JS-90/90/5c"
REGION="-R0/360/0/90"
COLORSCHEME="rainbow"
TXTCOORDS="-D-0.2i/2.6i"
HDRCOORDS="-D-0.5i/10.6i"
ARROWCOORDS="-D5.0i/10.5i"
XARROWCOORDS="-D6.0i/10.9i"
STEREOCOORDS="-D4.5i/10.0i"

m8="stereo"
m9="CUBIC"

if [ $# -ge 8 ]; then
   if [ $8 = ortho ] ; then
      BORDER="-B90g90/90g90"
      PROJ="-JG-45/45/5c"
      REGION="-Rg"
      TXTCOORDS="-D-0.2i/1.6i"
      HDRCOORDS="-D-0.5i/9.7i"
      m8="ortho"
   fi
   if [ $8 = equal ] ; then
      PROJ="-JA-90/90/5c"
      TXTCOORDS="-D-0.2i/2.1i"
      HDRCOORDS="-D-0.5i/10.4i"
      m8="equal"
   fi
fi

if [ $# -ge 9 ]; then
    m9="$9"
fi

if [ $# -ge 4 ]; then
   COLORSCHEME="$4"
fi



#  gsr note: The nex few lines will make contours appear continuous.
#  If you want the traditional discrete plot style, comment out "z=$(echo "scale=9; $y/10.0" | bc)" and uncomment z=$7, below.
#  If you want to add the lines between contours, find "Adds the contour line" and remove all 
#  comments in that section that are marked by "#g"

if [ ${DO_DISCRETE} = "0" ] ; then
y=$7
z=$(echo "scale=9; $y/10.0" | bc)
#z=$7
else
y=$7
#z=$(echo "scale=9; $y/10.0" | bc)
z=$7
fi
echo "z =" $z

      
PDF_CPT_FILE="${t}PDF.cpt"

if [ $# -ge 7 ]; then
   gmt makecpt -C"$COLORSCHEME"  -T"$5"/"$6"/"$z" > "${PDF_CPT_FILE}"
#  was -T0.0/3/0.5
else
   gmt makecpt -C"$COLORSCHEME"  -T0.0/8./1.0 > "${PDF_CPT_FILE}"
#    makecpt -C"$COLORSCHEME"  -Qo -V > "${PDF_CPT_FILE}"
#  tried the log scale (Q) option briefly but it's basically for powers of 10
fi

echo "You requested " $1 " stereograms to be drawn"

COUNTER=1
OFFSET=0.0i       #  offset for plotting - different for Inv PFs
Y_OFFSET=6.5i   #  y offset
FIRST_TIME=0    #  to track whether it's the first plot or not, and for offsets
CLIPPED=0   #  did we clip?

if [ "$3" = IF ] || [ "$3" = DI ] || [ "$3" = IX ] ; then
   Y_OFFSET=3.5i
   if [ $m9 = CUBIC ] || [ $m9 = cubic ] ; then
      OFFSET=-4.0i
      PROJ="-JA-90/90/18c"
   fi
   if [ $m9 = HEX ] || [ $m9 = hex ] ; then
      OFFSET=-2.5i
      PROJ="-JA-90/90/11c"
   fi
fi

# set up size for accumulated pole figures
if [ "$3" = DA ] ; then
#   OFFSET=-4.0i
#   Y_OFFSET=3.5i
   OFFSET=0
   Y_OFFSET=6.0i
   PROJ="-JA-90/90/18c"
fi

# echo 'grdimage "${t}TempPDF.grd" -C"${PDF_CPT_FILE}" -JA-90/90/5 -Bwesn -K -P'
# echo grdimage -X$COUNTER

while [  $COUNTER -le $1 ]; do
   echo  
   echo Working on stereogram number ${COUNTER}
# echo grdimage "${t}TempPDF.grd" -C"${PDF_CPT_FILE}" -JA-90/90/5 -Bwesn -K -P -X$COUNTER"i" -Y3i
#             let OFFSET=COUNTER*3+2
#  echo offset = $OFFSET cm
#             let COUNTER=COUNTER+1 
#  incremented below

   if [ $3 = 2d ] ; then
      echo "current input file: " "${t}".dat
      read HEADER < "${t}".dat
#  nearneighbor "$t".st$COUNTER -G"${t}TempPDF.grd" -I3/3 -R0/360/0/90 -H1 -Lxy -S15
#  nearneighbor "${t}"_5d_degr${COUNTER}.dat -G"${t}TempPDF.grd" -I3/3 -R0/360/0/90 -H1 -Lxy -S15
#       nearneighbor "${t}".dat -G"${t}TempPDF.grd" -I3/3 -R0/360/0/90 -H1 -S15
#  GMT4:      nearneighbor "${t}".dat -G"${t}TempPDFnn.grd"  -E0. -N2/2 -Lg -I2.5 -fg -Rg -H1 -S30
#  Region = global, seems to eliminate the problem of contours not crossing the x-axis
      #gmt nearneighbor "${t}".dat -G"${t}TempPDFnn.grd"  -E0. -N2/2 -I2.5 -fg -Rg -h1 -S30
  #  old version above
      gmt nearneighbor "${t}".dat -G"${t}TempPDFnn.grd" -fg -I2.5 -Rg -h1 -S15d -N4/4 #-Vl #-i1,2,3 #-S15
   fi

   if [ $3 = PF ] ||  [ $3 = IF ] ; then
      if [ -e "${t}"${COUNTER}.gpf ]
      then
         echo "current input file: " "${t}"${COUNTER}.gpf
         if [ $3 = IF ] ; then
            HDRCOORDS="-D-1.6i/10.2i"
            ARROWCOORDS="-D5.0i/10.5i"
            XARROWCOORDS="-D5.2i/10.5i"
            STEREOCOORDS="-D4.5i/10.0i"
         fi
         if [ $3 = PF ] ; then
            HDRCOORDS="-D-0.6i/10.6i"
            ARROWCOORDS="-D5.0i/10.5i"
            XARROWCOORDS="-D6.i/10.9i"
            STEREOCOORDS="-D5.0i/10.4i"
         fi
      else
         echo "you asked for " "${t}"${COUNTER}.gpf " but it does not exist!  Exiting ..."
         exit 1
      fi
      read HEADER < "${t}"${COUNTER}.gpf
#  nearneighbor "$t".st$COUNTER -G"${t}TempPDF.grd" -I3/3 -R0/360/0/90 -H1 -Lxy -S15
#  nearneighbor "${t}"_5d_degr${COUNTER}.dat -G"${t}TempPDF.grd" -I3/3 -R0/360/0/90 -H1 -Lxy -S15
#       nearneighbor "${t}"${COUNTER}.gpf -G"${t}TempPDF.grd" -I3/3 -R0/360/0/90 -H1 -S15
#       nearneighbor "${t}"${COUNTER}.gpf -G"${t}TempPDFnn.grd" -fg -I3/3 $REGION -H1 -S15
      gmt nearneighbor "${t}"${COUNTER}.gpf -G"${t}TempPDFnn.grd" -fg -I2.5 -Rg -h1 -S15d -N4/4 #-Vl #-i1,2,3 #-S15
   fi

   if  [ $3 = DP ]; then
      if [ -e "${t}"${COUNTER}.dpf ]
      then
         echo "current input file: " "${t}"${COUNTER}.dpf
      else
         echo "you asked for " "${t}"${COUNTER}.dpf " but it does not exist!  Exiting ..."
         exit 1
      fi
      read HEADER < "${t}"${COUNTER}.dpf
#  no need to do nearneighbor on discrete PFs
   fi

   if  [ $3 = DA ]; then
      if [ -e "${t}".da${COUNTER} ]
      then
         echo "cumulative PFs: input file = " "${t}".da${COUNTER}
      else
         echo "you asked for " "${t}".da${COUNTER} " but it does not exist!  Exiting ..."
         exit 1
      fi
      if [ -e "${t}".db${COUNTER} ]
      then
         echo "cumulative PFs: input file = " "${t}".db${COUNTER}
      else
         echo "you asked for " "${t}".db${COUNTER} " but it does not exist!  Exiting ..."
         exit 1
      fi
      read HEADER < "${t}".da${COUNTER}
#  no need to do nearneighbor on discrete PFs
   fi

   if  [ $3 = PX ]; then
      if [ -e "${t}"${COUNTER}.dpx ]
      then
         echo "current input file: " "${t}"${COUNTER}.dpx
      else
         echo "you asked for " "${t}"${COUNTER}.dpx " but it does not exist!  Exiting ..."
         exit 1
      fi
      read HEADER < "${t}"${COUNTER}.dpx
#  no need to do nearneighbor on discrete PFs
   fi

   if  [ $3 = P2 ]; then
      if [ -e "${t}"${COUNTER}.dpx ] && [ -e "${t}"${COUNTER}.dpx2 ]
      then
         echo "current input files: " "${t}"${COUNTER}.dpx  "${t}"${COUNTER}.dpx2
      else
         echo "you asked for " "${t}"${COUNTER}.dpx " and "  "${t}"${COUNTER}.dpx2 " but they do not exist!  Exiting ..."
         exit 1
      fi
      read HEADER < "${t}"${COUNTER}.dpx
      read HEADER < "${t}"${COUNTER}.dpx2
#  no need to do nearneighbor on discrete PFs
   fi

   if  [ $3 = DI ]; then
      if [ -e "${t}"${COUNTER}.dip ]
      then
         echo "current input file: " "${t}"${COUNTER}.dip
      else
         echo "you asked for " "${t}"${COUNTER}.dip " but it does not exist!  Exiting ..."
         exit 1
      fi
      read HEADER < "${t}"${COUNTER}.dip
#  no need to do nearneighbor on discrete IPFs
   fi

   if  [ $3 = DL ]; then
      if [ -e "${t}"${COUNTER}.txt ]
      then
         echo "current input file: " "${t}"${COUNTER}.txt
      else
         echo "you asked for " "${t}"${COUNTER}.txt " but it does not exist!  Exiting ..."
         exit 1
      fi
      read HEADER < "${t}"${COUNTER}.txt
#  no need to do nearneighbor on discrete PFs for PSTEXT
   fi

   if  [ $3 = IX ]; then
      if [ -e "${t}"${COUNTER}.ipx1 ] && [ -e "${t}"${COUNTER}.ipx2 ]
      then
         echo "current input files: " "${t}"${COUNTER}.ipx1  "${t}"${COUNTER}.ipx2
      else
         echo "you asked for " "${t}"${COUNTER}.ipx[1 and 2] " but they do not exist!  Exiting ..."
         exit 1
      fi
      read HEADER < "${t}"${COUNTER}.ipx1
      read HEADER < "${t}"${COUNTER}.ipx2
#  no need to do nearneighbor on discrete IPFs
   fi

   if [ $3 = 5d ] ; then
      if [ -e "${t}"${COUNTER}.dat ]
      then
         echo "current input file: " "${t}"${COUNTER}.dat
      else
         echo "you asked for " "${t}"${COUNTER}.dat " but it does not exist!"
         exit 1
      fi
#      echo "current input file: " "${t}"${COUNTER}.dat
      read HEADER < "${t}"${COUNTER}.dat
#  nearneighbor "$t".st$COUNTER -G"${t}TempPDF.grd" -I3/3 -R0/360/0/90 -H1 -Lxy -S15
#  nearneighbor "${t}"_5d_degr${COUNTER}.dat -G"${t}TempPDF.grd" -I3/3 -R0/360/0/90 -H1 -Lxy -S15
#       nearneighbor "${t}"${COUNTER}.dat -G"${t}TempPDF.grd" -I3/3 -R0/360/0/90 -H1 -S15

#  GMT4:      nearneighbor "${t}"${COUNTER}.dat -G"${t}TempPDFnn.grd"  -E0. -N2/2 -Lg -I10/10 -fg -Rg -H1 -S20
  #  for GMT5:
  #    gmt nearneighbor "${t}"${COUNTER}.dat -G"${t}TempPDFnn.grd"  -E0. -N2/2  -I10/10 -fg -Rg -h1 -S20
  #  old version above
      gmt nearneighbor "${t}"${COUNTER}.dat -G"${t}TempPDFnn.grd" -fg -I2.5 -Rg -h1 -S15d -N4/4 #-Vl #-i1,2,3 #-S15

#      if [ $# -ge 8 ] && [ $8 = ortho ] ; then
# defaults to stereo projection - similar below
#       nearneighbor "${t}"${COUNTER}.dat -G"${t}TempPDFnn.grd" -I3/3 -Rg -H1 -S15
#      else
#       nearneighbor "${t}"${COUNTER}.dat -G"${t}TempPDFnn.grd" -I3/3 -R0/360/0/90 -H1 -S15
#      fi
   fi  
#  end of 5d section
   echo "1st line =" $HEADER

#  nearneighbor "$t".st$COUNTER -G"${t}TempPDF.grd" -I3/3 -R0/360/0/90 -H1 -Lxy -S15
#  nearneighbor "${t}"_5d_degr${COUNTER}.dat -G"${t}TempPDF.grd" -I3/3 -R0/360/0/90 -H1 -Lxy -S15
#  -H = 1 header line
#  -R  = range
#  -I  grid spacing
#  -S  search radius in same units as I
#  -L  periodic in x and/or y

   if  [ $3 != DP ] && [ $3 != DI ] && [ $3 != DL ] &&  [ $3 != IX ] && [ $3 != PX ]  && [ $3 != P2 ] && [ $3 != DA ] ; then

#    grd2xyz "${t}TempPDF.grd">TempPDF.out
# surface TempPDF.out -G"${t}TempPDF.grd" -T0.5 -I.5/.5 -R0/360/0/90 -H1 -L
#    surface TempPDF.out -G"${t}TempPDF.grd" -T0.9 -I.5/.5 -R0/360/0/90 -H1 -f0x,1y
#  was -T1  =  tension
#  -f0x,1y  defines longitude, latitude

#  this version pipes the output from grd2xyz (faster?)
#  and also uses blockmedian to perform a bit of smoothing
#    grd2xyz "${t}TempPDFnn.grd" |  surface -G"${t}TempPDF.grd" -T0.9 -I.5/.5 -R0/360/0/90 -H1 -f0x,1y

#   grd2xyz "${t}TempPDFnn.grd" | blockmedian $REGION -I5 | surface -G"${t}TempPDF.grd" -fg -T0.9 -I.5/.5 $REGION -H1 -f0x,1y
#   grd2xyz -Rg "${t}TempPDFnn.grd" | blockmedian -I15 -Rg | surface -G"${t}TempPDF.grd" -fg -T0./0.8 -I1 $REGION -H1
#      grd2xyz $REGION "${t}TempPDFnn.grd" | blockmean $REGION -I15 | surface -G"${t}TempPDF.grd" -fg -T0.8 -I.5 $REGION -H1
#   grd2xyz -Rg "${t}TempPDFnn.grd" | blockmedian -I15 -Rg | surface -GTempPDF0.nc -fg -T0./0.8 -I1 -Rg -H1
#   grdfilter TempPDF0.nc  -D4 -Fg250 -I1 -Rg -GTempPDF1.nc -V
#   grd2xyz -Rg TempPDF1.nc | xyz2grd -G"${t}TempPDF.grd" -I1 $REGION -H1
#  new idea= use grdfilter, 30 v 11

#  next 2 lines:  attempt to filter the data - not obvious that this helped
#   grdfilter "${t}TempPDFnn.grd" -D4 -Fc1000 -GTemp00.grd $REGION -I.5
#   grd2xyz Temp00.grd | blockmean $REGION -I2.5 | surface -G"${t}TempPDF.grd" -fg -T0 -I.5/.5 $REGION -H1 -f0x,1y

#  for GMT5
      #gmt grd2xyz $REGION "${t}TempPDFnn.grd" | gmt blockmean $REGION -I15 | gmt surface -G"${t}TempPDF.grd" -fg -T0.8 -I.5 $REGION -h1
      gmt grd2xyz $REGION "${t}TempPDFnn.grd" | gmt blockmean $REGION -I2.5 | gmt surface -G"${t}TempPDF.grd" -fg -T0.8 -I.5 $REGION -h1

#    if [ $# -ge 8 ] && [ $8 = ortho ] ; then
#       grd2xyz "${t}TempPDFnn.grd" | blockmedian -Rg -I5 | surface -G"${t}TempPDF.grd" -T0.9 -I.5/.5 -Rg -H1 -f0x,1y
#    else
#       grd2xyz "${t}TempPDFnn.grd" | blockmedian -R0/360/0/90 -I5 | surface -G"${t}TempPDF.grd" -T0.9 -I.5/.5 -R0/360/0/90 -H1 -f0x,1y
#    fi

      gmt grdinfo "${t}TempPDF.grd" -C -M > "${t}gridinfo.txt"

   fi
#  end of gridding (not for discrete plots)

# MakePDFInfo "$t" $3
#
# Generates color codes for contours
# -T command parameters are lowest, highest and step
FILENAME=`basename "${t}"`
   if [ $FIRST_TIME -eq 0 ] ; then
      LINE=0
#      echo "1st plot, LINE = " $LINE ;

#  this is the first plot

      if [ $3 != DP ] && [ $3 != DI ] && [ $3 != DL ] &&  [ $3 != IX ] && [ $3 != PX ] && [ $3 != P2 ] && [ $3 != DA ] ; then

         gmt psscale -C"${PDF_CPT_FILE}" -D2.5i/9.9i/4i/.25ih -B"$TEMP":"Multiples of Random": -K -P > "$t".ps

         gmt pstext -F+a0+f16p,Times-Roman,blue+jLM -K -O -P -N -R0/11/0/11 -JX11i <<EOF >> "$t".ps
0. 10.1 Filename root: "$FILENAME"
EOF

      else

#        pstext -K -P -N -R0/11/0/11 -JX11i -Gblue <<EOF > "$t".ps
#.15 10.3 16 0 0 LM Filename root: "$t"
#EOF
         gmt pstext -F+a0+f16p,Times-Roman,blue+jLM -K -P -N -R0/11/0/11 -JX11i <<EOF > "$t".ps
0. 10.1 Filename root: "$FILENAME"
EOF

#  do not want the scale for discrete plots
      fi

#      echo "psxy first"
      gmt psxy -K -P -O -N -R0/11/0/11 -JX11i -Sv+e -Gblack -Wthin,black <<EOF >> "$t".ps
5. 10.4 0.35 0. 2. 
EOF
#      echo "psxy first: END"
      gmt pstext -F+a0+f16p,Times-Roman,black+jLM -K -P -O -N -R0/11/0/11 -JX11i <<EOF >>"$t".ps
6 10.4 X
EOF

      if [ $m8 = equal ] ; then
         gmt pstext -F+a0+f16p,Times-Roman,blue+jLM -K -O -N -R0/11/0/11 -JX11i  <<EOF >>"$t".ps
5.5 9.9 Equal Area
EOF
      fi
      if [ $m8 = ortho ] ; then
         gmt pstext -F+a0+f16p,Times-Roman,blue+jLM -K -O -N -R0/11/0/11 -JX11i  <<EOF >>"$t".ps
5.5 9.9 Orthographic
EOF
      fi
      if [ $m8 = stereo ] ; then
#        pstext -K -O -N -R0/11/0/11 -JX11i  <<EOF >>"$t".ps
         gmt pstext -F+a0+f16p,Times-Roman,blue+jLM -K -O -N -R0/11/0/11 -JX11i  <<EOF >>"$t".ps
5.5 9.9 Stereographic
EOF
      fi

      SHIFT=" -X"$OFFSET" -Y"$Y_OFFSET
      if [ $3 = IF ] || [ $3 = DI ]  || [ $3 = IX ] ; then
#        echo "test IF" ;
#  clip the region to plot
         CLIPPED=1
#        echo "at CLIP, CLIPPED = " $CLIPPED "SHIFT= " $SHIFT ;
#         psxy  $PROJ $REGION $BORDER -K -P $SHIFT << END > "$t".ps
      if [ $m9 = CUBIC ] || [ $m9 = cubic ] ; then
         gmt psclip  $PROJ $REGION -K -O -P $SHIFT << END >> "$t".ps
$STRING
END
fi
      if [ $m9 = HEX ] || [ $m9 = hex ] ; then
         gmt psclip  $PROJ $REGION -K -O -P $SHIFT << END >> "$t".ps
$HEXSTRING
END
fi
         SHIFT=""
#        echo "SHIFT =  " $SHIFT
      fi
#  end of IF section

      if  [ $3 != DP ] && [ $3 != DI ] && [ $3 != DL ] &&  [ $3 != IX ] && [ $3 != PX ] && [ $3 != P2 ] && [ $3 != DA ] ; then
#  if not discrete, then do this
         #echo REGION = $REGION
         gmt grdimage "${t}TempPDF.grd" -C"${PDF_CPT_FILE}" $PROJ $REGION -O -K -P $SHIFT >> "$t".ps
#      grdimage "${t}TempPDF.grd" -C"${PDF_CPT_FILE}" $PROJ $REGION -O -K -P $SHIFT >> "$t".ps
#      grdimage "${t}TempPDF.grd" -fg -C"${PDF_CPT_FILE}" $PROJ -Rg -O -K -P $SHIFT >> "$t".ps
#      echo "added  description" 
      fi

   else     #  otherwise this is not the first image (FIRSTLINE)
      let "LINE = $FIRST_TIME % 3"
#       echo "LINE = " $LINE
#   this manages the offset needed to get stereograms onto successive lines

      if [ $LINE -eq 0 ] ; then
         SHIFT=" -X-12c -Y-6.3c "
         if [ $3 = IF ] || [ $3 = DI ] || [ $3 = IX ] ; then
#           echo "LINE=0, test IF" ;
#  clip the region to plot
            CLIPPED=1
#           echo "at -X12c -Y-6.3c, CLIPPED = " $CLIPPED ;
#  now to clip
      if [ $m9 = CUBIC ] || [ $m9 = cubic ] ; then
            gmt psclip $PROJ $REGION -V -K -O -P $SHIFT << END >> "$t".ps
$STRING
END
fi
      if [ $m9 = HEX ] || [ $m9 = hex ] ; then
            gmt psclip $PROJ $REGION -V -K -O -P $SHIFT << END >> "$t".ps
$HEXSTRING
END
fi
            SHIFT=""
            #echo "SHIFT =  " $SHIFT
         fi  #  end of inv. PF section
           
         if  [ $3 != DP ] && [ $3 != DI ] && [ $3 != DL ] &&  [ $3 != IX ] && [ $3 != PX ] && [ $3 != P2 ] && [ $3 != DA ] ; then
#  if not discrete, then do this
            gmt grdimage "${t}TempPDF.grd" -fg -C"${PDF_CPT_FILE}" $PROJ $REGION -K -O -P $SHIFT >> "$t".ps
         fi
      else   #  LINE not eq 0
         SHIFT=" -X6c -Y0c "
         if [ $3 = IF ] || [ $3 = DI ] || [ $3 = IX ] ; then
#           echo "test IF" ;
#  clip the region to plot
            CLIPPED=1
#           echo "at -X6c -Y0c, CLIPPED = " $CLIPPED ;
      if [ $m9 = CUBIC ] || [ $m9 = cubic ] ; then
            gmt psclip $PROJ $REGION -V -K -O -P $SHIFT << END >> "$t".ps
$STRING
END
fi
      if [ $m9 = HEX ] || [ $m9 = hex ] ; then
            gmt psclip $PROJ $REGION -V -K -O -P $SHIFT << END >> "$t".ps
$HEXSTRING
END
fi
            SHIFT=""
#            echo "SHIFT =  " $SHIFT
         fi
         if  [ $3 != DP ] && [ $3 != DI ] && [ $3 != DL ] &&  [ $3 != IX ] && [ $3 != PX ] && [ $3 != P2 ] && [ $3 != DA ] ; then
#  if not discrete, then do this
            gmt grdimage "${t}TempPDF.grd" -fg -C"${PDF_CPT_FILE}" $PROJ $REGION -K -O -P $SHIFT >> "$t".ps
         fi
      fi  #  test whether LINE=0 or not
   fi  #  first image?

#   echo "made image, no labeling"

# Adds the contour line
   if  [ $3 != DP ] && [ $3 != DI ] && [ $3 != DL ] &&  [ $3 != IX ] && [ $3 != PX ] && [ $3 != P2 ] && [ $3 != DA ] && [ $DO_DISCRETE = "1" ] ; then
#  if not discrete, then do this
       gmt grdcontour "${t}TempPDF.grd" -fg -C"${PDF_CPT_FILE}" $PROJ $REGION -K -O -A- >>"$t".ps
   else
#  here we do the discrete plot
      if  [ $3 = DA ] ; then
          SHIFT=""
          #SHIFT=" -X6c -Y0c "
          if [ $COUNTER = 1 ] ; then 
              gmt psxy "${t}".da${COUNTER} -Ss0.3i -Gred $PROJ $REGION  -K -O -P $SHIFT >> "$t".ps
              gmt psxy "${t}".db${COUNTER} -Ss0.3i -Gblue $PROJ $REGION  -K -O -P $SHIFT >> "$t".ps
          fi
          if [ $COUNTER = 2 ] ; then
              gmt psxy "${t}".da${COUNTER} -St0.25i -Gred $PROJ $REGION  -K -O -P $SHIFT >> "$t".ps
              gmt psxy "${t}".db${COUNTER} -St0.25i -Gblue $PROJ $REGION  -K -O -P $SHIFT >> "$t".ps
          fi
          if [ $COUNTER = 3 ] ; then
              gmt psxy "${t}".da${COUNTER} -Sd0.2i -Gred $PROJ $REGION  -K -O -P $SHIFT >> "$t".ps
              gmt psxy "${t}".db${COUNTER} -Sd0.2i -Gblue $PROJ $REGION  -K -O -P $SHIFT >> "$t".ps
          fi
          if [ $COUNTER = 4 ] ; then
              gmt psxy "${t}".da${COUNTER} -Sc0.15i -Gred $PROJ $REGION  -K -O -P $SHIFT >> "$t".ps
              gmt psxy "${t}".db${COUNTER} -Sc0.15i -Gblue $PROJ $REGION  -K -O -P $SHIFT >> "$t".ps
          fi
          if [ $COUNTER = 5 ] ; then
              gmt psxy "${t}".da${COUNTER} -Sa0.13i -Gred $PROJ $REGION  -K -O -P $SHIFT >> "$t".ps
              gmt psxy "${t}".db${COUNTER} -Sa0.13i -Gblue $PROJ $REGION  -K -O -P $SHIFT >> "$t".ps
          fi
          if [ $COUNTER = 6 ] ; then
              gmt psxy "${t}".da${COUNTER} -S+0.10i -Wthin,red  $PROJ $REGION  -K -O -P $SHIFT >> "$t".ps
              gmt psxy "${t}".db${COUNTER} -S+0.10i -Wthin,blue  $PROJ $REGION  -K -O -P $SHIFT >> "$t".ps
          fi
      fi
      if  [ $3 = DP ] ; then
          gmt psxy "${t}"${COUNTER}.dpf -Sc0.03i $PROJ $REGION  -K -O -P $SHIFT >> "$t".ps
      fi
      if  [ $3 = DI ] ; then
          gmt psxy "${t}"${COUNTER}.dip -Sc0.03i $PROJ $REGION  -K -O -P $SHIFT >> "$t".ps
      fi
      if  [ $3 = DL ] ; then
          gmt pstext "${t}"${COUNTER}.txt $PROJ $REGION -N -K -O -P $SHIFT >> "$t".ps
      fi
      if  [ $3 = PX ] ; then
          gmt psxy "${t}"${COUNTER}.dpx -Svs0.025c/0.15c/0.05c -Gblue $PROJ $REGION  -K -O -P $SHIFT >> "$t".ps
#      -Svs draws a vector from col1+col2 to col3+col4
      fi
      if  [ $3 = P2 ] ; then
          gmt psxy "${t}"${COUNTER}.dpx -Svs0.025c/0.15c/0.05c -Gblue $PROJ $REGION  -K -O -P $SHIFT >> "$t".ps
          gmt psxy "${t}"${COUNTER}.dpx2 -Svs0.025c/0.15c/0.05c -Gred  $PROJ $REGION  -K -O -P       >> "$t".ps
#      -Svs draws a vector from col1+col2 to col3+col4
      fi
      if  [ $3 = IX ] ; then
          gmt psxy "${t}"${COUNTER}.ipx1 -Svs0.025c/0.15c/0.05c -Gblue $PROJ $REGION  -K -O -P $SHIFT >> "$t".ps
          gmt psxy "${t}"${COUNTER}.ipx2 -Svs0.025c/0.15c/0.05c -Gred $PROJ $REGION  -K -O -P         >> "$t".ps
#      -Svs draws a vector from col1+col2 to col3+col4
      fi
   fi     # end of discrete plot section

#   echo "added contour line"
# "-K" and "-O" means continuing on

#   echo "after drawing: CLIPPED = " $CLIPPED
   if [ $3 = IF ] || [ $3 = DI ] || [ $3 = IX ] ; then
       if [ $m9 = CUBIC ] || [ $m9 = cubic ] ; then
           gmt psclip $PROJ $REGION -V -K -O -C $SHIFT << END >> "$t".ps
$STRING
END
           if [ $CLIPPED -eq 1 ] ; then
               gmt psxy -Wthick $PROJ $REGION  -K -O -P $SHIFT << END >> "$t".ps
$STRING
END
           fi
       fi  #  CUBIC
       if [ $m9 = HEX ] || [ $m9 = hex ] ; then
           gmt psclip $PROJ $REGION -V -K -O -C $SHIFT << END >> "$t".ps
$HEXSTRING
END
           if [ $CLIPPED -eq 1 ] ; then
               gmt psxy -Wthick $PROJ $REGION  -K -O -P $SHIFT << END >> "$t".ps
$HEXSTRING
END
           fi
       fi  #  HEX
         let CLIPPED=0
      #fi
   fi

   if [ $3 = PF  ] || [ $3 = IF ] ||  [ $3 = DP ] || [ $3 = DI ] || [ $3 = DL ] || [ $3 = IX ] || [ $3 = PX ]  || [ $3 = P2 ] || [ $3 = DA ] ; then
      if [ $3 = IF  ] || [ $3 = DI ] || [ $3 = IX ] ; then
#         gmt pstext -K -O -N $REGION -D3.2i/2.9i $PROJ -F+a0+f16p,Helvetica,black+jLM << EOF >> "$t".ps
#-125 -30 ${HEADER:0:6}
#EOF
#         gmt pstext -K -O -N $REGION -D2.4i/3.3i $PROJ -F+a0+f16p,Helvetica,black+jLM << EOF >> "$t".ps
#-125 -30 001
#EOF
#         gmt pstext -K -O -N $REGION -D4.1i/3.3i $PROJ -F+a0+f16p,Helvetica,black+jLM << EOF >> "$t".ps
#-125 -30 101
#EOF
#         gmt pstext -K -O -N $REGION -D3.5i/5.1i $PROJ -F+a0+f16p,Helvetica,black+jLM << EOF >> "$t".ps
#-125 -30 111
#EOF
      if [ $m9 = CUBIC ] || [ $m9 = cubic ] ; then
         gmt pstext -K -O -N $REGION -D3.2i/2.9i $PROJ -F+a0+f16p,Helvetica,black+jLM << EOF >> "$t".ps
-125 -30 ${HEADER:0:6}
EOF
         gmt pstext -K -O -N $REGION -D2.4i/3.3i $PROJ -F+a0+f16p,Helvetica,black+jLM << EOF >> "$t".ps
-125 -30 001
EOF
         gmt pstext -K -O -N $REGION -D4.1i/3.3i $PROJ -F+a0+f16p,Helvetica,black+jLM << EOF >> "$t".ps
-125 -30 101
EOF
         gmt pstext -K -O -N $REGION -D3.5i/5.1i $PROJ -F+a0+f16p,Helvetica,black+jLM << EOF >> "$t".ps
-125 -30 111
EOF
      fi
      if [ $m9 = HEX ] || [ $m9 = hex ] ; then
         gmt pstext -K -O -N $REGION -D2.0i/3.1i $PROJ -F+a0+f16p,Helvetica,black+jLM << EOF >> "$t".ps
-125 -30 ${HEADER:0:6}
EOF
         gmt pstext -K -O -N $REGION -D1.55i/2.6i $PROJ -F+a0+f16p,Helvetica,black+jLM << EOF >> "$t".ps
-125 -30 0001
EOF
         gmt pstext -K -O -N $REGION -D3.2i/2.0i $PROJ -F+a0+f16p,Helvetica,black+jLM << EOF >> "$t".ps
-125 -30 2110
EOF
         gmt pstext -K -O -N $REGION -D2.9i/3.4i $PROJ -F+a0+f16p,Helvetica,black+jLM << EOF >> "$t".ps
-125 -30 1010
EOF
      fi  #  HEX
      fi   #   if IF
      if [ $3 = PF  ] ; then   #  do Pole Figures
         #echo "${HEADER} ${HEADER:0:6}"
         gmt pstext -K -O -N $REGION -D-0.2i/2.3i $PROJ -F+a0+f16p,Helvetica,black+jLM << EOF >> "$t".ps
-125 -30 ${HEADER:0:6}
EOF
         gmt psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> "$t".ps
0 0 
180 0 
END
         gmt psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> "$t".ps
90 0 
270 0 
END
      fi
      #  need something with offsets for the DA option (stacked PFs)
      if [ $3 = DA ] ; then   #  do discrete Pole Figures
#         pstext -K -O -N $REGION -D-0.2i/2.3i $PROJ << EOF >> "$t".ps
#-125 -30 16 0 0 LM  ${HEADER:0:6}
#EOF
         gmt psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> "$t".ps
0 0 
90 0 
180 0 
270 0 
360 0
END
         gmt psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> "$t".ps
0 0 
180 0 
END
         gmt psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> "$t".ps
90 0 
270 0 
END
      fi
      if [ $3 = DP ] || [ $3 = PX ] || [ $3 = P2 ] ; then   #  do discrete Pole Figures
         gmt pstext -K -O -N $REGION -D-0.2i/2.3i $PROJ -F+a0+f16p,Helvetica,black+jLM << EOF >> "$t".ps
-125 -30 ${HEADER:0:6}
EOF
         gmt psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> "$t".ps
0 0 
90 0 
180 0 
270 0 
360 0
END
         gmt psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> "$t".ps
0 0 
180 0 
END
         gmt psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> "$t".ps
90 0 
270 0 
END
      fi
      if [ $3 = DL  ] ; then   #  do discrete Pole Figures with labels
         gmt pstext -K -O -N $REGION -D-0.2i/2.3i $PROJ -F+a0+f16p,Helvetica,black+jLM << EOF >> "$t".ps
-125 -30 ${HEADER:1:6}
EOF
         gmt psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> "$t".ps
0 0 
90 0 
180 0 
270 0 
360 0
END
         gmt psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> "$t".ps
0 0 
180 0 
END
         gmt psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> "$t".ps
90 0 
270 0 
END
         if [ $m9 = CUBIC ] || [ $m9 = cubic ] ; then
#  default to cubic
            gmt psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> "$t".ps
45 0 
45 90 
225 0
END
            gmt psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> "$t".ps
0 0 
90 45 
180 0
END
            gmt psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> "$t".ps
0 0 
270 45 
180 0
END
            gmt psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> "$t".ps
135 0 
45 90 
315 0
END
            gmt psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> "$t".ps
90 0 
0 45 
270 0
END
            gmt psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> "$t".ps
90 0 
180 45 
270 0
END
            gmt psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> "$t".ps
90 0 
270 0 
END
            psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> "$t".ps
0 0 
180 0 
END
         fi
      fi
#   fi
   else   #  do 2d / 5d 
      gmt pstext -K -O -N $REGION $TXTCOORDS $PROJ -F+a0+f16p,Helvetica,black+jLM << EOF >>"$t".ps
-125 -30 ${HEADER:0:18}
EOF
   fi

   if [ $3 = 5d ] || [ $3 = 2d ] ; then
      if [ $m9 = HEX ] || [ $m9 = hex ] ; then
         gmt psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> "$t".ps
0 0 
180 0 
END
         gmt psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> "$t".ps
60 0 
240 0 
END
         gmt psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> "$t".ps
120 0 
300 0 
END
         gmt psxy -Wthinnest,white  $PROJ $REGION -O -K -P  << END >> "$t".ps
30 0 
210 0 
END
         gmt psxy -Wthinnest,white  $PROJ $REGION -O -K -P  << END >> "$t".ps
90 0 
270 0 
END
         gmt psxy -Wthinnest,white  $PROJ $REGION -O -K -P  << END >> "$t".ps
150 0 
330 0 
END
      fi
      if [ $m9 = ORT ] || [ $m9 = ort ] ; then
         echo "psxy1"
         gmt psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> "$t".ps
90 0 
270 0 
END
         echo "psxy2"
         gmt psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> "$t".ps
0 0 
180 0 
END
      fi
      if [ $m9 = CUBIC ] || [ $m9 = cubic ] ; then
#  default to cubic
         gmt psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> "$t".ps
45 0 
45 90 
225 0
END
         gmt psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> "$t".ps
0 0 
90 45 
180 0
END
         gmt psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> "$t".ps
0 0 
270 45 
180 0
END
         gmt psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> "$t".ps
135 0 
45 90 
315 0
END
         gmt psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> "$t".ps
90 0 
0 45 
270 0
END
         gmt psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> "$t".ps
90 0 
180 45 
270 0
END
         gmt psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> "$t".ps
90 0 
270 0 
END
         gmt psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> "$t".ps
0 0 
180 0 
END
      fi
#      echo "psxy3"
      gmt psxy -Wthick,black  $PROJ $REGION -O -K -P  << END >> "$t".ps
0 0
90 0 
180 0
270 0
360 0 
END
   fi

   let COUNTER=COUNTER+1 
   let FIRST_TIME=FIRST_TIME+1 

done     
#  end of the WHILE loop

echo "Figure creation Complete"

echo "Converting Postscript file to PNG graphic file"
convert "${t}".ps "${t}".png

#  changed to JPG Dec 06 - more efficient than pdf
#  changed again, to GIF, Jun 08, for Intel Macs
#  changed again, to PNG, Oct 09

#  if [ $? -eq 0 ] ; then
#  successful convert, hopefully
#     rm "${t}".ps
#  fi
echo "Removing Temp Files"
if  [ $3 != DP ] && [ $3 != DI ] && [ $3 != DL ] && [ $3 != DI ] && [ $3 != IX ] && [ $3 != PX ] && [ $3 != P2 ] && [ $3 != DA ]  ; then
    rm "${t}TempPDFnn.grd"
    rm "${t}TempPDF.grd"
    rm "${t}gridinfo.txt"
fi
#rm TempPDF.out
rm "${PDF_CPT_FILE}"
             
# __________________________________
#  left over code .....

# pstext  -K -O -N -R0/360/0/90 -D3i/2i -JA-90/90/5>>"$t".ps
# 5 Parameter Analysis plotted by GMT
# echo "added title"
# "-K" and "-O" means continuing on

#  if "$5"=="nospot" goto nospot
#    if [ "$8" = "spot" ] ; then
#       echo "adding the spot at the max."
# Add maximum and spot at the location
#       psxy PDFMax.txt -K -O -Sc0.25i -N -G0/0/255 -R0/360/0/90 -JA-90/90/5>>"$t".ps
# psxy "max" -K -O -Sc0.25i -N -G0/0/255 -R0/360/0/90 -JA-90/90/5>>"$t".ps
# "-K" and "-O" means continuing on

#    fi

# :nospot

# Make EPS file and  add preview
#  on Mac OS X, use other program to do the conversions to EPS
# epstool -b -zbmp256 -o$1.eps -t6p $1.ps

# Make PDF file
# perl c:\utils\epstopdf.pl --outfile=$1.pdf $1.eps

# psclip -JA-90/90/5c -R0/360/0/90 -V -K -O -P << END >>"$t".ps
# 0 90
# 0 45
# 45 35.3
# 0 90
#END


exit 0
