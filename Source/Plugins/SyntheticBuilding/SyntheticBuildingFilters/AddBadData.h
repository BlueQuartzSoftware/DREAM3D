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

#include "SyntheticBuilding/SyntheticBuildingDLLExport.h"

/**
 * @brief The AddBadData class. See [Filter documentation](@ref addbaddata) for details.
 */
class SyntheticBuilding_EXPORT AddBadData : public AbstractFilter
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(AddBadData SUPERCLASS AbstractFilter)
    PYB11_PROPERTY(DataArrayPath GBEuclideanDistancesArrayPath READ getGBEuclideanDistancesArrayPath WRITE setGBEuclideanDistancesArrayPath)
    PYB11_PROPERTY(bool PoissonNoise READ getPoissonNoise WRITE setPoissonNoise)
    PYB11_PROPERTY(float PoissonVolFraction READ getPoissonVolFraction WRITE setPoissonVolFraction)
    PYB11_PROPERTY(bool BoundaryNoise READ getBoundaryNoise WRITE setBoundaryNoise)
    PYB11_PROPERTY(float BoundaryVolFraction READ getBoundaryVolFraction WRITE setBoundaryVolFraction)
public:
  SIMPL_SHARED_POINTERS(AddBadData)
  SIMPL_FILTER_NEW_MACRO(AddBadData)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(AddBadData, AbstractFilter)

  ~AddBadData() override;

  SIMPL_FILTER_PARAMETER(DataArrayPath, GBEuclideanDistancesArrayPath)
  Q_PROPERTY(DataArrayPath GBEuclideanDistancesArrayPath READ getGBEuclideanDistancesArrayPath WRITE setGBEuclideanDistancesArrayPath)

  SIMPL_FILTER_PARAMETER(bool, PoissonNoise)
  Q_PROPERTY(bool PoissonNoise READ getPoissonNoise WRITE setPoissonNoise)

  SIMPL_FILTER_PARAMETER(float, PoissonVolFraction)
  Q_PROPERTY(float PoissonVolFraction READ getPoissonVolFraction WRITE setPoissonVolFraction)

  SIMPL_FILTER_PARAMETER(bool, BoundaryNoise)
  Q_PROPERTY(bool BoundaryNoise READ getBoundaryNoise WRITE setBoundaryNoise)

  SIMPL_FILTER_PARAMETER(float, BoundaryVolFraction)
  Q_PROPERTY(float BoundaryVolFraction READ getBoundaryVolFraction WRITE setBoundaryVolFraction)

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
  AddBadData();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief add_noise Adds "noise" to all arrays that belong to a particular Attribute Matrix.
   * "Noise" in this sense means a tuple value of 0.
   */
  void add_noise();

private:
  DEFINE_DATAARRAY_VARIABLE(int32_t, GBEuclideanDistances)

public:
  AddBadData(const AddBadData&) = delete;            // Copy Constructor Not Implemented
  AddBadData(AddBadData&&) = delete;                 // Move Constructor Not Implemented
  AddBadData& operator=(const AddBadData&) = delete; // Copy Assignment Not Implemented
  AddBadData& operator=(AddBadData&&) = delete;      // Move Assignment Not Implemented
};

