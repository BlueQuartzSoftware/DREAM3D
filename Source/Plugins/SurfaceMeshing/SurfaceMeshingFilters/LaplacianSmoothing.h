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

#define OUTPUT_DEBUG_VTK_FILES 1

#include "SurfaceMeshing/SurfaceMeshingDLLExport.h"

/**
 * @brief The LaplacianSmoothing class. See [Filter documentation](@ref laplaciansmoothing) for details.
 */
class SurfaceMeshing_EXPORT LaplacianSmoothing : public SurfaceMeshFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(LaplacianSmoothing SUPERCLASS SurfaceMeshFilter)
  PYB11_SHARED_POINTERS(LaplacianSmoothing)
  PYB11_FILTER_NEW_MACRO(LaplacianSmoothing)
  PYB11_FILTER_PARAMETER(DataArrayPath, SurfaceDataContainerName)
  PYB11_FILTER_PARAMETER(DataArrayPath, SurfaceMeshNodeTypeArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
  PYB11_FILTER_PARAMETER(int, IterationSteps)
  PYB11_FILTER_PARAMETER(float, Lambda)
  PYB11_FILTER_PARAMETER(float, SurfacePointLambda)
  PYB11_FILTER_PARAMETER(float, TripleLineLambda)
  PYB11_FILTER_PARAMETER(float, QuadPointLambda)
  PYB11_FILTER_PARAMETER(float, SurfaceTripleLineLambda)
  PYB11_FILTER_PARAMETER(float, SurfaceQuadPointLambda)
  PYB11_FILTER_PARAMETER(bool, UseTaubinSmoothing)
  PYB11_FILTER_PARAMETER(float, MuFactor)
  PYB11_PROPERTY(DataArrayPath SurfaceDataContainerName READ getSurfaceDataContainerName WRITE setSurfaceDataContainerName)
  PYB11_PROPERTY(DataArrayPath SurfaceMeshNodeTypeArrayPath READ getSurfaceMeshNodeTypeArrayPath WRITE setSurfaceMeshNodeTypeArrayPath)
  PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)
  PYB11_PROPERTY(int IterationSteps READ getIterationSteps WRITE setIterationSteps)
  PYB11_PROPERTY(float Lambda READ getLambda WRITE setLambda)
  PYB11_PROPERTY(float SurfacePointLambda READ getSurfacePointLambda WRITE setSurfacePointLambda)
  PYB11_PROPERTY(float TripleLineLambda READ getTripleLineLambda WRITE setTripleLineLambda)
  PYB11_PROPERTY(float QuadPointLambda READ getQuadPointLambda WRITE setQuadPointLambda)
  PYB11_PROPERTY(float SurfaceTripleLineLambda READ getSurfaceTripleLineLambda WRITE setSurfaceTripleLineLambda)
  PYB11_PROPERTY(float SurfaceQuadPointLambda READ getSurfaceQuadPointLambda WRITE setSurfaceQuadPointLambda)
  PYB11_PROPERTY(bool UseTaubinSmoothing READ getUseTaubinSmoothing WRITE setUseTaubinSmoothing)
  PYB11_PROPERTY(float MuFactor READ getMuFactor WRITE setMuFactor)
#endif

