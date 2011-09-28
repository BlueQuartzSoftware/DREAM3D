

#include "FileConversion.h"

#include "MXA/Common/LogTime.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileConversion::FileConversion()
{
std::cout << "FileConversion Constructor" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileConversion::~FileConversion()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversion::execute()
{
  int err = 0;
  // Instantiate our FileConversionFunc object
  m = FileConversionFunc::New();
  // Initialize some benchmark timers
  START_CLOCK()

  /****************************************************************
  * Put your Pipeline Code Here
  ****************************************************************/
  
  
  
  updateProgressAndMessage(("Generation Completed"), 100);
}
