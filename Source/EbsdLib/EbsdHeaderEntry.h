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



#ifndef _ebsdheaderentry_h_
#define _ebsdheaderentry_h_

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdSetGetMacros.h"

#if EbsdLib_HDF5_SUPPORT
#include "H5Support/QH5Lite.h"
#endif

/**
 * @brief Creates a static "New" method that creates an instance of thisClass
 */
#define HEADERENTRY_NEW_SUPERCLASS(thisClass, SuperClass)\
  typedef SuperClass::Pointer SuperClass##Type;\
  static SuperClass##Type New##SuperClass(const QString &key) \
  { \
    SuperClass##Type sharedPtr (new thisClass(key)); \
    return sharedPtr; \
  }


#define HEADERENTRY_NEW_SUPERCLASS_VALUE(thisClass, SuperClass)\
  static SuperClass##Type New##SuperClass(const QString &key, T value) \
  { \
    thisClass* object = new thisClass(key);\
    object->setValue(value);\
    SuperClass##Type sharedPtr(object); \
    return sharedPtr; \
  }\


  /**
   * @class EbsdHeaderEntry EbsdHeaderEntry.h EbsdLib/EbsdHeaderEntry.h
   * @brief  This class defines the interface that subclasses need to implement in
   * order to have a conforming object.
   *
   * @date Aug 8, 2011
   * @version 1.0
   */
  class EbsdLib_EXPORT EbsdHeaderEntry
  {
    public:
      EBSD_SHARED_POINTERS(EbsdHeaderEntry)
      EBSD_TYPE_MACRO(EbsdHeaderEntry)

      virtual ~EbsdHeaderEntry() {}

      /**
       * @brief Returns the key of the header entry
       */
      virtual QString getKey() = 0;

#if EbsdLib_HDF5_SUPPORT
      virtual QString getHDFType() = 0;
#endif

      /**
       * @brief Parses the value for a given header key
       * @param value The string as it was parsed from the file
       * @param start The starting position to look for the value
       * @param length The ending position to look for the value
       */
      virtual void parseValue(QByteArray& value) = 0;

      /**
       * @brief prints out the key and value for this header entry
       * @param out The output stream to printe the key/value pair
       */
      virtual void print(std::ostream& out) = 0;

    protected:
      EbsdHeaderEntry() {}


    private:
      EbsdHeaderEntry(const EbsdHeaderEntry&); // Copy Constructor Not Implemented
      void operator=(const EbsdHeaderEntry&); // Operator '=' Not Implemented

  };



#endif /* _EbsdHeaderEntry.h_  */
