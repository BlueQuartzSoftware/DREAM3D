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

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "SurfaceMeshing/SurfaceMeshingFilters/SurfaceMeshFilter.h"
#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"

#include "SurfaceMeshing/SurfaceMeshingDLLExport.h"

/**
 * @class VerifyTriangleWinding VerifyTriangleWinding.h /SurfaceMeshFilters/VerifyTriangleWinding.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class SurfaceMeshing_EXPORT VerifyTriangleWinding : public SurfaceMeshFilter
{
    Q_OBJECT
  //  PYB11_CREATE_BINDINGS(VerifyTriangleWinding SUPERCLASS SurfaceMeshFilter)
  //  PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)
  public:
    using Self = VerifyTriangleWinding;
    using Pointer = std::shared_ptr<Self>;
    using ConstPointer = std::shared_ptr<const Self>;
    using WeakPointer = std::weak_ptr<Self>;
    using ConstWeakPointer = std::weak_ptr<const Self>;
    static Pointer NullPointer();

    static std::shared_ptr<VerifyTriangleWinding> New();

    /**
     * @brief Returns the name of the class for VerifyTriangleWinding
     */
    QString getNameOfClass() const override;
    /**
     * @brief Returns the name of the class for VerifyTriangleWinding
     */
    static QString ClassName();

    ~VerifyTriangleWinding() override;
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
     * @brief Setter property for SurfaceMeshNodeFacesArrayName
     */
    void setSurfaceMeshNodeFacesArrayName(const QString& value);
    /**
     * @brief Getter property for SurfaceMeshNodeFacesArrayName
     * @return Value of SurfaceMeshNodeFacesArrayName
     */
    QString getSurfaceMeshNodeFacesArrayName() const;

    typedef QMap<int, QSet<int> >                      LabelFaceMap_t;
    typedef QVector<int32_t>                               FaceList_t;

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    /**
     * @brief Setter property for SurfaceMeshFaceLabelsArrayPath
     */
    void setSurfaceMeshFaceLabelsArrayPath(const DataArrayPath& value);
    /**
     * @brief Getter property for SurfaceMeshFaceLabelsArrayPath
     * @return Value of SurfaceMeshFaceLabelsArrayPath
     */
    DataArrayPath getSurfaceMeshFaceLabelsArrayPath() const;

    Q_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)

    QString getCompiledLibraryName() const override;
    AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;
    QString getGroupName() const override;
    QString getSubGroupName() const override;

    /**
     * @brief getUuid Return the unique identifier for this filter.
     * @return A QUuid object.
     */
    QUuid getUuid() const override;

    /**
     * @brief This returns a string that is displayed in the GUI. It should be readable
     * and understandable by humans.
     */
    QString getHumanLabel() const override;

    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
    */
    void setupFilterParameters() override;

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

    /**
    * @brief Reimplemented from @see AbstractFilter class
    */
    void execute() override;

    /**
    * @brief This function runs some sanity checks on the DataContainer and inputs
    * in an attempt to ensure the filter can process the inputs.
    */
    void preflight() override;

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    VerifyTriangleWinding();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();


    /**
     * @brief This method generates a mapping between a "Label" (Feature Id) and the set of triangles that belong to that
     * lable value.
     * @param trianglesToLabelMap
     */
    void getLabelTriangelMap(LabelFaceMap_t& trianglesToLabelMap);

    /**
     * @brief This method verifies the winding of all the triangles and makes them consistent
     * @return
     */
    int verifyTriangleWinding();

    /**
     * @brief VerifyTriangleWinding::getSeedTriangle
     * @param label
     * @param triangleIndices
     * @return
     */
    int32_t getSeedTriangle(int32_t label, QSet<int32_t>& triangleIndices);

  private:
    std::weak_ptr<DataArray<int32_t>> m_SurfaceMeshFaceLabelsPtr;
    int32_t* m_SurfaceMeshFaceLabels = nullptr;

    QString m_SurfaceDataContainerName = {};
    QString m_SurfaceMeshNodeFacesArrayName = {};
    DataArrayPath m_SurfaceMeshFaceLabelsArrayPath = {};

    bool m_DoNodeFaceConnectivityFilter;

  public:
    VerifyTriangleWinding(const VerifyTriangleWinding&) = delete; // Copy Constructor Not Implemented
    VerifyTriangleWinding(VerifyTriangleWinding&&) = delete;      // Move Constructor Not Implemented
    VerifyTriangleWinding& operator=(const VerifyTriangleWinding&) = delete; // Copy Assignment Not Implemented
    VerifyTriangleWinding& operator=(VerifyTriangleWinding&&) = delete;      // Move Assignment Not Implemented
};




