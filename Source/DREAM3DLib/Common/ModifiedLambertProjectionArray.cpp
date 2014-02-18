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
#include "ModifiedLambertProjectionArray.h"

#include <list>

#include "MXA/Utilities/StringUtils.h"


#include "H5Support/H5Utilities.h"

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
void ModifiedLambertProjectionArray::SetName(const std::string& name)
{
  m_Name = name;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string ModifiedLambertProjectionArray::GetName()
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
void* ModifiedLambertProjectionArray::GetVoidPointer(size_t i)
{
#ifndef NDEBUG
  if(m_ModifiedLambertProjectionArray.size() > 0)
  {
    BOOST_ASSERT(i < m_ModifiedLambertProjectionArray.size());
  }
#endif
  if(i >= this->GetNumberOfTuples())
  {
    return 0x0;
  }
  return (void*)(&(m_ModifiedLambertProjectionArray[i]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t ModifiedLambertProjectionArray::GetNumberOfTuples()
{
  return m_ModifiedLambertProjectionArray.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t ModifiedLambertProjectionArray::GetSize()
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
    BOOST_ASSERT(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ModifiedLambertProjectionArray::GetNumberOfComponents()
{
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t ModifiedLambertProjectionArray::GetTypeSize()
{
  return sizeof(ModifiedLambertProjection);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ModifiedLambertProjectionArray::EraseTuples(std::vector<size_t>& idxs)
{
  int err = 0;

  // If nothing is to be erased just return
  if(idxs.size() == 0)
  {
    return 0;
  }

  if (idxs.size() >= GetNumberOfTuples() )
  {
    Resize(0);
    return 0;
  }

  // Sanity Check the Indices in the vector to make sure we are not trying to remove any indices that are
  // off the end of the array and return an error code.
  for(std::vector<size_t>::size_type i = 0; i < idxs.size(); ++i)
  {
    if (idxs[i] >= m_ModifiedLambertProjectionArray.size()) { return -100; }
  }


  std::vector<ModifiedLambertProjection::Pointer> replacement(m_ModifiedLambertProjectionArray.size() - idxs.size());
  size_t idxsIndex = 0;
  size_t rIdx = 0;
  for(size_t dIdx = 0; dIdx < m_ModifiedLambertProjectionArray.size(); ++dIdx)
  {
    if (dIdx != idxs[idxsIndex])
    {
      replacement[rIdx] = m_ModifiedLambertProjectionArray[dIdx];
      ++rIdx;
    }
    else
    {
      ++idxsIndex;
      if (idxsIndex == idxs.size() ) { idxsIndex--;}
    }
  }
  m_ModifiedLambertProjectionArray = replacement;
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ModifiedLambertProjectionArray::CopyTuple(size_t currentPos, size_t newPos)
{
  m_ModifiedLambertProjectionArray[newPos] = m_ModifiedLambertProjectionArray[currentPos];
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ModifiedLambertProjectionArray::InitializeTuple(size_t i, double p)
{
  BOOST_ASSERT(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ModifiedLambertProjectionArray::initializeWithZeros()
{

  for(size_t i = 0; i < m_ModifiedLambertProjectionArray.size(); ++i)
  {
    m_ModifiedLambertProjectionArray[i]->initializeSquares(1, 1);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer ModifiedLambertProjectionArray::deepCopy()
{
  ModifiedLambertProjectionArray::Pointer daCopyPtr = ModifiedLambertProjectionArray::New();
  daCopyPtr->Resize(GetNumberOfTuples());
  ModifiedLambertProjectionArray& daCopy = *daCopyPtr;
  for(size_t i=0;i<GetNumberOfTuples();i++)
  {
    daCopy[i] = m_ModifiedLambertProjectionArray[i];
  }

  return daCopyPtr;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t ModifiedLambertProjectionArray::RawResize(size_t size)
{
  m_ModifiedLambertProjectionArray.resize(size);
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t ModifiedLambertProjectionArray::Resize(size_t numTuples)
{
  return RawResize(numTuples);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ModifiedLambertProjectionArray::printTuple(std::ostream& out, size_t i, char delimiter)
{
  BOOST_ASSERT(false);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ModifiedLambertProjectionArray::printComponent(std::ostream& out, size_t i, int j)
{
  BOOST_ASSERT(false);
}

// -------------------------------------------------------------------------- */
void AppendRowToH5Dataset(hid_t gid, const std::string& dsetName, int lambertSize, double* north, double* south)
{
  hid_t dataspace = -1, dataset = -1;
  hid_t filespace = -1;
  hsize_t currentDims[2] = { 1, 0 };
  hsize_t newDims[2];
  hsize_t offset[2];
  hsize_t hyperDims[2] = { 1, 0 };
  int rank;
  herr_t status;
  /*  printf("CPU [%d,%d] Expanding '%s' array with additional Row \n", home->myDomain, home->cycle, dsetName);
      fflush(stdout); */
  dataset = H5Dopen2(gid, dsetName.c_str(), H5P_DEFAULT);
  filespace = H5Dget_space(dataset); /* Get filespace handle first. */
  rank = H5Sget_simple_extent_ndims(filespace);
  status = H5Sget_simple_extent_dims(filespace, currentDims, NULL);
  /*  printf("dataset '%s' rank %d, dims %lu x %lu \n", dsetName, rank, (unsigned long)(dims[0]), (unsigned long)(dims[1]));*/

  /* Try extending the dataset*/
  newDims[0] = currentDims[0] + 1;
  newDims[1] = currentDims[1]; // Number of columns

//  printf("dataset '%s' rank %d, size %lu x %lu \n", dsetName.c_str(), rank, (unsigned long)(newDims[0]), (unsigned long)(newDims[1]));
  status = H5Dset_extent(dataset, newDims);
  if (status < 0)
  {
    std::cout << "Error Extending Data set" << std::endl;
    BOOST_ASSERT(false);
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
    std::cout << "Error appending north square" << std::endl;
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
    std::cout << "Error Writing Chunked Data set to file" << std::endl;
  }


  H5Dclose(dataset);
  H5Sclose(dataspace);
  H5Sclose(filespace);
}
/* -----------------------------------------------------------------------------
//
// -------------------------------------------------------------------------- */
void Create2DExpandableDataset(hid_t gid, const std::string& dsetName, int lambertSize, hsize_t chunk_dim,
                               double* north, double* south)
{

  hid_t dataspace = -1;
  hid_t filespace = -1;
  hid_t dataset = -1;
  hid_t cparms = -1;
  herr_t status = -1;
  hsize_t maxdims[2] = { H5S_UNLIMITED, H5S_UNLIMITED }; // Allow for 2D Arrays
  hsize_t chunk_dims[2] =  { 1, chunk_dim };
  hsize_t dims[2] = { 1, lambertSize };
  hsize_t size[2];
  hsize_t offset[2];
  hsize_t hyperDims[2] =  { 1, lambertSize };
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
  dataset = H5Dcreate2(gid, dsetName.c_str(), H5T_NATIVE_DOUBLE, dataspace, H5P_DEFAULT, cparms, H5P_DEFAULT);
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
    std::cout << "Error Writing Chunked Data set to file" << std::endl;
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
    std::cout << "Error Writing Chunked Data set to file" << std::endl;
  }

  H5Dclose(dataset);
  H5Sclose(dataspace);
  H5Sclose(filespace);
  H5Pclose(cparms);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ModifiedLambertProjectionArray::writeH5Data(hid_t parentId)
{
  herr_t err = 0;
  if (m_ModifiedLambertProjectionArray.size() == 0)
  {
    return -2;
  }
  hid_t gid = H5Utilities::createGroup(parentId, DREAM3D::HDF5::GBCD);
  if (gid < 0)
  {
    return -1;
  }

  std::string dsetName = StringUtils::numToString(m_Phase);
  ModifiedLambertProjection::Pointer tmp = m_ModifiedLambertProjectionArray[0];
  tmp->getDimension();
  int lambertDimension = tmp->getDimension();
  hsize_t lambertElements =  tmp->getDimension() * tmp->getDimension();
  float sphereRadius = tmp->getSphereRadius();

  Create2DExpandableDataset(gid, dsetName, lambertElements, lambertElements * 2, tmp->getNorthSquare()->GetPointer(0), tmp->getSouthSquare()->GetPointer(0) );


  DoubleArrayType* north = NULL;
  DoubleArrayType* south = NULL;


  // We start numbering our phases at 1. Anything in slot 0 is considered "Dummy" or invalid
  for(size_t i = 1; i < m_ModifiedLambertProjectionArray.size(); ++i)
  {
    if (m_ModifiedLambertProjectionArray[i].get() != NULL)
    {
      north = m_ModifiedLambertProjectionArray[i]->getNorthSquare().get();
      south = m_ModifiedLambertProjectionArray[i]->getSouthSquare().get();
      AppendRowToH5Dataset(gid, dsetName, lambertElements, north->GetPointer(0), south->GetPointer(0));
    }
  }

  err = H5Lite::writeScalarAttribute(gid, dsetName, "Lambert Dimension", lambertDimension);
  err = H5Lite::writeScalarAttribute(gid, dsetName, "Lambert Sphere Radius", sphereRadius);
  err = H5Utilities::closeHDF5Object(gid);
  return err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ModifiedLambertProjectionArray::readH5Data(hid_t parentId)
{
  int err = 0;
  std::string statsType;
  hid_t gid = H5Utilities::openHDF5Object(parentId, DREAM3D::HDF5::Statistics);
  if(gid < 0)
  {
    return err;
  }

  std::list<std::string> names;
  err = H5Utilities::getGroupObjects(gid, H5Utilities::H5Support_GROUP, names);
  if(err < 0)
  {
    err |= H5Utilities::closeHDF5Object(gid);
    return err;
  }

  for (std::list<std::string>::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    int index = 0;
    statsType = "";
    StringUtils::stringToNum(index, *iter);
    H5Lite::readStringAttribute(gid, *iter, DREAM3D::HDF5::StatsType, statsType);
    hid_t statId = H5Utilities::openHDF5Object(gid, *iter);
    if(statId < 0)
    {
      continue;
      err |= -1;
    }
    err |= H5Utilities::closeHDF5Object(statId);
  }

  // Do not forget to close the object
  err |= H5Utilities::closeHDF5Object(gid);

  return err;
}
