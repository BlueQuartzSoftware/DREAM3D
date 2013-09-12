/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#include <iostream>
#include <QtCore/QString>
#include <vector>
#include <fstream>
#include <map>

#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

#include "H5Support/H5Utilities.h"
#include "H5Support/H5Lite.h"



#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DREAM3DVersion.h"
#include "DREAM3DLib/HDF5/VTKH5Constants.h"


#define APPEND_DATA_TRUE 1
#define APPEND_DATA_FALSE 0

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


/*
  void tokenize(const string& str, std::vector<string>& tokens, const
  string& delimiters = " ")
  ====================================================================
  Taken from http://www.oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html

  "A very common operation with strings, is to tokenize it with a
  delimiter of your own choice. This way you can easily split the
  string up in smaller pieces, without fiddling with the find()
  methods too much. In C, you could use strtok() for character arrays,
  but no equal function exists for strings. This means you have to
  make your own. Here is a couple of suggestions, use what suits your
  best."
  ====================================================================
*/
void tokenize(const std::string& str,
                      std::vector<std::string>& tokens,
                      const std::string& delimiters = " ")
{
    // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);

    // Find first "non-delimiter".
  std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));

        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);

        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

/**
 *
 * @param FileName
 * @param data
 * @param nx X Dimension
 * @param ny Y Dimension
 * @param nz Z Dimension
 */
