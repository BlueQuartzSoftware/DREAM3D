#include <iostream>

#include "OrientationLib/Core/OrientationTransformation.hpp"
#include "OrientationLib/Core/Orientation.hpp"
#include "OrientationLib/Core/Quaternion.hpp"
#include "OrientationLib/Test/TestPrintFunctions.h"

#include "OrientationLib/LaueOps/HexagonalLowOps.h"

int32_t main(int argc, char* argv[])
{

  std::cout << "Input Euler Angle 1" << std::endl;
  OrientationType o1(4.0526571, 1.2298285, 2.0490279);
  OrientationPrinters::Print_EU(o1);
  QuatType quat1 = OrientationTransformation::eu2qu<OrientationType, QuatType>(o1);
  OrientationPrinters::Print_QU<OrientationType, double>(quat1);

  std::cout << "Input Euler Angle 2" << std::endl;
  OrientationType o2(0.66373795, 0.897506, 5.7032394);
  OrientationPrinters::Print_EU(o2);

  QuatType quat2 = OrientationTransformation::eu2qu<OrientationType, QuatType>(o2);
  OrientationPrinters::Print_QU<OrientationType, double>(quat2);

  double n1 = 0.0, n2 = 0.0, n3 = 0.0;
  double w = std::numeric_limits<float>::max();
  HexagonalLowOps::Pointer cubic = HexagonalLowOps::New();
  w = cubic->getMisoQuat(quat1, quat2, n1, n2, n3);

  QuatType misoQuat(n1, n2, n3, w);
  std::cout << "OUTPUT Angle" << std::endl;
  std::cout << "w: " << w << " Rads " << w * 180.0 / M_PI << " Degs" << std::endl;
  OrientationPrinters::Print_QU<OrientationType, double>(misoQuat);
  OrientationType euOut = OrientationTransformation::qu2eu<QuatType, OrientationType>(misoQuat);
  OrientationPrinters::Print_EU(euOut);

  return 0;
}
