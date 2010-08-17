///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////

#include "AngFileHelper.h"
#include <AIM/ANG/AngFileReader.h>
#include <math.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngFileHelper::AngFileHelper() :
m_Cancel(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngFileHelper::~AngFileHelper()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngFileHelper::loadData(Voxel voxels[], int xpoints, int ypoints, int zpoints, int slice)
{
  const int zero = 0;
  const int checked = 1;
  const int init = -1;
  int index = 0;
  AngFileReader::Pointer reader = AngFileReader::New();
  std::string angFName = m_DirectoryPattern->generateFullPathAngFileName(slice + m_ZIndexStart);

  int err = reader->readFile(angFName, 0);
  int readerIndex = 0;
  int xpointstemp = reader->getNumEvenCols();
  int ypointstemp = reader->getNumRows();
  float* euler1Ptr = reader->getPhi1Data()->getPointer(0);
  float* euler2Ptr = reader->getPhiData()->getPointer(0);
  float* euler3Ptr = reader->getPhi2Data()->getPointer(0);
  float* xcPtr = reader->getXData()->getPointer(0);
  float* ycPtr = reader->getYData()->getPointer(0);
  float* confPtr = reader->getConfidenceIndexData()->getPointer(0);
  float* imqualPtr = reader->getImageQualityData()->getPointer(0);
  int xstartspot = (xpoints-xpointstemp)/2;
  int ystartspot = (ypoints-ypointstemp)/2;
  for (int j = 0; j < ypointstemp; j++)
  {
      for (int i = 0; i < xpointstemp; i++)
      {
          index = ((zpoints-1-slice)*xpoints*ypoints)+((j+ystartspot)*xpoints)+ (i+xstartspot);
          voxels[index].euler1 = euler1Ptr[readerIndex]; // Phi1
          voxels[index].euler2 = euler2Ptr[readerIndex]; // Phi
          voxels[index].euler3 = euler3Ptr[readerIndex];  // Phi2
          voxels[index].imagequality = imqualPtr[readerIndex];// Image Quality
          voxels[index].confidence = confPtr[readerIndex];// Confidence
          double s=sin(0.5*euler2Ptr[readerIndex]);
          double c=cos(0.5*euler2Ptr[readerIndex]);
          double s1=sin(0.5*(euler1Ptr[readerIndex]-euler3Ptr[readerIndex]));
          double c1=cos(0.5*(euler1Ptr[readerIndex]-euler3Ptr[readerIndex]));
          double s2=sin(0.5*(euler1Ptr[readerIndex]+euler3Ptr[readerIndex]));
          double c2=cos(0.5*(euler1Ptr[readerIndex]+euler3Ptr[readerIndex]));
          double q1 = s*c1;
          double q2 = s*s1;
          double q3 = c*s2;
          double q4 = c*c2;
          voxels[index].quat[0] = 0;
          voxels[index].quat[1] = q1;
          voxels[index].quat[2] = q2;
          voxels[index].quat[3] = q3;
          voxels[index].quat[4] = q4;
          ++readerIndex;
      }
    }
 }
