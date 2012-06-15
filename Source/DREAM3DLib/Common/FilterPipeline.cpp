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

#include "MXA/MXA.h"
#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXADir.h"


#define CREATE_INPUT_FILENAME(f, n)\
    std::string f = m_InputDirectory + MXADir::Separator + n;\
    f = MXADir::toNativeSeparators(f);

#define CREATE_OUTPUT_FILENAME(f, n)\
    std::string f = m_InputDirectory + MXADir::Separator + n;\
    f = MXADir::toNativeSeparators(f);

#define DREAM3D_BENCHMARKS 0

#if DREAM3D_BENCHMARKS
#define START_CLOCK()\
  unsigned long long int millis;\
  millis = MXA::getMilliSeconds();
#else
#define START_CLOCK() unsigned long long int millis = 0;\
  millis = 0;
#endif


#define CHECK_FOR_CANCELED(FuncClass, Message, name)\
    if (this->getCancel() ) { \
              pipelineProgressMessage(#Message);\
              pipelineProgress(0);\
              pipelineFinished();\
              return;}\


#define CHECK_FOR_ERROR(FuncClass, Message, err)\
    if(err < 0) {\
      setErrorCondition(err);\
      std::string msg = std::string(Message);\
      pipelineErrorMessage(msg.c_str());\
      pipelineProgress(0);\
      pipelineFinished();\
      return;   }


#define MAKE_OUTPUT_FILE_PATH(outpath, filename)\
    std::string outpath = m_OutputDirectory + MXADir::Separator + m_OutputFilePrefix + filename;





// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::FilterPipeline() :
    Observer(),
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
  return m_Pipeline.empty();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FilterPipeline::removeFirstFilterByName(const std::string &name)
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
int FilterPipeline::preflightPipeline()
{
  // Create the DataContainer object
  DataContainer::Pointer m = DataContainer::New();
  m->addObserver(static_cast<Observer*>(this));
  setErrorCondition(0);
  int preflightError = 0;
  std::stringstream ss;


  // Start looping through the Pipeline and preflight everything
  for (FilterContainerType::iterator filter = m_Pipeline.begin(); filter != m_Pipeline.end(); ++filter)
  {
    (*filter)->setDataContainer(m.get());
    setCurrentFilter(*filter);
    (*filter)->preflight();
    int err = (*filter)->getErrorCondition();
    if(err < 0)
    {
      preflightError |= err;
      setErrorCondition(preflightError);
      setErrorCondition(err);
      pipelineErrorMessage((*filter)->getErrorMessage().c_str());
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
void FilterPipeline::execute()
{
  int err = 0;

  // Run the preflight first to make sure we can run this combination of filters
  // Inside the function we create a new DataContainer object so we don't mess
  // with any currently set DataContainer
  err = preflightPipeline();
  if(err < 0)
  {
    return;
  }

  // Create the DataContainer object
  if(NULL == m_DataContainer.get())
  {
    m_DataContainer = DataContainer::New();
  }

  m_DataContainer->addObserver(static_cast<Observer*>(this));

  // Start looping through the Pipeline
  float progress = 0.0f;
  std::stringstream ss;

// Start a Benchmark Clock so we can keep track of each filter's execution time
  START_CLOCK()

  for (FilterContainerType::iterator iter = m_Pipeline.begin(); iter != m_Pipeline.end(); ++iter)
  {
    progress = progress + 1.0f;
    pipelineProgress(progress / (m_Pipeline.size() + 1) * 100.0f);
    ss.str("");
    ss << "[" << progress << "/" << m_Pipeline.size() << "] " << (*iter)->getHumanLabel() << " ";
    pipelineProgressMessage(ss.str());
    (*iter)->setMessagePrefix(ss.str());
    (*iter)->addObserver(static_cast<Observer*>(this));
    (*iter)->setDataContainer(m_DataContainer.get());
    setCurrentFilter(*iter);
    (*iter)->execute();
    (*iter)->removeObserver(static_cast<Observer*>(this));
    (*iter)->setDataContainer(NULL);
    err = (*iter)->getErrorCondition();
    if(err < 0)
    {
      setErrorCondition(err);
      pipelineErrorMessage((*iter)->getErrorMessage().c_str());
      pipelineProgress(100);
      pipelineFinished();
      return;
    }
    CHECK_FOR_CANCELED(DataContainer, "Pipeline was canceled", write_fielddata)

    if(DREAM3D_BENCHMARKS)
    {
      std::cout << (*iter)->getNameOfClass() << " Finish Time(ms): " << (MXA::getMilliSeconds() - millis) << std::endl;
      millis = MXA::getMilliSeconds();
    }
  }
  ss.str("");
  ss << "Pipeline Complete";
  pipelineProgressMessage(ss.str());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterPipeline::printFilterNames(std::ostream &out)
{
  out << "---------------------------------------------------------------------" << std::endl;
  for (FilterContainerType::iterator iter = m_Pipeline.begin(); iter != m_Pipeline.end(); ++iter )
  {
    out << (*iter)->getNameOfClass() << std::endl;
  }
  out << "---------------------------------------------------------------------" << std::endl;
}
