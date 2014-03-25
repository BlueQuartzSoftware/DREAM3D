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

#include "SubFilterPipeline.h"

#include "MXA/MXA.h"
#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXADir.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SubFilterPipeline::SubFilterPipeline() :
    Observer(),
    m_ErrorCondition(0),
    m_Cancel(false)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SubFilterPipeline::~SubFilterPipeline()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SubFilterPipeline::pipelineFinished()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SubFilterPipeline::setCancel(bool value)
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
bool SubFilterPipeline::getCancel()
{
  return m_Cancel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SubFilterPipeline::run()
{
  execute();
  pipelineFinished();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SubFilterPipeline::pushFront(AbstractFilter::Pointer f)
{
  m_Pipeline.push_front(f);
  updatePrevNextFilters();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SubFilterPipeline::popFront()
{
  m_Pipeline.pop_front();
  updatePrevNextFilters();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SubFilterPipeline::pushBack(AbstractFilter::Pointer f)
{
  m_Pipeline.push_back(f);
  updatePrevNextFilters();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SubFilterPipeline::popBack()
{
  m_Pipeline.pop_back();
  updatePrevNextFilters();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SubFilterPipeline::insert(size_t index, AbstractFilter::Pointer f)
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
void SubFilterPipeline::erase(size_t index)
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
void SubFilterPipeline::clear()
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
size_t SubFilterPipeline::size()
{
  return m_Pipeline.size();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SubFilterPipeline::empty()
{
  return m_Pipeline.empty();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer SubFilterPipeline::removeFirstFilterByName(const std::string &name)
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
SubFilterPipeline::FilterContainerType& SubFilterPipeline::getFilterContainer()
{
  return m_Pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SubFilterPipeline::updatePrevNextFilters()
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
int SubFilterPipeline::preflightPipeline()
{
  // Create the DataContainer object
  VoxelDataContainer::Pointer m = getVoxelDataContainer();
  SurfaceMeshDataContainer::Pointer sm = getSurfaceMeshDataContainer();
  SolidMeshDataContainer::Pointer solid = getSolidMeshDataContainer();

  m->addObserver(static_cast<Observer*>(this));
  setErrorCondition(0);
  int preflightError = 0;
  std::stringstream ss;
  int err = 0;

  // Start looping through the Pipeline and preflight everything
  for (FilterContainerType::iterator filter = m_Pipeline.begin(); filter != m_Pipeline.end(); ++filter)
  {
    (*filter)->setVoxelDataContainer(m.get());
    (*filter)->setSurfaceMeshDataContainer(sm.get());
    (*filter)->setSolidMeshDataContainer(solid.get());
    setCurrentFilter(*filter);
    (*filter)->preflight();
    (*filter)->setVoxelDataContainer(NULL);
    (*filter)->setSurfaceMeshDataContainer(NULL);
    (*filter)->setSolidMeshDataContainer(NULL);
    std::vector<PipelineMessage> msgs = (*filter)->getPipelineMessages();
    // Loop through all the messages making sure they are all error messages. If they are all
    // warning messages we are going to let the preflight pass. Hopefully if the warning
    // turns into an error the filter will handle it correctly and gracefully fail with
    // a nice message to the user.
    for(std::vector<PipelineMessage>::iterator iter = msgs.begin(); iter != msgs.end(); ++iter)
    {
       if ( (*iter).getMessageType() == PipelineMessage::Error)
        {
          err |= (*filter)->getErrorCondition();
        }
        else if ((*iter).getMessageType() == PipelineMessage::Warning)
        {
          err |= 0;
        }
    }
    if(err < 0)
    {
      preflightError |= err;
      setErrorCondition(preflightError);
      sendPipelineMessages( (*filter)->getPipelineMessages());
    }
  }

#if 0
  std::list<std::string> cellNames = m->getCellArrayNameList();
  for (std::list<std::string>::iterator name = cellNames.begin(); name != cellNames.end(); ++name )
  {
    std::cout << *name << std::endl;
  }
#endif
  return preflightError;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SubFilterPipeline::execute()
{
  int err = 0;


  VoxelDataContainer::Pointer dataContainer = getVoxelDataContainer();
  dataContainer->addObserver(static_cast<Observer*>(this));

  SurfaceMeshDataContainer::Pointer sm = getSurfaceMeshDataContainer();
  sm->addObserver(static_cast<Observer*>(this));

  SolidMeshDataContainer::Pointer solid = getSolidMeshDataContainer();
  solid->addObserver(static_cast<Observer*>(this));

  // Start looping through the Pipeline
  float progress = 0.0f;
  std::stringstream ss;

// Start a Benchmark Clock so we can keep track of each filter's execution time
  DEFINE_CLOCK;
  START_CLOCK;
  PipelineMessage progValue("", "", 0, PipelineMessage::StatusValue, -1);
  for (FilterContainerType::iterator iter = m_Pipeline.begin(); iter != m_Pipeline.end(); ++iter)
  {
    progress = progress + 1.0f;
    progValue.setMessageType(PipelineMessage::StatusValue);
    progValue.setProgressValue(static_cast<int>( progress / (m_Pipeline.size() + 1) * 100.0f ));
    sendPipelineMessage(progValue);
    //pipelineProgress(static_cast<int>( progress / (m_Pipeline.size() + 1) * 100.0f ));

    ss.str("");
    ss << "[" << progress << "/" << m_Pipeline.size() << "] " << (*iter)->getHumanLabel() << " ";
  //  std::cout << ss.str() << std::endl;
    progValue.setMessageType(PipelineMessage::StatusMessage);
    progValue.setMessageText(ss.str());
    sendPipelineMessage(progValue);
    (*iter)->setMessagePrefix(ss.str());
    (*iter)->addObserver(static_cast<Observer*>(this));
    (*iter)->setVoxelDataContainer(dataContainer.get());
    (*iter)->setSurfaceMeshDataContainer(sm.get());
    (*iter)->setSolidMeshDataContainer(solid.get());
    setCurrentFilter(*iter);
    (*iter)->execute();
    (*iter)->removeObserver(static_cast<Observer*>(this));
    (*iter)->setVoxelDataContainer(NULL);
    (*iter)->setSurfaceMeshDataContainer(NULL);
    (*iter)->setSolidMeshDataContainer(NULL);
    err = (*iter)->getErrorCondition();
    if(err < 0)
    {
      setErrorCondition(err);
      sendPipelineMessages((*iter)->getPipelineMessages());
      progValue.setMessageType(PipelineMessage::Error);
      progValue.setProgressValue(100);
      sendPipelineMessage(progValue);
      pipelineFinished();
      return;
    }
    if (this->getCancel() == true)
    {
      break;
    }
    ss.str("");
     ss << (*iter)->getNameOfClass() << " Filter Complete";
    END_CLOCK(ss.str());
  }

  PipelineMessage completMessage("", "Pipeline Complete", 0, PipelineMessage::StatusMessage, -1);
  sendPipelineMessage(completMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SubFilterPipeline::printFilterNames(std::ostream &out)
{
  out << "---------------------------------------------------------------------" << std::endl;
  for (FilterContainerType::iterator iter = m_Pipeline.begin(); iter != m_Pipeline.end(); ++iter )
  {
    out << (*iter)->getNameOfClass() << std::endl;
  }
  out << "---------------------------------------------------------------------" << std::endl;
}
