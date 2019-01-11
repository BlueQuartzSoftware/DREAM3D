/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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


#pragma once

/* Define our buffer size for reading data */
#define kBufferSize 1024

/**
 * @brief These macros are used to read header values from an HDF5 file, NOT From a .ang or .ctf file
 */

#define READ_EBSD_HEADER_DATA(cname, class, Type, getName, key, gid)                                                                                                                                   \
  {                                                                                                                                                                                                    \
    auto t = static_cast<Type>(0);                                                                                                                                                                     \
    err = QH5Lite::readScalarDataset(gid, key, t);                                                                                                                                                     \
    if(err < 0)                                                                                                                                                                                        \
    {                                                                                                                                                                                                  \
      QString ss =                                                                                                                                                                                     \
          QObject::tr("%1: The header value for '%2' was not found in the H5EBSD file. Was this header originally found in the files that were imported into this H5EBSD File?").arg(cname, key);      \
      setErrorCode(-90001);                                                                                                                                                                            \
      setErrorMessage(ss);                                                                                                                                                                             \
      err = H5Gclose(gid);                                                                                                                                                                             \
      return -1;                                                                                                                                                                                       \
    }                                                                                                                                                                                                  \
    EbsdHeaderEntry::Pointer p = m_HeaderMap[key];                                                                                                                                                     \
    auto c = dynamic_cast<class*>(p.get());                                                                                                                                                            \
    c->setValue(t);                                                                                                                                                                                    \
  }

#define READ_EBSD_HEADER_STRING_DATA(cname, class, Type, getName, key, gid)                                                                                                                            \
  {                                                                                                                                                                                                    \
    QString t;                                                                                                                                                                                         \
    err = QH5Lite::readStringDataset(gid, key, t);                                                                                                                                                     \
    if(err < 0)                                                                                                                                                                                        \
    {                                                                                                                                                                                                  \
      QString ss =                                                                                                                                                                                     \
          QObject::tr("%1: The header value for '%2' was not found in the H5EBSD file. Was this header originally found in the files that were imported into this H5EBSD File?").arg(cname, key);      \
      setErrorCode(-90002);                                                                                                                                                                            \
      setErrorMessage(ss);                                                                                                                                                                             \
      err = H5Gclose(gid);                                                                                                                                                                             \
      return -1;                                                                                                                                                                                       \
    }                                                                                                                                                                                                  \
    EbsdHeaderEntry::Pointer p = m_HeaderMap[key];                                                                                                                                                     \
    auto c = dynamic_cast<class*>(p.get());                                                                                                                                                            \
    c->setValue(t);                                                                                                                                                                                    \
  }

#define READ_PHASE_STRING_DATA(cname, pid, fqKey, key, phase)                                                                                                                                          \
  {                                                                                                                                                                                                    \
    QString t;                                                                                                                                                                                         \
    err = QH5Lite::readStringDataset(pid, fqKey, t);                                                                                                                                                   \
    if(err < 0)                                                                                                                                                                                        \
    {                                                                                                                                                                                                  \
      QString ss =                                                                                                                                                                                     \
          QObject::tr("%1: The header value for '%2' was not found in the H5EBSD file. Was this header originally found in the files that were imported into this H5EBSD File?").arg(cname, fqKey);    \
      setErrorCode(-90003);                                                                                                                                                                            \
      setErrorMessage(ss);                                                                                                                                                                             \
      err = H5Gclose(pid);                                                                                                                                                                             \
      H5Gclose(phasesGid);                                                                                                                                                                             \
      H5Gclose(gid);                                                                                                                                                                                   \
      return -1;                                                                                                                                                                                       \
    }                                                                                                                                                                                                  \
    phase->set##key(t);                                                                                                                                                                                \
  }

#define READ_PHASE_HEADER_DATA(cname, pid, Type, fqKey, key, phase)                                                                                                                                    \
  {                                                                                                                                                                                                    \
    Type t;                                                                                                                                                                                            \
    err = QH5Lite::readScalarDataset(pid, fqKey, t);                                                                                                                                                   \
    if(err < 0)                                                                                                                                                                                        \
    {                                                                                                                                                                                                  \
      QString ss =                                                                                                                                                                                     \
          QObject::tr("%1: The header value for '%2' was not found in the H5EBSD file. Was this header originally found in the files that were imported into this H5EBSD File?").arg(cname, fqKey);    \
      setErrorCode(-90004);                                                                                                                                                                            \
      setErrorMessage(ss);                                                                                                                                                                             \
      err = H5Gclose(pid);                                                                                                                                                                             \
      H5Gclose(phasesGid);                                                                                                                                                                             \
      H5Gclose(gid);                                                                                                                                                                                   \
      return -1;                                                                                                                                                                                       \
    }                                                                                                                                                                                                  \
    phase->set##key(t);                                                                                                                                                                                \
  }

