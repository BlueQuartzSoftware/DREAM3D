/* ============================================================================
 * Copyright (c) 2014 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2014 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
 #include "TimeSeriesBundle.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TimeSeriesBundle::TimeSeriesBundle()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TimeSeriesBundle::~TimeSeriesBundle()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TimeSeriesBundle::append(DataContainer::Pointer dc)
{
  m_DataContainers.append(dc);
  m_TimeIndices.append(-1);
  m_TimeSteps.append(-1);
  m_RealTimeSteps.append(-1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TimeSeriesBundle::prepend(DataContainer::Pointer dc)
{
  m_DataContainers.prepend(dc);
  m_TimeIndices.prepend(-1);
  m_TimeSteps.prepend(-1);
  m_RealTimeSteps.prepend(-1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TimeSeriesBundle::append(DataContainer::Pointer dc, size_t index, size_t timeStep, float realTimeStep)
{
  m_DataContainers.append(dc);
  m_TimeIndices.append(index);
  m_TimeSteps.append(timeStep);
  m_RealTimeSteps.append(realTimeStep);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TimeSeriesBundle::prepend(DataContainer::Pointer dc, size_t index, size_t timeStep, float realTimeStep)
{
  m_DataContainers.prepend(dc);
  m_TimeIndices.prepend(index);
  m_TimeSteps.prepend(timeStep);
  m_RealTimeSteps.prepend(realTimeStep);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TimeSeriesBundle::remove(DataContainer::Pointer dc)
{
  for(qint32 i = 0; i < m_DataContainers.size(); i++)
  {
    if(m_DataContainers[i].get() == dc.get())
    {
      m_DataContainers.remove(i);
      m_TimeIndices.remove(i);
      m_TimeSteps.remove(i);
      m_RealTimeSteps.remove(i);
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TimeSeriesBundle::remove(const QString &name)
{
  for(qint32 i = 0; i < m_DataContainers.size(); i++)
  {
    if(m_DataContainers[i]->getName().compare(name) == 0)
    {
      m_DataContainers.remove(i);
      m_TimeIndices.remove(i);
      m_TimeSteps.remove(i);
      m_RealTimeSteps.remove(i);
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TimeSeriesBundle::remove(qint32 i)
{
  m_DataContainers.remove(i);
  m_TimeIndices.remove(i);
  m_TimeSteps.remove(i);
  m_RealTimeSteps.remove(i);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TimeSeriesBundle::removeIndexedDataContainer(size_t index)
{
  for(qint32 i = 0; i < m_DataContainers.size(); i++)
  {
    if(m_TimeIndices[i] == index)
    {
      m_DataContainers.remove(i);
      m_TimeIndices.remove(i);
      m_TimeSteps.remove(i);
      m_RealTimeSteps.remove(i);
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TimeSeriesBundle::removeTimeStepDataContainer(size_t timeStep)
{
  for(qint32 i = 0; i < m_DataContainers.size(); i++)
  {
    if(m_TimeSteps[i] == timeStep)
    {
      m_DataContainers.remove(i);
      m_TimeIndices.remove(i);
      m_TimeSteps.remove(i);
      m_RealTimeSteps.remove(i);
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TimeSeriesBundle::pop_back()
{
  m_DataContainers.pop_back();
  m_TimeIndices.pop_back();
  m_TimeSteps.pop_back();
  m_RealTimeSteps.pop_back();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TimeSeriesBundle::pop_front()
{
  m_DataContainers.pop_front();
  m_TimeIndices.pop_front();
  m_TimeSteps.pop_front();
  m_RealTimeSteps.pop_front();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::Pointer TimeSeriesBundle::value(qint32 index)
{
  return m_DataContainers.value(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t TimeSeriesBundle::indexValue(qint32 index)
{
  return m_TimeIndices.value(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t TimeSeriesBundle::timeStepValue(qint32 index)
{
  return m_TimeSteps.value(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float TimeSeriesBundle::realTimeStepValue(qint32 index)
{
  return m_RealTimeSteps.value(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
qint32 TimeSeriesBundle::count()
{
  return m_DataContainers.count();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TimeSeriesBundle::clear()
{
  m_DataContainers.clear();
  m_TimeIndices.clear();
  m_TimeSteps.clear();
  m_RealTimeSteps.clear();
}
