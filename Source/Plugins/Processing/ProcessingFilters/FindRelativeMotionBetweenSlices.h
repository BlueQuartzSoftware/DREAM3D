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

class IDataArray;
using IDataArrayWkPtrType = std::weak_ptr<IDataArray>;

#include "Processing/ProcessingDLLExport.h"

/**
 * @brief The FindRelativeMotionBetweenSlices class. See [Filter documentation](@ref findrelativemotionbetweenslices) for details.
 */
class Processing_EXPORT FindRelativeMotionBetweenSlices : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(FindRelativeMotionBetweenSlices SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(FindRelativeMotionBetweenSlices)
  PYB11_FILTER_NEW_MACRO(FindRelativeMotionBetweenSlices)
  PYB11_FILTER_PARAMETER(DataArrayPath, SelectedArrayPath)
  PYB11_FILTER_PARAMETER(unsigned int, Plane)
  PYB11_FILTER_PARAMETER(int, PSize1)
  PYB11_FILTER_PARAMETER(int, PSize2)
  PYB11_FILTER_PARAMETER(int, SSize1)
  PYB11_FILTER_PARAMETER(int, SSize2)
  PYB11_FILTER_PARAMETER(int, SliceStep)
  PYB11_FILTER_PARAMETER(QString, MotionDirectionArrayName)
  PYB11_PROPERTY(DataArrayPath SelectedArrayPath READ getSelectedArrayPath WRITE setSelectedArrayPath)
  PYB11_PROPERTY(unsigned int Plane READ getPlane WRITE setPlane)
  PYB11_PROPERTY(int PSize1 READ getPSize1 WRITE setPSize1)
  PYB11_PROPERTY(int PSize2 READ getPSize2 WRITE setPSize2)
  PYB11_PROPERTY(int SSize1 READ getSSize1 WRITE setSSize1)
  PYB11_PROPERTY(int SSize2 READ getSSize2 WRITE setSSize2)
  PYB11_PROPERTY(int SliceStep READ getSliceStep WRITE setSliceStep)
  PYB11_PROPERTY(QString MotionDirectionArrayName READ getMotionDirectionArrayName WRITE setMotionDirectionArrayName)
#endif

public:
  using Self = FindRelativeMotionBetweenSlices;
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
   * @brief Returns the name of the class for FindRelativeMotionBetweenSlices
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FindRelativeMotionBetweenSlices
   */
  static QString ClassName();

  ~FindRelativeMotionBetweenSlices() override;

  /**
   * @brief Setter property for SelectedArrayPath
   */
  void setSelectedArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SelectedArrayPath
   * @return Value of SelectedArrayPath
   */
  DataArrayPath getSelectedArrayPath() const;

  Q_PROPERTY(DataArrayPath SelectedArrayPath READ getSelectedArrayPath WRITE setSelectedArrayPath)

  /**
   * @brief Setter property for Plane
   */
  void setPlane(unsigned int value);
  /**
   * @brief Getter property for Plane
   * @return Value of Plane
   */
  unsigned int getPlane() const;

  Q_PROPERTY(unsigned int Plane READ getPlane WRITE setPlane)

  /**
   * @brief Setter property for PSize1
   */
  void setPSize1(int value);
  /**
   * @brief Getter property for PSize1
   * @return Value of PSize1
   */
  int getPSize1() const;

  Q_PROPERTY(int PSize1 READ getPSize1 WRITE setPSize1)

  /**
   * @brief Setter property for PSize2
   */
  void setPSize2(int value);
  /**
   * @brief Getter property for PSize2
   * @return Value of PSize2
   */
  int getPSize2() const;

  Q_PROPERTY(int PSize2 READ getPSize2 WRITE setPSize2)

  /**
   * @brief Setter property for SSize1
   */
  void setSSize1(int value);
  /**
   * @brief Getter property for SSize1
   * @return Value of SSize1
   */
  int getSSize1() const;

  Q_PROPERTY(int SSize1 READ getSSize1 WRITE setSSize1)

  /**
   * @brief Setter property for SSize2
   */
  void setSSize2(int value);
  /**
   * @brief Getter property for SSize2
   * @return Value of SSize2
   */
  int getSSize2() const;

  Q_PROPERTY(int SSize2 READ getSSize2 WRITE setSSize2)

  /**
   * @brief Setter property for SliceStep
   */
  void setSliceStep(int value);
  /**
   * @brief Getter property for SliceStep
   * @return Value of SliceStep
   */
  int getSliceStep() const;

  Q_PROPERTY(int SliceStep READ getSliceStep WRITE setSliceStep)

  /**
   * @brief Setter property for MotionDirectionArrayName
   */
  void setMotionDirectionArrayName(const QString& value);
  /**
   * @brief Getter property for MotionDirectionArrayName
   * @return Value of MotionDirectionArrayName
   */
  QString getMotionDirectionArrayName() const;

  Q_PROPERTY(QString MotionDirectionArrayName READ getMotionDirectionArrayName WRITE setMotionDirectionArrayName)

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
  FindRelativeMotionBetweenSlices();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  IDataArrayWkPtrType m_InDataPtr;
  void* m_InData = nullptr;

  std::weak_ptr<DataArray<float>> m_MotionDirectionPtr;
  float* m_MotionDirection = nullptr;

  DataArrayPath m_SelectedArrayPath = {};
  unsigned int m_Plane = {};
  int m_PSize1 = {};
  int m_PSize2 = {};
  int m_SSize1 = {};
  int m_SSize2 = {};
  int m_SliceStep = {};
  QString m_MotionDirectionArrayName = {};

public:
  FindRelativeMotionBetweenSlices(const FindRelativeMotionBetweenSlices&) = delete; // Copy Constructor Not Implemented
  FindRelativeMotionBetweenSlices(FindRelativeMotionBetweenSlices&&) = delete;      // Move Constructor Not Implemented
  FindRelativeMotionBetweenSlices& operator=(const FindRelativeMotionBetweenSlices&) = delete; // Copy Assignment Not Implemented
  FindRelativeMotionBetweenSlices& operator=(FindRelativeMotionBetweenSlices&&) = delete;      // Move Assignment Not Implemented
};

