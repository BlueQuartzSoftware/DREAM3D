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
#ifndef _ConvertOrientations_H_
#define _ConvertOrientations_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

#include "OrientationLib/OrientationOps/OrientationOps.h"

/**
 * @brief The ConvertOrientations class. See [Filter documentation](@ref convertorientations) for details.
 */
class  ConvertOrientations : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(ConvertOrientations)
    DREAM3D_STATIC_NEW_MACRO(ConvertOrientations)
    DREAM3D_TYPE_MACRO_SUPER(ConvertOrientations, AbstractFilter)

    virtual ~ConvertOrientations();

    DREAM3D_FILTER_PARAMETER(int, InputType)
    Q_PROPERTY(int InputType READ getInputType WRITE setInputType)

    DREAM3D_FILTER_PARAMETER(int, OutputType)
    Q_PROPERTY(int OutputType READ getOutputType WRITE setOutputType)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, InputOrientationArrayPath)
    Q_PROPERTY(DataArrayPath InputOrientationArrayPath READ getInputOrientationArrayPath WRITE setInputOrientationArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, OutputOrientationArrayName)
    Q_PROPERTY(QString OutputOrientationArrayName READ getOutputOrientationArrayName WRITE setOutputOrientationArrayName)

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

    /**
     * @brief execute
     */
    virtual void execute();

    /**
     * @brief preflight
     */
    virtual void preflight();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    ConvertOrientations();

    void dataCheck();

  private:

    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, InputOrientations)

    DEFINE_CREATED_DATAARRAY_VARIABLE(float, OutputOrientations)

    ConvertOrientations(const ConvertOrientations&); // Copy Constructor Not Implemented
    void operator=(const ConvertOrientations&); // Operator '=' Not Implemented
};

#endif /* ConvertOrientations_H_ */
