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

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "Statistics/StatisticsDLLExport.h"

/**
 * @class FindSaltykovSizes FindSaltykovSizes.h DREAM3DLib/GenericFilters/FindSaltykovSizes.h
 * @brief
 * @author Joseph C. Tucker
 * @date Dec 9, 2014
 * @version 5.2
 */
class Statistics_EXPORT FindSaltykovSizes : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(FindSaltykovSizes SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(FindSaltykovSizes)
  PYB11_FILTER_NEW_MACRO(FindSaltykovSizes)
  PYB11_FILTER_PARAMETER(DataArrayPath, EquivalentDiametersArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, SaltykovEquivalentDiametersArrayPath)
  PYB11_PROPERTY(DataArrayPath EquivalentDiametersArrayPath READ getEquivalentDiametersArrayPath WRITE setEquivalentDiametersArrayPath)
  PYB11_PROPERTY(DataArrayPath SaltykovEquivalentDiametersArrayPath READ getSaltykovEquivalentDiametersArrayPath WRITE setSaltykovEquivalentDiametersArrayPath)
#endif

public:
  using Self = FindSaltykovSizes;
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
   * @brief Returns the name of the class for FindSaltykovSizes
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FindSaltykovSizes
   */
  static QString ClassName();

  ~FindSaltykovSizes() override;

  /**
   * @brief Setter property for EquivalentDiametersArrayPath
   */
  void setEquivalentDiametersArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for EquivalentDiametersArrayPath
   * @return Value of EquivalentDiametersArrayPath
   */
  DataArrayPath getEquivalentDiametersArrayPath() const;

  Q_PROPERTY(DataArrayPath EquivalentDiametersArrayPath READ getEquivalentDiametersArrayPath WRITE setEquivalentDiametersArrayPath)

  /**
   * @brief Setter property for SaltykovEquivalentDiametersArrayPath
   */
  void setSaltykovEquivalentDiametersArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SaltykovEquivalentDiametersArrayPath
   * @return Value of SaltykovEquivalentDiametersArrayPath
   */
  DataArrayPath getSaltykovEquivalentDiametersArrayPath() const;

  Q_PROPERTY(DataArrayPath SaltykovEquivalentDiametersArrayPath READ getSaltykovEquivalentDiametersArrayPath WRITE setSaltykovEquivalentDiametersArrayPath)

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
  FindSaltykovSizes();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief find_saltykov_sizes
   */
  void find_saltykov_sizes();

  /**
   * @brief do_saltykov
   * @param nA
   * @param Dmax
   * @param k
   * @return
   */
  int do_saltykov(std::vector<int> nA, float Dmax, int k);

  /**
   * @brief forward_difference
   * @param fx
   * @param f1
   * @param f0
   * @param x1
   * @param x0
   * @return
   */
  int forward_difference(int fx, int f1, int f0, int x1, int x0);

  /**
   * @brief round_to_nearest_int
   * @param x
   * @return
   */
  template <class T> int round_to_nearest_int(T x);

private:
  std::weak_ptr<DataArray<float>> m_EquivalentDiametersPtr;
  float* m_EquivalentDiameters = nullptr;
  std::weak_ptr<DataArray<float>> m_SaltykovEquivalentDiametersPtr;
  float* m_SaltykovEquivalentDiameters = nullptr;

  DataArrayPath m_EquivalentDiametersArrayPath = {};
  DataArrayPath m_SaltykovEquivalentDiametersArrayPath = {};

public:
  FindSaltykovSizes(const FindSaltykovSizes&) = delete; // Copy Constructor Not Implemented
  FindSaltykovSizes(FindSaltykovSizes&&) = delete;      // Move Constructor Not Implemented
  FindSaltykovSizes& operator=(const FindSaltykovSizes&) = delete; // Copy Assignment Not Implemented
  FindSaltykovSizes& operator=(FindSaltykovSizes&&) = delete;      // Move Assignment Not Implemented
};

