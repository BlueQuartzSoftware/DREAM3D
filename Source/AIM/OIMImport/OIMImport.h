
#ifndef OIMIMPORT_H_
#define OIMIMPORT_H_



#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif



#include <vector>
#include <string>

#include "MXA/Common/MXASetGetMacros.h"
#include "MXA/MXATypes.h"

#include "ANG/H5AngImporter.h"

#include "AIM/Common/AIMCommonConfiguration.h"
#include "AIM/Common/Constants.h"


#ifdef AIM_USE_QT
#include <QtCore/QObject>

#define AIM_STRING QString
#else
#define AIM_STRING std::string
#endif


/**
 * @class OIMImport OIMImport.h AIM/ANG/OIMImport.h
 * @brief
 * @author Michael A. Jackson for BlueQuartz Software
 * @date March 23, 2011
 * @version 1.2
 *
 */
class OIMImport
#ifdef AIM_USE_QT
 : public QObject
#endif
{
#ifdef AIM_USE_QT
Q_OBJECT
#endif

  public:
    MXA_SHARED_POINTERS(OIMImport)
    MXA_TYPE_MACRO(OIMImport)

#ifdef AIM_USE_QT
    static Pointer New (QObject* parent = 0);
#else
    MXA_STATIC_NEW_MACRO(OIMImport)
#endif

    virtual ~OIMImport();

    MXA_INSTANCE_STRING_PROPERTY(OutputFile)
    MXA_INSTANCE_PROPERTY(int, ZStartIndex)
    MXA_INSTANCE_PROPERTY(int, ZEndIndex)
    MXA_INSTANCE_PROPERTY(float, ZResolution)
    MXA_INSTANCE_PROPERTY(std::vector<std::string>, AngFileList);

    /**
     * @brief Cancel the operation
     */
    MXA_INSTANCE_PROPERTY(bool, Cancel);

    /**
     * @brief Either prints a message or sends the message to the User Interface
     * @param message The message to print
     * @param progress The progress of the GrainGenerator normalized to a value between 0 and 100
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
      OIMImport(
  #ifdef AIM_USE_QT
          QObject* parent = 0
  #endif
      );


  private:
    OIMImport(const OIMImport&); // Copy Constructor Not Implemented
    void operator=(const OIMImport&); // Operator '=' Not Implemented
};


#endif /* OIMIMPORT_H_ */
