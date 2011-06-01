/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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
#include "OIMImport.h"

#include "MXA/HDF5/H5Utilities.h"
#include "MXA/Utilities/StringUtils.h"

#include "ANG/H5AngImporter.h"

#ifdef DREAM3D_USE_QT
#define CHECK_FOR_CANCELED(AClass)\
  if (this->m_Cancel) { \
    err = H5Fclose(fileId);\
  QString msg = #AClass; \
  msg += " was Canceled"; \
  emit updateMessage(msg);\
  emit updateProgress(0);\
  emit finished();\
  return;}

#define CHECK_ERROR(name)\
    if(err < 0) {\
      setErrorCondition(err);\
      err = H5Fclose(fileId);\
      QString msg = #name;\
      msg += " Returned an error condition. Grain Generator has stopped.";\
      emit updateMessage(msg);\
      emit updateProgress(0);\
      emit finished();\
      return;   }

#define PROGRESS_MESSAGE(str, percent)\
  progressMessage(QString::fromStdString(str), percent);

#else
#define CHECK_FOR_CANCELED(AClass)\
  ;
#define CHECK_ERROR(name)\
    ;
#define PROGRESS_MESSAGE(str, percent)\
  progressMessage(str, percent);
#endif


#if DREAM3D_USE_QT
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OIMImport::Pointer OIMImport::New( QObject* parent)
{
  Pointer sharedPtr(new OIMImport(parent));
  return sharedPtr;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OIMImport::OIMImport(
#if DREAM3D_USE_QT
QObject* parent
#endif
) :
#if DREAM3D_USE_QT
QObject(parent),
#endif
m_ZStartIndex(0),
m_ZEndIndex(0),
m_ZResolution(1.0)
#if DREAM3D_USE_QT
  ,m_Cancel(false)
#endif
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OIMImport::~OIMImport()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImport::compute()
{
  herr_t err = 0;
  hid_t fileId = -1;

  if (m_OutputFile.empty() == true)
  {
    std::string s("H5AngImport Error: The output file was not set correctly or is empty. The current value is '");
    s.append("'. Please set the output file before running the importer. ");
    m_Cancel = true;
    PROGRESS_MESSAGE(s, 100);
    err = -1;
    CHECK_FOR_CANCELED(OIMImport)
    return;
  }
  // Create File
  fileId = H5Utilities::createFile(m_OutputFile);
  if (fileId < 0) {
    std::string s("The Output HDF5 file could not be created. Check Permissions, if the File is in use by another program.");
    m_Cancel = true;
    PROGRESS_MESSAGE(s, 100);
    CHECK_FOR_CANCELED(OIMImport)
    return;
  }

  // Write Z index start, Z index end and Z Resolution to the HDF5 file
  err = H5Lite::writeScalarDataset(fileId, Ang::ZStartIndex, m_ZStartIndex);
  err = H5Lite::writeScalarDataset(fileId, Ang::ZEndIndex, m_ZEndIndex);
  err = H5Lite::writeScalarDataset(fileId, Ang::ZResolution, m_ZResolution);
  // Write the Manufacturer of the OIM file here
  err = H5Lite::writeStringDataset(fileId, Ang::Manufacturer, Ang::TSL );

  std::vector<int> indices;
  // Loop on Each Ang File
  float total = m_ZEndIndex - m_ZStartIndex;
  int progress = 0;
  int z = m_ZStartIndex;

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
  for (std::vector<std::string>::iterator filepath = m_AngFileList.begin(); filepath != m_AngFileList.end(); ++filepath )
  {
    std::string angFName = *filepath;

    CHECK_FOR_CANCELED(OIMImport)
    progress = z - m_ZStartIndex;
    progress = (int)(100.0f * (float)(progress)/total);
    std::string msg = "Importing: " + angFName;
    PROGRESS_MESSAGE(msg, progress );
    H5AngImporter::Pointer conv = H5AngImporter::New();
    err = conv->importAngFile(fileId, z, angFName);
    if (err < 0)
    {
      setCancel(true);
    }
    indices.push_back(z);
    ++z;
  }

  if (false == m_Cancel) {
  // Write an Index data set which contains all the z index values which
  // should help speed up the reading side of this file
    std::vector<hsize_t> dims(1, indices.size());
    err = H5Lite::writeVectorDataset(fileId, Ang::Index, dims, indices);
  }
  err = H5Fclose(fileId);
  PROGRESS_MESSAGE("Import Complete", 100);
#if DREAM3D_USE_QT
  emit finished();
#endif
}

#ifdef DREAM3D_USE_QT
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImport::on_CancelWorker()
{
  setCancel(true);
}

#endif
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImport::progressMessage(AIM_STRING message, int progress)
{
#ifdef DREAM3D_USE_QT
  emit updateMessage(QString(message));
  emit updateProgress(progress);
  //  std::cout << message.toStdString() << std::endl;
#else
  std::cout << message << std::endl;
#endif
}
