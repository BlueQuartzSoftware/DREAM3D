#include "OrientationConvertor.h"

#include <iostream>


#include "OrientationLib/Math/OrientationTransforms.h"
#include "OrientationLib/Math/OrientationArray.hpp"

typedef OrientationArray<float> FloatOrientationArray_t;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationConvertor::OrientationConvertor()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationConvertor::~OrientationConvertor()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationConvertor::convertRepresentationTo( OrientationType repType )
{
    if(repType == Euler) { toEulers(); }
    if(repType == OrientationMatrix) { toOrientationMatrix(); }
    if(repType == Quaternion) { toQuaternion(); }
    if(repType == AxisAngle) { toAxisAngle(); }
    if(repType == Rodrigues) { toRodrigues(); }
    if(repType == Homochoric) { toHomochoric(); }
    if(repType == Cubochoric) { toCubochoric(); }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EulerConvertor::EulerConvertor()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EulerConvertor::EulerConvertor(FloatArrayType::Pointer data) :
  OrientationConvertor()
{
  setInputData(data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EulerConvertor::~EulerConvertor()
{

}

void EulerConvertor::toEulers()
{

}
void EulerConvertor::toOrientationMatrix()
{

}
void EulerConvertor::toQuaternion()
{
  FloatArrayType::Pointer input = getInputData();
  size_t nTuples = input->getNumberOfTuples();
  int stride = input->getNumberOfComponents();

  QVector<size_t> cDims(1, 4);
  FloatArrayType::Pointer output = FloatArrayType::CreateArray(nTuples, cDims, "Quaternions");
  OrientationTransforms rotTrans;

  float* inPtr = input->getPointer(0);

  for (size_t i = 0; i < nTuples; ++i) {
    inPtr = inPtr + stride;
    FloatOrientationArray_t rot(inPtr, stride);
  //  rotTrans.eu2qu<FloatOrientationArray_t>(rot, )

  }


  setOutputData(output);
}
void EulerConvertor::toAxisAngle()
{

}
void EulerConvertor::toRodrigues()
{

}
void EulerConvertor::toHomochoric()
{

}
void EulerConvertor::toCubochoric()
{

}


QuaternionConvertor::QuaternionConvertor()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuaternionConvertor::QuaternionConvertor(FloatArrayType::Pointer data) :
  OrientationConvertor()
{
setInputData(data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuaternionConvertor::~QuaternionConvertor()
{

}

void QuaternionConvertor::toEulers()
{

}
void QuaternionConvertor::toOrientationMatrix()
{

}
void QuaternionConvertor::toQuaternion()
{

}
void QuaternionConvertor::toAxisAngle()
{

}
void QuaternionConvertor::toRodrigues()
{

}
void QuaternionConvertor::toHomochoric()
{

}
void QuaternionConvertor::toCubochoric()
{

}