int  ReadPHFile(std::string FileName, std::vector<int> &data, int &nx, int &ny, int &nz)
{
  std::string line;
  line.resize(1024);

  std::string delimeters(", ;\t"); /* delimeters to split the data */
  std::vector<std::string> tokens; /* vector to store the split data */
  //std::vector<int> data; /* vector to store the data */

  int error, spin; /* dummy variables */
  //int nx, ny, nz;

  std::ifstream InFile;
  InFile.open(FileName.toLatin1().data(), std::ios_base::binary);
  if (!InFile)
  {
    std::cout << "Failed to open: " << FileName << std::endl;
    return -1;
  }

  getline(InFile, line);

  tokenize(line, tokens, delimeters);

  // Process the header information from the PH file.
  error = 0;
  error += sscanf(tokens[0].toLatin1().data(), "%d", &nx);
  error += sscanf(tokens[1].toLatin1().data(), "%d", &ny);
  error += sscanf(tokens[2].toLatin1().data(), "%d", &nz);
  if (error < 0)
  {
    std::cout << "Error parsing Dimensions from ph file. The line that is being parsed was \n'" <<
       line << "'" <<  std::endl;
    return -1;
  }
  tokens.clear();

  //  cout << "INFO: PH file grid size: " << nx << "\t" << ny << "\t" << nz << endl;;

  //MCgrid3D* grid = new grid(nx,ny,nz);

  // Get the remaining two lines of the header and ignore
  getline(InFile, line, '\n');
  getline(InFile, line, '\n');

  //The PH file has a unique format of 20 entries on each line. I have
  //now idea who initiated this insanity but I am about to propetuate
  //it.
  //
  //The most simple thing todo is to read the entire dataset into one
  //long vector and then read that vector to assign values to the grid

  while (getline(InFile, line, '\n') != NULL)
  {
    tokens.clear();
    error = 0;
    tokenize(line, tokens, delimeters);
    //        cout << line << endl;
    //        for(int i=0; i < tokens.size(); i++ )
    //              cout << setw(6) << tokens[i];
    //        cout << endl;

    for (size_t in_spins = 0; in_spins < tokens.size(); in_spins++)
    {
      error += sscanf(tokens[in_spins].toLatin1().data(), "%d", &spin);
      data.push_back(spin);
    }
    //        if(error != 20)
    //              {
    //                cout << "ERROR: Invalid number of line entries in PH file" << endl;
    //              }
  }

  tokens.clear();

  InFile.close();
  return 0;
}

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
                    const std::vector<T> &scalar_data,
                    const char *label,
                    int numComp, int32_t rank, hsize_t* dims)
{
  hid_t gid = H5Gopen(m_FileId, hdfPath.toLatin1().data(), H5P_DEFAULT );
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

  T* data = const_cast<T*>(&(scalar_data.front()));


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

/**
 *
 * @param h5File
 * @param data
 * @param nx
 * @param ny
 * @param nz
 * @return
 */
int writePhDataToHDF5File(const std::string &h5File, std::vector<int> &data, int &nx, int &ny, int &nz)
{
  int err = 0;
  err = openHDF5File(h5File, true);

  int totalPoints = nx * ny * nz;
  int32_t rank = 1;
  hsize_t dims[1] =
  { totalPoints };

  int numComp = 1;
  err = writeScalarData(DREAM3D::HDF5::VolumeDataContainerName, data, DREAM3D::CellData::GrainIds.toLatin1().data(), numComp, rank, dims);
  if (err < 0)
  {
    std::cout << "Error Writing Scalars '" << DREAM3D::CellData::GrainIds.toLatin1().data() << "' to " << DREAM3D::HDF5::VolumeDataContainerName << std::endl;
    return err;
  }
  // Close the file when we are done with it.
  err = closeHDF5File();
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int writeEulerDataToHDF5File(const std::string &h5File, std::vector<float> &data, int numComp, int32_t rank, hsize_t* dims)
{
  int err = 0;
  err = openHDF5File(h5File, true);

  err = writeScalarData(DREAM3D::HDF5::VolumeDataContainerName, data, DREAM3D::CellData::EulerAngles.toLatin1().data(), numComp, rank, dims);
  if (err < 0)
  {
    std::cout << "Error Writing Scalars '" << DREAM3D::CellData::EulerAngles.toLatin1().data() << "' to " << DREAM3D::HDF5::VolumeDataContainerName << std::endl;
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
  FILE* f = fopen(filename.toLatin1().data(), "rb");
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
  std::cout << "Starting Ph to HDF5 Merging..." << std::endl;

  try
  {
    // Handle program options passed on command line.
    TCLAP::CmdLine cmd("PhToHDF5", ' ', DREAM3DLib::Version::Complete());

    TCLAP::ValueArg<std::string> phFileArg( "p", "phfile", "Ph Input File", true, "", "Ph Input File");
    cmd.add(phFileArg);

    TCLAP::ValueArg<std::string> angleFileArg( "e", "eulerfile", "Euler Angle File", false, "", "Euler Angle File");
    cmd.add(angleFileArg);

    TCLAP::ValueArg<std::string> h5InputFileArg( "t", "h5file", "Target HDF5 File", true, "", "Target HDF5 File");
    cmd.add(h5InputFileArg);

    // Parse the argv array.
    cmd.parse(argc, argv);
    if (argc == 1)
    {
      std::cout << "PhToHDF5 program was not provided any arguments. Use the --help argument to show the help listing." << std::endl;
      return EXIT_FAILURE;
    }


    std::string phFile = phFileArg.getValue();
    std::string h5File = h5InputFileArg.getValue();

    std::vector<int> voxels;
    int nx = 0;
    int ny = 0;
    int nz = 0;

    std::cout << "Merging the GrainID data from " << phFile << std::endl;
    std::cout << "  into" << std::endl;
    std::cout << "file: " << h5File << std::endl;


    std::cout << "Reading the Ph data file...." << std::endl;
    int err = ReadPHFile(phFile, voxels, nx, ny, nz);
    if (err < 0)
    {
     return EXIT_FAILURE;
    }
    std::cout << "Ph File has dimensions: " << nx << " x " << ny << " x " << nz << std::endl;


    std::cout << "Now Overwriting the GrainID data set in the HDF5 file...." << std::endl;
    err = writePhDataToHDF5File(h5File, voxels, nz, ny, nz);
    if (err < 0)
    {
     std::cout << "There was an error writing the grain id data. Check other errors for possible clues." << std::endl;
     return EXIT_FAILURE;
    }
    std::cout << "+ Done Writing the Grain ID Data." << std::endl;


    std::map<int, EulerSet> gidToEulerMap;
    if (angleFileArg.getValue().empty() == false)
    {
      std::cout << "Reading the Euler Angle Data...." << std::endl;
      err = ReadEulerFile(angleFileArg.getValue(), gidToEulerMap);
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
      std::vector<float> dataf(totalPoints * 3);
      for (int i = 0; i < totalPoints; ++i)
      {
        EulerSet& angle = gidToEulerMap[voxels[i]];
        dataf[i * 3] = angle.e0;
        dataf[i * 3 + 1] = angle.e1;
        dataf[i * 3 + 2] = angle.e2;
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

  }
  catch (TCLAP::ArgException &e) // catch any exceptions
  {
    std::cerr << logTime() << " error: " << e.error() << " for arg " << e.argId() << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "Successfully completed the merge." << std::endl;

  return EXIT_SUCCESS;
}
