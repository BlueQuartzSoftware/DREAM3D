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
void Observable::setMessagePrefix(const std::string& str)
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
void Observable::notifyMessage(PipelineMessage& msg)
{
  // If the programmer set a prefix (which FilterPipeline does) we are going to
  // use the prefix in place of the 'FilterName' because this gives us more
  // information to use and display to the user.
  if (m_Prefix.empty() == false)
  {
    msg.setMessagePrefix(m_Prefix);
  }
  for (std::vector<Observer*>::iterator iter = m_Observers.begin(); iter != m_Observers.end(); ++iter)
  {
    (*iter)->sendPipelineMessage(msg);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observable::notifyErrorMessage(std::string errDesc, int errCode)
{
  PipelineMessage errorMsg(getNameOfClass(), errDesc, errCode, PipelineMessage::Error);
  notifyMessage(errorMsg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observable::notifyWarningMessage(std::string warnDesc, int warnCode)
{
  PipelineMessage warningMsg(getNameOfClass(), warnDesc, warnCode, PipelineMessage::Warning);
  notifyMessage(warningMsg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observable::notifyStatusMessage(std::string statusDesc)
{
  PipelineMessage statusMsg(getNameOfClass(), statusDesc, 0, PipelineMessage::StatusMessage);
  notifyMessage(statusMsg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observable::notifyProgressValue(int statusVal)
{
  PipelineMessage statusValueUpdate(getNameOfClass(), "", 0, PipelineMessage::StatusValue, statusVal);
  notifyMessage(statusValueUpdate);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observable::notifyStatusAndProgress(std::string statusDesc, int statusVal)
{
  PipelineMessage statusUpdate(getNameOfClass(), statusDesc, 0, PipelineMessage::StatusMessageAndValue, statusVal);
  notifyMessage(statusUpdate);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//void Observable::notifyMessage(std::vector<ErrorMessage::Pointer> messages, int progress, ObserverAspect a)
//{
//  for (std::vector<ErrorMessage::Pointer>::iterator iter = messages.begin(); iter != messages.end(); ++iter)
//  {
//    notifyMessage(*iter, progress, a);
//  }
//}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<Observer*> Observable::getObservers()
{
  return this->m_Observers;
}
#if 0
void Observable::getObservers(std::vector<Observer*>& observers)
{
  observers.resize(m_Observers.size());
  for(size_t i = 0; i < m_Observers.size(); ++i)
  {
    observers[i] = m_Observers[i];
  }
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Observable::setObservers(std::vector<Observer*> obs)
{
  this->m_Observers = obs;
}

