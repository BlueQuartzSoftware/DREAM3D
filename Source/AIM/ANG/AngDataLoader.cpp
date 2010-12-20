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

#include "AngDataLoader.h"

#include <cmath>

#include <AIM/ANG/AngReader.h>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngDataLoader::AngDataLoader() :
m_ZResolution(1.0f),
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
                                   double &xres, double &yres, double &zres)
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
int AngDataLoader::loadData(Voxel voxels[], int xpoints, int ypoints, int zpoints)
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
  double s;
  double c;
  double s1;
  double c1;
  double s2;
  double c2;
  double q1;
  double q2;
  double q3;
  double q4;

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
        voxels[index].imagequality2 = imqual2Ptr[readerIndex];// Image Quality
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
