///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////

#ifndef AIMTHREAD_H_
#define AIMTHREAD_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


#include <MXA/Common/MXASetGetMacros.h>
#include <MXA/MXATypes.h>

//-- Qt Includes
#include <QtCore/QThread>


class AIMThread : public QThread
{
    Q_OBJECT

  public:

    MXA_SHARED_POINTERS(AIMThread)
    MXA_TYPE_MACRO(AIMThread)



    static Pointer New(qint32 threadSlot,
                       QString tname,
                       QObject* parent = 0);

    virtual ~AIMThread();

    signals:
      /**
       * @brief Use this signal to send a percentage of completeness to the GUI thread
       */
      void threadProgressed(float progress);

      /**
       * @brief Use this signal to send a text message that will be displayed in the GUI
       */
      void threadHasMessage(QString message);

      /**
       * @brief Use this signal when the import is completed.
       */
      void threadFinished(int slot);

  public slots:
      void on_KillIt();


  public:

  /**
     * @brief Sets the value of the slot in the R3DThreadGroup that holds the pointer
     * to this thread.
     */
    MXA_INSTANCE_PROPERTY(qint32, ThreadSlot);

    /**
     * Sets the name of the thread which should be unique
     */
    MXA_INSTANCE_PROPERTY(QString, ThreadName)

    /**
     * @brief Sets and gets any error condition that might be noteworthy during the
     * execution of the thread.
     */
    MXA_INSTANCE_PROPERTY(qint32, ErrorCondition);

    /**
     * @brief Should this thread try to nicely cancel itself
     */
    MXA_INSTANCE_PROPERTY(bool, Cancel);


  protected:
    AIMThread(QObject* parent);

  void run();

  private:
    AIMThread(const AIMThread&);    // Copy Constructor Not Implemented
    void operator=(const AIMThread&);  // Operator '=' Not Implemented

};

#endif /* AIMTHREAD_H_ */
