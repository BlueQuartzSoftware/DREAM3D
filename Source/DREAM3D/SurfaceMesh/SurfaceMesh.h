/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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

#ifndef SURFACEMESH_H_
#define SURFACEMESH_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif


#include <MXA/Common/MXASetGetMacros.h>
#include <MXA/MXATypes.h>
#include "DREAM3D/Common/Constants.h"
#include <DREAM3D/SurfaceMesh/SurfaceMeshFunc.h>

#ifdef AIM_USE_QT
#include <QtCore/QObject>

#define AIM_STRING QString
#else
#define AIM_STRING std::string
#endif

/**
* @class SurfaceMesh SurfaceMesh.h AIM/SurfaceMesh/SurfaceMesh.h
* @brief This class serves as the main entry point to execute the Surface Mesh
* codes. When used from a Qt framework this class inherits from QThread thus making this class able to be excuted
* on another thread of execution so that the User interface does not lock up while the code executes. The main method
* to implement is the 'compute()' method. This method will be called from the 'run()' method during the execution of
* the thread. The normal constructor is protected so that the end programmer must instantiate this class through the
* static "New()" method which will produce a boost::shared_ptr instance.
* @author Michael A. Jackson for BlueQuartz Software, Dr. Michael Groeber, USAFRL
* @date Nov 3, 2009
* @version 1.0
*/
class SurfaceMesh
#ifdef AIM_USE_QT
 : public QObject
#endif
{
#ifdef AIM_USE_QT
Q_OBJECT
#endif

  public:
    MXA_SHARED_POINTERS(SurfaceMesh);
    MXA_TYPE_MACRO(SurfaceMesh);

#ifdef AIM_USE_QT
    static Pointer New (QObject* parent = 0);
#else
    MXA_STATIC_NEW_MACRO(SurfaceMesh);
#endif
    virtual ~SurfaceMesh();

    MXA_INSTANCE_STRING_PROPERTY(InputDirectory)
    MXA_INSTANCE_STRING_PROPERTY(InputFile)
    MXA_INSTANCE_STRING_PROPERTY(ScalarName)

    MXA_INSTANCE_PROPERTY(int, XDim)
    MXA_INSTANCE_PROPERTY(int, YDim)
    MXA_INSTANCE_PROPERTY(int, ZDim)
    MXA_INSTANCE_PROPERTY(double, XRes)
    MXA_INSTANCE_PROPERTY(double, YRes)
    MXA_INSTANCE_PROPERTY(double, ZRes)

    MXA_INSTANCE_STRING_PROPERTY(OutputDirectory)
    MXA_INSTANCE_STRING_PROPERTY(OutputFilePrefix)
    MXA_INSTANCE_PROPERTY(bool, ConformalMesh)
    MXA_INSTANCE_PROPERTY(bool, BinaryVTKFile)

    MXA_INSTANCE_PROPERTY(bool, DeleteTempFiles)

    MXA_INSTANCE_PROPERTY(bool, SmoothMesh)
    MXA_INSTANCE_PROPERTY(int, SmoothIterations)
    MXA_INSTANCE_PROPERTY(int, SmoothFileOutputIncrement)
    MXA_INSTANCE_PROPERTY(bool, SmoothLockQuadPoints)
    MXA_INSTANCE_PROPERTY(int, ErrorCondition)

    /**
     * @brief Cancel the operation
     */
    MXA_INSTANCE_PROPERTY(bool, Cancel);

    /**
     * @brief Either prints a message or sends the message to the User Interface
     * @param message The message to print
     * @param progress The progress of the SurfaceMesh normalized to a value between 0 and 100
     */
    void progressMessage(AIM_STRING message, int progress);

#ifdef AIM_USE_QT


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

    SurfaceMesh(
#ifdef AIM_USE_QT
        QObject* parent = 0
#endif
        );

  private:
	  SurfaceMeshFunc::Pointer m;

    SurfaceMesh(const SurfaceMesh&);    // Copy Constructor Not Implemented
    void operator=(const SurfaceMesh&);  // Operator '=' Not Implemented
};


#endif /* SURFACEMESH_H_ */
