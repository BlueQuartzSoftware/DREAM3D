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

#include "DataContainerBundle.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerBundle::DataContainerBundle()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerBundle::~DataContainerBundle()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerBundle::setDataContainers(QVector<DataContainer::Pointer> &containers)
{
  m_DataContainers = containers;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> DataContainerBundle::getDataContainerNames()
{
  QVector<QString> dcNames;
  for (size_t i=0;i<m_DataContainers.size();i++)
  {
    dcNames.push_back(m_DataContainers[i]->getName());
  }
  return dcNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerBundle::append(DataContainer::Pointer dc)
{
  m_DataContainers.append(dc);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerBundle::prepend(DataContainer::Pointer dc)
{
  m_DataContainers.prepend(dc);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerBundle::remove(DataContainer::Pointer dc)
{
  for(qint32 i = 0; i < m_DataContainers.size(); i++)
  {
    if(m_DataContainers[i].get() == dc.get())
    {
      m_DataContainers.remove(i);
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerBundle::remove(const QString &name)
{
  for(qint32 i = 0; i < m_DataContainers.size(); i++)
  {
    if(m_DataContainers[i]->getName().compare(name) == 0)
    {
      m_DataContainers.remove(i);
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerBundle::remove(qint32 i)
{
  m_DataContainers.remove(i);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerBundle::pop_back()
{
  m_DataContainers.pop_back();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerBundle::pop_front()
{
  m_DataContainers.pop_front();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::Pointer DataContainerBundle::value(qint32 index)
{
  return m_DataContainers.value(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
qint32 DataContainerBundle::count()
{
  return m_DataContainers.count();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerBundle::clear()
{
  return m_DataContainers.clear();
}
