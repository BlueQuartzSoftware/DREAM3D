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


/**
* @brief This is a subclass of the FilterPipeline class that controls this pipeline
* execution. This class is here to simply wrap the methods with Qt's signals and
* slots mechanism to push messages and progress to the Qt based GUI.
*/
class QFilterPipeline : public QObject, public FilterPipeline
{
  Q_OBJECT
  public:

    QFilterPipeline(QObject* parent = 0);
    virtual ~QFilterPipeline();

    /**
     * @brief Either prints a message or sends the message to the User Interface
     * @param message The message to print
     * @param progress The progress of the GrainGenerator normalized to a value between 0 and 100
     */
    virtual void updateProgressAndMessage(const char* message, int value);

    /**
     * @brief Either prints a message or sends the message to the User Interface
     * @param message The message to print
     * @param progress The progress of the GrainGenerator normalized to a value between 0 and 100
     */
    virtual void updateProgressAndMessage(const std::string &message, int progress);

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

    /**
     * @brief This message reports some human readable message suitable for display
     * on a GUI or printed to a console or possibly saved to a log file
     * @param message
     */
    virtual void pipelineWarningMessage(const char* message);

    /**
     * @brief This message reports some human readable message suitable for display
     * on a GUI or printed to a console or possibly saved to a log file
     * @param message
     */
    virtual void pipelineErrorMessage(const char* message);

    /**
     * @brief This method is called from the run() method just before exiting and
     * signals the end of the pipeline execution
     */
    virtual void pipelineFinished();



    /**
     * Qt Signals for connections
     */
     signals:
       void progressMessage(const QString &message);
       void warningMessage(const QString &message);
       void errorMessage(const QString &message);
       void updateProgress(int value);
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
