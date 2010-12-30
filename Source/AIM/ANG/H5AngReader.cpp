/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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

#include "H5AngReader.h"
#include "AIM/Common/Constants.h"
#include "AIM/ANG/AngConstants.h"
#include "MXA/HDF5/H5Lite.h"
#include "MXA/HDF5/H5Utilities.h"
#include "MXA/Utilities/StringUtils.h"

#define PI_OVER_2f       1.57079632679489661f
#define THREE_PI_OVER_2f 4.71238898038468985f
#define TWO_PIf          6.28318530717958647f
#define ONE_PIf          3.14159265358979323f


#define SHUFFLE_ARRAY(name, var)\
  { float* f = allocateArray<float>(numElements);\
  for (size_t i = 0; i < totalDataRows; ++i)\
  {\
    size_t nIdx = shuffleTable[i];\
    f[nIdx] = var[i];\
  }\
  set##name##Pointer(f); }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5AngReader::H5AngReader() :
AngReader()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5AngReader::~H5AngReader()
{
 deletePointers();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5AngReader::readFile()
{
  int err = -1;
  if (m_HDF5Path.empty() == true)
  {
    std::cout << "H5AngReader Error: HDF5 Path is empty." << std::endl;
    return -1;
  }

  hid_t fileId = H5Utilities::openFile(getFileName(), true);
  if (fileId < 0)
  {
    std::cout << "H5AngReader Error: Could not open HDF5 file '" << getFileName() << "'" << std::endl;
    return -1;
  }

  hid_t gid = H5Gopen(fileId, m_HDF5Path.c_str());
  if (gid < 0)
  {
    std::cout << "H5AngReader Error: Could not open path '" << m_HDF5Path << "'" << std::endl;
    return -1;
  }

  // Read all the header information
  err = readHeader(gid);

  // Read and transform data
  err = readData(gid);

  err = H5Gclose(gid);
  err = H5Fclose(fileId);
  return err;
}

#define READ_ANG_HEADER_DATA(class, type, getName, key)\
{\
  type t;\
  err = H5Lite::readScalarDataset(gid, key, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss << "H5AngImporter Error: Could not write Ang Header value '" << t\
    <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
    std::cout << ss.str() << std::endl;\
    err = H5Gclose(gid);\
    return -1; }\
  else {\
    HeaderEntry::Pointer p = m_Headermap[key];\
    class* c = dynamic_cast<class*>(p.get());\
    c->setValue(t);\
  }\
}

#define READ_ANG_HEADER_STRING_DATA(class, type, getName, key)\
{\
  std::string t;\
  err = H5Lite::readStringDataset(gid, key, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss << "H5AngImporter Error: Could not write Ang Header value '" << t\
    <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
    std::cout << ss.str() << std::endl;\
    err = H5Gclose(gid);\
    return -1; }\
    else {\
      HeaderEntry::Pointer p = m_Headermap[key];\
      class* c = dynamic_cast<class*>(p.get());\
      c->setValue(t);\
    }\
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5AngReader::readHeader(hid_t parId)
{
  int err = -1;
  hid_t gid = H5Gopen(parId, AIM::ANG::Header.c_str());
  if (gid < 0)
  {
    std::cout << "H5AngReader Error: Could not open 'Header' Group" << std::endl;
    return -1;
  }


  READ_ANG_HEADER_DATA(AngHeaderEntry<float>, float, TEMPIXPerUM, TSL::OIM::TEMPIXPerUM)
  READ_ANG_HEADER_DATA(AngHeaderEntry<float>, float, XStar, TSL::OIM::XStar)
  READ_ANG_HEADER_DATA(AngHeaderEntry<float>, float, YStar, TSL::OIM::YStar)
  READ_ANG_HEADER_DATA(AngHeaderEntry<float>, float, ZStar, TSL::OIM::ZStar)
  READ_ANG_HEADER_DATA(AngHeaderEntry<float>, float, WorkingDistance, TSL::OIM::WorkingDistance)
  READ_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, Phase, TSL::OIM::Phase)
  READ_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, MaterialName, TSL::OIM::MaterialName)
  READ_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, Formula, TSL::OIM::Formula)
  READ_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, Info, TSL::OIM::Info)
  READ_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, Symmetry, TSL::OIM::Symmetry)
  READ_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, LatticeConstants, TSL::OIM::LatticeConstants)
  READ_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, NumberFamilies, TSL::OIM::NumberFamilies)
  READ_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, HKLFamilies, TSL::OIM::HKLFamilies)
  READ_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, Categories, TSL::OIM::Categories)
  READ_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, Grid, TSL::OIM::Grid)
  READ_ANG_HEADER_DATA(AngHeaderEntry<float>, float, XStep, TSL::OIM::XStep)
  READ_ANG_HEADER_DATA(AngHeaderEntry<float>, float, YStep, TSL::OIM::YStep)
  READ_ANG_HEADER_DATA(AngHeaderEntry<int>, int, NumOddCols, TSL::OIM::NColsOdd)
  READ_ANG_HEADER_DATA(AngHeaderEntry<int>, int, NumEvenCols, TSL::OIM::NColsEven)
  READ_ANG_HEADER_DATA(AngHeaderEntry<int>, int, NumRows, TSL::OIM::NRows)
  READ_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, OIMOperator, TSL::OIM::Operator)
  READ_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, SampleID, TSL::OIM::SampleId)
  READ_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, ScanID, TSL::OIM::ScanId)

  std::string completeHeader;
  err = H5Lite::readStringDataset(gid, AIM::ANG::AngHeader, completeHeader);
  setCompleteHeader(completeHeader);

  err = H5Gclose(gid);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5AngReader::readData(hid_t parId)
{
  int err = -1;

  // Delete any currently existing pointers
  deletePointers();
  // Initialize new pointers
  size_t numElements = 0;

  std::string grid = getGrid();

  int nOddCols = getNumOddCols();
  int nEvenCols = getNumEvenCols();
  int nRows = getNumRows();

  if (nRows < 1)
  {
    return -200;
  }
  else if (grid.find(TSL::OIM::SquareGrid) == 0)
  {
    // if (nCols > 0) { numElements = nRows * nCols; }
    if (nOddCols > 0)
    {
      numElements = nRows * nOddCols;/* nCols = nOddCols;*/
    }
    else if (nEvenCols > 0)
    {
      numElements = nRows * nEvenCols; /* nCols = nEvenCols; */
    }
    else
    {
      numElements = 0;
    }
  }
  else if (grid.find(TSL::OIM::HexGrid) == 0)
  {
    std::cout << "Ang Files with Hex Grids Are NOT currently supported." << std::endl;
    return -400;
  }
  else // Grid was not set
  {
    return -300;
  }

  initPointers(numElements);
  if (NULL == getPhi1Pointer() || NULL == getPhiPointer() || NULL == getPhi2Pointer()
      || NULL == getImageQualityPointer()  || NULL == getConfidenceIndexPointer()
      || NULL == getPhasePointer() || getXPosPointer() == NULL || getYPosPointer() == NULL
      || NULL == getD1Pointer() || NULL == getD2Pointer())
  {
    return -1;
  }

  float xstep = getXStep();
  float ystep = getYStep();
  float xMaxValue = static_cast<float > ((nEvenCols - 1) * xstep);
  float yMaxValue = static_cast<float > ((nRows - 1) * ystep);

  //double progress = 0.0;
  size_t totalDataRows = nRows * nEvenCols;
 // int counter = 0;

  hid_t gid = H5Gopen(parId, AIM::ANG::Data.c_str());
  if (gid < 0)
  {
    std::cout << "H5AngReader Error: Could not open 'Data' Group" << std::endl;
    return -1;
  }

  err = H5Lite::readPointerDataset(gid, TSL::OIM::Phi1, getPhi1Pointer());
  err = H5Lite::readPointerDataset(gid, TSL::OIM::Phi, getPhiPointer());
  err = H5Lite::readPointerDataset(gid, TSL::OIM::Phi2, getPhi2Pointer());
  err = H5Lite::readPointerDataset(gid, TSL::OIM::ImageQuality, getImageQualityPointer());
  err = H5Lite::readPointerDataset(gid, TSL::OIM::ConfidenceIndex, getConfidenceIndexPointer());
  err = H5Lite::readPointerDataset(gid, TSL::OIM::PhaseData, getPhasePointer());
  err = H5Lite::readPointerDataset(gid, TSL::OIM::XPosition, getXPosPointer());
  err = H5Lite::readPointerDataset(gid, TSL::OIM::YPosition, getYPosPointer());

  err = H5Lite::readPointerDataset(gid, TSL::OIM::D2, getD2Pointer());
  if (err < 0)
  {
    setNumFields(9);
  }
  err = H5Lite::readPointerDataset(gid, TSL::OIM::D1, getD1Pointer());
  if (err < 0)
  {
    setNumFields(8);
  }

  float* p1 = getPhi1Pointer();
  float* p = getPhiPointer();
  float* p2 = getPhi2Pointer();
  float* x = getXPosPointer();
  float* y = getYPosPointer();
  float* iqual = getImageQualityPointer();
  float* conf = getConfidenceIndexPointer();
  float* ph = getPhasePointer();
  float* d1 = getD1Pointer();
  float* d2 = getD2Pointer();

  int nCols = getNumEvenCols();
  size_t offset = 0;
  std::vector<size_t> shuffleTable(totalDataRows, 0);
  int userOrigin = getUserOrigin();

  for(size_t i = 0; i < totalDataRows; ++i)
  {
    // Do we transform the data
      if (userOrigin == UpperRightOrigin)
      {
        x[i] = xMaxValue - x[i];
        if (p1[i] - PI_OVER_2f < 0.0)
        {
          p1[i] = p1[i] + THREE_PI_OVER_2f;
        }
        else
        {
          p1[i] = p1[i] - PI_OVER_2f;
        }
      }
      else if (userOrigin == UpperLeftOrigin)
      {
        if (p1[i] + PI_OVER_2f > TWO_PIf)
        {
          p1[i] = p1[i] - THREE_PI_OVER_2f;
        }
        else
        {
          p1[i] = p1[i] + PI_OVER_2f;
        }
        if (p[i] + ONE_PIf > TWO_PIf)
        {
          p[i] = p[i] - ONE_PIf;
        }
        else
        {
          p[i] = p[i] + ONE_PIf;
        }
      }
      else if (userOrigin == LowerLeftOrigin)
      {
        y[i] = yMaxValue - y[i];
        if (p1[i] + PI_OVER_2f > TWO_PIf)
        {
          p1[i] = p1[i] - THREE_PI_OVER_2f;
        }
        else
        {
          p1[i] = p1[i] + PI_OVER_2f;
        }
      }
      else if (userOrigin == LowerRightOrigin)
      {
        x[i] = xMaxValue - x[i];
        y[i] = yMaxValue - y[i];
      }

      if (userOrigin == NoOrientation) 
      {
        // If the user/programmer sets "NoOrientation" then we simply read the data
        // from the file and copy the values into the arrays without any regard for
        // the true X and Y positions in the grid. We are simply trying to keep the
        // data as close to the original as possible.
        offset = i;
      }
      else
      {
        // The next set of calculations figures out where to place the data
        // in the arrays, ie, which offset based on the array really being a 
        // 2D array that is laid out with the X Axis moving the fastest and
        // the Y Axis moving the slowest. On Visual Studio there seems to be some
        // sort of round off error when the floats are converted to size_t types.
        // In order for Visual Studio compilers to get the conversion correct
        // it seems that we need to break up the calculations, at least on 32 bit
        // compiles. 64 Bit compiles did not show this issue. If the user/programmer
        // sets any type of Orientation preference then we have to calculate
        // the offsets based on the x, y, xStep and yStep values. Changes to the
        // x and y values from the "if" statements above will effect what the final
        // offset is calculated to be.
        float xTemp = x[i] / xstep;
        float yTemp = y[i] / ystep;
        size_t xTempSizeT = static_cast<size_t>(xTemp);
        size_t yTempSizeT = static_cast<size_t>(yTemp);
        offset = yTempSizeT * nCols + xTempSizeT;
      }
      shuffleTable[i] = offset;
  }

  SHUFFLE_ARRAY(Phi1, p1)
  SHUFFLE_ARRAY(Phi, p)
  SHUFFLE_ARRAY(Phi2, p2)
  SHUFFLE_ARRAY(XPos, x)
  SHUFFLE_ARRAY(YPos, y)
  SHUFFLE_ARRAY(ImageQuality, iqual)
  SHUFFLE_ARRAY(ConfidenceIndex, conf)
  SHUFFLE_ARRAY(Phase, ph)
  if (NULL != d1)
  {
    SHUFFLE_ARRAY(D1, d1)
  }
  if (NULL != d2)
  {
    SHUFFLE_ARRAY(D2, d2)
  }

  err = H5Gclose(gid);
  return err;
}