public:
  using Self = LaplacianSmoothing;
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
   * @brief Returns the name of the class for LaplacianSmoothing
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for LaplacianSmoothing
   */
  static QString ClassName();

  ~LaplacianSmoothing() override;

  /**
   * @brief Setter property for SurfaceDataContainerName
   */
  void setSurfaceDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for SurfaceDataContainerName
   * @return Value of SurfaceDataContainerName
   */
  DataArrayPath getSurfaceDataContainerName() const;

  Q_PROPERTY(DataArrayPath SurfaceDataContainerName READ getSurfaceDataContainerName WRITE setSurfaceDataContainerName)

  /**
   * @brief Setter property for SurfaceMeshNodeTypeArrayPath
   */
  void setSurfaceMeshNodeTypeArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SurfaceMeshNodeTypeArrayPath
   * @return Value of SurfaceMeshNodeTypeArrayPath
   */
  DataArrayPath getSurfaceMeshNodeTypeArrayPath() const;

  Q_PROPERTY(DataArrayPath SurfaceMeshNodeTypeArrayPath READ getSurfaceMeshNodeTypeArrayPath WRITE setSurfaceMeshNodeTypeArrayPath)

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
   * @brief Setter property for IterationSteps
   */
  void setIterationSteps(int value);
  /**
   * @brief Getter property for IterationSteps
   * @return Value of IterationSteps
   */
  int getIterationSteps() const;

  Q_PROPERTY(int IterationSteps READ getIterationSteps WRITE setIterationSteps)

  /**
   * @brief Setter property for Lambda
   */
  void setLambda(float value);
  /**
   * @brief Getter property for Lambda
   * @return Value of Lambda
   */
  float getLambda() const;

  Q_PROPERTY(float Lambda READ getLambda WRITE setLambda)

  /**
   * @brief Setter property for SurfacePointLambda
   */
  void setSurfacePointLambda(float value);
  /**
   * @brief Getter property for SurfacePointLambda
   * @return Value of SurfacePointLambda
   */
  float getSurfacePointLambda() const;

  Q_PROPERTY(float SurfacePointLambda READ getSurfacePointLambda WRITE setSurfacePointLambda)

  /**
   * @brief Setter property for TripleLineLambda
   */
  void setTripleLineLambda(float value);
  /**
   * @brief Getter property for TripleLineLambda
   * @return Value of TripleLineLambda
   */
  float getTripleLineLambda() const;

  Q_PROPERTY(float TripleLineLambda READ getTripleLineLambda WRITE setTripleLineLambda)

  /**
   * @brief Setter property for QuadPointLambda
   */
  void setQuadPointLambda(float value);
  /**
   * @brief Getter property for QuadPointLambda
   * @return Value of QuadPointLambda
   */
  float getQuadPointLambda() const;

  Q_PROPERTY(float QuadPointLambda READ getQuadPointLambda WRITE setQuadPointLambda)

  /**
   * @brief Setter property for SurfaceTripleLineLambda
   */
  void setSurfaceTripleLineLambda(float value);
  /**
   * @brief Getter property for SurfaceTripleLineLambda
   * @return Value of SurfaceTripleLineLambda
   */
  float getSurfaceTripleLineLambda() const;

  Q_PROPERTY(float SurfaceTripleLineLambda READ getSurfaceTripleLineLambda WRITE setSurfaceTripleLineLambda)

  /**
   * @brief Setter property for SurfaceQuadPointLambda
   */
  void setSurfaceQuadPointLambda(float value);
  /**
   * @brief Getter property for SurfaceQuadPointLambda
   * @return Value of SurfaceQuadPointLambda
   */
  float getSurfaceQuadPointLambda() const;

  Q_PROPERTY(float SurfaceQuadPointLambda READ getSurfaceQuadPointLambda WRITE setSurfaceQuadPointLambda)

  /**
   * @brief Setter property for UseTaubinSmoothing
   */
  void setUseTaubinSmoothing(bool value);
  /**
   * @brief Getter property for UseTaubinSmoothing
   * @return Value of UseTaubinSmoothing
   */
  bool getUseTaubinSmoothing() const;

  Q_PROPERTY(bool UseTaubinSmoothing READ getUseTaubinSmoothing WRITE setUseTaubinSmoothing)

  /**
   * @brief Setter property for MuFactor
   */
  void setMuFactor(float value);
  /**
   * @brief Getter property for MuFactor
   * @return Value of MuFactor
   */
  float getMuFactor() const;

  Q_PROPERTY(float MuFactor READ getMuFactor WRITE setMuFactor)

  /* This class is designed to be subclassed so that thoes subclasses can add
   * more functionality such as constrained surface nodes or Triple Lines. We use
   * this array to assign each vertex a specific Lambda value. Subclasses can set
   * this array then simply call the 'smooth' protected method to actually run the
   * smoothing iterations
   */
  /**
   * @brief Setter property for LambdaArray
   */
  virtual void setLambdaArray(const DataArray<float>::Pointer& value);
  /**
   * @brief Getter property for LambdaArray
   * @return Value of LambdaArray
   */
  virtual DataArray<float>::Pointer getLambdaArray() const;

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
  LaplacianSmoothing();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief generateLambdaArray Generates the Lambda array that will be use during the smoothing
   * @return Integer error code
   */
  virtual int32_t generateLambdaArray();

  /**
   * @brief edgeBasedSmoothing Version of the smoothing algorithm uses Edge->Vertex connectivity information for its algorithm
   * @return Integer error code
   */
  virtual int32_t edgeBasedSmoothing();

private:
  DataArrayPath m_SurfaceDataContainerName = {};
  DataArrayPath m_SurfaceMeshNodeTypeArrayPath = {};
  DataArrayPath m_SurfaceMeshFaceLabelsArrayPath = {};
  int m_IterationSteps = {};
  float m_Lambda = {};
  float m_SurfacePointLambda = {};
  float m_TripleLineLambda = {};
  float m_QuadPointLambda = {};
  float m_SurfaceTripleLineLambda = {};
  float m_SurfaceQuadPointLambda = {};
  bool m_UseTaubinSmoothing = {};
  float m_MuFactor = {};
  DataArray<float>::Pointer m_LambdaArray = {};
  std::weak_ptr<DataArray<int8_t>> m_SurfaceMeshNodeTypePtr;
  int8_t* m_SurfaceMeshNodeType = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_SurfaceMeshFaceLabelsPtr;
  int32_t* m_SurfaceMeshFaceLabels = nullptr;

public:
  LaplacianSmoothing(const LaplacianSmoothing&) = delete;            // Copy Constructor Not Implemented
  LaplacianSmoothing(LaplacianSmoothing&&) = delete;                 // Move Constructor Not Implemented
  LaplacianSmoothing& operator=(const LaplacianSmoothing&) = delete; // Copy Assignment Not Implemented
  LaplacianSmoothing& operator=(LaplacianSmoothing&&) = delete;      // Move assignment Not Implemented
};

