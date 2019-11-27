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

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "ImportExport/ImportExportDLLExport.h"

/**
 * @class GoldfeatherReader GoldfeatherReader.h /IOFilters/GoldfeatherReader.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class ImportExport_EXPORT GoldfeatherReader : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(GoldfeatherReader SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(GoldfeatherReader)
  PYB11_FILTER_NEW_MACRO(GoldfeatherReader)
  PYB11_FILTER_PARAMETER(DataArrayPath, SurfaceDataContainerName)
  PYB11_FILTER_PARAMETER(QString, VertexAttributeMatrixName)
  PYB11_FILTER_PARAMETER(QString, FaceAttributeMatrixName)
  PYB11_FILTER_PARAMETER(QString, SurfaceMeshPrincipalCurvature1sArrayName)
  PYB11_FILTER_PARAMETER(QString, SurfaceMeshPrincipalCurvature2sArrayName)
  PYB11_FILTER_PARAMETER(QString, SurfaceMeshPrincipalDirection1sArrayName)
  PYB11_FILTER_PARAMETER(QString, SurfaceMeshPrincipalDirection2sArrayName)
  PYB11_FILTER_PARAMETER(QString, SurfaceMeshNodeNormalsArrayName)
  PYB11_FILTER_PARAMETER(QString, SurfaceMeshFaceLabelsArrayName)
  PYB11_FILTER_PARAMETER(QString, SurfaceMeshFaceNormalsArrayName)
  PYB11_FILTER_PARAMETER(QString, InputFile)
  PYB11_PROPERTY(DataArrayPath SurfaceDataContainerName READ getSurfaceDataContainerName WRITE setSurfaceDataContainerName)
  PYB11_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)
  PYB11_PROPERTY(QString FaceAttributeMatrixName READ getFaceAttributeMatrixName WRITE setFaceAttributeMatrixName)
  PYB11_PROPERTY(QString SurfaceMeshPrincipalCurvature1sArrayName READ getSurfaceMeshPrincipalCurvature1sArrayName WRITE setSurfaceMeshPrincipalCurvature1sArrayName)
  PYB11_PROPERTY(QString SurfaceMeshPrincipalCurvature2sArrayName READ getSurfaceMeshPrincipalCurvature2sArrayName WRITE setSurfaceMeshPrincipalCurvature2sArrayName)
  PYB11_PROPERTY(QString SurfaceMeshPrincipalDirection1sArrayName READ getSurfaceMeshPrincipalDirection1sArrayName WRITE setSurfaceMeshPrincipalDirection1sArrayName)
  PYB11_PROPERTY(QString SurfaceMeshPrincipalDirection2sArrayName READ getSurfaceMeshPrincipalDirection2sArrayName WRITE setSurfaceMeshPrincipalDirection2sArrayName)
  PYB11_PROPERTY(QString SurfaceMeshNodeNormalsArrayName READ getSurfaceMeshNodeNormalsArrayName WRITE setSurfaceMeshNodeNormalsArrayName)
  PYB11_PROPERTY(QString SurfaceMeshFaceLabelsArrayName READ getSurfaceMeshFaceLabelsArrayName WRITE setSurfaceMeshFaceLabelsArrayName)
  PYB11_PROPERTY(QString SurfaceMeshFaceNormalsArrayName READ getSurfaceMeshFaceNormalsArrayName WRITE setSurfaceMeshFaceNormalsArrayName)
  PYB11_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
#endif

public:
  using Self = GoldfeatherReader;
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
   * @brief Returns the name of the class for GoldfeatherReader
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for GoldfeatherReader
   */
  static QString ClassName();

  ~GoldfeatherReader() override;

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
   * @brief Setter property for FaceAttributeMatrixName
   */
  void setFaceAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for FaceAttributeMatrixName
   * @return Value of FaceAttributeMatrixName
   */
  QString getFaceAttributeMatrixName() const;

  Q_PROPERTY(QString FaceAttributeMatrixName READ getFaceAttributeMatrixName WRITE setFaceAttributeMatrixName)

  /**
   * @brief Setter property for SurfaceMeshPrincipalCurvature1sArrayName
   */
  void setSurfaceMeshPrincipalCurvature1sArrayName(const QString& value);
  /**
   * @brief Getter property for SurfaceMeshPrincipalCurvature1sArrayName
   * @return Value of SurfaceMeshPrincipalCurvature1sArrayName
   */
  QString getSurfaceMeshPrincipalCurvature1sArrayName() const;

  Q_PROPERTY(QString SurfaceMeshPrincipalCurvature1sArrayName READ getSurfaceMeshPrincipalCurvature1sArrayName WRITE setSurfaceMeshPrincipalCurvature1sArrayName)
  /**
   * @brief Setter property for SurfaceMeshPrincipalCurvature2sArrayName
   */
  void setSurfaceMeshPrincipalCurvature2sArrayName(const QString& value);
  /**
   * @brief Getter property for SurfaceMeshPrincipalCurvature2sArrayName
   * @return Value of SurfaceMeshPrincipalCurvature2sArrayName
   */
  QString getSurfaceMeshPrincipalCurvature2sArrayName() const;

  Q_PROPERTY(QString SurfaceMeshPrincipalCurvature2sArrayName READ getSurfaceMeshPrincipalCurvature2sArrayName WRITE setSurfaceMeshPrincipalCurvature2sArrayName)
  /**
   * @brief Setter property for SurfaceMeshPrincipalDirection1sArrayName
   */
  void setSurfaceMeshPrincipalDirection1sArrayName(const QString& value);
  /**
   * @brief Getter property for SurfaceMeshPrincipalDirection1sArrayName
   * @return Value of SurfaceMeshPrincipalDirection1sArrayName
   */
  QString getSurfaceMeshPrincipalDirection1sArrayName() const;

  Q_PROPERTY(QString SurfaceMeshPrincipalDirection1sArrayName READ getSurfaceMeshPrincipalDirection1sArrayName WRITE setSurfaceMeshPrincipalDirection1sArrayName)
  /**
   * @brief Setter property for SurfaceMeshPrincipalDirection2sArrayName
   */
  void setSurfaceMeshPrincipalDirection2sArrayName(const QString& value);
  /**
   * @brief Getter property for SurfaceMeshPrincipalDirection2sArrayName
   * @return Value of SurfaceMeshPrincipalDirection2sArrayName
   */
  QString getSurfaceMeshPrincipalDirection2sArrayName() const;

  Q_PROPERTY(QString SurfaceMeshPrincipalDirection2sArrayName READ getSurfaceMeshPrincipalDirection2sArrayName WRITE setSurfaceMeshPrincipalDirection2sArrayName)
  /**
   * @brief Setter property for SurfaceMeshNodeNormalsArrayName
   */
  void setSurfaceMeshNodeNormalsArrayName(const QString& value);
  /**
   * @brief Getter property for SurfaceMeshNodeNormalsArrayName
   * @return Value of SurfaceMeshNodeNormalsArrayName
   */
  QString getSurfaceMeshNodeNormalsArrayName() const;

  Q_PROPERTY(QString SurfaceMeshNodeNormalsArrayName READ getSurfaceMeshNodeNormalsArrayName WRITE setSurfaceMeshNodeNormalsArrayName)
  /**
   * @brief Setter property for SurfaceMeshFaceLabelsArrayName
   */
  void setSurfaceMeshFaceLabelsArrayName(const QString& value);
  /**
   * @brief Getter property for SurfaceMeshFaceLabelsArrayName
   * @return Value of SurfaceMeshFaceLabelsArrayName
   */
  QString getSurfaceMeshFaceLabelsArrayName() const;

  Q_PROPERTY(QString SurfaceMeshFaceLabelsArrayName READ getSurfaceMeshFaceLabelsArrayName WRITE setSurfaceMeshFaceLabelsArrayName)
  /**
   * @brief Setter property for SurfaceMeshFaceNormalsArrayName
   */
  void setSurfaceMeshFaceNormalsArrayName(const QString& value);
  /**
   * @brief Getter property for SurfaceMeshFaceNormalsArrayName
   * @return Value of SurfaceMeshFaceNormalsArrayName
   */
  QString getSurfaceMeshFaceNormalsArrayName() const;

  Q_PROPERTY(QString SurfaceMeshFaceNormalsArrayName READ getSurfaceMeshFaceNormalsArrayName WRITE setSurfaceMeshFaceNormalsArrayName)

  /**
   * @brief Setter property for InputFile
   */
  void setInputFile(const QString& value);
  /**
   * @brief Getter property for InputFile
   * @return Value of InputFile
   */
  QString getInputFile() const;

  Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

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
  GoldfeatherReader();
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

