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
#include "ModifiedLambertProjectionArray.h"

#include <QtCore/QList>


#include "H5Support/QH5Utilities.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ModifiedLambertProjectionArray::ModifiedLambertProjectionArray() :
  m_Name("")
{
  m_IsAllocated = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ModifiedLambertProjectionArray::~ModifiedLambertProjectionArray()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ModifiedLambertProjectionArray::getXdmfTypeAndSize(QString& xdmfTypeName, int& precision)
{
  xdmfTypeName = getNameOfClass();
  precision = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ModifiedLambertProjectionArray::getTypeAsString() { return "ModifiedLambertProjectionArray"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer ModifiedLambertProjectionArray::createNewArray(size_t numElements, int rank, size_t* dims, const QString& name, bool allocate)
{
  return ModifiedLambertProjectionArray::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer ModifiedLambertProjectionArray::createNewArray(size_t numElements, std::vector<size_t> dims, const QString& name, bool allocate)
{
  return ModifiedLambertProjectionArray::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer ModifiedLambertProjectionArray::createNewArray(size_t numElements, QVector<size_t> dims, const QString& name, bool allocate)
{
  return ModifiedLambertProjectionArray::NullPointer();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ModifiedLambertProjectionArray::isAllocated()
{
  return m_IsAllocated;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ModifiedLambertProjectionArray::clearAll()
{
  m_ModifiedLambertProjectionArray.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ModifiedLambertProjectionArray::setModifiedLambertProjection(int index, ModifiedLambertProjection::Pointer ModifiedLambertProjection)
{
  if(index >= static_cast<int>(m_ModifiedLambertProjectionArray.size()))
  {
    size_t old = m_ModifiedLambertProjectionArray.size();
    m_ModifiedLambertProjectionArray.resize(index + 1);
    // Initialize with zero length Vectors
    for (int i = old; i < m_ModifiedLambertProjectionArray.size(); ++i)
    {
      m_ModifiedLambertProjectionArray[i] = ModifiedLambertProjection::New();
    }
  }
  m_ModifiedLambertProjectionArray[index] = ModifiedLambertProjection;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ModifiedLambertProjectionArray::fillArrayWithNewModifiedLambertProjection(size_t n)
{
  m_ModifiedLambertProjectionArray.resize(n);
  for (size_t i = 0; i < n; ++i)
  {
    if (m_ModifiedLambertProjectionArray[i].get() == NULL)
    {
      m_ModifiedLambertProjectionArray[i] = ModifiedLambertProjection::New();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ModifiedLambertProjection::Pointer ModifiedLambertProjectionArray::getModifiedLambertProjection(int idx)
{
#ifndef NDEBUG
  if(m_ModifiedLambertProjectionArray.size() > 0)
  {
    Q_ASSERT(idx < static_cast<int>(m_ModifiedLambertProjectionArray.size()));
  }
#endif
  return m_ModifiedLambertProjectionArray[idx];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ModifiedLambertProjection::Pointer ModifiedLambertProjectionArray::operator[](int idx)
{
#ifndef NDEBUG
  if(m_ModifiedLambertProjectionArray.size() > 0)
  {
    Q_ASSERT(idx < static_cast<int>(m_ModifiedLambertProjectionArray.size()));
  }
#endif
  return m_ModifiedLambertProjectionArray[idx];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ModifiedLambertProjectionArray::setName(const QString& name)
{
  m_Name = name;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ModifiedLambertProjectionArray::getName()
{
  return m_Name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ModifiedLambertProjectionArray::takeOwnership()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ModifiedLambertProjectionArray::releaseOwnership()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void* ModifiedLambertProjectionArray::getVoidPointer(size_t i)
{
#ifndef NDEBUG
  if(m_ModifiedLambertProjectionArray.size() > 0)
  {
    Q_ASSERT(i < static_cast<size_t>(m_ModifiedLambertProjectionArray.size()));
  }
#endif
  if(i >= this->getNumberOfTuples())
  {
    return 0x0;
  }
  return (void*)(&(m_ModifiedLambertProjectionArray[i]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t ModifiedLambertProjectionArray::getNumberOfTuples()
{
  return m_ModifiedLambertProjectionArray.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t ModifiedLambertProjectionArray::getSize()
{
  return m_ModifiedLambertProjectionArray.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ModifiedLambertProjectionArray::SetNumberOfComponents(int nc)
{
  if (nc != 1)
  {
    Q_ASSERT(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ModifiedLambertProjectionArray::getNumberOfComponents()
{
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<size_t> ModifiedLambertProjectionArray::getComponentDimensions()
{
  QVector<size_t> dims(1, 1);
  return dims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ModifiedLambertProjectionArray::SetRank(int rnk)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ModifiedLambertProjectionArray::getRank()
{
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t ModifiedLambertProjectionArray::getTypeSize()
{
  return sizeof(ModifiedLambertProjection);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ModifiedLambertProjectionArray::eraseTuples(QVector<size_t>& idxs)
{
  int err = 0;

  // If nothing is to be erased just return
  if(idxs.size() == 0)
  {
    return 0;
  }

  if (static_cast<size_t>(idxs.size()) >= getNumberOfTuples() )
  {
    resize(0);
    return 0;
  }

  // Sanity Check the Indices in the vector to make sure we are not trying to remove any indices that are
  // off the end of the array and return an error code.
  for(QVector<size_t>::size_type i = 0; i < idxs.size(); ++i)
  {
    if (idxs[i] >= static_cast<size_t>(m_ModifiedLambertProjectionArray.size()))
    {
      return -100;
    }
  }


  QVector<ModifiedLambertProjection::Pointer> replacement(m_ModifiedLambertProjectionArray.size() - idxs.size());
  qint32 idxsIndex = 0;
  size_t rIdx = 0;
  size_t count = static_cast<size_t>(m_ModifiedLambertProjectionArray.size());
  for(size_t dIdx = 0; dIdx < count; ++dIdx)
  {
    if (dIdx != idxs[idxsIndex])
    {
      replacement[rIdx] = m_ModifiedLambertProjectionArray[dIdx];
      ++rIdx;
    }
    else
    {
      ++idxsIndex;
      if (idxsIndex == idxs.size() )
      {
        idxsIndex--;
      }
    }
  }
  m_ModifiedLambertProjectionArray = replacement;
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ModifiedLambertProjectionArray::copyTuple(size_t currentPos, size_t newPos)
{
  m_ModifiedLambertProjectionArray[newPos] = m_ModifiedLambertProjectionArray[currentPos];
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ModifiedLambertProjectionArray::copyData(size_t destTupleOffset, IDataArray::Pointer sourceArray)
{
  if(!m_IsAllocated) { return false; }
  if(0 == m_ModifiedLambertProjectionArray.size()) { return false; }
  if(destTupleOffset >= m_ModifiedLambertProjectionArray.size()) { return false; }
  if(!sourceArray->isAllocated()) { return false; }
  Self* source = dynamic_cast<Self*>(sourceArray.get());

  if(sourceArray->getNumberOfComponents() != getNumberOfComponents()) { return false; }

  if( sourceArray->getNumberOfTuples()*sourceArray->getNumberOfComponents() + destTupleOffset*getNumberOfComponents() > m_ModifiedLambertProjectionArray.size() ) { return false; }

  size_t sourceNTuples = source->getNumberOfTuples();

  for(size_t i = 0; i < sourceNTuples; i++)
  {
    m_ModifiedLambertProjectionArray[destTupleOffset + i] = (*source)[i];
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ModifiedLambertProjectionArray::initializeTuple(size_t i, double p)
{
  Q_ASSERT(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ModifiedLambertProjectionArray::initializeWithZeros()
{

  for(qint32 i = 0; i < m_ModifiedLambertProjectionArray.size(); ++i)
  {
    m_ModifiedLambertProjectionArray[i]->initializeSquares(1, 1);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer ModifiedLambertProjectionArray::deepCopy(bool forceNoAllocate)
{
  ModifiedLambertProjectionArray::Pointer daCopyPtr = ModifiedLambertProjectionArray::New();
  if(forceNoAllocate == false)
  {
    daCopyPtr->resize(getNumberOfTuples());
    ModifiedLambertProjectionArray& daCopy = *daCopyPtr;
    for(size_t i = 0; i < getNumberOfTuples(); i++)
    {
      daCopy[i] = m_ModifiedLambertProjectionArray[i];
    }
  }
  return daCopyPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t ModifiedLambertProjectionArray::resizeTotalElements(size_t size)
{
  m_ModifiedLambertProjectionArray.resize(size);
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t ModifiedLambertProjectionArray::resize(size_t numTuples)
{
  return resizeTotalElements(numTuples);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ModifiedLambertProjectionArray::printTuple(QTextStream& out, size_t i, char delimiter)
{
  Q_ASSERT(false);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ModifiedLambertProjectionArray::printComponent(QTextStream& out, size_t i, int j)
{
  Q_ASSERT(false);
}

// -------------------------------------------------------------------------- */
void AppendRowToH5Dataset(hid_t gid, const QString& dsetName, int lambertSize, double* north, double* south)
{
  hid_t dataspace = -1, dataset = -1;
  hid_t filespace = -1;
  hsize_t currentDims[2] = { 1, 0 };
  hsize_t newDims[2];
  hsize_t offset[2];
  hsize_t hyperDims[2] = { 1, 0 };
  int rank = 0;
  herr_t status = -1;
  /*  printf("CPU [%d,%d] Expanding '%s' array with additional Row \n", home->myDomain, home->cycle, dsetName);
      fflush(stdout); */
  dataset = H5Dopen2(gid, dsetName.toLatin1().data(), H5P_DEFAULT);
  filespace = H5Dget_space(dataset); /* Get filespace handle first. */
  rank = H5Sget_simple_extent_ndims(filespace);
  status = H5Sget_simple_extent_dims(filespace, currentDims, NULL);
  /*  printf("dataset '%s' rank %d, dims %lu x %lu \n", dsetName, rank, (unsigned long)(dims[0]), (unsigned long)(dims[1]));*/

  /* Try extending the dataset*/
  newDims[0] = currentDims[0] + 1;
  newDims[1] = currentDims[1]; // Number of columns

  //  printf("dataset '%s' rank %d, size %lu x %lu \n", dsetName.toLatin1().data(), rank, (unsigned long)(newDims[0]), (unsigned long)(newDims[1]));
  status = H5Dset_extent(dataset, newDims);
  if (status < 0)
  {
    qDebug() << "Error Extending Data set" ;
    Q_ASSERT(false);
  }
  /*// Select a hyperslab.*/
  filespace = H5Dget_space(dataset);
  offset[0] = currentDims[0];
  offset[1] = 0; // Start of Row
  /*  printf("dataset '%s' rank %d, offset %lu x %lu \n", dsetName, rank, (unsigned long)(offset[0]), (unsigned long)(offset[1]));*/
  hyperDims[0] = 1; /* We want 1 single row - so force the dimension correctly */
  hyperDims[1] = currentDims[1] / 2; /* We DO want how ever many columns are needed. */
  /*  printf("dataset '%s' rank %d, dims1 %lu x %lu \n", dsetName, rank, (unsigned long)(dims1[0]), (unsigned long)(dims1[1]));*/
  status = H5Sselect_hyperslab(filespace, H5S_SELECT_SET, offset, NULL, hyperDims, NULL);
  /* Define a Memory Space*/
  //  currentDims[0] = 1;
  //  currentDims[1] = currentDims[1] / 2;
  dataspace = H5Screate_simple(rank, hyperDims, NULL);
  /* Write the data to the hyperslab.*/
  /*  printf("dataset '%s' rank %d, dims %lu x %lu \n", dsetName, rank, (unsigned long)(dims[0]), (unsigned long)(dims[1]));*/
  status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, dataspace, filespace, H5P_DEFAULT, north);
  if (status < 0)
  {
    qDebug() << "Error appending north square" ;
  }


  filespace = H5Dget_space(dataset);
  //offset[0] = currentDims[0]; // Offset to current row being added
  offset[1] = lambertSize; // Offset 0 Column
  status = H5Sselect_hyperslab(filespace, H5S_SELECT_SET, offset, NULL, hyperDims, NULL);
  /*
   * Write the data to the hyperslab.
   */
  status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, dataspace, filespace, H5P_DEFAULT, south);
  if (status < 0)
  {
    qDebug() << "Error Writing Chunked Data set to file" ;
  }


  H5Dclose(dataset);
  H5Sclose(dataspace);
  H5Sclose(filespace);
}
/* -----------------------------------------------------------------------------
//
// -------------------------------------------------------------------------- */
void Create2DExpandableDataset(hid_t gid, const QString& dsetName, int lambertSize, hsize_t chunk_dim,
                               double* north, double* south)
{

  hid_t dataspace = -1;
  hid_t filespace = -1;
  hid_t dataset = -1;
  hid_t cparms = -1;
  herr_t status = -1;
  hsize_t maxdims[2] = { H5S_UNLIMITED, H5S_UNLIMITED }; // Allow for 2D Arrays
  hsize_t chunk_dims[2] =  { 1, chunk_dim };
  hsize_t dims[2] = { 1ULL, static_cast<hsize_t>(lambertSize) };
  hsize_t size[2];
  hsize_t offset[2];
  hsize_t hyperDims[2] =  { 1ULL, static_cast<hsize_t>(lambertSize) };
  double fillvalue = -1.0;
  int rank = 2;
  //  int i = 0;
  //  int strSize = 0;
  //  char buffer[32];

  if (lambertSize == 1)
  {
    rank = 1;
  }
  /*  printf("CPU [%d,%d] Writing '%s' initial value to array. \n", home->myDomain, home->cycle, dsetName);
      fflush(stdout);*/

  /* Create the data space with unlimited dimensions */
  dataspace = H5Screate_simple(rank, dims, maxdims);

  /* Modify dataset creation properties, i.e. enable chunking.*/
  cparms = H5Pcreate(H5P_DATASET_CREATE);
  status = H5Pset_chunk(cparms, rank, chunk_dims);
  status = H5Pset_fill_value(cparms, H5T_NATIVE_DOUBLE, &fillvalue);

  /* Create a new dataset within the file using cparms creation properties.*/
  //  dataset = H5Dcreate(gid, dsetName, H5T_NATIVE_DOUBLE, dataspace, cparms, H5P_DEFAULT, H5P_DEFAULT);
  dataset = H5Dcreate2(gid, dsetName.toLatin1().data(), H5T_NATIVE_DOUBLE, dataspace, H5P_DEFAULT, cparms, H5P_DEFAULT);
  /*  Extend the dataset. This call assures that dataset is at least 1 */
  size[0] = 1; // Single Row
  size[1] = chunk_dim; // N Columns - What ever the user asked for
  status = H5Dset_extent(dataset, size);
  /* Select a hyperslab. */
  filespace = H5Dget_space(dataset);
  offset[0] = 0; // Offset 0 row
  offset[1] = 0; // Offset 0 Column
  status = H5Sselect_hyperslab(filespace, H5S_SELECT_SET, offset, NULL, hyperDims, NULL);

  /*
   * Write the data to the hyperslab.
   */
  status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, dataspace, filespace, H5P_DEFAULT, north);
  if (status < 0)
  {
    qDebug() << "Error Writing Chunked Data set to file" ;
  }

  filespace = H5Dget_space(dataset);
  offset[0] = 0; // Offset 0 row
  offset[1] = lambertSize; // Offset 0 Column
  status = H5Sselect_hyperslab(filespace, H5S_SELECT_SET, offset, NULL, hyperDims, NULL);

  /*
   * Write the data to the hyperslab.
   */
  status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, dataspace, filespace, H5P_DEFAULT, south);
  if (status < 0)
  {
    qDebug() << "Error Writing Chunked Data set to file" ;
  }

  H5Dclose(dataset);
  H5Sclose(dataspace);
  H5Sclose(filespace);
  H5Pclose(cparms);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ModifiedLambertProjectionArray::writeH5Data(hid_t parentId, QVector<size_t> tDims)
{
  herr_t err = 0;
  if (m_ModifiedLambertProjectionArray.size() == 0)
  {
    return -2;
  }
  hid_t gid = QH5Utilities::createGroup(parentId, DREAM3D::StringConstants::GBCD);
  if (gid < 0)
  {
    return -1;
  }

  QString dsetName = QString::number(m_Phase);
  ModifiedLambertProjection::Pointer tmp = m_ModifiedLambertProjectionArray[0];
  tmp->getDimension();
  int lambertDimension = tmp->getDimension();
  hsize_t lambertElements =  tmp->getDimension() * tmp->getDimension();
  float sphereRadius = tmp->getSphereRadius();

  Create2DExpandableDataset(gid, dsetName, lambertElements, lambertElements * 2, tmp->getNorthSquare()->getPointer(0), tmp->getSouthSquare()->getPointer(0) );


  DoubleArrayType* north = NULL;
  DoubleArrayType* south = NULL;


  // We start numbering our phases at 1. Anything in slot 0 is considered "Dummy" or invalid
  for(qint32 i = 1; i < m_ModifiedLambertProjectionArray.size(); ++i)
  {
    if (m_ModifiedLambertProjectionArray[i].get() != NULL)
    {
      north = m_ModifiedLambertProjectionArray[i]->getNorthSquare().get();
      south = m_ModifiedLambertProjectionArray[i]->getSouthSquare().get();
      AppendRowToH5Dataset(gid, dsetName, lambertElements, north->getPointer(0), south->getPointer(0));
    }
  }

  err = QH5Lite::writeScalarAttribute(gid, dsetName, "Lambert Dimension", lambertDimension);
  err = QH5Lite::writeScalarAttribute(gid, dsetName, "Lambert Sphere Radius", sphereRadius);
  err = QH5Utilities::closeHDF5Object(gid);
  return err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ModifiedLambertProjectionArray::readH5Data(hid_t parentId)
{
  //bool ok = false;
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
    //int index = 0;
    statsType = "";
    //index = QString( *iter ).toInt(&ok, 10);
    QH5Lite::readStringAttribute(gid, *iter, DREAM3D::StringConstants::StatsType, statsType);
    hid_t statId = QH5Utilities::openHDF5Object(gid, *iter);
    if(statId < 0)
    {
      continue;
      err |= -1;
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
int ModifiedLambertProjectionArray::writeXdmfAttribute(QTextStream& out, int64_t* volDims, const QString& hdfFileName,
                                                       const QString& groupPath, const QString& labelb)
{
  out << "<!-- Xdmf is not supported for " << getNameOfClass() << " with type " << getTypeAsString() << " --> ";
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ModifiedLambertProjectionArray::getInfoString(DREAM3D::InfoStringFormat format)
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
    ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Attribute Array Count:</th><td>" << getNumberOfTuples() << "</td></tr>";

    //        QString compDimStr = "(";
    //        for(int i = 0; i < m_CompDims.size(); i++)
    //        {
    //          compDimStr = compDimStr + QString::number(m_CompDims[i]);
    //          if(i < m_CompDims.size() - 1) {
    //             compDimStr = compDimStr + QString(", ");
    //          }
    //        }
    //        compDimStr = compDimStr + ")";
    //        ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Component Dimensions:</th><td>" << compDimStr << "</td></tr>";
    //        ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Total Elements:</th><td>" << m_Size << "</td></tr>";

    ss << "</tbody></table>\n";
    ss << "<br/>";
    ss << "</body></html>";
  }
  else
  {

  }
  return info;
}
