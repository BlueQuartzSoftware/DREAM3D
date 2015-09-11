/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


#ifndef _WarpRegularGrid_H_
#define _WarpRegularGrid_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/SecondOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/ThirdOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/FourthOrderPolynomialFilterParameter.h"

/**
 * @brief The WarpRegularGrid class. See [Filter documentation](@ref warpregulargrid) for details.
 */
class WarpRegularGrid : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    SIMPL_SHARED_POINTERS(WarpRegularGrid)
    SIMPL_STATIC_NEW_MACRO(WarpRegularGrid)
    SIMPL_TYPE_MACRO_SUPER(WarpRegularGrid, AbstractFilter)

    virtual ~WarpRegularGrid();

    SIMPL_FILTER_PARAMETER(QString, NewDataContainerName)
    Q_PROPERTY(QString NewDataContainerName READ getNewDataContainerName WRITE setNewDataContainerName)

    SIMPL_FILTER_PARAMETER(DataArrayPath, CellAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath CellAttributeMatrixPath READ getCellAttributeMatrixPath WRITE setCellAttributeMatrixPath)

    SIMPL_FILTER_PARAMETER(int, PolyOrder)
    Q_PROPERTY(int PolyOrder READ getPolyOrder WRITE setPolyOrder)

    SIMPL_FILTER_PARAMETER(Float2ndOrderPoly_t, SecondOrderACoeff)
    Q_PROPERTY(Float2ndOrderPoly_t SecondOrderACoeff READ getSecondOrderACoeff WRITE setSecondOrderACoeff)

    SIMPL_FILTER_PARAMETER(Float2ndOrderPoly_t, SecondOrderBCoeff)
    Q_PROPERTY(Float2ndOrderPoly_t SecondOrderBCoeff READ getSecondOrderBCoeff WRITE setSecondOrderBCoeff)

    SIMPL_FILTER_PARAMETER(Float3rdOrderPoly_t, ThirdOrderACoeff)
    Q_PROPERTY(Float3rdOrderPoly_t ThirdOrderACoeff READ getThirdOrderACoeff WRITE setThirdOrderACoeff)

    SIMPL_FILTER_PARAMETER(Float3rdOrderPoly_t, ThirdOrderBCoeff)
    Q_PROPERTY(Float3rdOrderPoly_t ThirdOrderBCoeff READ getThirdOrderBCoeff WRITE setThirdOrderBCoeff)

    SIMPL_FILTER_PARAMETER(Float4thOrderPoly_t, FourthOrderACoeff)
    Q_PROPERTY(Float4thOrderPoly_t FourthOrderACoeff READ getFourthOrderACoeff WRITE setFourthOrderACoeff)

    SIMPL_FILTER_PARAMETER(Float4thOrderPoly_t, FourthOrderBCoeff)
    Q_PROPERTY(Float4thOrderPoly_t FourthOrderBCoeff READ getFourthOrderBCoeff WRITE setFourthOrderBCoeff)

    SIMPL_FILTER_PARAMETER(bool, SaveAsNewDataContainer)
    Q_PROPERTY(bool SaveAsNewDataContainer READ getSaveAsNewDataContainer WRITE setSaveAsNewDataContainer)

    /**
     * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getCompiledLibraryName();

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
     * @brief writeFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

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
    WarpRegularGrid();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief determine_warped_coordinates Warps the supplied x and y coordinates by the supplied polynomial coefficients.
     * @param x Input x coordinate to be warped.
     * @param y Input y coordinate to be warped.
     * @param newX Output warped x coordinate.
     * @param newY Output warped y coordiante.
     */
    void determine_warped_coordinates(float x, float y, float& newX, float& newY);

  private:
    WarpRegularGrid(const WarpRegularGrid&); // Copy Constructor Not Implemented
    void operator=(const WarpRegularGrid&); // Operator '=' Not Implemented
};

#endif /* WarpRegularGrid_H_ */
