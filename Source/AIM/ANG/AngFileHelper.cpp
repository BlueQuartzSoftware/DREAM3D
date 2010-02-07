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
void AngFileHelper::loadData(Voxel voxels[],
                             size_t xpoints, size_t ypoints, size_t zpoints,
                             double resz, double minseedconfidence, double minseedimagequality, int cmaxx, int cminx, int cmaxy, int cminy)
{
  const int zero = 0;
  const int checked = 1;
  const int badgrain = 0;
  const int init = -1;
  const double badeuler = 12.566370010375977;
  size_t index = 0;
  int nSlices = (m_ZIndexEnd - m_ZIndexStart);
  for (int k = 0; k < nSlices; k++)
  {
    if (m_Cancel == true) { break; }
    {
      AngFileReader::Pointer reader = AngFileReader::New();
      //int slice = m_ZIndexStart + k;
      std::string angFName = m_DirectoryPattern->generateFullPathAngFileName(k + m_ZIndexStart);
     // std::cout << "Reading ANG File '" << angFName << "'" << std::endl;

      int err = reader->readFile(angFName);
      if (err < 0)
      {
        std::cout << "Error reading file '" << angFName << "'" << std::endl;
        break;
      }
      size_t readerIndex = 0;
      float* euler1Ptr = reader->getPhi1Data()->getPointer(0);
      float* euler2Ptr = reader->getPhiData()->getPointer(0);
      float* euler3Ptr = reader->getPhi2Data()->getPointer(0);
      float* xcPtr = reader->getXData()->getPointer(0);
      float* ycPtr = reader->getYData()->getPointer(0);
      float* confPtr = reader->getConfidenceIndexData()->getPointer(0);
      float* imqualPtr = reader->getImageQualityData()->getPointer(0);
      for (size_t j = 0; j < ypoints; j++)
      {
      //  if (j > 0 && j < ypoints - 1)
        {
          for (size_t i = 0; i < xpoints; i++)
          {
            index = ((k * xpoints * ypoints) + (j * xpoints) + i);
            voxels[index].euler1 = euler1Ptr[readerIndex]; // Phi1
            voxels[index].euler2 = euler2Ptr[readerIndex]; // Phi
            voxels[index].euler3 = euler3Ptr[readerIndex];  // Phi2
           // >> junk1   // Image Quality
            voxels[index].imagequality = imqualPtr[readerIndex];// Confidence
            voxels[index].confidence = confPtr[readerIndex];// Confidence
           // >> junk2  // Phase Data
           // >> junk3
           // >> junk4;
            voxels[index].alreadychecked = zero;
            voxels[index].grainname = init;
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
			voxels[index].quat[1] = q1;
			voxels[index].quat[2] = q2;
			voxels[index].quat[3] = q3;
			voxels[index].quat[4] = q4;
            if ((voxels[index].euler1 == badeuler
                && voxels[index].euler2 == badeuler
				&& voxels[index].euler3 == badeuler) || (voxels[index].imagequality < minseedimagequality && voxels[index].confidence < minseedconfidence))
            {
              voxels[index].confidence = zero;
              voxels[index].alreadychecked = checked;
              voxels[index].grainname = badgrain;
            }
            ++readerIndex;
          }
        }
      }
    }
  }
}

