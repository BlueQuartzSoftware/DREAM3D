
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//

#include "MXAPrivateFileWriter.h"
#include <MXA/Common/LogTime.h>



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MXAFILEWRITER_CLASS_NAME::MXAFILEWRITER_CLASS_NAME(const std::string &filename) :
  _filename(filename)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MXAFILEWRITER_CLASS_NAME::~MXAFILEWRITER_CLASS_NAME()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MXAFILEWRITER_CLASS_NAME::initWriter()
{
  _outStream.open(this->_filename.c_str(), std::ios::out | std::ios::binary);
  if ( _outStream.fail() ) {
    std::cout << logTime() << "Output File '" << this->_filename << "' could not be opened for writing. Please" <<
     " check the path is correct and permissions are set correctly on the parent folder." << std::endl;
     return false;
  }
  return true;
}

