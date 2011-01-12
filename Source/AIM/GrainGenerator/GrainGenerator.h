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

#ifndef GRAINGENERATOR_H_
#define GRAINGENERATOR_H_

#include <MXA/Common/MXASetGetMacros.h>
#include <MXA/MXATypes.h>
#include <AIM/Common/Constants.h>
#include <AIM/Common/GrainGeneratorFunc.h>


#ifdef AIM_USE_QT
#include <QtCore/QObject>
#include <QtCore/QThread>
#define AIM_STRING QString
#else
#define AIM_STRING std::string
#endif

#include "MXA/Common/MXASetGetMacros.h"
#include "AIM/Common/AIMCommonConfiguration.h"

/**
* @class GrainGenerator GrainGenerator.h AIM/GrainGenerator/GrainGenerator.h
* @brief This class serves as the main entry point to execute the Synthetic Grain
* generation. When used from a Qt framework this class inherits from QThread thus making this class able to be excuted
* on another thread of execution so that the User interface does not lock up while the code executes. The main method
* to implement is the 'compute()' method. This method will be called from the 'run()' method during the execution of
* the thread. The normal constructor is protected so that the end programmer must instantiate this class through the
* static "New()" method which will produce a boost::shared_ptr instance.
* @author Michael A. Jackson for BlueQuartz Software, Dr. Michael Groeber, USAFRL
* @date Nov 3, 2009
* @version 1.0
*/
class GrainGenerator
#ifdef AIM_USE_QT
 : public QThread
#endif
{
#ifdef AIM_USE_QT
Q_OBJECT
#endif

  public:
    MXA_SHARED_POINTERS(GrainGenerator);
    MXA_TYPE_MACRO(GrainGenerator);

#ifdef AIM_USE_QT
    static Pointer New (QObject* parent = 0);
#else
    MXA_STATIC_NEW_MACRO(GrainGenerator);
#endif
    virtual ~GrainGenerator();


    MXA_INSTANCE_STRING_PROPERTY(H5StatsFile)
    MXA_INSTANCE_STRING_PROPERTY(OutputDirectory)
    MXA_INSTANCE_PROPERTY(int, NumGrains)
    MXA_INSTANCE_PROPERTY(int, ShapeClass)
    MXA_INSTANCE_PROPERTY(double, XResolution)
    MXA_INSTANCE_PROPERTY(double, YResolution)
    MXA_INSTANCE_PROPERTY(double, ZResolution)
    MXA_INSTANCE_PROPERTY(double, FillingErrorWeight)
    MXA_INSTANCE_PROPERTY(double, NeighborhoodErrorWeight)
    MXA_INSTANCE_PROPERTY(double, SizeDistErrorWeight)
    MXA_INSTANCE_PROPERTY(double, FractionPrecipitates)
    MXA_INSTANCE_PROPERTY(bool, AlreadyFormed)
    MXA_INSTANCE_PROPERTY(int, Precipitates)
    MXA_INSTANCE_PROPERTY(AIM::Reconstruction::CrystalStructure, CrystalStructure)

    MXA_INSTANCE_PROPERTY(int, ErrorCondition);

    /**
     * @brief Either prints a message or sends the message to the User Interface
     * @param message The message to print
     * @param progress The progress of the GrainGenerator normalized to a value between 0 and 100
     */
    void progressMessage(AIM_STRING message, int progress);

#ifdef AIM_USE_QT

    /**
     * @brief Cancel the operation
     */
    MXA_INSTANCE_PROPERTY(bool, Cancel);

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
    GrainGenerator(QObject* parent = 0);
    virtual void run();
#else

    GrainGenerator();
#endif

  private:
    GrainGeneratorFunc::Pointer m;

    GrainGenerator(const GrainGenerator&);    // Copy Constructor Not Implemented
    void operator=(const GrainGenerator&);  // Operator '=' Not Implemented
};

#endif /* GRAINGENERATOR_H_ */
