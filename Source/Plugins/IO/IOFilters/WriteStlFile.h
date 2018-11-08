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

#include "IO/IODLLExport.h"

/**
 * @brief The WriteStlFile class. See [Filter documentation](@ref surfacemeshtostl) for details.
 */
class IO_EXPORT WriteStlFile : public AbstractFilter
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(WriteStlFile SUPERCLASS AbstractFilter)
    PYB11_PROPERTY(QString OutputStlDirectory READ getOutputStlDirectory WRITE setOutputStlDirectory)
    PYB11_PROPERTY(QString OutputStlPrefix READ getOutputStlPrefix WRITE setOutputStlPrefix)
    PYB11_PROPERTY(bool GroupByPhase READ getGroupByPhase WRITE setGroupByPhase)
    PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)
    PYB11_PROPERTY(DataArrayPath SurfaceMeshFacePhasesArrayPath READ getSurfaceMeshFacePhasesArrayPath WRITE setSurfaceMeshFacePhasesArrayPath)
public:
  SIMPL_SHARED_POINTERS(WriteStlFile)
  SIMPL_FILTER_NEW_MACRO(WriteStlFile)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(WriteStlFile, AbstractFilter)

  ~WriteStlFile() override;

  /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */
  SIMPL_FILTER_PARAMETER(QString, OutputStlDirectory)
  Q_PROPERTY(QString OutputStlDirectory READ getOutputStlDirectory WRITE setOutputStlDirectory)

  SIMPL_FILTER_PARAMETER(QString, OutputStlPrefix)
  Q_PROPERTY(QString OutputStlPrefix READ getOutputStlPrefix WRITE setOutputStlPrefix)

  SIMPL_FILTER_PARAMETER(bool, GroupByPhase)
  Q_PROPERTY(bool GroupByPhase READ getGroupByPhase WRITE setGroupByPhase)

  SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
  Q_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFacePhasesArrayPath)
  Q_PROPERTY(DataArrayPath SurfaceMeshFacePhasesArrayPath READ getSurfaceMeshFacePhasesArrayPath WRITE setSurfaceMeshFacePhasesArrayPath)

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
  * @brief readFilterParametersFromJson Reads the filter parameters from a file
  * @param reader Reader that is used to read the parameters from a file
  */
  void readFilterParameters(QJsonObject& obj) override;

  /**
  * @brief writeFilterParametersToJson Writes the filter parameters to a file
  * @param root The root json object
  */
  void writeFilterParameters(QJsonObject& obj) const override;

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
  WriteStlFile();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  DEFINE_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFaceLabels)
  DEFINE_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFacePhases)

  /**
   * @brief writeHeader Writes the header of the STL file
   * @param f File instance pointer
   * @param header Header to write to file
   * @param triCount Number of triangles
   * @return Integer error value
   */
  int32_t writeHeader(FILE* f, const QString& header, int32_t triCount);

  /**
   * @brief writeNumTrianglesToFile Writes the number of triangles to the STL file
   * @param filename Name of the output file
   * @param triCount Number of triangles
   * @return Integer error value
   */
  int32_t writeNumTrianglesToFile(const QString& filename, int32_t triCount);

public:
  WriteStlFile(const WriteStlFile&) = delete;            // Copy Constructor Not Implemented
  WriteStlFile(WriteStlFile&&) = delete;                 // Move Constructor Not Implemented
  WriteStlFile& operator=(const WriteStlFile&) = delete; // Copy Assignment Not Implemented
  WriteStlFile& operator=(WriteStlFile&&) = delete;      // Move Assignment Not Implemented
};

