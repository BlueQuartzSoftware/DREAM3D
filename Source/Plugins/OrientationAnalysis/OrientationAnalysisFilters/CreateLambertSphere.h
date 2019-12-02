/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#pragma once

#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Geometry/IGeometry.h"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

class QuadGeom;

/**
 * @brief The CreateLambertSphere fitler creates an equal area mesh on a unit
 * sphere.
 */
class OrientationAnalysis_EXPORT CreateLambertSphere : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(CreateLambertSphere SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(CreateLambertSphere)
  PYB11_FILTER_NEW_MACRO(CreateLambertSphere)
  PYB11_FILTER_PARAMETER(int, Hemisphere)
  PYB11_FILTER_PARAMETER(DataArrayPath, ImageDataArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, QuadDataContainerName)
  PYB11_FILTER_PARAMETER(DataArrayPath, TriangleDataContainerName)
  PYB11_FILTER_PARAMETER(DataArrayPath, EdgeDataContainerName)
  PYB11_FILTER_PARAMETER(DataArrayPath, VertexDataContainerName)
  PYB11_FILTER_PARAMETER(QString, VertexAttributeMatrixName)
  PYB11_FILTER_PARAMETER(QString, EdgeAttributeMatrixName)
  PYB11_FILTER_PARAMETER(QString, FaceAttributeMatrixName)
  PYB11_FILTER_PARAMETER(bool, CreateVertexGeometry)
  PYB11_FILTER_PARAMETER(bool, CreateEdgeGeometry)
  PYB11_FILTER_PARAMETER(bool, CreateTriangleGeometry)
  PYB11_FILTER_PARAMETER(bool, CreateQuadGeometry)
  PYB11_FILTER_PARAMETER(bool, UseExistingImage)
  PYB11_PROPERTY(int Hemisphere READ getHemisphere WRITE setHemisphere)
  PYB11_PROPERTY(DataArrayPath ImageDataArrayPath READ getImageDataArrayPath WRITE setImageDataArrayPath)
  PYB11_PROPERTY(DataArrayPath QuadDataContainerName READ getQuadDataContainerName WRITE setQuadDataContainerName)
  PYB11_PROPERTY(DataArrayPath TriangleDataContainerName READ getTriangleDataContainerName WRITE setTriangleDataContainerName)
  PYB11_PROPERTY(DataArrayPath EdgeDataContainerName READ getEdgeDataContainerName WRITE setEdgeDataContainerName)
  PYB11_PROPERTY(DataArrayPath VertexDataContainerName READ getVertexDataContainerName WRITE setVertexDataContainerName)
  PYB11_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)
  PYB11_PROPERTY(QString EdgeAttributeMatrixName READ getEdgeAttributeMatrixName WRITE setEdgeAttributeMatrixName)
  PYB11_PROPERTY(QString FaceAttributeMatrixName READ getFaceAttributeMatrixName WRITE setFaceAttributeMatrixName)
  //  PYB11_PROPERTY(QString ImageFaceDataArrayName READ getImageFaceDataArrayName WRITE setImageFaceDataArrayName)
  PYB11_PROPERTY(bool CreateVertexGeometry READ getCreateVertexGeometry WRITE setCreateVertexGeometry)
  PYB11_PROPERTY(bool CreateEdgeGeometry READ getCreateEdgeGeometry WRITE setCreateEdgeGeometry)
  PYB11_PROPERTY(bool CreateTriangleGeometry READ getCreateTriangleGeometry WRITE setCreateTriangleGeometry)
  PYB11_PROPERTY(bool CreateQuadGeometry READ getCreateQuadGeometry WRITE setCreateQuadGeometry)
#endif

