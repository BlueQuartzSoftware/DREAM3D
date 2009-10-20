///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////

#include "AIMThreadGroup.h"

//-- Qt Thread Related Stuff
#include <QtCore/QThread>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMThreadGroup::Pointer AIMThreadGroup::New(int maxThreads)
{
  AIMThreadGroup::Pointer sharedPtr(new AIMThreadGroup(maxThreads));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMThreadGroup::AIMThreadGroup(int maxThreads) :
  m_maxThreads(maxThreads), m_shouldDeleteThreads(true)
{
  m_completedThreads = new char[maxThreads];
  ::memset(m_completedThreads, 0, maxThreads);
  m_threads.resize(maxThreads);
  for (int i = 0; i < maxThreads; ++i)
  {
    m_threads[i] = AIMThread::NullPointer();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMThreadGroup::~AIMThreadGroup()
{
  //std::cout << logTime() << " ~AIMThreadGroup()" << std::endl;
  AIMThread::Pointer currentThread = AIMThread::NullPointer();
  for (int slot = 0; slot < this->m_maxThreads; ++slot)
  {
    currentThread = _remove_thread(slot);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AIMThreadGroup::getMaxThreads()
{
  return m_maxThreads;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AIMThreadGroup::waitForOpenSlot()
{
  //std::cout << logTime() << "AIMThreadGroup::waitForOpenSlot()" << std::endl;
  AIMThread::Pointer currentThread = AIMThread::NullPointer();
  int32 openSlot = -1;
  while (openSlot == -1)
  {
    QMutexLocker lk(&m_monitor);
    for (int slot = 0; slot < this->m_maxThreads; ++slot)
    {
      if (m_completedThreads[slot] == 1)
      {
        currentThread = _remove_thread(slot);
        currentThread->wait(); // Wait for the thread to be truly finished
      }
    }

    for (int slot = 0; slot < this->m_maxThreads; ++slot)
    {
      if (m_threads[slot].get() == NULL)
      {
        openSlot = slot;
        break;
      }
    }
    if (openSlot == -1) // No open slots available, so wait for one to open
    {
      //std::cout << logTime() << "   AIMThreadGroup::waitForOpenSlot() --- waiting for thread slot to become available" << std::endl;
      m_appShouldWait.wait(&m_monitor);
    }
  }
  // std::cout << logTime() << "  AIMThreadGroup::waitForOpenSlot() --- Done Waiting for a slot: " << openSlot << std::endl;
  return openSlot;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMThreadGroup::startThread(AIMThread::Pointer thrd)
{
  int slot = waitForOpenSlot();
  QMutexLocker lk(&m_monitor);
  thrd->setThreadSlot(slot);
  m_threads[slot] = thrd;
  m_completedThreads[slot] = 0;
  thrd->start();
}

void AIMThreadGroup::addThread(AIMThread::Pointer thrd, int slot)
{
  if (thrd.get() != NULL)
  {
    QMutexLocker lk(&m_monitor);
    m_threads[slot] = thrd;
    m_completedThreads[slot] = 0;
  }
}

void AIMThreadGroup::removeThread(AIMThread::Pointer thread)
{
  QMutexLocker lk(&m_monitor);
  for (int i = 0; i < m_maxThreads; ++i)
  {
    if (m_threads[i] == thread)
    {
      m_threads[i] = AIMThread::NullPointer();
      m_completedThreads[i] = 0;
    }
  }
}

AIMThread::Pointer AIMThreadGroup::getThread(int32 slot)
{
  QMutexLocker lk(&m_monitor);
  return m_threads[slot];
}

void AIMThreadGroup::joinAll()
{
  for (int slot = 0; slot < this->m_maxThreads; ++slot)
  {
    QMutexLocker lk(&m_monitor);
    if (NULL != m_threads[slot])
    {
      while (m_completedThreads[slot] == 0)
      {
        // std::cout << logTime() << "  AIMThreadGroup::join_all Waiting for thread to complete: slot=" << slot << std::endl;
        m_appShouldWait.wait(&m_monitor);
      }
      // std::cout << logTime() << " AIMThreadGroup::join_all Joining slot " << slot << std::endl;
      m_threads[slot]->wait();
    }
  }
}

void AIMThreadGroup::cancelAll()
{
  QMutexLocker lk(&m_monitor);
  for (int slot = 0; slot < this->m_maxThreads; ++slot)
  {
    if (NULL != m_threads[slot])
    {
      m_threads[slot]->setCancel(true);
    }
  }
}

size_t AIMThreadGroup::size()
{
  QMutexLocker lk(&m_monitor);
  return m_threads.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMThreadGroup::markThreadComplete(int slot)
{
 // std::cout << logTime() << "AIMThreadGroup::markThreadComplete(" << slot << ")" << std::endl;
  {
    QMutexLocker lk(&m_monitor);
    m_completedThreads[slot] = 1;
  }
  m_appShouldWait.wakeOne();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMThread::Pointer AIMThreadGroup::_remove_thread(int slot)
{
  //std::cout << logTime() << "AIMThreadGroup::_remove_thread(" << slot << ", " << delete_thread << ")" << std::endl;
  AIMThread::Pointer thrd = m_threads[slot];
  m_threads[slot] = AIMThread::NullPointer();
  m_completedThreads[slot] = 0;
  return thrd;
}

