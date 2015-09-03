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


#ifndef _FindSchmids_H_
#define _FindSchmids_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"

#include "OrientationLib/SpaceGroupOps/SpaceGroupOps.h"

/**
 * @brief The FindSchmids class. See [Filter documentation](@ref findschmids) for details.
 */
class FindSchmids : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    SIMPL_SHARED_POINTERS(FindSchmids)
    SIMPL_STATIC_NEW_MACRO(FindSchmids)
    SIMPL_TYPE_MACRO_SUPER(FindSchmids, AbstractFilter)

    virtual ~FindSchmids();

    SIMPL_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
    Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, AvgQuatsArrayPath)
    Q_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)

    SIMPL_FILTER_PARAMETER(QString, SchmidsArrayName)
    Q_PROPERTY(QString SchmidsArrayName READ getSchmidsArrayName WRITE setSchmidsArrayName)

    SIMPL_FILTER_PARAMETER(QString, SlipSystemsArrayName)
    Q_PROPERTY(QString SlipSystemsArrayName READ getSlipSystemsArrayName WRITE setSlipSystemsArrayName)

    SIMPL_FILTER_PARAMETER(QString, PolesArrayName)
    Q_PROPERTY(QString PolesArrayName READ getPolesArrayName WRITE setPolesArrayName)

    SIMPL_FILTER_PARAMETER(QString, PhisArrayName)
    Q_PROPERTY(QString PhisArrayName READ getPhisArrayName WRITE setPhisArrayName)

    SIMPL_FILTER_PARAMETER(QString, LambdasArrayName)
    Q_PROPERTY(QString LambdasArrayName READ getLambdasArrayName WRITE setLambdasArrayName)

    SIMPL_FILTER_PARAMETER(FloatVec3_t, LoadingDirection)
    Q_PROPERTY(FloatVec3_t LoadingDirection READ getLoadingDirection WRITE setLoadingDirection)

    SIMPL_FILTER_PARAMETER(bool, StoreAngleComponents)
    Q_PROPERTY(bool StoreAngleComponents READ getStoreAngleComponents WRITE setStoreAngleComponents)

    SIMPL_FILTER_PARAMETER(bool, OverrideSystem)
    Q_PROPERTY(bool OverrideSystem READ getOverrideSystem WRITE setOverrideSystem)

    SIMPL_FILTER_PARAMETER(FloatVec3_t, SlipPlane)
    Q_PROPERTY(FloatVec3_t SlipPlane READ getSlipPlane WRITE setSlipPlane)

    SIMPL_FILTER_PARAMETER(FloatVec3_t, SlipDirection)
    Q_PROPERTY(FloatVec3_t SlipDirection READ getSlipDirection WRITE setSlipDirection)

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
    FindSchmids();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

  private:
    QVector<SpaceGroupOps::Pointer> m_OrientationOps;

    DEFINE_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    DEFINE_DATAARRAY_VARIABLE(float, AvgQuats)
    DEFINE_DATAARRAY_VARIABLE(unsigned int, CrystalStructures)

    DEFINE_DATAARRAY_VARIABLE(float, Schmids)
    DEFINE_DATAARRAY_VARIABLE(float, Phis)
    DEFINE_DATAARRAY_VARIABLE(float, Lambdas)
    DEFINE_DATAARRAY_VARIABLE(int32_t, Poles)
    DEFINE_DATAARRAY_VARIABLE(int32_t, SlipSystems)

    FindSchmids(const FindSchmids&); // Copy Constructor Not Implemented
    void operator=(const FindSchmids&); // Operator '=' Not Implemented
};

#endif /* FindSchmids_H_ */
