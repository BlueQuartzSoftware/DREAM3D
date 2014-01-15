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

#include "FilterPipeline.h"
#include "DREAM3DLib/DataContainers/DataContainerArray.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::FilterPipeline() :
  QObject(),
  m_ErrorCondition(0),
  m_Cancel(false)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::~FilterPipeline()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::pipelineFinished()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::setCancel(bool value)
{
  this->m_Cancel = value;
  if (NULL != m_CurrentFilter.get())
  {
    m_CurrentFilter->setCancel(value);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterPipeline::getCancel()
{
  return m_Cancel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::run()
{
  execute();
  pipelineFinished();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::pushFront(AbstractFilter::Pointer f)
{
  m_Pipeline.push_front(f);
  updatePrevNextFilters();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::popFront()
{
  m_Pipeline.pop_front();
  updatePrevNextFilters();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::pushBack(AbstractFilter::Pointer f)
{
  m_Pipeline.push_back(f);
  updatePrevNextFilters();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::popBack()
{
  m_Pipeline.pop_back();
  updatePrevNextFilters();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::insert(size_t index, AbstractFilter::Pointer f)
{
  FilterContainerType::iterator it = m_Pipeline.begin();
  for(size_t i = 0; i < index; ++i)
  {
    ++it;
  }
  m_Pipeline.insert(it, f);
  updatePrevNextFilters();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::erase(size_t index)
{
  FilterContainerType::iterator it = m_Pipeline.begin();
  for(size_t i = 0; i < index; ++i)
  {
    ++it;
  }
  m_Pipeline.erase(it);
  updatePrevNextFilters();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::clear()
{
  for (FilterContainerType::iterator iter = m_Pipeline.begin(); iter != m_Pipeline.end(); ++iter)
  {
    (*iter)->setPreviousFilter(AbstractFilter::NullPointer());
    (*iter)->setNextFilter(AbstractFilter::NullPointer());
  }
  m_Pipeline.clear();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t FilterPipeline::size()
{
  return m_Pipeline.size();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterPipeline::empty()
{
  return m_Pipeline.isEmpty();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FilterPipeline::removeFirstFilterByName(const QString& name)
{
  AbstractFilter::Pointer f = AbstractFilter::NullPointer();
  for(FilterContainerType::iterator it = m_Pipeline.begin(); it != m_Pipeline.end(); ++it)
  {
    if((*it)->getHumanLabel().compare(name) == 0)
    {
      f = *it;
      m_Pipeline.erase(it);
      break;
    }
  }
  updatePrevNextFilters();

  return f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::FilterContainerType& FilterPipeline::getFilterContainer()
{
  return m_Pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::updatePrevNextFilters()
{
  FilterContainerType::iterator prev;
  FilterContainerType::iterator next;

  for (FilterContainerType::iterator iter = m_Pipeline.begin(); iter != m_Pipeline.end(); ++iter)
  {
    // currFilt = *iter;
    if(iter != m_Pipeline.begin())
    {
      prev = iter;
      prev--;
      // prevFilt = *prev;
      (*iter)->setPreviousFilter(*prev);
    }

    if(iter != m_Pipeline.end())
    {
      next = iter;
      next++;
      //  nextFilt = *next;
      if(next != m_Pipeline.end()) { (*iter)->setNextFilter(*next); }
    }
  }
  int index = 0;
  for (FilterContainerType::iterator filter = m_Pipeline.begin(); filter != m_Pipeline.end(); ++filter)
  {
    (*filter)->setPipelineIndex(index++);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::addMessageReceiver(QObject* obj)
{
  m_MessageReceivers.push_back(obj);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::connectFilterNotifications(QObject* filter)
{
  for(int i = 0; i < m_MessageReceivers.size(); i++)
  {
    connect(filter, SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
            m_MessageReceivers.at(i), SLOT(processPipelineMessage(const PipelineMessage&)) );
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::disconnectFilterNotifications(QObject* filter)
{
  for(int i = 0; i < m_MessageReceivers.size(); i++)
  {
    disconnect(filter, SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
               m_MessageReceivers.at(i), SLOT(processPipelineMessage(const PipelineMessage&)) );
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FilterPipeline::preflightPipeline()
{
  // Create the DataContainer object
  DataContainerArray::Pointer dca = DataContainerArray::New();

  setErrorCondition(0);
  int preflightError = 0;

  // Start looping through each filter in the Pipeline and preflight everything
  for (FilterContainerType::iterator filter = m_Pipeline.begin(); filter != m_Pipeline.end(); ++filter)
  {
    (*filter)->setDataContainerArray(dca);
    setCurrentFilter(*filter);
    connectFilterNotifications( (*filter).get() );
    //qDebug() << "Preflighting Filter [" << (*filter)->getHumanLabel() << "] (" << (*filter)->getNameOfClass() << ")";
    (*filter)->preflight();
    disconnectFilterNotifications( (*filter).get() );

    (*filter)->setDataContainerArray(DataContainerArray::NullPointer());
    preflightError |= (*filter)->getErrorCondition();
  }
  setCurrentFilter(AbstractFilter::NullPointer());
  return preflightError;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::execute()
{
  int err = 0;

  DataContainerArray::Pointer dca = DataContainerArray::New();

  // Start looping through the Pipeline
  float progress = 0.0f;

  // Connect this object to anything that wants to know about PipelineMessages
  for(int i = 0; i < m_MessageReceivers.size(); i++)
  {
    connect(this, SIGNAL(pipelineGeneratedMessage(const PipelineMessage&)),
            m_MessageReceivers.at(i), SLOT(processPipelineMessage(const PipelineMessage&)) );
  }

  // Start a Benchmark Clock so we can keep track of each filter's execution time
  PipelineMessage progValue("", "", 0, PipelineMessage::ProgressValue, -1);
  for (FilterContainerType::iterator iter = m_Pipeline.begin(); iter != m_Pipeline.end(); ++iter)
  {
    progress = progress + 1.0f;
    progValue.setType(PipelineMessage::ProgressValue);
    progValue.setProgressValue(static_cast<int>( progress / (m_Pipeline.size() + 1) * 100.0f ));
    emit pipelineGeneratedMessage(progValue);

    QString ss = QObject::tr("[%1/%2] %3 ").arg(progress).arg(m_Pipeline.size()).arg( (*iter)->getHumanLabel());

    progValue.setType(PipelineMessage::StatusMessage);
    progValue.setText(ss);
    emit pipelineGeneratedMessage(progValue);


    (*iter)->setMessagePrefix(ss);
    connectFilterNotifications( (*iter).get() );
    (*iter)->setDataContainerArray(dca);
    setCurrentFilter(*iter);
    (*iter)->execute();
    disconnectFilterNotifications( (*iter).get() );
    err = (*iter)->getErrorCondition();
    if(err < 0)
    {
      setErrorCondition(err);

      progValue.setType(PipelineMessage::Error);
      progValue.setProgressValue(100);
      emit pipelineGeneratedMessage(progValue);

      pipelineFinished();
      return;
    }
    if (this->getCancel() == true)
    {
      break;
    }
    ss = QObject::tr("%1 Filter Complete").arg((*iter)->getNameOfClass());
  }

  PipelineMessage completMessage("", "Pipeline Complete", 0, PipelineMessage::StatusMessage, -1);
  emit pipelineGeneratedMessage(completMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::printFilterNames(QTextStream& out)
{
  out << "---------------------------------------------------------------------" ;
  for (FilterContainerType::iterator iter = m_Pipeline.begin(); iter != m_Pipeline.end(); ++iter )
  {
    out << (*iter)->getNameOfClass() << "\n";
  }
  out << "---------------------------------------------------------------------" ;
}
