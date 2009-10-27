///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _RECONSTRUCTION_H_
#define _RECONSTRUCTION_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


#include <MXA/Common/MXASetGetMacros.h>
#include <MXA/Common/MXATypes.h>
#include <AIM/Common/Constants.h>
#include <AIM/Common/MicroGen3D.h>

#ifdef AIM_USE_QT
#include <QtCore/QObject>
#include <QtCore/QThread>
#define AIM_STRING QString
#else
#define AIM_STRING std::string
#endif
/*
 *
 */
class Reconstruction
#ifdef AIM_USE_QT
 : public QThread
#endif
{
#ifdef AIM_USE_QT
Q_OBJECT
#endif

  public:
    MXA_SHARED_POINTERS(Reconstruction);
    MXA_TYPE_MACRO(Reconstruction);

#ifdef AIM_USE_QT
    static Pointer New (QObject* parent = 0);
#else
    MXA_STATIC_NEW_MACRO(Reconstruction);
#endif
    virtual ~Reconstruction();


    MXA_INSTANCE_STRING_PROPERTY(InputDirectory, m_InputDirectory)
    MXA_INSTANCE_STRING_PROPERTY(OutputDirectory, m_OutputDirectory)
    MXA_INSTANCE_STRING_PROPERTY(AngFilePrefix, m_AngFilePrefix)
    MXA_INSTANCE_PROPERTY_m(int, AngSeriesMaxSlice)
    MXA_INSTANCE_PROPERTY_m(int, ZStartIndex)
    MXA_INSTANCE_PROPERTY_m(int, ZEndIndex)
    MXA_INSTANCE_PROPERTY_m(double, ZResolution)
    MXA_INSTANCE_PROPERTY_m(bool, MergeTwins)
    MXA_INSTANCE_PROPERTY_m(int32, MinAllowedGrainSize)
    MXA_INSTANCE_PROPERTY_m(double, MinSeedConfidence)
    MXA_INSTANCE_PROPERTY_m(double, MisorientationTolerance)
    MXA_INSTANCE_PROPERTY_m(AIM::Representation::CrystalStructure, CrystalStructure)
    MXA_INSTANCE_PROPERTY_m(bool, AlreadyFormed)
    MXA_INSTANCE_PROPERTY_m(int, ErrorCondition);

    void parseAngFile();

    /**
     * @brief Either prints a message or sends the message to the User Interface
     * @param message The message to print
     * @param progress The progress of the Reconstruction normalized to a value between 0 and 100
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
    Reconstruction(QObject* parent = 0);
    virtual void run();

#else
    Reconstruction();
#endif

  private:
    MicroGen3D::Pointer m_microgen;



    Reconstruction(const Reconstruction&);    // Copy Constructor Not Implemented
    void operator=(const Reconstruction&);  // Operator '=' Not Implemented
};

#endif /* RECONSTRUCTION_H_ */
