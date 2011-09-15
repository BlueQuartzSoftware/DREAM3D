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

#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>

#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

#include "H5Support/H5Utilities.h"
#include "H5Support/H5Lite.h"

#include "EbsdLib/EbsdConstants.h"

#include "MXA/Common/MXASetGetMacros.h"
#include "MXA/Common/LogTime.h"

#include "DREAM3D/DREAM3DVersion.h"
#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/Common/AIMArray.hpp"
#include "DREAM3D/HDF5/VTKH5Constants.h"
#include "DREAM3D/HDF5/H5VoxelWriter.h"
#include "DREAM3D/Common/VTKUtils/VTKRectilinearGridFileReader.h"


#define APPEND_DATA_TRUE 1
#define APPEND_DATA_FALSE 0

#define TEST 0

hid_t m_FileId = 0;

/** @brief Holds a single Euler Angle Set */
class EulerSet
{
  public:
    float e0;
    float e1;
    float e2;
  virtual ~EulerSet() {};
    EulerSet() : e0(0.0f), e1(0.0f), e2(0.0f) {}
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int openHDF5File(const std::string m_FileName, bool appendData)
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
int closeHDF5File()
{
  // Close the file when we are finished with it
  return H5Utilities::closeFile(m_FileId);
}

/**
 *
 */
template<typename T>
int writeScalarData(const std::string &hdfPath,
                    typename AIMArray<T>::Pointer scalar_data,
                    const char *label,
                    int numComp, int32_t rank, hsize_t* dims)
{
  hid_t gid = H5Gopen(m_FileId, hdfPath.c_str(), H5P_DEFAULT );
  if (gid < 0)
  {
    std::cout << "Error opening Group " << hdfPath << std::endl;
    return gid;
  }
  herr_t err = H5Utilities::createGroupsFromPath(H5_SCALAR_DATA_GROUP_NAME, gid);
  if (err < 0)
  {
    std::cout << "Error creating HDF Group " << H5_SCALAR_DATA_GROUP_NAME << std::endl;
    return err;
  }
  hid_t cellGroupId = H5Gopen(gid, H5_SCALAR_DATA_GROUP_NAME, H5P_DEFAULT );
  if(err < 0)
  {
    std::cout << "Error writing string attribute to HDF Group " << H5_SCALAR_DATA_GROUP_NAME << std::endl;
    return err;
  }

  T* data = const_cast<T*>(scalar_data->GetPointer(0));


  std::string name (label);
  err = H5Lite::replacePointerDataset(cellGroupId, name, rank, dims, data);
  if (err < 0)
  {
    std::cout << "Error writing array with name: " << std::string (label) << std::endl;
  }
  err = H5Lite::writeScalarAttribute(cellGroupId, name, std::string(H5_NUMCOMPONENTS), numComp);
  if (err < 0)
  {
    std::cout << "Error writing dataset " << label << std::endl;
  }
  err = H5Gclose(cellGroupId);

  err = H5Gclose(gid);
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class GrainIdVoxels
{
  public:
    MXA_SHARED_POINTERS(GrainIdVoxels);
    MXA_STATIC_NEW_MACRO(GrainIdVoxels);
    MXA_TYPE_MACRO(GrainIdVoxels);

    virtual ~GrainIdVoxels() {}

    float resx;
    float resy;
    float resz;

    int xpoints;
    int ypoints;
    int zpoints;
    int totalpoints;
    std::vector<Ebsd::CrystalStructure>                    crystruct;
    std::vector<DREAM3D::Reconstruction::PhaseType>        phaseType;

    DECLARE_WRAPPED_ARRAY(grain_indicies, m_GrainIndicies, int)
    DECLARE_WRAPPED_ARRAY(phases, m_Phases, int);
    DECLARE_WRAPPED_ARRAY(euler1s, m_Euler1s, float);
    DECLARE_WRAPPED_ARRAY(euler2s, m_Euler2s, float);
    DECLARE_WRAPPED_ARRAY(euler3s, m_Euler3s, float);

  protected:
    GrainIdVoxels() {}
  private:
    GrainIdVoxels(const GrainIdVoxels&); // Copy Constructor Not Implemented
    void operator=(const GrainIdVoxels&); // Operator '=' Not Implemented
};



/**
 *
 * @param h5File
 * @param data
 * @param nx
 * @param ny
 * @param nz
 * @return
 */
int writeVtkDataToHDF5File(const std::string &h5File, AIMArray<int>::Pointer data, int &nx, int &ny, int &nz)
{
  int err = 0;
  err = openHDF5File(h5File, true);

  int totalPoints = nx * ny * nz;
  int32_t rank = 1;
  hsize_t dims[1] =
  { totalPoints };

  int numComp = 1;
  err = writeScalarData<int>(DREAM3D::HDF5::VoxelDataName, data, DREAM3D::VTK::GrainIdScalarName.c_str(), numComp, rank, dims);
  if (err < 0)
  {
    std::cout << "Error Writing Scalars '" << DREAM3D::VTK::GrainIdScalarName.c_str() << "' to " << DREAM3D::HDF5::VoxelDataName << std::endl;
    return err;
  }
  // Close the file when we are done with it.
  err = closeHDF5File();
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int writeEulerDataToHDF5File(const std::string &h5File, AIMArray<float>::Pointer data, int numComp, int32_t rank, hsize_t* dims)
{
  int err = 0;
  err = openHDF5File(h5File, true);

  err = writeScalarData<float>(DREAM3D::HDF5::VoxelDataName, data, DREAM3D::VTK::EulerAnglesName.c_str(), numComp, rank, dims);
  if (err < 0)
  {
    std::cout << "Error Writing Scalars '" << DREAM3D::VTK::EulerAnglesName.c_str() << "' to " << DREAM3D::HDF5::VoxelDataName << std::endl;
    return err;
  }
  // Close the file when we are done with it.
  err = closeHDF5File();

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ReadEulerFile(const std::string &filename, std::map<int, EulerSet> &gidToEulerMap)
{
  int err = -1;
  FILE* f = fopen(filename.c_str(), "rb");
  if (NULL == f)
  {
    std::cout << "Could not open Euler Angle File '" << filename << "'" << std::endl;
    return err;
  }
  err = 1;
  int read = 4;
  int gid;
  while (read == 4)
  {
    EulerSet e;
    read = fscanf(f, "%d %f %f %f", &gid, &(e.e0), &(e.e1), &(e.e2) );
    gidToEulerMap[gid] = e;
  }

  //Close the file when we are done
  fclose(f);

  return err;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{

  std::cout << "Starting Vtk to HDF5 Conversion..." << std::endl;

  try
  {
#if TEST
    std::string vFile = "C:\\Data\\Test.vtk";
    std::string h5File = "C:\\Data\\Test.h5voxel";
    std::string eulerFile = "C:\\Data\\Test.euler";
#else
    // Handle program options passed on command line.
    TCLAP::CmdLine cmd("VtkToHDF5", ' ', DREAM3DLib::Version::Complete);

    TCLAP::ValueArg<std::string> vtkFile( "i", "vtkfile", "VTK Rectilinear Input File", true, "", "Vtk Input File");
    cmd.add(vtkFile);

//    TCLAP::ValueArg<std::string> angleFileArg( "e", "eulerfile", "Euler Angle File", false, "", "Euler Angle File");
//    cmd.add(angleFileArg);

    TCLAP::ValueArg<std::string> h5InputFileArg( "t", "h5file", "Target HDF5 File", true, "", "Target HDF5 File");
    cmd.add(h5InputFileArg);

    // Parse the argv array.
    cmd.parse(argc, argv);

    if (argc == 1)
    {
      std::cout << "VtkToHDF5 program was not provided any arguments. Use the --help argument to show the help listing." << std::endl;
      return EXIT_FAILURE;
    }


    std::string vFile = vtkFile.getValue();
    std::string h5File = h5InputFileArg.getValue();
  //  std::string eulerFile = angleFileArg.getValue();

#endif

    AIMArray<int>::Pointer voxels;
    int nx = 0;
    int ny = 0;
    int nz = 0;

    std::cout << "Merging the GrainID data from " << vFile << std::endl;
    std::cout << "  into" << std::endl;
    std::cout << "file: " << h5File << std::endl;


    std::cout << "Reading the Vtk data file...." << std::endl;
    VTKRectilinearGridFileReader::Pointer reader = VTKRectilinearGridFileReader::New();
    reader->setInputFileName(vFile);
    int err = reader->readFile();
    if (err < 0)
    {
     return EXIT_FAILURE;
    }
    reader->getDims(nx, ny, nz);
    std::cout << "Vtk File has dimensions: " << nx << " x " << ny << " x " << nz << std::endl;
    voxels = reader->getGrainIds();


    H5VoxelWriter::Pointer h5VolWriter = H5VoxelWriter::New();
    if (h5VolWriter.get() == NULL)
    {
      std::cout << "Could not create H5VoxelWriter." << std::endl;
      return EXIT_FAILURE;
    }
    h5VolWriter->setFilename(h5File);
    err = h5VolWriter->writeGrainIds(voxels->GetPointer(0), voxels->GetNumberOfTuples(), false);
    if (err < 0)
    {
      return EXIT_FAILURE;
    }
    int volDims[3] = { nx, ny, nz };
    float spacing[3] = {1.0f, 1.0f, 1.0f};
    reader->getScaling(spacing);
    float origin[3] = { 0.0f, 0.0f, 0.0f };
    err = h5VolWriter->writeStructuredPoints(volDims, spacing, origin, true);
    if (err < 0)
    {
      return EXIT_FAILURE;
    }

#if 0
    std::cout << "Now Overwriting the GrainID data set in the HDF5 file...." << std::endl;
    err = writeVtkDataToHDF5File(h5File, voxels, nz, ny, nz);
    if (err < 0)
    {
     std::cout << "There was an error writing the grain id data. Check other errors for possible clues." << std::endl;
     return EXIT_FAILURE;
    }
    std::cout << "+ Done Writing the Grain ID Data." << std::endl;


    std::map<int, EulerSet> gidToEulerMap;
    if (eulerFile.empty() == false)
    {
      std::cout << "Reading the Euler Angle Data...." << std::endl;
      err = ReadEulerFile(eulerFile, gidToEulerMap);
      if (err < 0)
      {
        std::cout << "Error Reading the Euler Angle File" << std::endl;
        return EXIT_FAILURE;
      }

    // Over Write the Euler Angles if the Euler File was supplied

      std::cout << "Now Over Writing the Euler Angles data in the HDF5 file....." << std::endl;
      int totalPoints = nx * ny * nz;
      int numComp = 3;
      // Loop over each Voxel getting its Grain ID and then setting the Euler Angle
      AIMArray<float>::Pointer dataf = AIMArray<float>::CreateArray(totalPoints * 3);
      for (int i = 0; i < totalPoints; ++i)
      {
        EulerSet& angle = gidToEulerMap[voxels->GetValue(i)];
        dataf->SetValue(i*3, angle.e0);
        dataf->SetValue(i * 3 + 1, angle.e1);
        dataf->SetValue(i * 3 + 2, angle.e2);
      }
      // This is going to be a 2 Dimension Table Data set.
      int32_t rank = 2;
      hsize_t dims[2] = {totalPoints, numComp};
      err = writeEulerDataToHDF5File(h5File, dataf, numComp, rank, dims);
      if (err < 0)
      {
       std::cout << "There was an error writing the Euler Angle data. Check other errors for possible clues." << std::endl;
       return EXIT_FAILURE;
      }
      std::cout << "+ Done Writing the Euler Angle Data." << std::endl;
    }
#endif
  }
  catch (TCLAP::ArgException &e) // catch any exceptions
  {
    std::cerr << logTime() << " error: " << e.error() << " for arg " << e.argId() << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "Successfully completed the Writing of the .h5voxel." << std::endl;

  return EXIT_SUCCESS;
}


