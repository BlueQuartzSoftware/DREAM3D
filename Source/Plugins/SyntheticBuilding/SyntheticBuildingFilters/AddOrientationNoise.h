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


#ifndef _AddOrientationNoise_H_
#define _AddOrientationNoise_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

/**
 * @brief The AddOrientationNoise class. See [Filter documentation](@ref addorientationnoise) for details.
 */
class AddOrientationNoise : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(AddOrientationNoise)
    DREAM3D_STATIC_NEW_MACRO(AddOrientationNoise)
    DREAM3D_TYPE_MACRO_SUPER(AddOrientationNoise, AbstractFilter)

    virtual ~AddOrientationNoise();

    DREAM3D_FILTER_PARAMETER(float, Magnitude)
    Q_PROPERTY(float Magnitude READ getMagnitude WRITE setMagnitude)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellEulerAnglesArrayPath)
    Q_PROPERTY(DataArrayPath CellEulerAnglesArrayPath READ getCellEulerAnglesArrayPath WRITE setCellEulerAnglesArrayPath)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();
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
    virtual void preflight();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    AddOrientationNoise();

    /**
     * @brief add_orientation_noise Applies a random axis-angle rotation to an
     * array of Euler angles
     */
    void add_orientation_noise();

  private:
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, CellEulerAngles)

    void dataCheck();

    AddOrientationNoise(const AddOrientationNoise&); // Copy Constructor Not Implemented
    void operator=(const AddOrientationNoise&); // Operator '=' Not Implemented
};

#endif /* AddOrientationNoise_H_ */