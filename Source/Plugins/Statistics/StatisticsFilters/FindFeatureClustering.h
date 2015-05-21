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


#ifndef _FindFeatureClustering_H_
#define _FindFeatureClustering_H_

#include <vector>
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"
#include "DREAM3DLib/DataContainers/DataContainer.h"

#include "Statistics/StatisticsConstants.h"
/**
 * @class FindFeatureClustering FindFeatureClustering.h DREAM3DLib/GenericFilters/FindFeatureClustering.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class FindFeatureClustering : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(FindFeatureClustering)
    DREAM3D_STATIC_NEW_MACRO(FindFeatureClustering)
    DREAM3D_TYPE_MACRO_SUPER(FindFeatureClustering, AbstractFilter)

    virtual ~FindFeatureClustering();

    DREAM3D_INSTANCE_STRING_PROPERTY(ErrorOutputFile)

    DREAM3D_FILTER_PARAMETER(int, NumberOfBins)
    Q_PROPERTY(int NumberOfBins READ getNumberOfBins WRITE setNumberOfBins)

    DREAM3D_FILTER_PARAMETER(int, PhaseNumber)
    Q_PROPERTY(int PhaseNumber READ getPhaseNumber WRITE setPhaseNumber)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedFeatureArrayPath)
    Q_PROPERTY(DataArrayPath SelectedFeatureArrayPath READ getSelectedFeatureArrayPath WRITE setSelectedFeatureArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellEnsembleAttributeMatrixName)
    Q_PROPERTY(DataArrayPath CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(bool, RemoveBiasedFeatures)
    Q_PROPERTY(bool RemoveBiasedFeatures READ getRemoveBiasedFeatures WRITE setRemoveBiasedFeatures)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, BiasedFeaturesArrayPath)
    Q_PROPERTY(DataArrayPath BiasedFeaturesArrayPath READ getBiasedFeaturesArrayPath WRITE setBiasedFeaturesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, EquivalentDiametersArrayPath)
    Q_PROPERTY(DataArrayPath EquivalentDiametersArrayPath READ getEquivalentDiametersArrayPath WRITE setEquivalentDiametersArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
    Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CentroidsArrayPath)
    Q_PROPERTY(DataArrayPath CentroidsArrayPath READ getCentroidsArrayPath WRITE setCentroidsArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, ClusteringListArrayName)
    Q_PROPERTY(QString ClusteringListArrayName READ getClusteringListArrayName WRITE setClusteringListArrayName)

    DREAM3D_FILTER_PARAMETER(QString, NewEnsembleArrayArrayName)
    Q_PROPERTY(QString NewEnsembleArrayArrayName READ getNewEnsembleArrayArrayName WRITE setNewEnsembleArrayArrayName)

    DREAM3D_FILTER_PARAMETER(QString, MaxMinArrayName)
    Q_PROPERTY(QString MaxMinArrayName READ getMaxMinArrayName WRITE setMaxMinArrayName)

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
    FindFeatureClustering();

    void find_clustering();

  private:
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, Centroids)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, EquivalentDiameters)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, NewEnsembleArray)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, MaxMinArray)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(bool, BiasedFeatures)
    NeighborList<float>::WeakPointer m_ClusteringList;

    std::vector<float> m_RandomCentroids;

    void dataCheck();

    FindFeatureClustering(const FindFeatureClustering&); // Copy Constructor Not Implemented
    void operator=(const FindFeatureClustering&); // Operator '=' Not Implemented
};


#endif /* FINDFIELDCLUSTERING_H_ */



