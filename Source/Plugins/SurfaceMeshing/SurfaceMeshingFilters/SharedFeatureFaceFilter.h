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
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "SurfaceMeshing/SurfaceMeshingFilters/SurfaceMeshFilter.h"

#include "SurfaceMeshing/SurfaceMeshingDLLExport.h"

/**
 * @brief The SharedFeatureFaceFilter class. See [Filter documentation](@ref sharedfeaturefacefilter) for details.
 */
class SurfaceMeshing_EXPORT SharedFeatureFaceFilter : public SurfaceMeshFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(SharedFeatureFaceFilter SUPERCLASS SurfaceMeshFilter)
  PYB11_SHARED_POINTERS(SharedFeatureFaceFilter)
  PYB11_FILTER_NEW_MACRO(SharedFeatureFaceFilter)
  PYB11_FILTER_PARAMETER(QString, FaceFeatureAttributeMatrixName)
  PYB11_FILTER_PARAMETER(QString, SurfaceMeshFeatureFaceIdsArrayName)
  PYB11_FILTER_PARAMETER(QString, SurfaceMeshFeatureFaceLabelsArrayName)
  PYB11_FILTER_PARAMETER(QString, SurfaceMeshFeatureFaceNumTrianglesArrayName)
  PYB11_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
  PYB11_PROPERTY(QString FaceFeatureAttributeMatrixName READ getFaceFeatureAttributeMatrixName WRITE setFaceFeatureAttributeMatrixName)
  PYB11_PROPERTY(QString SurfaceMeshFeatureFaceIdsArrayName READ getSurfaceMeshFeatureFaceIdsArrayName WRITE setSurfaceMeshFeatureFaceIdsArrayName)
  PYB11_PROPERTY(QString SurfaceMeshFeatureFaceLabelsArrayName READ getSurfaceMeshFeatureFaceLabelsArrayName WRITE setSurfaceMeshFeatureFaceLabelsArrayName)
  PYB11_PROPERTY(QString SurfaceMeshFeatureFaceNumTrianglesArrayName READ getSurfaceMeshFeatureFaceNumTrianglesArrayName WRITE setSurfaceMeshFeatureFaceNumTrianglesArrayName)
  PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)
#endif

public:
  using Self = SharedFeatureFaceFilter;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  
  /**
   * @brief Returns a NullPointer wrapped by a shared_ptr<>
   * @return
   */
  static Pointer NullPointer();

  /**
   * @brief Creates a new object wrapped in a shared_ptr<>
   * @return
   */
  static Pointer New();

  /**
   * @brief Returns the name of the class for SharedFeatureFaceFilter
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for SharedFeatureFaceFilter
   */
  static QString ClassName();

  ~SharedFeatureFaceFilter() override;

  typedef QVector<int32_t> FaceIds_t;
  typedef QMap<int32_t, FaceIds_t> SharedFeatureFaces_t;

  /**
   * @brief Setter property for FaceFeatureAttributeMatrixName
   */
  void setFaceFeatureAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for FaceFeatureAttributeMatrixName
   * @return Value of FaceFeatureAttributeMatrixName
   */
  QString getFaceFeatureAttributeMatrixName() const;

  Q_PROPERTY(QString FaceFeatureAttributeMatrixName READ getFaceFeatureAttributeMatrixName WRITE setFaceFeatureAttributeMatrixName)

  /**
   * @brief Setter property for SurfaceMeshFeatureFaceIdsArrayName
   */
  void setSurfaceMeshFeatureFaceIdsArrayName(const QString& value);
  /**
   * @brief Getter property for SurfaceMeshFeatureFaceIdsArrayName
   * @return Value of SurfaceMeshFeatureFaceIdsArrayName
   */
  QString getSurfaceMeshFeatureFaceIdsArrayName() const;

  Q_PROPERTY(QString SurfaceMeshFeatureFaceIdsArrayName READ getSurfaceMeshFeatureFaceIdsArrayName WRITE setSurfaceMeshFeatureFaceIdsArrayName)

  /**
   * @brief Setter property for SurfaceMeshFeatureFaceLabelsArrayName
   */
  void setSurfaceMeshFeatureFaceLabelsArrayName(const QString& value);
  /**
   * @brief Getter property for SurfaceMeshFeatureFaceLabelsArrayName
   * @return Value of SurfaceMeshFeatureFaceLabelsArrayName
   */
  QString getSurfaceMeshFeatureFaceLabelsArrayName() const;

  Q_PROPERTY(QString SurfaceMeshFeatureFaceLabelsArrayName READ getSurfaceMeshFeatureFaceLabelsArrayName WRITE setSurfaceMeshFeatureFaceLabelsArrayName)

  /**
   * @brief Setter property for SurfaceMeshFeatureFaceNumTrianglesArrayName
   */
  void setSurfaceMeshFeatureFaceNumTrianglesArrayName(const QString& value);
  /**
   * @brief Getter property for SurfaceMeshFeatureFaceNumTrianglesArrayName
   * @return Value of SurfaceMeshFeatureFaceNumTrianglesArrayName
   */
  QString getSurfaceMeshFeatureFaceNumTrianglesArrayName() const;

  Q_PROPERTY(QString SurfaceMeshFeatureFaceNumTrianglesArrayName READ getSurfaceMeshFeatureFaceNumTrianglesArrayName WRITE setSurfaceMeshFeatureFaceNumTrianglesArrayName)

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

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
   */
  QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  QUuid getUuid() const override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  QString getHumanLabel() const override;

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
  SharedFeatureFaceFilter();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  std::weak_ptr<DataArray<int32_t>> m_SurfaceMeshFaceLabelsPtr;
  int32_t* m_SurfaceMeshFaceLabels = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_SurfaceMeshFeatureFaceIdsPtr;
  int32_t* m_SurfaceMeshFeatureFaceIds = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_SurfaceMeshFeatureFaceLabelsPtr;
  int32_t* m_SurfaceMeshFeatureFaceLabels = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_SurfaceMeshFeatureFaceNumTrianglesPtr;
  int32_t* m_SurfaceMeshFeatureFaceNumTriangles = nullptr;

  QString m_FaceFeatureAttributeMatrixName = {};
  QString m_SurfaceMeshFeatureFaceIdsArrayName = {};
  QString m_SurfaceMeshFeatureFaceLabelsArrayName = {};
  QString m_SurfaceMeshFeatureFaceNumTrianglesArrayName = {};
  DataArrayPath m_SurfaceMeshFaceLabelsArrayPath = {};

public:
  SharedFeatureFaceFilter(const SharedFeatureFaceFilter&) = delete;            // Copy Constructor Not Implemented
  SharedFeatureFaceFilter(SharedFeatureFaceFilter&&) = delete;                 // Move Constructor Not Implemented
  SharedFeatureFaceFilter& operator=(const SharedFeatureFaceFilter&) = delete; // Copy Assignment Not Implemented
  SharedFeatureFaceFilter& operator=(SharedFeatureFaceFilter&&) = delete;      //           // Move assignment Not Implemented
};

