/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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

#include "AIMH5DataWriter.h"
#include "AIM/Common/ReconstructionFunc.h"





#define APPEND_DATA_TRUE 1
#define APPEND_DATA_FALSE 0

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMH5DataWriter::AIMH5DataWriter() :
m_FileId(-1),
m_CurrentGroupId(-1)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMH5DataWriter::~AIMH5DataWriter()
{
  if (m_CurrentGroupId != -1)
  {
    closeCurrentGroup();
  }
  closeFile();
}

int AIMH5DataWriter::createGroup(const std::string &hdfGroupPath, const char* vtkDataObjectType)
{
  // std::cout << "   vtkH5DataWriter::WritePoints()" << std::endl;
  herr_t err = H5Utilities::createGroupsFromPath(hdfGroupPath, m_FileId);
  if (err < 0)
  {
    std::cout << "Error creating HDF Group " << hdfGroupPath << std::endl;
  }
  err = H5Lite::writeStringAttribute(m_FileId, hdfGroupPath, H5_VTK_DATA_OBJECT, vtkDataObjectType );
  if(err < 0)
  {
    std::cout << "Error writing string attribute to HDF Group " << hdfGroupPath << std::endl;
  }
  return err;
}

int AIMH5DataWriter::openGroup(const std::string &hdfGroupPath)
{
  m_CurrentGroupId = H5Gopen(m_FileId, hdfGroupPath.c_str() );
  m_CurrentGroupPath = hdfGroupPath;

  return m_CurrentGroupId;
}

int AIMH5DataWriter::closeCurrentGroup()
{
  // Close the PolyData group when we are finished with it
  int err = H5Gclose(m_CurrentGroupId);
  if (err < 0)
  {
     std::cout << "Error closing group: "  << m_CurrentGroupPath << std::endl;
  }
  m_CurrentGroupPath = "";
  m_CurrentGroupId = -1;
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AIMH5DataWriter::openFile(bool appendData)
{
  // Try to open a file to append data into
    if (APPEND_DATA_TRUE == appendData)
    {
      m_FileId = H5Utilities::openFile(m_FileName, false);
    }
    // No file was found or we are writing new data only to a clean file
    if (APPEND_DATA_FALSE == appendData || m_FileId < 0)
    {
      m_FileId = H5Utilities::createFile (m_FileName);
    }

    //Something went wrong either opening or creating the file. Error messages have
    // Alread been written at this point so just return.
    if (m_FileId < 0)
    {
       std::cout << logTime() << "The hdf5 file could not be opened or created.\n The Given filename was:\n\t[" << m_FileName<< "]" << std::endl;
    }
    return m_FileId;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AIMH5DataWriter::closeFile()
{
  // Close the file when we are finished with it
  return H5Utilities::closeFile(m_FileId);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AIMH5DataWriter::writeUnstructuredGrid(const std::vector<float> &points, const std::vector<int32_t> &cells,
                                           const std::vector<int32_t> &cell_types)
{
  // Write the Points
  int err = writePoints(points);
  if (err < 0)
  {
    std::cout << "Error writing Points for Unstructured Grid into " << m_CurrentGroupPath << std::endl;
    return err;
  }

  // Write the CELL array
  int32_t* tempArray = const_cast<int32_t*>(&(cells.front()));
  int32_t rank =1;
  uint64_t dims[1] = {cells.size()};
  err = H5Lite::writePointerDataset(m_CurrentGroupId, H5_CELLS, rank, dims, tempArray);
  if (err < 0)
  {
     std::cout << "Error Writing CELL array for " << m_CurrentGroupPath << std::endl;
  }
  int32_t size = cell_types.size();
  err = H5Lite::writeScalarAttribute(m_CurrentGroupId, H5_CELLS, "Number Of Cells", size);

  // Write the CELL_TYPE array
  int32_t* cTypePtr = const_cast<int32_t*>(&(cell_types.front()));
  dims[0] = cell_types.size();
  err = H5Lite::writePointerDataset(m_CurrentGroupId, H5_CELL_TYPES, rank, dims, cTypePtr);
  if (err < 0)
  {
     std::cout << "Error Writing CELL_TYPES for " << m_CurrentGroupPath << std::endl;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AIMH5DataWriter::writePolyCells( const std::vector<int> &cells, const char *label, int polygonEntrySize)
{
  // std::cout << "   vtkH5DataWriter::WriteCells()" << std::endl;
  if ( cells.size() == 0 )
    {
    return 1;
    }

  int ncells=cells.size() / polygonEntrySize;
  int size=cells.size();

  if ( ncells < 1 )
    {
    return 1;
    }
  int* tempArray = const_cast<int*>(&(cells.front()));
  int32_t rank =1;
  uint64_t dims[1] = {size};
  herr_t err = H5Lite::writePointerDataset(m_CurrentGroupId, label, rank, dims, tempArray);
  if (err < 0)
  {
    // std::cout << "Error Writing Vertices." << std::endl;
  }
  err = H5Lite::writeScalarAttribute(m_CurrentGroupId, label, "Number Of Cells", ncells);

  return err;
}

