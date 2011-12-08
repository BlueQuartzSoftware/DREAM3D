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

#include "FindBoundingBoxGrains.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBoundingBoxGrains::FindBoundingBoxGrains()
{
    totalsurfacearea = NULL;
    INIT_AIMARRAY(m_TotalSurfaceArea, float);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBoundingBoxGrains::~FindBoundingBoxGrains()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundingBoxGrains::execute()
{
  DataContainer* m = getDataContainer();
  setErrorCondition(0);

  if(m->zpoints > 1) find_boundingboxgrains();
  if(m->zpoints == 1) find_boundingboxgrains2D();

  notify("FindBoundingBoxGrains Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundingBoxGrains::find_boundingboxgrains()
{
  DataContainer* m = getDataContainer();
  size_t size = m->m_Grains.size();
  float boundbox[7];
  float coords[7];
  float x, y, z;
  float dist[7];
  float mindist;
  int sidetomove, move;
  boundbox[1] = 0;
  boundbox[2] = m->xpoints*m->resx;
  boundbox[3] = 0;
  boundbox[4] = m->ypoints*m->resy;
  boundbox[5] = 0;
  boundbox[6] = m->zpoints*m->resz;
  for (size_t i = 1; i < size; i++)
  {
	  if(m->m_Grains[i]->surfacefield == true)
	  {
		  move = 1;
		  mindist = 10000000000.0;
		  x = m->m_Grains[i]->centroidx;
		  y = m->m_Grains[i]->centroidy;
		  z = m->m_Grains[i]->centroidz;
		  coords[1] = x;
		  coords[2] = x;
		  coords[3] = y;
		  coords[4] = y;
		  coords[5] = z;
		  coords[6] = z;
		  for(int j=1;j<7;j++)
		  {
		    dist[j] = 10000000000.0;
			if(j%2 == 1)
			{
				if(coords[j] > boundbox[j]) dist[j] = (coords[j]-boundbox[j]);
				if(coords[j] <= boundbox[j]) move = 0;
			}
			if(j%2 == 0)
			{
				if(coords[j] < boundbox[j]) dist[j] = (boundbox[j]-coords[j]);
				if(coords[j] >= boundbox[j]) move = 0;
			}
			if(dist[j] < mindist) mindist = dist[j], sidetomove = j;
		  }
		  if(move == 1) boundbox[sidetomove] = coords[sidetomove];
	  }
  }
  for (size_t j = 1; j < size; j++)
  {
	if(m->m_Grains[j]->centroidx <= boundbox[1]) m->m_Grains[j]->outsideboundbox = true;
	if(m->m_Grains[j]->centroidx >= boundbox[2]) m->m_Grains[j]->outsideboundbox = true;
	if(m->m_Grains[j]->centroidy <= boundbox[3]) m->m_Grains[j]->outsideboundbox = true;
	if(m->m_Grains[j]->centroidy >= boundbox[4]) m->m_Grains[j]->outsideboundbox = true;
	if(m->m_Grains[j]->centroidz <= boundbox[5]) m->m_Grains[j]->outsideboundbox = true;
	if(m->m_Grains[j]->centroidz >= boundbox[6]) m->m_Grains[j]->outsideboundbox = true;
  }
}
void FindBoundingBoxGrains::find_boundingboxgrains2D()
{
  DataContainer* m = getDataContainer();
  size_t size = m->m_Grains.size();
  float boundbox[5];
  float coords[5];
  float x, y;
  float dist[5];
  float mindist;
  int sidetomove, move;
  boundbox[1] = 0;
  boundbox[2] = m->xpoints*m->resx;
  boundbox[3] = 0;
  boundbox[4] = m->ypoints*m->resy;
  for (size_t i = 1; i < size; i++)
  {
	  if(m->m_Grains[i]->surfacefield == true)
	  {
		  move = 1;
		  mindist = 10000000000.0;
		  x = m->m_Grains[i]->centroidx;
		  y = m->m_Grains[i]->centroidy;
		  coords[1] = x;
		  coords[2] = x;
		  coords[3] = y;
		  coords[4] = y;
		  for(int j=1;j<5;j++)
		  {
		    dist[j] = 10000000000.0;
			if(j%2 == 1)
			{
				if(coords[j] > boundbox[j]) dist[j] = (coords[j]-boundbox[j]);
				if(coords[j] <= boundbox[j]) move = 0;
			}
			if(j%2 == 0)
			{
				if(coords[j] < boundbox[j]) dist[j] = (boundbox[j]-coords[j]);
				if(coords[j] >= boundbox[j]) move = 0;
			}
			if(dist[j] < mindist) mindist = dist[j], sidetomove = j;
		  }
		  if(move == 1) boundbox[sidetomove] = coords[sidetomove];
	  }
  }
  for (size_t j = 1; j < size; j++)
  {
	if(m->m_Grains[j]->centroidx <= boundbox[1]) m->m_Grains[j]->outsideboundbox = true;
	if(m->m_Grains[j]->centroidx >= boundbox[2]) m->m_Grains[j]->outsideboundbox = true;
	if(m->m_Grains[j]->centroidy <= boundbox[3]) m->m_Grains[j]->outsideboundbox = true;
	if(m->m_Grains[j]->centroidy >= boundbox[4]) m->m_Grains[j]->outsideboundbox = true;
  }
}
