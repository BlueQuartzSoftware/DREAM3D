/* ============================================================================
 * Copyright (c) 2018 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"


/**
 * @brief The RodriguesConvertor class. See [Filter documentation](@ref rodriguesconvertor) for details.
 */
class RodriguesConvertor : public AbstractFilter
{
  Q_OBJECT

  public:
    SIMPL_SHARED_POINTERS(RodriguesConvertor)
    SIMPL_FILTER_NEW_MACRO(RodriguesConvertor)
    SIMPL_TYPE_MACRO_SUPER(RodriguesConvertor, AbstractFilter)

    ~RodriguesConvertor() override;

    SIMPL_FILTER_PARAMETER(DataArrayPath, RodriguesDataArrayPath)
    Q_PROPERTY(DataArrayPath RodriguesDataArrayPath READ getRodriguesDataArrayPath WRITE setRodriguesDataArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, OutputDataArrayPath)
    Q_PROPERTY(DataArrayPath OutputDataArrayPath READ getOutputDataArrayPath WRITE setOutputDataArrayPath)

    SIMPL_FILTER_PARAMETER(bool, DeleteOriginalData)
    Q_PROPERTY(bool DeleteOriginalData READ getDeleteOriginalData WRITE setDeleteOriginalData)


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
    RodriguesConvertor();

    /**
    * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
    */
    void dataCheck();

    /**
    * @brief Initializes all the private instance variables.
    */
    void initialize();
  private:
  DEFINE_DATAARRAY_VARIABLE(float, RodriguesVectors)
  DEFINE_DATAARRAY_VARIABLE(float, OutputRodriguesVectors)

  
  public:
    /* Rule of 5: All special member functions should be defined if any are defined.
    * https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-default-operation-define-or-delete-them-all
    */
  public:
    RodriguesConvertor(const RodriguesConvertor&) = delete;             // Copy Constructor Not Implemented
    RodriguesConvertor& operator=(const RodriguesConvertor&) = delete;  // Copy Assignment Not Implemented
    RodriguesConvertor(RodriguesConvertor&&) = delete;                  // Move Constructor Not Implemented
    RodriguesConvertor& operator=(RodriguesConvertor&&) = delete;       // Move Assignment Not Implemented

};

