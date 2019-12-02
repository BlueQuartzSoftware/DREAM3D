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


#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

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
    using Self = BinaryNodesTrianglesReader;
    using Pointer = std::shared_ptr<Self>;
    using ConstPointer = std::shared_ptr<const Self>;
    using WeakPointer = std::weak_ptr<Self>;
    using ConstWeakPointer = std::weak_ptr<const Self>;
    static Pointer NullPointer();

    static std::shared_ptr<BinaryNodesTrianglesReader> New();

    /**
     * @brief Returns the name of the class for BinaryNodesTrianglesReader
     */
    QString getNameOfClass() const override;
    /**
     * @brief Returns the name of the class for BinaryNodesTrianglesReader
     */
    static QString ClassName();

    ~BinaryNodesTrianglesReader() override;

    /**
     * @brief Setter property for SurfaceDataContainerName
     */
    void setSurfaceDataContainerName(const QString& value);
    /**
     * @brief Getter property for SurfaceDataContainerName
     * @return Value of SurfaceDataContainerName
     */
    QString getSurfaceDataContainerName() const;

    /**
     * @brief Setter property for VertexAttributeMatrixName
     */
    void setVertexAttributeMatrixName(const QString& value);
    /**
     * @brief Getter property for VertexAttributeMatrixName
     * @return Value of VertexAttributeMatrixName
     */
    QString getVertexAttributeMatrixName() const;

    /**
     * @brief Setter property for FaceAttributeMatrixName
     */
    void setFaceAttributeMatrixName(const QString& value);
    /**
     * @brief Getter property for FaceAttributeMatrixName
     * @return Value of FaceAttributeMatrixName
     */
    QString getFaceAttributeMatrixName() const;

    /**
     * @brief Setter property for FaceLabelsArrayName
     */
    void setFaceLabelsArrayName(const QString& value);
    /**
     * @brief Getter property for FaceLabelsArrayName
     * @return Value of FaceLabelsArrayName
     */
    QString getFaceLabelsArrayName() const;

    /**
     * @brief Setter property for SurfaceMeshNodeTypesArrayName
     */
    void setSurfaceMeshNodeTypesArrayName(const QString& value);
    /**
     * @brief Getter property for SurfaceMeshNodeTypesArrayName
     * @return Value of SurfaceMeshNodeTypesArrayName
     */
    QString getSurfaceMeshNodeTypesArrayName() const;

    /**
     * @brief Setter property for BinaryNodesFile
     */
    void setBinaryNodesFile(const QString& value);
    /**
     * @brief Getter property for BinaryNodesFile
     * @return Value of BinaryNodesFile
     */
    QString getBinaryNodesFile() const;

    /**
     * @brief Setter property for BinaryTrianglesFile
     */
    void setBinaryTrianglesFile(const QString& value);
    /**
     * @brief Getter property for BinaryTrianglesFile
     * @return Value of BinaryTrianglesFile
     */
    QString getBinaryTrianglesFile() const;

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
    std::weak_ptr<DataArray<int8_t>> m_SurfaceMeshNodeTypesPtr;
    int8_t* m_SurfaceMeshNodeTypes = nullptr;
    std::weak_ptr<DataArray<int32_t>> m_FaceLabelsPtr;
    int32_t* m_FaceLabels = nullptr;

    QString m_SurfaceDataContainerName = {};
    QString m_VertexAttributeMatrixName = {};
    QString m_FaceAttributeMatrixName = {};
    QString m_FaceLabelsArrayName = {};
    QString m_SurfaceMeshNodeTypesArrayName = {};
    QString m_BinaryNodesFile = {};
    QString m_BinaryTrianglesFile = {};

  public:
    BinaryNodesTrianglesReader(const BinaryNodesTrianglesReader&) = delete;            // Copy Constructor Not Implemented
    BinaryNodesTrianglesReader(BinaryNodesTrianglesReader&&) = delete;                 // Move Constructor Not Implemented
    BinaryNodesTrianglesReader& operator=(const BinaryNodesTrianglesReader&) = delete; // Copy Assignment Not Implemented
    BinaryNodesTrianglesReader& operator=(BinaryNodesTrianglesReader&&) = delete;      // Move Assignment Not Implemented
};

