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
#include "SIMPLib/Geometry/IGeometry.h"

#include "SurfaceMeshing/SurfaceMeshingDLLExport.h"

/**
 * @brief The FindTriangleGeomSizes class. See [Filter documentation](@ref findtrianglegeomsizes) for details.
 */
class SurfaceMeshing_EXPORT FindTriangleGeomSizes : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(FindTriangleGeomSizes SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(FindTriangleGeomSizes)
  PYB11_FILTER_NEW_MACRO(FindTriangleGeomSizes)
  PYB11_FILTER_PARAMETER(DataArrayPath, FaceLabelsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureAttributeMatrixName)
  PYB11_FILTER_PARAMETER(QString, VolumesArrayName)
  PYB11_PROPERTY(DataArrayPath FaceLabelsArrayPath READ getFaceLabelsArrayPath WRITE setFaceLabelsArrayPath)
  PYB11_PROPERTY(DataArrayPath FeatureAttributeMatrixName READ getFeatureAttributeMatrixName WRITE setFeatureAttributeMatrixName)
  PYB11_PROPERTY(QString VolumesArrayName READ getVolumesArrayName WRITE setVolumesArrayName)
#endif

public:
  using Self = FindTriangleGeomSizes;
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
   * @brief Returns the name of the class for FindTriangleGeomSizes
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FindTriangleGeomSizes
   */
  static QString ClassName();

  ~FindTriangleGeomSizes() override;

  /**
   * @brief Setter property for FaceLabelsArrayPath
   */
  void setFaceLabelsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FaceLabelsArrayPath
   * @return Value of FaceLabelsArrayPath
   */
  DataArrayPath getFaceLabelsArrayPath() const;

  Q_PROPERTY(DataArrayPath FaceLabelsArrayPath READ getFaceLabelsArrayPath WRITE setFaceLabelsArrayPath)

  /**
   * @brief Setter property for FeatureAttributeMatrixName
   */
  void setFeatureAttributeMatrixName(const DataArrayPath& value);
  /**
   * @brief Getter property for FeatureAttributeMatrixName
   * @return Value of FeatureAttributeMatrixName
   */
  DataArrayPath getFeatureAttributeMatrixName() const;

  Q_PROPERTY(DataArrayPath FeatureAttributeMatrixName READ getFeatureAttributeMatrixName WRITE setFeatureAttributeMatrixName)

  /**
   * @brief Setter property for VolumesArrayName
   */
  void setVolumesArrayName(const QString& value);
  /**
   * @brief Getter property for VolumesArrayName
   * @return Value of VolumesArrayName
   */
  QString getVolumesArrayName() const;

  Q_PROPERTY(QString VolumesArrayName READ getVolumesArrayName WRITE setVolumesArrayName)

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
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

  /**
  * @brief preflight Reimplemented from @see AbstractFilter class
  */
  void preflight() override;

signals:
  /**
   * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
   * be pushed from a user-facing control (such as a widget)
   * @param filter Filter instance pointer
   */
  void updateFilterParameters(AbstractFilter* filter);

  /**
   * @brief parametersChanged Emitted when any Filter parameter is changed internally
   */
  void parametersChanged();

  /**
   * @brief preflightAboutToExecute Emitted just before calling dataCheck()
   */
  void preflightAboutToExecute();

  /**
   * @brief preflightExecuted Emitted just after calling dataCheck()
   */
  void preflightExecuted();

protected:
  FindTriangleGeomSizes();

  /**
   * @brief findTetrahedronVolume Computes the signed volume of the tetrahedron where the
   * base of the tetrahedron is given by triangle defined by the vertIds indices and a
   * fourth point set to the origin
   * @param vertIds Indices of triangle vertices for the base of the tetrahedron
   * @param vertPtr Vertex coordinates for triangle base
   * @return float Signed volume of tetrahedron
   */
  float findTetrahedronVolume(MeshIndexType vertIds[3], float* vertPtr);

  /**
  * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
  */
  void dataCheck();

  /**
  * @brief Initializes all the private instance variables.
  */
  void initialize();

private:
  std::weak_ptr<DataArray<int32_t>> m_FaceLabelsPtr;
  int32_t* m_FaceLabels = nullptr;
  std::weak_ptr<DataArray<float>> m_VolumesPtr;
  float* m_Volumes = nullptr;

  DataArrayPath m_FaceLabelsArrayPath = {};
  DataArrayPath m_FeatureAttributeMatrixName = {};
  QString m_VolumesArrayName = {};

public:
  FindTriangleGeomSizes(const FindTriangleGeomSizes&) = delete; // Copy Constructor Not Implemented
  FindTriangleGeomSizes(FindTriangleGeomSizes&&) = delete;      // Move Constructor Not Implemented
  FindTriangleGeomSizes& operator=(const FindTriangleGeomSizes&) = delete; // Copy Assignment Not Implemented
  FindTriangleGeomSizes& operator=(FindTriangleGeomSizes&&) = delete;      // Move assignment Not Implemented
};

