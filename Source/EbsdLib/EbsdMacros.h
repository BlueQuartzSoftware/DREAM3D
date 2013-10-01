/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef EBSDMACROS_H_
#define EBSDMACROS_H_

/* Define our buffer size for reading data */
#define kBufferSize 1024

/**
 * @brief These macros are used to read header values from an HDF5 file, NOT From a .ang or .ctf file
 */

#define READ_EBSD_HEADER_DATA(cname, class, m_msgType, getName, key)\
{\
  m_msgType t;\
  err = H5Lite::readScalarDataset(gid, key, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss <<  cname << ": The header value for '" << key << "' was not found in the H5EBSD file. Was this header originally found in the files that were imported into this H5EBSD File?" << std::endl;\
    setErrorCode(-90001);\
    setErrorMessage(ss.str());\
    err = H5Gclose(gid);\
    return -1; }\
  else {\
    EbsdHeaderEntry::Pointer p = m_HeaderMap[key];\
    class* c = dynamic_cast<class*>(p.get());\
    c->setValue(t);\
  }\
}

#define READ_EBSD_HEADER_STRING_DATA(cname, class, m_msgType, getName, key)\
{\
  std::string t;\
  err = H5Lite::readStringDataset(gid, key, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss <<  cname << ": The header value for '" << key << "' was not found in the H5EBSD file. Was this header originally found in the files that were imported into this H5EBSD File?" << std::endl;\
    setErrorCode(-90002);\
    setErrorMessage(ss.str());\
    err = H5Gclose(gid);\
    return -1; }\
  else {\
      EbsdHeaderEntry::Pointer p = m_HeaderMap[key];\
      class* c = dynamic_cast<class*>(p.get());\
      c->setValue(t);\
  }\
}


#define READ_PHASE_STRING_DATA(cname, pid, fqKey, key, phase)\
{\
  std::string t;\
  err = H5Lite::readStringDataset(pid, fqKey, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss <<  cname << ": The header value for '" << fqKey << "' was not found in the H5EBSD file. Was this header originally found in the files that were imported into this H5EBSD File?" << std::endl;\
    setErrorCode(-90003);\
    setErrorMessage(ss.str());\
    err = H5Gclose(pid); H5Gclose(phasesGid);H5Gclose(gid);\
    return -1; }\
    else {\
      phase->set##key(t);\
    }\
}

#define READ_PHASE_HEADER_DATA(cname, pid, m_msgType, fqKey, key, phase)\
{\
  m_msgType t;\
  err = H5Lite::readScalarDataset(pid, fqKey, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss <<  cname << ": The header value for '" << fqKey << "' was not found in the H5EBSD file. Was this header originally found in the files that were imported into this H5EBSD File?" << std::endl;\
    setErrorCode(-90004);\
    setErrorMessage(ss.str());\
    err = H5Gclose(pid);H5Gclose(phasesGid);H5Gclose(gid);\
    return -1; }\
  else {\
    phase->set##key(t);\
  }\
}

#define READ_PHASE_HEADER_DATA_CAST(cname, pid, cast, m_msgType, fqKey, key, phase)\
{\
  m_msgType t;\
 /* if (H5Lite::datasetExists(pid, fqKey) == false) { return 0; }*/\
  err = H5Lite::readScalarDataset(pid, fqKey, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss <<  cname << ": The header value for '" << fqKey << "' was not found in the H5EBSD file. Was this header originally found in the files that were imported into this H5EBSD File?" << std::endl;\
    setErrorCode(-90005);\
    setErrorMessage(ss.str());\
    err = H5Gclose(pid);H5Gclose(phasesGid);H5Gclose(gid);\
    return -1; }\
  else {\
    phase->set##key(static_cast<cast>(t));\
  }\
}

#define READ_PHASE_HEADER_ARRAY(cname, pid, m_msgType, fqKey, key, phase)\
{\
  m_msgType t;\
  err = H5Lite::readVectorDataset(pid, fqKey, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss <<  cname << ": The header value for '" << fqKey << "' was not found in the H5EBSD file. Was this header originally found in the files that were imported into this H5EBSD File?" << std::endl;\
    setErrorCode(-90006);\
    setErrorMessage(ss.str());\
    err = H5Gclose(pid);H5Gclose(phasesGid);H5Gclose(gid);\
    return -1; }\
  else {\
    phase->set##key(t);\
  }\
}


#define SHUFFLE_ARRAY(name, var, m_msgType)\
  { m_msgType* f = allocateArray<m_msgType>(totalDataRows);\
  for (size_t i = 0; i < totalDataRows; ++i)\
  {\
    size_t nIdx = shuffleTable[i];\
    f[nIdx] = var[i];\
  }\
  set##name##Pointer(f); }


#endif /* EBSDMACROS_H_ */
