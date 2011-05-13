/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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

#include "AngDataLoader.h"

#include <cmath>

#include <ANG/AngReader.h>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngDataLoader::AngDataLoader() :
m_ZResolution(1.0f),
m_Orientation(Ang::NoOrientation),
m_Cancel(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngDataLoader::~AngDataLoader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AngDataLoader::getSizeAndResolution(int &xpoints, int &ypoints, int &zpoints,
                                   float &xres, float &yres, float &zres)
{
  zpoints = (getZEndIndex() - getZStartIndex() );
  int xpointstemp = 0;
  int ypointstemp = 0;
  xpoints = 0;
  ypoints = 0;
  for(int i=0;i<zpoints;i++)
  {
    std::string angFName = m_DirectoryPattern->generateFullPathAngFileName(i + getZStartIndex());
    AngReader reader;
    reader.setFileName(angFName);
    int err = reader.readHeaderOnly();
    if (err < 0)
    {
      std::cout << "Error reading the .ang file " << angFName << std::endl;
      return -1;
    }
    xres = reader.getXStep();
    yres = reader.getYStep();
    zres = getZResolution();
    xpointstemp = reader.getNumEvenCols();
    ypointstemp = reader.getNumRows();
    if(xpointstemp > xpoints) xpoints = xpointstemp;
    if(ypointstemp > ypoints) ypoints = ypointstemp;
  }
  return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AngDataLoader::loadData(ReconstructionVoxel voxels[], int xpoints, int ypoints, int zpoints)
{
  int index = 0;
  int err = -1;

  int readerIndex;
  int xpointstemp;
  int ypointstemp;
  float* euler1Ptr;
  float* euler2Ptr;
  float* euler3Ptr;
  float* confPtr;
  float* imqualPtr;
  float* imqual2Ptr;
  int xstartspot;
  int ystartspot;
  float s;
  float c;
  float s1;
  float c1;
  float s2;
  float c2;
  float q1;
  float q2;
  float q3;
  float q4;

  for (int slice = 0; slice < zpoints; ++slice)
  {
    AngReader reader;

    std::string angFName = m_DirectoryPattern->generateFullPathAngFileName(slice + m_ZStartIndex);
    reader.setFileName(angFName);

    err = reader.readFile();
    if (err < 0)
    {
      std::cout << "AngFileHelper Error: There was an issue loading the data from the .ang file." << std::endl;
      return -1;
    }
    readerIndex = 0;
    xpointstemp = reader.getNumEvenCols();
    ypointstemp = reader.getNumRows();
    euler1Ptr = reader.getPhi1Pointer();
    euler2Ptr = reader.getPhiPointer();
    euler3Ptr = reader.getPhi2Pointer();
    confPtr = reader.getConfidenceIndexPointer();
    imqualPtr = reader.getImageQualityPointer();
    imqual2Ptr = reader.getImageQuality2Pointer();
    xstartspot = (xpoints - xpointstemp) / 2;
    ystartspot = (ypoints - ypointstemp) / 2;
    for (int j = 0; j < ypointstemp; j++)
    {
      for (int i = 0; i < xpointstemp; i++)
      {
        index = ((zpoints - 1 - slice) * xpoints * ypoints) + ((j + ystartspot) * xpoints) + (i + xstartspot);
        voxels[index].euler1 = euler1Ptr[readerIndex]; // Phi1
        voxels[index].euler2 = euler2Ptr[readerIndex]; // Phi
        voxels[index].euler3 = euler3Ptr[readerIndex]; // Phi2
        voxels[index].imagequality = imqualPtr[readerIndex];// Image Quality
        voxels[index].confidence = confPtr[readerIndex];// Confidence
        s = sin(0.5 * euler2Ptr[readerIndex]);
        c = cos(0.5 * euler2Ptr[readerIndex]);
        s1 = sin(0.5 * (euler1Ptr[readerIndex] - euler3Ptr[readerIndex]));
        c1 = cos(0.5 * (euler1Ptr[readerIndex] - euler3Ptr[readerIndex]));
        s2 = sin(0.5 * (euler1Ptr[readerIndex] + euler3Ptr[readerIndex]));
        c2 = cos(0.5 * (euler1Ptr[readerIndex] + euler3Ptr[readerIndex]));
        q1 = s * c1;
        q2 = s * s1;
        q3 = c * s2;
        q4 = c * c2;
        voxels[index].quat[0] = 0;
        voxels[index].quat[1] = q1;
        voxels[index].quat[2] = q2;
        voxels[index].quat[3] = q3;
        voxels[index].quat[4] = q4;
        ++readerIndex;
      }
    }
  }
  return err;
}
