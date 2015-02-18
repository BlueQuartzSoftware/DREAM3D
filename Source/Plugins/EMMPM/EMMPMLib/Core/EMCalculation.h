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

#ifndef _EMCalculation_H_
#define _EMCalculation_H_

#include <QtCore/QObject>
#include <QtCore/QString>

#include "DREAM3DLib/Common/Observable.h"

#include "EMMPMLib/EMMPMLib.h"
#include "EMMPMLib/Core/EMMPM_Data.h"
#include "EMMPMLib/Core/EMMPM_Constants.h"
#include "EMMPMLib/Core/EMMPM.h"
#include "EMMPMLib/Common/StatsDelegate.h"


/**
 * @class EMCalculation EMCalculation Common/CurvatureEM.h
 * @brief This class performs the EM Loops of the EM/MPM algorithm
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Sep 10, 2012
 * @version 1.0
 */
class EMMPMLib_EXPORT EMCalculation : public Observable
{
	Q_OBJECT;

  public:
    DREAM3D_SHARED_POINTERS(EMCalculation)
    DREAM3D_STATIC_NEW_MACRO(EMCalculation)
    DREAM3D_TYPE_MACRO(EMCalculation)

    virtual ~EMCalculation();

    DREAM3D_INSTANCE_PROPERTY(EMMPM_Data::Pointer, Data);
    DREAM3D_INSTANCE_PROPERTY(StatsDelegate*, StatsDelegate);
	  DREAM3D_INSTANCE_PROPERTY(int, ErrorCondition)
	  DREAM3D_INSTANCE_PROPERTY(QString, MessagePrefix)

	/**
	* @brief This returns a string that is displayed in the GUI. It should be readable
	* and understandable by humans.
	*/
	virtual const QString getHumanLabel();

    /**
     * @fn void EMMPM_CurvatureEMLoops(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks)
     * @brief Runs the EM portion of the EM/MPM Algorithm
     * @param data a non NULL EMMPM_Data pointer
     * @param callbacks a non NULL EMMPM_CallbackFunctions
     */
    void execute();

  protected:
    EMCalculation();

  private:
    EMCalculation(const EMCalculation&); // Copy Constructor Not Implemented
    void operator=(const EMCalculation&); // Operator '=' Not Implemented

};

#endif /* _EMCalculation_H_ */
