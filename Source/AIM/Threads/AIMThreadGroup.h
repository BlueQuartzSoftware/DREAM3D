///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////

#ifndef AIMTHREADGROUP_H_
#define AIMTHREADGROUP_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


//-- MXA Includes
#include <MXA/MXATypes.h>
#include <MXA/Common/LogTime.h>
#include <MXA/Common/MXASetGetMacros.h>

#include <AIM/Threads/AIMThread.h>

//-- C++ includes
#include <list>
#include <vector>

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>

/**
 *
 */

class AIMThreadGroup : public QObject
{
    Q_OBJECT

  public:

     MXA_SHARED_POINTERS(AIMThreadGroup)
     MXA_TYPE_MACRO(AIMThreadGroup)


     /**
      * @brief Static new method which should be used to instantiate this class
      * @param maxThreads The maximum concurrent threads to allow at any given time
      * @return boost::shared_ptr to the thread Group
      */
     static Pointer New(int maxThreads);

     virtual ~AIMThreadGroup();

     /**
      * @brief Returns the maximum number of threads this group is managing.
      * @return
      */
     int getMaxThreads();

     /**
      * @brief Waits for an open slot to start another thread.
      * @return The value of the open slot.
      */
     int waitForOpenSlot();

     /**
      * @brief Creates a new thread using the class T as the function to run on
      * the thread.
      * @param t The function to run on the thread.
      * @return
      */
     void startThread(AIMThread::Pointer thrd);

     /**
      * @brief Adds a thread to this group.
      * @param thrd The thread to add
      * @param slot The slot number to use
      */
     void addThread(AIMThread::Pointer thrd, int slot);

     /**
      * @brief Removes a thread from the thread group.
      * @param thread The thread to remove from the thread group.
      */
     void removeThread(AIMThread::Pointer thread);

     /**
      * @brief Returns the thread object associated with the slot
      * @param slot
      * @return
      */
     AIMThread::Pointer getThread(qint32 slot);

     /**
      * @brief calls "join()" on all the threads currently running and managed
      * by this class.
      */
     void joinAll();

     /**
      * @brief Calls interrupt() on all the threads being managed by this class.
      */
     void cancelAll();

     /**
      * @brief Returns the total number of threads this class can manage.
      * @return
      */
     size_t size();

     /**
      * @brief Sets a flag to mark a specific thread as having completed its execution.
      * @param slot
      */


   public slots:
     void markThreadComplete(int slot);

   protected:
     /**
      * @brief Protected constructor as the Static New method should be use instead.
      * @param maxThreads The maximum number of threads to run concurrently
      * @return
      */
     AIMThreadGroup(int maxThreads);

   private:
     int                        m_maxThreads;
     char*                      m_completedThreads;
     std::vector<AIMThread::Pointer>    m_threads;

     QMutex                     m_monitor;
     QWaitCondition             m_appShouldWait;
     bool                       m_shouldDeleteThreads;

 /**
  * @brief Removes a thread and optionally 'delete's  the thread from this group.
  * @param slot The slot number where the thread is residing
  * @param delete_thread Should delete be called on the thread pointer.
  * @return A pointer to the thread that was removed or NULL if the delete_thread flag was true.
  */
     AIMThread::Pointer _remove_thread(int slot);


 };


#endif /* AIMTHREADGROUP_H_ */
