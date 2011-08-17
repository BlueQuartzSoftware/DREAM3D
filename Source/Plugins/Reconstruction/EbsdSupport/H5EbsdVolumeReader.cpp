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

#include "H5EbsdVolumeReader.h"

#include "hdf5.h"

#include "H5Support/H5Utilities.h"
#include "H5Support/H5Lite.h"

#include "Reconstruction/ReconstructionFunc.h"
#include "Reconstruction/EbsdSupport/H5AngVolumeReader.h"
#include "Reconstruction/EbsdSupport/H5CtfVolumeReader.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5EbsdVolumeReader::H5EbsdVolumeReader() :
    m_Orientation(Ebsd::NoOrientation)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5EbsdVolumeReader::~H5EbsdVolumeReader()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int H5EbsdVolumeReader::loadData(ReconstructionFunc* m, std::vector<QualityMetricFilter::Pointer> filters)
{
  int err = -1;
  // This is meant to be subclassed.

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMArray<bool>::Pointer H5EbsdVolumeReader::determineGoodVoxels(std::vector<QualityMetricFilter::Pointer> filters,
                                                               std::vector<void*> dataPointers,
                                                               size_t nPoints,
                                                               std::vector<Ebsd::NumType> dTypes)
{
  if (filters.size() == 0) { return AIMArray<bool>::NullPointer(); }
  size_t i = 0;
  size_t nFilters = filters.size();
  for (size_t i = 0; i < nFilters; ++i)
  {
    filters[i]->setInput(dataPointers[i]);
    filters[i]->setDataType(dTypes[i]);
    filters[i]->setNumValues(nPoints);
    filters[i]->filter();
  }

  // Get the first bool array to use as a reference
  AIMArray<bool>::Pointer array = filters[0]->getOutput();

  for (size_t i = 1; i < nFilters; ++i)
  {
    AIMArray<bool>::Pointer array2 = filters[i]->getOutput();
    bool* ref = array->GetPointer(0);
    bool* ar2 = array2->GetPointer(0);
    for (size_t p = 0; p < nPoints; ++p)
    {
      if (ref[p] == false || ar2[p] == false)
      {
        ref[p] == false;
      }
    }
  }
  return array;
}
