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
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "ImportExport/ImportExportDLLExport.h"

/**
 * @class CMUSmoothingToVtk CMUSmoothingToVtk.h NeverMind/Code/NeverMindFilters/CMUSmoothingToVtk.h
 * @brief Converts the Nodes and Triangles files produced by the CMU SurfaceMeshing
 * and Surface Smoothing algorithms into a VTK Polydata File
 * @author
 * @date
 * @version 1.0
 */
class ImportExport_EXPORT NodesTrianglesToVtk : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(NodesTrianglesToVtk SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(NodesTrianglesToVtk)
  PYB11_FILTER_NEW_MACRO(NodesTrianglesToVtk)
  PYB11_FILTER_PARAMETER(QString, NodesFile)
  PYB11_FILTER_PARAMETER(QString, TrianglesFile)
  PYB11_FILTER_PARAMETER(QString, OutputVtkFile)
  PYB11_FILTER_PARAMETER(bool, WriteBinaryFile)
  PYB11_FILTER_PARAMETER(bool, WriteConformalMesh)
  PYB11_PROPERTY(QString NodesFile READ getNodesFile WRITE setNodesFile)
  PYB11_PROPERTY(QString TrianglesFile READ getTrianglesFile WRITE setTrianglesFile)
  PYB11_PROPERTY(QString OutputVtkFile READ getOutputVtkFile WRITE setOutputVtkFile)
  PYB11_PROPERTY(bool WriteBinaryFile READ getWriteBinaryFile WRITE setWriteBinaryFile)
  PYB11_PROPERTY(bool WriteConformalMesh READ getWriteConformalMesh WRITE setWriteConformalMesh)
#endif

public:
  using Self = NodesTrianglesToVtk;
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
   * @brief Returns the name of the class for NodesTrianglesToVtk
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for NodesTrianglesToVtk
   */
  static QString ClassName();

  ~NodesTrianglesToVtk() override;

  /**
   * @brief Setter property for NodesFile
   */
  void setNodesFile(const QString& value);
  /**
   * @brief Getter property for NodesFile
   * @return Value of NodesFile
   */
  QString getNodesFile() const;

  Q_PROPERTY(QString NodesFile READ getNodesFile WRITE setNodesFile)

  /**
   * @brief Setter property for TrianglesFile
   */
  void setTrianglesFile(const QString& value);
  /**
   * @brief Getter property for TrianglesFile
   * @return Value of TrianglesFile
   */
  QString getTrianglesFile() const;

  Q_PROPERTY(QString TrianglesFile READ getTrianglesFile WRITE setTrianglesFile)

  /**
   * @brief Setter property for OutputVtkFile
   */
  void setOutputVtkFile(const QString& value);
  /**
   * @brief Getter property for OutputVtkFile
   * @return Value of OutputVtkFile
   */
  QString getOutputVtkFile() const;

  Q_PROPERTY(QString OutputVtkFile READ getOutputVtkFile WRITE setOutputVtkFile)

  /**
   * @brief Setter property for WriteBinaryFile
   */
  void setWriteBinaryFile(bool value);
  /**
   * @brief Getter property for WriteBinaryFile
   * @return Value of WriteBinaryFile
   */
  bool getWriteBinaryFile() const;

  Q_PROPERTY(bool WriteBinaryFile READ getWriteBinaryFile WRITE setWriteBinaryFile)

  /**
   * @brief Setter property for WriteConformalMesh
   */
  void setWriteConformalMesh(bool value);
  /**
   * @brief Getter property for WriteConformalMesh
   * @return Value of WriteConformalMesh
   */
  bool getWriteConformalMesh() const;

  Q_PROPERTY(bool WriteConformalMesh READ getWriteConformalMesh WRITE setWriteConformalMesh)

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
  NodesTrianglesToVtk();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  int writeBinaryCellData(const QString& TrianglesFile, FILE* vtkFile, int nTriangles, bool conformalMesh);
  int writeASCIICellData(const QString& TrianglesFile, FILE* vtkFile, int nTriangles, bool conformalMesh);

  int writeBinaryPointData(const QString& NodesFile, FILE* vtkFile, int nNodes, bool conformalMesh);
  int writeASCIIPointData(const QString& NodesFile, FILE* vtkFile, int nNodes, bool conformalMesh);

private:
  QString m_NodesFile = {};
  QString m_TrianglesFile = {};
  QString m_OutputVtkFile = {};
  bool m_WriteBinaryFile = {};
  bool m_WriteConformalMesh = {};

  int8_t* m_NodeKind;

public:
  NodesTrianglesToVtk(const NodesTrianglesToVtk&) = delete;            // Copy Constructor Not Implemented
  NodesTrianglesToVtk(NodesTrianglesToVtk&&) = delete;                 // Move Constructor Not Implemented
  NodesTrianglesToVtk& operator=(const NodesTrianglesToVtk&) = delete; // Copy Assignment Not Implemented
  NodesTrianglesToVtk& operator=(NodesTrianglesToVtk&&) = delete;      // Move Assignment Not Implemented
};
