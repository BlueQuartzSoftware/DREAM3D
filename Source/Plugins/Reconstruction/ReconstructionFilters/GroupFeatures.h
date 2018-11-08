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
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "Plugins/Reconstruction/ReconstructionConstants.h"

#include "Reconstruction/ReconstructionDLLExport.h"

/**
 * @brief The GroupFeatures class. This class serves as a superclass for other classes
 * in the Reconstruction plugin.
 */
class Reconstruction_EXPORT GroupFeatures : public AbstractFilter
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(GroupFeatures SUPERCLASS AbstractFilter)
    PYB11_PROPERTY(DataArrayPath ContiguousNeighborListArrayPath READ getContiguousNeighborListArrayPath WRITE setContiguousNeighborListArrayPath)
    PYB11_PROPERTY(DataArrayPath NonContiguousNeighborListArrayPath READ getNonContiguousNeighborListArrayPath WRITE setNonContiguousNeighborListArrayPath)
    PYB11_PROPERTY(bool UseNonContiguousNeighbors READ getUseNonContiguousNeighbors WRITE setUseNonContiguousNeighbors)
    PYB11_PROPERTY(bool PatchGrouping READ getPatchGrouping WRITE setPatchGrouping)
public:
  SIMPL_SHARED_POINTERS(GroupFeatures)
  SIMPL_FILTER_NEW_MACRO(GroupFeatures)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(GroupFeatures, AbstractFilter)

  ~GroupFeatures() override;

  SIMPL_FILTER_PARAMETER(DataArrayPath, ContiguousNeighborListArrayPath)
  Q_PROPERTY(DataArrayPath ContiguousNeighborListArrayPath READ getContiguousNeighborListArrayPath WRITE setContiguousNeighborListArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, NonContiguousNeighborListArrayPath)
  Q_PROPERTY(DataArrayPath NonContiguousNeighborListArrayPath READ getNonContiguousNeighborListArrayPath WRITE setNonContiguousNeighborListArrayPath)

  SIMPL_FILTER_PARAMETER(bool, UseNonContiguousNeighbors)
  Q_PROPERTY(float UseNonContiguousNeighbors READ getUseNonContiguousNeighbors WRITE setUseNonContiguousNeighbors)

  SIMPL_FILTER_PARAMETER(bool, PatchGrouping)
  Q_PROPERTY(float PatchGrouping READ getPatchGrouping WRITE setPatchGrouping)

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
  GroupFeatures();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief getSeed Initializes a new seed from which to star the burn algorithm
   * @param newFid Feature Id to initialize seed
   * @return Integer seed index
   */
  virtual int32_t getSeed(int32_t newFid);

  /**
   * @brief determineGrouping Determines if a neighbor should be added to the growing seed
   * @param referenceFeature Point of growing seed
   * @param neighborFeature Point to be compared for adding
   * @param newFid Feature Id of growing seed
   * @return Boolean check for whether the neighbor was added to the seed
   */
  virtual bool determineGrouping(int32_t referenceFeature, int32_t neighborFeature, int32_t newFid);

  /**
   * @brief growPatch Iteratively grows a patch
   * @param currentPatch Patch to grow
   * @return Boolean check for whether patch was grown
   */
  virtual bool growPatch(int32_t currentPatch);

  /**
   * @brief growGrouping Adds a Feature to an existing patch
   * @param referenceFeature Existing patch
   * @param neighborFeature Feature to be added
   * @param newFid Existing patch Id
   * @return Boolean check for whether Feature was added to the patch
   */
  virtual bool growGrouping(int32_t referenceFeature, int32_t neighborFeature, int32_t newFid);

private:
  NeighborList<int32_t>::WeakPointer m_ContiguousNeighborList;
  NeighborList<int32_t>::WeakPointer m_NonContiguousNeighborList;

public:
  GroupFeatures(const GroupFeatures&) = delete;  // Copy Constructor Not Implemented
  GroupFeatures(GroupFeatures&&) = delete;       // Move Constructor Not Implemented
  GroupFeatures& operator=(const GroupFeatures&) = delete; // Copy Assignment Not Implemented
  GroupFeatures& operator=(GroupFeatures&&) = delete;      // Move Assignment Not Implemented
};

