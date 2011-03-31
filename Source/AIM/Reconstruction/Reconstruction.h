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

#ifndef _RECONSTRUCTION_H_
#define _RECONSTRUCTION_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


#include <MXA/Common/MXASetGetMacros.h>
#include <MXA/MXATypes.h>
#include "AIM/Common/Constants.h"
#include "AIM/Common/ReconstructionFunc.h"
#include "AIM/ANG/AngConstants.h"

#ifdef AIM_USE_QT
#include <QtCore/QObject>

#define AIM_STRING QString
#else
#define AIM_STRING std::string
#endif

/**
* @class Reconstruction Reconstruction AIM/Reconstruction.h/Reconstruction.h
* @brief This class serves as the main entry point to execute the reconstruction codes
* When used from a Qt framework this class inherits from QThread thus making this class able to be excuted
* on another thread of execution so that the User interface does not lock up while the code executes. The main method
* to implement is the 'compute()' method. This method will be called from the 'run()' method during the execution of
* the thread. The normal constructor is protected so that the end programmer must instantiate this class through the
* static "New()" method which will produce a boost::shared_ptr instance.
* @author Michael A. Jackson for BlueQuartz Software, Dr. Michael Groeber, USAFRL
* @date Nov 3, 2009
* @version 1.0
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


    MXA_INSTANCE_STRING_PROPERTY(H5AngFile)
    MXA_INSTANCE_PROPERTY(int, ZStartIndex)
    MXA_INSTANCE_PROPERTY(int, ZEndIndex)
    MXA_INSTANCE_STRING_PROPERTY(OutputDirectory)
    MXA_INSTANCE_PROPERTY(bool, MergeTwins)
    MXA_INSTANCE_PROPERTY(bool, MergeColonies)
    MXA_INSTANCE_PROPERTY(bool, FillinSample)
    MXA_INSTANCE_PROPERTY(int32_t, MinAllowedGrainSize)
    MXA_INSTANCE_PROPERTY(double, MinSeedConfidence)
    MXA_INSTANCE_PROPERTY(double, SizeBinStepSize)
    MXA_INSTANCE_PROPERTY(double, DownSampleFactor)
    MXA_INSTANCE_PROPERTY(double, MinSeedImageQuality)
    MXA_INSTANCE_PROPERTY(double, MisorientationTolerance)
//    MXA_INSTANCE_PROPERTY(vector<AIM::Reconstruction::CrystalStructure>, CrystalStructure)
    MXA_INSTANCE_PROPERTY(AIM::Reconstruction::AlignmentMethod, AlignmentMethod)
    MXA_INSTANCE_PROPERTY(bool, AlreadyFormed)
    MXA_INSTANCE_PROPERTY(Ang::Orientation, Orientation)

    MXA_INSTANCE_PROPERTY(bool, WriteVisualizationFile)
    MXA_INSTANCE_PROPERTY(bool, WriteIPFFile)
    MXA_INSTANCE_PROPERTY(bool, WriteDisorientationFile)
    MXA_INSTANCE_PROPERTY(bool, WriteImageQualityFile)
    MXA_INSTANCE_PROPERTY(bool, WriteSchmidFactorFile)
    MXA_INSTANCE_PROPERTY(bool, WriteDownSampledFile)
    MXA_INSTANCE_PROPERTY(bool, WriteHDF5GrainFile)

    MXA_INSTANCE_PROPERTY(int, ErrorCondition)

    /**
     * @brief Cancel the operation
     */
    MXA_INSTANCE_PROPERTY(bool, Cancel);
  
    /**
    * @brief
    * @param ostream
    */
    void printSettings(std::ostream &ostream);

    /**
     * @brief Either prints a message or sends the message to the User Interface
     * @param message The message to print
     * @param progress The progress of the Reconstruction normalized to a value between 0 and 100
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

    Reconstruction(
#ifdef AIM_USE_QT
        QObject* parent = 0
#endif
        );


  private:
    ReconstructionFunc::Pointer m;
    
    Reconstruction(const Reconstruction&);    // Copy Constructor Not Implemented
    void operator=(const Reconstruction&);  // Operator '=' Not Implemented
};

#endif /* RECONSTRUCTION_H_ */
