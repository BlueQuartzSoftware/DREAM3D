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

    DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellAttributeMatrixName)


    /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */
    DREAM3D_FILTER_PARAMETER(QString, StlFilePrefix)
    Q_PROPERTY(QString StlFilePrefix READ getStlFilePrefix WRITE setStlFilePrefix)
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


    DREAM3D_INSTANCE_PROPERTY(QVector<AxisAngleInput_t>, AxisAngleInputs)

    DREAM3D_INSTANCE_PROPERTY(QVector<QString>, StrVector)

    DREAM3D_INSTANCE_PROPERTY(QVector<int8_t>, Int8Array)
    DREAM3D_INSTANCE_PROPERTY(QVector<int16_t>, Int16Array)
    DREAM3D_INSTANCE_PROPERTY(QVector<int32_t>, Int32Array)
    DREAM3D_INSTANCE_PROPERTY(QVector<int64_t>, Int64Array)
    DREAM3D_INSTANCE_PROPERTY(QVector<uint8_t>, UInt8Array)
    DREAM3D_INSTANCE_PROPERTY(QVector<uint16_t>, UInt16Array)
    DREAM3D_INSTANCE_PROPERTY(QVector<uint32_t>, UInt32Array)
    DREAM3D_INSTANCE_PROPERTY(QVector<uint64_t>, UInt64Array)
    DREAM3D_INSTANCE_PROPERTY(QVector<float>, FloatArray)
    DREAM3D_INSTANCE_PROPERTY(QVector<double>, DoubleArray)

    DREAM3D_INSTANCE_PROPERTY(int8_t, Int8Value)
    DREAM3D_INSTANCE_PROPERTY(int16_t, Int16Value)
    DREAM3D_INSTANCE_PROPERTY(int32_t, Int32Value)
    DREAM3D_INSTANCE_PROPERTY(int64_t, Int64Value)
    DREAM3D_INSTANCE_PROPERTY(uint8_t, UInt8Value)
    DREAM3D_INSTANCE_PROPERTY(uint16_t, UInt16Value)
    DREAM3D_INSTANCE_PROPERTY(uint32_t, UInt32Value)
    DREAM3D_INSTANCE_PROPERTY(uint64_t, UInt64Value)
    DREAM3D_INSTANCE_PROPERTY(float, FloatValue)
    DREAM3D_INSTANCE_PROPERTY(double, DoubleValue)


    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedVolumeVertexArrayName)
    Q_PROPERTY(DataArrayPath SelectedVolumeVertexArrayName READ getSelectedVolumeVertexArrayName WRITE setSelectedVolumeVertexArrayName)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedVolumeEdgeArrayName)
    Q_PROPERTY(DataArrayPath SelectedVolumeEdgeArrayName READ getSelectedVolumeEdgeArrayName WRITE setSelectedVolumeEdgeArrayName)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedVolumeFaceArrayName)
    Q_PROPERTY(DataArrayPath SelectedVolumeFaceArrayName READ getSelectedVolumeFaceArrayName WRITE setSelectedVolumeFaceArrayName)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedVolumeCellArrayName)
    Q_PROPERTY(DataArrayPath SelectedVolumeCellArrayName READ getSelectedVolumeCellArrayName WRITE setSelectedVolumeCellArrayName)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedVolumeFeatureArrayName)
    Q_PROPERTY(DataArrayPath SelectedVolumeFeatureArrayName READ getSelectedVolumeFeatureArrayName WRITE setSelectedVolumeFeatureArrayName)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedVolumeEnsembleArrayName)
    Q_PROPERTY(DataArrayPath SelectedVolumeEnsembleArrayName READ getSelectedVolumeEnsembleArrayName WRITE setSelectedVolumeEnsembleArrayName)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedSurfaceVertexArrayName)
    Q_PROPERTY(DataArrayPath SelectedSurfaceVertexArrayName READ getSelectedSurfaceVertexArrayName WRITE setSelectedSurfaceVertexArrayName)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedSurfaceEdgeArrayName)
    Q_PROPERTY(DataArrayPath SelectedSurfaceEdgeArrayName READ getSelectedSurfaceEdgeArrayName WRITE setSelectedSurfaceEdgeArrayName)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedSurfaceFaceArrayName)
    Q_PROPERTY(DataArrayPath SelectedSurfaceFaceArrayName READ getSelectedSurfaceFaceArrayName WRITE setSelectedSurfaceFaceArrayName)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedSurfaceFeatureArrayName)
    Q_PROPERTY(DataArrayPath SelectedSurfaceFeatureArrayName READ getSelectedSurfaceFeatureArrayName WRITE setSelectedSurfaceFeatureArrayName)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedSurfaceEnsembleArrayName)
    Q_PROPERTY(DataArrayPath SelectedSurfaceEnsembleArrayName READ getSelectedSurfaceEnsembleArrayName WRITE setSelectedSurfaceEnsembleArrayName)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedEdgeVertexArrayName)
    Q_PROPERTY(DataArrayPath SelectedEdgeVertexArrayName READ getSelectedEdgeVertexArrayName WRITE setSelectedEdgeVertexArrayName)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedEdgeEdgeArrayName)
    Q_PROPERTY(DataArrayPath SelectedEdgeEdgeArrayName READ getSelectedEdgeEdgeArrayName WRITE setSelectedEdgeEdgeArrayName)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedEdgeFeatureArrayName)
    Q_PROPERTY(DataArrayPath SelectedEdgeFeatureArrayName READ getSelectedEdgeFeatureArrayName WRITE setSelectedEdgeFeatureArrayName)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedEdgeEnsembleArrayName)
    Q_PROPERTY(DataArrayPath SelectedEdgeEnsembleArrayName READ getSelectedEdgeEnsembleArrayName WRITE setSelectedEdgeEnsembleArrayName)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedVertexVertexArrayName)
    Q_PROPERTY(DataArrayPath SelectedVertexVertexArrayName READ getSelectedVertexVertexArrayName WRITE setSelectedVertexVertexArrayName)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedVertexFeatureArrayName)
    Q_PROPERTY(DataArrayPath SelectedVertexFeatureArrayName READ getSelectedVertexFeatureArrayName WRITE setSelectedVertexFeatureArrayName)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedVertexEnsembleArrayName)
    Q_PROPERTY(DataArrayPath SelectedVertexEnsembleArrayName READ getSelectedVertexEnsembleArrayName WRITE setSelectedVertexEnsembleArrayName)


    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellPatternQualityArrayPath)
    Q_PROPERTY(DataArrayPath CellPatternQualityArrayPath READ getCellPatternQualityArrayPath WRITE setCellPatternQualityArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, GoodVoxelsArrayPath)
    Q_PROPERTY(DataArrayPath GoodVoxelsArrayPath READ getGoodVoxelsArrayPath WRITE setGoodVoxelsArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, CellEulerAnglesArrayName)
    Q_PROPERTY(QString CellEulerAnglesArrayName READ getCellEulerAnglesArrayName WRITE setCellEulerAnglesArrayName)

    DREAM3D_FILTER_PARAMETER(QString, GoodVoxelsArrayName)
    Q_PROPERTY(QString GoodVoxelsArrayName READ getGoodVoxelsArrayName WRITE setGoodVoxelsArrayName)

    virtual const QString getCompiledLibraryName() { return Test::TestBaseName; }
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName() { return DREAM3D::FilterGroups::TestFilters; }

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel() { return "Generic Example"; }

    /**
    * @brief This returns a string that is displayed in the GUI and helps to sort the filters into
    * a subgroup. It should be readable and understandable by humans.
    */
    virtual const QString getSubGroupName() { return "Misc"; }

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

    // This is just here for testing purpose.
    void dataCheck2();

  private:
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, FeatureIds)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, CellPatternQuality)
    DEFINE_CREATED_DATAARRAY(float, CellEulerAngles)
    DEFINE_CREATED_DATAARRAY(bool, GoodVoxels)

    GenericExample(const GenericExample&); // Copy Constructor Not Implemented
    void operator=(const GenericExample&); // Operator '=' Not Implemented
};

#endif /* _GenericExample_H_ */



