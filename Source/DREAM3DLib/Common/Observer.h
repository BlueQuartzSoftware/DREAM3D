/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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

#ifndef OBSERVER_H_
#define OBSERVER_H_

#include <QtCore/QVector>
#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/PipelineMessage.h"

/**
 * @class Observer Observer.h DREAM3D/Common/Observer.h
 * @brief This class implements the <b>Observer</b> pattern from the <b>Gang of
 * Four Design Patterns</b> book. There are various methods that are used for
 * notifications. The <b>Observable</b> will decide which method to call.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date September 22, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT Observer
{
  public:
    Observer();
    DREAM3D_TYPE_MACRO(Observer)

    virtual ~Observer();

    /* --------- These methods are typically called from a subclass of Observer *******/

    /**
     * @brief This function prints a progress integer to the console.  Progress integers are assumed to be
     * between 0 and 100.
     * @param progress The progress of the FeatureGenerator normalized to a value between 0 and 100
     */
    virtual void updatePipelineProgress(int progress);

    /**
     * @brief These functions print human readable messages to the console.
     * @param msg The message to be printed
     */
    virtual void updatePipelineMessage(const char* msg);
    virtual void updatePipelineMessage(const QString& msg);

    /**
     * @brief These functions print both a human readable message and a progress integer to the console.
     * Progress integers are assumed to be between 0 and 100.
     * @param msg The message to be printed
     * @param progress The progress of the FeatureGenerator normalized to a value between 0 and 100
     */
    virtual void updatePipelineProgressAndMessage(const char* msg, int progress);
    virtual void updatePipelineProgressAndMessage(const QString& msg, int progress);

    /* --------------------------------------- */

    /**
     * @brief These functions send one or many pipeline messages to the DREAM3D GUI.  Error and Warning messages
     * are sent to the table widget, and status messages are sent to the status bar.
     * @param msg The message to be printed
     * @param msgs The messages to be printed
     */
    virtual void sendPipelineMessage(PipelineMessage& msg);
    virtual void sendPipelineMessages(QVector<PipelineMessage> msgs);


  private:
    Observer(const Observer&); // Copy Constructor Not Implemented
    void operator=(const Observer&); // Operator '=' Not Implemented
};

#endif /* OBSERVER_H_ */
