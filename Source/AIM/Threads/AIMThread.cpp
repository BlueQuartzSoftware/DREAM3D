///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////

#include "AIMThread.h"
#include <AIM/Threads/AIMThreadGroup.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMThread::Pointer AIMThread::New(int32 threadSlot, QString tname,
                                QObject* parent)
{
  Pointer sharedPtr(new AIMThread(parent));
  sharedPtr->setThreadSlot(threadSlot);
  sharedPtr->setThreadName(tname);
  return sharedPtr;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMThread::AIMThread(QObject* parent) :
QThread(parent),
m_Cancel(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMThread::~AIMThread()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMThread::run()
{
  exec();
  emit threadFinished(getThreadSlot());
}

void AIMThread::on_KillIt()
{
  std::cout << "AIMThread::on_KillIt()" << std::endl;
  emit threadFinished(getThreadSlot());
}

