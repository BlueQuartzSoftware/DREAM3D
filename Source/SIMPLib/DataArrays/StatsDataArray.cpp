/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "StatsDataArray.h"

#include <QtCore/QList>

#include "SIMPLib/StatsData/PrimaryStatsData.h"
#include "SIMPLib/StatsData/PrecipitateStatsData.h"
#include "SIMPLib/StatsData/TransformationStatsData.h"
#include "SIMPLib/StatsData/BoundaryStatsData.h"
#include "SIMPLib/StatsData/MatrixStatsData.h"


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
StatsDataArray::Pointer StatsDataArray::CreateArray(size_t numElements, const QString& name, bool allocate)
{
  if (name.isEmpty() == true)
  {
    return NullPointer();
  }
  StatsDataArray::Pointer ptr = StatsDataArray::New();
  ptr->setName(name);
  std::vector<unsigned int> phase_types(numElements, DREAM3D::PhaseType::UnknownPhaseType);
  if(allocate) { ptr->fillArrayWithNewStatsData(numElements, &(phase_types.front()) ); }
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsDataArray::Pointer StatsDataArray::CreateArray(size_t numTuples, int rank, size_t* dims, const QString& name, bool allocate)
{
  if (name.isEmpty() == true)
  {
    return NullPointer();
  }
  StatsDataArray::Pointer ptr = StatsDataArray::New();
  ptr->setName(name);
  std::vector<unsigned int> phase_types(numTuples, DREAM3D::PhaseType::UnknownPhaseType);
  if(allocate) { ptr->fillArrayWithNewStatsData(numTuples, &(phase_types.front()) ); }
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsDataArray::Pointer StatsDataArray::CreateArray(size_t numTuples, std::vector<size_t> cDims, const QString& name, bool allocate)
{
  if (name.isEmpty() == true)
  {
    return NullPointer();
  }
  StatsDataArray::Pointer ptr = StatsDataArray::New();
  ptr->setName(name);
  std::vector<unsigned int> phase_types(numTuples, DREAM3D::PhaseType::UnknownPhaseType);
  if(allocate) { ptr->fillArrayWithNewStatsData(numTuples, &(phase_types.front()) ); }
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsDataArray::Pointer StatsDataArray::CreateArray(size_t numTuples, QVector<size_t> cDims, const QString& name, bool allocate)
{
  if (name.isEmpty() == true)
  {
    return NullPointer();
  }
  StatsDataArray::Pointer ptr = StatsDataArray::New();
  ptr->setName(name);
  std::vector<unsigned int> phase_types(numTuples, DREAM3D::PhaseType::UnknownPhaseType);
  if(allocate) { ptr->fillArrayWithNewStatsData(numTuples, &(phase_types.front()) ); }
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsDataArray::Pointer StatsDataArray::CreateArray(QVector<size_t> tDims, QVector<size_t> cDims, const QString& name, bool allocate)
{
  if (name.isEmpty() == true)
  {
    return NullPointer();
  }
  size_t numTuples = tDims[0];
  qint32 size = tDims.size();
  for(qint32 iter = 1; iter < size; iter++)
  {
    numTuples *= tDims[iter];
  }
  StatsDataArray::Pointer ptr = StatsDataArray::New();
  ptr->setName(name);
  std::vector<unsigned int> phase_types(numTuples, DREAM3D::PhaseType::UnknownPhaseType);
  if(allocate) { ptr->fillArrayWithNewStatsData(numTuples, &(phase_types.front()) ); }
  return ptr;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer StatsDataArray::createNewArray(size_t numElements, int rank, size_t* dims, const QString& name, bool allocate)
{
  return StatsDataArray::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer StatsDataArray::createNewArray(size_t numElements, std::vector<size_t> dims, const QString& name, bool allocate)
{
  return StatsDataArray::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer StatsDataArray::createNewArray(size_t numElements, QVector<size_t> dims, const QString& name, bool allocate)
{
  return StatsDataArray::NullPointer();
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
    if (idxs[i] >= static_cast<size_t>(m_StatsDataArray.size()))
    {
      return -100;
    }
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
      if (idxsIndex == static_cast<size_t>(idxs.size()))
      {
        idxsIndex--;
      }
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
bool StatsDataArray::copyData(size_t destTupleOffset, IDataArray::Pointer sourceArray)
{
  if(!m_IsAllocated) { return false; }
  if(0 == m_StatsDataArray.size()) { return false; }
  if(destTupleOffset >= m_StatsDataArray.size()) { return false; }
  if(!sourceArray->isAllocated()) { return false; }
  Self* source = dynamic_cast<Self*>(sourceArray.get());

  if(sourceArray->getNumberOfComponents() != getNumberOfComponents()) { return false; }

  if( sourceArray->getNumberOfTuples()*sourceArray->getNumberOfComponents() + destTupleOffset*getNumberOfComponents() > m_StatsDataArray.size() ) { return false; }

  size_t sourceNTuples = source->getNumberOfTuples();

  for(size_t i = 0; i < sourceNTuples; i++)
  {
    m_StatsDataArray[destTupleOffset + i] = (*source)[i];
  }

  return true;
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
IDataArray::Pointer StatsDataArray::deepCopy(bool forceNoAllocate)
{
  StatsDataArray::Pointer daCopyPtr = StatsDataArray::New();
  if(forceNoAllocate == false)
  {
    daCopyPtr->resize(getNumberOfTuples());
    StatsDataArray& daCopy = *daCopyPtr;
    for(size_t i = 0; i < getNumberOfTuples(); i++)
    {
      daCopy[i] = m_StatsDataArray[i];
    }
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
  hid_t gid = QH5Utilities::createGroup(parentId, m_Name);
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
  hid_t gid = QH5Utilities::openHDF5Object(parentId, m_Name);
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsDataArray::writeXdmfAttribute(QTextStream& out, int64_t* volDims, const QString& hdfFileName,
                                       const QString& groupPath, const QString& labelb)
{
  out << "<!-- Xdmf is not supported for " << getNameOfClass() << " with type " << getTypeAsString() << " --> ";
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StatsDataArray::getInfoString(DREAM3D::InfoStringFormat format)
{
  QString info;
  QTextStream ss (&info);
  if(format == DREAM3D::HtmlFormat)
  {
    ss << "<html><head></head>\n";
    ss << "<body>\n";
    ss << "<table cellpadding=\"4\" cellspacing=\"0\" border=\"0\">\n";
    ss << "<tbody>\n";
    ss << "<tr bgcolor=\"#D3D8E0\"><th colspan=2>Attribute Array Info</th></tr>";

    ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Name:</th><td>" << getName() << "</td></tr>";


    ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Type:</th><td>" << getTypeAsString() << "</td></tr>";
    ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Number of Tuples:</th><td>" << getNumberOfTuples() << "</td></tr>";

//    QString compDimStr = "(";
//    for(int i = 0; i < m_CompDims.size(); i++)
//    {
//      compDimStr = compDimStr + QString::number(m_CompDims[i]);
//      if(i < m_CompDims.size() - 1) {
//         compDimStr = compDimStr + QString(", ");
//      }
//    }
//    compDimStr = compDimStr + ")";
//    ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Component Dimensions:</th><td>" << compDimStr << "</td></tr>";
//    ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Total Elements:</th><td>" << m_Size << "</td></tr>";

    ss << "</tbody></table>\n";
    ss << "<br/>";
    ss << "</body></html>";
  }
  else
  {

  }
  return info;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StatsDataArray::getTypeAsString()
{ return "StatsDataArray"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::getXdmfTypeAndSize(QString& xdmfTypeName, int& precision)
{
  xdmfTypeName = getNameOfClass();
  precision = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool StatsDataArray::isAllocated()
{
  return m_IsAllocated;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::clearAll()
{
  m_StatsDataArray.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::setStatsData(int index, StatsData::Pointer statsData)
{
  if(index >= static_cast<int>(m_StatsDataArray.size()))
  {
    size_t old = m_StatsDataArray.size();
    m_StatsDataArray.resize(index + 1);
    // Initialize with zero length Vectors
    for (int i = old; i < m_StatsDataArray.size(); ++i)
    {
      m_StatsDataArray[i] = StatsData::New();
    }
  }
  m_StatsDataArray[index] = statsData;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsDataArray::fillArrayWithNewStatsData(size_t n, unsigned int* phase_types)
{
  m_StatsDataArray.resize(n);
  for (size_t i = 0; i < n; ++i)
  {
    if (phase_types != NULL)
    {
      if(phase_types[i] == DREAM3D::PhaseType::PrimaryPhase) { m_StatsDataArray[i] = PrimaryStatsData::New(); }
      else if(phase_types[i] == DREAM3D::PhaseType::PrecipitatePhase) { m_StatsDataArray[i] = PrecipitateStatsData::New(); }
      else if(phase_types[i] == DREAM3D::PhaseType::TransformationPhase) { m_StatsDataArray[i] = TransformationStatsData::New(); }
      else if(phase_types[i] == DREAM3D::PhaseType::BoundaryPhase) { m_StatsDataArray[i] = BoundaryStatsData::New(); }
      else if(phase_types[i] == DREAM3D::PhaseType::MatrixPhase) { m_StatsDataArray[i] = MatrixStatsData::New(); }
      else { m_StatsDataArray[i] = StatsData::New(); }
    }
    if(phase_types == NULL) { m_StatsDataArray[i] = StatsData::New(); }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsData::Pointer StatsDataArray::getStatsData(int idx)
{
#ifndef NDEBUG
  if(m_StatsDataArray.size() > 0)
  {
    BOOST_ASSERT(idx < static_cast<int>(m_StatsDataArray.size()));
  }
#endif
  return m_StatsDataArray[idx];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsData::Pointer StatsDataArray::operator[](int idx)
{
#ifndef NDEBUG
  if(m_StatsDataArray.size() > 0)
  {
    BOOST_ASSERT(idx < static_cast<int>(m_StatsDataArray.size()));
  }
#endif
  return m_StatsDataArray[idx];
}

