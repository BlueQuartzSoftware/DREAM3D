/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "PhWriter.h"

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <map>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhWriter::PhWriter() :
DREAM3D::FileWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhWriter::~PhWriter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PhWriter::writeHeader()
{
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PhWriter::writeFile()
{
//   std::string OutputName;
  DataContainer* m = getDataContainer();
  if (NULL == m)
  {
    std::stringstream ss;
    ss << "DataContainer Pointer was NULL and Must be valid." << __FILE__ << "("<<__LINE__<<")";
    setErrorMessage(ss.str());
    setErrorCondition(-1);
    return -1;
  }

  int32_t* grain_indicies = 0;
  {
    IDataArray::Pointer iDataArray = m->getVoxelData(DREAM3D::VoxelData::GrainIds);
    if (iDataArray.get() == 0) {
      return -10;
    }
    if (static_cast<size_t>(m->totalPoints()) != iDataArray->GetNumberOfTuples()) {
      return -20;
    }
    grain_indicies =
    IDataArray::SafeReinterpretCast<IDataArray*, Int32ArrayType*, int32_t* >(m->getVoxelData(DREAM3D::VoxelData::GrainIds).get());
    if (0 == grain_indicies) {
      return -30;
    }
  }

  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] = {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };
  int64_t totalpoints = dims[0] * dims[1] * dims[2];
  // Change the name of the input filename for outout
  // std::vector<std::string> tokens;
  // std::string delimeters = "."; // Only a period
  //  std::tokenize(filename, tokens, delimeters);

  //OutputName = tokens[0] + ".ph";
  std::ofstream outfile;
  outfile.open(getFileName().c_str(), std::ios_base::binary);
  if(!outfile)
  {
    std::cout << "Failed to open: " << getFileName() << std::endl;
    return -1;
  }


  // Find the unique number of grains
  std::map<int, bool> used;
  for (int i = 0; i < totalpoints; ++i)
  {
    used[grain_indicies[i]] = true;
  }

  int grains = 0;
  typedef std::map<int, bool>::iterator iterator;
  for (iterator i = used.begin(); i != used.end(); i++)
  {
    if((*i).second == true)
    {
      grains++;
    }
  }
  //std::cout<<grains<< " " << used.size() << std::endl;

  outfile << "     " << dims[0] << "     " << dims[1] << "     " << dims[2] << "\n";
  outfile << "\'DREAM3\'              52.00  1.000  1.0       " << grains << "\n";
  outfile << " 0.000 0.000 0.000          0        \n"; // << grains << endl;

  int count = 0;
  for (int k = 0; k < totalpoints; k++)
  {
    outfile << std::setw(6) << grain_indicies[k];
    count++;
    if(count == 20)
    {
      outfile << "\n";
      count = 0;
    }
    //                    outfile << grid[i][j][k] << endl;
  }
  outfile << "\n";
  outfile.close();
  return 0;
}

