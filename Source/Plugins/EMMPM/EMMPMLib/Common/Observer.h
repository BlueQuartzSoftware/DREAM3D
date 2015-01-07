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

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"



#include "EMMPMLib/EMMPMLib.h"
#include "EMMPMLib/Core/EMMPM_Data.h"

/**
 * @class Observer Observer.h DREAM3D/Common/Observer.h
 * @brief This class implmements the <b>Observer</b> pattern from the <b>Gang of
 * Four Design Patterns</b> book. There are various methods that are used for
 * notifications. The <b>Observerable</b> will decide which method to call.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Sep 22, 2011
 * @version 1.0
 */
class EMMPMLib_EXPORT Observer
{
  public:
    Observer();
    DREAM3D_TYPE_MACRO(Observer)

    virtual ~Observer();

    /**
      * @brief Either prints a message or sends the message to the User Interface
      * @param message The message to print
      * @param progress The progress of the GrainGenerator normalized to a value between 0 and 100
      */
     virtual void updateProgressAndMessage(const char* message, int progress);
     virtual void updateProgressAndMessage(const std::string &msg, int progress);

     /**
      * @brief This method reports progress such that a user interface element such
      * as a progress bar could be updated. It is assumed the value will fluctuate
      * between 0 and 100.
      * @param value
      */
     virtual void pipelineProgress(int value);

     /**
      * @brief This message reports some human readable message suitable for display
      * on a GUI or printed to a console or possibly saved to a log file
      * @param message
      */
     virtual void pipelineProgressMessage(const char* message);
     virtual void pipelineProgressMessage(const std::string &msg);
     /**
      * @brief This message reports some human readable message suitable for display
      * on a GUI or printed to a console or possibly saved to a log file
      * @param message
      */
     virtual void pipelineWarningMessage(const char* message);
     virtual void pipelineWarningMessage(const std::string &msg);

     /**
      * @brief This message reports some human readable message suitable for display
      * on a GUI or printed to a console or possibly saved to a log file
      * @param message
      */
     virtual void pipelineErrorMessage(const char* message);
     virtual void pipelineErrorMessage(const std::string &msg);



  private:
    Observer(const Observer&); // Copy Constructor Not Implemented
    void operator=(const Observer&); // Operator '=' Not Implemented
};

#endif /* OBSERVER_H_ */
