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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _WriteStatsGenOdfAngleFile_H_
#define _WriteStatsGenOdfAngleFile_H_

#include <QtCore/QString>

#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"

/**
 * @class WriteStatsGenOdfAngleFile WriteStatsGenOdfAngleFile.h /IOFilters/WriteStatsGenOdfAngleFile.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class WriteStatsGenOdfAngleFile : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(WriteStatsGenOdfAngleFile)
    DREAM3D_STATIC_NEW_MACRO(WriteStatsGenOdfAngleFile)
    DREAM3D_TYPE_MACRO_SUPER(WriteStatsGenOdfAngleFile, AbstractFilter)

    virtual ~WriteStatsGenOdfAngleFile();

    /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */

    DREAM3D_FILTER_PARAMETER(QString, OutputFile)
    Q_PROPERTY(QString OutputFile READ getOutputFile WRITE setOutputFile)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
    Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellEulerAnglesArrayPath)
    Q_PROPERTY(DataArrayPath CellEulerAnglesArrayPath READ getCellEulerAnglesArrayPath WRITE setCellEulerAnglesArrayPath)

    DREAM3D_FILTER_PARAMETER(bool, ConvertToDegrees)
    Q_PROPERTY(bool ConvertToDegrees READ getConvertToDegrees WRITE setConvertToDegrees)

    DREAM3D_FILTER_PARAMETER(bool, UseGoodVoxels)
    Q_PROPERTY(bool UseGoodVoxels READ getUseGoodVoxels WRITE setUseGoodVoxels)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, GoodVoxelsArrayPath)
    Q_PROPERTY(DataArrayPath GoodVoxelsArrayPath READ getGoodVoxelsArrayPath WRITE setGoodVoxelsArrayPath)


    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();

    /**
     * @brief getSubGroupName This returns the subgroup within the main group for this filter.
     * @return
     */
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
    WriteStatsGenOdfAngleFile();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param features The number of features
    * @param ensembles The number of ensembles
    */
    void dataCheck();

    int determineOutputLineCount(int64_t totalPoints, int32_t phase);
    int writeOutputFile(QTextStream& out, int32_t lineCount, int64_t totalPoints, int32_t phase);


  private:
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, CellEulerAngles)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, CellPhases)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(uint32_t, CrystalStructures)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(bool, GoodVoxels)

    WriteStatsGenOdfAngleFile(const WriteStatsGenOdfAngleFile&); // Copy Constructor Not Implemented
    void operator=(const WriteStatsGenOdfAngleFile&); // Operator '=' Not Implemented
};

#endif /* _WriteStatsGenOdfAngleFile_H_ */







