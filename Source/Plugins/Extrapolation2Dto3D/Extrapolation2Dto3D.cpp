

#include "Extrapolation2Dto3D.h"

#include "MXA/Common/LogTime.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Extrapolation2Dto3D::Extrapolation2Dto3D()
{
std::cout << "Extrapolation2Dto3D Constructor" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Extrapolation2Dto3D::~Extrapolation2Dto3D()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Extrapolation2Dto3D::execute()
{
  int err = 0;
  // Instantiate our Extrapolation2Dto3DFunc object
  m = Extrapolation2Dto3DFunc::New();
  // Initialize some benchmark timers
  START_CLOCK()

  /****************************************************************
  * Put your Pipeline Code Here
  ****************************************************************/
  
  
  
  updateProgressAndMessage(("Generation Completed"), 100);
}
