///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////

#ifndef ANGREADER_H_
#define ANGREADER_H_

#include <string>
#include <map>

#include "AIM/Common/AIMCommonConfiguration.h"
#include "AngConstants.h"
#include "AngHeaderEntry.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
/**
 * @brief Creates a "setter" method to set the property.
 */
#define angSetMacro_old(type, prpty) \
  void set##prpty(type value) { this->m_##prpty = value; }

#define angGetMacro_old(type, prpty)\
    type get##prpty() {return m_##prpty; }


#define angInstanceProperty_old(type, prpty)\
  private:\
      type   m_##prpty;\
  public:\
    angSetMacro_old(type, prpty)\
    angGetMacro_old(type, prpty)


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#define angSetStringMacro_old(prpty, varname) \
  void set##prpty(const std::string &value) { this->m_##prpty = value; }


/**
 * @brief Creates a "getter" method to retrieve the value of the property.
 */
#define angGetStringMacro_old( prpty, varname) \
  std::string get##prpty() { return varname; }


/**
 * @brief Creates setters and getters in the form of 'setXXX()' and 'getXXX()' methods
 */
#define angStringProperty_old(prpty)\
  private:\
      std::string   m_##prpty;\
  public:\
    angGetStringMacro_old(prpty, m_##prpty)\
    angSetStringMacro_old(prpty, m_##prpty)


#define STATIC_STRING_CONSTANT(str, quoted)\
  static std::string str##() { return std::string(quoted); }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
/**
 * @brief Creates a "setter" method to set the property.
 */
#define angSetMacro( HeaderType, type, prpty, key) \
  void set##prpty(type value) { \
    HeaderType* p = dynamic_cast<HeaderType*>(m_Headermap[key].get()); \
    if (NULL != p) { p->setValue(value); } else {\
      std::cout << "Value for Key: " << key << " was null." << std::endl;} }

/**
 * @brief Creates a "getter" method to retrieve the value of the property.
 */
#define angGetMacro(HeaderType, type, prpty, key) \
  type get##prpty() { \
    HeaderType* p = dynamic_cast<HeaderType*>(m_Headermap[key].get());\
    if (NULL != p) { return p->getValue(); } else {\
      std::cout << "Value for Key: " << key << " was null." << std::endl; return 0;} }


#define angInstanceProperty(HeaderType, type, prpty, key)\
  public:\
    angSetMacro(HeaderType, type, prpty, key)\
    angGetMacro(HeaderType, type, prpty, key)

