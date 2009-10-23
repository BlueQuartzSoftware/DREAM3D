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

#include <MXA/Common/MXASetGetMacros.h>
#include <MXA/Common/MXATypes.h>
#include <AIM/Common/Constants.h>

#ifdef AIM_USE_QT
#include <QtCore/QObject>
#include <QtCore/QThread>
#endif
/*
 *
 */
class Reconstruction
#ifdef AIM_USE_QT
 : public QObject
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
    MXA_INSTANCE_PROPERTY_m(int, ZIndexStart)
    MXA_INSTANCE_PROPERTY_m(int, ZIndexEnd)
    MXA_INSTANCE_PROPERTY_m(double, ZResolution)
    MXA_INSTANCE_PROPERTY_m(bool, MergeTwins)
    MXA_INSTANCE_PROPERTY_m(int32, MinAllowedGrainSize)
    MXA_INSTANCE_PROPERTY_m(double, MinSeedConfidence)
    MXA_INSTANCE_PROPERTY_m(double, MisorientationTolerance)
    MXA_INSTANCE_PROPERTY_m(AIM::Reconstruction::CrystalStructure, CrystalStructure)
    MXA_INSTANCE_PROPERTY_m(bool, AlreadyFormed)
    MXA_INSTANCE_PROPERTY_m(int, ErrorCondition);

    void parseAngFile();


#ifdef AIM_USE_QT

    /**
     * @brief Cancel the operation
     */
    MXA_INSTANCE_PROPERTY_m(bool, Cancel);

    /**
     * Qt Signals for connections
     */
    signals:
      void workerComplete();
      void workerHasMessage(QString message);


    public slots:
    /**
     * @brief Slot to receive a signal to cancel the operation
     */
      void on_CancelWorker()
      {
        std::cout << "Reconstruction::cancelWorker()" << std::endl;
        this->m_Cancel = true;
      }
#endif
      /**
       * @brief Main method to run the operation
       */

      void compute();


  protected:
#ifdef AIM_USE_QT
      Reconstruction(QObject* parent = 0);
#else
    Reconstruction();
#endif

  private:
    Reconstruction(const Reconstruction&);    // Copy Constructor Not Implemented
    void operator=(const Reconstruction&);  // Operator '=' Not Implemented
};

#endif /* RECONSTRUCTION_H_ */
