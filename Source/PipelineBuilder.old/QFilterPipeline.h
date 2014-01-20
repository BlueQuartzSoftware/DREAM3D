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

#ifndef QFilterPipeline_H_
#define QFilterPipeline_H_


#include <QtCore/QObject>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/Common/PipelineMessage.h"


/**
* @brief This is a subclass of the FilterPipeline class that controls this pipeline
* execution. This class is here to simply wrap the methods with Qt's signals and
* slots mechanism to push messages and progress to the Qt based GUI.
*/
class QFilterPipeline : public FilterPipeline
{
    Q_OBJECT
  public:

    QFilterPipeline(QObject* parent = 0);
    virtual ~QFilterPipeline();

    /**
     * @brief This message reports some human readable message suitable for display
     * on a GUI or printed to a console or possibly saved to a log file
     * @param message
     */
    virtual void sendPipelineMessage(PipelineMessage& msg);

    /**
     * @brief Sends a Vector of PipelineMessage to the observers
     * @param messages The messages to send.
     */
    virtual void sendPipelineMessages(QVector<PipelineMessage> messages);

    /**
     * @brief This method is called from the run() method just before exiting and
     * signals the end of the pipeline execution
     */
    virtual void pipelineFinished();



    /**
     * Qt Signals for connections
     */
  signals:
//       void progressMessage(const QString &message);
    void firePipelineMessage(PipelineMessage errMsg);
//       void updateProgress(int value);
    void finished();

  public slots:
    /**
     * @brief Slot to receive a signal to cancel the operation
     */
    void on_CancelWorker();

    void run();

  protected:


  private:
    QFilterPipeline(const QFilterPipeline&); // Copy Constructor Not Implemented
    void operator=(const QFilterPipeline&); // Operator '=' Not Implemented
};

#endif /* QGRAINGENERATOR_H_ */

