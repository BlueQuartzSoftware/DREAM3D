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

#ifndef _VOLUMEMESH_H_
#define _VOLUMEMESH_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif


#include <MXA/Common/MXASetGetMacros.h>
#include <MXA/Common/MXATypes.h>
#include <AIM/Common/Constants.h>


#ifdef AIM_USE_QT
#include <QtCore/QObject>
#include <QtCore/QThread>
#define AIM_STRING QString
#else
#define AIM_STRING std::string
#endif


class VolumeMesh
#ifdef AIM_USE_QT
 : public QThread
#endif
{
#ifdef AIM_USE_QT
Q_OBJECT
#endif

  public:
    MXA_SHARED_POINTERS(VolumeMesh);
    MXA_TYPE_MACRO(VolumeMesh);

#ifdef AIM_USE_QT
    static Pointer New (QObject* parent = 0);
#else
    MXA_STATIC_NEW_MACRO(VolumeMesh);
#endif
    virtual ~VolumeMesh();



    MXA_INSTANCE_STRING_PROPERTY_m(NodesFile)
    MXA_INSTANCE_STRING_PROPERTY_m(TrianglesFile)
    MXA_INSTANCE_STRING_PROPERTY_m(OutputDirectory)

    MXA_INSTANCE_PROPERTY_m(double, XDim)
    MXA_INSTANCE_PROPERTY_m(double, YDim)
    MXA_INSTANCE_PROPERTY_m(double, ZDim)

    MXA_INSTANCE_PROPERTY_m(double, XRes)
    MXA_INSTANCE_PROPERTY_m(double, YRes)
    MXA_INSTANCE_PROPERTY_m(double, ZRes)

    MXA_INSTANCE_PROPERTY_m(int, NumGrains)


    MXA_INSTANCE_PROPERTY_m(int, ErrorCondition)



    /**
     * @brief Either prints a message or sends the message to the User Interface
     * @param message The message to print
     * @param progress The progress of the VolumeMesh normalized to a value between 0 and 100
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
    VolumeMesh(QObject* parent = 0);
    virtual void run();

#else
    VolumeMesh();
#endif

  private:
    //MicroGen3D::Pointer m_microgen;



    VolumeMesh(const VolumeMesh&);    // Copy Constructor Not Implemented
    void operator=(const VolumeMesh&);  // Operator '=' Not Implemented
};


#endif /* _VOLUMEMESH_H_ */
