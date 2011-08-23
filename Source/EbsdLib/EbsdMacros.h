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

#define READ_EBSD_HEADER_DATA(cname, class, type, getName, key)\
{\
  type t;\
  err = H5Lite::readScalarDataset(gid, key, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss <<  cname << " Error: Could not read Ang Header value '" << t\
    <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
    std::cout << ss.str() << std::endl;\
    err = H5Gclose(gid);\
    return -1; }\
  else {\
    EbsdHeaderEntry::Pointer p = m_Headermap[key];\
    class* c = dynamic_cast<class*>(p.get());\
    c->setValue(t);\
  }\
}

#define READ_EBSD_HEADER_STRING_DATA(cname, class, type, getName, key)\
{\
  std::string t;\
  err = H5Lite::readStringDataset(gid, key, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss << cname << " Error: Could not read Ang Header value '" << t\
    <<  "' to the HDF5 file with data set name '" << key << "'" << std::endl;\
    std::cout << ss.str() << std::endl;\
    err = H5Gclose(gid);\
    return -1; }\
    else {\
      EbsdHeaderEntry::Pointer p = m_Headermap[key];\
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
    ss <<  cname << " Error: Could not read Ang Header value '" << t\
    <<  "' to the HDF5 file with data set name '" << fqKey << "'" << std::endl;\
    std::cout << ss.str() << std::endl;\
    err = H5Gclose(pid); H5Gclose(phasesGid);H5Gclose(gid);\
    return -1; }\
    else {\
      phase->set##key(t);\
    }\
}

#define READ_PHASE_HEADER_DATA(cname, pid, type, fqKey, key, phase)\
{\
  type t;\
  err = H5Lite::readScalarDataset(pid, fqKey, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss <<  cname << " Error: Could not read Ang Header value '" << t\
    <<  "' to the HDF5 file with data set name '" << fqKey << "'" << std::endl;\
    std::cout << ss.str() << std::endl;\
    err = H5Gclose(pid);H5Gclose(phasesGid);H5Gclose(gid);\
    return -1; }\
  else {\
    phase->set##key(t);\
  }\
}

#define READ_PHASE_HEADER_DATA_CAST(cname, pid, cast, type, fqKey, key, phase)\
{\
  type t;\
  err = H5Lite::readScalarDataset(pid, fqKey, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss <<  cname << " Error: Could not read Ang Header value '" << t\
    <<  "' to the HDF5 file with data set name '" << fqKey << "'" << std::endl;\
    std::cout << ss.str() << std::endl;\
    err = H5Gclose(pid);H5Gclose(phasesGid);H5Gclose(gid);\
    return -1; }\
  else {\
    phase->set##key(static_cast<cast>(t));\
  }\
}

#define READ_PHASE_HEADER_ARRAY(cname, pid, type, fqKey, key, phase)\
{\
  type t;\
  err = H5Lite::readVectorDataset(pid, fqKey, t);\
  if (err < 0) {\
    std::ostringstream ss;\
    ss <<  cname << " Error: Could not read Ang Header value "\
    <<  " to the HDF5 file with data set name '" << fqKey << "'" << std::endl;\
    std::cout << ss.str() << std::endl;\
    err = H5Gclose(pid);H5Gclose(phasesGid);H5Gclose(gid);\
    return -1; }\
  else {\
    phase->set##key(t);\
  }\
}



#endif /* EBSDMACROS_H_ */
