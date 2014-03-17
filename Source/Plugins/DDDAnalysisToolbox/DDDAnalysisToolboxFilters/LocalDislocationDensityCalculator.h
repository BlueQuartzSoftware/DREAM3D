/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2013 Dr. Joseph C. Tucker (UES, Inc.)
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *                   FA8650-07-D-5800 and FA8650-10-D-5226
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef LocalDislocationDensityCalculator_H_
#define LocalDislocationDensityCalculator_H_

#include <QtCore/QString>
#include <set>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

#include "DDDAnalysisToolbox/DDDAnalysisToolboxConstants.h"

/**
 * @class LocalDislocationDensityCalculator LocalDislocationDensityCalculator.h /FilterCategoryFilters/LocalDislocationDensityCalculator.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class LocalDislocationDensityCalculator : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(LocalDislocationDensityCalculator)
    DREAM3D_STATIC_NEW_MACRO(LocalDislocationDensityCalculator)
    DREAM3D_TYPE_MACRO_SUPER(LocalDislocationDensityCalculator, AbstractFilter)

    virtual ~LocalDislocationDensityCalculator();
    DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)

    DREAM3D_INSTANCE_STRING_PROPERTY(EdgeAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(FloatVec3_t, CellSize)
    Q_PROPERTY(FloatVec3_t CellSize READ getCellSize WRITE setCellSize)

    // The user selects a new DataContainerName
    DREAM3D_FILTER_PARAMETER(QString, OutputDataContainerName)
    Q_PROPERTY(QString OutputDataContainerName READ getOutputDataContainerName WRITE setOutputDataContainerName)
    // Name the new AttributeMatrix that will get created
    DREAM3D_FILTER_PARAMETER(QString, OutputAttributeMatrixName)
    Q_PROPERTY(QString OutputAttributeMatrixName READ getOutputAttributeMatrixName WRITE setOutputAttributeMatrixName)
    // Give the created data array a name
    DREAM3D_FILTER_PARAMETER(QString, OutputArrayName)
    Q_PROPERTY(QString OutputArrayName READ getOutputArrayName WRITE setOutputArrayName)

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const QString getCompiledLibraryName() { return DDDAnalysisToolbox::DDDAnalysisToolboxBaseName; }
    virtual const QString getGroupName() { return DREAM3D::FilterGroups::DDDAnalysisFilters; }
    virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::StatisticsFilters; }

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel() { return "Calculate Local Dislocation Densities"; }

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
    LocalDislocationDensityCalculator();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param features The number of features
    * @param ensembles The number of ensembles
    */
    void dataCheck();
    void updateCellInstancePointers();

  private:
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, OutputArray)

    LocalDislocationDensityCalculator(const LocalDislocationDensityCalculator&); // Copy Constructor Not Implemented
    void operator=(const LocalDislocationDensityCalculator&); // Operator '=' Not Implemented
};

#endif /* LocalDislocationDensityCalculator_H_ */






