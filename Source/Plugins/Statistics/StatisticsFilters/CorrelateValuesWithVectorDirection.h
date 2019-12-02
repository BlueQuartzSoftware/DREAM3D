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
#include <vector>

#include "OrientationLib/LaueOps/LaueOps.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Utilities/SIMPLibEndian.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "Statistics/StatisticsDLLExport.h"

#include "Statistics/StatisticsConstants.h"
#include "Statistics/StatisticsVersion.h"
/**
 * @class CorrelateValuesWithVectorDirection CorrelateValuesWithVectorDirection.h DREAM3DLib/GenericFilters/CorrelateValuesWithVectorDirection.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class Statistics_EXPORT CorrelateValuesWithVectorDirection : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(CorrelateValuesWithVectorDirection SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(CorrelateValuesWithVectorDirection)
  PYB11_FILTER_NEW_MACRO(CorrelateValuesWithVectorDirection)
  PYB11_FILTER_PARAMETER(DataArrayPath, CorrelatedDataArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, VectorDataArrayPath)
  PYB11_PROPERTY(DataArrayPath CorrelatedDataArrayPath READ getCorrelatedDataArrayPath WRITE setCorrelatedDataArrayPath)
  PYB11_PROPERTY(DataArrayPath VectorDataArrayPath READ getVectorDataArrayPath WRITE setVectorDataArrayPath)
#endif

public:
  using Self = CorrelateValuesWithVectorDirection;
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
   * @brief Returns the name of the class for CorrelateValuesWithVectorDirection
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for CorrelateValuesWithVectorDirection
   */
  static QString ClassName();

  ~CorrelateValuesWithVectorDirection() override;

  /**
   * @brief Setter property for CorrelatedDataArrayPath
   */
  void setCorrelatedDataArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CorrelatedDataArrayPath
   * @return Value of CorrelatedDataArrayPath
   */
  DataArrayPath getCorrelatedDataArrayPath() const;

  Q_PROPERTY(DataArrayPath CorrelatedDataArrayPath READ getCorrelatedDataArrayPath WRITE setCorrelatedDataArrayPath)
  /**
   * @brief Setter property for VectorDataArrayPath
   */
  void setVectorDataArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for VectorDataArrayPath
   * @return Value of VectorDataArrayPath
   */
  DataArrayPath getVectorDataArrayPath() const;

  Q_PROPERTY(DataArrayPath VectorDataArrayPath READ getVectorDataArrayPath WRITE setVectorDataArrayPath)

  /**
   * @brief Setter property for Logfile
   */
  void setLogfile(const QString& value);
  /**
   * @brief Getter property for Logfile
   * @return Value of Logfile
   */
  QString getLogfile() const;

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
  CorrelateValuesWithVectorDirection();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  void makeLambertProjection(size_t numComps);
  int determineSquareCoordsandBin(float xyz[3]);
  void determineXYZCoords(float sqCoords[2], float xyz[3]);
  void writeLambertProjection(size_t numComps);
  void writePFStats(size_t numComps);
  void createSterographicProjections(size_t numComps);

private:
  std::weak_ptr<DataArray<float>> m_VectorDataPtr;
  float* m_VectorData = nullptr;

  DataArrayPath m_CorrelatedDataArrayPath = {};
  DataArrayPath m_VectorDataArrayPath = {};
  QString m_Logfile = {};

  DoubleArrayType::Pointer m_LambertProj;

  float m_MaxCoord;
  int m_Dimension;
  float m_StepSize;

  /**
   * @brief This function writes a set of Axis coordinates to that are needed
   * for a Rectilinear Grid based data set.
   * @param f The "C" FILE* pointer to the file being written to.
   * @param axis The name of the Axis that is being written
   * @param type The type of primitive being written (float, int, ...)
   * @param npoints The total number of points in the array
   * @param min The minimum value of the axis
   * @param max The maximum value of the axis
   * @param step The step value between each point on the axis.
   */
  int writeCoords(FILE* f, const char* axis, const char* type, int64_t npoints, float min, float step);

public:
  CorrelateValuesWithVectorDirection(const CorrelateValuesWithVectorDirection&) = delete; // Copy Constructor Not Implemented
  CorrelateValuesWithVectorDirection(CorrelateValuesWithVectorDirection&&) = delete;      // Move Constructor Not Implemented
  CorrelateValuesWithVectorDirection& operator=(const CorrelateValuesWithVectorDirection&) = delete; // Copy Assignment Not Implemented
  CorrelateValuesWithVectorDirection& operator=(CorrelateValuesWithVectorDirection&&) = delete;      // Move Assignment Not Implemented
};

