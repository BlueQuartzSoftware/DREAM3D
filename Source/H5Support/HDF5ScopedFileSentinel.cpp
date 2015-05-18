/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its 
* contributors may be used to endorse or promote products derived from this software 
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "HDF5ScopedFileSentinel.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HDF5ScopedFileSentinel::HDF5ScopedFileSentinel(hid_t* fileId, bool turnOffErrors) :
  m_FileId(fileId),
  m_TurnOffErrors(turnOffErrors)
{
  if (m_TurnOffErrors == true)
  {
    H5Eget_auto(H5E_DEFAULT, &_oldHDF_error_func, &_oldHDF_error_client_data);
    H5Eset_auto(H5E_DEFAULT, NULL, NULL);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HDF5ScopedFileSentinel::~HDF5ScopedFileSentinel()
{
  if (m_TurnOffErrors == true)
  {
    H5Eset_auto(H5E_DEFAULT, _oldHDF_error_func, _oldHDF_error_client_data);
  }
  for(std::vector<hid_t*>::size_type i = 0; i < m_Groups.size(); ++i)
  {
    hid_t* temp = m_Groups[i];
    if (*temp > 0) { H5Gclose(*temp); *temp = -1; }
  }

  if (*m_FileId > 0)
  {
    H5Utilities::closeFile(*m_FileId);
    *m_FileId = -1;
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HDF5ScopedFileSentinel::setFileId(hid_t* fileId)
{
  m_FileId = fileId;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
hid_t* HDF5ScopedFileSentinel::getFileId()
{
  return m_FileId;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HDF5ScopedFileSentinel::addGroupId(hid_t* gid)
{
  m_Groups.push_back(gid);
}








// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HDF5ScopedGroupSentinel::HDF5ScopedGroupSentinel(hid_t* gid, bool turnOffErrors) :
  m_TurnOffErrors(turnOffErrors)
{
  m_Groups.push_back(gid);
  if (m_TurnOffErrors == true)
  {
    H5Eget_auto(H5E_DEFAULT, &_oldHDF_error_func, &_oldHDF_error_client_data);
    H5Eset_auto(H5E_DEFAULT, NULL, NULL);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HDF5ScopedGroupSentinel::~HDF5ScopedGroupSentinel()
{
  if (m_TurnOffErrors == true)
  {
    H5Eset_auto(H5E_DEFAULT, _oldHDF_error_func, _oldHDF_error_client_data);
  }
  for(std::vector<hid_t*>::size_type i = 0; i < m_Groups.size(); ++i)
  {
    hid_t* temp = m_Groups[i];
    if (*temp > 0) { H5Gclose(*temp); *temp = -1; }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HDF5ScopedGroupSentinel::addGroupId(hid_t* gid)
{
  m_Groups.push_back(gid);
}




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HDF5ScopedObjectSentinel::HDF5ScopedObjectSentinel(hid_t* gid, bool turnOffErrors) :
  m_TurnOffErrors(turnOffErrors)
{
  m_Groups.push_back(gid);
  if (m_TurnOffErrors == true)
  {
    H5Eget_auto(H5E_DEFAULT, &_oldHDF_error_func, &_oldHDF_error_client_data);
    H5Eset_auto(H5E_DEFAULT, NULL, NULL);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HDF5ScopedObjectSentinel::~HDF5ScopedObjectSentinel()
{
  if (m_TurnOffErrors == true)
  {
    H5Eset_auto(H5E_DEFAULT, _oldHDF_error_func, _oldHDF_error_client_data);
  }
  for(std::vector<hid_t*>::size_type i = 0; i < m_Groups.size(); ++i)
  {
    hid_t* temp = m_Groups[i];
    if (*temp > 0) { H5Utilities::closeHDF5Object(*temp); *temp = -1; }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HDF5ScopedObjectSentinel::addGroupId(hid_t* gid)
{
  m_Groups.push_back(gid);
}


