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
///////////////////////////////////////////////////////////////////////////////
// This code was partly written under US Air Force Contract FA8650-07-D-5800
///////////////////////////////////////////////////////////////////////////////

#include "H5AngReader.h"
//#include "AIM/Common/Constants.h"
#include "AngConstants.h"
#include "ANG/HDF5/H5Lite.h"
#include "ANG/HDF5/H5Utilities.h"
#include "ANG/Utilities/StringUtils.h"

#define PI_OVER_2f       1.57079632679489661f
#define THREE_PI_OVER_2f 4.71238898038468985f
#define TWO_PIf          6.28318530717958647f
#define ONE_PIf          3.14159265358979323f


#define SHUFFLE_ARRAY(name, var, type)\
  { type* f = allocateArray<type>(numElements);\
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
    ss << "H5AngReader Error: Could not read Ang Header value '" << t\
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
    ss << "H5AngReader Error: Could not read Ang Header value '" << t\
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


#define READ_PHASE_STRING_DATA(pid, fqKey, key, phase)\
{\
  std::string t;\
  err = H5Lite::readStringDataset(pid, fqKey, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss << "H5AngReader Error: Could not read Ang Header value '" << t\
    <<  "' to the HDF5 file with data set name '" << fqKey << "'" << std::endl;\
    std::cout << ss.str() << std::endl;\
    err = H5Gclose(pid); H5Gclose(phasesGid);H5Gclose(gid);\
    return -1; }\
    else {\
      phase->set##key(t);\
    }\
}

#define READ_PHASE_HEADER_DATA(pid, type, fqKey, key, phase)\
{\
  type t;\
  err = H5Lite::readScalarDataset(pid, fqKey, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss << "H5AngReader Error: Could not read Ang Header value '" << t\
    <<  "' to the HDF5 file with data set name '" << fqKey << "'" << std::endl;\
    std::cout << ss.str() << std::endl;\
    err = H5Gclose(pid);H5Gclose(phasesGid);H5Gclose(gid);\
    return -1; }\
  else {\
    phase->set##key(t);\
  }\
}

#define READ_PHASE_HEADER_DATA_CAST(pid, cast, type, fqKey, key, phase)\
{\
  type t;\
  err = H5Lite::readScalarDataset(pid, fqKey, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss << "H5AngReader Error: Could not read Ang Header value '" << t\
    <<  "' to the HDF5 file with data set name '" << fqKey << "'" << std::endl;\
    std::cout << ss.str() << std::endl;\
    err = H5Gclose(pid);H5Gclose(phasesGid);H5Gclose(gid);\
    return -1; }\
  else {\
    phase->set##key(static_cast<cast>(t));\
  }\
}

#define READ_PHASE_HEADER_ARRAY(pid, type, fqKey, key, phase)\
{\
  type t;\
  err = H5Lite::readVectorDataset(pid, fqKey, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss << "H5AngReader Error: Could not read Ang Header value "\
    <<  " to the HDF5 file with data set name '" << fqKey << "'" << std::endl;\
    std::cout << ss.str() << std::endl;\
    err = H5Gclose(pid);H5Gclose(phasesGid);H5Gclose(gid);\
    return -1; }\
  else {\
    phase->set##key(t);\
  }\
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5AngReader::readHeader(hid_t parId)
{
  int err = -1;
  hid_t gid = H5Gopen(parId, Ang::Header.c_str());
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
  READ_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, Grid, TSL::OIM::Grid)
  READ_ANG_HEADER_DATA(AngHeaderEntry<float>, float, XStep, TSL::OIM::XStep)
  READ_ANG_HEADER_DATA(AngHeaderEntry<float>, float, YStep, TSL::OIM::YStep)
  READ_ANG_HEADER_DATA(AngHeaderEntry<int>, int, NumOddCols, TSL::OIM::NColsOdd)
  READ_ANG_HEADER_DATA(AngHeaderEntry<int>, int, NumEvenCols, TSL::OIM::NColsEven)
  READ_ANG_HEADER_DATA(AngHeaderEntry<int>, int, NumRows, TSL::OIM::NRows)
  READ_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, OIMOperator, TSL::OIM::Operator)
  READ_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, SampleID, TSL::OIM::SampleId)
  READ_ANG_HEADER_STRING_DATA(AngStringHeaderEntry, std::string, ScanID, TSL::OIM::ScanId)

  hid_t phasesGid = H5Gopen(gid, Ang::Phases.c_str());
  if (phasesGid < 0)
  {
    std::cout << "H5AngReader Error: Could not open Header/Phases HDF Group. Is this an older file?" << std::endl;
    H5Gclose(gid);
    return -1;
  }

  std::list<std::string> names;
  err = H5Utilities::getGroupObjects(phasesGid, H5Utilities::ANG_GROUP, names);
  if (err < 0 || names.size() == 0)
  {
    std::cout << "H5AngReader Error: There were no Phase groups present in the HDF5 file" << std::endl;
    H5Gclose(phasesGid);
    H5Gclose(gid);
    return -1;
  }
  m_Phases.clear();
  for (std::list<std::string>::iterator phaseGroupName = names.begin(); phaseGroupName != names.end(); ++phaseGroupName )
  {
    hid_t pid = H5Gopen(phasesGid, (*phaseGroupName).c_str());
    m_CurrentPhase = AngPhase::New();
    READ_PHASE_HEADER_DATA(pid, int, TSL::OIM::Phase, Phase, m_CurrentPhase)
    READ_PHASE_STRING_DATA(pid, TSL::OIM::MaterialName, MaterialName, m_CurrentPhase)
    READ_PHASE_STRING_DATA(pid, TSL::OIM::Formula, Formula, m_CurrentPhase)
    READ_PHASE_STRING_DATA(pid, TSL::OIM::Info, Info, m_CurrentPhase)
    READ_PHASE_HEADER_DATA_CAST(pid, TSL::OIM::PhaseSymmetry, int, TSL::OIM::Symmetry, Symmetry, m_CurrentPhase)
    READ_PHASE_HEADER_ARRAY(pid, std::vector<float>, TSL::OIM::LatticeConstants, LatticeConstants, m_CurrentPhase)
    READ_PHASE_HEADER_DATA(pid, int, TSL::OIM::NumberFamilies, NumberFamilies, m_CurrentPhase)

    hid_t hklGid = H5Gopen(pid, TSL::OIM::HKLFamilies.c_str());
    err = readHKLFamilies(hklGid, m_CurrentPhase);
    err = H5Gclose(hklGid);

    READ_PHASE_HEADER_ARRAY(pid, std::vector<int>, TSL::OIM::Categories, Categories, m_CurrentPhase)
    m_Phases.push_back(m_CurrentPhase);
    err = H5Gclose(pid);
  }
  err = H5Gclose(phasesGid);

  std::string completeHeader;
  err = H5Lite::readStringDataset(gid, Ang::AngHeader, completeHeader);
  setCompleteHeader(completeHeader);

  err = H5Gclose(gid);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5AngReader::readHKLFamilies(hid_t hklGid, AngPhase::Pointer phase)
{
//  herr_t err = 0;
  hid_t dataset, memtype;
  herr_t status;
  HKLFamily_t data;
  std::vector<HKLFamily::Pointer> families;
  for (int i = 0; i < phase->getNumberFamilies(); ++i)
  {
    std::string dsetName = StringUtils::numToString(i);

    dataset = H5Dopen(hklGid, dsetName.c_str());

    memtype = H5Tcreate (H5T_COMPOUND, sizeof (HKLFamily_t));
    status = H5Tinsert(memtype, "H", HOFFSET (HKLFamily_t, h), H5T_NATIVE_INT);
    status = H5Tinsert(memtype, "K", HOFFSET (HKLFamily_t, k), H5T_NATIVE_INT);
    status = H5Tinsert(memtype, "L", HOFFSET (HKLFamily_t, l), H5T_NATIVE_INT);
    status = H5Tinsert(memtype, "Solution 1", HOFFSET (HKLFamily_t, s1), H5T_NATIVE_INT);
    status = H5Tinsert(memtype, "Diffraction Intensity", HOFFSET (HKLFamily_t, diffractionIntensity), H5T_NATIVE_FLOAT);
    status = H5Tinsert(memtype, "Solution 2", HOFFSET (HKLFamily_t, s2), H5T_NATIVE_INT);

    status = H5Dread(dataset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, (void*)(&data));
    if (status < 0)
    {
      std::cout << "H5AngReader Error: Could not read the HKLFamily data for family number " << i << std::endl;
      break;
    }
    HKLFamily::Pointer f = HKLFamily::New();
    f->copyFromStruct(&data);
    families.push_back(f);
  }
  phase->setHKLFamilies(families);
  return status;
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

  size_t nOddCols = getNumOddCols();
  size_t nEvenCols = getNumEvenCols();
  size_t nRows = getNumRows();

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
      || NULL == getSEMSignalPointer() || NULL == getFitPointer())
  {
    return -1;
  }