public:
  using Self = CreateLambertSphere;
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
   * @brief Returns the name of the class for CreateLambertSphere
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for CreateLambertSphere
   */
  static QString ClassName();

  ~CreateLambertSphere() override;

  /**
   * @brief Setter property for Hemisphere
   */
  void setHemisphere(int value);
  /**
   * @brief Getter property for Hemisphere
   * @return Value of Hemisphere
   */
  int getHemisphere() const;

  Q_PROPERTY(int Hemisphere READ getHemisphere WRITE setHemisphere)

  /**
   * @brief Setter property for ImageDataArrayPath
   */
  void setImageDataArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for ImageDataArrayPath
   * @return Value of ImageDataArrayPath
   */
  DataArrayPath getImageDataArrayPath() const;

  Q_PROPERTY(DataArrayPath ImageDataArrayPath READ getImageDataArrayPath WRITE setImageDataArrayPath)

  /**
   * @brief Setter property for QuadDataContainerName
   */
  void setQuadDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for QuadDataContainerName
   * @return Value of QuadDataContainerName
   */
  DataArrayPath getQuadDataContainerName() const;

  Q_PROPERTY(DataArrayPath QuadDataContainerName READ getQuadDataContainerName WRITE setQuadDataContainerName)

  /**
   * @brief Setter property for TriangleDataContainerName
   */
  void setTriangleDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for TriangleDataContainerName
   * @return Value of TriangleDataContainerName
   */
  DataArrayPath getTriangleDataContainerName() const;

  Q_PROPERTY(DataArrayPath TriangleDataContainerName READ getTriangleDataContainerName WRITE setTriangleDataContainerName)

  /**
   * @brief Setter property for EdgeDataContainerName
   */
  void setEdgeDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for EdgeDataContainerName
   * @return Value of EdgeDataContainerName
   */
  DataArrayPath getEdgeDataContainerName() const;

  Q_PROPERTY(DataArrayPath EdgeDataContainerName READ getEdgeDataContainerName WRITE setEdgeDataContainerName)

  /**
   * @brief Setter property for VertexDataContainerName
   */
  void setVertexDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for VertexDataContainerName
   * @return Value of VertexDataContainerName
   */
  DataArrayPath getVertexDataContainerName() const;

  Q_PROPERTY(DataArrayPath VertexDataContainerName READ getVertexDataContainerName WRITE setVertexDataContainerName)

  /**
   * @brief Setter property for VertexAttributeMatrixName
   */
  void setVertexAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for VertexAttributeMatrixName
   * @return Value of VertexAttributeMatrixName
   */
  QString getVertexAttributeMatrixName() const;

  Q_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)

  /**
   * @brief Setter property for EdgeAttributeMatrixName
   */
  void setEdgeAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for EdgeAttributeMatrixName
   * @return Value of EdgeAttributeMatrixName
   */
  QString getEdgeAttributeMatrixName() const;

  Q_PROPERTY(QString EdgeAttributeMatrixName READ getEdgeAttributeMatrixName WRITE setEdgeAttributeMatrixName)

  /**
   * @brief Setter property for FaceAttributeMatrixName
   */
  void setFaceAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for FaceAttributeMatrixName
   * @return Value of FaceAttributeMatrixName
   */
  QString getFaceAttributeMatrixName() const;

  Q_PROPERTY(QString FaceAttributeMatrixName READ getFaceAttributeMatrixName WRITE setFaceAttributeMatrixName)

  //  SIMPL_FILTER_PARAMETER(QString, ImageFaceDataArrayName)
  //  Q_PROPERTY(QString ImageFaceDataArrayName READ getImageFaceDataArrayName WRITE setImageFaceDataArrayName)

  /**
   * @brief Setter property for CreateVertexGeometry
   */
  void setCreateVertexGeometry(bool value);
  /**
   * @brief Getter property for CreateVertexGeometry
   * @return Value of CreateVertexGeometry
   */
  bool getCreateVertexGeometry() const;

  Q_PROPERTY(bool CreateVertexGeometry READ getCreateVertexGeometry WRITE setCreateVertexGeometry)

  /**
   * @brief Setter property for CreateEdgeGeometry
   */
  void setCreateEdgeGeometry(bool value);
  /**
   * @brief Getter property for CreateEdgeGeometry
   * @return Value of CreateEdgeGeometry
   */
  bool getCreateEdgeGeometry() const;

  Q_PROPERTY(bool CreateEdgeGeometry READ getCreateEdgeGeometry WRITE setCreateEdgeGeometry)

  /**
   * @brief Setter property for CreateTriangleGeometry
   */
  void setCreateTriangleGeometry(bool value);
  /**
   * @brief Getter property for CreateTriangleGeometry
   * @return Value of CreateTriangleGeometry
   */
  bool getCreateTriangleGeometry() const;

  Q_PROPERTY(bool CreateTriangleGeometry READ getCreateTriangleGeometry WRITE setCreateTriangleGeometry)

  /**
   * @brief Setter property for CreateQuadGeometry
   */
  void setCreateQuadGeometry(bool value);
  /**
   * @brief Getter property for CreateQuadGeometry
   * @return Value of CreateQuadGeometry
   */
  bool getCreateQuadGeometry() const;

  Q_PROPERTY(bool CreateQuadGeometry READ getCreateQuadGeometry WRITE setCreateQuadGeometry)

  /**
   * @brief Setter property for UseExistingImage
   */
  void setUseExistingImage(bool value);
  /**
   * @brief Getter property for UseExistingImage
   * @return Value of UseExistingImage
   */
  bool getUseExistingImage() const;

  Q_PROPERTY(bool UseExistingImage READ getUseExistingImage WRITE setUseExistingImage)
  
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
  /**
   * @brief CreateLambertSphere
   */
  CreateLambertSphere();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief createVertices
   */
  void createVertices();

  /**
   * @brief CreateVertexGeometry
   */
  void createVertexGeometry();

  /**
   * @brief CreateEdgeGeometry
   */
  void createEdgeGeometry();

  /**
   * @brief CreateTriangleGeometry
   */
  void createTriangleGeometry();

  /**
   * @brief CreateQuadGeometry
   */
  void createQuadGeometry();

