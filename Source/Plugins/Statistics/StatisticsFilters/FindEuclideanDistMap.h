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

#ifndef _FindEuclideanDistMap_H_
#define _FindEuclideanDistMap_H_

#include <vector>
#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/DataContainers/DataContainer.h"

#include "Statistics/StatisticsConstants.h"
/**
 * @class FindEuclideanDistMap FindEuclideanDistMap.h DREAM3DLib/GenericFilters/FindEuclideanDistMap.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class FindEuclideanDistMap : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(FindEuclideanDistMap)
    DREAM3D_STATIC_NEW_MACRO(FindEuclideanDistMap)
    DREAM3D_TYPE_MACRO_SUPER(FindEuclideanDistMap, AbstractFilter)

    virtual ~FindEuclideanDistMap();

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, GBEuclideanDistancesArrayName)
    Q_PROPERTY(QString GBEuclideanDistancesArrayName READ getGBEuclideanDistancesArrayName WRITE setGBEuclideanDistancesArrayName)

    DREAM3D_FILTER_PARAMETER(QString, TJEuclideanDistancesArrayName)
    Q_PROPERTY(QString TJEuclideanDistancesArrayName READ getTJEuclideanDistancesArrayName WRITE setTJEuclideanDistancesArrayName)

    DREAM3D_FILTER_PARAMETER(QString, QPEuclideanDistancesArrayName)
    Q_PROPERTY(QString QPEuclideanDistancesArrayName READ getQPEuclideanDistancesArrayName WRITE setQPEuclideanDistancesArrayName)

    DREAM3D_FILTER_PARAMETER(QString, NearestNeighborsArrayName)
    Q_PROPERTY(QString NearestNeighborsArrayName READ getNearestNeighborsArrayName WRITE setNearestNeighborsArrayName)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();
    virtual const QString getHumanLabel();

    DREAM3D_FILTER_PARAMETER(bool, DoBoundaries)
    Q_PROPERTY(bool DoBoundaries READ getDoBoundaries WRITE setDoBoundaries)
    DREAM3D_FILTER_PARAMETER(bool, DoTripleLines)
    Q_PROPERTY(bool DoTripleLines READ getDoTripleLines WRITE setDoTripleLines)
    DREAM3D_FILTER_PARAMETER(bool, DoQuadPoints)
    Q_PROPERTY(bool DoQuadPoints READ getDoQuadPoints WRITE setDoQuadPoints)
    DREAM3D_FILTER_PARAMETER(bool, SaveNearestNeighbors)
    Q_PROPERTY(bool SaveNearestNeighbors READ getSaveNearestNeighbors WRITE setSaveNearestNeighbors)
    DREAM3D_FILTER_PARAMETER(bool, CalcOnlyManhattanDist)
    Q_PROPERTY(bool CalcOnlyManhattanDist READ getCalcOnlyManhattanDist WRITE setCalcOnlyManhattanDist)

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
    FindEuclideanDistMap();

    void find_euclideandistmap();

  private:
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, FeatureIds)
    DEFINE_CREATED_DATAARRAY_VARIABLE(int32_t, NearestNeighbors)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, GBEuclideanDistances)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, TJEuclideanDistances)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, QPEuclideanDistances)

    void dataCheck();

    FindEuclideanDistMap(const FindEuclideanDistMap&); // Copy Constructor Not Implemented
    void operator=(const FindEuclideanDistMap&); // Operator '=' Not Implemented
};

#endif /* FINDEUCLIDEANDISTMAP_H_ */



