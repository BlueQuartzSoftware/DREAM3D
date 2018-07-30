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

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "IO/IODLLExport.h"

/**
 * @brief The AbaqusHexahedronWriter class. See [Filter documentation](@ref abaqushexahedronwriter) for details.
 */
class IO_EXPORT AbaqusHexahedronWriter : public AbstractFilter
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(AbaqusHexahedronWriter SUPERCLASS AbstractFilter)
    PYB11_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)
    PYB11_PROPERTY(QString FilePrefix READ getFilePrefix WRITE setFilePrefix)
    PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
    PYB11_PROPERTY(int HourglassStiffness READ getHourglassStiffness WRITE setHourglassStiffness)
    PYB11_PROPERTY(QString JobName READ getJobName WRITE setJobName)

public:
  SIMPL_SHARED_POINTERS(AbaqusHexahedronWriter)
  SIMPL_FILTER_NEW_MACRO(AbaqusHexahedronWriter)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(AbaqusHexahedronWriter, AbstractFilter)

  ~AbaqusHexahedronWriter() override;

  SIMPL_FILTER_PARAMETER(QString, OutputPath)
  Q_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)

  SIMPL_FILTER_PARAMETER(QString, FilePrefix)
  Q_PROPERTY(QString FilePrefix READ getFilePrefix WRITE setFilePrefix)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

  SIMPL_FILTER_PARAMETER(int, HourglassStiffness)
  Q_PROPERTY(int HourglassStiffness READ getHourglassStiffness WRITE setHourglassStiffness)

  SIMPL_FILTER_PARAMETER(QString, JobName)
  Q_PROPERTY(QString JobName READ getJobName WRITE setJobName)

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
  AbaqusHexahedronWriter();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)

  /**
   * @brief writeNodes Writes the _nodes.inp file
   * @param fileNames QList of output file names
   * @param cDims Dimensions of incoming volume (points)
   * @param origin Origin of volume
   * @param spacing Resolution of volume
   * @return Integer error value
   */
  int32_t writeNodes(const QList<QString>& fileNames, size_t* cDims, float* origin, float* spacing);

  /**
   * @brief writeElems Writes the _elems.inp file
   * @param fileNames QList of output file names
   * @param cDims Dimensions of incoming volume (points)
   * @param pDims Dimensions of incoming volume (elements)
   * @return Integer error value
   */
  int32_t writeElems(const QList<QString>& fileNames, size_t* cDims, size_t* pDims);

  /**
   * @brief writeSects Writes the _sects.inp file
   * @param file Output file name
   * @param totalPoints Total points of incoming volume
   * @return Integer error value
   */
  int32_t writeSects(const QString& file, size_t totalPoints);

  /**
   * @brief writeElset Writes the _elset.inp file
   * @param fileNames QList of output file names
   * @param totalPoints Total points of incoming volume
   * @return Integer error value
   */
  int32_t writeElset(const QList<QString>& fileNames, size_t totalPoints);

  /**
   * @brief writeMaster Writes the master file
   * @param file Output file name
   * @return Integer error value
   */
  int32_t writeMaster(const QString& file);

  /**
   * @brief getNodeIds Returns a vector of node Ids for a given
   * set of dimensional indices
   * @param x X coordinate
   * @param y Y coordinate
   * @param z Z coordinate
   * @param pDims Dimensions of incoming volume
   * @return  Vector of node Ids
   */
  std::vector<int64_t> getNodeIds(size_t x, size_t y, size_t z, size_t* pDims);

  /**
   * @brief deleteFile Removes written files
   * @param fileNames QList of output file names
   */
  void deleteFile(const QList<QString>& fileNames);

public:
  AbaqusHexahedronWriter(const AbaqusHexahedronWriter&) = delete; // Copy Constructor Not Implemented
  AbaqusHexahedronWriter(AbaqusHexahedronWriter&&) = delete;      // Move Constructor Not Implemented
  AbaqusHexahedronWriter& operator=(const AbaqusHexahedronWriter&) = delete; // Copy Assignment Not Implemented
  AbaqusHexahedronWriter& operator=(AbaqusHexahedronWriter&&) = delete;      // Move Assignment Not Implemented
};

