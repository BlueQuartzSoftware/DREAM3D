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

#include "H5Support/H5Utilities.h"
#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/MXADir.h"
#include "MXA/Utilities/StringUtils.h"

#include "EbsdLib/EbsdImporter.h"
#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/TSL/H5AngImporter.h"
#include "EbsdLib/HKL/CtfConstants.h"
#include "EbsdLib/HKL/H5CtfImporter.h"

#include "DREAM3DLib/Common/Observable.h"

/**
 * @brief Just a dummy class in order to make the EBSDImoprt work with some
 * of the other source codes.
 */
class Hex2SqrConverterFunc : public Observable
{
  public:
    DREAM3D_SHARED_POINTERS(Hex2SqrConverterFunc)
    DREAM3D_STATIC_NEW_MACRO(Hex2SqrConverterFunc)


    virtual ~Hex2SqrConverterFunc()
    {

	}


  protected:
    Hex2SqrConverterFunc()
    {
    }

  private:
    Hex2SqrConverterFunc(const Hex2SqrConverterFunc&); // Copy Constructor Not Implemented
    void operator=(const Hex2SqrConverterFunc&); // Operator '=' Not Implemented

};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Hex2SqrConverter::Hex2SqrConverter() :
    m_ZStartIndex(0), m_ZEndIndex(0), m_XResolution(1.0), m_YResolution(1.0)
{

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
void Hex2SqrConverter::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Hex2SqrConverter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
//  DataContainer* m = getDataContainer();

//  for (std::vector<std::string>::iterator iter = m_EbsdFileList.begin(); iter != m_EbsdFileList.end(); ++iter )
//  {
//    std::cout << (*iter) << std::endl;
//  }


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
  hid_t fileId = -1;
  // This is just a dummy variable to keep the macros happy
  Hex2SqrConverterFunc::Pointer m = Hex2SqrConverterFunc::New();

  // Start the Benchmark clock
  START_CLOCK()

  EbsdImporter::Pointer fileImporter;

  // Write the Manufacturer of the OIM file here
  // This list will grow to be the number of EBSD file formats we support
  std::string ext = MXAFileInfo::extension(m_EbsdFileList.front());
  if(ext.compare(Ebsd::Ang::FileExt) == 0)
  {
    fileImporter = H5AngImporter::New();
  }
  else if(ext.compare(Ebsd::Ctf::FileExt) == 0)
  {
    fileImporter = H5CtfImporter::New();
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

  std::vector<int> indices;
  // Loop on Each EBSD File
  float total = static_cast<float>( m_ZEndIndex - m_ZStartIndex );
  int progress = 0;
  int64_t z = m_ZStartIndex;
  int64_t xDim, yDim;
  float xRes, yRes;
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
  int totalSlicesImported = 0;
  for (std::vector<std::string>::iterator filepath = m_EbsdFileList.begin(); filepath != m_EbsdFileList.end(); ++filepath)
  {
    std::string ebsdFName = *filepath;
    progress = static_cast<int>( z - m_ZStartIndex );
    progress = (int)(100.0f * (float)(progress) / total);
    std::string msg = "Converting File: " + ebsdFName;
    ss.str("");

    notifyStatusMessage(msg.c_str());
    err = fileImporter->importFile(fileId, z, ebsdFName);
    if (err < 0)
    {
      addErrorMessage(getHumanLabel(), fileImporter->getPipelineMessage(), fileImporter->getErrorCondition());
      setErrorCondition(err);
      return;
    }
    totalSlicesImported = totalSlicesImported + fileImporter->numberOfSlicesImported();

    fileImporter->getDims(xDim, yDim);
    fileImporter->getResolution(xRes, yRes);

    if(err < 0)
    {
      ss.str("");
      ss << "Could not write dataset for slice to HDF5 file";
      addErrorMessage(getHumanLabel(), ss.str(), err);
      setErrorCondition(-1);
    }
    indices.push_back( static_cast<int>(z) );
    ++z;
    if(getCancel() == true)
    {
     notifyStatusMessage("Conversion was Canceled");
      return;
    }
  }

  m = Hex2SqrConverterFunc::NullPointer();
 notifyStatusMessage("Import Complete");
}

