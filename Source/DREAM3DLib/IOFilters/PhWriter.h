/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#ifndef _PHWRITER_HPP_
#define _PHWRITER_HPP_


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/IOFilters/FileWriter.h"
#include "DREAM3DLib/Common/Constants.h"

/**
 * @class PhWriter PhWriter.h DREAM3DLib/IOFilters/PhWriter.h
 * @brief This class writes a "Ph" file from the Feature Generator Data but is templated
 * so that possibly other classes can use this writing method. This code was adapted
 * from code supplied by S. Sintay and J. Tucker.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jun 7, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT PhWriter : public FileWriter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(PhWriter)
    DREAM3D_STATIC_NEW_MACRO(PhWriter)
    DREAM3D_TYPE_MACRO_SUPER(PhWriter, FileWriter)

    virtual ~PhWriter();

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

    virtual const QString getCompiledLibraryName() { return IO::IOBaseName; }
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName() { return DREAM3D::FilterGroups::IOFilters; }

    /**
    * @brief getSubGroupName Returns the Subgroup Name for this filter
    * @return
    */
    virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::OutputFilters; }

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel() { return "Write Ph File (Feature Ids)"; }

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
    PhWriter();

    virtual int writeHeader();

    virtual int writeFile();

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
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, FeatureIds)

    PhWriter(const PhWriter&); // Copy Constructor Not Implemented
    void operator=(const PhWriter&); // Operator '=' Not Implemented
};

#endif /* PHWRITER_HPP_ */




