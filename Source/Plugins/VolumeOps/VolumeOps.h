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

#ifndef VolumeOps_H_
#define VolumeOps_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/AbstractPipeline.h"
#include "DREAM3DLib/Common/Observer.h"
#include "DREAM3DLib/Common/DataContainer.h"



/**
* @class VolumeOps VolumeOps.h AIM/VolumeOps/VolumeOps.h
* @brief This class serves as the main entry point to execute the VolumeOps
* pipeline
* @author
* @author
* @date
* @version 1.0
*/
class VolumeOps : public AbstractPipeline, public Observer
{
  public:
    DREAM3D_SHARED_POINTERS(VolumeOps);
    DREAM3D_TYPE_MACRO(VolumeOps);
    DREAM3D_STATIC_NEW_MACRO(VolumeOps);

    virtual ~VolumeOps();

    DREAM3D_INSTANCE_STRING_PROPERTY(H5InputFile)
    DREAM3D_INSTANCE_STRING_PROPERTY(H5OutputFile)

    DREAM3D_INSTANCE_PROPERTY(int, XMin)
    DREAM3D_INSTANCE_PROPERTY(int, YMin)
    DREAM3D_INSTANCE_PROPERTY(int, ZMin)

    DREAM3D_INSTANCE_PROPERTY(int, XMax)
    DREAM3D_INSTANCE_PROPERTY(int, YMax)
    DREAM3D_INSTANCE_PROPERTY(int, ZMax)

    DREAM3D_INSTANCE_PROPERTY(float, XRes)
    DREAM3D_INSTANCE_PROPERTY(float, YRes)
    DREAM3D_INSTANCE_PROPERTY(float, ZRes)

	/**
    * @brief Main method to run the operation
    */
    virtual void execute();

  protected:
    VolumeOps();

  private:
    DataContainer::Pointer m;

    VolumeOps(const VolumeOps&);    // Copy Constructor Not Implemented
    void operator=(const VolumeOps&);  // Operator '=' Not Implemented
};



#endif /* VolumeOps_H_ */
