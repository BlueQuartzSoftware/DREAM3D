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

#if 0
void EulerConvertor::toEulers()
{

}
void EulerConvertor::toOrientationMatrix()
{

}
void EulerConvertor::toQuaternion()
{

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
#endif


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

