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
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "Sampling/SamplingFilters/SampleSurfaceMesh.h"

#include "Sampling/SamplingDLLExport.h"

/**
 * @brief The SampleSurfaceMeshSpecifiedPoints class. See [Filter documentation](@ref samplesurfacemeshspecifiedpoints) for details.
 */
class Sampling_EXPORT SampleSurfaceMeshSpecifiedPoints : public SampleSurfaceMesh
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(SampleSurfaceMeshSpecifiedPoints SUPERCLASS SampleSurfaceMesh)
  PYB11_SHARED_POINTERS(SampleSurfaceMeshSpecifiedPoints)
  PYB11_FILTER_NEW_MACRO(SampleSurfaceMeshSpecifiedPoints)
  PYB11_FILTER_PARAMETER(QString, InputFilePath)
  PYB11_FILTER_PARAMETER(QString, OutputFilePath)
  PYB11_PROPERTY(QString InputFilePath READ getInputFilePath WRITE setInputFilePath)
  PYB11_PROPERTY(QString OutputFilePath READ getOutputFilePath WRITE setOutputFilePath)
#endif

public:
  using Self = SampleSurfaceMeshSpecifiedPoints;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<Self>;
  
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
   * @brief Returns the name of the class for SampleSurfaceMeshSpecifiedPoints
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for SampleSurfaceMeshSpecifiedPoints
   */
  static QString ClassName();

  virtual ~SampleSurfaceMeshSpecifiedPoints();

  /**
   * @brief Setter property for InputFilePath
   */
  void setInputFilePath(const QString& value);
  /**
   * @brief Getter property for InputFilePath
   * @return Value of InputFilePath
   */
  QString getInputFilePath() const;

  Q_PROPERTY(QString InputFilePath READ getInputFilePath WRITE setInputFilePath)

  /**
   * @brief Setter property for OutputFilePath
   */
  void setOutputFilePath(const QString& value);
  /**
   * @brief Getter property for OutputFilePath
   * @return Value of OutputFilePath
   */
  QString getOutputFilePath() const;

  Q_PROPERTY(QString OutputFilePath READ getOutputFilePath WRITE setOutputFilePath)

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
  SampleSurfaceMeshSpecifiedPoints();
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
  std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;

  QString m_InputFilePath = {};
  QString m_OutputFilePath = {};

  // number of specified points
  int64_t m_NumPoints;

  /**
   * @brief updateVertexInstancePointers Updates raw Vertex pointers
   */
  void updateVertexInstancePointers();

public:
  SampleSurfaceMeshSpecifiedPoints(const SampleSurfaceMeshSpecifiedPoints&) = delete; // Copy Constructor Not Implemented
  SampleSurfaceMeshSpecifiedPoints(SampleSurfaceMeshSpecifiedPoints&&) = delete;      // Move Constructor Not Implemented
  SampleSurfaceMeshSpecifiedPoints& operator=(const SampleSurfaceMeshSpecifiedPoints&) = delete; // Copy Assignment Not Implemented
  SampleSurfaceMeshSpecifiedPoints& operator=(SampleSurfaceMeshSpecifiedPoints&&) = delete;      // Move Assignment Not Implemented
};