#define SET_POINTER(name, var)\
void set##name##Pointer(float* f)\
  {\
    if (m_##var != NULL && m_##var != f)\
    {\
      deallocateArrayData(m_##var);\
      m_##var = NULL;\
    }\
    m_##var = f;\
  }

/**
* @class AngReader AngReader.h AngReader/AngReader.h
* @brief This class is a self contained TSL OIM .ang file reader. It should only be dependent on
* standard C++ features
* @author Michael A. Jackson for BlueQuartz Software
* @date Mar 1, 2010
* @version 1.0
*/
class AIMCOMMON_EXPORT AngReader
{
  public:
    AngReader();
    virtual ~AngReader();

    /** @brief Constants defined for the 5 orientation options */
    const static int UpperRightOrigin = 0;
    const static int UpperLeftOrigin = 1;
    const static int LowerLeftOrigin = 2;
    const static int LowerRightOrigin = 3;
    const static int NoOrientation = 4;

    /** @brief Allow the user to set the origin of the scan */
    angInstanceProperty_old(int, UserOrigin)

    /** @brief Sets the file name of the ang file to be read */
    angStringProperty_old( FileName )
    angInstanceProperty_old(size_t, NumberOfElements);
    angInstanceProperty_old(int, NumFields);

    MXA_INSTANCE_STRING_PROPERTY(CompleteHeader);

    /** @brief Header Values from the TSL ang file */

    angInstanceProperty(AngHeaderEntry<float>, float, TEMpixPerum, TSL::OIM::TEMPIXPerUM)
    angInstanceProperty(AngHeaderEntry<float>, float, XStar, TSL::OIM::XStar)
    angInstanceProperty(AngHeaderEntry<float>, float, YStar, TSL::OIM::YStar)
    angInstanceProperty(AngHeaderEntry<float>, float, ZStar, TSL::OIM::ZStar)
    angInstanceProperty(AngHeaderEntry<float>, float, WorkingDistance, TSL::OIM::WorkingDistance)
    angInstanceProperty(AngStringHeaderEntry, std::string, Phase, TSL::OIM::Phase)
    angInstanceProperty(AngStringHeaderEntry, std::string, MaterialName, TSL::OIM::MaterialName)
    angInstanceProperty(AngStringHeaderEntry, std::string, Formula, TSL::OIM::Formula)
    angInstanceProperty(AngStringHeaderEntry, std::string, Info, TSL::OIM::Info)
    angInstanceProperty(AngStringHeaderEntry, std::string, Symmetry, TSL::OIM::Symmetry)
    angInstanceProperty(AngStringHeaderEntry, std::string, LatticeConstants, TSL::OIM::LatticeConstants)
    angInstanceProperty(AngStringHeaderEntry, std::string, NumberFamilies, TSL::OIM::NumberFamilies)
    angInstanceProperty(AngStringHeaderEntry, std::string, HKLFamilies, TSL::OIM::HKLFamilies)
    angInstanceProperty(AngStringHeaderEntry, std::string, Categories, TSL::OIM::Categories)
    angInstanceProperty(AngStringHeaderEntry, std::string, Grid, TSL::OIM::Grid)
    angInstanceProperty(AngHeaderEntry<float>, float, XStep, TSL::OIM::XStep)
    angInstanceProperty(AngHeaderEntry<float>, float, YStep, TSL::OIM::YStep)
//    angInstanceProperty(AngHeaderEntry<float>, float, ZStep, TSL::OIM::ZStep) // NOT actually in the file, but may be needed
//    angInstanceProperty(AngHeaderEntry<float>, float, ZPos, TSL::OIM::ZPos) // NOT actually in the file, but may be needed
//    angInstanceProperty(AngHeaderEntry<float>, float, ZMax, TSL::OIM::ZMax) // NOT actually in the file, but may be needed
    angInstanceProperty(AngHeaderEntry<int>, int, NumOddCols, TSL::OIM::NColsOdd)
    angInstanceProperty(AngHeaderEntry<int>, int, NumEvenCols, TSL::OIM::NColsEven)
    angInstanceProperty(AngHeaderEntry<int>, int, NumRows, TSL::OIM::NRows)
    angInstanceProperty(AngStringHeaderEntry, std::string, OIMOperator, TSL::OIM::Operator)
    angInstanceProperty(AngStringHeaderEntry, std::string, SampleID, TSL::OIM::SampleId)
    angInstanceProperty(AngStringHeaderEntry, std::string, ScanID, TSL::OIM::ScanId)

    /**
    * @brief Reads the complete TSL .ang file.
    * @return 1 on success
    */
    virtual int readFile();

    /**
    * @brief Reads ONLY the header portion of the TSL .ang file
    * @return 1 on success
    */
    virtual int readHeaderOnly();


    /** @brief Get a pointer to the Phi1 data. Note that this array WILL be deleted
    * when this object is deleted. Most programs should copy the data from the array
    * into their own array storage. */
    float* getPhi1Pointer() { return m_Phi1; }
    float* getPhiPointer() { return m_Phi; }
    float* getPhi2Pointer() { return m_Phi2; }
    float* getXPosPointer() { return m_X; }
    float* getYPosPointer() { return m_Y; }
    float* getImageQualityPointer() { return m_Iq; }
    float* getImageQuality2Pointer() { return m_D1; }
    float* getConfidenceIndexPointer() { return m_Ci; }
    float* getPhasePointer() { return m_PhaseData; }
    float* getD1Pointer() { return m_D1; }
    float* getD2Pointer() { return m_D1; }

    SET_POINTER(Phi1, Phi1)
    SET_POINTER(Phi, Phi)
    SET_POINTER(Phi2, Phi2)
    SET_POINTER(XPos, X)
    SET_POINTER(YPos, Y)
    SET_POINTER(ImageQuality, Iq)
    SET_POINTER(ConfidenceIndex, Ci)
    SET_POINTER(Phase, PhaseData)
    SET_POINTER(D1, D1)
    SET_POINTER(D2, D2)

protected:
    // Needed by subclasses
    std::map<std::string, HeaderEntry::Pointer> m_Headermap;

    /** @brief Allocates the proper amount of memory (after reading the header portion of the file)
    * and then splats '0' across all the bytes of the memory allocation
    */
    void initPointers(size_t numElements);

    /** @brief 'free's the allocated memory and sets the pointer to NULL
    */
    void deletePointers();

    /**
     * @brief Allocats a contiguous chunk of memory to store values from the .ang file
     * @param numberOfElements The number of elements in the Array. This method can
     * also optionally produce SSE aligned memory for use with SSE intrinsics
     * @return Pointer to allocated memory
     */
      template<typename T>
      T* allocateArray(size_t numberOfElements)
      {
  #if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
        T* m_buffer = static_cast<T*>( _mm_malloc (numberOfElements * sizeof(T), 16) );
  #else
        T*  m_buffer = new T[numberOfElements];
  #endif
        m_NumberOfElements = numberOfElements;
        return m_buffer;
      }

    /**
     * @brief Deallocates memory that has been previously allocated. This will set the
     * value of the pointer passed in as the argument to NULL.
     * @param ptr The pointer to be freed.
     */
      template<typename T>
      void deallocateArrayData(T* &ptr)
      {
        if (ptr != NULL && this->m_ManageMemory == true)
        {
  #if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
          _mm_free(ptr );
  #else
          delete[] ptr;
  #endif
          ptr = NULL;
          m_NumberOfElements = 0;
        }
      }

private:


    float* m_Phi1;
    float* m_Phi;
    float* m_Phi2;
    float* m_Iq;
    float* m_Ci;
    float* m_PhaseData;
    float* m_X;
    float* m_Y;
    float* m_D1;
    float* m_D2;
    bool m_ManageMemory;  // We are going to forcibly manage the memory. There is currently NO option otherwise.
    bool m_headerComplete;

    /** @brief Parses the value from a single line of the header section of the TSL .ang file
    * @param line The line to parse
    */
    void parseHeaderLine(char* buf, size_t length);

    /** @brief Parses the data from a line of data from the TSL .ang file
    * @param line The line of data to parse
    */
    void readData(const std::string &line, float xMaxValue, float yMaxValue, int nCols, float xstep, float ystep);

    AngReader(const AngReader&);    // Copy Constructor Not Implemented
    void operator=(const AngReader&);  // Operator '=' Not Implemented

};

#endif /* ANGREADER_H_ */
