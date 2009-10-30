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
#include <MXA/Common/MXATypes.h>
#include <AIM/Common/Constants.h>
//#include <AIM/Common/MicroGen3D.h>

#ifdef AIM_USE_QT
#include <QtCore/QObject>
#include <QtCore/QThread>
#define AIM_STRING QString
#else
#define AIM_STRING std::string
#endif


class SurfaceMesh
#ifdef AIM_USE_QT
 : public QThread
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

    MXA_INSTANCE_STRING_PROPERTY(DXFile, m_DxFile)
    MXA_INSTANCE_STRING_PROPERTY(EdgeTableFile, m_EdgeTableFile)
    MXA_INSTANCE_STRING_PROPERTY(NeighSpinTableFile, m_NeighSpinTableFile)
    MXA_INSTANCE_STRING_PROPERTY(OutputDirectory, m_OutputDirectory)
    MXA_INSTANCE_PROPERTY_m(int, XDim)
    MXA_INSTANCE_PROPERTY_m(int, YDim)
    MXA_INSTANCE_PROPERTY_m(int, ZDim)
    MXA_INSTANCE_PROPERTY_m(bool, SmoothMesh)
    MXA_INSTANCE_PROPERTY_m(int, SmoothIterations)
    MXA_INSTANCE_PROPERTY_m(int, SmoothFileOutputIncrement)
    MXA_INSTANCE_PROPERTY_m(bool, SmoothLockQuadPoints)
    MXA_INSTANCE_PROPERTY_m(int, ErrorCondition)



    /**
     * @brief Either prints a message or sends the message to the User Interface
     * @param message The message to print
     * @param progress The progress of the SurfaceMesh normalized to a value between 0 and 100
     */
    void progressMessage(AIM_STRING message, int progress);

#ifdef AIM_USE_QT

    /**
     * @brief Cancel the operation
     */
    MXA_INSTANCE_PROPERTY_m(bool, Cancel);

    /**
     * Qt Signals for connections
     */
    signals:
    //  void workerComplete();
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
    SurfaceMesh(QObject* parent = 0);
    virtual void run();

#else
    SurfaceMesh();
#endif

  private:
    //MicroGen3D::Pointer m_microgen;



    SurfaceMesh(const SurfaceMesh&);    // Copy Constructor Not Implemented
    void operator=(const SurfaceMesh&);  // Operator '=' Not Implemented
};


#endif /* SURFACEMESH_H_ */
