/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#ifndef _HDF5ScopedFileSentineal_H_
#define _HDF5ScopedFileSentineal_H_




#include "H5Support/H5Utilities.h"
#include "H5Support/H5Lite.h"

#include "H5Support/H5Support.h"
#include "H5Support/H5SupportDLLExport.h"

/**
 * @brief The HDF5FileSentinel class ensures the HDF5 file that is currently open
 * is closed when the variable goes out of Scope
 */
class H5Support_EXPORT HDF5ScopedFileSentinel
{
  public:
    HDF5ScopedFileSentinel(hid_t* fileId, bool turnOffErrors);
    virtual ~HDF5ScopedFileSentinel();

    void setFileId(hid_t* fileId);
    hid_t* getFileId();
    void addGroupId(hid_t* gid);

  private:
    hid_t* m_FileId;
    bool m_TurnOffErrors;
    std::vector<hid_t*> m_Groups;

    herr_t (*_oldHDF_error_func)(hid_t, void*);
    void* _oldHDF_error_client_data;
};

class H5Support_EXPORT HDF5ScopedGroupSentinel
{
  public:
    HDF5ScopedGroupSentinel(hid_t* gid, bool turnOffErrors);
    virtual ~HDF5ScopedGroupSentinel();
    void addGroupId(hid_t* gid);

  private:
    bool m_TurnOffErrors;
    std::vector<hid_t*> m_Groups;

    herr_t (*_oldHDF_error_func)(hid_t, void*);
    void* _oldHDF_error_client_data;
};





#endif /* _HDF5ScopedFileSentineal_H_ */

