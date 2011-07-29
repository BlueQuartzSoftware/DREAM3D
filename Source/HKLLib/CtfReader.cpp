/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "CtfReader.h"

CtfReader::CtfReader()
{
  // TODO Auto-generated constructor stub

}

CtfReader::~CtfReader()
{
  // TODO Auto-generated destructor stub
}


// hkl2tsl.cxx

//   g++ -o hkl2tsl hkl2tsl.cxx

#include<iostream>
#include <iomanip>
#include<fstream>
#include<cmath>
#include<sstream>
#include<vector>
using namespace std;

struct phase_struct{
  double a, b, c, alpha, beta, gamma;
  string MaterialName, Formula, Symmetry;
};

#if 0
int main(int argc, char* argv[])
{
  // check argument list



  // open file objects
  ifstream hkl(argv[1]);
  ofstream tsl(argv[2]);


  // HKL header data, in order
  string Channel, Prj, Author, JobMode;
  int    XCells, YCells;
  double XStep, YStep, AcqE1, AcqE2, AcqE3;
  int    Mag, Coverage, Device, KV, TiltAngle, TiltAxis;
  int    Phases;

  // TSL header data, in order
  double TEM_PIXperUM, xstar, ystar, zstar, WorkingDistance;
  string GRID;
  double XSTEP, YSTEP;
  int    NCOLS_ODD, NCOLS_EVEN, NROWS;
  string OPERATOR, SAMPLEID, SCANID;

  vector<phase_struct> phases;

  int iqPhi2 ;
  double tmp ;

  // read HKL header


  //  iqPhi2 = 1 ;
  // do subtract 30 degr from phi-2

  cout<<"This converts an HKL .ctf file to a TSL .ang "<<" \n";
  cout<<"note that 30 degrees probably should be subtracted from the "<<" \n";
  cout<<" 3rd Euler angle (phi 2) "<<" \n";
  cout<<"This is almost certainly required to go from systems with Cartesian-X  \n" ;
  cout<<" aligned parallel to 1 0 -1 0, into TSL with X // 2 -1 -1 0   \n" ;
  cout<<"90 degrees is added to the 1st Euler angle (phi1) "<<" \n";
  cout<<"and the 2nd Euler angle (PHI) is negated "<<" \n\n";
  //  was addition of 90 degrees, but 23 jun 08, subtracted instead
  cout<<"You have to choose the 30 degr offset for yourself"<<" \n\n";
  cout<<"Mainly written by Jason Gruber, "<<" \n";
  cout<<" with mods by AD Rollett, March 2007"<<"\n\n";
  cout<<" Subtract 30 degrees from phi-2 [1 = yes] ?"<<"\n" ;
  cin>>iqPhi2 ;

  cout<<"check iqPhi2: "<<iqPhi2<<"\n" ;

  while (1) {
    string line, label;
    getline(hkl,line);
    stringstream sstream(line) ;
    sstream>>label;
    //    cout << "label: " << label << endl ;
    if (label=="Channel") {
      sstream>>Channel ;
      //      cout <<"found Channel label" << endl ;
    }
    if (label=="Prj")     sstream>>Prj;
    if (label=="Author")  sstream>>Author;
    Author = "oimuser" ;
    //  per HMM's list
    if (label=="JobMode") sstream>>JobMode;
    if (label=="XCells")  sstream>>XCells;
    if (label=="YCells")  sstream>>YCells;
    if (label=="XStep")   sstream>>XStep;
    if (label=="YStep")   sstream>>YStep;
    if (label=="AcqE1")   sstream>>AcqE1;
    if (label=="AcqE2")   sstream>>AcqE2;
    if (label=="AcqE3")   sstream>>AcqE3;
    if (label=="Euler") {
      //      cout <<"found Euler" << endl ;
      sstream>>label>>label>>label>>label;
      sstream>>label>>label>>label;
      sstream>>label>>Mag;
      sstream>>label>>Coverage;
      sstream>>label>>Device;
      sstream>>label>>KV;
      sstream>>label>>TiltAngle;
      sstream>>label>>TiltAxis;
    }
    if (label=="Phases") {
      sstream>>Phases;
      break;
    }
  }
  phases.resize(Phases+1);
  for (int i=1; i<=Phases; i++) {
    cout << "Phase no. " << i ;
    string line ;
    getline(hkl,line) ;
    int p = line.find(',',0) ;
    while (p!=string::npos) {line[p]='.'; p=line.find(',',p);}
    p = line.find(';',0) ;
    while (p!=string::npos) {line[p]=' '; p=line.find(';',p);}
    stringstream sstream(line) ;
    sstream>>phases[i].a>>phases[i].b>>phases[i].c ;
    sstream>>phases[i].alpha>>phases[i].beta>>phases[i].gamma ;
    sstream>>phases[i].MaterialName ;
    phases[i].Formula = "" ;
    //    phases[i].Symmetry = "6/mmm" ;
    phases[i].Symmetry = "6" ;
    //  not provided by HKL
  }
  hkl.ignore(1000,'\n');


  // convert HKL header data to TSL header data
  TEM_PIXperUM = 1.0 ;
  // ?
  //  xstar = AcqE1 ;
  //  ystar = AcqE2 ;
  //  zstar = AcqE3 ;
  //  xstar = 0.473 ;
  //  ystar = 0.5773 ;
  //  zstar = 0.6503 ;
  // per HMM's list
  xstar = 240. ;
  ystar = 240. ;
  zstar = 240. ;
  //  per the example SquareGrid.ang
  WorkingDistance = 17.0 ;
  // per HMM's list
  GRID = "SqrGrid" ;
  XSTEP = XStep ;
  YSTEP = YStep ;
  //  NCOLS_ODD = XCells ;
  //  NCOLS_EVEN = XCells ;
  NCOLS_ODD = XCells/2 ;
  NCOLS_EVEN = XCells/2 ;
  //  for an actual hex grid, this would require more care!
  NROWS = YCells ;
  OPERATOR = Author ;
  SAMPLEID = "" ;
  //  SCANID = Prj ;
  SCANID = "" ;
  // per HMM's list


  // write TSL header
  tsl<<"# TEM_PIXperUM            "<<TEM_PIXperUM<<"\n";
  tsl<<"# x-star                  "<<xstar<<"\n";
  tsl<<"# y-star                  "<<ystar<<"\n";
  tsl<<"# z-star                  "<<zstar<<"\n";
  tsl<<"# WorkingDistance         "<<WorkingDistance<<"\n";
  tsl<<"# \n";

  for (int i=1; i<phases.size(); i++) {
    tsl<<"# Phase "<<i<<"\n";
    tsl<<"# MaterialName    "<<phases[i].MaterialName<<"\n";
    //  make this GENERAL!!!!
    if (phases[i].MaterialName == "Zirc-alloy4" ) {
      phases[i].Formula = "Zr" ;
      tsl<<"# Formula        "<<phases[i].Formula<<"\n";
      tsl<<"# Info \n";
      tsl<<"# Symmetry      "<<phases[i].Symmetry<<"\n";
      tsl<<"# LatticeConstants  ";
      tsl<<phases[i].a<<" "<<phases[i].b<<" "<<phases[i].c<<" ";
      tsl<<phases[i].alpha<<" "<<phases[i].beta<<" "<<phases[i].gamma<<"\n";
      //    tsl<<"# NumberFamilies  "<<"0 \n";
      tsl<<"# NumberFamilies  "<<"5 \n";
      tsl<<"# hklFamilies      1  0  0 0 202.000000 0"<<"5 \n" ;
      tsl<<"# hklFamilies      1  1  0 1 999.000000 1"<<"5 \n" ;
      tsl<<"# hklFamilies      1  1  1 1 234.000000 1"<<"5 \n" ;
      tsl<<"# hklFamilies      2  0  0 1 307.000000 1"<<"5 \n" ;
      tsl<<"# hklFamilies      2  1  1 1 315.000000 1"<<"5 \n" ;
    }
    if (phases[i].MaterialName == "Ti-Hex" ) {
      phases[i].Formula = "Ti" ;
      tsl<<"# Formula        "<<phases[i].Formula<<"\n";
      tsl<<"# Info \n";
      tsl<<"# Symmetry      "<<phases[i].Symmetry<<"\n";
      tsl<<"# LatticeConstants  ";
      tsl<<phases[i].a<<" "<<phases[i].b<<" "<<phases[i].c<<" ";
      tsl<<phases[i].alpha<<" "<<phases[i].beta<<" "<<phases[i].gamma<<"\n";
      //    tsl<<"# NumberFamilies  "<<"0 \n";
      tsl<<"# NumberFamilies  "<<"8 \n";
      tsl<<"# hklFamilies      1  0  0 1 0.000000 23 \n" ;
      tsl<<"# hklFamilies      0  0  2 1 0.000000 263184312 \n" ;
      tsl<<"# hklFamilies      1  0  1 1 0.000000 2016139636 \n" ;
      tsl<<"# hklFamilies      1  0  2 1 0.000000 263184620 \n" ;
      tsl<<"# hklFamilies      1  1  0 1 0.000000 2117862543 \n" ;
      tsl<<"# hklFamilies      1  0  3 1 0.000000 2117699632 \n" ;
      tsl<<"# hklFamilies      1  1  2 1 0.000000 -1 \n" ;
      tsl<<"# hklFamilies      2  0  1 1 0.000000 2117699626 \n" ;
    }
    if (phases[i].MaterialName == "Cu" ) {
      phases[i].Formula = "Cu" ;
      tsl<<"# Formula        "<<phases[i].Formula<<"\n";
      tsl<<"# Info \n";
      tsl<<"# Symmetry       "<<phases[i].Symmetry<<"\n";
      tsl<<"# LatticeConstants  ";
      tsl<<phases[i].a<<" "<<phases[i].b<<" "<<phases[i].c<<" ";
      tsl<<phases[i].alpha<<" "<<phases[i].beta<<" "<<phases[i].gamma<<"\n";
      //    tsl<<"# NumberFamilies  "<<"0 \n";
      tsl<<"# NumberFamilies  "<<"0 \n";
      /*
        tsl<<"# hklFamilies      1  0  0 0 202.000000 0"<<"5 \n" ;
        tsl<<"# hklFamilies      1  1  0 1 999.000000 1"<<"5 \n" ;
        tsl<<"# hklFamilies      1  1  1 1 234.000000 1"<<"5 \n" ;
        tsl<<"# hklFamilies      2  0  0 1 307.000000 1"<<"5 \n" ;
        tsl<<"# hklFamilies      2  1  1 1 315.000000 1"<<"5 \n" ;
      */
    }
    // per HMM's list - ugly but should work
    tsl<<"# Categories "<<"1 1 1 1 1 \n";
    tsl<<"# \n";
  }

  tsl<<"# GRID: "<<GRID<<"\n";
  tsl<<"# XSTEP: "<<XSTEP<<"\n";
  tsl<<"# YSTEP: "<<YSTEP<<"\n";
  tsl<<"# NCOLS_ODD: "<<NCOLS_ODD<<"\n";
  tsl<<"# NCOLS_EVEN: "<<NCOLS_EVEN<<"\n";
  tsl<<"# NROWS: "<<NROWS<<"\n";
  tsl<<"# \n";
  tsl<<"# OPERATOR: "<<OPERATOR<<"\n";
  tsl<<"# \n";
  tsl<<"# SAMPLEID: "<<SAMPLEID<<"\n";
  tsl<<"# \n";
  tsl<<"# SCANID: "<<SCANID<<"\n";
  tsl<<"# \n";

  // HKL record data, in order
  int    Phase;
  double X, Y;
  int    Bands, Error;
  double Euler1, Euler2, Euler3, MAD;
  int    BC, BS;

  // TSL record data, in order
  double phi1, PHI, phi2;
  double x, y;
  double IQ, CI;
  int    A, phase;
  double B;

  int counter = 0 ;
  string line ;
  double zero = 0.0 ;
  int badcount = 0 ;

  // read, convert, and write record data
  tsl << setiosflags(ios::fixed) << setprecision(2) ;
  while (!hkl.eof()) {
    counter++ ;
    if ( counter%10000 == 0 ) {
      cout<<"reading line no. " << counter << endl ;
    }
    getline(hkl,line);
    int p = line.find(',',0);
    while (p!=string::npos) {line[p]='.'; p=line.find(',',p);}

    // read HKL record from string stream
    stringstream sstream(line);
    sstream>>Phase>>X>>Y>>Bands>>Error;
    sstream>>Euler1>>Euler2>>Euler3>>MAD>>BC>>BS;
    if (counter < 210 ) {
      cout<<"Euler1 "<<Euler1<<",  Euler2 "<<Euler2<<",  Euler3 "<<Euler3<< " , MAD: " << MAD << "\n" ;
    }
    //  now let's catch the points that were not indexed
    if ( Euler1==zero && Euler2==zero && Euler3==zero && MAD==zero ) {
      //      cout << "bad point at " << counter << "\n" ;
      badcount++ ;
      phi1 = 999. ;
      PHI  = 999. ;
      phi2 = 999. ;
    } else {
      // convert HKL record to TSL record

      //    cout<<"iqPhi2   "<<iqPhi2<<"\n" ;
      //    tmp = ( -1. * Euler1 ) + 90. ;
      tmp = Euler1 + 90. ;
      //  see above for change to subtract, 23 vi 08, ADR
      // and back to what it says in the slides, v 09 !

      if ( tmp > 360. ) tmp = tmp - 360. ;
      if ( tmp < 0. ) tmp = tmp + 360. ;
      // corrected from < 360 !!
      phi1  = M_PI/180.0 * tmp ;

      //    PHI   = M_PI/180.0 * ( -1. * Euler2 ) ;
      PHI   = M_PI/180.0 * Euler2 ;
      //  I do not understand the "-1 *" !
      //  none of the .CTF files have negative values for the second Euler angle

      //    Euler3 = Euler3 * -1. ;

      if ( Euler3 < 360. ) Euler3 = Euler3 + 360. ;

      if( iqPhi2 == 1 ) {
  tmp = Euler3 - 30. ;
  if ( tmp < 0. ) tmp = tmp + 360. ;
  phi2  = M_PI/180.0 * tmp ;
      } else {
  phi2  = M_PI/180.0 * Euler3 ;
      }
    }
    x     = X;
    y     = Y;
    //    IQ    = 1.0;    // ?
    IQ    = BC ;    // ADR:  Error -> IQ
    //    CI    = 1.0;    // ?
    CI    = MAD ;           // ADR
    A     = 0 ;     // ?
    phase = Phase;    // ?
    B     = 0.0 ;     // ?
    // write TSL record to file
    tsl       << setw(10) << phi1 ;
    tsl <<" " << setw(10) << PHI ;
    tsl <<" " << setw(10) << phi2 ;
    tsl <<" " << setw(10) << x ;
    tsl <<" " << setw(10) << y ;
    tsl <<" " << setw(10) << IQ ;
    tsl <<" " << setw(10) << CI ;
    tsl <<" " << setw(10) << A ;
    tsl <<" " << setw(10) << phase ;
    tsl <<" " << setw(10) << B << "\n";
    //    cout<<phi1/M_PI*180.0<<"  \t"<<PHI/M_PI*180.0<<"  \t"<<phi2/M_PI*180.0
    // tsl <<"  \t"<<x<<"\t"<<y<<"\n\n";
  }

  cout << "badcount= " << badcount <<", out of total= " << counter <<"\n" ;
}
#endif