private:
  std::weak_ptr<DataArray<double>> m_SurfaceMeshPrincipalCurvature1sPtr;
  double* m_SurfaceMeshPrincipalCurvature1s = nullptr;
  std::weak_ptr<DataArray<double>> m_SurfaceMeshPrincipalCurvature2sPtr;
  double* m_SurfaceMeshPrincipalCurvature2s = nullptr;
  std::weak_ptr<DataArray<double>> m_SurfaceMeshPrincipalDirection1sPtr;
  double* m_SurfaceMeshPrincipalDirection1s = nullptr;
  std::weak_ptr<DataArray<double>> m_SurfaceMeshPrincipalDirection2sPtr;
  double* m_SurfaceMeshPrincipalDirection2s = nullptr;
  std::weak_ptr<DataArray<double>> m_SurfaceMeshNodeNormalsPtr;
  double* m_SurfaceMeshNodeNormals = nullptr;
  std::weak_ptr<DataArray<double>> m_SurfaceMeshFaceNormalsPtr;
  double* m_SurfaceMeshFaceNormals = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_SurfaceMeshFaceLabelsPtr;
  int32_t* m_SurfaceMeshFaceLabels = nullptr;

  DataArrayPath m_SurfaceDataContainerName = {};
  QString m_VertexAttributeMatrixName = {};
  QString m_FaceAttributeMatrixName = {};
  QString m_SurfaceMeshPrincipalCurvature1sArrayName = {};
  QString m_SurfaceMeshPrincipalCurvature2sArrayName = {};
  QString m_SurfaceMeshPrincipalDirection1sArrayName = {};
  QString m_SurfaceMeshPrincipalDirection2sArrayName = {};
  QString m_SurfaceMeshNodeNormalsArrayName = {};
  QString m_SurfaceMeshFaceLabelsArrayName = {};
  QString m_SurfaceMeshFaceNormalsArrayName = {};
  QString m_InputFile = {};

public:
  GoldfeatherReader(const GoldfeatherReader&) = delete;            // Copy Constructor Not Implemented
  GoldfeatherReader(GoldfeatherReader&&) = delete;                 // Move Constructor Not Implemented
  GoldfeatherReader& operator=(const GoldfeatherReader&) = delete; // Copy Assignment Not Implemented
  GoldfeatherReader& operator=(GoldfeatherReader&&) = delete;      // Move Assignment Not Implemented
};
