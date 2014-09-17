/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#ifndef _LoadAdditiveMonitoringData_H_
#define _LoadAdditiveMonitoringData_H_



#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif


#include <QtCore/QVector>
#include <QtCore/QString>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/Observer.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "IO/IOConstants.h"
/**
 * @class LoadAdditiveMonitoringData LoadAdditiveMonitoringData.h LoadAdditiveMonitoringData/LoadAdditiveMonitoringData.h
 * @brief This class is used to convert Hex grid TSL .ang files into Square grid
 * .ang files
 * @author Michael A. Jackson for BlueQuartz Software
 * @author Dr. Michael Groeber, US Air Force Research Laboratories
 * @date July 23, 2012
 * @version 1.2
 *
 */
class LoadAdditiveMonitoringData : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(LoadAdditiveMonitoringData)
    DREAM3D_STATIC_NEW_MACRO(LoadAdditiveMonitoringData)
    DREAM3D_TYPE_MACRO_SUPER(LoadAdditiveMonitoringData, AbstractFilter)

    virtual ~LoadAdditiveMonitoringData();

    DREAM3D_FILTER_PARAMETER(QString, VolumeDataContainerName)
    Q_PROPERTY(QString VolumeDataContainerName READ getVolumeDataContainerName WRITE setVolumeDataContainerName)
    DREAM3D_FILTER_PARAMETER(QString, CellAttributeMatrixName)
    Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
    DREAM3D_FILTER_PARAMETER(QString, CurrentArrayName)
    Q_PROPERTY(QString CurrentArrayName READ getCurrentArrayName WRITE setCurrentArrayName)
    DREAM3D_FILTER_PARAMETER(QString, SpeedArrayName)
    Q_PROPERTY(QString SpeedArrayName READ getSpeedArrayName WRITE setSpeedArrayName)

    DREAM3D_INSTANCE_PROPERTY(int64_t, ZStartIndex)
    DREAM3D_INSTANCE_PROPERTY(int64_t, ZEndIndex)

    DREAM3D_FILTER_PARAMETER(FloatVec3_t, Resolution)
    DREAM3D_FILTER_PARAMETER(FloatVec3_t, Origin)

    DREAM3D_FILTER_PARAMETER(QString, InputPath)
    DREAM3D_FILTER_PARAMETER(QString, FilePrefix)
    DREAM3D_FILTER_PARAMETER(QString, FileSuffix)
    DREAM3D_FILTER_PARAMETER(QString, FileExtension)
    DREAM3D_FILTER_PARAMETER(int, PaddingDigits)
    DREAM3D_FILTER_PARAMETER(uint32_t, RefFrameZDir)

    DREAM3D_FILTER_PARAMETER(int, Stack)
    Q_PROPERTY(int Stack READ getStack WRITE setStack)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName()  { return DREAM3D::FilterSubGroups::ResolutionFilters; }
    virtual const QString getHumanLabel();

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

    DREAM3D_INSTANCE_PROPERTY(int, NumCols)
    DREAM3D_INSTANCE_PROPERTY(int, NumRows)
    DREAM3D_INSTANCE_PROPERTY(bool, HeaderIsComplete)

  signals:

    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    LoadAdditiveMonitoringData();

    void dataCheck();
    void updateCellInstancePointers();

    void generateFileList();


  private:
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, Current)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, Speed)

//    QString int_to_string(int value);
//    QString float_to_string(float value);

    LoadAdditiveMonitoringData(const LoadAdditiveMonitoringData&); // Copy Constructor Not Implemented
    void operator=(const LoadAdditiveMonitoringData&); // Operator '=' Not Implemented
};

#endif /* LoadAdditiveMonitoringData_H_ */



