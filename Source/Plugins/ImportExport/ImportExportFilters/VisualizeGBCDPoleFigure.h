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

// Needed for AxisAngle_t
#include <memory>

#include "EbsdLib/EbsdConstants.h"

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/FilterParameters/AxisAngleInput.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "OrientationLib/LaueOps/LaueOps.h"

#include "ImportExport/ImportExportDLLExport.h"

/**
 * @brief The VisualizeGBCDPoleFigure class. See [Filter documentation](@ref visualizegbcdpolefigure) for details.
 */
class ImportExport_EXPORT VisualizeGBCDPoleFigure : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(VisualizeGBCDPoleFigure SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(VisualizeGBCDPoleFigure)
  PYB11_FILTER_NEW_MACRO(VisualizeGBCDPoleFigure)
  PYB11_FILTER_PARAMETER(QString, OutputFile)
  PYB11_FILTER_PARAMETER(int, PhaseOfInterest)
  PYB11_FILTER_PARAMETER(AxisAngleInput_t, MisorientationRotation)
  PYB11_FILTER_PARAMETER(DataArrayPath, GBCDArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  PYB11_PROPERTY(QString OutputFile READ getOutputFile WRITE setOutputFile)
  PYB11_PROPERTY(int PhaseOfInterest READ getPhaseOfInterest WRITE setPhaseOfInterest)
  PYB11_PROPERTY(AxisAngleInput_t MisorientationRotation READ getMisorientationRotation WRITE setMisorientationRotation)
  PYB11_PROPERTY(DataArrayPath GBCDArrayPath READ getGBCDArrayPath WRITE setGBCDArrayPath)
  PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
#endif

public:
  using Self = VisualizeGBCDPoleFigure;
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
   * @brief Returns the name of the class for VisualizeGBCDPoleFigure
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for VisualizeGBCDPoleFigure
   */
  static QString ClassName();

  ~VisualizeGBCDPoleFigure() override;

  /**
   * @brief Setter property for OutputFile
   */
  void setOutputFile(const QString& value);
  /**
   * @brief Getter property for OutputFile
   * @return Value of OutputFile
   */
  QString getOutputFile() const;

  Q_PROPERTY(QString OutputFile READ getOutputFile WRITE setOutputFile)

  /**
   * @brief Setter property for PhaseOfInterest
   */
  void setPhaseOfInterest(int value);
  /**
   * @brief Getter property for PhaseOfInterest
   * @return Value of PhaseOfInterest
   */
  int getPhaseOfInterest() const;

  Q_PROPERTY(int PhaseOfInterest READ getPhaseOfInterest WRITE setPhaseOfInterest)

  /**
   * @brief Setter property for MisorientationRotation
   */
  void setMisorientationRotation(const AxisAngleInput_t& value);
  /**
   * @brief Getter property for MisorientationRotation
   * @return Value of MisorientationRotation
   */
  AxisAngleInput_t getMisorientationRotation() const;

  Q_PROPERTY(AxisAngleInput_t MisorientationRotation READ getMisorientationRotation WRITE setMisorientationRotation)

  /**
   * @brief Setter property for GBCDArrayPath
   */
  void setGBCDArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for GBCDArrayPath
   * @return Value of GBCDArrayPath
   */
  DataArrayPath getGBCDArrayPath() const;

  Q_PROPERTY(DataArrayPath GBCDArrayPath READ getGBCDArrayPath WRITE setGBCDArrayPath)

  /**
   * @brief Setter property for CrystalStructuresArrayPath
   */
  void setCrystalStructuresArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CrystalStructuresArrayPath
   * @return Value of CrystalStructuresArrayPath
   */
  DataArrayPath getCrystalStructuresArrayPath() const;

  Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

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
  VisualizeGBCDPoleFigure();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief getSquareCoord Computes the square based coordinate based on the incoming normal
   * @param xstl1_norm1 Incoming normal
   * @param sqCoord Computed square coordinate
   * @return Boolean value for whether coordinate lies in the norther hemisphere
   */
  bool getSquareCoord(float* xstl1_norm1, float* sqCoord);

private:
  std::weak_ptr<DataArray<double>> m_GBCDPtr;
  double* m_GBCD = nullptr;
  std::weak_ptr<DataArray<unsigned int>> m_CrystalStructuresPtr;
  unsigned int* m_CrystalStructures = nullptr;

  QString m_OutputFile = {};
  int m_PhaseOfInterest = {};
  AxisAngleInput_t m_MisorientationRotation = {};
  DataArrayPath m_GBCDArrayPath = {};
  DataArrayPath m_CrystalStructuresArrayPath = {};

  QVector<LaueOps::Pointer> m_OrientationOps;

  /**
   * @brief writeCoords Writes a set of Axis coordinates to that are needed
   * for a Rectilinear Grid based data set to a VTK file
   * @param f File instance pointer
   * @param axis The name of the axis that is being written
   * @param type The type of primitive being written (float, int, ...)
   * @param npoints The total number of points in the array
   * @param min The minimum value of the axis
   * @param max The maximum value of the axis
   * @param step The step value between each point on the axis.
   * @return Integer error value
   */
  int32_t writeCoords(FILE* f, const char* axis, const char* type, int64_t npoints, float min, float step);

public:
  VisualizeGBCDPoleFigure(const VisualizeGBCDPoleFigure&) = delete;            // Copy Constructor Not Implemented
  VisualizeGBCDPoleFigure(VisualizeGBCDPoleFigure&&) = delete;                 // Move Constructor Not Implemented
  VisualizeGBCDPoleFigure& operator=(const VisualizeGBCDPoleFigure&) = delete; // Copy Assignment Not Implemented
  VisualizeGBCDPoleFigure& operator=(VisualizeGBCDPoleFigure&&) = delete;      // Move Assignment Not Implemented
};
