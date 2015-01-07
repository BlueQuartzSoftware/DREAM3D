/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "Observable.h"
#include "Observer.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Observable::Observable()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Observable::~Observable()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observable::addObserver(Observer* observer)
{
  m_Observers.push_back(observer);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observable::removeObserver(Observer* observer)
{
  for (std::vector<Observer*>::iterator iter = m_Observers.begin(); iter != m_Observers.end(); ++iter )
  {
    if ((*iter) == observer)
    {
      m_Observers.erase(iter);
      break;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observable::setMessagePrefix(const std::string &str)
{
  m_Prefix = str;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string Observable::getMessagePrefix()
{
  return m_Prefix;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observable::notify(const std::string msg, int progress, ObserverAspect a)
{
  std::string s = msg;
  if(m_Prefix.empty() == false)
  {
    s = m_Prefix + msg;
  }
  for (std::vector<Observer*>::iterator iter = m_Observers.begin(); iter != m_Observers.end(); ++iter)
  {
    switch(a)
    {
      case UpdateProgressValue:
        (*iter)->pipelineProgress(progress);
        break;
      case UpdateProgressMessage:
        (*iter)->pipelineProgressMessage(s);
        break;
      case UpdateWarningMessage:
        (*iter)->pipelineWarningMessage(s);
        break;
      case UpdateErrorMessage:
        (*iter)->pipelineErrorMessage(s);
        break;
      case UpdateProgressValueAndMessage:
        (*iter)->updateProgressAndMessage(s, progress);
        break;
      default:
        break;
    }
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observable::notify(const char* msg, int progress, ObserverAspect a)
{
  std::string m(msg);
  notify(m, progress, a);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<Observer*> Observable::getObservers()
{
  return this->m_Observers;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observable::setObservers(std::vector<Observer*> obs)
{
  this->m_Observers = obs;
}

