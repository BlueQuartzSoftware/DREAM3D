#ifndef _OrientationConvertor_H_
#define _OrientationConvertor_H_


#include "DREAM3DLib/DataArrays/DataArray.hpp"

#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/Math/OrientationArray.hpp"
#include "OrientationLib/Math/OrientationTransforms.h"

template<typename T>
class OrientationConvertor
{

public:

  enum OrientationType
  {
    Euler,
    OrientationMatrix,
    Quaternion,
    AxisAngle,
    Rodrigues,
    Homochoric,
    Cubochoric,
    UnknownOrientationType
  };

  OrientationConvertor() {}
  virtual ~OrientationConvertor() {}

  /**
   * @brief getOrientationRepresentation
   * @return
   */
  virtual OrientationType getOrientationRepresentation() { return UnknownOrientationType; }

  /**
   * @brief convert
   * @param src
   * @return
   */
  void convertRepresentationTo(OrientationType repType)
  {
    if(repType == Euler) { toEulers(); }
    else if(repType == OrientationMatrix) { toOrientationMatrix(); }
    else if(repType == Quaternion) { toQuaternion(); }
    else if(repType == AxisAngle) { toAxisAngle(); }
    else if(repType == Rodrigues) { toRodrigues(); }
    else if(repType == Homochoric) { toHomochoric(); }
    else if(repType == Cubochoric) { toCubochoric(); }
  }

  virtual void toEulers() = 0;
  virtual void toOrientationMatrix() = 0;
  virtual void toQuaternion() = 0;
  virtual void toAxisAngle() = 0;
  virtual void toRodrigues() = 0;
  virtual void toHomochoric() = 0;
  virtual void toCubochoric() = 0;

  DREAM3D_INSTANCE_PROPERTY(typename DataArray<T>::Pointer, InputData)
    DREAM3D_INSTANCE_PROPERTY(typename DataArray<T>::Pointer, OutputData)

protected:

private:
  OrientationConvertor(const OrientationConvertor&); // Copy Constructor Not Implemented
  void operator=( const OrientationConvertor& ); // Operator '=' Not Implemented

};

#define OC_CONVERT_BODY(OUTSTRIDE, OUT_ARRAY_NAME, CONVERSION_METHOD)\
      typedef typename OrientationArray<T> OrientationArray_t;\
      typename DataArray<T>::Pointer input = getInputData();\
      T* inPtr = input->getPointer(0);\
      size_t nTuples = input->getNumberOfTuples();\
      int inStride = input->getNumberOfComponents();\
      int outStride = OUTSTRIDE;\
      QVector<size_t> cDims(1, outStride); /* Create the 4 component (4x1) based array.*/ \
      typename DataArray<T>::Pointer output = DataArray<T>::CreateArray(nTuples, cDims, #OUT_ARRAY_NAME);\
      output->initializeWithZeros(); /* Intialize the array with Zeros */ \
      T* quatPtr = output->getPointer(0);\
      for (size_t i = 0; i < nTuples; ++i) { \
        OrientationArray_t rot(inPtr, inStride); \
        OrientationArray_t res(quatPtr, outStride); \
        OrientationTransforms<OrientationArray_t, T>::CONVERSION_METHOD(rot, res); \
        inPtr = inPtr + inStride; /* Increment euler pointer */ \
        quatPtr = quatPtr + outStride; /* Increment quaternion pointer*/ \
            }\
      setOutputData(output);



template<typename T>
class EulerConvertor : public OrientationConvertor < T >
{
public:
  EulerConvertor() {}
  explicit EulerConvertor(FloatArrayType::Pointer data) :
    OrientationConvertor()
  {
    setInputData(data);
  }
  virtual ~EulerConvertor() {}

  virtual OrientationType getOrientationRepresentation() { return Euler; }

  virtual void toEulers()
  {
    // We are converting from Euler to Euler? Just copy the array?
    // Copy the array? 
  }

  virtual void toOrientationMatrix()
  {
    OC_CONVERT_BODY(9, Quaternions, eu2om)
  }

  virtual void toQuaternion()
  {
    OC_CONVERT_BODY(4, Quaternions, eu2qu)
  }

  virtual void toAxisAngle() 
  {
    OC_CONVERT_BODY(4, AxixAngle, eu2ax)
  }

  virtual void toRodrigues()
  {
    OC_CONVERT_BODY(4, Rodrigues, eu2ro)
  }

  virtual void toHomochoric()
  {
    OC_CONVERT_BODY(3, Homochoric, eu2ho)
  }

  virtual void toCubochoric()
  {
    OC_CONVERT_BODY(4, Cubochoric, eu2cu)
  }

private:

  EulerConvertor(const EulerConvertor&); // Copy Constructor Not Implemented
  void operator=(const EulerConvertor&); // Operator '=' Not Implemented
};

template<typename T>
class QuaternionConvertor : public OrientationConvertor<T>
{
public:
  QuaternionConvertor() {}
  explicit QuaternionConvertor(FloatArrayType::Pointer data) {}
  virtual ~QuaternionConvertor() {}

  virtual OrientationType getOrientationRepresentation()
  {
    return Quaternion;
  }


  virtual void toEulers() {}
  virtual void toOrientationMatrix() {}
  virtual void toQuaternion() {}
  virtual void toAxisAngle() {}
  virtual void toRodrigues() {}
  virtual void toHomochoric() {}
  virtual void toCubochoric() {}

private:

  QuaternionConvertor(const QuaternionConvertor&); // Copy Constructor Not Implemented
  void operator=( const QuaternionConvertor& ); // Operator '=' Not Implemented
  };




#endif /* _OrientationConvertor_H_ */
