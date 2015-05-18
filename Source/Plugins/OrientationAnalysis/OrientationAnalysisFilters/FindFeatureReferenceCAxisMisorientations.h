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


#ifndef _FindFeatureReferenceCAxisMisorientations_H_
#define _FindFeatureReferenceCAxisMisorientations_H_

#include <vector>
#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/DataContainers/DataContainer.h"

#include "OrientationLib/SpaceGroupOps/CubicOps.h"
#include "OrientationLib/SpaceGroupOps/HexagonalOps.h"
#include "OrientationLib/SpaceGroupOps/SpaceGroupOps.h"
#include "OrientationLib/SpaceGroupOps/OrthoRhombicOps.h"

/**
 * @class FindFeatureReferenceCAxisMisorientations FindFeatureReferenceCAxisMisorientations.h DREAM3DLib/GenericFilters/FindFeatureReferenceCAxisMisorientations.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class FindFeatureReferenceCAxisMisorientations : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(FindFeatureReferenceCAxisMisorientations)
    DREAM3D_STATIC_NEW_MACRO(FindFeatureReferenceCAxisMisorientations)
    DREAM3D_TYPE_MACRO_SUPER(FindFeatureReferenceCAxisMisorientations, AbstractFilter)

    virtual ~FindFeatureReferenceCAxisMisorientations();

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
    Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, AvgCAxesArrayPath)
    Q_PROPERTY(DataArrayPath AvgCAxesArrayPath READ getAvgCAxesArrayPath WRITE setAvgCAxesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, QuatsArrayPath)
    Q_PROPERTY(DataArrayPath QuatsArrayPath READ getQuatsArrayPath WRITE setQuatsArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, FeatureAvgCAxisMisorientationsArrayName)
    Q_PROPERTY(QString FeatureAvgCAxisMisorientationsArrayName READ getFeatureAvgCAxisMisorientationsArrayName WRITE setFeatureAvgCAxisMisorientationsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, FeatureStdevCAxisMisorientationsArrayName)
    Q_PROPERTY(QString FeatureStdevCAxisMisorientationsArrayName READ getFeatureStdevCAxisMisorientationsArrayName WRITE setFeatureStdevCAxisMisorientationsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, FeatureReferenceCAxisMisorientationsArrayName)
    Q_PROPERTY(QString FeatureReferenceCAxisMisorientationsArrayName READ getFeatureReferenceCAxisMisorientationsArrayName WRITE setFeatureReferenceCAxisMisorientationsArrayName)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();
    virtual const QString getHumanLabel();

    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
    */
    virtual void setupFilterParameters();

    /**
     * @brief writeFilterParameters
     * @param writer
     * @param index
     * @return
     */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();
    virtual void preflight();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:

    FindFeatureReferenceCAxisMisorientations();

  private:
    QVector<SpaceGroupOps::Pointer> m_OrientationOps;
    CubicOps::Pointer m_CubicOps;
    HexagonalOps::Pointer m_HexOps;
    OrthoRhombicOps::Pointer m_OrthoOps;

    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, FeatureIds)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, CellPhases)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, Quats)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, AvgCAxes)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, FeatureReferenceCAxisMisorientations)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, FeatureAvgCAxisMisorientations)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, FeatureStdevCAxisMisorientations)

    void dataCheck();

    FindFeatureReferenceCAxisMisorientations(const FindFeatureReferenceCAxisMisorientations&); // Copy Constructor Not Implemented
    void operator=(const FindFeatureReferenceCAxisMisorientations&); // Operator '=' Not Implemented
};

#endif /* FindFeatureReferenceCAxisMisorientations_H_ */



