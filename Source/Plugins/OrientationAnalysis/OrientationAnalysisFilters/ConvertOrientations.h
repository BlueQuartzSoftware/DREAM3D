/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _ConvertOrientations_H_
#define _ConvertOrientations_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

#include "OrientationLib/OrientationOps/OrientationOps.h"

/**
 * @brief The ConvertOrientations class. See Filter documentation for details.
 */
class  ConvertOrientations : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(ConvertOrientations)
    DREAM3D_STATIC_NEW_MACRO(ConvertOrientations)
    DREAM3D_TYPE_MACRO_SUPER(ConvertOrientations, AbstractFilter)

    virtual ~ConvertOrientations();

    DREAM3D_FILTER_PARAMETER(int, InputType)
    Q_PROPERTY(int InputType READ getInputType WRITE setInputType)

    DREAM3D_FILTER_PARAMETER(int, OutputType)
    Q_PROPERTY(int OutputType READ getOutputType WRITE setOutputType)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellEulerAnglesArrayPath)
    Q_PROPERTY(DataArrayPath CellEulerAnglesArrayPath READ getCellEulerAnglesArrayPath WRITE setCellEulerAnglesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellQuatsArrayPath)
    Q_PROPERTY(DataArrayPath CellQuatsArrayPath READ getCellQuatsArrayPath WRITE setCellQuatsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellRodriguesVectorsArrayPath)
    Q_PROPERTY(DataArrayPath CellRodriguesVectorsArrayPath READ getCellRodriguesVectorsArrayPath WRITE setCellRodriguesVectorsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellAxisAnglesArrayPath)
    Q_PROPERTY(DataArrayPath CellAxisAnglesArrayPath READ getCellAxisAnglesArrayPath WRITE setCellAxisAnglesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
    Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, EulerAnglesArrayName)
    Q_PROPERTY(QString EulerAnglesArrayName READ getEulerAnglesArrayName WRITE setEulerAnglesArrayName)

    DREAM3D_FILTER_PARAMETER(QString, QuatsArrayName)
    Q_PROPERTY(QString QuatsArrayName READ getQuatsArrayName WRITE setQuatsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, RodriguesVectorsArrayName)
    Q_PROPERTY(QString RodriguesVectorsArrayName READ getRodriguesVectorsArrayName WRITE setRodriguesVectorsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, AxisAnglesArrayName)
    Q_PROPERTY(QString AxisAnglesArrayName READ getAxisAnglesArrayName WRITE setAxisAnglesArrayName)

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
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    virtual void execute();
    virtual void preflight();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    ConvertOrientations();

  private:
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, CellPhases)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, CellEulerAngles)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, CellQuats)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, CellRodriguesVectors)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, CellAxisAngles)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(uint32_t, CrystalStructures)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, EulerAngles)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, Quats)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, RodriguesVectors)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, AxisAngles)

    QVector<OrientationOps::Pointer> m_OrientationOps;

    void dataCheck();

    ConvertOrientations(const ConvertOrientations&); // Copy Constructor Not Implemented
    void operator=(const ConvertOrientations&); // Operator '=' Not Implemented
};

#endif /* ConvertOrientations_H_ */
