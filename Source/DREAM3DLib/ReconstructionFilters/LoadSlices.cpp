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

#include "LoadSlices.h"
#include <map>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include <cstddef>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <fstream>
#include <list>
#include <algorithm>
#include <numeric>

#include "EbsdLib/H5EbsdVolumeInfo.h"
#include "EbsdLib/H5EbsdVolumeReader.h"
#include "EbsdLib/TSL/AngDirectoryPatterns.h"
#include "EbsdLib/TSL/AngReader.h"
#include "EbsdLib/TSL/AngPhase.h"
#include "EbsdLib/TSL/H5AngVolumeReader.h"
#include "EbsdLib/HKL/H5CtfVolumeReader.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"


#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

const static float m_pi = M_PI;

using namespace std;

#define NEW_SHARED_ARRAY(var, type, size)\
  boost::shared_array<type> var##Array(new type[size]);\
  type* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LoadSlices::LoadSlices()
{
  Seed = MXA::getMilliSeconds();

  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(m_HexOps.get());
  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(m_CubicOps.get());
  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(m_OrthoOps.get());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LoadSlices::~LoadSlices()
{
}

void LoadSlices::execute()
{
  DataContainer* m = getDataContainer();
	int err = 0;
  DREAM3D_RANDOMNG_NEW()

 // updateProgressAndMessage(("Gathering Size and Resolution Information from OIM Data"), 1);
  std::string manufacturer;
  // Get the Size and Resolution of the Volume
  {
    H5EbsdVolumeInfo::Pointer volumeInfoReader = H5EbsdVolumeInfo::New();
    volumeInfoReader->setFileName(m_H5AngFile);
    err = volumeInfoReader->readVolumeInfo();

    volumeInfoReader->getDimsAndResolution(m->xpoints, m->ypoints, m->zpoints, m->resx, m->resy, m->resz);
    //Now Calculate our "subvolume" of slices, ie, those start and end values that the user selected from the GUI
    // The GUI code has already added 1 to the end index so nothing special needs to be done
    // for this calculation

    m->zpoints = m_ZEndIndex - m_ZStartIndex + 1;
    manufacturer = volumeInfoReader->getManufacturer();
    volumeInfoReader = H5EbsdVolumeInfo::NullPointer();
  }
  H5EbsdVolumeReader::Pointer ebsdReader;
  std::vector<float> precipFractions;
  std::vector<Ebsd::CrystalStructure> crystalStructures;
  if (manufacturer.compare(Ebsd::Ang::Manufacturer) == 0)
  {
    ebsdReader = H5AngVolumeReader::New();
    if (NULL == ebsdReader)
    {
      return;
    }
    H5AngVolumeReader* angReader = dynamic_cast<H5AngVolumeReader*>(ebsdReader.get());
    err = loadInfo<H5AngVolumeReader, AngPhase>(angReader, precipFractions, crystalStructures );
  }
  else if (manufacturer.compare(Ebsd::Ctf::Manufacturer) == 0)
  {
    ebsdReader = H5CtfVolumeReader::New();
    if (NULL == ebsdReader)
    {
      return;
    }
    H5CtfVolumeReader* ctfReader = dynamic_cast<H5CtfVolumeReader*>(ebsdReader.get());
    err = loadInfo<H5CtfVolumeReader, CtfPhase>(ctfReader, precipFractions, crystalStructures );
  }
  else
  {
    return;
  }

  initialize(m->xpoints, m->ypoints, m->zpoints,
                m->resx, m->resy, m->resz,
                crystalStructures, m_PhaseTypes, precipFractions);


  // During the loading of the EBSD data the Quality Metric Filters will be run
  // and fill in the ReconstrucionFunc->goodVoxels array.
  ebsdReader->setSliceStart(m_ZStartIndex);
  ebsdReader->setSliceEnd(m_ZEndIndex);
  err = ebsdReader->loadData(m->euler1s, m->euler2s, m->euler3s, m->phases, m->goodVoxels, m->xpoints, m->ypoints, m->zpoints, m_RefFrameZDir, m_QualityMetricFilters);
  float radianconversion = M_PI/180.0;
  if (manufacturer.compare(Ebsd::Ctf::Manufacturer) == 0)
  {
	  for(int i = 0; i < (m->xpoints*m->ypoints*m->zpoints); i++)
	  {
		  m->euler1s[i] = m->euler1s[i] * radianconversion;
		  m->euler2s[i] = m->euler2s[i] * radianconversion;
		  m->euler3s[i] = m->euler3s[i] * radianconversion;
	  }
  }

  initializeQuats();

  threshold_points();

  // If there is an error set this to something negative and also set a message
  notify("LoadSlices Completed", 0, Observable::UpdateProgressMessage);
  setErrorCondition(0);
}
void LoadSlices::initialize(int nX, int nY, int nZ, float xRes, float yRes, float zRes,
                                    std::vector<Ebsd::CrystalStructure> crystalStructures,
									std::vector<DREAM3D::Reconstruction::PhaseType> phaseTypes,
                                    std::vector<float> precipFractions)
{
  notify("Initializing Variables", 0, Observable::UpdateProgressValueAndMessage);
  DataContainer* m = getDataContainer();
  m->crystruct = crystalStructures;
  m->phaseType = m_PhaseTypes;
  m->pptFractions = precipFractions;

  m->xpoints = nX;
  m->ypoints = nY;
  m->zpoints = nZ;
  m->resx = xRes;
  m->resy = yRes;
  m->resz = zRes;

  m->totalpoints = m->xpoints * m->ypoints * m->zpoints;

  int numgrains = 100;
  size_t oldSize = 0;
  m->m_Grains.resize(numgrains + 1);
  for(size_t g = oldSize; g < m->m_Grains.size(); ++g)
  {
    m->m_Grains[g] = Grain::New();
  }

  m->grain_indicies = m->m_GrainIndicies->WritePointer(0, m->totalpoints);
  m->phases = m->m_Phases->WritePointer(0, m->totalpoints);
  m->euler1s = m->m_Euler1s->WritePointer(0, m->totalpoints);
  m->euler2s = m->m_Euler2s->WritePointer(0, m->totalpoints);
  m->euler3s = m->m_Euler3s->WritePointer(0, m->totalpoints);
  m->neighbors = m->m_Neighbors->WritePointer(0, m->totalpoints);
  m->surfacevoxels = m->m_SurfaceVoxels->WritePointer(0, m->totalpoints);
  m->quats = m->m_Quats->WritePointer(0, m->totalpoints*5);
  m->m_Quats->SetNumberOfComponents(5);

  m->alreadychecked = m->m_AlreadyChecked->WritePointer(0, m->totalpoints);

  m->goodVoxels = m->m_GoodVoxels->WritePointer(0, m->totalpoints);

  for(int i=0;i<m->totalpoints;i++)
  {
    m->grain_indicies[i] = -1;
    m->phases[i] = 1;
    m->euler1s[i] = -1;
    m->euler2s[i] = -1;
    m->euler3s[i] = -1;
    m->neighbors[i] = -1;
    m->surfacevoxels[i] = 0;
    m->alreadychecked[i] = false;
    m->goodVoxels[i] = false; // All Voxels are "Bad"
  }

}

void LoadSlices::initializeQuats()
{
  DataContainer* m = getDataContainer();
  float qr[5];
  Ebsd::CrystalStructure xtal = Ebsd::UnknownCrystalStructure;
  int phase = -1;
  for (int i = 0; i < (m->xpoints * m->ypoints * m->zpoints); i++)
  {
    OrientationMath::eulertoQuat(qr, m->euler1s[i], m->euler2s[i], m->euler3s[i]);
    phase = m->phases[i];
    xtal = m->crystruct[phase];
    if (xtal == Ebsd::UnknownCrystalStructure)
    {
      qr[1] = 0.0;
      qr[2] = 0.0;
      qr[3] = 0.0;
      qr[4] = 1.0;
    }
    else
    {
      m_OrientationOps[xtal]->getFZQuat(qr);
    }

    m->quats[i*5 + 0] = 1.0f;
    m->quats[i*5 + 1] = qr[1];
    m->quats[i*5 + 2] = qr[2];
    m->quats[i*5 + 3] = qr[3];
    m->quats[i*5 + 4] = qr[4];
  }
}
void LoadSlices::threshold_points()
{
  DataContainer* m = getDataContainer();
  int neighpoints[6];
  neighpoints[0] = -(m->xpoints * m->ypoints);
  neighpoints[1] = -m->xpoints;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = m->xpoints;
  neighpoints[5] = (m->xpoints * m->ypoints);
  float w, n1, n2, n3;
  float q1[5];
  float q2[5];
//  int index;
  int good = 0;
  size_t count = 0;
  int currentpoint = 0;
  int neighbor = 0;
  int col, row, plane;
//  int noborder = 0;
  Ebsd::CrystalStructure phase1, phase2;
  int initialVoxelsListSize = 10000;
  std::vector<int> voxelslist(initialVoxelsListSize, -1);

  for (size_t iter = 0; iter < (m->totalpoints); iter++)
  {
    m->alreadychecked[iter] = false;
    if(m->goodVoxels[iter] == 0) m->grain_indicies[iter] = 0;
    if(m->goodVoxels[iter] == 1 && m->phases[iter] > 0)
    {
      m->grain_indicies[iter] = -1;
      voxelslist[count] = iter;
      m->alreadychecked[iter] = true;
      count++;
      if(count >= voxelslist.size()) voxelslist.resize(count + initialVoxelsListSize, -1);
    }
  }
  for (size_t j = 0; j < count; j++)
  {
	currentpoint = voxelslist[j];
	col = currentpoint % m->xpoints;
	row = (currentpoint / m->xpoints) % m->ypoints;
	plane = currentpoint / (m->xpoints * m->ypoints);
	q1[0] = 0;
	q1[1] = m->quats[currentpoint*5 + 1];
	q1[2] = m->quats[currentpoint*5 + 2];
	q1[3] = m->quats[currentpoint*5 + 3];
	q1[4] = m->quats[currentpoint*5 + 4];
	phase1 = m->crystruct[m->phases[currentpoint]];
	for (int i = 0; i < 6; i++)
	{
	  good = 1;
	  neighbor = currentpoint + m->neighbors[i];
	  if (i == 0 && plane == 0) good = 0;
	  if (i == 5 && plane == (m->zpoints - 1)) good = 0;
	  if (i == 1 && row == 0) good = 0;
	  if (i == 4 && row == (m->ypoints - 1)) good = 0;
	  if (i == 2 && col == 0) good = 0;
	  if (i == 3 && col == (m->xpoints - 1)) good = 0;
	  if (good == 1 && m->grain_indicies[neighbor] == 0 && m->phases[neighbor] > 0)
	  {
		w = 10000.0;
		q2[0] = 0;
		q2[1] = m->quats[neighbor*5 + 1];
		q2[2] = m->quats[neighbor*5 + 2];
		q2[3] = m->quats[neighbor*5 + 3];
		q2[4] = m->quats[neighbor*5 + 4];
		phase2 = m->crystruct[m->phases[neighbor]];
		if (phase1 == phase2)
		{
		  w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
		}
		if (w < m_misorientationtolerance)
		{
		  m->grain_indicies[neighbor] = -1;
		  m->alreadychecked[neighbor] = true;
		  voxelslist[count] = neighbor;
		  count++;
		  if (count >= voxelslist.size()) voxelslist.resize(count + initialVoxelsListSize, -1);
		}
	  }
	}
  }
  voxelslist.clear();
}