#define READ_PHASE_HEADER_DATA_CAST(cname, pid, cast, Type, fqKey, key, phase)                                                                                                                         \
  {                                                                                                                                                                                                    \
    Type t;                                                                                                                                                                                            \
    err = QH5Lite::readScalarDataset(pid, fqKey, t);                                                                                                                                                   \
    if(err < 0)                                                                                                                                                                                        \
    {                                                                                                                                                                                                  \
      QString ss =                                                                                                                                                                                     \
          QObject::tr("%1: The header value for '%2' was not found in the H5EBSD file. Was this header originally found in the files that were imported into this H5EBSD File?").arg(cname, fqKey);    \
      setErrorCode(-90005);                                                                                                                                                                            \
      setErrorMessage(ss);                                                                                                                                                                             \
      err = H5Gclose(pid);                                                                                                                                                                             \
      H5Gclose(phasesGid);                                                                                                                                                                             \
      H5Gclose(gid);                                                                                                                                                                                   \
      return -1;                                                                                                                                                                                       \
    }                                                                                                                                                                                                  \
    phase->set##key(static_cast<cast>(t));                                                                                                                                                             \
  }

#define READ_PHASE_HEADER_ARRAY(cname, pid, Type, fqKey, key, phase)                                                                                                                                   \
  {                                                                                                                                                                                                    \
    std::vector<Type> t;                                                                                                                                                                               \
    err = QH5Lite::readVectorDataset(pid, fqKey, t);                                                                                                                                                   \
    if(err < 0)                                                                                                                                                                                        \
    {                                                                                                                                                                                                  \
      QString ss =                                                                                                                                                                                     \
          QObject::tr("%1: The header value for '%2' was not found in the H5EBSD file. Was this header originally found in the files that were imported into this H5EBSD File?").arg(cname, fqKey);    \
      setErrorCode(-90006);                                                                                                                                                                            \
      setErrorMessage(ss);                                                                                                                                                                             \
      err = H5Gclose(pid);                                                                                                                                                                             \
      H5Gclose(phasesGid);                                                                                                                                                                             \
      H5Gclose(gid);                                                                                                                                                                                   \
      return -1;                                                                                                                                                                                       \
    }                                                                                                                                                                                                  \
    phase->set##key(QVector<Type>::fromStdVector(t));                                                                                                                                                  \
  }

#define SHUFFLE_ARRAY(name, var, Type)                                                                                                                                                                 \
  {                                                                                                                                                                                                    \
    Type* f = allocateArray<Type>(totalDataRows);                                                                                                                                                      \
    for(size_t i = 0; i < totalDataRows; ++i)                                                                                                                                                          \
    {                                                                                                                                                                                                  \
      size_t nIdx = shuffleTable[i];                                                                                                                                                                   \
      f[nIdx] = var[i];                                                                                                                                                                                \
    }                                                                                                                                                                                                  \
    set##name##Pointer(f);                                                                                                                                                                             \
  }

#define ANG_READER_ALLOCATE_AND_READ(name, h5name, type)                                                                                                                                               \
  free##name##Pointer(); /* Always free the current data before reading new data */                                                                                                                    \
  if(m_ReadAllArrays == true || m_ArrayNames.find(h5name) != m_ArrayNames.end())                                                                                                                       \
  {                                                                                                                                                                                                    \
    auto _##name = allocateArray<type>(totalDataRows);                                                                                                                                                 \
    if(nullptr != _##name)                                                                                                                                                                             \
    {                                                                                                                                                                                                  \
      ::memset(_##name, 0, numBytes);                                                                                                                                                                  \
      err = QH5Lite::readPointerDataset(gid, h5name, _##name);                                                                                                                                         \
      if(err < 0)                                                                                                                                                                                      \
      {                                                                                                                                                                                                \
        deallocateArrayData(_##name); /*deallocate the array*/                                                                                                                                         \
        setErrorCode(-90020);                                                                                                                                                                          \
        ss << "Error reading dataset '" << #name                                                                                                                                                       \
           << "' from the HDF5 file. This data set is required to be in the file because either "                                                                                                      \
              "the program is set to read ALL the Data arrays or the program was instructed to read this array.";                                                                                      \
        setErrorMessage(ss.string());                                                                                                                                                                  \
        err = H5Gclose(gid);                                                                                                                                                                           \
        return -90020;                                                                                                                                                                                 \
      }                                                                                                                                                                                                \
    }                                                                                                                                                                                                  \
    set##name##Pointer(_##name);                                                                                                                                                                       \
  }
