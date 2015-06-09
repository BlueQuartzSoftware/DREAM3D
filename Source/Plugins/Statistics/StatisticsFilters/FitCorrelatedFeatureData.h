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


#ifndef _FitCorrelatedFeatureData_H_
#define _FitCorrelatedFeatureData_H_

#include <QtCore/QString>
#include <set>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "DREAM3DLib/DataArrays/StatsDataArray.h"

#include "DREAM3DLib/DataContainers/DataContainer.h"
#include "Statistics/DistributionAnalysisOps/DistributionAnalysisOps.h"

#include "Statistics/StatisticsConstants.h"
/**
 * @class FitCorrelatedFeatureData FitCorrelatedFeatureData.h /FilterCategoryFilters/FitCorrelatedFeatureData.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class FitCorrelatedFeatureData : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(FitCorrelatedFeatureData)
    DREAM3D_STATIC_NEW_MACRO(FitCorrelatedFeatureData)
    DREAM3D_TYPE_MACRO_SUPER(FitCorrelatedFeatureData, AbstractFilter)

    virtual ~FitCorrelatedFeatureData();
    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellEnsembleAttributeMatrixName)
    Q_PROPERTY(DataArrayPath CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedFeatureArrayPath)
    Q_PROPERTY(DataArrayPath SelectedFeatureArrayPath READ getSelectedFeatureArrayPath WRITE setSelectedFeatureArrayPath)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, CorrelatedFeatureArrayPath)
    Q_PROPERTY(DataArrayPath CorrelatedFeatureArrayPath READ getCorrelatedFeatureArrayPath WRITE setCorrelatedFeatureArrayPath)

    DREAM3D_FILTER_PARAMETER(unsigned int, DistributionType)
    Q_PROPERTY(unsigned int DistributionType READ getDistributionType WRITE setDistributionType)
    DREAM3D_FILTER_PARAMETER(int, NumberOfCorrelatedBins)
    Q_PROPERTY(int NumberOfCorrelatedBins READ getNumberOfCorrelatedBins WRITE setNumberOfCorrelatedBins)
    DREAM3D_FILTER_PARAMETER(bool, RemoveBiasedFeatures)
    Q_PROPERTY(bool RemoveBiasedFeatures READ getRemoveBiasedFeatures WRITE setRemoveBiasedFeatures)

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
    Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, BiasedFeaturesArrayPath)
    Q_PROPERTY(DataArrayPath BiasedFeaturesArrayPath READ getBiasedFeaturesArrayPath WRITE setBiasedFeaturesArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, NewEnsembleArrayArrayName)
    Q_PROPERTY(QString NewEnsembleArrayArrayName READ getNewEnsembleArrayArrayName WRITE setNewEnsembleArrayArrayName)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
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

    /**
    * @brief Reimplemented from @see AbstractFilter class
    */
    virtual void execute();

    /**
    * @brief This function runs some sanity checks on the DataContainer and inputs
    * in an attempt to ensure the filter can process the inputs.
    */
    virtual void preflight();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    FitCorrelatedFeatureData();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param features The number of features
    * @param ensembles The number of ensembles
    */
    void dataCheck();

  private:
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(bool, BiasedFeatures)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, NewEnsembleArray)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, FeaturePhases)

    FitCorrelatedFeatureData(const FitCorrelatedFeatureData&); // Copy Constructor Not Implemented
    void operator=(const FitCorrelatedFeatureData&); // Operator '=' Not Implemented
};

#endif /* FitCorrelatedFeatureData_H_ */





