

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
  const std::string filename = "test.txt";
  // Instantiate our Extrapolation2Dto3DFunc object
  m = Extrapolation2Dto3DFunc::New();
  // Initialize some benchmark timers
  START_CLOCK()

  updateProgressAndMessage(("Loading Data"), 16);
  m->loadtwodimxgrains(filename);
  m->loadtwodimygrains(filename);
  m->loadtwodimzgrains(filename);
  CHECK_FOR_CANCELED(Extrapolation2Dto3DFunc, "2D to 3D Extrapolation was canceled", loadtwodimygrains)

  updateProgressAndMessage(("Cleaning Data"), 16);
  m->find_bincontributions();
  CHECK_FOR_CANCELED(Extrapolation2Dto3DFunc, "2D to 3D Extrapolation was canceled", loadtwodimygrains)

  updateProgressAndMessage(("Cleaning Data"), 16);
  m->find_pixels();
  CHECK_FOR_CANCELED(Extrapolation2Dto3DFunc, "2D to 3D Extrapolation was canceled", loadtwodimygrains)

  updateProgressAndMessage(("Generation Completed"), 100);
}
