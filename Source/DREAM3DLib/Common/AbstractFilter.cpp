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
m_VoxelDataContainer(NULL),
m_SurfaceMeshDataContainer(NULL),
m_SolidMeshDataContainer(NULL),
m_ErrorCondition(0),
m_Cancel(false)
{
  setupFilterParameters();
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
void AbstractFilter::setupFilterParameters()
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
  setErrorCondition(-999);
  notifyErrorMessage("AbstractFilter does not implement a preflight method. Please use a subclass instead.", -999);
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
void AbstractFilter::addCreatedArrayHelpIndexEntry(CreatedArrayHelpIndexEntry::Pointer entry)
{
  m_CreatedArrayHelpIndexEntries.push_back(entry);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbstractFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  BOOST_ASSERT(writer != NULL);
  std::cout << "AbstractFilter::writeFilterParameters() -> Writing Filter Options" << std::endl;
  return -1;
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
void AbstractFilter::addErrorMessage(const std::string &filterHumanLabel, const std::string &errorDescription, int errorCode)
{
  PipelineMessage em(getNameOfClass(), errorDescription, errorCode, PipelineMessage::Error);
  em.setFilterHumanLabel(getHumanLabel());
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
  msg.setFilterHumanLabel(getHumanLabel());
  msg.setFilterClassName(getNameOfClass());
  m_PipelineMessages.push_back(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::addWarningMessage(const std::string &filterName, const std::string &warnDescription, int warnCode)
{
  PipelineMessage em(getNameOfClass(), warnDescription, warnCode, PipelineMessage::Warning);
  em.setFilterHumanLabel(getHumanLabel());
  em.setFilterClassName(getNameOfClass());
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractFilter::tbbTaskProgress()
{

}


