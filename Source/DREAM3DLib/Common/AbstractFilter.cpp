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

#include "AbstractFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::AbstractFilter() :
m_DataContainer(NULL),
m_ErrorCondition(0),
m_Cancel(false)
{
  setupFilterOptions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::~AbstractFilter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::setupFilterOptions()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::execute()
{
  setErrorCondition(-999);
  notifyErrorMessage("AbstractFilter does not implement an execute method. Please use a subclass instead.", -999);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::preflight()
{
	std::cout << "AbstractFilter::Preflight needs to be added in some class";
	assert(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AbstractFilter::doesPipelineContainFilterBeforeThis(const std::string &name)
{
  bool contains = false;
  // Check the previous filter
  AbstractFilter::Pointer prev = getPreviousFilter();
  while(prev.get() != NULL)
  {
    if (prev->getNameOfClass().compare(name) == 0)
    {
      contains = true;
      break;
    }
    prev = prev->getPreviousFilter();
  }
  return contains;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AbstractFilter::doesPipelineContainFilterAfterThis(const std::string &name)
{
  bool contains = false;
  // Check the previous filter
  AbstractFilter::Pointer next = getNextFilter();
  while(next.get() != NULL)
  {
    if (next->getNameOfClass().compare(name) == 0)
    {
      contains = true;
      break;
    }
    next = next->getNextFilter();
  }
  return contains;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::addRequiredCellData(const std::string &name)
{
  m_RequiredCellData.insert(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::addCreatedCellData(const std::string &name)
{
  m_CreatedCellData.insert(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::addRequiredFieldData(const std::string &name)
{
  m_RequiredFieldData.insert(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::addCreatedFieldData(const std::string &name)
{
  m_CreatedFieldData.insert(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::addRequiredEnsembleData(const std::string &name)
{
  m_RequiredEnsembleData.insert(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::addCreatedEnsembleData(const std::string &name)
{
  m_CreatedEnsembleData.insert(name);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::writeFilterOptions(AbstractFilterOptionsWriter* writer)
{
  assert(writer != NULL);
  std::cout << "AbstractFilter::writeFilterOptions() -> Writing Filter Options" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::addErrorMessage(PipelineMessage &msg)
{
  m_PipelineMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::addErrorMessage(const std::string &filterName, const std::string &errorDescription, int errorCode)
{
  PipelineMessage em(filterName, errorDescription, errorCode, PipelineMessage::Error);
  m_PipelineMessages.push_back(em);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::addErrorMessages(std::vector<PipelineMessage> msgVector) {
  for (std::vector<PipelineMessage>::size_type i=0; i < msgVector.size(); ++i) {
    m_PipelineMessages.push_back(msgVector[i]);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::addWarningMessage(PipelineMessage &msg)
{
  m_PipelineMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::addWarningMessage(const std::string &filterName, const std::string &warnDescription, int warnCode)
{
  PipelineMessage em(filterName, warnDescription, warnCode, PipelineMessage::Warning);
  m_PipelineMessages.push_back(em);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::addWarningMessages(std::vector<PipelineMessage> msgVector) {
  for (std::vector<PipelineMessage>::size_type i=0; i < msgVector.size(); ++i) {
    m_PipelineMessages.push_back(msgVector[i]);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::removeErrorMessage(PipelineMessage msg) {
  for (std::vector<PipelineMessage>::iterator iter = m_PipelineMessages.begin(); iter!=m_PipelineMessages.end(); ++iter) {
    if (*iter == msg) {
      m_PipelineMessages.erase(iter);
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::removeErrorMessage(int index) {
  int count = 0;

  for (std::vector<PipelineMessage>::iterator iter = m_PipelineMessages.begin(); iter!=m_PipelineMessages.end(); ++iter) {
    if (count == index) {
      m_PipelineMessages.erase(iter);
      return;
    }
    count++;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::removeErrorMessages(int start, int end) {
  int count = 0;

  for (std::vector<PipelineMessage>::iterator iter = m_PipelineMessages.begin(); iter!=m_PipelineMessages.end(); ++iter) {
    if (count == start) {
      while (count <= end) {
        iter = m_PipelineMessages.erase(iter);
        count++;
      }
      return;
    }
    count++;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::clearErrorMessages() {
  std::vector<PipelineMessage>::iterator iter = m_PipelineMessages.begin();

  while ( iter != m_PipelineMessages.end() ) {
    iter = m_PipelineMessages.erase(iter);
  }
}
