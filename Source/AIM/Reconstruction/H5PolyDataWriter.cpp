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

#include "H5PolyDataWriter.h"
#include "AIM/Common/ReconstructionFunc.h"
#include "MXA/HDF5/H5Utilities.h"
#include "MXA/HDF5/H5Lite.h"
#include "VTKH5Constants.h"



#define APPEND_DATA_TRUE 1
#define APPEND_DATA_FALSE 0

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5PolyDataWriter::H5PolyDataWriter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5PolyDataWriter::~H5PolyDataWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5PolyDataWriter::writePolyData(const std::string &hdfFile,
                                    ReconstructionFunc* r,
                                    bool appendData)
{
  hid_t fileId = -1;
  // Try to open a file to append data into
  if (APPEND_DATA_TRUE == appendData)
  {
    fileId = H5Utilities::openFile(hdfFile, false);
  }
  // No file was found or we are writing new data only to a clean file
  if (APPEND_DATA_FALSE == appendData || fileId < 0)
  {
    fileId = H5Utilities::createFile (hdfFile);
  }

  //Something went wrong either opening or creating the file. Error messages have
  // Alread been written at this point so just return.
  if (fileId < 0)
  {
     std::cout << logTime() << "The hdf5 file could not be opened or created.\n The Given filename was:\n\t[" << hdfFile<< "]" << std::endl;
  }

  std::stringstream ss;
  std::string hdfPath;
  for (int i = 1; i < r->numgrains; i++)
  {
    ss.str("");
    ss << "/" << i;
    hdfPath = ss.str();
    std::cout << "HDFPath: " << hdfPath << std::endl;
    herr_t err = H5Utilities::createGroupsFromPath(hdfPath, fileId);
    if (err < 0)
    {
      std::cout << "Error creating HDF Group " << hdfPath << std::endl;
      H5Utilities::closeFile(fileId);
      return -1;
    }
    hid_t fp = H5Gopen(fileId, hdfPath.c_str() );
    err = H5Lite::writeStringAttribute(fileId, hdfPath, H5_VTK_DATA_OBJECT, H5_VTK_POLYDATA );
    if(err < 0)
    {
      std::cout << "Error writing string attribute to HDF Group " << hdfPath << std::endl;
      H5Utilities::closeFile(fileId);
      return -1;
    }
    //Write the Field Data
    //TODO: Look at vtkH5PolyDataWriter.cpp(137)
    //Write the Points

    //Write the Verts

    //Write the Cells



    // Close the PolyData group when we are finished with it
    err = H5Gclose(fp);
    if (err < 0)
    {
      // std::cout << "Error closing group: " << this->HDFPath << std::endl;
    }
  }
  // Close the file when we are finished with it
  H5Utilities::closeFile(fileId);

#if 0
  ofstream outFile;
  char extension[15] = ".vtk";
  char index[5];
  vector<int >* vlist;
  vector<int > plist(((r->xpoints + 1) * (r->ypoints + 1) * (r->zpoints + 1)), 0);
  int pcount = 0;
  int ocol, orow, oplane;
  int col, row, plane;
  int vid, pid;
  for (int i = 1; i < numgrains; i++)
  {
    char filename[15] = "Grain ";
    sprintf(index, "%5.5d", i);
    strcat(filename, index);
    strcat(filename, extension);
    outFile.open(filename);
    outFile << "# vtk DataFile Version 2.0" << endl;
    outFile << "data set from FFT2dx_GB" << endl;
    outFile << "ASCII" << endl;
    outFile << "DATASET UNSTRUCTURED_GRID" << endl;
    vlist = grains[i].voxellist;
    pcount = 0;
    for (int j = 0; j < vlist->size(); j++)
    {
      vid = vlist->at(j);
      ocol = vid % xpoints;
      orow = (vid / xpoints) % ypoints;
      oplane = vid / (xpoints * ypoints);
      for (int k = 0; k < 8; k++)
      {
        if (k == 0) col = ocol, row = orow, plane = oplane;
        if (k == 1) col = ocol + 1, row = orow, plane = oplane;
        if (k == 2) col = ocol, row = orow + 1, plane = oplane;
        if (k == 3) col = ocol + 1, row = orow + 1, plane = oplane;
        if (k == 4) col = ocol, row = orow, plane = oplane + 1;
        if (k == 5) col = ocol + 1, row = orow, plane = oplane + 1;
        if (k == 6) col = ocol, row = orow + 1, plane = oplane + 1;
        if (k == 7) col = ocol + 1, row = orow + 1, plane = oplane + 1;
        pid = (plane * (xpoints + 1) * (ypoints + 1)) + (row * (xpoints + 1)) + col;
        if (plist[pid] == 0)
        {
          plist[pid] = pcount;
          pcount++;
        }
      }
    }
    outFile << "POINTS " << pcount << " float" << endl;
    pcount = 0;
    plist.clear();
    plist.resize(((xpoints + 1) * (ypoints + 1) * (zpoints + 1)), 0);
    for (int j = 0; j < vlist->size(); j++)
    {
      vid = vlist->at(j);
      ocol = vid % xpoints;
      orow = (vid / xpoints) % ypoints;
      oplane = vid / (xpoints * ypoints);
      for (int k = 0; k < 8; k++)
      {
        if (k == 0) col = ocol, row = orow, plane = oplane;
        if (k == 1) col = ocol + 1, row = orow, plane = oplane;
        if (k == 2) col = ocol, row = orow + 1, plane = oplane;
        if (k == 3) col = ocol + 1, row = orow + 1, plane = oplane;
        if (k == 4) col = ocol, row = orow, plane = oplane + 1;
        if (k == 5) col = ocol + 1, row = orow, plane = oplane + 1;
        if (k == 6) col = ocol, row = orow + 1, plane = oplane + 1;
        if (k == 7) col = ocol + 1, row = orow + 1, plane = oplane + 1;
        pid = (plane * (xpoints + 1) * (ypoints + 1)) + (row * (xpoints + 1)) + col;
        if (plist[pid] == 0)
        {
          plist[pid] = pcount;
          pcount++;
          outFile << (col * resx) << "  " << (row * resy) << "  " << (plane * resz) << endl;
        }
      }
    }
    outFile << endl;
    outFile << "CELLS " << vlist->size() << " " << vlist->size() * 9 << endl;
    for (int j = 0; j < vlist->size(); j++)
    {
      vid = vlist->at(j);
      ocol = vid % xpoints;
      orow = (vid / xpoints) % ypoints;
      oplane = vid / (xpoints * ypoints);
      outFile << "8 ";
      for (int k = 0; k < 8; k++)
      {
        if (k == 0) col = ocol, row = orow, plane = oplane;
        if (k == 1) col = ocol + 1, row = orow, plane = oplane;
        if (k == 2) col = ocol, row = orow + 1, plane = oplane;
        if (k == 3) col = ocol + 1, row = orow + 1, plane = oplane;
        if (k == 4) col = ocol, row = orow, plane = oplane + 1;
        if (k == 5) col = ocol + 1, row = orow, plane = oplane + 1;
        if (k == 6) col = ocol, row = orow + 1, plane = oplane + 1;
        if (k == 7) col = ocol + 1, row = orow + 1, plane = oplane + 1;
        pid = (plane * (xpoints + 1) * (ypoints + 1)) + (row * (xpoints + 1)) + col;
        outFile << plist[pid] << "  ";
      }
      outFile << endl;
    }
    outFile << endl;
    outFile << "CELL_TYPES " << vlist->size() << endl;
    for (int j = 0; j < vlist->size(); j++)
    {
      outFile << "11" << endl;
    }
    outFile << endl;
    outFile << "CELL_DATA " << vlist->size() << endl;
    outFile << "SCALARS GrainID int" << endl;
    outFile << "LOOKUP_TABLE default" << endl;
    for (int j = 0; j < vlist->size(); j++)
    {
      vid = vlist->at(j);
      outFile << voxels[vid].grainname << endl;
    }
    outFile << endl;
    outFile << "SCALARS KernelAvgDisorientation float" << endl;
    outFile << "LOOKUP_TABLE default" << endl;
    for (int j = 0; j < vlist->size(); j++)
    {
      vid = vlist->at(j);
      outFile << voxels[vid].kernelmisorientation << endl;
    }
    outFile << endl;
    outFile << "SCALARS GrainAvgDisorientation float" << endl;
    outFile << "LOOKUP_TABLE default" << endl;
    for (int j = 0; j < vlist->size(); j++)
    {
      vid = vlist->at(j);
      outFile << voxels[vid].misorientation << endl;
    }
    outFile << endl;
    outFile << "SCALARS ImageQuality float" << endl;
    outFile << "LOOKUP_TABLE default" << endl;
    for (int j = 0; j < vlist->size(); j++)
    {
      vid = vlist->at(j);
      outFile << voxels[vid].imagequality << endl;
    }
    outFile << endl;
    outFile << "SCALARS SchmidFactor float" << endl;
    outFile << "LOOKUP_TABLE default" << endl;
    for (int j = 0; j < vlist->size(); j++)
    {
      outFile << grains[i].schmidfactor << endl;
    }
    outFile.close();
    plist.clear();
    plist.resize(((xpoints + 1) * (ypoints + 1) * (zpoints + 1)), 0);
  }
#endif


}