private:
  std::weak_ptr<DataArray<uint8_t>> m_ImageDataPtr;
  uint8_t* m_ImageData = nullptr;
  std::weak_ptr<DataArray<uint8_t>> m_EdgeDataPtr;
  uint8_t* m_EdgeData = nullptr;
  std::weak_ptr<DataArray<uint8_t>> m_TriangleFaceDataPtr;
  uint8_t* m_TriangleFaceData = nullptr;
  std::weak_ptr<DataArray<uint8_t>> m_QuadFaceDataPtr;
  uint8_t* m_QuadFaceData = nullptr;

  int m_Hemisphere = {};
  DataArrayPath m_ImageDataArrayPath = {};
  DataArrayPath m_QuadDataContainerName = {};
  DataArrayPath m_TriangleDataContainerName = {};
  DataArrayPath m_EdgeDataContainerName = {};
  DataArrayPath m_VertexDataContainerName = {};
  QString m_VertexAttributeMatrixName = {};
  QString m_EdgeAttributeMatrixName = {};
  QString m_FaceAttributeMatrixName = {};
  bool m_CreateVertexGeometry = {};
  bool m_CreateEdgeGeometry = {};
  bool m_CreateTriangleGeometry = {};
  bool m_CreateQuadGeometry = {};
  bool m_UseExistingImage = {};

  // DEFINE_DATAARRAY_VARIABLE(uint8_t, VertexData)

  std::shared_ptr<SharedVertexList> m_Vertices;

  QString m_VertexDataName;
  QString m_EdgeDataName;
  QString m_TriangleDataName;
  QString m_QuadDataName;

  /**
   * @brief Transforms the initial flat grid to a sphere using equations from
   * D. Rosca's paper.
   * @param verts The vertices to transform.
   */
  void transformFromLambertSquareToSphere(SharedVertexList* verts);

  /**
   * @brief Internal helper function
   * @param p The float to adjust.
   * @return
   */
  float cp(float p);

public:
  CreateLambertSphere(const CreateLambertSphere&) = delete; // Copy Constructor Not Implemented
  CreateLambertSphere(CreateLambertSphere&&) = delete;      // Move Constructor Not Implemented
  CreateLambertSphere& operator=(const CreateLambertSphere&) = delete; // Copy Assignment Not Implemented
  CreateLambertSphere& operator=(CreateLambertSphere&&) = delete;      // Move Assignment Not Implemented
};

