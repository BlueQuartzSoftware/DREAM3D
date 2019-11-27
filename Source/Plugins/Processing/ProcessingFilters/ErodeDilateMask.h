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

#include "Processing/ProcessingDLLExport.h"

/**
 * @brief The ErodeDilateMask class. See [Filter documentation](@ref erodedilatemask) for details.
 */
class Processing_EXPORT ErodeDilateMask : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(ErodeDilateMask SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(ErodeDilateMask)
  PYB11_FILTER_NEW_MACRO(ErodeDilateMask)
  PYB11_FILTER_PARAMETER(unsigned int, Direction)
  PYB11_FILTER_PARAMETER(int, NumIterations)
  PYB11_FILTER_PARAMETER(bool, XDirOn)
  PYB11_FILTER_PARAMETER(bool, YDirOn)
  PYB11_FILTER_PARAMETER(bool, ZDirOn)
  PYB11_FILTER_PARAMETER(DataArrayPath, MaskArrayPath)
  PYB11_PROPERTY(unsigned int Direction READ getDirection WRITE setDirection)
  PYB11_PROPERTY(int NumIterations READ getNumIterations WRITE setNumIterations)
  PYB11_PROPERTY(bool XDirOn READ getXDirOn WRITE setXDirOn)
  PYB11_PROPERTY(bool YDirOn READ getYDirOn WRITE setYDirOn)
  PYB11_PROPERTY(bool ZDirOn READ getZDirOn WRITE setZDirOn)
  PYB11_PROPERTY(DataArrayPath MaskArrayPath READ getMaskArrayPath WRITE setMaskArrayPath)
#endif

public:
  using Self = ErodeDilateMask;
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
   * @brief Returns the name of the class for ErodeDilateMask
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ErodeDilateMask
   */
  static QString ClassName();

  ~ErodeDilateMask() override;

  /**
   * @brief Setter property for Direction
   */
  void setDirection(unsigned int value);
  /**
   * @brief Getter property for Direction
   * @return Value of Direction
   */
  unsigned int getDirection() const;

  Q_PROPERTY(unsigned int Direction READ getDirection WRITE setDirection)

  /**
   * @brief Setter property for NumIterations
   */
  void setNumIterations(int value);
  /**
   * @brief Getter property for NumIterations
   * @return Value of NumIterations
   */
  int getNumIterations() const;

  Q_PROPERTY(int NumIterations READ getNumIterations WRITE setNumIterations)

  /**
   * @brief Setter property for XDirOn
   */
  void setXDirOn(bool value);
  /**
   * @brief Getter property for XDirOn
   * @return Value of XDirOn
   */
  bool getXDirOn() const;

  Q_PROPERTY(bool XDirOn READ getXDirOn WRITE setXDirOn)

  /**
   * @brief Setter property for YDirOn
   */
  void setYDirOn(bool value);
  /**
   * @brief Getter property for YDirOn
   * @return Value of YDirOn
   */
  bool getYDirOn() const;

  Q_PROPERTY(bool YDirOn READ getYDirOn WRITE setYDirOn)

  /**
   * @brief Setter property for ZDirOn
   */
  void setZDirOn(bool value);
  /**
   * @brief Getter property for ZDirOn
   * @return Value of ZDirOn
   */
  bool getZDirOn() const;

  Q_PROPERTY(bool ZDirOn READ getZDirOn WRITE setZDirOn)

  /**
   * @brief Setter property for MaskArrayPath
   */
  void setMaskArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for MaskArrayPath
   * @return Value of MaskArrayPath
   */
  DataArrayPath getMaskArrayPath() const;

  Q_PROPERTY(DataArrayPath MaskArrayPath READ getMaskArrayPath WRITE setMaskArrayPath)

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
  ErodeDilateMask();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  std::weak_ptr<DataArray<bool>> m_MaskPtr;
  bool* m_Mask = nullptr;

  unsigned int m_Direction = {};
  int m_NumIterations = {};
  bool m_XDirOn = {};
  bool m_YDirOn = {};
  bool m_ZDirOn = {};
  DataArrayPath m_MaskArrayPath = {};

  bool* m_MaskCopy;

public:
  ErodeDilateMask(const ErodeDilateMask&) = delete; // Copy Constructor Not Implemented
  ErodeDilateMask(ErodeDilateMask&&) = delete;      // Move Constructor Not Implemented
  ErodeDilateMask& operator=(const ErodeDilateMask&) = delete; // Copy Assignment Not Implemented
  ErodeDilateMask& operator=(ErodeDilateMask&&) = delete;      // Move Assignment Not Implemented
};

