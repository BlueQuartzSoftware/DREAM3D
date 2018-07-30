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

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "SurfaceMeshing/SurfaceMeshingDLLExport.h"

/**
 * @brief The FindTriangleGeomShapes class. See [Filter documentation](@ref findtrianglegeomshapes) for details.
 */
class SurfaceMeshing_EXPORT FindTriangleGeomShapes : public AbstractFilter
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(FindTriangleGeomShapes SUPERCLASS AbstractFilter)
    PYB11_PROPERTY(DataArrayPath FeatureAttributeMatrixName READ getFeatureAttributeMatrixName WRITE setFeatureAttributeMatrixName)
    PYB11_PROPERTY(DataArrayPath FaceLabelsArrayPath READ getFaceLabelsArrayPath WRITE setFaceLabelsArrayPath)
    PYB11_PROPERTY(DataArrayPath CentroidsArrayPath READ getCentroidsArrayPath WRITE setCentroidsArrayPath)
    PYB11_PROPERTY(DataArrayPath VolumesArrayPath READ getVolumesArrayPath WRITE setVolumesArrayPath)
    PYB11_PROPERTY(QString Omega3sArrayName READ getOmega3sArrayName WRITE setOmega3sArrayName)
    PYB11_PROPERTY(QString AxisLengthsArrayName READ getAxisLengthsArrayName WRITE setAxisLengthsArrayName)
    PYB11_PROPERTY(QString AxisEulerAnglesArrayName READ getAxisEulerAnglesArrayName WRITE setAxisEulerAnglesArrayName)
    PYB11_PROPERTY(QString AspectRatiosArrayName READ getAspectRatiosArrayName WRITE setAspectRatiosArrayName)
public:
  SIMPL_SHARED_POINTERS(FindTriangleGeomShapes)
  SIMPL_FILTER_NEW_MACRO(FindTriangleGeomShapes)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(FindTriangleGeomShapes, AbstractFilter)

  ~FindTriangleGeomShapes() override;
  SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureAttributeMatrixName)
  Q_PROPERTY(DataArrayPath FeatureAttributeMatrixName READ getFeatureAttributeMatrixName WRITE setFeatureAttributeMatrixName)

  SIMPL_DECLARE_ARRAY(double, featuremoments, FeatureMoments) // N x 6 Array

  SIMPL_DECLARE_ARRAY(double, featureeigenvals, FeatureEigenVals) // N x 3 Array

  SIMPL_FILTER_PARAMETER(DataArrayPath, FaceLabelsArrayPath)
  Q_PROPERTY(DataArrayPath FaceLabelsArrayPath READ getFaceLabelsArrayPath WRITE setFaceLabelsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, CentroidsArrayPath)
  Q_PROPERTY(DataArrayPath CentroidsArrayPath READ getCentroidsArrayPath WRITE setCentroidsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, VolumesArrayPath)
  Q_PROPERTY(DataArrayPath VolumesArrayPath READ getVolumesArrayPath WRITE setVolumesArrayPath)

  SIMPL_FILTER_PARAMETER(QString, Omega3sArrayName)
  Q_PROPERTY(QString Omega3sArrayName READ getOmega3sArrayName WRITE setOmega3sArrayName)

  SIMPL_FILTER_PARAMETER(QString, AxisLengthsArrayName)
  Q_PROPERTY(QString AxisLengthsArrayName READ getAxisLengthsArrayName WRITE setAxisLengthsArrayName)

  SIMPL_FILTER_PARAMETER(QString, AxisEulerAnglesArrayName)
  Q_PROPERTY(QString AxisEulerAnglesArrayName READ getAxisEulerAnglesArrayName WRITE setAxisEulerAnglesArrayName)

  SIMPL_FILTER_PARAMETER(QString, AspectRatiosArrayName)
  Q_PROPERTY(QString AspectRatiosArrayName READ getAspectRatiosArrayName WRITE setAspectRatiosArrayName)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  const QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
  */
  const QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  const QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  const QUuid getUuid() override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  const QString getHumanLabel() const override;

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
  FindTriangleGeomShapes();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief findTetrahedronInfo Creates a tetrahedron using the given vertex ids as the base
   * and the given centroid as the fourth element; the tetrahedron is then subdivided into
   * 8 smaller tetrahedra, and for each tetrahedron the volume is computed
   * @param vertIds Base triangle vertices
   * @param vertPtr Vertex coordinates pointer
   * @param centroid Fourth vertex (centroid of feature)
   * @param tetInfo Array to store information about subdivided tetrahedra
   */
  void findTetrahedronInfo(int64_t vertIds[3], float* vertPtr, float centroid[3], float tetInfo[32]);

  /**
   * @brief find_moments Determines the second order moments for each Feature
   */
  void find_moments();

  /**
   * @brief find_moments2D Determines the second order moments for each Feature (2D version)
   */
  void find_moments2D();

  /**
   * @brief find_axes Determine principal axis lengths for each Feature
   */
  void find_axes();

  /**
   * @brief find_axes2D Determine principal axis lengths for each Feature (2D version)
   */
  void find_axes2D();

  /**
   * @brief find_axiseulers Determine principal axis directions for each Feature
   */
  void find_axiseulers();

  /**
   * @brief find_axiseulers2D Determine principal axis directions for each Feature (2D version)
   */
  void find_axiseulers2D();

private:
  DEFINE_DATAARRAY_VARIABLE(int32_t, FaceLabels)
  DEFINE_DATAARRAY_VARIABLE(float, Centroids)
  DEFINE_DATAARRAY_VARIABLE(float, Volumes)
  DEFINE_DATAARRAY_VARIABLE(float, AxisEulerAngles)
  DEFINE_DATAARRAY_VARIABLE(float, AxisLengths)
  DEFINE_DATAARRAY_VARIABLE(float, Omega3s)
  DEFINE_DATAARRAY_VARIABLE(float, AspectRatios)

  double m_ScaleFactor;

public:
  FindTriangleGeomShapes(const FindTriangleGeomShapes&) = delete; // Copy Constructor Not Implemented
  FindTriangleGeomShapes(FindTriangleGeomShapes&&) = delete;      // Move Constructor Not Implemented
  FindTriangleGeomShapes& operator=(const FindTriangleGeomShapes&) = delete; // Copy Assignment Not Implemented
  FindTriangleGeomShapes& operator=(FindTriangleGeomShapes&&) = delete;      // Move Assignme
};

