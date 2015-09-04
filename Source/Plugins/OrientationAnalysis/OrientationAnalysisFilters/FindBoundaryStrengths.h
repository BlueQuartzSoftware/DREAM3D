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


#ifndef _FindBoundaryStrengths_H_
#define _FindBoundaryStrengths_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"

#include "OrientationLib/SpaceGroupOps/SpaceGroupOps.h"

/**
 * @brief The FindBoundaryStrengths class. See [Filter documentation](@ref findboundarystrengths) for details.
 */
class FindBoundaryStrengths : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    SIMPL_SHARED_POINTERS(FindBoundaryStrengths)
    SIMPL_STATIC_NEW_MACRO(FindBoundaryStrengths)
    SIMPL_TYPE_MACRO_SUPER(FindBoundaryStrengths, AbstractFilter)

    virtual ~FindBoundaryStrengths();

    SIMPL_FILTER_PARAMETER(FloatVec3_t, Loading)
    Q_PROPERTY(FloatVec3_t Loading READ getLoading WRITE setLoading)

    SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, AvgQuatsArrayPath)
    Q_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
    Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

    SIMPL_FILTER_PARAMETER(QString, SurfaceMeshF1sArrayName)
    Q_PROPERTY(QString SurfaceMeshF1sArrayName READ getSurfaceMeshF1sArrayName WRITE setSurfaceMeshF1sArrayName)

    SIMPL_FILTER_PARAMETER(QString, SurfaceMeshF1sptsArrayName)
    Q_PROPERTY(QString SurfaceMeshF1sptsArrayName READ getSurfaceMeshF1sptsArrayName WRITE setSurfaceMeshF1sptsArrayName)

    SIMPL_FILTER_PARAMETER(QString, SurfaceMeshF7sArrayName)
    Q_PROPERTY(QString SurfaceMeshF7sArrayName READ getSurfaceMeshF7sArrayName WRITE setSurfaceMeshF7sArrayName)

    SIMPL_FILTER_PARAMETER(QString, SurfaceMeshmPrimesArrayName)
    Q_PROPERTY(QString SurfaceMeshmPrimesArrayName READ getSurfaceMeshmPrimesArrayName WRITE setSurfaceMeshmPrimesArrayName)

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
    FindBoundaryStrengths();

  private:
    QVector<SpaceGroupOps::Pointer> m_OrientationOps;

    DEFINE_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    DEFINE_DATAARRAY_VARIABLE(float, AvgQuats)
    DEFINE_DATAARRAY_VARIABLE(unsigned int, CrystalStructures)
    DEFINE_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFaceLabels)

    DEFINE_DATAARRAY_VARIABLE(float, SurfaceMeshF1s)
    DEFINE_DATAARRAY_VARIABLE(float, SurfaceMeshF1spts)
    DEFINE_DATAARRAY_VARIABLE(float, SurfaceMeshF7s)
    DEFINE_DATAARRAY_VARIABLE(float, SurfaceMeshmPrimes)

    /**
     * @brief dataCheckVoxel Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheckVoxel();

    /**
     * @brief dataCheckSurfaceMesh Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheckSurfaceMesh();

    FindBoundaryStrengths(const FindBoundaryStrengths&); // Copy Constructor Not Implemented
    void operator=(const FindBoundaryStrengths&); // Operator '=' Not Implemented
};

#endif /* FindBoundaryStrengths_H_ */
