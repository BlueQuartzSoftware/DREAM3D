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

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "MXA/HDF5/H5Utilities.h"
#include "MXA/HDF5/H5Lite.h"

#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/HDF5/VTKH5Constants.h"


#define APPEND_DATA_TRUE 1
#define APPEND_DATA_FALSE 0

hid_t m_FileId = 0;

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
 * @param nx
 * @param ny
 * @param nz
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
  InFile.open(FileName.c_str(), std::ios_base::binary);
  if (!InFile)
  {
    std::cout << "Failed to open: " << FileName << std::endl;
    return -1;
  }

  getline(InFile, line);

  tokenize(line, tokens, delimeters);

  // Process the header information from the PH file.
  error = 0;
  error += sscanf(tokens[0].c_str(), "%d", &nx);
  error += sscanf(tokens[1].c_str(), "%d", &ny);
  error += sscanf(tokens[2].c_str(), "%d", &nz);
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

    for (int in_spins = 0; in_spins < tokens.size(); in_spins++)
    {
      error += sscanf(tokens[in_spins].c_str(), "%d", &spin);
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
  hid_t gid = H5Gopen(m_FileId, hdfPath.c_str() );
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
  hid_t cellGroupId = H5Gopen(gid, H5_SCALAR_DATA_GROUP_NAME );
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
int writePhDataToHDF5File(const std::string h5File, std::vector<int> &data, int &nx, int &ny, int &nz)
{
  int err = 0;


  err = openHDF5File(h5File, true);

  int totalPoints = nx * ny * nz;

  int32_t rank = 1;
  hsize_t dims[1] =
  { totalPoints };

  int numComp = 1;
  err = writeScalarData(AIM::HDF5::VoxelDataName, data, AIM::VTK::GrainIdScalarName.c_str(), numComp, rank, dims);
  if (err < 0)
  {
    std::cout << "Error Writing Scalars '" << AIM::VTK::GrainIdScalarName.c_str() << "' to " << AIM::HDF5::VoxelDataName << std::endl;
    return err;
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  std::cout << "Starting Ph to HDF5 Merging..." << std::endl;
  /* 2 Arguments are the input ph file and the input HDF5 file where you want to
   * over write the current GrainID data.
   */

  if (argc != 3)
  {
    std::cout << "2 Arguments are needed: The input ph file and the inptu HDF5 file.n The data from the Ph file will be merged into the HDF5 file." << std::endl;
    return EXIT_FAILURE;
  }


  std::cout << "Merging the GrainID data from " << argv[1] << std::endl;
  std::cout << "  into" << std::endl;
  std::cout << "file: " << argv[2] << std::endl;

  std::string phFile = argv[1];

  std::vector<int> data;
  int nx = 0;
  int ny = 0;
  int nz = 0;

  std::cout << "Reading the Ph data file...." << std::endl;
  int err = ReadPHFile(phFile, data, nx, ny, nz);
  if (err < 0)
  {
    return EXIT_FAILURE;
  }
  std::cout << "Ph File has dimensions: " << nx << " x " << ny << " x " << nz << std::endl;

  std::cout << "Now Overwriting the GrainID data set in the HDF5 file...." << std::endl;
  std::string h5File(argv[2]);
  err = writePhDataToHDF5File(h5File, data, nz, ny, nz);
  if (err < 0)
  {
    std::cout << "There was an error writing the grain id data. Check other errors for possible clues." << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Successfully completed the merge." << std::endl;

  return EXIT_SUCCESS;
}
