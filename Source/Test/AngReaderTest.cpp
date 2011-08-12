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

#include "H5Support/H5Utilities.h"
#include "MXA/Utilities/StringUtils.h"

#include "EbsdLib/TSL/AngReader.h"
#include "EbsdLib/TSL/H5AngReader.h"

#include "OIMImport/OIMImport.h"


#if _WIN32_

#else
  #define ANG_FILE "/Users/Shared/Data/Ang_Data/Test1/Test.ang"
  #define H5ANG_FILE "/tmp/Test.h5ang"
#endif

int main(int argc, char **argv)
{
  std::cout << "Starting AngReaderTest...." << std::endl;
  std::string angFile(ANG_FILE);
  AngReader reader;
  reader.setFileName(angFile);
  int err = reader.readHeaderOnly();

  std::vector<AngPhase::Pointer> phases = reader.getPhaseVector();
  if (phases.size() != 2)
  {
    std::cout << "Phase Count is Wrong. Should be 2 and is " << phases.size() << std::endl;
  }


  for (std::vector<AngPhase::Pointer>::iterator iter = phases.begin(); iter != phases.end(); ++iter )
  {
    std::cout << "=========================================================" << std::endl;
    (*iter)->printSelf(std::cout);
  }

  std::cout << "\n ******* Testing Import of Ang File to HDF5" << std::endl;
  if (err < 0)
  {
    std::cout << "Error reading file" << angFile << std::endl;
    return EXIT_FAILURE;
  }

  std::vector<std::string> filelist;
  filelist.push_back(angFile);

  OIMImport::Pointer h5importer = OIMImport::New();
  h5importer->setEbsdFileList(filelist);
  h5importer->setOutputFile(std::string(H5ANG_FILE) );
  h5importer->setZStartIndex(0);
  h5importer->setZEndIndex(1);
  h5importer->setZResolution(0.25);
  h5importer->run();
  std::cout << "=====> Import Complete" << std::endl;


  std::cout << "\n ***** Starting H5AngReader Test" << std::endl;
  hid_t fileId = H5Utilities::openFile(H5ANG_FILE, true);
  if (fileId < 0)
  {
    std::cout << "Error H5AngReader Test" << std::endl;
    return EXIT_FAILURE;
  }


  std::string index = StringUtils::numToString(0);
  hid_t gid = H5Gopen(fileId, index.c_str(), H5P_DEFAULT);
  H5AngReader::Pointer h5reader = H5AngReader::New();
  h5reader->setHDF5Path(index);
  err = h5reader->readHeader(gid);
  if (err < 0)
  {
    std::cout << "Error reading the .HDF5 Ang Header data" << std::endl;
    err = H5Gclose(gid);
    err = H5Fclose(fileId);
    return EXIT_FAILURE;
  }
  err = H5Gclose(gid);
  err = H5Fclose(fileId);


  std::cout << "Ending H5AngReaderTest" << std::endl;
  return EXIT_SUCCESS;
}
