///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, mjackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//
///////////////////////////////////////////////////////////////////////////////

//-- Our Headers
#include <AIM/ANG/AngFileReader.h>
#include <AIM/Common/FeiConstants.h>


//-- MXA Includes
#include <MXA/Common/LogTime.h>


//-- STL Headers
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>

// -----------------------------------------------------------------------------
//  These are the constants for the ANG file
// -----------------------------------------------------------------------------

#define ANG_TEM_PIXPERUM "tem_pixperum"
#define ANG_X_STAR "x-star"
#define ANG_Y_STAR "y-star"
#define ANG_Z_STAR "z-star"
#define ANG_WORKING_DISTANCE "workingdistance"

#define ANG_PHASE "phase"
#define ANG_MATERIAL_NAME "materialname"
#define ANG_FORMULA "formula"
#define ANG_INFO "info"
#define ANG_SYMMETRY "symmetry"
#define ANG_LATTICE_CONSTANTS "latticeconstants"
#define ANG_NUMBER_FAMILIES "numberfamilies"
#define ANG_HKL_FAMILIES "hklfamilies"
#define ANG_CATEGORIES "categories"

#define ANG_GRID          "grid"
#define ANG_X_STEP        "xstep"
#define ANG_Y_STEP        "ystep"
#define ANG_NCOLS_ODD     "ncols_odd"
#define ANG_NCOLS_EVEN    "ncols_even"
#define ANG_NROWS         "nrows"

#define ANG_OPERATOR      "operator"

#define ANG_SAMPLE_ID     "sampleid"

#define ANG_SCAN_ID       "scanid"
#define ANG_SQUARE_GRID   "sqrgrid"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngFileReader::AngFileReader()  :
  _xStar(0.0f ),
  _yStar(0.0f ),
  _zStar(0.0f ),
  _workingdistance (0.0f ),

  _phase( ),
  _materialname( ),
  _formula( ),
  _info( ),
  _symmetry( ),
  _latticeconstants( ),
  _numberfamilies( ),
  _hklfamilies( ),
  _categories( ),

  _grid( ),
  _xstep(0.0f ),
  _ystep(0.0f ),
  _ncols_odd(-1 ),
  _ncols_even(-1),
  _nrows(-1),

  _oimOperator( ),
  _sampleid( ),
  _scanid()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngFileReader::~AngFileReader()
{
  //std::cout << "~AngFileReader" << std::endl;
}

// -----------------------------------------------------------------------------
//  Import the ANG data from the File
// -----------------------------------------------------------------------------
int32 AngFileReader::readFile(std::string angFilename)  {
  int32 err = 0;

  const unsigned int size ( 1024 );
  char buf [ size ];
  bool headerComplete (false);
  std::ifstream in ( angFilename.c_str() );

  if  ( !in.is_open() ) {
    std::cout << "Ang file could not be opened: " << angFilename << std::endl;
    return -100;
  }
 // std::cout << "Importing ANG File: " << angFilename << std::endl;
  while ( !in.eof() && !headerComplete )
  {
    in.getline( buf, size );

    if( buf[0] != '#' )
    {
      headerComplete = true;
    } else {
      this->_readHeader( buf );
    }
  }

  //--Allocate the vectors to hold all the data
 uint32 dataSize = _nrows * _ncols_even;
 _phi1 = AIMArray<float>::New();
 _phi = AIMArray<float>::New();
 _phi2 = AIMArray<float>::New();
 _iq = AIMArray<float>::New();
 _ci = AIMArray<float>::New();
 _phaseData = AIMArray<float>::New();
 m_X = AIMArray<float>::New();
 m_Y = AIMArray<float>::New();

 _phi1->allocateArray(dataSize);
 _phi->allocateArray(dataSize);
 _phi2->allocateArray(dataSize);
 _iq->allocateArray(dataSize);
 _ci->allocateArray(dataSize);
 _phaseData->allocateArray(dataSize);
 m_X->allocateArray(dataSize);
 m_Y->allocateArray(dataSize);

  this->_readData(buf, 0); // Read what is in the buffer...
  uint32 i;
  for (i = 1; i < dataSize; ++i) {
    if (in.eof()) { break;}
    in.getline(buf, size);
    this->_readData(buf, i);
  }

  return err;
}

