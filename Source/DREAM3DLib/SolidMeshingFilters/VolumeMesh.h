/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef _VOLUMEMESH_H_
#define _VOLUMEMESH_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif


#include <MXA/Common/MXASetGetMacros.h>
#include "MXA/MXA.h"
#include "DREAM3DLib/Common/Constants.h"


#ifdef DREAM3D_USE_QT
#include <QtCore/QObject>

#define AIM_STRING QString
#else
#define AIM_STRING std::string
#endif

/**
* @class VolumeMesh VolumeMesh.h AIM/VolumeMesh/VolumeMesh.h
* @brief This class serves as the main entry point to execute the Volume Meshing Codes.
* When used from a Qt framework this class inherits from QThread thus making this class able to be excuted
* on another thread of execution so that the User interface does not lock up while the code executes. The main method
* to implement is the 'compute()' method. This method will be called from the 'run()' method during the execution of
* the thread. The normal constructor is protected so that the end programmer must instantiate this class through the
* static "New()" method which will produce a boost::shared_ptr instance.
* @author Michael A. Jackson for BlueQuartz Software, Dr. Michael Groeber, USAFRL
* @date Nov 3, 2009
* @version 1.0
*/
class VolumeMesh
#ifdef DREAM3D_USE_QT
  : public QObject
#endif
{
#ifdef DREAM3D_USE_QT
    Q_OBJECT
#endif

  public:
    MXA_SHARED_POINTERS(VolumeMesh)
    MXA_TYPE_MACRO(VolumeMesh)

#ifdef DREAM3D_USE_QT
    static Pointer New (QObject* parent = 0);
#else
    MXA_STATIC_NEW_MACRO(VolumeMesh)
#endif
    virtual ~VolumeMesh();

    MXA_INSTANCE_STRING_PROPERTY(NodesFile)
    MXA_INSTANCE_STRING_PROPERTY(TrianglesFile)
    MXA_INSTANCE_STRING_PROPERTY(OutputDirectory)
    MXA_INSTANCE_STRING_PROPERTY(OutputFilePrefix)

    MXA_INSTANCE_PROPERTY(double, XDim)
    MXA_INSTANCE_PROPERTY(double, YDim)
    MXA_INSTANCE_PROPERTY(double, ZDim)

    MXA_INSTANCE_PROPERTY(double, XRes)
    MXA_INSTANCE_PROPERTY(double, YRes)
    MXA_INSTANCE_PROPERTY(double, ZRes)

    MXA_INSTANCE_PROPERTY(int, NumGrains)


    MXA_INSTANCE_PROPERTY(int, ErrorCondition)

    /**
     * @brief Cancel the operation
     */
    MXA_INSTANCE_PROPERTY(bool, Cancel)

    /**
     * @brief Either prints a message or sends the message to the User Interface
     * @param message The message to print
     * @param progress The progress of the VolumeMesh normalized to a value between 0 and 100
     */
    void progressMessage(AIM_STRING message, int progress);

#ifdef DREAM3D_USE_QT
    /**
     * Qt Signals for connections
     */
  signals:
    void updateMessage(QString message);
    void updateProgress(int value);
    void finished();

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

    VolumeMesh(
#ifdef DREAM3D_USE_QT
      QObject* parent = 0
#endif
    );

  private:

    VolumeMesh(const VolumeMesh&);    // Copy Constructor Not Implemented
    void operator=(const VolumeMesh&);  // Operator '=' Not Implemented
};


#endif /* _VOLUMEMESH_H_ */
