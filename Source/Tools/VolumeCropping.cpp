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

#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

#include "DREAM3DLib/DREAM3DVersion.h"

#include "DREAM3DLib/HDF5/H5VoxelReader.h"
#include "DREAM3DLib/HDF5/H5VoxelWriter.h"



 /**
     * @brief This will parse the pixel coordinates for each class that is to be initialized
     * @param coords The raw string the user entered on the command line
     * @param inputs The EMMPM_Data structure to store the results
     * @return
     */
int parseInitCoords(const std::string &str, int64_t* values)
{
  int n = sscanf(str.c_str(), "%lld,%lld,%lld", values, values+1, values+2 );
  if (n != 3)
  {
    ::memset(values, 0, 12);
    return -1;
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int sanityCheckInputs(int64_t* min, int64_t* max, int64_t* volDims)
{

  for(size_t i = 0; i < 3; ++i) {
    if (min[i] < 0 || max[i] > volDims[i]) { return -1;}
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void processArray(int64_t* min, int64_t* max, int64_t* dims,
                 typename DataArray<T>::Pointer in,
                 typename DataArray<T>::Pointer out)
{

  T* src = NULL;
  T* dest = out->WritePointer(0, (max[0]-min[0]+1) * (max[1]-min[1]+1) *(max[2]-min[2]+1) );
  size_t in_index = 0;
  size_t nCopyElements = (max[0] - min[0] + 1);
  for (int z = min[2]; z <= max[2]; ++z)
  {
    for(int y = min[1]; y <= max[1]; ++y)
    {
      // Get the index into the input array for the start of this chunk of elements
      in_index = (dims[0] * dims[1] * z) + dims[0]*y; // + 0;
      src = in->GetPointer(in_index);
      // Copy x elements from array
      ::memcpy(dest, src, nCopyElements * sizeof(T));
      // Increase the pointer to the next location to copy data into
      dest = dest + nCopyElements;
    }

  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int cropVolume(H5VoxelReader::Pointer reader, const std::string &outfile, int64_t* min, int64_t* max)
{

  int err = 0;

  int64_t dims[3];
  float volRes[3];
  float origin[3];
  err = reader->getSizeResolutionOrigin(dims, volRes, origin);


  std::vector<Ebsd::CrystalStructure> crystruct;
  std::vector<DREAM3D::Reconstruction::PhaseType> phaseType;

  DECLARE_WRAPPED_ARRAY(grain_indicies, m_GrainIndicies, int)
  DECLARE_WRAPPED_ARRAY(phases, m_Phases, int);
  DECLARE_WRAPPED_ARRAY(eulerangles, m_EulerAngles, float);

  m_GrainIndicies = DataArray<int>::CreateArray(0);
  m_Phases = DataArray<int>::CreateArray(0);
  m_EulerAngles = DataArray<float>::CreateArray(0);


  int64_t totalpoints = dims[0] * dims[1] * dims[2];
  grain_indicies = m_GrainIndicies->WritePointer(0, totalpoints);
  phases = m_Phases->WritePointer(0, totalpoints);
  eulerangles = m_EulerAngles->WritePointer(0, 3*totalpoints);


  std::cout << "Reading Voxel Data" << std::endl;
  err = reader->readVoxelData(grain_indicies, phases, eulerangles, crystruct, phaseType, totalpoints);
  if (err < 0)
  {
    std::cout << "Error reading h5voxel file." << std::endl;
    return EXIT_FAILURE;
  }

  // Now process the data to crop out the needed volume
  // Allocate the output array
  DataArray<int>::Pointer outGrainIdices = DataArray<int>::CreateArray(0);
  processArray<int>(min, max, dims, m_GrainIndicies, outGrainIdices);

  DataArray<int>::Pointer outPhases = DataArray<int>::CreateArray(0);
  processArray<int>(min, max, dims, m_Phases, outPhases);

  DataArray<float>::Pointer outEulerAngles = DataArray<float>::CreateArray(0);
  processArray<float>(min, max, dims, m_EulerAngles, outEulerAngles);

  H5VoxelWriter::Pointer writer = H5VoxelWriter::New();
  writer->setFileName(outfile);

  err = writer->writeCrystalStructures(crystruct, false);
  if (err < 0) { return err; }

  err = writer->writePhaseTypes(phaseType, true);
  if (err < 0) { return err; }

  int64_t volDims[3] = { (max[0]-min[0]+1), (max[1]-min[1]+1), (max[2]-min[2]+1) };
  totalpoints = volDims[0] * volDims[1] * volDims[2];
  err = writer->writeEulerData(outEulerAngles->GetPointer(0), totalpoints, true);
  if (err < 0) { return err; }

  err = writer->writeGrainIds(outGrainIdices->GetPointer(0), totalpoints, true);
  if (err < 0) { return err; }

  err = writer->writePhaseIds(outPhases->GetPointer(0), totalpoints, true);
  if (err < 0) { return err; }


  err = writer->writeStructuredPoints(volDims, volRes, origin, true);
  if (err < 0) { return err; }



  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  std::cout << "VolumeCropping Version " <<  DREAM3DLib::Version::Major << "."
      << DREAM3DLib::Version::Minor  << " Starting.... " << std::endl;
  int err = 0;
  try
  {

  // Handle program options passed on command line.
     TCLAP::CmdLine cmd("VolumeCropping", ' ', DREAM3DLib::Version::Complete);

     TCLAP::ValueArg<std::string> inputFile( "i", "input", ".h5voxel input file", true, "", ".h5voxel Input file");
     cmd.add(inputFile);

     TCLAP::ValueArg<std::string> outputFile( "o", "output", ".h5voxel output file", true, "", ".h5voxel Output file");
     cmd.add(outputFile);

     TCLAP::ValueArg<std::string> minCoords( "", "min",
                                             "Comma Separated list of Coords for Corner with minimum coordinates", true, "", "Minimum Coordinates for corner of volume");
     cmd.add(minCoords);

     TCLAP::ValueArg<std::string> maxCoords( "", "max", "Comma Separated list of Coords for Corner with maximum coordinates", true, "", "Maximum Coordinates for corner of volume");
     cmd.add(maxCoords);

     TCLAP::SwitchArg showInfo ("", "info", "Show Volume Dimensions of input file and exit", false);
     cmd.add(showInfo);

     // Parse the argv array.
     cmd.parse(argc, argv);

     if (argc == 1)
     {
       std::cout << "VolumeCropping program was not provided any arguments. Use the --help argument to show the help listing." << std::endl;
       return EXIT_FAILURE;
     }


     H5VoxelReader::Pointer reader = H5VoxelReader::New();
     reader->setFileName(inputFile.getValue());
     int64_t dims[3];
     float volRes[3];
     float origin[3];
     err = reader->getSizeResolutionOrigin(dims, volRes, origin);
     if (err < 0)
     {
       std::cout << "Error reading the volume dimensions from the input file" << std::endl;
       return EXIT_FAILURE;
     }
     if (showInfo.getValue() == true)
     {
       std::cout << "Input File: " << inputFile.getValue() << std::endl;
       std::cout << "Volume Dimensions [0, 0, 0] and ["
           << dims[0] << ", " << dims[1] << ", " << dims[2] << "]" << std::endl;
         return EXIT_SUCCESS;
     }


     int64_t min[3];
     int64_t max[3];

     parseInitCoords(minCoords.getValue(), min);
     parseInitCoords(maxCoords.getValue(), max);

     err = sanityCheckInputs(min, max, dims);
     if (err < 0)
     {
       std::cout << "The cropped volume with corner [" << min[0] << ", " << min[1] << ", " << min[2]
       << "] and [" << max[0] << ", " << max[1] << ", " << max[2] << "] has a volume outside of" << std::endl;
       std::cout << "the volume [0, 0, 0] and [" << dims[0] << ", " << dims[1] << ", " << dims[2] << "]" << std::endl;
       return EXIT_FAILURE;
     }

     err = cropVolume(reader, outputFile.getValue(), min, max);
     if (err < 0)
     {
       std::cout << "Error Cropping the volume and writing the output file" << std::endl;
       return EXIT_FAILURE;
     }

  }
  catch (TCLAP::ArgException &e)
  {
    std::cerr << " error: " << e.error() << " for arg " << e.argId() << std::endl;
    std::cout << "** Unknown Arguments. Displaying help listing instead. **" << std::endl;
    return -1;
  }

  std::cout << "VolumeCropping Ending" << std::endl;
  return EXIT_SUCCESS;
}



