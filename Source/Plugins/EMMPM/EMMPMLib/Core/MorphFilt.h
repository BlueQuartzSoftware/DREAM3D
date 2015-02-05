/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _MORPHFILT_H_
#define _MORPHFILT_H_

#include <QtCore/QObject>
#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Observable.h"

#include "EMMPMLib/Core/EMMPM_Constants.h"
#include "EMMPMLib/Core/EMMPM_Data.h"


class EMMPMLib_EXPORT MorphFilter : public Observable
{
	Q_OBJECT;

  public:
    DREAM3D_SHARED_POINTERS(MorphFilter);
    DREAM3D_STATIC_NEW_MACRO(MorphFilter);
    DREAM3D_TYPE_MACRO(MorphFilter);
    virtual ~MorphFilter();

    unsigned int maxi(int a, int b);

   unsigned int mini(int a, int b);

    /**
     * @brief Performs the Morphological filtering on the input image
     * @param data Non NULL EMMPM_Data pointer
     * @param curve
     * @param se
     * @param r
     */
    void morphFilt(EMMPM_Data* data, unsigned char* curve, unsigned char* se, int r);

    /**
     * @brief Main entry point for the morphological filter
     * @param data Non NULL EMMPM_Data pointer
     */
    void multiSE(EMMPM_Data* data);

	DREAM3D_INSTANCE_PROPERTY(int, ErrorCondition)

	/**
	* @brief This returns a string that is displayed in the GUI. It should be readable
	* and understandable by humans.
	*/
	virtual const QString getHumanLabel();

  protected:
    MorphFilter();

  private:
    MorphFilter(const MorphFilter&); // Copy Constructor Not Implemented
    void operator=(const MorphFilter&); // Operator '=' Not Implemented

};

#endif /* _MORPHFILT_H_ */
