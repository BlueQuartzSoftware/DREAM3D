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
#ifndef RotateEulerRefFrame_H_
#define RotateEulerRefFrame_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

/**
 * @class RotateEulerRefFrame RotateEulerRefFrame.h DREAM3DLib/GenericFilters/RotateEulerRefFrame.h
 * @brief This filter will convert the Euler Angles. Simply enter the conversion
 * factor that you want to use. For Degrees to Radians 0.01745329 and for Radians
 * to Degrees use 57.2957785
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Apr 26, 2012
 * @version 1.0
 */
class DREAM3DLib_EXPORT RotateEulerRefFrame : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(RotateEulerRefFrame)
    DREAM3D_STATIC_NEW_MACRO(RotateEulerRefFrame)
    DREAM3D_TYPE_MACRO_SUPER(RotateEulerRefFrame, AbstractFilter)
    virtual ~RotateEulerRefFrame();

    //------ Required Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(CellEulerAnglesArrayName)

     DREAM3D_INSTANCE_PROPERTY(FloatVec3Widget_t, RotationAxis)
    DREAM3D_INSTANCE_PROPERTY(float, RotationAngle)

    virtual const std::string getGroupName()  { return DREAM3D::FilterGroups::ProcessingFilters; }
    virtual const std::string getSubGroupName()  { return DREAM3D::FilterSubGroups::ConversionFilters; }
    virtual const std::string getHumanLabel() { return "Rotate Euler Reference Frame"; }

    virtual void setupFilterParameters();
    virtual void writeFilterParameters(AbstractFilterParametersWriter* writer);

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void preflight();
    virtual void execute();

  protected:
    RotateEulerRefFrame();

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

  private:
    float* m_CellEulerAngles;

    RotateEulerRefFrame(const RotateEulerRefFrame&); // Copy Constructor Not Implemented
    void operator=(const RotateEulerRefFrame&); // Operator '=' Not Implemented
};

#endif /* RotateEulerRefFrame_H_ */
