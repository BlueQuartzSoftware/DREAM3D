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


#ifndef _InitializeSyntheticVolume_H_
#define _InitializeSyntheticVolume_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

/**
 * @brief The InitializeSyntheticVolume class. See [Filter documentation](@ref initializesyntheticvolume) for details.
 */
class InitializeSyntheticVolume : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(InitializeSyntheticVolume)
    DREAM3D_STATIC_NEW_MACRO(InitializeSyntheticVolume)
    DREAM3D_TYPE_MACRO_SUPER(InitializeSyntheticVolume, AbstractFilter)

    virtual ~InitializeSyntheticVolume();

    DREAM3D_FILTER_PARAMETER(QString, DataContainerName)
    Q_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)

    DREAM3D_FILTER_PARAMETER(QString, CellAttributeMatrixName)
    Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(IntVec3_t, Dimensions)
    Q_PROPERTY(IntVec3_t Dimensions READ getDimensions WRITE setDimensions)

    DREAM3D_FILTER_PARAMETER(FloatVec3_t, Resolution)
    Q_PROPERTY(FloatVec3_t Resolution READ getResolution WRITE setResolution)

    DREAM3D_FILTER_PARAMETER(FloatVec3_t, Origin)
    Q_PROPERTY(FloatVec3_t Origin READ getOrigin WRITE setOrigin)

    // These are for estimating the number of features that will be generated farther down the line.

    DREAM3D_FILTER_PARAMETER(DataArrayPath, InputStatsArrayPath)
    Q_PROPERTY(DataArrayPath InputStatsArrayPath READ getInputStatsArrayPath WRITE setInputStatsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, InputPhaseTypesArrayPath)
    Q_PROPERTY(DataArrayPath InputPhaseTypesArrayPath READ getInputPhaseTypesArrayPath WRITE setInputPhaseTypesArrayPath)


    DREAM3D_FILTER_PARAMETER(bool, EstimateNumberOfFeatures)
    Q_PROPERTY(bool EstimateNumberOfFeatures READ getEstimateNumberOfFeatures WRITE setEstimateNumberOfFeatures)

    DREAM3D_FILTER_PARAMETER(QString, InputStatsFile)
    Q_PROPERTY(QString InputStatsFile READ getInputStatsFile WRITE setInputStatsFile)

    int getEstimatedPrimaryFeatures();
    Q_PROPERTY(QVariant EstimatedPrimaryFeatures READ getEstimatedPrimaryFeatures)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();
    virtual const QString getHumanLabel();

    /**
     * @brief setupFilterParameters
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

    virtual void preflight();

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    InitializeSyntheticVolume();

    /**
     * @brief estimateNumFeatures Estimates the number of Features that will be generated based on the supplied statistics
     * @param dimensions Vector containing extents of the volume dimensions
     * @param res Vector containing resolution of the volume
     * @return Integer number of estimated features
     */
    int estimateNumFeatures(IntVec3_t dimensions, FloatVec3_t res);

  private:
    int32_t m_EstimatedPrimaryFeatures;

    void dataCheck();

    InitializeSyntheticVolume(const InitializeSyntheticVolume&); // Copy Constructor Not Implemented
    void operator=(const InitializeSyntheticVolume&); // Operator '=' Not Implemented
};

#endif /* _InitializeSyntheticVolume_H_ */



