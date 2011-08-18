
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//

#include "MXAPrivateFileReader.h"
#include <MXA/Common/LogTime.h>
#include <MXA/Common/MXAEndian.h>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MXAFILEREADER_CLASS_NAME::MXAFILEREADER_CLASS_NAME(const std::string &filename) :
  _filename(filename)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MXAFILEREADER_CLASS_NAME::~MXAFILEREADER_CLASS_NAME()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MXAFILEREADER_CLASS_NAME::initReader()
{
  // Open a stream with a large buffer.
  _buffer.resize(BUFF_SIZE, 0);
  _instream.rdbuf()->pubsetbuf ( &(_buffer.front()), BUFF_SIZE );
  _instream.open ( _filename.c_str(), std::ifstream::in | std::ifstream::binary );
  if ( !_instream.is_open() ) {
    std::cout << logTime() <<  "Error: Failed to open file: " + _filename << std::endl;
    return false;
  }

  return true;
}




