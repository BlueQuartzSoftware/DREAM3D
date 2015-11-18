/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _OrientationConverter_H_
#define _OrientationConverter_H_

#include <iostream>     // std::cout, std::fixed, std::scientific


#include <QtCore/QVector>
#include <QtCore/QString>

#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/OrientationMath/OrientationArray.hpp"
#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"

template<typename T>
class OrientationConverter
{

  public:

    SIMPL_SHARED_POINTERS(OrientationConverter<T> )
    SIMPL_TYPE_MACRO(OrientationConverter<T>)
    SIMPL_CLASS_VERSION(1)

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

    /**
     * @brief toEulers Converts the input orientations to Euler Angles
     */
    virtual void toEulers() = 0;

    /**
     * @brief toOrientationMatrix  Converts the input orientations to an Orientation Matrix (3x3)
     */
    virtual void toOrientationMatrix() = 0;

    /**
     * @brief toQuaternion  Converts the input orientations to Quaternions
     */
    virtual void toQuaternion() = 0;

    /**
     * @brief toAxisAngle  Converts the input orientations to Axis Angles
     */
    virtual void toAxisAngle() = 0;

    /**
     * @brief toRodrigues  Converts the input orientations to Rodrigues
     */
    virtual void toRodrigues() = 0;

    /**
     * @brief toHomochoric  Converts the input orientations to Homochoric
     */
    virtual void toHomochoric() = 0;

    /**
     * @brief toCubochoric  Converts the input orientations to Cubochoric
     */
    virtual void toCubochoric() = 0;

    /**
     * @brief compareRepresentations Compares 2 representations of the same type
     * and returns if the values differ only by the tolerance value
     * @param a
     * @param b
     * @param epsilon
     * @return
     */
    virtual bool compareRepresentations(T* a, T* b,
                                        const T& epsilon = std::numeric_limits<T>::epsilon()) = 0;

    /**
     * @brief sanityCheckInputData Runs basic checks on the input data to ensure
     * the intput data falls within certain data ranges.
     */
    virtual void sanityCheckInputData() = 0;

    /**
     * @brief printRepresentation Prints the values of a single representation to
     * an output stream;
     * @param a
     * @param label
     */
    virtual void printRepresentation(std::ostream& out, T* a, const std::string& label = std::string("")) = 0;

    /**
    * @brief
    */
    SIMPL_INSTANCE_PROPERTY(typename DataArray<T>::Pointer, InputData)

    /**
    * @brief
    */
    SIMPL_INSTANCE_PROPERTY(typename DataArray<T>::Pointer, OutputData)

    /**
     * @brief GetOrientationTypeStrings
     * @return
     */
    static QVector<QString> GetOrientationTypeStrings()
    {
      QVector<QString> otypes(7);
      otypes[0] = "Euler";
      otypes[1] = "Orientation Matrix";
      otypes[2] = "Quaternion";
      otypes[3] = "Axis-Angle";
      otypes[4] = "Rodrigues";
      otypes[5] = "Homochoric";
      otypes[6] = "Cubochoric";
      return otypes;
    }

    /**
     * @brief GetComponentCounts
     * @return
     */
    static QVector<int> GetComponentCounts()
    {
      QVector<int> counts(7);
      counts[0] = 3; // Euler
      counts[1] = 9; // Orientation Matrix
      counts[2] = 4; // Quaternion
      counts[3] = 4; // Axis-Angle
      counts[4] = 4; // Rodrigues
      counts[5] = 3; // Homchoric
      counts[6] = 3; // Cubochoric
      return counts;
    }

    /**
     * @brief GetOrientationTypes
     * @return
     */
    static QVector<OrientationType> GetOrientationTypes()
    {
      QVector<OrientationType> ocTypes(7);
      ocTypes[0] = Euler;
      ocTypes[1] = OrientationMatrix;
      ocTypes[2] = Quaternion;
      ocTypes[3] = AxisAngle;
      ocTypes[4] = Rodrigues;
      ocTypes[5] = Homochoric;
      ocTypes[6] = Cubochoric;
      return ocTypes;
    }

    /**
     * @brief GetMinIndex
     * @return
     */
    static int GetMinIndex() { return 0; }

