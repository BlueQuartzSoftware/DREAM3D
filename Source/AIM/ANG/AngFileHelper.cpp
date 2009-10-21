/*
 * AngFileHelper.cpp
 *
 *  Created on: Oct 21, 2009
 *      Author: mjackson
 */

#include "AngFileHelper.h"
#include <AIM/ANG/AngFileReader.h>

AngFileHelper::AngFileHelper()
{
  // TODO Auto-generated constructor stub

}

AngFileHelper::~AngFileHelper()
{
  // TODO Auto-generated destructor stub
}

void AngFileHelper::loadData(Voxel voxels[],
                             size_t xpoints, size_t ypoints, size_t zpoints,
                             double resz)
{

  double junk1;
  double junk2;
  double junk3;
  double junk4;
  int zero = 0;
  int checked = 1;
  int badgrain = -1;
  size_t index = 0;
  for (size_t k = 0; k < zpoints; k++)
  {
    if (k == 0 || k == zpoints - 1)
    {
      for (size_t j = 0; j < ypoints; j++)
      {
        for (size_t i = 0; i < xpoints; i++)
        {
          index = ((k * xpoints * ypoints) + (j * xpoints) + i);
          voxels[index].euler1 = -1;
          voxels[index].euler2 = -1;
          voxels[index].euler3 = -1;
          voxels[index].confidence = zero;
          voxels[index].alreadychecked = checked;
          voxels[index].grainname = badgrain;
        }
      }
    }
    if (k > 0 && k < zpoints - 1)
    {
      AngFileReader::Pointer reader = AngFileReader::New();
      int slice = m_ZIndexStart + k - 1;
      std::string angFName = m_DirectoryPattern->generateFullPathAngFileName(slice, 3);
      reader->readFile(angFName);


      for (size_t j = 0; j < ypoints; j++)
      {
        if (j == 0 || j == ypoints - 1)
        {
          for (size_t i = 0; i < xpoints; i++)
          {
            index = ((k * xpoints * ypoints) + (j * xpoints) + i);
            voxels[index].euler1 = -1;
            voxels[index].euler2 = -1;
            voxels[index].euler3 = -1;
            voxels[index].confidence = zero;
            voxels[index].alreadychecked = checked;
            voxels[index].grainname = badgrain;
          }
        }
        if (j > 0 && j < ypoints - 1)
        {
          index = ((k * xpoints * ypoints) + (j * xpoints) + 0);
          voxels[index].euler1 = -1;
          voxels[index].euler2 = -1;
          voxels[index].euler3 = -1;
          voxels[index].confidence = zero;
          voxels[index].alreadychecked = checked;
          voxels[index].grainname = badgrain;
          for (size_t i = 1; i < xpoints - 1; i++)
          {
            inputFile
            >> voxels[((k * xpoints * ypoints) + (j * xpoints) + i)].euler1 // Phi1
            >> voxels[((k * xpoints * ypoints) + (j * xpoints) + i)].euler2 // Phi
                >> voxels[((k * xpoints * ypoints) + (j * xpoints) + i)].euler3 // Phi2
                >> voxels[((k * xpoints * ypoints) + (j * xpoints) + i)].xc  // X
                >> voxels[((k * xpoints * ypoints) + (j * xpoints) + i)].xc  // Y
                >> junk1   // Image Quality
                >> voxels[((k * xpoints * ypoints) + (j * xpoints) + i)].confidence // Confidence
                >> junk2  // Phase Data
                >> junk3
                >> junk4;
            voxels[((k * xpoints * ypoints) + (j * xpoints) + i)].zc = (k - 1) * resz;
            voxels[((k * xpoints * ypoints) + (j * xpoints) + i)].alreadychecked = zero;
            voxels[((k * xpoints * ypoints) + (j * xpoints) + i)].grainname = badgrain;
            if (voxels[((k * xpoints * ypoints) + (j * xpoints) + i)].euler1 == 12.566 && voxels[((k * xpoints * ypoints) + (j * xpoints) + i)].euler2
                == 12.566 && voxels[((k * xpoints * ypoints) + (j * xpoints) + i)].euler3 == 12.566)
            {
              voxels[((k * xpoints * ypoints) + (j * xpoints) + i)].confidence = zero;
              voxels[((k * xpoints * ypoints) + (j * xpoints) + i)].alreadychecked = zero;
              voxels[((k * xpoints * ypoints) + (j * xpoints) + i)].grainname = badgrain;
            }
          }
          voxels[((k * xpoints * ypoints) + (j * xpoints) + xpoints - 1)].euler1 = -1;
          voxels[((k * xpoints * ypoints) + (j * xpoints) + xpoints - 1)].euler2 = -1;
          voxels[((k * xpoints * ypoints) + (j * xpoints) + xpoints - 1)].euler3 = -1;
          voxels[((k * xpoints * ypoints) + (j * xpoints) + xpoints - 1)].confidence = zero;
          voxels[((k * xpoints * ypoints) + (j * xpoints) + xpoints - 1)].alreadychecked = checked;
          voxels[((k * xpoints * ypoints) + (j * xpoints) + xpoints - 1)].grainname = badgrain;
        }
      }

    }
  }
}

