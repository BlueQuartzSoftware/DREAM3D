
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//

#include "MXAPrivateWindowsFileReader.h"
#include <MXA/Common/MXAEndian.h>
#include <MXA/Common/LogTime.h>
#include <iostream>



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MXAFILEREADER_CLASS_NAME::MXAFILEREADER_CLASS_NAME(const QString &filename) :
  _filename(filename)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MXAFILEREADER_CLASS_NAME::~MXAFILEREADER_CLASS_NAME()
{
#if defined (WINDOWS_LARGE_FILE_SUPPORT)
  int error = CloseHandle(_instream); //Close the file
  if (0 == error)
  {
    std::cout << "MXAFILEREADER_CLASS_NAME:: Error Closing File " << _filename << std::endl;
  }
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MXAFILEREADER_CLASS_NAME::initReader()
{
#if defined (WINDOWS_LARGE_FILE_SUPPORT)

  _instream = CreateFile(TEXT(this->_filename.c_str()),    // file to open
                   GENERIC_READ,          // open for reading
                   FILE_SHARE_READ,       // share for reading
                   NULL,                  // default security
                   OPEN_EXISTING,         // existing file only
                   FILE_FLAG_SEQUENTIAL_SCAN, // normal file
                   NULL);                 // no attr. template
  if (_instream == INVALID_HANDLE_VALUE)
  {
    std::cout << logTime() << "Error: Failed to open file: " + std::string(this->_filename) << " Error code:(" << GetLastError() << ")" << std::endl;
    return false;
  }
#else
  // Open a stream with a large buffer.

  _buffer.resize(BUFF_SIZE, 0);
  _instream.rdbuf()->pubsetbuf ( &(_buffer.front()), BUFF_SIZE );
  _instream.open ( _filename.c_str(), std::ifstream::in | std::ifstream::binary );
  if ( !_instream.is_open() ) {
    std::cout << logTime() <<  "Error: Failed to open file: " + _filename << std::endl;
    return false;
  }
#endif
  return true;
}



