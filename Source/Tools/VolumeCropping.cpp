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

#include "Dream3D/DREAM3DVersion.h"

#include "EbsdLib/H5EbsdVolumeReader.h"




 /**
     * @brief This will parse the pixel coordinates for each class that is to be initialized
     * @param coords The raw string the user entered on the command line
     * @param inputs The EMMPM_Data structure to store the results
     * @return
     */
int parseInitCoords(const std::string &coords, int* x, int* y, int* z)
{
  int n = sscanf(coords.c_str(), "%d,%d,%d", x, y, z );
  if (n != 3)
  {
    *x = *y = *z = 0;
    return -1;
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int sanityCheckInputs(int minX, int minY, int minZ, int maxX, int maxY, int maxZ, int volX, int volY, int volZ)
{
  if (minX < 0)
  {
    return -1;
  }
  if (minY < 0) { return -1;}
  if (minZ < 0) { return -1;}

  if (maxX > volX) { return -1;}
  if (maxY > volY) { return -1;}
  if (maxZ > volZ) { return -1;}

  return 0;
}

int main(int argc, char **argv)
{

  int err = 0;
  try
  {

  // Handle program options passed on command line.
     TCLAP::CmdLine cmd("VolumeCropping", ' ', DREAM3DLib::Version::Complete);

     TCLAP::ValueArg<std::string> inputFile( "i", "input", ".h5voxel input file", true, "", ".h5voxel Input file");
     cmd.add(inputFile);

     TCLAP::ValueArg<std::string> outputFile( "o", "output", ".h5voxel output file", true, "", ".h5voxel Output file");
     cmd.add(outputFile);

     TCLAP::ValueArg<std::string> minCoords( "", "min", "Corner with minimum coordinates", true, "", "Minimum Coordinates for corner of volume");
     cmd.add(minCoords);

     TCLAP::ValueArg<std::string> maxCoords( "", "max", "Corner with maximum coordinates", true, "", "Maximum Coordinates for corner of volume");
     cmd.add(maxCoords);

     // Parse the argv array.
     cmd.parse(argc, argv);

     if (argc == 1)
     {
       std::cout << "VolumeCropping program was not provided any arguments. Use the --help argument to show the help listing." << std::endl;
       return EXIT_FAILURE;
     }

     int minX, minY, minZ;
     int maxX, maxY, maxZ;

     parseInitCoords(minCoords.getValue(), &minX, &minY, &minZ);
     parseInitCoords(maxCoords.getValue(), &maxX, &maxY, &maxZ);


     H5EbsdVolumeReader::Pointer reader = H5EbsdVolumeReader::New();
     reader->setFileName(inputFile.getValue());

     int volX, volY, volZ;
     err = reader->getDims(volX, volY, volZ);
     if (err < 0)
     {
       std::cout << "Error reading the volume dimensions from the input file" << std::endl;
       return EXIT_FAILURE;
     }

     err = sanityCheckInputs(minX, minY, minZ, maxX, maxY, maxZ, volX, volY, volZ);
     if (err < 0)
     {
       std::cout << "The cropped volume with corner [" << minX << ", " << minY << ", " << minZ
       << "] and [" << maxX << ", " << maxY << ", " << maxZ << "] has a volume outside of" << std::endl;
       std::cout << "the volume [0, 0, 0] and [" << volX << ", " << volY << ", " << volZ << "]" << std::endl;
       return EXIT_FAILURE;
     }


  }
  catch (TCLAP::ArgException &e)
  {
    std::cerr << " error: " << e.error() << " for arg " << e.argId() << std::endl;
    std::cout << "** Unknown Arguments. Displaying help listing instead. **" << std::endl;
    return -1;
  }

  return EXIT_SUCCESS;
}



