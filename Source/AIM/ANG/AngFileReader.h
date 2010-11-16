///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2010 Michael A. Jackson for BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//
///////////////////////////////////////////////////////////////////////////////
#ifndef _AngFileReader_H
#define _AngFileReader_H

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


//-- MXA Includes
#include <MXA/MXATypes.h>
#include <MXA/Common/MXASetGetMacros.h>
#include <AIM/Common/AIMArray.hpp>


//-- Include the STL classes
#include <string>
#include <vector>




/**
 * @brief This class will read the .ang files produced by the FEI OIM instrument
 * @author Mike Jackson
 * @date March 2007
 * @header src/FEI/OIM/AngFileReader.h
 * @version $Revision: 1.3 $
 */
class AngFileReader
{
public:
    MXA_SHARED_POINTERS(AngFileReader)
    MXA_STATIC_NEW_MACRO(AngFileReader)


  virtual ~AngFileReader();

  typedef AIMArray<float>::Pointer   FloatArrayType;
/**
 * @brief
 * @param filename
 * @return Error Condition. Negative is error.
 */
  int32_t readFile(std::string filename, int header);

  float32 getTEMpixPerum() { return _tem_pixperum;}
  float32 getXStar() { return _xStar;}
  float32 getYStar() { return _yStar;}
  float32 getZStar() { return _zStar;}
  float32 getWorkingDistance() { return _workingdistance;}
  std::string getPhase() { return _phase;}
  std::string getMaterialName() { return _materialname;}
  std::string getFormula() { return _formula;}
  std::string getInfo() { return _info;}
  std::string getSymmetry() { return _symmetry;}
  std::string getLatticeConstants() { return _latticeconstants;}
  std::string getNumberFamilies() { return _numberfamilies;}
  std::string getHKLFamilies() { return _hklfamilies;}
  std::string getCategories() { return _categories; }
  std::string getGrid() { return _grid; }
  float32 getXStep() { return _xstep; }
  float32 getYStep() { return _ystep; }
  int32_t getNumOddCols() { return _ncols_odd;}
  int32_t getNumEvenCols()  { return _ncols_even;}
  int32_t getNumRows() { return _nrows;}
  std::string getOIMOperator() { return _oimOperator;}
  std::string getSampleID() { return _sampleid;}
  std::string getScanID() { return _scanid;}

  AIMArray<float>::Pointer getPhi1Data() { return _phi1;}
  AIMArray<float>::Pointer getPhiData() { return _phi;}
  AIMArray<float>::Pointer getPhi2Data() { return _phi2;}

  AIMArray<float>::Pointer getImageQualityData() { return _iq;}
  AIMArray<float>::Pointer getConfidenceIndexData() { return _ci;}
  AIMArray<float>::Pointer getPhaseData() { return _phaseData;}
  AIMArray<float>::Pointer getXData() { return m_X; }
  AIMArray<float>::Pointer getYData() { return m_Y; }

protected:
  AngFileReader();
  void _readHeader( const std::string& line );
  void _readData( const std::string& line, uint32_t index);

private:

  //-- These are all the header Variables
  float32 _tem_pixperum;
  float32 _xStar;
  float32 _yStar;
  float32 _zStar;
  float32 _workingdistance ;

  std::string _phase ;
  std::string _materialname;
  std::string _formula;
  std::string _info;
  std::string _symmetry;
  std::string _latticeconstants;
  std::string _numberfamilies;
  std::string _hklfamilies;
  std::string _categories;

  std::string _grid;
  float32 _xstep;
  float32 _ystep;
  int32_t _ncols_odd;
  int32_t _ncols_even;
  int32_t _nrows;

  std::string _oimOperator;
  std::string _sampleid;
  std::string _scanid;

  AIMArray<float>::Pointer _phi1;
  AIMArray<float>::Pointer _phi;
  AIMArray<float>::Pointer _phi2;
  AIMArray<float>::Pointer _iq;
  AIMArray<float>::Pointer _ci;
  AIMArray<float>::Pointer _phaseData;
  AIMArray<float>::Pointer m_X;
  AIMArray<float>::Pointer m_Y;


  AngFileReader(const AngFileReader&);
  void operator=(const AngFileReader);

};


#endif  // end Compile Guard
