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


#ifndef CorrelateValuesWithVectorDirection_H_
#define CorrelateValuesWithVectorDirection_H_

#include <vector>
#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "SIMPLib/Utilities/SIMPLibEndian.h"
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
    SIMPL_SHARED_POINTERS(CorrelateValuesWithVectorDirection)
    SIMPL_STATIC_NEW_MACRO(CorrelateValuesWithVectorDirection)
    SIMPL_TYPE_MACRO_SUPER(CorrelateValuesWithVectorDirection, AbstractFilter)

    virtual ~CorrelateValuesWithVectorDirection();

    SIMPL_FILTER_PARAMETER(DataArrayPath, CorrelatedDataArrayPath)
    Q_PROPERTY(DataArrayPath CorrelatedDataArrayPath READ getCorrelatedDataArrayPath WRITE setCorrelatedDataArrayPath)
    SIMPL_FILTER_PARAMETER(DataArrayPath, VectorDataArrayPath)
    Q_PROPERTY(DataArrayPath VectorDataArrayPath READ getVectorDataArrayPath WRITE setVectorDataArrayPath)

    SIMPL_INSTANCE_STRING_PROPERTY(Logfile)

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
    CorrelateValuesWithVectorDirection();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    void makeLambertProjection(size_t numComps);
    int determineSquareCoordsandBin(float xyz[3]);
    void determineXYZCoords(float sqCoords[2], float xyz[3]);
    void writeLambertProjection(size_t numComps);
    void writePFStats(size_t numComps);
    void createSterographicProjections(size_t numComps);

  private:
    DEFINE_DATAARRAY_VARIABLE(float, VectorData)

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
