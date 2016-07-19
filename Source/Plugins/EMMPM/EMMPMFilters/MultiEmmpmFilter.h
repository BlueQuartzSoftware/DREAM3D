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


#ifndef _multiemmpmfilter_h_
#define _multiemmpmfilter_h_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "EMMPMFilter.h"

/**
 * @brief The MultiEmmpmFilter class. See [Filter documentation](@ref multiemmpmfilter) for details.
 */
class MultiEmmpmFilter : public EMMPMFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    SIMPL_SHARED_POINTERS(MultiEmmpmFilter)
    SIMPL_STATIC_NEW_MACRO(MultiEmmpmFilter)
    SIMPL_TYPE_MACRO_SUPER(MultiEmmpmFilter, AbstractFilter)

    virtual ~MultiEmmpmFilter();

    SIMPL_FILTER_PARAMETER(QVector<DataArrayPath>, InputDataArrayVector)
    Q_PROPERTY(QVector<DataArrayPath> InputDataArrayVector READ getInputDataArrayVector WRITE setInputDataArrayVector)

    SIMPL_FILTER_PARAMETER(QString, OutputAttributeMatrixName)
    Q_PROPERTY(QString OutputAttributeMatrixName READ getOutputAttributeMatrixName WRITE setOutputAttributeMatrixName)

    SIMPL_FILTER_PARAMETER(QString, OutputArrayPrefix)
    Q_PROPERTY(QString OutputArrayPrefix READ getOutputArrayPrefix WRITE setOutputArrayPrefix)

    SIMPL_FILTER_PARAMETER(bool, UsePreviousMuSigma)
    Q_PROPERTY(bool UsePreviousMuSigma READ getUsePreviousMuSigma WRITE setUsePreviousMuSigma)

    /**
     * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getCompiledLibraryName();

    /**
     * @brief getBrandingString Returns the branding string for the filter, which is a tag
     * used to denote the filter's association with specific plugins
     * @return Branding string
    */
    virtual const QString getBrandingString();

    /**
     * @brief getFilterVersion Returns a version string for this filter. Default
     * value is an empty string.
     * @return
     */
    virtual const QString getFilterVersion();

    /**
     * @brief newFilterInstance Reimplemented from @see AbstractFilter class
     */
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);

    /**
     * @brief getGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getGroupName();

    /**
     * @brief getSubGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getSubGroupName();

    /**
     * @brief getHumanLabel Reimplemented from @see AbstractFilter class
     */
    virtual const QString getHumanLabel();

    /**
     * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void setupFilterParameters();

    /**
     * @brief readFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
     * @brief execute Reimplemented from @see AbstractFilter class
     */
    virtual void execute();

    /**
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    virtual void preflight();

  protected:
    MultiEmmpmFilter();
    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();


  private:
    DEFINE_DATAARRAY_VARIABLE(uint8_t, InputImage)
    DEFINE_DATAARRAY_VARIABLE(uint8_t, OutputImage)

    std::vector<float> m_PreviousMu;
    std::vector<float> m_PreviousSigma;

    MultiEmmpmFilter(const MultiEmmpmFilter&); // Copy Constructor Not Implemented
    void operator=(const MultiEmmpmFilter&); // Operator '=' Not Implemented
};

#endif /* _MultiEmmpmFilter_H_ */
