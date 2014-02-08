/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "Hex2SqrConverter.h"

#include <iostream>
#include <fstream>

#include "H5Support/H5Utilities.h"
#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/MXADir.h"
#include "MXA/Utilities/StringUtils.h"

#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/TSL/AngReader.h"
#include "EbsdLib/HKL/CtfConstants.h"
#include "EbsdLib/HKL/CtfReader.h"
#include "EbsdLib/EbsdMacros.h"
#include "EbsdLib/EbsdMath.h"

#include "DREAM3DLib/Common/Observable.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Hex2SqrConverter::Hex2SqrConverter() :
  m_ZStartIndex(0),
  m_ZEndIndex(0),
  m_XResolution(1.0),
  m_YResolution(1.0),
  m_NumCols(0),
  m_NumRows(0),
  m_HeaderIsComplete(false)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Hex2SqrConverter::~Hex2SqrConverter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Hex2SqrConverter::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Hex2SqrConverter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setZStartIndex( reader->readValue("ZStartIndex", getZStartIndex() ) );
  setZEndIndex( reader->readValue("ZEndIndex", getZEndIndex() ) );
  setXResolution( reader->readValue("XResolution", getXResolution() ) );
  setYResolution( reader->readValue("YResolution", getYResolution() ) );
  setEbsdFileList( reader->readValue("EbsdFileList", getEbsdFileList() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int Hex2SqrConverter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("ZStartIndex", getZStartIndex() );
  writer->writeValue("ZEndIndex", getZEndIndex() );
  writer->writeValue("XResolution", getXResolution() );
  writer->writeValue("ZYResolution", getYResolution() );
  writer->writeValue("EbsdFileList", getEbsdFileList() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Hex2SqrConverter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;

  if (m_EbsdFileList.size() == 0)
  {
    ss.str("");
    ss << "No files have been selected for import. Have you set the input directory?";
    addErrorMessage(getHumanLabel(), ss.str(), -11);
    setErrorCondition(-1);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Hex2SqrConverter::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Hex2SqrConverter::execute()
{
  std::stringstream ss;
  herr_t err = 0;

  std::vector<int> indices;
  // Loop on Each EBSD File
  float total = static_cast<float>( m_ZEndIndex - m_ZStartIndex );
  int progress = 0;
  int64_t z = m_ZStartIndex;
  /* There is a frailness about the z index and the file list. The programmer
   * using this code MUST ensure that the list of files that is sent into this
   * class is in the appropriate order to match up with the z index (slice index)
   * otherwise the import will have subtle errors. The programmer is urged NOT to
   * simply gather a list from the file system as those lists are sorted in such
   * a way that if the number of digits appearing in the filename are NOT the same
   * then the list will be wrong, ie, this example:
   *
   * slice_1.ang
   * slice_2.ang
   * ....
   * slice_10.ang
   *
   * Most, if not ALL C++ libraries when asked for that list will return the list
   * sorted like the following:
   *
   * slice_1.ang
   * slice_10.ang
   * slice_2.ang
   *
   * which is going to cause problems because the data is going to be placed
   * into the HDF5 file at the wrong index. YOU HAVE BEEN WARNED.
   */
  // int totalSlicesImported = 0;
  for (std::vector<std::string>::iterator filepath = m_EbsdFileList.begin(); filepath != m_EbsdFileList.end(); ++filepath)
  {
    std::string ebsdFName = *filepath;

    progress = static_cast<int>( z - m_ZStartIndex );
    progress = (int)(100.0f * (float)(progress) / total);
    std::string msg = "Converting File: " + ebsdFName;
    ss.str("");

    notifyStatusMessage(msg.c_str());

    // Write the Manufacturer of the OIM file here
    // This list will grow to be the number of EBSD file formats we support
    std::string ext = MXAFileInfo::extension(ebsdFName);
    std::string base = MXAFileInfo::fileNameWithOutExtension(ebsdFName);
    std::string path = MXAFileInfo::parentPath(ebsdFName);
    if(ext.compare(Ebsd::Ang::FileExt) == 0)
    {
      AngReader reader;
      reader.setFileName(ebsdFName);
      reader.setReadHexGrid(true);
      int err = reader.readFile();

      if(err < 0 && err != -600)
      {
        addErrorMessage(getHumanLabel(), reader.getErrorMessage(), reader.getErrorCode());
        setErrorCondition(reader.getErrorCode());
        return;
      }
      else if(reader.getGrid().find(Ebsd::Ang::SquareGrid) == 0)
      {
        ss.str("");
        ss << "Ang File is already a square grid: " << ebsdFName;
        setErrorCondition(-55000);
        addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
        return;
      }
      else
      {
        if (err == -600)
        {
          notifyWarningMessage( reader.getErrorMessage(), reader.getErrorCode() );
        }
        std::string origHeader = reader.getOriginalHeader();
        if (origHeader.empty() == true)
        {
          ss.str();
          ss << "Header could not be retrieved: " << ebsdFName;
          setErrorCondition(-55001);
          addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
        }
        char buf[kBufferSize];
        std::stringstream in(origHeader);

        std::string newEbsdFName = path + "/Sqr_" + base + "." + ext;
        std::ofstream outFile;
        outFile.open(newEbsdFName.c_str());

        m_HeaderIsComplete = false;

        float HexXStep = reader.getXStep();
        float HexYStep = reader.getYStep();
        int HexNumColsOdd = reader.getNumOddCols();
        int HexNumColsEven = reader.getNumEvenCols();
        int HexNumRows = reader.getNumRows();
        m_NumCols = (HexNumColsOdd*HexXStep)/m_XResolution;
        m_NumRows = (HexNumRows*HexYStep)/m_YResolution;
        float xSqr, ySqr, xHex1, yHex1, xHex2, yHex2;
        int point, point1, point2;
        int row1, row2, col1, col2;
        float dist1, dist2;
        float* phi1 = reader.getPhi1Pointer();
        float* PHI = reader.getPhiPointer();
        float* phi2 = reader.getPhi2Pointer();
        float* ci = reader.getConfidenceIndexPointer();
        float* iq = reader.getImageQualityPointer();
        float* semsig = reader.getSEMSignalPointer();
        float* fit = reader.getFitPointer();
        int* phase = reader.getPhaseDataPointer();
        while (!in.eof())
        {
          std::string line;
          ::memset(buf, 0, kBufferSize);
          in.getline(buf, kBufferSize);
          line = modifyAngHeaderLine(buf, kBufferSize);
          if(m_HeaderIsComplete == false) outFile << line << std::endl;
        }
        for(int j = 0; j < m_NumRows; j++)
        {
          for(int i = 0; i < m_NumCols; i++)
          {
            xSqr = float(i)*m_XResolution;
            ySqr = float(j)*m_YResolution;
            row1 = ySqr/(HexYStep);
            yHex1 = row1*HexYStep;
            row2 = row1 + 1;
            yHex2 = row2*HexYStep;
            if(row1%2 == 0)
            {
              col1 = xSqr/(HexXStep);
              xHex1 = col1*HexXStep;
              point1 = ((row1/2)*HexNumColsEven) + ((row1/2)*HexNumColsOdd) + col1;
              col2 = (xSqr-(HexXStep/2.0))/(HexXStep);
              xHex2 = col2*HexXStep + (HexXStep/2.0);
              point2 = ((row1/2)*HexNumColsEven) + (((row1/2)+1)*HexNumColsOdd) + col2;
            }
            else
            {
              col1 = (xSqr-(HexXStep/2.0))/(HexXStep);
              xHex1 = col1*HexXStep + (HexXStep/2.0);
              point1 = ((row1/2)*HexNumColsEven) + (((row1/2)+1)*HexNumColsOdd) + col1;
              col2 = xSqr/(HexXStep);
              xHex2 = col2*HexXStep;
              point2 = (((row1/2)+1)*HexNumColsEven) + (((row1/2)+1)*HexNumColsOdd) + col2;
            }
            dist1 = ((xSqr-xHex1)*(xSqr-xHex1)) + ((ySqr-yHex1)*(ySqr-yHex1));
            dist2 = ((xSqr-xHex2)*(xSqr-xHex2)) + ((ySqr-yHex2)*(ySqr-yHex2));
            if(dist1 <= dist2 || row1 == (HexNumRows-1)) {point = point1;}
            else {point = point2;}
            outFile << "  " << phi1[point] << "	" << PHI[point] << "	" << phi2[point] << "	" << xSqr << "	" << ySqr << "	" << iq[point] << "	" << ci[point] << "	" << phase[point] << "	" << semsig[point] << "	" << fit[point] << "	" << std::endl;
          }
        }
      }
    }
    else if(ext.compare(Ebsd::Ctf::FileExt) == 0)
    {
      std::cout << "Ctf files are not on a hexagonal grid and do not need to be converted." << std::endl;
    }
    else
    {
      err = -1;
      ss.str("");
      ss << "The File extension was not detected correctly";
      addErrorMessage(getHumanLabel(), ss.str(), err);
      setErrorCondition(-1);
      return;
    }

  }

  notifyStatusMessage("Import Complete");
}

// -----------------------------------------------------------------------------
//  Modify the Header line of the ANG file if necessary
// -----------------------------------------------------------------------------
std::string Hex2SqrConverter::modifyAngHeaderLine(char* buf, size_t length)
{
  std::string line = "";
  if (buf[0] != '#')
  {
    line = buf;
    m_HeaderIsComplete = true;
    return line;
  }
  // Start at the first character and walk until you find another non-space character
  size_t i = 1;
  while(buf[i] == ' ')
  {
    ++i;
  }
  size_t wordStart = i;
  size_t wordEnd = i+1;
  while(1)
  {
    if (buf[i] == 45 || buf[i] == 95) { ++i; } // "-" or "_" character
    else if (buf[i] >= 65 && buf[i] <=90) { ++i; } // Upper case alpha character
    else if (buf[i] >= 97 && buf[i] <=122) {++i; } // Lower case alpha character
    else { break;}
  }
  wordEnd = i;

  std::string word( &(buf[wordStart]), wordEnd - wordStart);

  if (word.size() == 0)
  {
    line = buf;
    return line;
  }
  if (word.compare(Ebsd::Ang::Grid) == 0)
  {
    line = "# " + word + ": SqrGrid";
  }
  else if (word.compare(Ebsd::Ang::XStep) == 0)
  {
    line = "# " + word + ": " + float_to_string(m_XResolution);
  }
  else if (word.compare(Ebsd::Ang::YStep) == 0)
  {
    line = "# " + word + ": " + float_to_string(m_YResolution);
  }
  else if (word.compare(Ebsd::Ang::NColsOdd) == 0)
  {
    line = "# " + word + ": " + int_to_string(m_NumCols);
  }
  else if (word.compare(Ebsd::Ang::NColsEven) == 0)
  {
    line = "# " + word + ": " + int_to_string(m_NumCols);
  }
  else if (word.compare(Ebsd::Ang::NRows) == 0)
  {
    line = "# " + word + ": " + int_to_string(m_NumRows);
  }
  else
  {
    line = buf;
  }
  return line;
}

std::string Hex2SqrConverter::int_to_string(int value)
{
  std::stringstream oss;
  oss << value;
  return oss.str();
}

std::string Hex2SqrConverter::float_to_string(float value)
{
  std::stringstream oss;
  oss << value;
  return oss.str();
}
