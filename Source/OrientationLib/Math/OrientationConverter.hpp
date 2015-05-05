/* ============================================================================
 * Copyright (c) 2015 BlueQuartz Software, LLC
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Jackson, BlueQuartz Software nor the names of
 * its contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was written under United States Air Force Contract number
 *                 FA8650-07-D-5800 & FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _OrientationConverter_H_
#define _OrientationConverter_H_


#include <QtCore/QVector>
#include <QtCore/QString>

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/Math/OrientationArray.hpp"
#include "OrientationLib/Math/OrientationTransforms.hpp"

template<typename T>
class OrientationConverter
{

  public:

    DREAM3D_SHARED_POINTERS(OrientationConverter<T> )
    DREAM3D_TYPE_MACRO(OrientationConverter<T>)
    DREAM3D_CLASS_VERSION(1)

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


    virtual ~OrientationConverter() {}

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

    virtual bool compareRepresentations(T* a, T* b,
                                        const float& epsilon = std::numeric_limits<float>::epsilon()) = 0;
    virtual void sanityCheckInputData() = 0;

    virtual void printRepresentation(T* a) = 0;

    DREAM3D_INSTANCE_PROPERTY(typename DataArray<T>::Pointer, InputData)
    DREAM3D_INSTANCE_PROPERTY(typename DataArray<T>::Pointer, OutputData)

    static QVector<QString> GetOrientationTypeStrings()
    {
      QVector<QString> otypes(6);
      otypes[0] = "Euler";
      otypes[1] = "Orientation Matrix";
      otypes[2] = "Quaternion";
      otypes[3] = "Axis-Angle";
      otypes[4] = "Rodrigues";
      otypes[5] = "Homochoric";
      //otypes[6] = "Cubochoric";
      return otypes;
    }

    static QVector<int> GetComponentCounts()
    {
      QVector<int> counts(6);
      counts[0] = 3;
      counts[1] = 9;
      counts[2] = 4;
      counts[3] = 4;
      counts[4] = 4;
      counts[5] = 3;
      //counts[6] = 3;
      return counts;
    }

    static QVector<OrientationType> GetOrientationTypes()
    {
      QVector<OrientationType> ocTypes(6);
      ocTypes[0] = Euler;
      ocTypes[1] = OrientationMatrix;
      ocTypes[2] = Quaternion;
      ocTypes[3] = AxisAngle;
      ocTypes[4] = Rodrigues;
      ocTypes[5] = Homochoric;
      //ocTypes[6] = Cubochoric;
      return ocTypes;
    }

    static int GetMinIndex() { return 0; }
    static int GetMaxIndex() { return 5; }

  protected:
    OrientationConverter() {}

  private:
    OrientationConverter(const OrientationConverter&); // Copy Constructor Not Implemented
    void operator=( const OrientationConverter& ); // Operator '=' Not Implemented

};

#define OC_CONVERT_BODY(OUTSTRIDE, OUT_ARRAY_NAME, CONVERSION_METHOD)\
  typedef OrientationArray<T> OrientationArray_t;\
  typename DataArray<T>::Pointer input = this->getInputData();\
  T* inPtr = input->getPointer(0);\
  size_t nTuples = input->getNumberOfTuples();\
  int inStride = input->getNumberOfComponents();\
  int outStride = OUTSTRIDE;\
  QVector<size_t> cDims(1, outStride); /* Create the n component (nx1) based array.*/ \
  typename DataArray<T>::Pointer output = DataArray<T>::CreateArray(nTuples, cDims, #OUT_ARRAY_NAME);\
  output->initializeWithZeros(); /* Intialize the array with Zeros */ \
  T* quatPtr = output->getPointer(0);\
  for (size_t i = 0; i < nTuples; ++i) { \
    OrientationArray_t rot(inPtr, inStride); \
    OrientationArray_t res(quatPtr, outStride); \
    OrientationTransforms<OrientationArray_t, T>::CONVERSION_METHOD(rot, res); \
    inPtr = inPtr + inStride; /* Increment input pointer */ \
    quatPtr = quatPtr + outStride; /* Increment output pointer*/ \
  }\
  this->setOutputData(output);



template<typename T>
class EulerConvertor : public OrientationConverter<T>
{
  public:
    DREAM3D_SHARED_POINTERS(EulerConvertor<T> )
    DREAM3D_TYPE_MACRO_SUPER(EulerConvertor<T>, OrientationConverter<T>)
    DREAM3D_CLASS_VERSION(1)

    DREAM3D_STATIC_NEW_MACRO(EulerConvertor<T> )

    virtual ~EulerConvertor() {}

    virtual typename OrientationConverter<T>::OrientationType getOrientationRepresentation()
    { return OrientationConverter<T>::Euler; }

    virtual void toEulers()
    {
      typedef typename DataArray<T>::Pointer PointerType;
      PointerType input = this->getInputData();
      PointerType output = boost::dynamic_pointer_cast<DataArray<T> >(input->deepCopy());
      this->setOutputData(output);
    }

    virtual void toOrientationMatrix()
    {
      sanityCheckInputData();
      OC_CONVERT_BODY(9, OrientationMatrix, eu2om)
    }

    virtual void toQuaternion()
    {
      sanityCheckInputData();
      OC_CONVERT_BODY(4, Quaternions, eu2qu)
    }

    virtual void toAxisAngle()
    {
      sanityCheckInputData();
      OC_CONVERT_BODY(4, AxisAngle, eu2ax)
    }

    virtual void toRodrigues()
    {
      sanityCheckInputData();
      OC_CONVERT_BODY(4, Rodrigues, eu2ro)
    }

    virtual void toHomochoric()
    {
      sanityCheckInputData();
      OC_CONVERT_BODY(3, Homochoric, eu2ho)
    }

    virtual void toCubochoric()
    {
      sanityCheckInputData();
      OC_CONVERT_BODY(4, Cubochoric, eu2cu)
    }

    virtual void sanityCheckInputData()
    {
      typename DataArray<T>::Pointer input = this->getInputData();
      T* inPtr = input->getPointer(0);
      size_t nTuples = input->getNumberOfTuples();
      int inStride = input->getNumberOfComponents();
      for (size_t i = 0; i < nTuples; ++i)
      {
        if(inPtr[0] < 0.0) { inPtr[0] = inPtr[0] + DREAM3D::Constants::k_2Pi; }
        else {inPtr[0] = fmod(inPtr[0], DREAM3D::Constants::k_2Pi); }
        if(inPtr[1] < 0.0) { inPtr[1] = inPtr[1] + DREAM3D::Constants::k_Pi; }
        else { inPtr[1] = fmod(inPtr[1], DREAM3D::Constants::k_Pi); }
        if(inPtr[2] < 0.0) { inPtr[2] = inPtr[2] + DREAM3D::Constants::k_2Pi; }
        else { inPtr[2] = fmod(inPtr[2], DREAM3D::Constants::k_2Pi); }

        inPtr = inPtr + inStride;
      }
    }

    virtual bool compareRepresentations(T* a, T* b, const float& epsilon = std::numeric_limits<float>::epsilon())
    {
      bool close = false;
      for(int i = 0; i < 3; i++)
      {
        close = (epsilon > std::abs(a[i] - b[i]));
        if(!close) { return close; }
      }
      return close;
    }


    virtual void printRepresentation(T* a)
    {
      printf("%0.8f, %0.8f, %0.8f", a[0], a[1], a[2]);
    }

  protected:
    EulerConvertor() :
      OrientationConverter<T>()
    {}

    explicit EulerConvertor(typename DataArray<T>::Pointer data) :
      OrientationConverter<T>()
    {
      this->setInputData(data);
    }

  private:

    EulerConvertor(const EulerConvertor&); // Copy Constructor Not Implemented
    void operator=(const EulerConvertor&); // Operator '=' Not Implemented
};


template<typename T>
class QuaternionConvertor : public OrientationConverter<T>
{
  public:
    DREAM3D_SHARED_POINTERS(QuaternionConvertor<T> )
    DREAM3D_TYPE_MACRO_SUPER(QuaternionConvertor<T>, OrientationConverter<T>)
    DREAM3D_CLASS_VERSION(1)
    DREAM3D_STATIC_NEW_MACRO(QuaternionConvertor<T> )

    virtual ~QuaternionConvertor() {}

    virtual typename OrientationConverter<T>::OrientationType getOrientationRepresentation()
    { return OrientationConverter<T>::Quaternion; }

    virtual void toEulers()
    {
      OC_CONVERT_BODY(3, Eulers, qu2eu)
    }

    virtual void toOrientationMatrix()
    {
      OC_CONVERT_BODY(9, OrientationMatrix, qu2om)
    }

    virtual void toQuaternion()
    {
      typedef typename DataArray<T>::Pointer PointerType;
      PointerType input = this->getInputData();
      PointerType output = boost::dynamic_pointer_cast<DataArray<T> >(input->deepCopy());
      this->setOutputData(output);
    }

    virtual void toAxisAngle()
    {
      OC_CONVERT_BODY(4, AxisAngle, qu2ax)
    }

    virtual void toRodrigues()
    {
      OC_CONVERT_BODY(4, Rodrigues, qu2ro)
    }

    virtual void toHomochoric()
    {
      OC_CONVERT_BODY(3, Homochoric, qu2ho)
    }

    virtual void toCubochoric()
    {
      OC_CONVERT_BODY(4, Cubochoric, qu2cu)
    }

    virtual void sanityCheckInputData()
    {
    }

    virtual void printRepresentation(T* om)
    {
      // if(layout == QuaternionMath<float>::QuaternionVectorScalar)
      {
        printf("<%3.6f\t%3.6f\t%3.6f> %3.6f\n", om[0], om[1], om[2], om[3] );
      }

//      else if(layout == QuaternionMath<float>::QuaternionScalarVector)
//      {
//        printf("%3.6f <%3.6f\t%3.6f\t%3.6f>\n", om[0], om[1], om[2], om[3] );
//      }
    }

    virtual bool compareRepresentations(T* a, T* b, const float& epsilon = std::numeric_limits<float>::epsilon())
    {
      bool close = false;
      return close;
    }

  protected:
    QuaternionConvertor() :
      OrientationConverter<T>()
    {}
    explicit QuaternionConvertor(typename DataArray<T>::Pointer data) :
      OrientationConverter<T>()
    {}


  private:

    QuaternionConvertor(const QuaternionConvertor&); // Copy Constructor Not Implemented
    void operator=( const QuaternionConvertor& ); // Operator '=' Not Implemented
};


template<typename T>
class OrientationMatrixConvertor : public OrientationConverter<T>
{
  public:
    DREAM3D_SHARED_POINTERS(OrientationMatrixConvertor<T> )
    DREAM3D_TYPE_MACRO_SUPER(OrientationMatrixConvertor<T>, OrientationConverter<T>)
    DREAM3D_CLASS_VERSION(1)
    DREAM3D_STATIC_NEW_MACRO(OrientationMatrixConvertor<T> )

    virtual ~OrientationMatrixConvertor() {}

    virtual typename OrientationConverter<T>::OrientationType getOrientationRepresentation()
    { return OrientationConverter<T>::OrientationMatrix; }

    virtual void toEulers()
    {
      OC_CONVERT_BODY(3, Eulers, qu2eu)
    }

    virtual void toOrientationMatrix()
    {
      typedef typename DataArray<T>::Pointer PointerType;
      PointerType input = this->getInputData();
      PointerType output = boost::dynamic_pointer_cast<DataArray<T> >(input->deepCopy());
      this->setOutputData(output);
    }

    virtual void toQuaternion()
    {
      OC_CONVERT_BODY(4, Quaternion, qu2om)
    }

    virtual void toAxisAngle()
    {
      OC_CONVERT_BODY(4, AxisAngle, qu2ax)
    }

    virtual void toRodrigues()
    {
      OC_CONVERT_BODY(4, Rodrigues, qu2ro)
    }

    virtual void toHomochoric()
    {
      OC_CONVERT_BODY(3, Homochoric, qu2ho)
    }

    virtual void toCubochoric()
    {
      OC_CONVERT_BODY(4, Cubochoric, qu2cu)
    }

    virtual void sanityCheckInputData()
    {
    }

    virtual bool compareRepresentations(T* a, T* b, const float& epsilon = std::numeric_limits<float>::epsilon())
    {
      bool close = false;
      return close;
    }

    virtual void printRepresentation(T* om)
    {
      printf("|    % 3.6f    % 3.6f    % 3.6f    |\n", om[0], om[1], om[2]);
      printf("|    % 3.6f    % 3.6f    % 3.6f    |\n", om[3], om[4], om[5]);
      printf("|    % 3.6f    % 3.6f    % 3.6f    |\n", om[6], om[7], om[8]);

    }

  protected:

    OrientationMatrixConvertor() :
      OrientationConverter<T>()
    {}
    explicit OrientationMatrixConvertor(typename DataArray<T>::Pointer data) :
      OrientationConverter<T>()
    {}

  private:

    OrientationMatrixConvertor(const OrientationMatrixConvertor&); // Copy Constructor Not Implemented
    void operator=( const OrientationMatrixConvertor& ); // Operator '=' Not Implemented
};


template<typename T>
class AxisAngleConvertor : public OrientationConverter<T>
{
  public:
    DREAM3D_SHARED_POINTERS(AxisAngleConvertor<T> )
    DREAM3D_TYPE_MACRO_SUPER(AxisAngleConvertor<T>, OrientationConverter<T>)
    DREAM3D_CLASS_VERSION(1)
    DREAM3D_STATIC_NEW_MACRO(AxisAngleConvertor<T> )

    virtual ~AxisAngleConvertor() {}

    virtual typename OrientationConverter<T>::OrientationType getOrientationRepresentation()
    { return OrientationConverter<T>::AxisAngle; }

    virtual void toEulers()
    {
      OC_CONVERT_BODY(3, Eulers, ax2eu)
    }

    virtual void toOrientationMatrix()
    {
      OC_CONVERT_BODY(9, OrientationMatrix, ax2om)
    }

    virtual void toQuaternion()
    {
      OC_CONVERT_BODY(4, Quaternions, ax2qu)
    }

    virtual void toAxisAngle()
    {
      typedef typename DataArray<T>::Pointer PointerType;
      PointerType input = this->getInputData();
      PointerType output = boost::dynamic_pointer_cast<DataArray<T> >(input->deepCopy());
      this->setOutputData(output);
    }

    virtual void toRodrigues()
    {
      OC_CONVERT_BODY(4, Rodrigues, ax2ro)
    }

    virtual void toHomochoric()
    {
      OC_CONVERT_BODY(3, Homochoric, ax2ho)
    }

    virtual void toCubochoric()
    {
      OC_CONVERT_BODY(4, Cubochoric, ax2cu)
    }

    virtual void sanityCheckInputData()
    {
    }

    virtual bool compareRepresentations(T* a, T* b, const float& epsilon = std::numeric_limits<float>::epsilon())
    {
      bool close = false;
      return close;
    }

    virtual void printRepresentation(T* om)
    {

    }

  protected:
    AxisAngleConvertor() :
      OrientationConverter<T>()
    {}

    explicit AxisAngleConvertor(typename DataArray<T>::Pointer data) :
      OrientationConverter<T>()
    {
      this->setInputData(data);
    }

  private:

    AxisAngleConvertor(const AxisAngleConvertor&); // Copy Constructor Not Implemented
    void operator=(const AxisAngleConvertor&); // Operator '=' Not Implemented
};


template<typename T>
class RodriguesConvertor : public OrientationConverter<T>
{
  public:
    DREAM3D_SHARED_POINTERS(RodriguesConvertor<T> )
    DREAM3D_TYPE_MACRO_SUPER(RodriguesConvertor<T>, OrientationConverter<T>)
    DREAM3D_CLASS_VERSION(1)
    DREAM3D_STATIC_NEW_MACRO(RodriguesConvertor<T> )


    virtual ~RodriguesConvertor() {}

    virtual typename OrientationConverter<T>::OrientationType getOrientationRepresentation()
    { return OrientationConverter<T>::Rodrigues; }

    virtual void toEulers()
    {
      OC_CONVERT_BODY(3, Eulers, ro2eu)
    }

    virtual void toOrientationMatrix()
    {
      OC_CONVERT_BODY(9, OrientationMatrix, ro2om)
    }

    virtual void toQuaternion()
    {
      OC_CONVERT_BODY(4, Quaternions, ro2qu)
    }

    virtual void toAxisAngle()
    {
      OC_CONVERT_BODY(4, AxisAngle, ro2qu)
    }

    virtual void toRodrigues()
    {
      typedef typename DataArray<T>::Pointer PointerType;
      PointerType input = this->getInputData();
      PointerType output = boost::dynamic_pointer_cast<DataArray<T> >(input->deepCopy());
      this->setOutputData(output);
    }

    virtual void toHomochoric()
    {
      OC_CONVERT_BODY(3, Homochoric, ro2ho)
    }

    virtual void toCubochoric()
    {
      OC_CONVERT_BODY(4, Cubochoric, ro2cu)
    }

    virtual void sanityCheckInputData()
    {
    }

    virtual bool compareRepresentations(T* a, T* b, const float& epsilon = std::numeric_limits<float>::epsilon())
    {
      bool close = false;
      return close;
    }

    virtual void printRepresentation(T* om)
    {

    }
  protected:

    RodriguesConvertor() :
      OrientationConverter<T>()
    {}

    explicit RodriguesConvertor(typename DataArray<T>::Pointer data) :
      OrientationConverter<T>()
    {
      this->setInputData(data);
    }

  private:

    RodriguesConvertor(const RodriguesConvertor&); // Copy Constructor Not Implemented
    void operator=(const RodriguesConvertor&); // Operator '=' Not Implemented
};


template<typename T>
class HomochoricConvertor : public OrientationConverter<T>
{
  public:
    DREAM3D_SHARED_POINTERS(HomochoricConvertor<T> )
    DREAM3D_TYPE_MACRO_SUPER(HomochoricConvertor<T>, OrientationConverter<T>)
    DREAM3D_CLASS_VERSION(1)
    DREAM3D_STATIC_NEW_MACRO(HomochoricConvertor<T> )


    virtual ~HomochoricConvertor() {}

    virtual typename OrientationConverter<T>::OrientationType getOrientationRepresentation()
    { return OrientationConverter<T>::Homochoric; }

    virtual void toEulers()
    {
      OC_CONVERT_BODY(3, Eulers, ho2eu)
    }

    virtual void toOrientationMatrix()
    {
      OC_CONVERT_BODY(9, OrientationMatrix, ho2om)
    }

    virtual void toQuaternion()
    {
      OC_CONVERT_BODY(4, Quaternions, ho2qu)
    }

    virtual void toAxisAngle()
    {
      OC_CONVERT_BODY(4, AxisAngle, ho2qu)
    }

    virtual void toRodrigues()
    {
      OC_CONVERT_BODY(4, Rodrigues, ho2ro)
    }

    virtual void toHomochoric()
    {
      typedef typename DataArray<T>::Pointer PointerType;
      PointerType input = this->getInputData();
      PointerType output = boost::dynamic_pointer_cast<DataArray<T> >(input->deepCopy());
      this->setOutputData(output);
    }

    virtual void toCubochoric()
    {
      OC_CONVERT_BODY(4, Cubochoric, ho2cu)
    }

    virtual void sanityCheckInputData()
    {
    }

    virtual bool compareRepresentations(T* a, T* b, const float& epsilon = std::numeric_limits<float>::epsilon())
    {
      bool close = false;
      return close;
    }

    virtual void printRepresentation(T* om)
    {

    }

  protected:

    HomochoricConvertor() :
      OrientationConverter<T>()
    {}

    explicit HomochoricConvertor(typename DataArray<T>::Pointer data) :
      OrientationConverter<T>()
    {
      this->setInputData(data);
    }

  private:

    HomochoricConvertor(const HomochoricConvertor&); // Copy Constructor Not Implemented
    void operator=(const HomochoricConvertor&); // Operator '=' Not Implemented
};

template<typename T>
class CubochoricConvertor : public OrientationConverter<T>
{
  public:
    DREAM3D_SHARED_POINTERS(CubochoricConvertor<T> )
    DREAM3D_TYPE_MACRO_SUPER(CubochoricConvertor<T>, OrientationConverter<T>)
    DREAM3D_CLASS_VERSION(1)
    DREAM3D_STATIC_NEW_MACRO(CubochoricConvertor<T> )


    virtual ~CubochoricConvertor() {}

    virtual typename OrientationConverter<T>::OrientationType getOrientationRepresentation()
    { return OrientationConverter<T>::Cubochoric; }

    virtual void toEulers()
    {
      //  OC_CONVERT_BODY(3, Eulers, cu2eu)
    }

    virtual void toOrientationMatrix()
    {
      // OC_CONVERT_BODY(9, OrientationMatrix, cu2om)
    }

    virtual void toQuaternion()
    {
      OC_CONVERT_BODY(4, Quaternions, cu2qu)
    }

    virtual void toAxisAngle()
    {
      OC_CONVERT_BODY(4, AxisAngle, cu2qu)
    }

    virtual void toRodrigues()
    {
      // OC_CONVERT_BODY(4, Rodrigues, cu2ro)
    }

    virtual void toHomochoric()
    {
      OC_CONVERT_BODY(3, Homochoric, cu2ho)
    }

    virtual void toCubochoric()
    {
      typedef typename DataArray<T>::Pointer PointerType;
      PointerType input = this->getInputData();
      PointerType output = boost::dynamic_pointer_cast<DataArray<T> >(input->deepCopy());
      this->setOutputData(output);
    }

    virtual void sanityCheckInputData()
    {
    }

    virtual bool compareRepresentations(T* a, T* b, const float& epsilon = std::numeric_limits<float>::epsilon())
    {
      bool close = false;
      return close;
    }

    virtual void printRepresentation(T* om)
    {

    }

  protected:
    CubochoricConvertor() :
      OrientationConverter<T>()
    {}

    explicit CubochoricConvertor(typename DataArray<T>::Pointer data) :
      OrientationConverter<T>()
    {
      this->setInputData(data);
    }

  private:

    CubochoricConvertor(const CubochoricConvertor&); // Copy Constructor Not Implemented
    void operator=(const CubochoricConvertor&); // Operator '=' Not Implemented
};

#endif /* _OrientationConverter_H_ */
