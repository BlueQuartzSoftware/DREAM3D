///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////

#ifndef SURFACEMESH_H_
#define SURFACEMESH_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif


#include <MXA/Common/MXASetGetMacros.h>
#include <MXA/MXATypes.h>
#include <AIM/Common/Constants.h>
#include <AIM/Common/SurfaceMeshFunc.h>

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
    MXA_INSTANCE_STRING_PROPERTY(OutputDirectory)
    MXA_INSTANCE_PROPERTY(int, XDim)
    MXA_INSTANCE_PROPERTY(int, YDim)
    MXA_INSTANCE_PROPERTY(int, ZDim)
    MXA_INSTANCE_PROPERTY(double, XRes)
    MXA_INSTANCE_PROPERTY(double, YRes)
    MXA_INSTANCE_PROPERTY(double, ZRes)
    MXA_INSTANCE_PROPERTY(bool, SmoothMesh)
    MXA_INSTANCE_PROPERTY(int, SmoothIterations)
    MXA_INSTANCE_PROPERTY(int, SmoothFileOutputIncrement)
    MXA_INSTANCE_PROPERTY(bool, SmoothLockQuadPoints)
    MXA_INSTANCE_PROPERTY(int, ErrorCondition)
    MXA_INSTANCE_PROPERTY(bool, BinaryVTKFile)
    MXA_INSTANCE_PROPERTY(bool, ConformalMesh)


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
