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
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The FindGBCD class. See [Filter documentation](@ref findgbcd) for details.
 */
class OrientationAnalysis_EXPORT FindGBCD : public AbstractFilter
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(FindGBCD SUPERCLASS AbstractFilter)
  PYB11_PROPERTY(QString FaceEnsembleAttributeMatrixName READ getFaceEnsembleAttributeMatrixName WRITE setFaceEnsembleAttributeMatrixName)
  PYB11_PROPERTY(float GBCDRes READ getGBCDRes WRITE setGBCDRes)
  PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)
  PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceNormalsArrayPath READ getSurfaceMeshFaceNormalsArrayPath WRITE setSurfaceMeshFaceNormalsArrayPath)
  PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceAreasArrayPath READ getSurfaceMeshFaceAreasArrayPath WRITE setSurfaceMeshFaceAreasArrayPath)
  PYB11_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)
  PYB11_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
  PYB11_PROPERTY(QString GBCDArrayName READ getGBCDArrayName WRITE setGBCDArrayName)
public:
  SIMPL_SHARED_POINTERS(FindGBCD)
  SIMPL_FILTER_NEW_MACRO(FindGBCD)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(FindGBCD, AbstractFilter)

  ~FindGBCD() override;

  SIMPL_FILTER_PARAMETER(QString, FaceEnsembleAttributeMatrixName)
  Q_PROPERTY(QString FaceEnsembleAttributeMatrixName READ getFaceEnsembleAttributeMatrixName WRITE setFaceEnsembleAttributeMatrixName)

  SIMPL_FILTER_PARAMETER(float, GBCDRes)
  Q_PROPERTY(float GBCDRes READ getGBCDRes WRITE setGBCDRes)

  SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
  Q_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceNormalsArrayPath)
  Q_PROPERTY(DataArrayPath SurfaceMeshFaceNormalsArrayPath READ getSurfaceMeshFaceNormalsArrayPath WRITE setSurfaceMeshFaceNormalsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceAreasArrayPath)
  Q_PROPERTY(DataArrayPath SurfaceMeshFaceAreasArrayPath READ getSurfaceMeshFaceAreasArrayPath WRITE setSurfaceMeshFaceAreasArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureEulerAnglesArrayPath)
  Q_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
  Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

  SIMPL_FILTER_PARAMETER(QString, GBCDArrayName)
  Q_PROPERTY(QString GBCDArrayName READ getGBCDArrayName WRITE setGBCDArrayName)

  SIMPL_INSTANCE_PROPERTY(QVector<ComparisonInput_t>, GBCDArrayNames)

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
  FindGBCD();

  /**
   * @brief dataCheckSurfaceMesh Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheckSurfaceMesh();

  /**
   * @brief dataCheckVoxel Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheckVoxel();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief sizeGBCD Determines the sizing for the GBCD arrays
   * @param faceChunkSize Number of triangles per chunk
   * @param numMisoReps Dimensionality for bins
   */
  void sizeGBCD(size_t faceChunkSize, size_t numMisoReps);

private:
  DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshFaceAreas)
  DEFINE_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFaceLabels)
  DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshFaceNormals)
  DEFINE_DATAARRAY_VARIABLE(float, FeatureEulerAngles)
  DEFINE_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
  DEFINE_DATAARRAY_VARIABLE(unsigned int, CrystalStructures)
  DEFINE_DATAARRAY_VARIABLE(double, GBCD)

  FloatArrayType::Pointer m_GbcdDeltasArray;
  Int32ArrayType::Pointer m_GbcdSizesArray;
  FloatArrayType::Pointer m_GbcdLimitsArray;
  Int32ArrayType::Pointer m_GbcdBinsArray;
  BoolArrayType::Pointer m_GbcdHemiCheckArray;

  float* m_GbcdDeltas;
  int32_t* m_GbcdSizes;
  float* m_GbcdLimits;
  int32_t* m_GbcdBins;
  bool* m_HemiCheck;

public:
  FindGBCD(const FindGBCD&) = delete;            // Copy Constructor Not Implemented
  FindGBCD(FindGBCD&&) = delete;                 // Move Constructor Not Implemented
  FindGBCD& operator=(const FindGBCD&) = delete; // Copy Assignment Not Implemented
  FindGBCD& operator=(FindGBCD&&) = delete;      // Move Assignment Not Implemented
};

