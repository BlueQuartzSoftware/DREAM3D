/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */




#include "H5CtfReader.h"


#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"
#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/EbsdMacros.h"
#include "EbsdLib/Utilities/StringUtils.h"

#include "EbsdLib/HKL/CtfConstants.h"

#define PI_OVER_2f       1.57079632679489661f
#define THREE_PI_OVER_2f 4.71238898038468985f
#define TWO_PIf          6.28318530717958647f
#define ONE_PIf          3.14159265358979323f

#define SHUFFLE_ARRAY(name, var, type)\
  { type* f = allocateArray<type>(totalDataRows);\
  for (size_t i = 0; i < totalDataRows; ++i)\
  {\
    size_t nIdx = shuffleTable[i];\
    f[nIdx] = var[i];\
  }\
  set##name##Pointer(f); }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5CtfReader::H5CtfReader() :
CtfReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5CtfReader::~H5CtfReader()
{
  deletePointers();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5CtfReader::readFile()
{
  int err = -1;
  if (m_HDF5Path.empty() == true)
  {
    std::cout << "H5CtfReader Error: HDF5 Path is empty." << std::endl;
    return -1;
  }

  hid_t fileId = H5Utilities::openFile(getFileName(), true);
  if (fileId < 0)
  {
    std::cout << "H5CtfReader Error: Could not open HDF5 file '" << getFileName() << "'" << std::endl;
    return -1;
  }

  hid_t gid = H5Gopen(fileId, m_HDF5Path.c_str(), H5P_DEFAULT);
  if (gid < 0)
  {
    std::cout << "H5CtfReader Error: Could not open path '" << m_HDF5Path << "'" << std::endl;
    err = H5Utilities::closeFile(fileId);
    return -1;
  }

  // Read all the header information
 // std::cout << "H5CtfReader:: reading Header .. " << std::endl;
  err = readHeader(gid);

  // Read and transform data
 // std::cout << "H5CtfReader:: Reading Data .. " << std::endl;
  err = readData(gid);

  err = H5Gclose(gid);
  err = H5Fclose(fileId);
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5CtfReader::readHeader(hid_t parId)
{
  int err = -1;
  hid_t gid = H5Gopen(parId, Ebsd::H5::Header.c_str(), H5P_DEFAULT);
  if (gid < 0)
  {
    std::cout << "H5CtfReader Error: Could not open 'Header' Group" << std::endl;
    return -1;
  }

  READ_EBSD_HEADER_STRING_DATA("H5CtfReader", CtfStringHeaderEntry, std::string, Prj, Ebsd::Ctf::Prj)
  READ_EBSD_HEADER_STRING_DATA("H5CtfReader", CtfStringHeaderEntry, std::string, Author, Ebsd::Ctf::Author)
  READ_EBSD_HEADER_STRING_DATA("H5CtfReader", CtfStringHeaderEntry, std::string, JobMode, Ebsd::Ctf::JobMode)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<int>, int, XCells, Ebsd::Ctf::XCells)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<int>, int, YCells, Ebsd::Ctf::YCells)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<float>, float, XStep, Ebsd::Ctf::XStep)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<float>, float, YStep, Ebsd::Ctf::YStep)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<float>, float, AcqE1, Ebsd::Ctf::AcqE1)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<float>, float, AcqE2, Ebsd::Ctf::AcqE2)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<float>, float, AcqE3, Ebsd::Ctf::AcqE3)
  READ_EBSD_HEADER_STRING_DATA("H5CtfReader", CtfStringHeaderEntry, std::string, Euler, Ebsd::Ctf::Euler)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<int>, int, Mag, Ebsd::Ctf::Mag)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<int>, int, Coverage, Ebsd::Ctf::Coverage)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<int>, int, Device, Ebsd::Ctf::Device)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<int>, int, KV, Ebsd::Ctf::KV)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<float>, float, TiltAngle, Ebsd::Ctf::TiltAngle)
  READ_EBSD_HEADER_DATA("H5CtfReader", CtfHeaderEntry<float>, float, TiltAxis, Ebsd::Ctf::TiltAxis)

  hid_t phasesGid = H5Gopen(gid, Ebsd::H5::Phases.c_str(), H5P_DEFAULT);
  if (phasesGid < 0)
  {
    std::cout << "H5CtfReader Error: Could not open Header/Phases HDF Group. Is this an older file?" << std::endl;
    H5Gclose(gid);
    return -1;
  }

  std::list<std::string> names;
  err = H5Utilities::getGroupObjects(phasesGid, H5Utilities::H5Support_GROUP, names);
  if (err < 0 || names.size() == 0)
  {
    std::cout << "H5CtfReader Error: There were no Phase groups present in the HDF5 file" << std::endl;
    H5Gclose(phasesGid);
    H5Gclose(gid);
    return -1;
  }
  m_Phases.clear();
  for (std::list<std::string>::iterator phaseGroupName = names.begin(); phaseGroupName != names.end(); ++phaseGroupName )
  {
    hid_t pid = H5Gopen(phasesGid, (*phaseGroupName).c_str(), H5P_DEFAULT);
    CtfPhase::Pointer m_CurrentPhase = CtfPhase::New();

    READ_PHASE_HEADER_ARRAY("H5CtfReader", pid, std::vector<float>, Ebsd::Ctf::LatticeDimensions, LatticeDimensions, m_CurrentPhase);
    READ_PHASE_HEADER_ARRAY("H5CtfReader", pid, std::vector<float>, Ebsd::Ctf::LatticeAngles, LatticeAngles, m_CurrentPhase);
    READ_PHASE_STRING_DATA("H5CtfReader", pid, Ebsd::Ctf::PhaseName, PhaseName, m_CurrentPhase)
    READ_PHASE_HEADER_DATA("H5CtfReader", pid, Ebsd::Ctf::PhaseSymmetry, Ebsd::Ctf::LaueGroup, Symmetry, m_CurrentPhase)
    READ_PHASE_STRING_DATA("H5CtfReader", pid, Ebsd::Ctf::Section4, Section4, m_CurrentPhase)
    READ_PHASE_STRING_DATA("H5CtfReader", pid, Ebsd::Ctf::Section5, Section5, m_CurrentPhase)
    READ_PHASE_STRING_DATA("H5CtfReader", pid, Ebsd::Ctf::Section6, Section6, m_CurrentPhase)
    READ_PHASE_STRING_DATA("H5CtfReader", pid, Ebsd::Ctf::Comment, Comment, m_CurrentPhase)

    m_Phases.push_back(m_CurrentPhase);
    err = H5Gclose(pid);
  }
  err = H5Gclose(phasesGid);

  std::string completeHeader;
  err = H5Lite::readStringDataset(gid, Ebsd::H5::OriginalHeader, completeHeader);
  setOriginalHeader(completeHeader);

  err = H5Gclose(gid);
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5CtfReader::readData(hid_t parId)
{
  int err = -1;

  // Delete any currently existing pointers
  deletePointers();
  // Initialize new pointers
  size_t nRows = getYCells();
  size_t nCols = getXCells();
  size_t totalDataRows = nRows * nCols;


  initPointers(totalDataRows);
  if (NULL == getPhasePointer() || NULL == getXPointer() || NULL == getYPointer()
      || NULL == getBandCountPointer()  || NULL == getErrorPointer()
      || NULL == getEuler1Pointer() || getEuler2Pointer() == NULL || getEuler3Pointer() == NULL
      || NULL == getMeanAngularDeviationPointer() || NULL == getBandContrastPointer() || NULL == getBandSlopePointer())
  {
    return -1;
  }


  hid_t gid = H5Gopen(parId, Ebsd::H5::Data.c_str(), H5P_DEFAULT);
  if (gid < 0)
  {
    std::cout << "H5CtfReader Error: Could not open 'Data' Group" << std::endl;
    return -1;
  }

  err = H5Lite::readPointerDataset(gid, Ebsd::Ctf::Phase, getPhasePointer());
  err = H5Lite::readPointerDataset(gid, Ebsd::Ctf::X, getXPointer());
  err = H5Lite::readPointerDataset(gid, Ebsd::Ctf::Y, getYPointer());
  err = H5Lite::readPointerDataset(gid, Ebsd::Ctf::BandCount, getBandCountPointer());
  err = H5Lite::readPointerDataset(gid, Ebsd::Ctf::Error, getErrorPointer());
  err = H5Lite::readPointerDataset(gid, Ebsd::Ctf::Euler1, getEuler1Pointer());
  err = H5Lite::readPointerDataset(gid, Ebsd::Ctf::Euler2, getEuler2Pointer());
  err = H5Lite::readPointerDataset(gid, Ebsd::Ctf::Euler3, getEuler3Pointer());
  err = H5Lite::readPointerDataset(gid, Ebsd::Ctf::MeanAngularDeviation, getMeanAngularDeviationPointer());
  err = H5Lite::readPointerDataset(gid, Ebsd::Ctf::BandContrast, getBandContrastPointer());
  err = H5Lite::readPointerDataset(gid, Ebsd::Ctf::BandSlope, getBandSlopePointer());


  int* phase = getPhasePointer();
  int* bCount = getBandCountPointer();
  int* error = getErrorPointer();
  float* p1 = getEuler1Pointer();
  float* p = getEuler2Pointer();
  float* p2 = getEuler3Pointer();

  float* mad = getMeanAngularDeviationPointer();
  int* bc = getBandContrastPointer();
  int* bs = getBandSlopePointer();

  size_t offset = 0;
  std::vector<size_t> shuffleTable(totalDataRows, 0);

  size_t i = 0;
  for(size_t row = 0; row < nRows; ++row)
  {
    for(size_t col = 0; col < nCols; ++col)
    {
    // Do we transform the data
		  if (getUserOrigin() == Ebsd::UpperRightOrigin)
		  {
			  if (getUserZDir() == Ebsd::IntoSlice)
			  {
				offset = (((nCols-1)-col)*nRows)+(row);
				if (p1[i] - PI_OVER_2f < 0.0)
				{
				  p1[i] = p1[i] + THREE_PI_OVER_2f;
				}
				else
				{
				  p1[i] = p1[i] - PI_OVER_2f;
				}
			  }
			  if (getUserZDir() == Ebsd::OutofSlice)
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
		  }
		  else if (getUserOrigin() == Ebsd::UpperLeftOrigin)
		  {
			  if (getUserZDir() == Ebsd::IntoSlice)
			  {
				offset = (row*nCols)+(col);
				if (p1[i] - PI_OVER_2f < 0.0)
				{
				  p1[i] = p1[i] + THREE_PI_OVER_2f;
				}
				else
				{
				  p1[i] = p1[i] - PI_OVER_2f;
				}
			  }
			  if (getUserZDir() == Ebsd::OutofSlice)
			  {
				offset = (col*nRows)+(row);
				if (p1[i] - PI_OVER_2f < 0.0)
				{
				  p1[i] = p1[i] + THREE_PI_OVER_2f;
				}
				else
				{
				  p1[i] = p1[i] - PI_OVER_2f;
				}
			  }
		  }
		  else if (getUserOrigin() == Ebsd::LowerLeftOrigin)
		  {
			  if (getUserZDir() == Ebsd::IntoSlice)
			  {
				offset = (col*nRows)+((nRows-1)-row);
				if (p1[i] - PI_OVER_2f < 0.0)
				{
				  p1[i] = p1[i] + THREE_PI_OVER_2f;
				}
				else
				{
				  p1[i] = p1[i] - PI_OVER_2f;
				}
			  }
			  if (getUserZDir() == Ebsd::OutofSlice)
			  {
				offset = (((nRows-1)-row)*nCols)+(col);
				if (p1[i] - PI_OVER_2f < 0.0)
				{
				  p1[i] = p1[i] + THREE_PI_OVER_2f;
				}
				else
				{
				  p1[i] = p1[i] - PI_OVER_2f;
				}
			  }
		  }
		  else if (getUserOrigin() == Ebsd::LowerRightOrigin)
		  {
			  if (getUserZDir() == Ebsd::IntoSlice)
			  {
				offset = (((nRows-1)-row)*nCols)+((nCols-1)-col);
				if (p1[i] - PI_OVER_2f < 0.0)
				{
				  p1[i] = p1[i] + THREE_PI_OVER_2f;
				}
				else
				{
				  p1[i] = p1[i] - PI_OVER_2f;
				}
			  }
			  if (getUserZDir() == Ebsd::OutofSlice)
			  {
				offset = (((nCols-1)-col)*nRows)+((nRows-1)-row);
				if (p1[i] - PI_OVER_2f < 0.0)
				{
				  p1[i] = p1[i] + THREE_PI_OVER_2f;
				}
				else
				{
				  p1[i] = p1[i] - PI_OVER_2f;
				}
			  }
		  }

		  if (getUserOrigin() == Ebsd::NoOrientation)
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

  SHUFFLE_ARRAY(Phase, phase, int)
  SHUFFLE_ARRAY(BandCount, bCount, int)
  SHUFFLE_ARRAY(Error, error, int)
  SHUFFLE_ARRAY(Euler1, p1, float)
  SHUFFLE_ARRAY(Euler2, p, float)
  SHUFFLE_ARRAY(Euler3, p2, float)
  SHUFFLE_ARRAY(MeanAngularDeviation, mad, float)
  SHUFFLE_ARRAY(BandContrast, bc, int)
  SHUFFLE_ARRAY(BandSlope, bs, int)


  err = H5Gclose(gid);
  if(getUserOrigin() == Ebsd::UpperRightOrigin && getUserZDir() == Ebsd::IntoSlice) setAxesFlipped(true);
  if(getUserOrigin() == Ebsd::UpperRightOrigin && getUserZDir() == Ebsd::OutofSlice) setAxesFlipped(false);
  if(getUserOrigin() == Ebsd::UpperLeftOrigin && getUserZDir() == Ebsd::IntoSlice) setAxesFlipped(false);
  if(getUserOrigin() == Ebsd::UpperLeftOrigin && getUserZDir() == Ebsd::OutofSlice) setAxesFlipped(true);
  if(getUserOrigin() == Ebsd::LowerLeftOrigin && getUserZDir() == Ebsd::IntoSlice) setAxesFlipped(true);
  if(getUserOrigin() == Ebsd::LowerLeftOrigin && getUserZDir() == Ebsd::OutofSlice) setAxesFlipped(false);
  if(getUserOrigin() == Ebsd::LowerRightOrigin && getUserZDir() == Ebsd::IntoSlice) setAxesFlipped(false);
  if(getUserOrigin() == Ebsd::LowerRightOrigin && getUserZDir() == Ebsd::OutofSlice) setAxesFlipped(true);
  if(getAxesFlipped() == true)
  {
	setYCells(nRows);
	setXCells(nCols);
  }
  return err;
}

