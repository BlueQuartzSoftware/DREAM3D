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

#ifndef CorrelateValuesWithVectorDirection_H_
#define CorrelateValuesWithVectorDirection_H_

#include <vector>
#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/DataContainers/DataContainer.h"

#include "DREAM3DLib/Utilities/DREAM3DEndian.h"
#include "OrientationLib/SpaceGroupOps/SpaceGroupOps.h"

#include "Statistics/StatisticsConstants.h"
/**
 * @class CorrelateValuesWithVectorDirection CorrelateValuesWithVectorDirection.h DREAM3DLib/GenericFilters/CorrelateValuesWithVectorDirection.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class CorrelateValuesWithVectorDirection : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(CorrelateValuesWithVectorDirection)
    DREAM3D_STATIC_NEW_MACRO(CorrelateValuesWithVectorDirection)
    DREAM3D_TYPE_MACRO_SUPER(CorrelateValuesWithVectorDirection, AbstractFilter)

    virtual ~CorrelateValuesWithVectorDirection();

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CorrelatedDataArrayPath)
    Q_PROPERTY(DataArrayPath CorrelatedDataArrayPath READ getCorrelatedDataArrayPath WRITE setCorrelatedDataArrayPath)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, VectorDataArrayPath)
    Q_PROPERTY(DataArrayPath VectorDataArrayPath READ getVectorDataArrayPath WRITE setVectorDataArrayPath)

    DREAM3D_INSTANCE_STRING_PROPERTY(Logfile)
    /**
    * @brief This returns the group that the filter belongs to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel();

    virtual void setupFilterParameters();

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

    DoubleArrayType::Pointer getFitLambertProjection();

signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    CorrelateValuesWithVectorDirection();

    void makeLambertProjection(size_t numComps);
    int determineSquareCoordsandBin(float xyz[3]);
    void determineXYZCoords(float sqCoords[2], float xyz[3]);
    void writeLambertProjection(size_t numComps);
    void writePFStats(size_t numComps);
    void createSterographicProjections(size_t numComps);

    void dataCheck();

  private:
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, VectorData)

    DoubleArrayType::Pointer m_LambertProj;

    float m_MaxCoord;
    int m_Dimension;
    float m_StepSize;

    /**
     * @brief This function writes a set of Axis coordinates to that are needed
     * for a Rectilinear Grid based data set.
     * @param f The "C" FILE* pointer to the file being written to.
     * @param axis The name of the Axis that is being written
     * @param type The type of primitive being written (float, int, ...)
     * @param npoints The total number of points in the array
     * @param min The minimum value of the axis
     * @param max The maximum value of the axis
     * @param step The step value between each point on the axis.
     */
    int writeCoords(FILE* f, const char* axis, const char* type, int64_t npoints, float min, float step);


    CorrelateValuesWithVectorDirection(const CorrelateValuesWithVectorDirection&); // Copy Constructor Not Implemented
    void operator=(const CorrelateValuesWithVectorDirection&); // Operator '=' Not Implemented
};

#endif /* CorrelateValuesWithVectorDirection_H_ */



