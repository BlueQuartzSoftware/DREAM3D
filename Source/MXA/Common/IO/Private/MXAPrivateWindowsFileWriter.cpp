
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//

#include "MXAPrivateWindowsFileWriter.h"
#include <MXA/Common/LogTime.h>




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MXAFILEWRITER_CLASS_NAME::MXAFILEWRITER_CLASS_NAME(const QString &filename) :
  _filename(filename)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MXAFILEWRITER_CLASS_NAME::~MXAFILEWRITER_CLASS_NAME()
{
#if defined (WINDOWS_LARGE_FILE_SUPPORT)
  int error = CloseHandle(_outStream); //Close the file
  if (0 == error)
  {
    std::cout << logTime() << "MXAFILEWRITER_CLASS_NAME:: Error Closing File " << _filename << std::endl;
  }
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MXAFILEWRITER_CLASS_NAME::initWriter()
{
#if defined (WINDOWS_LARGE_FILE_SUPPORT)

  _outStream = CreateFile(TEXT(this->_filename.c_str()),    // file to open
                   GENERIC_WRITE,          // open for writing
                   FILE_SHARE_WRITE,       // share for writing
                   NULL,                  // default security
                   CREATE_ALWAYS,         // Always create a new file, regardless if the file exists or not
                   FILE_ATTRIBUTE_NORMAL, // normal file
                   NULL);                 // no attr. template
  if (_outStream == INVALID_HANDLE_VALUE)
  {
    std::cout << logTime() << "Output File '" << this->_filename << "' could not be opened for writing. Please" <<
        " check the path is correct and permissions are set correctly on the parent folder." << std::endl;
        return false;
  }
#else
  _outStream.open(this->_filename.c_str(), std::ios::out | std::ios::binary);
  if ( _outStream.fail() ) {
    std::cout << logTime() << "Output File '" << this->_filename << "' could not be opened for writing. Please" <<
     " check the path is correct and permissions are set correctly on the parent folder." << std::endl;
     return false;
  }
#endif
  return true;
}