// -----------------------------------------------------------------------------
//  Read the Header part of the ANG file
// -----------------------------------------------------------------------------
void AngFileReader::_readHeader( const std::string& line )
{
  std::istringstream in ( std::string ( line.begin() + 1, line.end() ) );

  std::string word;

  in >> word;

  std::string::iterator end = std::remove ( word.begin(), word.end(), ':' );
  word.resize( end - word.begin() );
  std::transform ( word.begin(), word.end(), word.begin(), ::tolower );

  if (ANG_TEM_PIXPERUM  == word ) {
        in >> _tem_pixperum ;
  } else if (ANG_X_STAR  == word ) {
    in >> _xStar;
  } else if (ANG_Y_STAR  == word ) {
    in >> _yStar;
  } else if (ANG_Z_STAR  == word ) {
    in >> _zStar;
  } else if (ANG_WORKING_DISTANCE  == word ) {
    in >> _workingdistance;
  } else if ( ANG_PHASE  == word ) {
    in >> _phase;
  } else if ( ANG_MATERIAL_NAME == word ) {
    in >> _materialname;
  } else if ( ANG_FORMULA == word ) {
    in >> _formula;
  } else if ( ANG_INFO  == word ) {
    in >> _info;
  } else if ( ANG_SYMMETRY  == word ) {
    in >> _symmetry;
  } else if ( ANG_LATTICE_CONSTANTS == word ) {
    in >> _latticeconstants;
  } else if ( ANG_NUMBER_FAMILIES == word ) {
    in >> _numberfamilies;
  } else if ( ANG_HKL_FAMILIES == word ) {
    _hklfamilies += in.str().substr(word.length() + 1 );
    std::replace(_hklfamilies.begin(), _hklfamilies.end(), '\t', ' ');
    std::replace(_hklfamilies.begin(), _hklfamilies.end(), '\r', '\n');
  } else if ( ANG_CATEGORIES == word ) {
    in >> _categories;
  } else if ( ANG_GRID == word ) {
    in >> _grid;
  } else if ( ANG_X_STEP == word ) {
    in >> _xstep;
  } else if ( ANG_Y_STEP == word ) {
    in >> _ystep;
  } else if ( ANG_NCOLS_ODD == word ) {
    in >> _ncols_odd;
  } else if ( ANG_NCOLS_EVEN == word ) {
    in >> _ncols_even;
  } else if ( ANG_NROWS == word ) {
    in >> _nrows;
  } else if ( ANG_OPERATOR == word ) {
    in >> _oimOperator;
  } else if ( ANG_SAMPLE_ID == word ) {
    in >> _sampleid;
  } else if ( ANG_SCAN_ID == word ) {
    in >>  _scanid;
  }

}

// -----------------------------------------------------------------------------
//  Read the data part of the ANG file
// -----------------------------------------------------------------------------
void AngFileReader::_readData( const std::string &line, uint32 index )
{
#if 1
  std::istringstream in ( line );
  float f;
  float d1, d2;
  in >> f; _phi1->setValue(f, index);
  in >> f; _phi->setValue(f, index);
  in >> f; _phi2->setValue(f, index);
  in >> f; m_X->setValue(f, index);
  in >> f; m_Y->setValue(f, index);
  // _phi1[index] = x;
  in >> f; _iq->setValue(f, index);
  in >> f; _ci->setValue(f, index);
  in >> f; _phaseData->setValue(f, index);
  in >> d1 >> d2;
#else
  std::istringstream in(line);
  float p1, p, p2, x, y, iqual, conf, ph, d1, d2;
  in >> p1;
  in >> p;
  in >> p2;
  in >> x;
  in >> y;
  in >> iqual;
  in >> conf;
  in >> ph;
  in >> d1 >> d2;

  float xMaxIndex = (_ncols_odd - 1);
  // Now figure out the correct offset into the array to set all the values

  size_t offset = static_cast<size_t>((y/_ystep)*_ncols_odd + (-1*(x/_xstep)) + xMaxIndex);

  p1 = p1 - 90.0f;
  if (p1 < 0) { p1 = p1 + 270.0f; }
  _phi1->setValue(p1 , offset);
  _phi->setValue(p , offset);
  _phi2->setValue(p2 , offset);
  _iq->setValue(iqual , offset);
  _ci->setValue( conf , offset);
  _phaseData->setValue( ph, offset);
  m_X->setValue(x , offset);
  m_Y->setValue(y , offset);

#endif
}




