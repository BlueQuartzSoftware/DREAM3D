/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "StatsDataArray.h"

#include <QtCore/QList>




#include "H5Support/QH5Utilities.h"
#include "H5Support/HDF5ScopedFileSentinel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsDataArray::StatsDataArray() :
  m_Name(DREAM3D::EnsembleData::Statistics)
{
  m_IsAllocated = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsDataArray::~StatsDataArray()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::setName(const QString& name)
{
  m_Name = name;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StatsDataArray::getName()
{
  return m_Name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StatsDataArray::getFullNameOfClass()
{
  return QString("Statistics");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::takeOwnership()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::releaseOwnership()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void* StatsDataArray::getVoidPointer(size_t i)
{
#ifndef NDEBUG
  if(m_StatsDataArray.size() > 0)
  {
    BOOST_ASSERT(i < static_cast<size_t>(m_StatsDataArray.size()));
  }
#endif
  if(i >= this->getNumberOfTuples())
  {
    return 0x0;
  }
  return (void*)(&(m_StatsDataArray[i]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t StatsDataArray::getNumberOfTuples()
{
  return m_StatsDataArray.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t StatsDataArray::getSize()
{
  return m_StatsDataArray.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsDataArray::getNumberOfComponents()
{
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<size_t> StatsDataArray::getComponentDimensions()
{
  QVector<size_t> dims(1, 1);
  return dims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t StatsDataArray::getTypeSize()
{
  return sizeof(StatsData);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsDataArray::eraseTuples(QVector<size_t>& idxs)
{
  int err = 0;

  // If nothing is to be erased just return
  if(idxs.size() == 0)
  {
    return 0;
  }

  if ( static_cast<size_t>(idxs.size()) >= getNumberOfTuples() )
  {
    resize(0);
    return 0;
  }

  // Sanity Check the Indices in the vector to make sure we are not trying to remove any indices that are
  // off the end of the array and return an error code.
  for(QVector<size_t>::size_type i = 0; i < idxs.size(); ++i)
  {
    if (idxs[i] >= static_cast<size_t>(m_StatsDataArray.size())) { return -100; }
  }


  QVector<StatsData::Pointer> replacement(m_StatsDataArray.size() - idxs.size());
  size_t idxsIndex = 0;
  size_t rIdx = 0;
  size_t size = static_cast<size_t>(m_StatsDataArray.size());
  for(size_t dIdx = 0; dIdx < size; ++dIdx)
  {
    if (dIdx != idxs[idxsIndex])
    {
      replacement[rIdx] = m_StatsDataArray[dIdx];
      ++rIdx;
    }
    else
    {
      ++idxsIndex;
      if (idxsIndex == static_cast<size_t>(idxs.size())) { idxsIndex--;}
    }
  }
  m_StatsDataArray = replacement;
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsDataArray::copyTuple(size_t currentPos, size_t newPos)
{
  m_StatsDataArray[newPos] = m_StatsDataArray[currentPos];
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer StatsDataArray::reorderCopy(QVector<size_t> newOrderMap)
{
  if( static_cast<size_t>(newOrderMap.size()) != getNumberOfTuples())
  {
    return IDataArray::Pointer();
  }
  StatsDataArray::Pointer daCopyPtr = StatsDataArray::New();
  daCopyPtr->resize(getNumberOfTuples());
  daCopyPtr->initializeWithZeros();
  StatsDataArray& daCopy = *daCopyPtr;
  for(size_t i = 0; i < getNumberOfTuples(); i++)
  {
    daCopy[newOrderMap[i]] = m_StatsDataArray[i];
  }

  return daCopyPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::initializeTuple(size_t i, double p)
{
  BOOST_ASSERT(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::initializeWithZeros()
{

  for(qint32 i = 0; i < m_StatsDataArray.size(); ++i)
  {
    m_StatsDataArray[i]->initialize();
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer StatsDataArray::deepCopy()
{
  StatsDataArray::Pointer daCopyPtr = StatsDataArray::New();
  daCopyPtr->resize(getNumberOfTuples());
  StatsDataArray& daCopy = *daCopyPtr;
  for(size_t i = 0; i < getNumberOfTuples(); i++)
  {
    daCopy[i] = m_StatsDataArray[i];
  }

  return daCopyPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t StatsDataArray::resizeTotalElements(size_t size)
{
  m_StatsDataArray.resize(size);
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t StatsDataArray::resize(size_t numTuples)
{
  return resizeTotalElements(numTuples);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::printTuple(QTextStream& out, size_t i, char delimiter)
{
  BOOST_ASSERT(false);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::printComponent(QTextStream& out, size_t i, int j)
{
  BOOST_ASSERT(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsDataArray::writeH5Data(hid_t parentId, QVector<size_t> tDims)
{
  herr_t err = 0;
  hid_t gid = QH5Utilities::createGroup(parentId, DREAM3D::StringConstants::Statistics);
  if (gid < 0)
  {
    return -1;
  }
  HDF5ScopedGroupSentinel scopedFileSentinel(&gid, false); // This makes sure our H5Group is closed up as we exit this function

  // We start numbering our phases at 1. Anything in slot 0 is considered "Dummy" or invalid
  for(qint32 i = 1; i < m_StatsDataArray.size(); ++i)
  {
    if (m_StatsDataArray[i].get() != NULL)
    {
      QString indexString = QString::number(i);
      hid_t tupleId = QH5Utilities::createGroup(gid, indexString);
      err = QH5Lite::writeStringAttribute(gid, indexString, DREAM3D::StringConstants::StatsType, m_StatsDataArray[i]->getStatsType() );
      err = QH5Lite::writeScalarAttribute(gid, indexString, DREAM3D::StringConstants::PhaseType, m_StatsDataArray[i]->getPhaseType() );
      err = m_StatsDataArray[i]->writeHDF5Data(tupleId);
      err = QH5Utilities::closeHDF5Object(tupleId);
    }
  }

  QVector<size_t> cDims(1, 1);
  err = H5DataArrayWriter::writeDataArrayAttributes<StatsDataArray>(parentId, this, tDims, cDims);

  return err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsDataArray::readH5Data(hid_t parentId)
{
  bool ok = false;
  int err = 0;
  QString statsType;
  hid_t gid = QH5Utilities::openHDF5Object(parentId, DREAM3D::StringConstants::Statistics);
  if(gid < 0)
  {
    return err;
  }

  QList<QString> names;
  err = QH5Utilities::getGroupObjects(gid, H5Utilities::H5Support_GROUP, names);
  if(err < 0)
  {
    err |= QH5Utilities::closeHDF5Object(gid);
    return err;
  }

  for (QList<QString>::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    int index = 0;
    statsType = "";

    index = QString(*iter).toInt(&ok, 10);
    QH5Lite::readStringAttribute(gid, *iter, DREAM3D::StringConstants::StatsType, statsType);
    hid_t statId = QH5Utilities::openHDF5Object(gid, *iter);
    if(statId < 0)
    {
      continue;
      err |= -1;
    }
    if(statsType.compare(DREAM3D::StringConstants::PrimaryStatsData) == 0)
    {
      PrimaryStatsData::Pointer data = PrimaryStatsData::New();
      data->readHDF5Data(statId);
      setStatsData(index, data);
    }
    else if(statsType.compare(DREAM3D::StringConstants::PrecipitateStatsData) == 0)
    {
      PrecipitateStatsData::Pointer data = PrecipitateStatsData::New();
      data->readHDF5Data(statId);
      setStatsData(index, data);
    }
    else if(statsType.compare(DREAM3D::StringConstants::TransformationStatsData) == 0)
    {
      TransformationStatsData::Pointer data = TransformationStatsData::New();
      data->readHDF5Data(statId);
      setStatsData(index, data);
    }
    else if(statsType.compare(DREAM3D::StringConstants::MatrixStatsData) == 0)
    {
      MatrixStatsData::Pointer data = MatrixStatsData::New();
      data->readHDF5Data(statId);
      setStatsData(index, data);
    }
    else if(statsType.compare(DREAM3D::StringConstants::BoundaryStatsData) == 0)
    {
      BoundaryStatsData::Pointer data = BoundaryStatsData::New();
      data->readHDF5Data(statId);
      setStatsData(index, data);
    }
    else
    {
      qDebug() << "The Type of the stats data could not be read." ;
      return -1100;
    }
    err |= QH5Utilities::closeHDF5Object(statId);
  }

  // Do not forget to close the object
  err |= QH5Utilities::closeHDF5Object(gid);

  return err;
}
