/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _GenericExample_H_
#define _GenericExample_H_

#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/DataContainers/DataArrayPathBundle.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"


/**
 * @class GenericExample GenericExample.h ExamplePlugin/Code/ExamplePluginFilters/GenericExample.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class DREAM3DLib_EXPORT GenericExample : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(GenericExample)
    DREAM3D_STATIC_NEW_MACRO(GenericExample)
    DREAM3D_TYPE_MACRO_SUPER(GenericExample, AbstractFilter)

    virtual ~GenericExample();

    /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */
    DREAM3D_FILTER_PARAMETER(QString, StlFilePrefix)
    Q_PROPERTY(QString StlFilePrefix READ getStlFilePrefix WRITE setStlFilePrefix)

    DREAM3D_FILTER_PARAMETER(bool, ShowPrefix)
    Q_PROPERTY(bool ShowPrefix READ getShowPrefix WRITE setShowPrefix)

    DREAM3D_FILTER_PARAMETER(int, MaxIterations)
    Q_PROPERTY(int MaxIterations READ getMaxIterations WRITE setMaxIterations)

    DREAM3D_FILTER_PARAMETER(double, MisorientationTolerance)
    Q_PROPERTY(double MisorientationTolerance READ getMisorientationTolerance WRITE setMisorientationTolerance)

    DREAM3D_FILTER_PARAMETER(QString, InputFile)
    Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

    DREAM3D_FILTER_PARAMETER(QString, InputPath)
    Q_PROPERTY(QString InputPath READ getInputPath WRITE setInputPath)

    DREAM3D_FILTER_PARAMETER(QString, OutputFile)
    Q_PROPERTY(QString OutputFile READ getOutputFile WRITE setOutputFile)

    DREAM3D_FILTER_PARAMETER(QString, OutputPath)
    Q_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)

	DREAM3D_FILTER_PARAMETER(DataArrayPathBundle, MultiArraySelections)
	Q_PROPERTY(DataArrayPathBundle MultiArraySelections READ getMultiArraySelections WRITE setMultiArraySelections)

    DREAM3D_FILTER_PARAMETER(bool, WriteAlignmentShifts)
    Q_PROPERTY(bool WriteAlignmentShifts READ getWriteAlignmentShifts WRITE setWriteAlignmentShifts)

    DREAM3D_FILTER_PARAMETER(int, ConversionType)
    Q_PROPERTY(int ConversionType READ getConversionType WRITE setConversionType)


    DREAM3D_FILTER_PARAMETER(IntVec3_t, Dimensions)
    Q_PROPERTY(IntVec3_t Dimensions READ getDimensions WRITE setDimensions)

    DREAM3D_FILTER_PARAMETER(FloatVec3_t, Origin)
    Q_PROPERTY(FloatVec3_t Origin READ getOrigin WRITE setOrigin)

    DREAM3D_FILTER_PARAMETER(AxisAngleInput_t, CrystalSymmetryRotations)
    Q_PROPERTY(AxisAngleInput_t CrystalSymmetryRotations READ getCrystalSymmetryRotations WRITE setCrystalSymmetryRotations)


    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, AttributeMatrixPath)
    Q_PROPERTY(DataArrayPath AttributeMatrixPath READ getAttributeMatrixPath WRITE setAttributeMatrixPath)

    DREAM3D_FILTER_PARAMETER(QString, DataContainerName)
    Q_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CreatedDataArray)
    Q_PROPERTY(DataArrayPath CreatedDataArray READ getCreatedDataArray WRITE setCreatedDataArray)

    DREAM3D_FILTER_PARAMETER(bool, Bool1)
    Q_PROPERTY(bool Bool1 READ getBool1 WRITE setBool1)

    DREAM3D_FILTER_PARAMETER(bool, Bool2)
    Q_PROPERTY(bool Bool2 READ getBool2 WRITE setBool2)

    DREAM3D_FILTER_PARAMETER(int, AlgorithmSelection)
    Q_PROPERTY(int AlgorithmSelection READ getAlgorithmSelection WRITE setAlgorithmSelection)

    DREAM3D_FILTER_PARAMETER(int, DistanceMetric)
    Q_PROPERTY(int DistanceMetric READ getDistanceMetric WRITE setDistanceMetric)



    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel();

    /**
    * @brief This returns a string that is displayed in the GUI and helps to sort the filters into
    * a subgroup. It should be readable and understandable by humans.
    */
    virtual const QString getSubGroupName();

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
    GenericExample();

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

    GenericExample(const GenericExample&); // Copy Constructor Not Implemented
    void operator=(const GenericExample&); // Operator '=' Not Implemented
};

#endif /* _GenericExample_H_ */



