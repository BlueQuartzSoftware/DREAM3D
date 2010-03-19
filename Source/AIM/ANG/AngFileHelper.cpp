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
void AngFileHelper::loadData(Voxel tempvoxels[],
                             int tempxpoints, int tempypoints, int slice,
                             double resz, double minseedconfidence, double minseedimagequality)
{
  const int zero = 0;
  const int checked = 1;
  const int badgrain = 0;
  const int init = -1;
  const double badeuler = 12.566370010375977;
  int index = 0;
  int newindex = 0;
  int nSlices = (m_ZIndexEnd - m_ZIndexStart);
  AngFileReader::Pointer reader = AngFileReader::New();
  //int slice = m_ZIndexStart + k;
  std::string angFName = m_DirectoryPattern->generateFullPathAngFileName(slice + m_ZIndexStart);
 // std::cout << "Reading ANG File '" << angFName << "'" << std::endl;

  int err = reader->readFile(angFName);
  if (err < 0)
  {
    std::cout << "Error reading file '" << angFName << "'" << std::endl;
  }
  int readerIndex = 0;
  float* euler1Ptr = reader->getPhi1Data()->getPointer(0);
  float* euler2Ptr = reader->getPhiData()->getPointer(0);
  float* euler3Ptr = reader->getPhi2Data()->getPointer(0);
  float* xcPtr = reader->getXData()->getPointer(0);
  float* ycPtr = reader->getYData()->getPointer(0);
  float* confPtr = reader->getConfidenceIndexData()->getPointer(0);
  float* imqualPtr = reader->getImageQualityData()->getPointer(0);
  for (int j = 0; j < tempypoints; j++)
  {
      for (int i = 0; i < tempxpoints; i++)
      {
        index = ((j * tempxpoints) + i);
	    if(slice == 1)
		{
			newindex = index+(tempxpoints*tempypoints);
			tempvoxels[newindex].euler1 = tempvoxels[index].euler1; 
			tempvoxels[newindex].euler2 = tempvoxels[index].euler2; 
			tempvoxels[newindex].euler3 = tempvoxels[index].euler3; 
			tempvoxels[newindex].quat[0] = tempvoxels[index].quat[0];
			tempvoxels[newindex].quat[1] = tempvoxels[index].quat[1];
			tempvoxels[newindex].quat[2] = tempvoxels[index].quat[2];
			tempvoxels[newindex].quat[3] = tempvoxels[index].quat[3];
			tempvoxels[newindex].quat[4] = tempvoxels[index].quat[4];
			tempvoxels[newindex].confidence = tempvoxels[index].confidence; 
			tempvoxels[newindex].imagequality = tempvoxels[index].imagequality; 
			tempvoxels[newindex].alreadychecked = tempvoxels[index].alreadychecked; 
			tempvoxels[newindex].grainname = tempvoxels[index].grainname; 
		}
        tempvoxels[index].euler1 = euler1Ptr[readerIndex]; // Phi1
        tempvoxels[index].euler2 = euler2Ptr[readerIndex]; // Phi
        tempvoxels[index].euler3 = euler3Ptr[readerIndex];  // Phi2
        tempvoxels[index].imagequality = imqualPtr[readerIndex];// Image Quality
        tempvoxels[index].confidence = confPtr[readerIndex];// Confidence
        tempvoxels[index].alreadychecked = zero;
        tempvoxels[index].grainname = badgrain;
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
		tempvoxels[index].quat[1] = q1;
		tempvoxels[index].quat[2] = q2;
		tempvoxels[index].quat[3] = q3;
		tempvoxels[index].quat[4] = q4;
        if ((tempvoxels[index].euler1 == badeuler
            && tempvoxels[index].euler2 == badeuler
			&& tempvoxels[index].euler3 == badeuler) || (tempvoxels[index].imagequality < minseedimagequality && tempvoxels[index].confidence < minseedconfidence))
        {
          tempvoxels[index].confidence = zero;
          tempvoxels[index].alreadychecked = checked;
          tempvoxels[index].grainname = badgrain;
        }
        ++readerIndex;
      }
    }
 }
