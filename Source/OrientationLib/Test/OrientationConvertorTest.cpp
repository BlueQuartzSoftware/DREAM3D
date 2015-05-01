



#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Utilities/UnitTestSupport.hpp"

#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/Math/OrientationConvertor.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestEulerConversion()
{

  QVector<size_t> cDims(1, 3);
  FloatArrayType::Pointer eulers = FloatArrayType::CreateArray(2, cDims, "Eulers");

  OrientationConvertor* ocEulers = new EulerConvertor(eulers);
  ocEulers->convertRepresentationTo(OrientationConvertor::Quaternion);

  FloatArrayType::Pointer quats = ocEulers->getOutputData();


}



// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{

  int err = EXIT_SUCCESS;

  DREAM3D_REGISTER_TEST( TestEulerConversion() );


  return err;
}
