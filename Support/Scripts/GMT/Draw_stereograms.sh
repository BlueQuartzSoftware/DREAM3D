#!/bin/bash 

#  adapted from DOS batch file supplied by Sven Vogel, LANL
#  edited to handle multiple Pole figures, ADR  x 04
#  switched from sh to bash
#  last edit:  2 Jan 07, ADR
#  last edit:  2 Jul 08, ADR; discrete PFs
#  added DL option, 30 i 10, ADR; plot PFs with labels
#  1 Dec 12, GSR; continuous contours, find "gsr note:" to reverse
#  last edit:  11 Dec 12, sutatch; helps eliminate equator discontinuity, see lines 125-127

echo "last revised 11 Dec 12"

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

if [ $# -eq 0 ]; then
   echo " usage= ./Draw_stereograms number_of_files Stereo_file_name [2d/5d/PF/IF] [wysiwyg/gray/polar/...] [Lo] [Hi] [step] [stereo/equal/ortho]  [CUBIC/HEX/ORT]"
   echo " If #3='2d' then it looks for only one file (with extension .dat )"
   echo " If #3='5d' then it looks for several stereogram files (with extension .dat )"
   echo " If #3='PF' it looks for pole figure files (with extension .gpf )"
   echo " If #3='IF' it looks for inverse pole figure files (with extension .gpf )"
   echo " If #3='DP' it looks for discrete pole figure files (with extension .dpf )"
   echo " If #3='DI' it looks for discrete inv. pole figure files (with extension .dip )"
   echo " If #3='DL' it looks for discrete PFs with PSTEXT lines (with extension .txt )"
   echo " If #9='HEX' then it draws lines for hexagonal symmetry instead of cubic "
   echo " If #9='ORT' then it draws lines for orthorhombic symmetry instead of cubic "
   exit

else

    echo " Script for drawing stereograms from 5-parameter GB analysis, or pole figures"
    echo " Note that the input files (data) must have longitude, latitude, intensity"
    echo " Tony Rollett, CMU, with thanks to Sven Vogel, Los Alamos "

    if [ $# -ge 3 ]; then
       if [ $3 != 2d ] && [ $3 != 5d ] && [ $3 != PF ] && [ $3 != IF ] && [ $3 != DP ]  && [ $3 != DI ]  && [ $3 != DL ]; then
       	  echo "data can only be 2d, 5d, PF, IF, DP, DI or DL"
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
#  echo "var1 (with everything from the last . on stripped out) = $t"
echo "filename base: " $t
#  $t now contains the basename
#  exit 0
   rm ${t}.ps
#  get rid of any PS file left over

#  makecpt -Cwysiwyg -I -T0.0/3/0.25 > PDF.cpt

if [ $# -lt 7 ]; then
   TEMP="1"   # default for later use
else
    TEMP=$7
fi

BORDER="-Bwesn"
PROJ="-JS-90/90/5c"
#  REGION="-Rg"
#  let's try using Global consistently
#  PROJ="-JS-90/90/5c"
#g REGION="-R0/360/0/90" 
#g REGION modified to partially solve discontinuity at the equator
REGION="-R-180/180/0/90"
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

      y=$7
	  z=$(echo "scale=9; $y/10.0" | bc)
	  #z=$7
      echo "z =" $z

if [ $# -ge 7 ]; then
    makecpt -C"$COLORSCHEME"  -T"$5"/"$6"/"$z" > PDF.cpt
#  was -T0.0/3/0.5
else
    makecpt -C"$COLORSCHEME"  -T0.0/8./1.0 > PDF.cpt
#    makecpt -C"$COLORSCHEME"  -Qo -V > PDF.cpt
#  tried the log scale (Q) option briefly but it's basically for powers of 10
fi

echo "You requested " $1 " stereograms to be drawn"

COUNTER=1
OFFSET=0.0i       #  offset for plotting - different for Inv PFs
Y_OFFSET=6.5i   #  y offset
FIRST_TIME=0    #  to track whether it's the first plot or not, and for offsets
CLIPPED=0   #  did we clip?

if [ $3 = IF ] || [ $3 = DI ] ; then
   OFFSET=-4.0i
   Y_OFFSET=3.5i
   PROJ="-JA-90/90/18c"
fi

# echo 'grdimage TempPDF.grd -CPDF.cpt -JA-90/90/5 -Bwesn -K -P'
# echo grdimage -X$COUNTER

while [  $COUNTER -le $1 ]; do
   echo  
   echo Working on stereogram number ${COUNTER}
# echo grdimage TempPDF.grd -CPDF.cpt -JA-90/90/5 -Bwesn -K -P -X$COUNTER"i" -Y3i
#             let OFFSET=COUNTER*3+2
#  echo offset = $OFFSET cm
#             let COUNTER=COUNTER+1 
#  incremented below

   if [ $3 = 2d ] ; then
      echo "current input file: " ${t}.dat
      read HEADER < ${t}.dat
#  nearneighbor $t.st$COUNTER -GTempPDF.grd -I3/3 -R0/360/0/90 -H1 -Lxy -S15
#  nearneighbor ${t}_5d_degr${COUNTER}.dat -GTempPDF.grd -I3/3 -R0/360/0/90 -H1 -Lxy -S15
#	nearneighbor ${t}.dat -GTempPDF.grd -I3/3 -R0/360/0/90 -H1 -S15
      nearneighbor ${t}.dat -GTempPDFnn.grd -I3/3 -fg -Rg -H1 -S15
#  Region = global, seems to eliminate the problem of contours not crossing the x-axis
   fi

   if [ $3 = PF ] ||  [ $3 = IF ] ; then
       if [ -e ${t}${COUNTER}.gpf ]
       then
	   echo "current input file: " ${t}${COUNTER}.gpf
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
	   echo "you asked for " ${t}${COUNTER}.gpf " but it does not exist!  Exiting ..."
	   exit 1
       fi
      read HEADER < ${t}${COUNTER}.gpf
#  nearneighbor $t.st$COUNTER -GTempPDF.grd -I3/3 -R0/360/0/90 -H1 -Lxy -S15
#  nearneighbor ${t}_5d_degr${COUNTER}.dat -GTempPDF.grd -I3/3 -R0/360/0/90 -H1 -Lxy -S15
#	nearneighbor ${t}${COUNTER}.gpf -GTempPDF.grd -I3/3 -R0/360/0/90 -H1 -S15
#	nearneighbor ${t}${COUNTER}.gpf -GTempPDFnn.grd -fg -I3/3 $REGION -H1 -S15
	nearneighbor ${t}${COUNTER}.gpf -GTempPDFnn.grd -fg -I3/3 -Rg -H1 -S15
   fi

   if  [ $3 = DP ]; then
       if [ -e ${t}${COUNTER}.dpf ]
       then
	   echo "current input file: " ${t}${COUNTER}.dpf
       else
	   echo "you asked for " ${t}${COUNTER}.dpf " but it does not exist!  Exiting ..."
	   exit 1
       fi
      read HEADER < ${t}${COUNTER}.dpf
#  no need to do nearneighbor on discrete PFs
   fi

   if  [ $3 = DI ]; then
       if [ -e ${t}${COUNTER}.dip ]
       then
	   echo "current input file: " ${t}${COUNTER}.dip
       else
	   echo "you asked for " ${t}${COUNTER}.dip " but it does not exist!  Exiting ..."
	   exit 1
       fi
      read HEADER < ${t}${COUNTER}.dip
#  no need to do nearneighbor on discrete IPFs
   fi

   if  [ $3 = DL ]; then
       if [ -e ${t}${COUNTER}.txt ]
       then
	   echo "current input file: " ${t}${COUNTER}.txt
       else
	   echo "you asked for " ${t}${COUNTER}.txt " but it does not exist!  Exiting ..."
	   exit 1
       fi
      read HEADER < ${t}${COUNTER}.txt
#  no need to do nearneighbor on discrete PFs for PSTEXT
   fi

   if [ $3 = 5d ] ; then
       if [ -e ${t}${COUNTER}.dat ]
       then
	   echo "current input file: " ${t}${COUNTER}.dat
       else
	   echo "you asked for " ${t}${COUNTER}.dat " but it does not exist!"
	   exit 1
       fi
#      echo "current input file: " ${t}${COUNTER}.dat
      read HEADER < ${t}${COUNTER}.dat
#  nearneighbor $t.st$COUNTER -GTempPDF.grd -I3/3 -R0/360/0/90 -H1 -Lxy -S15
#  nearneighbor ${t}_5d_degr${COUNTER}.dat -GTempPDF.grd -I3/3 -R0/360/0/90 -H1 -Lxy -S15
#	nearneighbor ${t}${COUNTER}.dat -GTempPDF.grd -I3/3 -R0/360/0/90 -H1 -S15

      nearneighbor ${t}${COUNTER}.dat -GTempPDFnn.grd -fg -I3/3 -Rg -H1 -S15
#      if [ $# -ge 8 ] && [ $8 = ortho ] ; then
# defaults to stereo projection - similar below
#	nearneighbor ${t}${COUNTER}.dat -GTempPDFnn.grd -I3/3 -Rg -H1 -S15
#      else
#	nearneighbor ${t}${COUNTER}.dat -GTempPDFnn.grd -I3/3 -R0/360/0/90 -H1 -S15
#      fi
   fi  
#  end of 5d section
   echo "1st line =" $HEADER

#  nearneighbor $t.st$COUNTER -GTempPDF.grd -I3/3 -R0/360/0/90 -H1 -Lxy -S15
#  nearneighbor ${t}_5d_degr${COUNTER}.dat -GTempPDF.grd -I3/3 -R0/360/0/90 -H1 -Lxy -S15
#  -H = 1 header line
#  -R  = range
#  -I  grid spacing
#  -S  search radius in same units as I
#  -L  periodic in x and/or y

   if  [ $3 != DP ] && [ $3 != DI ] && [ $3 != DL ] ; then

#    grd2xyz TempPDF.grd>TempPDF.out
# surface TempPDF.out -GTempPDF.grd -T0.5 -I.5/.5 -R0/360/0/90 -H1 -L
#    surface TempPDF.out -GTempPDF.grd -T0.9 -I.5/.5 -R0/360/0/90 -H1 -f0x,1y
#  was -T1  =  tension
#  -f0x,1y  defines longitude, latitude

#  this version pipes the output from grd2xyz (faster?)
#  and also uses blockmedian to perform a bit of smoothing
#    grd2xyz TempPDFnn.grd |  surface -GTempPDF.grd -T0.9 -I.5/.5 -R0/360/0/90 -H1 -f0x,1y

#   grd2xyz TempPDFnn.grd | blockmedian $REGION -I5 | surface -GTempPDF.grd -fg -T0.9 -I.5/.5 $REGION -H1 -f0x,1y
   grd2xyz TempPDFnn.grd | blockmedian -Rg -I5 | surface -GTempPDF.grd -fg -T0.9 -I.5/.5 $REGION -H1 -f0x,1y

#    if [ $# -ge 8 ] && [ $8 = ortho ] ; then
#       grd2xyz TempPDFnn.grd | blockmedian -Rg -I5 | surface -GTempPDF.grd -T0.9 -I.5/.5 -Rg -H1 -f0x,1y
#    else
#       grd2xyz TempPDFnn.grd | blockmedian -R0/360/0/90 -I5 | surface -GTempPDF.grd -T0.9 -I.5/.5 -R0/360/0/90 -H1 -f0x,1y
#    fi

    grdinfo TempPDF.grd -C -M > gridinfo.txt

   fi
#  end of gridding (not for discrete plots)

# MakePDFInfo $t $3
#
# Generates color codes for contours
# -T command parameters are lowest, highest and step

   if [ $FIRST_TIME -eq 0 ] ; then
      LINE=0
#      echo "1st plot, LINE = " $LINE ;

#  this is the first plot

   if  [ $3 != DP ] && [ $3 != DI ] && [ $3 != DL ] ; then

      psscale -CPDF.cpt -D2.5i/9.9i/4i/.25ih -B"$TEMP":"Multiples of Random": -K -P > $t.ps

      pstext -K -O -P -N -R0/11/0/11 -JX11i -Gblue <<EOF >> $t.ps
.15 10.3 16 0 0 LM Filename root: $t
EOF

      else

      pstext -K -P -N -R0/11/0/11 -JX11i -Gblue <<EOF > $t.ps
.15 10.3 16 0 0 LM Filename root: $t
EOF

#  do not want the scale for discrete plots
   fi

      psxy -K -P -O -N -R0/11/0/11 -JX11i -Sv -Gblue -W <<EOF >> $t.ps
5. 10.4 0. 2.
EOF
      pstext -K -P -O -N -R0/11/0/11 -JX11i -Gblue <<EOF >>$t.ps
6 10.4 16 0 0 LM X
EOF

      if [ $m8 = equal ] ; then
	  pstext -K -O -N -R0/11/0/11 -JX11i  <<EOF >>$t.ps
5.5 9.9 16 0 0 LM Equal Area
EOF
      fi
      if [ $m8 = ortho ] ; then
	  pstext -K -O -N -R0/11/0/11 -JX11i  <<EOF >>$t.ps
5.5 9.9 16 0 0 LM Orthographic
EOF
      fi
      if [ $m8 = stereo ] ; then
	  pstext -K -O -N -R0/11/0/11 -JX11i  <<EOF >>$t.ps
5.5 9.9 16 0 0 LM Stereographic
EOF
      fi

      SHIFT=" -X"$OFFSET" -Y"$Y_OFFSET
      if [ $3 = IF ] || [ $3 = DI ] ; then
#	 echo "test IF" ;
#  clip the region to plot
         CLIPPED=1
#	 echo "at CLIP, CLIPPED = " $CLIPPED "SHIFT= " $SHIFT ;
#         psxy  $PROJ $REGION $BORDER -K -P $SHIFT << END > $t.ps
         psclip  $PROJ $REGION -K -O -P $SHIFT << END >> $t.ps
$STRING
END
	 SHIFT=""
#	 echo "SHIFT =  " $SHIFT
      fi
#  end of IF section

   if  [ $3 != DP ] && [ $3 != DI ] && [ $3 != DL ] ; then
#  if not discrete, then do this
      grdimage TempPDF.grd -fg -CPDF.cpt $PROJ $REGION -O -K -P $SHIFT >> $t.ps
#      grdimage TempPDF.grd -fg -CPDF.cpt $PROJ -Rg -O -K -P $SHIFT >> $t.ps
#      echo "added  description" 
   fi

   else     #  otherwise this is not the first image (FIRSTLINE)
      let "LINE = $FIRST_TIME % 3"
#	echo "LINE = " $LINE

      if [ $LINE -eq 0 ] ; then
         SHIFT=" -X-12c -Y-6.3c "
         if [ $3 = IF ] || [ $3 = DI ] ; then
#	    echo "LINE=0, test IF" ;
#  clip the region to plot
            CLIPPED=1
#	    echo "at -X12c -Y-6.3c, CLIPPED = " $CLIPPED ;
#  now to clip
            psclip $PROJ $REGION -V -K -O -P $SHIFT << END >> $t.ps
$STRING
END
	    SHIFT=""
	    echo "SHIFT =  " $SHIFT
         fi
           
	 if  [ $3 != DP ] && [ $3 != DI ] && [ $3 != DL ] ; then
#  if not discrete, then do this
	     grdimage TempPDF.grd -fg -CPDF.cpt $PROJ  $REGION -K -O -P $SHIFT >> $t.ps
	 fi
      else   #  LINE not eq 0
         SHIFT=" -X6c -Y0c "
         if [ $3 = IF ] || [ $3 = DI ] ; then
#	    echo "test IF" ;
#  clip the region to plot
            CLIPPED=1
#	    echo "at -X6c -Y0c, CLIPPED = " $CLIPPED ;
            psclip $PROJ $REGION -V -K -O -P $SHIFT << END >> $t.ps
$STRING
END
	    SHIFT=""
#	    echo "SHIFT =  " $SHIFT
         fi
	 if  [ $3 != DP ] && [ $3 != DI ] && [ $3 != DL ] ; then
#  if not discrete, then do this
             grdimage TempPDF.grd -fg -CPDF.cpt $PROJ  $REGION -K -O -P $SHIFT >> $t.ps
	 fi
      fi  #  test whether LINE=0 or not
   fi  #  first image?

#   echo "made image, no labeling"

# Adds the contour line (add #g to remove lines)
#g   if  [ $3 != DP ] && [ $3 != DI ] && [ $3 != DL ] ; then
#  if not discrete, then do this
#g       grdcontour TempPDF.grd -fg -CPDF.cpt $PROJ $REGION -K -O -A- >>$t.ps
#g   else
#  here we do the discrete plot
#g       if  [ $3 = DP ] ; then
#g	   psxy ${t}${COUNTER}.dpf -H1 -Sc0.03i $PROJ $REGION  -K -O -P $SHIFT >> $t.ps
#g       fi
#g      if  [ $3 = DI ] ; then
#g	   psxy ${t}${COUNTER}.dip -H1 -Sc0.03i $PROJ $REGION  -K -O -P $SHIFT >> $t.ps
#g       fi
#g       if  [ $3 = DL ] ; then
#g	   pstext ${t}${COUNTER}.txt -H1 $PROJ $REGION -N -K -O -P $SHIFT >> $t.ps
#g       fi
#g   fi

#   echo "added contour line"
# "-K" and "-O" means continuing on

#   echo "after drawing: CLIPPED = " $CLIPPED
   if [ $3 = IF ] || [ $3 = DI ] ; then
       if [ $CLIPPED -eq 1 ] ; then
	   psxy -Wthick $PROJ $REGION  -K -O -P $SHIFT << END >> $t.ps
$STRING
END
	   psclip  $PROJ $REGION -V -K -C -O >> $t.ps
#      psclip -K -O -C >> $t.ps
	   let CLIPPED=0
       fi
   fi

   if [ $3 = PF  ] || [ $3 = IF ] ||  [ $3 = DP ] || [ $3 = DI ] || [ $3 = DL ] ; then
      if [ $3 = IF  ] || [ $3 = DI ] ; then
         pstext -K -O -N $REGION -D3.2i/2.9i $PROJ << EOF >> $t.ps
-125 -30 16 0 0 LM ${HEADER:0:6}
EOF
         pstext -K -O -N $REGION -D2.4i/3.3i $PROJ << EOF >> $t.ps
-125 -30 16 0 0 LM 001
EOF
         pstext -K -O -N $REGION -D4.1i/3.3i $PROJ << EOF >> $t.ps
-125 -30 16 0 0 LM 101
EOF
         pstext -K -O -N $REGION -D3.5i/5.1i $PROJ << EOF >> $t.ps
-125 -30 16 0 0 LM 111
EOF
      fi
      if [ $3 = PF  ] ; then   #  do Pole Figures
         pstext -K -O -N $REGION -D-0.2i/2.3i $PROJ << EOF >> $t.ps
-125 -30 16 0 0 LM  ${HEADER:0:6}
EOF
	 psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> $t.ps
0 0 
180 0 
END
	 psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> $t.ps
90 0 
270 0 
END
      fi
      if [ $3 = DP  ] ; then   #  do discrete Pole Figures
         pstext -K -O -N $REGION -D-0.2i/2.3i $PROJ << EOF >> $t.ps
-125 -30 16 0 0 LM  ${HEADER:0:6}
EOF
	 psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> $t.ps
0 0 
90 0 
180 0 
270 0 
360 0
END
	 psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> $t.ps
0 0 
180 0 
END
	 psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> $t.ps
90 0 
270 0 
END
      fi
      if [ $3 = DL  ] ; then   #  do discrete Pole Figures with labels
         pstext -K -O -N $REGION -D-0.2i/2.3i $PROJ << EOF >> $t.ps
-125 -30 16 0 0 LM  ${HEADER:1:6}
EOF
	 psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> $t.ps
0 0 
90 0 
180 0 
270 0 
360 0
END
	 psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> $t.ps
0 0 
180 0 
END
	 psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> $t.ps
90 0 
270 0 
END
	 if [ $m9 = CUBIC ] || [ $m9 = cubic ] ; then
#  default to cubic
	     psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> $t.ps
45 0 
45 90 
225 0
END
	     psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> $t.ps
0 0 
90 45 
180 0
END
	     psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> $t.ps
0 0 
270 45 
180 0
END
	     psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> $t.ps
135 0 
45 90 
315 0
END
	     psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> $t.ps
90 0 
0 45 
270 0
END
	     psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> $t.ps
90 0 
180 45 
270 0
END
	     psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> $t.ps
90 0 
270 0 
END
	     psxy -Wthin,black  $PROJ $REGION -O -K -P  << END >> $t.ps
0 0 
180 0 
END
	 fi
      fi
#   fi
   else   #  do 2d / 5d 
      pstext -K -O -N $REGION $TXTCOORDS $PROJ << EOF >>$t.ps
-125 -30 16 0 0 LM ${HEADER:0:18}
EOF
   fi

   if [ $3 = 5d ] || [ $3 = 2d ] ; then
       if [ $m9 = HEX ] || [ $m9 = hex ] ; then
      psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> $t.ps
0 0 
180 0 
END
      psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> $t.ps
60 0 
240 0 
END
      psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> $t.ps
120 0 
300 0 
END
      psxy -Wthinnest,white  $PROJ $REGION -O -K -P  << END >> $t.ps
30 0 
210 0 
END
      psxy -Wthinnest,white  $PROJ $REGION -O -K -P  << END >> $t.ps
90 0 
270 0 
END
      psxy -Wthinnest,white  $PROJ $REGION -O -K -P  << END >> $t.ps
150 0 
330 0 
END
       fi
       if [ $m9 = ORT ] || [ $m9 = ort ] ; then
      psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> $t.ps
90 0 
270 0 
END
      psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> $t.ps
0 0 
180 0 
END
       fi
       if [ $m9 = CUBIC ] || [ $m9 = cubic ] ; then
#  default to cubic
      psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> $t.ps
45 0 
45 90 
225 0
END
      psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> $t.ps
0 0 
90 45 
180 0
END
      psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> $t.ps
0 0 
270 45 
180 0
END
      psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> $t.ps
135 0 
45 90 
315 0
END
      psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> $t.ps
90 0 
0 45 
270 0
END
      psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> $t.ps
90 0 
180 45 
270 0
END
      psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> $t.ps
90 0 
270 0 
END
      psxy -Wthin,white  $PROJ $REGION -O -K -P  << END >> $t.ps
0 0 
180 0 
END
       fi
      psxy -Wthick,black  $PROJ $REGION -O -K -P  << END >> $t.ps
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

echo "done: now to convert and clean up graphics files"

convert ${t}.ps ${t}.png
#  changed to JPG Dec 06 - more efficient than pdf
#  changed again, to GIF, Jun 08, for Intel Macs
#  changed again, to PNG, Oct 09

#  if [ $? -eq 0 ] ; then
#  successful convert, hopefully
#     rm ${t}.ps
#  fi

if  [ $3 != DP ] && [ $3 != DI ] && [ $3 != DL ] ; then
    rm TempPDFnn.grd
    rm TempPDF.grd
    rm gridinfo.txt
fi
# rm TempPDF.out
rm PDF.cpt
	     
# __________________________________
#  left over code .....

# pstext  -K -O -N -R0/360/0/90 -D3i/2i -JA-90/90/5>>$t.ps
# 5 Parameter Analysis plotted by GMT
# echo "added title"
# "-K" and "-O" means continuing on

#  if "$5"=="nospot" goto nospot
#    if [ "$8" = "spot" ] ; then
#	echo "adding the spot at the max."
# Add maximum and spot at the location
#	psxy PDFMax.txt -K -O -Sc0.25i -N -G0/0/255 -R0/360/0/90 -JA-90/90/5>>$t.ps
# psxy "max" -K -O -Sc0.25i -N -G0/0/255 -R0/360/0/90 -JA-90/90/5>>$t.ps
# "-K" and "-O" means continuing on

#    fi

# :nospot

# Make EPS file and  add preview
#  on Mac OS X, use other program to do the conversions to EPS
# epstool -b -zbmp256 -o$1.eps -t6p $1.ps

# Make PDF file
# perl c:\utils\epstopdf.pl --outfile=$1.pdf $1.eps

# psclip -JA-90/90/5c -R0/360/0/90 -V -K -O -P << END >>$t.ps
# 0 90
# 0 45
# 45 35.3
# 0 90
#END


exit 0