//  float xstep = getXStep();
//  float ystep = getYStep();
//  float xMaxValue = static_cast<float > ((nEvenCols - 1) * xstep);
//  float yMaxValue = static_cast<float > ((nRows - 1) * ystep);

  //double progress = 0.0;
  size_t totalDataRows = nRows * nEvenCols;
 // int counter = 0;

  hid_t gid = H5Gopen(parId, Ang::Data.c_str());
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

  err = H5Lite::readPointerDataset(gid, TSL::OIM::Fit, getFitPointer());
  if (err < 0)
  {
    setNumFields(9);
  }
  err = H5Lite::readPointerDataset(gid, TSL::OIM::SEMSignal, getSEMSignalPointer());
  if (err < 0)
  {
    setNumFields(8);
  }

  float* p1 = getPhi1Pointer();
  float* p = getPhiPointer();
  float* p2 = getPhi2Pointer();
//  float* x = getXPosPointer();
//  float* y = getYPosPointer();
  float* iqual = getImageQualityPointer();
  float* conf = getConfidenceIndexPointer();
  unsigned int* ph = getPhasePointer();
  float* semSignal = getSEMSignalPointer();
  float* fit = getFitPointer();

  size_t nCols = getNumEvenCols();
  size_t offset = 0;
  std::vector<size_t> shuffleTable(totalDataRows, 0);

  size_t i = 0;
  for(size_t row = 0; row < nRows; ++row)
  {
	  for(size_t col = 0; col < nCols; ++col)
	  {
    // Do we transform the data
		  if (getUserOrigin() == Ang::UpperRightOrigin)
		  {
			offset = (row*nCols)+((nCols-1)-col);
			if (p1[i] - PI_OVER_2f < 0.0)
			{
			  p1[i] = p1[i] + THREE_PI_OVER_2f;
			}
			else
			{
			  p1[i] = p1[i] - PI_OVER_2f;
			}
		  }
		  else if (getUserOrigin() == Ang::UpperLeftOrigin)
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
		  else if (getUserOrigin() == Ang::LowerLeftOrigin)
		  {
			offset = (((nRows-1)-row)*nCols)+col;
			if (p1[i] + PI_OVER_2f > TWO_PIf)
			{
			  p1[i] = p1[i] - THREE_PI_OVER_2f;
			}
			else
			{
			  p1[i] = p1[i] + PI_OVER_2f;
			}
		  }
		  else if (getUserOrigin() == Ang::LowerRightOrigin)
		  {
			offset = (((nRows-1)-row)*nCols)+((nCols-1)-col);
		  }

		  if (getUserOrigin() == Ang::NoOrientation)
		  {
			// If the user/programmer sets "NoOrientation" then we simply read the data
			// from the file and copy the values into the arrays without any regard for
			// the true X and Y positions in the grid. We are simply trying to keep the
			// data as close to the original as possible.
			offset = i;
		  }
		  shuffleTable[(row*nCols)+col] = offset;
		  ++i;
	  }
  }

  SHUFFLE_ARRAY(Phi1, p1, float)
  SHUFFLE_ARRAY(Phi, p, float)
  SHUFFLE_ARRAY(Phi2, p2, float)
  SHUFFLE_ARRAY(ImageQuality, iqual, float)
  SHUFFLE_ARRAY(ConfidenceIndex, conf, float)
  SHUFFLE_ARRAY(Phase, ph, unsigned int)
  if (NULL != semSignal)
  {
    SHUFFLE_ARRAY(SEMSignal, semSignal, float)
  }
  if (NULL != fit)
  {
    SHUFFLE_ARRAY(Fit, fit, float)
  }

  err = H5Gclose(gid);
  return err;
}

