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

#ifndef _NeighborOrientationCorrelation_H_
#define _NeighborOrientationCorrelation_H_

#include <vector>
#include <QtCore/QString>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/DataContainers/DataContainer.h"

#include "OrientationLib/OrientationOps/OrientationOps.h"



/**
 * @class NeighborOrientationCorrelation NeighborOrientationCorrelation.h DREAM3DLib/ReconstructionFilters/NeighborOrientationCorrelation.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class NeighborOrientationCorrelation : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(NeighborOrientationCorrelation)
    DREAM3D_STATIC_NEW_MACRO(NeighborOrientationCorrelation)
    DREAM3D_TYPE_MACRO_SUPER(NeighborOrientationCorrelation, AbstractFilter)

    virtual ~NeighborOrientationCorrelation();

    DREAM3D_FILTER_PARAMETER(float, MisorientationTolerance)
    Q_PROPERTY(float MisorientationTolerance READ getMisorientationTolerance WRITE setMisorientationTolerance)
    DREAM3D_FILTER_PARAMETER(float, MinConfidence)
    Q_PROPERTY(float MinConfidence READ getMinConfidence WRITE setMinConfidence)
    DREAM3D_FILTER_PARAMETER(int, Level)
    Q_PROPERTY(int Level READ getLevel WRITE setLevel)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, ConfidenceIndexArrayPath)
    Q_PROPERTY(DataArrayPath ConfidenceIndexArrayPath READ getConfidenceIndexArrayPath WRITE setConfidenceIndexArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
    Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, QuatsArrayPath)
    Q_PROPERTY(DataArrayPath QuatsArrayPath READ getQuatsArrayPath WRITE setQuatsArrayPath)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName()  { return DREAM3D::FilterSubGroups::CleanupFilters; }
    virtual const QString getHumanLabel();

    virtual void setupFilterParameters();
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
    NeighborOrientationCorrelation();

  private:
    QVector<OrientationOps::Pointer> m_OrientationOps;

    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, ConfidenceIndex)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, Quats)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, CellPhases)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(unsigned int, CrystalStructures)

    void dataCheck();

    NeighborOrientationCorrelation(const NeighborOrientationCorrelation&); // Copy Constructor Not Implemented
    void operator=(const NeighborOrientationCorrelation&); // Operator '=' Not Implemented
};

#endif /* NeighborOrientationCorrelation_H_ */