    /**
     * @brief GetMaxIndex
     * @return
     */
    static int GetMaxIndex() { return 6; }

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
  T* OUT_ARRAY_NAME##Ptr = output->getPointer(0);\
  for (size_t i = 0; i < nTuples; ++i) { \
    OrientationArray_t rot(inPtr, inStride); \
    OrientationArray_t res(OUT_ARRAY_NAME##Ptr, outStride); \
    OrientationTransforms<OrientationArray_t, T>::CONVERSION_METHOD(rot, res); \
    inPtr = inPtr + inStride; /* Increment input pointer */ \
    OUT_ARRAY_NAME##Ptr = OUT_ARRAY_NAME##Ptr + outStride; /* Increment output pointer*/ \
  }\
  this->setOutputData(output);



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
class EulerConverter : public OrientationConverter<T>
{
  public:
    SIMPL_SHARED_POINTERS(EulerConverter<T> )
    SIMPL_TYPE_MACRO_SUPER(EulerConverter<T>, OrientationConverter<T>)
    SIMPL_CLASS_VERSION(1)

    SIMPL_STATIC_NEW_MACRO(EulerConverter<T> )

    virtual ~EulerConverter() {}

    virtual typename OrientationConverter<T>::OrientationType getOrientationRepresentation()
    { return OrientationConverter<T>::Euler; }

    virtual void toEulers()
    {
      typedef typename DataArray<T>::Pointer PointerType;
      PointerType input = this->getInputData();
      PointerType output = std::dynamic_pointer_cast<DataArray<T> >(input->deepCopy());
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
      OC_CONVERT_BODY(3, Cubochoric, eu2cu)
    }

    virtual void sanityCheckInputData()
    {

      typename DataArray<T>::Pointer input = this->getInputData();
      T* inPtr = input->getPointer(0);
      size_t nTuples = input->getNumberOfTuples();
      int inStride = input->getNumberOfComponents();
      for (size_t i = 0; i < nTuples; ++i)
      {

        inPtr[0] = fmod(inPtr[0], SIMPLib::Constants::k_2Pi);
        inPtr[1] = fmod(inPtr[1], SIMPLib::Constants::k_Pi);
        inPtr[2] = fmod(inPtr[2], SIMPLib::Constants::k_2Pi);

        if(inPtr[0] < 0.0) { inPtr[0] *= static_cast<T>(-1.0); }
        if(inPtr[1] < 0.0) { inPtr[1] *= static_cast<T>(-1.0); }
        if(inPtr[2] < 0.0) { inPtr[2] *= static_cast<T>(-1.0); }

        inPtr = inPtr + inStride; // This is Pointer arithmetic!!
      }
    }

    virtual bool compareRepresentations(T* a, T* b, const T& epsilon = std::numeric_limits<T>::epsilon())
    {
      bool close = false;
      for(int i = 0; i < 3; i++)
      {
        close = (epsilon > std::fabs(a[i] - b[i]));
        if(!close) { return close; }
      }
      return close;
    }

    virtual void printRepresentation(std::ostream& out, T* eu, const std::string& label = std::string("Eu"))
    {
      out.precision(16);
      out << label << eu[0] << '\t' << eu[1] << '\t' << eu[2] << std::endl;
    }

  protected:
    EulerConverter() :
      OrientationConverter<T>()
    {}

    explicit EulerConverter(typename DataArray<T>::Pointer data) :
      OrientationConverter<T>()
    {
      this->setInputData(data);
    }

  private:

    EulerConverter(const EulerConverter&); // Copy Constructor Not Implemented
    void operator=(const EulerConverter&); // Operator '=' Not Implemented
};



template<typename T>
class OrientationMatrixConverter : public OrientationConverter<T>
{
  public:
    SIMPL_SHARED_POINTERS(OrientationMatrixConverter<T> )
    SIMPL_TYPE_MACRO_SUPER(OrientationMatrixConverter<T>, OrientationConverter<T>)
    SIMPL_CLASS_VERSION(1)
    SIMPL_STATIC_NEW_MACRO(OrientationMatrixConverter<T> )

    virtual ~OrientationMatrixConverter() {}

    virtual typename OrientationConverter<T>::OrientationType getOrientationRepresentation()
    { return OrientationConverter<T>::OrientationMatrix; }

    virtual void toEulers()
    {
      sanityCheckInputData();
      OC_CONVERT_BODY(3, Eulers, om2eu)
    }

    virtual void toOrientationMatrix()
    {
      typedef typename DataArray<T>::Pointer PointerType;
      PointerType input = this->getInputData();
      PointerType output = std::dynamic_pointer_cast<DataArray<T> >(input->deepCopy());
      this->setOutputData(output);
    }

    virtual void toQuaternion()
    {
      sanityCheckInputData();
      OC_CONVERT_BODY(4, Quaternion, om2qu)
    }

    virtual void toAxisAngle()
    {
      sanityCheckInputData();
      OC_CONVERT_BODY(4, AxisAngle, om2ax)
    }

    virtual void toRodrigues()
    {
      sanityCheckInputData();
      OC_CONVERT_BODY(4, Rodrigues, om2ro)
    }

    virtual void toHomochoric()
    {
      sanityCheckInputData();
      OC_CONVERT_BODY(3, Homochoric, om2ho)
    }

    virtual void toCubochoric()
    {
      sanityCheckInputData();
      OC_CONVERT_BODY(3, Cubochoric, om2cu)
    }

    virtual void sanityCheckInputData()
    {
      typename DataArray<T>::Pointer input = this->getInputData();
      T* inPtr = input->getPointer(0);
      size_t nTuples = input->getNumberOfTuples();
      int inStride = input->getNumberOfComponents();
      for (size_t i = 0; i < nTuples; ++i)
      {

        typedef OrientationArray<T> OrientationArrayType;
        typedef typename OrientationTransforms<OrientationArrayType, T>::ResultType ResultType;

        OrientationArrayType oaType(inPtr, 9);

        ResultType res = OrientationTransforms<OrientationArrayType, T>::om_check(oaType);
        if(res.result <= 0)
        {
          std::cout << res.msg << std::endl;
          printRepresentation(std::cout, input->getPointer(i * inStride), std::string("Bad OM"));
          //res = OrientationTransforms<OrientationArrayType, T>::om_check(oaType);
        }

        inPtr = inPtr + inStride; // This is Pointer arithmetic!!
      }
    }

    virtual bool compareRepresentations(T* a, T* b, const T& epsilon = std::numeric_limits<T>::epsilon())
    {
      bool close = false;
      for(int i = 0; i < 9; i++)
      {
        close = (epsilon > std::fabs(a[i] - b[i]));
        if(!close) { return close; }
      }
      return close;
    }

    virtual void printRepresentation(std::ostream& out, T* om, const std::string& label = std::string("Om"))
    {
      out.precision(16);
      out << label << om[0] << '\t' << om[1] << '\t' << om[2] << std::endl;
      out << label << om[3] << '\t' << om[4] << '\t' << om[5] << std::endl;
      out << label << om[6] << '\t' << om[7] << '\t' << om[8] << std::endl;
    }

  protected:

    OrientationMatrixConverter() :
      OrientationConverter<T>()
    {}
    explicit OrientationMatrixConverter(typename DataArray<T>::Pointer data) :
      OrientationConverter<T>()
    {}

  private:

    OrientationMatrixConverter(const OrientationMatrixConverter&); // Copy Constructor Not Implemented
    void operator=( const OrientationMatrixConverter& ); // Operator '=' Not Implemented
};



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
class QuaternionConverter : public OrientationConverter<T>
{
  public:
    SIMPL_SHARED_POINTERS(QuaternionConverter<T> )
    SIMPL_TYPE_MACRO_SUPER(QuaternionConverter<T>, OrientationConverter<T>)
    SIMPL_CLASS_VERSION(1)
    SIMPL_STATIC_NEW_MACRO(QuaternionConverter<T> )

    virtual ~QuaternionConverter() {}

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
      PointerType output = std::dynamic_pointer_cast<DataArray<T> >(input->deepCopy());
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
      OC_CONVERT_BODY(3, Cubochoric, qu2cu)
    }

    virtual void sanityCheckInputData()
    {
    }

    virtual void printRepresentation(std::ostream& out, T* qu, const std::string& label = std::string("Qu"))
    {
      // if(layout == QuaternionMath<float>::QuaternionVectorScalar)
      {
        out.precision(16);
        out << label << qu[0] << '\t' << qu[1] << '\t' << qu[2] << '\t' << qu[3] << std::endl;
      }

//      else if(layout == QuaternionMath<float>::QuaternionScalarVector)
//      {
//        printf("% 3.16f <% 3.16f\t% 3.16f\t% 3.16f>\n", om[0], om[1], om[2], om[3] );
//      }
    }

    virtual bool compareRepresentations(T* a, T* b, const T& epsilon = std::numeric_limits<T>::epsilon())
    {
      bool close = false;
      for(int i = 0; i < 4; i++)
      {
        close = (epsilon > std::fabs(a[i] - b[i]));
        if(!close) { return close; }
      }
      return close;
    }

  protected:
    QuaternionConverter() :
      OrientationConverter<T>()
    {}
    explicit QuaternionConverter(typename DataArray<T>::Pointer data) :
      OrientationConverter<T>()
    {}


  private:

    QuaternionConverter(const QuaternionConverter&); // Copy Constructor Not Implemented
    void operator=( const QuaternionConverter& ); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
class AxisAngleConverter : public OrientationConverter<T>
{
  public:
    SIMPL_SHARED_POINTERS(AxisAngleConverter<T> )
    SIMPL_TYPE_MACRO_SUPER(AxisAngleConverter<T>, OrientationConverter<T>)
    SIMPL_CLASS_VERSION(1)
    SIMPL_STATIC_NEW_MACRO(AxisAngleConverter<T> )

    virtual ~AxisAngleConverter() {}

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
      PointerType output = std::dynamic_pointer_cast<DataArray<T> >(input->deepCopy());
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
      OC_CONVERT_BODY(3, Cubochoric, ax2cu)
    }

    virtual void sanityCheckInputData()
    {
    }

    virtual bool compareRepresentations(T* a, T* b, const T& epsilon = std::numeric_limits<T>::epsilon())
    {
      bool close = false;
      for(int i = 0; i < 4; i++)
      {
        close = (epsilon > std::fabs(a[i] - b[i]));
        if(!close) { return close; }
      }
      return close;
    }

    virtual void printRepresentation(std::ostream& out, T* ax, const std::string& label = std::string("Ax"))
    {
      out.precision(16);
      out << label << "<" << ax[0] << '\t' << ax[1] << '\t' << ax[2] << ">\t" << ax[3] << std::endl;
    }

  protected:
    AxisAngleConverter() :
      OrientationConverter<T>()
    {}

    explicit AxisAngleConverter(typename DataArray<T>::Pointer data) :
      OrientationConverter<T>()
    {
      this->setInputData(data);
    }

  private:

    AxisAngleConverter(const AxisAngleConverter&); // Copy Constructor Not Implemented
    void operator=(const AxisAngleConverter&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
class RodriguesConverter : public OrientationConverter<T>
{
  public:
    SIMPL_SHARED_POINTERS(RodriguesConverter<T> )
    SIMPL_TYPE_MACRO_SUPER(RodriguesConverter<T>, OrientationConverter<T>)
    SIMPL_CLASS_VERSION(1)
    SIMPL_STATIC_NEW_MACRO(RodriguesConverter<T> )


    virtual ~RodriguesConverter() {}

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
      OC_CONVERT_BODY(4, AxisAngle, ro2ax)
    }

    virtual void toRodrigues()
    {
      typedef typename DataArray<T>::Pointer PointerType;
      PointerType input = this->getInputData();
      PointerType output = std::dynamic_pointer_cast<DataArray<T> >(input->deepCopy());
      this->setOutputData(output);
    }

    virtual void toHomochoric()
    {
      OC_CONVERT_BODY(3, Homochoric, ro2ho)
    }

    virtual void toCubochoric()
    {
      OC_CONVERT_BODY(3, Cubochoric, ro2cu)
    }

    virtual void sanityCheckInputData()
    {
    }

    virtual bool compareRepresentations(T* a, T* b, const T& epsilon = std::numeric_limits<T>::epsilon())
    {
      bool close = false;
      for(int i = 0; i < 4; i++)
      {
        close = (epsilon > std::fabs(a[i] - b[i]));
        if(!close) { return close; }
      }
      return close;
    }

    virtual void printRepresentation(std::ostream& out, T* ro, const std::string& label = std::string("Ro"))
    {
      out.precision(16);
      out << label << ro[0] << '\t' << ro[1] << '\t' << ro[2] << "\t" << ro[3] << std::endl;
    }
  protected:

    RodriguesConverter() :
      OrientationConverter<T>()
    {}

    explicit RodriguesConverter(typename DataArray<T>::Pointer data) :
      OrientationConverter<T>()
    {
      this->setInputData(data);
    }

  private:

    RodriguesConverter(const RodriguesConverter&); // Copy Constructor Not Implemented
    void operator=(const RodriguesConverter&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
class HomochoricConverter : public OrientationConverter<T>
{
  public:
    SIMPL_SHARED_POINTERS(HomochoricConverter<T> )
    SIMPL_TYPE_MACRO_SUPER(HomochoricConverter<T>, OrientationConverter<T>)
    SIMPL_CLASS_VERSION(1)
    SIMPL_STATIC_NEW_MACRO(HomochoricConverter<T> )


    virtual ~HomochoricConverter() {}

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
      OC_CONVERT_BODY(4, AxisAngle, ho2ax)
    }

    virtual void toRodrigues()
    {
      OC_CONVERT_BODY(4, Rodrigues, ho2ro)
    }

    virtual void toHomochoric()
    {
      typedef typename DataArray<T>::Pointer PointerType;
      PointerType input = this->getInputData();
      PointerType output = std::dynamic_pointer_cast<DataArray<T> >(input->deepCopy());
      this->setOutputData(output);
    }

    virtual void toCubochoric()
    {
      OC_CONVERT_BODY(3, Cubochoric, ho2cu)
    }

    virtual void sanityCheckInputData()
    {
    }

    virtual bool compareRepresentations(T* a, T* b, const T& epsilon = std::numeric_limits<T>::epsilon())
    {
      bool close = false;
      for(int i = 0; i < 3; i++)
      {
        close = (epsilon > std::fabs(a[i] - b[i]));
        if(!close) { return close; }
      }
      return close;
    }

    virtual void printRepresentation(std::ostream& out, T* ho, const std::string& label = std::string("No"))
    {
      out.precision(16);
      out << label << ho[0] << '\t' << ho[1] << '\t' << ho[2] << std::endl;
    }

  protected:

    HomochoricConverter() :
      OrientationConverter<T>()
    {}

    explicit HomochoricConverter(typename DataArray<T>::Pointer data) :
      OrientationConverter<T>()
    {
      this->setInputData(data);
    }

  private:

    HomochoricConverter(const HomochoricConverter&); // Copy Constructor Not Implemented
    void operator=(const HomochoricConverter&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
class CubochoricConverter : public OrientationConverter<T>
{
  public:
    SIMPL_SHARED_POINTERS(CubochoricConverter<T> )
    SIMPL_TYPE_MACRO_SUPER(CubochoricConverter<T>, OrientationConverter<T>)
    SIMPL_CLASS_VERSION(1)
    SIMPL_STATIC_NEW_MACRO(CubochoricConverter<T> )


    virtual ~CubochoricConverter() {}

    virtual typename OrientationConverter<T>::OrientationType getOrientationRepresentation()
    { return OrientationConverter<T>::Cubochoric; }

    virtual void toEulers()
    {
      OC_CONVERT_BODY(3, Eulers, cu2eu)
    }

    virtual void toOrientationMatrix()
    {
      OC_CONVERT_BODY(9, OrientationMatrix, cu2om)
    }

    virtual void toQuaternion()
    {
      OC_CONVERT_BODY(4, Quaternions, cu2qu)
    }

    virtual void toAxisAngle()
    {
      OC_CONVERT_BODY(4, AxisAngle, cu2ax)
    }

    virtual void toRodrigues()
    {
      OC_CONVERT_BODY(4, Rodrigues, cu2ro)
    }

    virtual void toHomochoric()
    {
      OC_CONVERT_BODY(3, Homochoric, cu2ho)
    }

    virtual void toCubochoric()
    {
      typedef typename DataArray<T>::Pointer PointerType;
      PointerType input = this->getInputData();
      PointerType output = std::dynamic_pointer_cast<DataArray<T> >(input->deepCopy());
      this->setOutputData(output);
    }

    virtual void sanityCheckInputData()
    {
    }

    virtual bool compareRepresentations(T* a, T* b, const T& epsilon = std::numeric_limits<T>::epsilon())
    {
      bool close = false;
      for(int i = 0; i < 3; i++)
      {
        close = (epsilon > std::fabs(a[i] - b[i]));
        if(!close) { return close; }
      }
      return close;
    }

    virtual void printRepresentation(std::ostream& out, T* cu, const std::string& label = std::string("Cu"))
    {
      out.precision(16);
      out << label << cu[0] << '\t' << cu[1] << '\t' << cu[2] << std::endl;
    }

  protected:
    CubochoricConverter() :
      OrientationConverter<T>()
    {}

    explicit CubochoricConverter(typename DataArray<T>::Pointer data) :
      OrientationConverter<T>()
    {
      this->setInputData(data);
    }

  private:

    CubochoricConverter(const CubochoricConverter&); // Copy Constructor Not Implemented
    void operator=(const CubochoricConverter&); // Operator '=' Not Implemented
};

#endif /* _OrientationConverter_H_ */
