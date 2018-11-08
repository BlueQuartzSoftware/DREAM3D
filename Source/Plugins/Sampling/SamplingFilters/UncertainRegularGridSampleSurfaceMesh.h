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
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/SIMPLib.h"

#include "Sampling/SamplingFilters/SampleSurfaceMesh.h"

#include "Sampling/SamplingDLLExport.h"

/**
 * @brief The UncertainRegularGridSampleSurfaceMesh class. See [Filter documentation](@ref uncertainregulargridsamplesurfacemesh) for details.
 */
class Sampling_EXPORT UncertainRegularGridSampleSurfaceMesh : public SampleSurfaceMesh
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(UncertainRegularGridSampleSurfaceMesh SUPERCLASS SampleSurfaceMesh)
    PYB11_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)
    PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
    PYB11_PROPERTY(int XPoints READ getXPoints WRITE setXPoints)
    PYB11_PROPERTY(int YPoints READ getYPoints WRITE setYPoints)
    PYB11_PROPERTY(int ZPoints READ getZPoints WRITE setZPoints)
    PYB11_PROPERTY(FloatVec3_t Resolution READ getResolution WRITE setResolution)
    PYB11_PROPERTY(FloatVec3_t Origin READ getOrigin WRITE setOrigin)
    PYB11_PROPERTY(FloatVec3_t Uncertainty READ getUncertainty WRITE setUncertainty)
    PYB11_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)
public:
  SIMPL_SHARED_POINTERS(UncertainRegularGridSampleSurfaceMesh)
  SIMPL_FILTER_NEW_MACRO(UncertainRegularGridSampleSurfaceMesh)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(UncertainRegularGridSampleSurfaceMesh, AbstractFilter)

  virtual ~UncertainRegularGridSampleSurfaceMesh();

  SIMPL_FILTER_PARAMETER(QString, DataContainerName)
  Q_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)

  SIMPL_FILTER_PARAMETER(QString, CellAttributeMatrixName)
  Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

  SIMPL_FILTER_PARAMETER(int, XPoints)
  Q_PROPERTY(int XPoints READ getXPoints WRITE setXPoints)

  SIMPL_FILTER_PARAMETER(int, YPoints)
  Q_PROPERTY(int YPoints READ getYPoints WRITE setYPoints)

  SIMPL_FILTER_PARAMETER(int, ZPoints)
  Q_PROPERTY(int ZPoints READ getZPoints WRITE setZPoints)

  SIMPL_FILTER_PARAMETER(FloatVec3_t, Resolution)
  Q_PROPERTY(FloatVec3_t Resolution READ getResolution WRITE setResolution)

  SIMPL_FILTER_PARAMETER(FloatVec3_t, Origin)
  Q_PROPERTY(FloatVec3_t Origin READ getOrigin WRITE setOrigin)

  SIMPL_FILTER_PARAMETER(FloatVec3_t, Uncertainty)
  Q_PROPERTY(FloatVec3_t Uncertainty READ getUncertainty WRITE setUncertainty)

  SIMPL_FILTER_PARAMETER(QString, FeatureIdsArrayName)
  Q_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

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

protected:
  UncertainRegularGridSampleSurfaceMesh();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief generate_points Reimplemented from @see SampleSurfaceMesh class
   * @return VertexGeom object
   */
  virtual VertexGeom::Pointer generate_points();

  /**
   * @brief assign_points Reimplemented from @see SampleSurfaceMesh class
   * @param iArray Sampled Feature Ids from superclass
   */
  virtual void assign_points(Int32ArrayType::Pointer iArray);

private:
  DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)

public:
  UncertainRegularGridSampleSurfaceMesh(const UncertainRegularGridSampleSurfaceMesh&) = delete; // Copy Constructor Not Implemented
  UncertainRegularGridSampleSurfaceMesh(UncertainRegularGridSampleSurfaceMesh&&) = delete;      // Move Constructor Not Implemented
  UncertainRegularGridSampleSurfaceMesh& operator=(const UncertainRegularGridSampleSurfaceMesh&) = delete; // Copy Assignment Not Implemented
  UncertainRegularGridSampleSurfaceMesh& operator=(UncertainRegularGridSampleSurfaceMesh&&) = delete;      // Move Assignment Not Implemented
};

