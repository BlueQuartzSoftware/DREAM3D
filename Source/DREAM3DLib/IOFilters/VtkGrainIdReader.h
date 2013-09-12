/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
 * All rights reserved.
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef VTKRECTILINEARGRIDREADER_H_
#define VTKRECTILINEARGRIDREADER_H_

#include <string.h> // needed for the ::memcpy function below
#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/FileReader.h"
#include "DREAM3DLib/Common/DataArray.hpp"


class DREAM3DLib_EXPORT VtkGrainIdReader : public FileReader
{
  public:
  DREAM3D_SHARED_POINTERS(VtkGrainIdReader)
  DREAM3D_STATIC_NEW_MACRO(VtkGrainIdReader)
  DREAM3D_TYPE_MACRO_SUPER(VtkGrainIdReader, FileReader)


    virtual ~VtkGrainIdReader();

    DREAM3D_INSTANCE_STRING_PROPERTY(InputFile)

    DREAM3D_INSTANCE_STRING_PROPERTY(Comment)
    DREAM3D_INSTANCE_STRING_PROPERTY(DatasetType)
    DREAM3D_INSTANCE_PROPERTY(bool, FileIsBinary)
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdScalarName)

    //------ Created Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)

    virtual const QString getGroupName() { return DREAM3D::FilterGroups::IOFilters; }
  virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::InputFilters; }
    virtual const QString getHumanLabel() { return "Read Vtk File (STRUCTURED_POINTS) Grain Ids Only"; }

    virtual void setupFilterParameters();

    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);
    
    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    virtual void preflight();

  protected:
    VtkGrainIdReader();

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);


    /**
     * @brief Reads the VTK header and sets the values that are described in the header
     * @return Error Condition. Negative is Error.
     */
    int readHeader();

    /**
     * @brief
     * @return Error Condition. Negative is Error.
     */
    virtual int readFile();

    /**
     *
     * @param input
     * @param value
     * @return Error Condition. Negative is Error.
     */
    int parseCoordinateLine(const char* input, size_t &value);

    /**
      * @brief Parses the byte size from a data set declaration line
      * @param text
      * @return
      */
     size_t parseByteSize(char text[256]);

     /**
      *
      */
     int ignoreData(std::ifstream &in, int byteSize, char* type, size_t xDim, size_t yDim, size_t zDim);

     /**
      *
      */
     template<typename T>
     int skipVolume(std::ifstream &inStream, int byteSize, size_t xDim, size_t yDim, size_t zDim, T &diff)
     {
       int err = 0;
       int64_t totalSize = (int64_t)xDim * (int64_t)yDim * (int64_t)zDim;
       if (getFileIsBinary() == true)
       {
         T* buffer = new T[totalSize];
         // Read all the xpoints in one shot into a buffer
         inStream.read(reinterpret_cast<char* > (buffer), (totalSize * sizeof(T)));
         if(inStream.gcount() != static_cast<std::streamsize>(totalSize * sizeof(T)))
         {
           qDebug() << " ERROR READING BINARY FILE. Bytes read was not the same as func->xDim *. " << byteSize << "." << inStream.gcount()
               << " vs " << (totalSize * sizeof(T)) << "\n";
           return -1;
         }
         if (totalSize > 1) {
           T t = buffer[totalSize-1];
           T t1 = buffer[totalSize-2];
           // Dont forget to byte swap since VTK Binary Files are explicitly Big Endian formatted
           DREAM3D::Endian::FromBigToSystem::convert<T>(t);
           DREAM3D::Endian::FromBigToSystem::convert<T>(t1);
           diff =t-t1;
         }
         else
         {
           diff = buffer[totalSize];
         }
         delete buffer;
       }
       else
       {
         T tmp;
         T t2;
         for (int64_t x = 0; x < totalSize; ++x)
         {
           if(x == 1)
           {
             t2 = tmp;
           }
           inStream >> tmp;
           if(x == 1)
           {
             diff = tmp - t2;
           }
         }
       }
       return err;
     }

     template<typename T>
     int skipVolume(std::ifstream &inStream, int byteSize, size_t xDim, size_t yDim, size_t zDim)
     {
       int err = 0;
       if(getFileIsBinary() == true)
       {
         int64_t totalSize = (int64_t)xDim * (int64_t)yDim * (int64_t)zDim;
         T* buffer = new T[totalSize];
         // Read all the xpoints in one shot into a buffer
         inStream.read(reinterpret_cast<char*>(buffer), (totalSize * sizeof(T)));
         if(inStream.gcount() != static_cast<std::streamsize>(totalSize * sizeof(T)))
         {
           qDebug() << " ERROR READING BINARY FILE. Bytes read was not the same as func->xDim *. " << byteSize << "." << inStream.gcount() << " vs "
               << (totalSize * sizeof(T)) << "\n";
           return -1;
         }
         delete buffer;
       }
       else
       {
         T tmp;
         for (size_t z = 0; z < zDim; ++z)
         {
           for (size_t y = 0; y < yDim; ++y)
           {
             for (size_t x = 0; x < xDim; ++x)
             {
               inStream >> tmp;
             }
           }
         }
       }
       return err;
     }



  private:
    int32_t* m_GrainIds;

    VtkGrainIdReader(const VtkGrainIdReader&); // Copy Constructor Not Implemented
    void operator=(const VtkGrainIdReader&); // Operator '=' Not Implemented
};

#endif /* VTKRECTILINEARGRIDREADER_H_ */
