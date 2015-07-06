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


#ifndef _CSVGRAINDATAREADER_H_
#define _CSVGRAINDATAREADER_H_

#include <QtCore/QString>

#define SET_GET_POINTER(name, var, type)\
  type* get##name##Pointer() { return m_##name; }\
  void set##name##Pointer(type* f)\
  {\
    if (m_##var != NULL && m_##var != f)\
    {\
      deallocateArrayData(m_##var);\
      m_##var = NULL;\
    }\
    m_##var = f;\
  }



/**
* @class CSVReader CSVReader.h CSVReader/CSVReader.h
* @brief This class is a self contained TSL OIM .ang file reader. It should only be dependent on
* standard C++ features
* @author Michael A. Jackson for BlueQuartz Software
* @date Mar 1, 2010
* @version 1.0
*/
class CSVGrainDataReader
{
  public:
    CSVGrainDataReader();
    virtual ~CSVGrainDataReader();


    /** @brief Sets the file name of the ang file to be read */
    void setFileName(const QString& f) { m_FileName = f; }
    QString getFileName() { return m_FileName;}

    void setNumberOfElements(size_t n) { m_NumberOfElements = n;}
    size_t getNumberOfElements() { return m_NumberOfElements;}

    void setNumFeatures(int n) { m_NumFeatures = n; }
    int getNumFeatures() { return m_NumFeatures; }

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

    SET_GET_POINTER(GrainId, GrainId, int)
    SET_GET_POINTER(Phi1, Phi1, float)
    SET_GET_POINTER(Phi, Phi, float)
    SET_GET_POINTER(Phi2, Phi2, float)
    SET_GET_POINTER(EquivDiam, EquivDiam, float)
    SET_GET_POINTER(B_Over_A, B_Over_A, float)
    SET_GET_POINTER(C_Over_A, C_Over_A, float)
    SET_GET_POINTER(Omega3, Omega3, float)
    SET_GET_POINTER(OutsideBoundingBox, OutsideBoundingBox, int)
    SET_GET_POINTER(NumNeighbors, NumNeighbors, int)
    SET_GET_POINTER(SurfaceGrain, SurfaceGrain, int)

  protected:


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
    void deallocateArrayData(T*& ptr)
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
    QString m_FileName;
    size_t m_NumberOfElements;
    int    m_NumFeatures;

    int*   m_GrainId;
    float* m_Phi1;
    float* m_Phi;
    float* m_Phi2;
    float* m_EquivDiam;
    float* m_B_Over_A;
    float* m_C_Over_A;
    float* m_Omega3;
    int* m_OutsideBoundingBox;
    int* m_NumNeighbors;
    int* m_SurfaceGrain;

    bool m_ManageMemory;  // We are going to forcibly manage the memory. There is currently NO option otherwise.
    bool m_headerComplete;

    /** @brief Parses the value from a single line of the header section of the TSL .ang file
    * @param line The line to parse
    */
    void parseHeaderLine(char* buf, size_t length);

    /** @brief Parses the data from a line of data from the TSL .ang file
    * @param line The line of data to parse
    */
    void readData(const QString& line, int row, size_t i);

    CSVGrainDataReader(const CSVGrainDataReader&);    // Copy Constructor Not Implemented
    void operator=(const CSVGrainDataReader&);  // Operator '=' Not Implemented

};


#endif /* CSVGRAINDATAREADER_H_ */
