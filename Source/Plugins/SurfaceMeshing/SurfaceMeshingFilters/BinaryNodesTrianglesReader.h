/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#pragma once

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SurfaceMeshing/SurfaceMeshingFilters/SurfaceMeshFilter.h"


#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"

#include "SurfaceMeshing/SurfaceMeshingDLLExport.h"

/**
 * @class BinaryNodesTrianglesReader BinaryNodesTrianglesReader.h /SurfaceMeshingFilters/BinaryNodesTrianglesReader.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class SurfaceMeshing_EXPORT BinaryNodesTrianglesReader : public SurfaceMeshFilter
{
    Q_OBJECT
  public:
    SIMPL_SHARED_POINTERS(BinaryNodesTrianglesReader)
    SIMPL_FILTER_NEW_MACRO(BinaryNodesTrianglesReader)
     SIMPL_TYPE_MACRO_SUPER_OVERRIDE(BinaryNodesTrianglesReader, SurfaceMeshFilter)

     ~BinaryNodesTrianglesReader() override;

     SIMPL_INSTANCE_STRING_PROPERTY(SurfaceDataContainerName)
     SIMPL_INSTANCE_STRING_PROPERTY(VertexAttributeMatrixName)
     SIMPL_INSTANCE_STRING_PROPERTY(FaceAttributeMatrixName)

     SIMPL_INSTANCE_STRING_PROPERTY(FaceLabelsArrayName)
     SIMPL_INSTANCE_STRING_PROPERTY(SurfaceMeshNodeTypesArrayName)

     SIMPL_INSTANCE_STRING_PROPERTY(BinaryNodesFile)
     SIMPL_INSTANCE_STRING_PROPERTY(BinaryTrianglesFile)

     /**
      * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
      */
     void setupFilterParameters() override;

     /**
      * @brief readFilterParameters Reimplemented from @see AbstractFilter class
      */
     void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

     /**
      * @brief execute Reimplemented from @see AbstractFilter class
      */
     void execute() override;

     /**
      * @brief preflight Reimplemented from @see AbstractFilter class
      */
     void preflight() override;

   protected:
     BinaryNodesTrianglesReader();
     /**
      * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
      */
     void dataCheck();

     /**
      * @brief Initializes all the private instance variables.
      */
     void initialize();

     void updateVertexInstancePointers();

     void updateFaceInstancePointers();

     int read();

   private:
     DEFINE_DATAARRAY_VARIABLE(int8_t, SurfaceMeshNodeTypes)
     DEFINE_DATAARRAY_VARIABLE(int32_t, FaceLabels)

   public:
     BinaryNodesTrianglesReader(const BinaryNodesTrianglesReader&) = delete;            // Copy Constructor Not Implemented
     BinaryNodesTrianglesReader(BinaryNodesTrianglesReader&&) = delete;                 // Move Constructor Not Implemented
     BinaryNodesTrianglesReader& operator=(const BinaryNodesTrianglesReader&) = delete; // Copy Assignment Not Implemented
     BinaryNodesTrianglesReader& operator=(BinaryNodesTrianglesReader&&) = delete;      // Move Assignment
};

