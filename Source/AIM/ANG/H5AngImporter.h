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

#ifndef _H5ANGIMPORTER_H_
#define _H5ANGIMPORTER_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif


#include "hdf5.h"

#include <MXA/Common/MXASetGetMacros.h>
#include <MXA/MXATypes.h>
#include <AIM/Common/Constants.h>

#ifdef AIM_USE_QT
#include <QtCore/QObject>
#include <QtCore/QThread>
#endif

#include "MXA/Common/MXASetGetMacros.h"
#include "AIM/Common/AIMCommonConfiguration.h"

/**
 * @class H5AngImporter H5AngImporter.h AIM/ANG/H5AngImporter.h
 * @brief
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Dec 13, 2010
 * @version 1.0
 */
class H5AngImporter
#ifdef AIM_USE_QT
 : public QThread
#endif
{
#ifdef AIM_USE_QT
Q_OBJECT
#endif

  public:
    MXA_SHARED_POINTERS(H5AngImporter)
    MXA_TYPE_MACRO(H5AngImporter)

#ifdef AIM_USE_QT
    static Pointer New (QObject* parent = 0);
#else
    MXA_STATIC_NEW_MACRO(H5AngImporter)
#endif

    virtual ~H5AngImporter();

    MXA_INSTANCE_STRING_PROPERTY(OutputFile)
    MXA_INSTANCE_STRING_PROPERTY(InputDirectory)
    MXA_INSTANCE_STRING_PROPERTY(AngFilePrefix)
    MXA_INSTANCE_PROPERTY(int, AngSeriesMaxSlice)
    MXA_INSTANCE_PROPERTY(int, ZStartIndex)
    MXA_INSTANCE_PROPERTY(int, ZEndIndex)
    MXA_INSTANCE_PROPERTY(float, ZResolution)

    /**
     * @brief Cancel the operation
     */
    MXA_INSTANCE_PROPERTY(bool, Cancel);

    /**
     * @brief Either prints a message or sends the message to the User Interface
     * @param message The message to print
     * @param progress The progress of the Reconstruction normalized to a value between 0 and 100
     */
    void progressMessage(const std::string &message, int progress);

#ifdef AIM_USE_QT
    /**
     * Qt Signals for connections
     */
    signals:
      void updateMessage(QString message);
      void updateProgress(int value);

    public slots:
    /**
     * @brief Slot to receive a signal to cancel the operation
     */
      void on_CancelWorker();
#endif

      /**
       * @brief Main method to run the operation
       */
      void compute();

  protected:
#ifdef AIM_USE_QT
    H5AngImporter(QObject* parent = 0);
    virtual void run();

#else
    H5AngImporter();
#endif

    int importAngFile(hid_t fileId, int index, const std::string &angFile);


  private:
    H5AngImporter(const H5AngImporter&); // Copy Constructor Not Implemented
    void operator=(const H5AngImporter&); // Operator '=' Not Implemented
};

#endif /* _H5ANGIMPORTER_H_ */
