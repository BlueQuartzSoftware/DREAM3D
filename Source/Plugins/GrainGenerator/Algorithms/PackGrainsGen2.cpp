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

#include "PackGrainsGen2.h"

#include <fstream>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

const static float m_pi = M_PI;



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PackGrainsGen2::PackGrainsGen2() :
    m_ErrorCondition(0)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PackGrainsGen2::~PackGrainsGen2()
{
}

void PackGrainsGen2::initialize_packinggrid()
{
  m->packingresx = m->resx * 2.0;
  m->packingresy = m->resy * 2.0;
  m->packingresz = m->resz * 2.0;
  m->packingxpoints = int(m->sizex / m->packingresx);
  m->packingypoints = int(m->sizey / m->packingresy);
  m->packingzpoints = int(m->sizez / m->packingresz);
  m->packingtotalpoints = m->packingxpoints * m->packingypoints * m->packingzpoints;
  m->grainowners.resize(m->packingxpoints);
  for (int i = 0; i < m->packingxpoints; i++)
  {
    m->grainowners[i].resize(m->packingypoints);
    for (int j = 0; j < m->packingypoints; j++)
    {
      m->grainowners[i][j].resize(m->packingzpoints, 0);
    }
  }
}

void PackGrainsGen2::generate_grain(int gnum, int phase)
{
  DREAM3D_RANDOMNG_NEW_SEEDED(m->GGseed)

//  int good = 0;
  float r1 = 1;
  float a1 = 0,  a3 = 0;
  float b1 = 0,  b3 = 0;
  float r2 = 0, r3 = 0;
  float diam = 0;
  float vol = 0;
  int volgood = 0;
  float phi1, PHI, phi2;
  while (volgood == 0)
  {
    volgood = 1;
    diam = rg.genrand_norm(m->avgdiam[phase], m->sddiam[phase]);
    diam = exp(diam);
    if(diam >= m->maxdiameter[phase]) volgood = 0;
    if(diam < m->mindiameter[phase]) volgood = 0;
    vol = (4.0 / 3.0) * (m_pi) * ((diam / 2.0) * (diam / 2.0) * (diam / 2.0));
  }
  int diameter = int((diam - m->mindiameter[phase]) / m->binstepsize[phase]);
  a1 = m->bovera[phase][diameter][0];
  b1 = m->bovera[phase][diameter][1];
  if(a1 == 0)
  {
      a1 = m->bovera[phase][diameter - 1][0];
      b1 = m->bovera[phase][diameter - 1][1];
  }
  r2 = rg.genrand_beta(a1, b1);
/*  a2 = m->covera[phase][diameter][0];
  b2 = m->covera[phase][diameter][1];
  if(a2 == 0)
  {
      a2 = m->covera[phase][diameter - 1][0];
      b2 = m->covera[phase][diameter - 1][1];
  }
  r3 = rg.genrand_beta(a2, b2);
  float cob = r3 / r2;
*/  a3 = m->coverb[phase][diameter][0];
  b3 = m->coverb[phase][diameter][1];
  if(a3 == 0)
  {
      a3 = m->coverb[phase][diameter - 1][0];
      b3 = m->coverb[phase][diameter - 1][1];
  }
  r3 = rg.genrand_beta(a3, b3) * r2;
  float random = rg.genrand_res53();
  int bin = 0;
  for (int i = 0; i < (36 * 36 * 36); i++)
  {
    if(random > m->axisodf[phase][i]) bin = i;
    if(random < m->axisodf[phase][i])
    {
      break;
    }
  }
  m->m_OrientatioOps[Ebsd::OrthoRhombic]->determineEulerAngles(bin, phi1, PHI, phi2);
  float mf = m->omega3[phase][diameter][0];
  float s = m->omega3[phase][diameter][1];
  float omega3f = rg.genrand_beta(mf, s);
  DREAM3D::SyntheticBuilder::ShapeType shapeclass = m->shapeTypes[phase];
  if(shapeclass == DREAM3D::SyntheticBuilder::EllipsoidShape) omega3f = 1;
  m->m_Grains[gnum]->volume = vol;
  m->m_Grains[gnum]->equivdiameter = diam;
  m->m_Grains[gnum]->radius1 = r1;
  m->m_Grains[gnum]->radius2 = r2;
  m->m_Grains[gnum]->radius3 = r3;
  m->m_Grains[gnum]->axiseuler1 = phi1;
  m->m_Grains[gnum]->axiseuler2 = PHI;
  m->m_Grains[gnum]->axiseuler3 = phi2;
  m->m_Grains[gnum]->omega3 = omega3f;
  m->m_Grains[gnum]->phase = phase;
  m->m_Grains[gnum]->neighbordistfunc[0] = 0;
  m->m_Grains[gnum]->neighbordistfunc[1] = 0;
  m->m_Grains[gnum]->neighbordistfunc[2] = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackGrainsGen2::execute()
{

  DREAM3D_RANDOMNG_NEW()

  float change1, change2, change;
  int phase;
  size_t randomgrain;
  float random;
//  int newgrain;
 // float check;
  float xc, yc, zc;
  float oldxc, oldyc, oldzc;
  m->oldfillingerror = 0;
  m->currentneighborhooderror = 0, m->oldneighborhooderror = 0;
  m->currentsizedisterror = 0, m->oldsizedisterror = 0;
  int acceptedmoves = 0;
  double totalprimaryfractions = 0.0;
  // find which phases are primary phases
  for (size_t i = 1; i < m->phaseType.size(); ++i)
  {
    if(m->phaseType[i] == DREAM3D::Reconstruction::PrimaryPhase)
    {
      m->primaryphases.push_back(i);
      m->primaryphasefractions.push_back(m->phasefraction[i]);
      totalprimaryfractions = totalprimaryfractions + m->phasefraction[i];
    }
  }
  // scale the primary phase fractions to total to 1
  for (size_t i = 0; i < m->primaryphasefractions.size(); i++)
  {
    m->primaryphasefractions[i] = m->primaryphasefractions[i] / totalprimaryfractions;
    if(i > 0) m->primaryphasefractions[i] = m->primaryphasefractions[i] + m->primaryphasefractions[i - 1];
  }
  // this initializes the arrays to hold the details of the locations of all of the grains during packing
  initialize_packinggrid();
  // initialize the sim and goal size distributions for the primary phases
  m->grainsizedist.resize(m->primaryphases.size());
  m->simgrainsizedist.resize(m->primaryphases.size());
  m->grainsizediststep.resize(m->primaryphases.size());
  for (size_t i = 0; i < m->primaryphases.size(); i++)
  {
    phase = m->primaryphases[i];
    m->grainsizedist[i].resize(40);
    m->simgrainsizedist[i].resize(40);
    m->grainsizediststep[i] = ((2 * m->maxdiameter[phase]) - (m->mindiameter[phase] / 2.0)) / m->grainsizedist[i].size();
    float input = 0;
    float previoustotal = 0;
    for (size_t j = 0; j < m->grainsizedist[i].size(); j++)
    {
      input = (float(j + 1) * m->grainsizediststep[i]) + (m->mindiameter[phase] / 2.0);
      if(logf(input) <= m->avgdiam[phase]) m->grainsizedist[i][j] = 0.5
          - 0.5 * (DREAM3DMath::erf((m->avgdiam[phase] - logf(float(input))) / (sqrtf(2 * m->sddiam[phase] * m->sddiam[phase])))) - previoustotal;
      if(logf(input) > m->avgdiam[phase]) m->grainsizedist[i][j] = 0.5
          + 0.5 * (DREAM3DMath::erf((logf(float(input)) - m->avgdiam[phase]) / (sqrtf(2 * m->sddiam[phase] * m->sddiam[phase])))) - previoustotal;
      previoustotal = previoustotal + m->grainsizedist[i][j];
    }
  }
  // generate the grains and monitor the size distribution error while doing so. After grains are generated, no new grains can enter or leave the structure.
  int gid = 1;
  float currentvol = 0.0;
  m->m_Grains.resize(2);
  m->m_Grains[1] = Grain::New();
  float factor = 1.0;
  float iter = 0;
  while (currentvol < (factor * m->totalvol))
  {
    iter++;
    m->GGseed++;
    random = rg.genrand_res53();
    for (size_t j = 0; j < m->primaryphases.size(); ++j)
    {
      if(random < m->primaryphasefractions[j])
      {
        phase = m->primaryphases[j];
        break;
      }
    }
    generate_grain(gid, phase);
	m->currentsizedisterror = check_sizedisterror(gid, -1000);
    change = (m->currentsizedisterror) - (m->oldsizedisterror);
	if(change > 0 || m->currentsizedisterror > (1.0-(iter*0.001)))
    {
       m->m_Grains[gid]->active = 1;
       m->oldsizedisterror = m->currentsizedisterror;
       currentvol = currentvol + m->m_Grains[gid]->volume;
       gid++;
       m->m_Grains.resize(gid + 1);
       m->m_Grains[gid] = Grain::New();
	   iter = 0;
    }
  }
  if(m->periodic_boundaries == false)
  {
	  iter = 0;
	  int xgrains, ygrains, zgrains;
	  xgrains = powf((m->m_Grains.size()*(m->sizex/m->sizey)*(m->sizex/m->sizez)),(1.0/3.0));
	  ygrains = xgrains*(m->sizey/m->sizex);
	  zgrains = xgrains*(m->sizez/m->sizex);
	  factor = 0.25 * (1.0 - (float((xgrains-2)*(ygrains-2)*(zgrains-2))/float(xgrains*ygrains*zgrains)));
	  while (currentvol < ((1+factor) * m->totalvol))
	  {
	    iter++;
		m->GGseed++;
		random = rg.genrand_res53();
		for (size_t j = 0; j < m->primaryphases.size(); ++j)
		{
		  if(random < m->primaryphasefractions[j])
		  {
			phase = m->primaryphases[j];
			break;
		  }
		}
		generate_grain(gid, phase);
		m->currentsizedisterror = check_sizedisterror(gid, -1000);
		change = (m->currentsizedisterror) - (m->oldsizedisterror);
		if(change > 0 || m->currentsizedisterror > (1.0-(iter*0.001)))
		{
		   m->m_Grains[gid]->active = 1;
		   m->oldsizedisterror = m->currentsizedisterror;
		   currentvol = currentvol + m->m_Grains[gid]->volume;
		   gid++;
		   m->m_Grains.resize(gid + 1);
		   m->m_Grains[gid] = Grain::New();
		   iter = 0;
		}
	  }
  }
  // initialize the sim and goal neighbor distribution for the primary phases
  m->neighbordist.resize(m->primaryphases.size());
  m->simneighbordist.resize(m->primaryphases.size());
  for (size_t i = 0; i < m->primaryphases.size(); i++)
  {
    phase = m->primaryphases[i];
    m->neighbordist[i].resize(m->numdiameterbins[phase]);
    m->simneighbordist[i].resize(m->numdiameterbins[phase]);
    for (size_t j = 0; j < m->neighbordist[i].size(); j++)
    {
      m->neighbordist[i][j].resize(3);
      m->simneighbordist[i][j].resize(3);
      m->neighbordist[i][j][0] = m->neighborparams[phase][j][0] * powf(0.5, m->neighborparams[phase][j][2]) + m->neighborparams[phase][j][1];
      m->neighbordist[i][j][1] = m->neighborparams[phase][j][0] * powf(1.5, m->neighborparams[phase][j][2]) + m->neighborparams[phase][j][1];
      m->neighbordist[i][j][2] = m->neighborparams[phase][j][0] * powf(2.5, m->neighborparams[phase][j][2]) + m->neighborparams[phase][j][1];
    }
  }
  //  for each grain : select centroid, determine voxels in grain, monitor filling error and decide of the 10 placements which
  // is the most beneficial, then the grain is added and its neighbors are determined
  m->fillingerror = 1;
  for (size_t i = 1; i < m->m_Grains.size(); i++)
  {
    xc = m->sizex / 2.0;
    yc = m->sizey / 2.0;
    zc = m->sizez / 2.0;
    m->m_Grains[i]->centroidx = xc;
    m->m_Grains[i]->centroidy = yc;
    m->m_Grains[i]->centroidz = zc;
    insert_grain(i);
    m->fillingerror = check_fillingerror(i,-1000);
    for (int iter = 0; iter < 10; iter++)
    {
      xc = rg.genrand_res53() * (m->xpoints * m->resx);
      yc = rg.genrand_res53() * (m->ypoints * m->resy);
      zc = rg.genrand_res53() * (m->zpoints * m->resz);
	  oldxc = m->m_Grains[i]->centroidx;
	  oldyc = m->m_Grains[i]->centroidy;
	  oldzc = m->m_Grains[i]->centroidz;
	  m->oldfillingerror = m->fillingerror;
	  m->fillingerror = check_fillingerror(-1000,i);
      move_grain(i, xc, yc, zc);
      m->fillingerror = check_fillingerror(i, -1000);
	  if(m->fillingerror > m->oldfillingerror)
	  {
		m->fillingerror = check_fillingerror(-1000,i);
		move_grain(i, oldxc, oldyc, oldzc);
		m->fillingerror = check_fillingerror(i,-1000);
	  }
    }
  }

  // determine initial filling and neighbor distribution errors
  m->oldneighborhooderror = check_neighborhooderror(-1000, -1000);
#if ERROR_TXT_OUT
  std::ofstream outFile;
  std::string filename = "error.txt";
  outFile.open(filename.c_str(), std::ios_base::binary);
#endif
  // begin swaping/moving/adding/removing grains to try to improve packing
  int numgrains = m->m_Grains.size();
  for (int iteration = 0; iteration < (100*numgrains); iteration++)
  {
    change1 = 0;
    change2 = 0;
    int option = iteration % 2;
#if ERROR_TXT_OUT
    if(iteration%25 == 0)
    {
      outFile << iteration << "	" << m->fillingerror << "	" << m->oldsizedisterror << "	" << m->oldneighborhooderror << "	" << numgrains << "	" << acceptedmoves << std::endl;
    }
#endif
    // JUMP - this option moves one grain to a random spot in the volume
    if(option == 0)
    {
      randomgrain = int(rg.genrand_res53() * numgrains);
      if(randomgrain == 0) randomgrain = 1;
      if(randomgrain == numgrains) randomgrain = numgrains - 1;
      m->GGseed++;
      xc = rg.genrand_res53() * (m->xpoints * m->resx);
      yc = rg.genrand_res53() * (m->ypoints * m->resy);
      zc = rg.genrand_res53() * (m->zpoints * m->resz);
	  oldxc = m->m_Grains[randomgrain]->centroidx;
	  oldyc = m->m_Grains[randomgrain]->centroidy;
	  oldzc = m->m_Grains[randomgrain]->centroidz;
	  m->oldfillingerror = m->fillingerror;
	  m->fillingerror = check_fillingerror(-1000,randomgrain);
      move_grain(randomgrain, xc, yc, zc);
	  m->fillingerror = check_fillingerror(randomgrain,-1000);
//      m->currentneighborhooderror = check_neighborhooderror(-1000, random);
//      change2 = (m->currentneighborhooderror * m->currentneighborhooderror) - (m->oldneighborhooderror * m->oldneighborhooderror);
	  if(m->fillingerror <= m->oldfillingerror)
      {
//        m->oldneighborhooderror = m->currentneighborhooderror;
        acceptedmoves++;
      }
	  else if(m->fillingerror > m->oldfillingerror)
	  {
		m->fillingerror = check_fillingerror(-1000,randomgrain);
		move_grain(randomgrain, oldxc, oldyc, oldzc);
		m->fillingerror = check_fillingerror(randomgrain,-1000);
	  }
    }
    // NUDGE - this option moves one grain to a spot close to its current centroid
    if(option == 1)
    {
      randomgrain = int(rg.genrand_res53() * numgrains);
      if(randomgrain == 0) randomgrain = 1;
      if(randomgrain == numgrains) randomgrain = numgrains - 1;
      m->GGseed++;
	  oldxc = m->m_Grains[randomgrain]->centroidx;
	  oldyc = m->m_Grains[randomgrain]->centroidy;
	  oldzc = m->m_Grains[randomgrain]->centroidz;
	  xc = oldxc + ((2.0 * (rg.genrand_res53() - 0.5)) * (2.0 * m->packingresx));
      yc = oldyc + ((2.0 * (rg.genrand_res53() - 0.5)) * (2.0 * m->packingresy));
      zc = oldzc + ((2.0 * (rg.genrand_res53() - 0.5)) * (2.0 * m->packingresz));
	  m->oldfillingerror = m->fillingerror;
	  m->fillingerror = check_fillingerror(-1000,randomgrain);
      move_grain(randomgrain, xc, yc, zc);
	  m->fillingerror = check_fillingerror(randomgrain,-1000);
//      m->currentneighborhooderror = check_neighborhooderror(-1000, random);
//      change2 = (m->currentneighborhooderror * m->currentneighborhooderror) - (m->oldneighborhooderror * m->oldneighborhooderror);
	  if(m->fillingerror <= m->oldfillingerror)
      {
//        m->oldneighborhooderror = m->currentneighborhooderror;
        acceptedmoves++;
      }
	  else if(m->fillingerror > m->oldfillingerror)
	  {
		m->fillingerror = check_fillingerror(-1000,randomgrain);
		move_grain(randomgrain, oldxc, oldyc, oldzc);
		m->fillingerror = check_fillingerror(randomgrain,-1000);
	  }
    }
  }
#if ERROR_TXT_OUT
  outFile.close();
#endif

#if ERROR_TXT_OUT1

//  ofstream outFile;
  filename = "test.vtk";
  outFile.open(filename.c_str(), std::ios_base::binary);
  outFile << "# vtk DataFile Version 2.0" << std::endl;
  outFile << "data set from FFT2dx_GB" << std::endl;
  outFile << "ASCII" << std::endl;
  outFile << "DATASET STRUCTURED_POINTS" << std::endl;
  outFile << "DIMENSIONS " << m->packingxpoints << " " << m->packingypoints << " " << m->packingzpoints << std::endl;
  outFile << "ORIGIN 0.0 0.0 0.0" << std::endl;
  outFile << "SPACING " << m->packingresx << " " << m->packingresy << " " << m->packingresz << std::endl;
  outFile << "POINT_DATA " << m->packingxpoints*m->packingypoints*m->packingzpoints << std::endl;
  outFile << std::endl;
  outFile << std::endl;
  outFile << "SCALARS GrainID int  1" << std::endl;
  outFile << "LOOKUP_TABLE default" << std::endl;
  for (int i = 0; i < (m->packingzpoints); i++)
  {
    for (int j = 0; j < (m->packingypoints); j++)
    {
      for (int k = 0; k < (m->packingxpoints); k++)
      {
        int name = m->grainowners[k][j][i];
        if(i%20 == 0 && i > 0) outFile << std::endl;
        outFile << "     ";
        if(name < 100) outFile << " ";
        if(name < 10) outFile << " ";
        outFile << name;
      }
    }
  }
  outFile.close();
#endif

  assign_voxels();

  // If there is an error set this to something negative and also set a message
  m_ErrorMessage = "PackGrainsGen2 Completed";
  m_ErrorCondition = 0;
}

void PackGrainsGen2::move_grain(size_t gnum, float xc, float yc, float zc)
{
 // int column, row, plane;
  int occolumn, ocrow, ocplane;
  int nccolumn, ncrow, ncplane;
  int shiftcolumn, shiftrow, shiftplane;
  float oxc = m->m_Grains[gnum]->centroidx;
  float oyc = m->m_Grains[gnum]->centroidy;
  float ozc = m->m_Grains[gnum]->centroidz;
  occolumn = (oxc - (m->packingresx / 2)) / m->packingresx;
  ocrow = (oyc - (m->packingresy / 2)) / m->packingresy;
  ocplane = (ozc - (m->packingresz / 2)) / m->packingresz;
  nccolumn = (xc - (m->packingresx / 2)) / m->packingresx;
  ncrow = (yc - (m->packingresy / 2)) / m->packingresy;
  ncplane = (zc - (m->packingresz / 2)) / m->packingresz;
  shiftcolumn = nccolumn - occolumn;
  shiftrow = ncrow - ocrow;
  shiftplane = ncplane - ocplane;
  m->m_Grains[gnum]->centroidx = xc;
  m->m_Grains[gnum]->centroidy = yc;
  m->m_Grains[gnum]->centroidz = zc;
  size_t size = m->m_Grains[gnum]->columnlist->size();
  std::vector<int>& columnlist = *(m->m_Grains[gnum]->columnlist);
  std::vector<int>& rowlist = *(m->m_Grains[gnum]->rowlist);
  std::vector<int>& planelist = *(m->m_Grains[gnum]->planelist);

  for (size_t i = 0; i < size; i++)
  {
    columnlist[i] = columnlist[i] + shiftcolumn;
    rowlist[i] = rowlist[i] + shiftrow;
    planelist[i] = planelist[i] + shiftplane;
  }
}
void PackGrainsGen2::determine_neighbors(size_t gnum, int add)
{
  float x, y, z;
  float xn, yn, zn;
  float dia, dia2;
  int DoverR;
  float xdist, ydist, zdist, totdist;
  int nnum = 0;
  nnum = 0;
  x = m->m_Grains[gnum]->centroidx;
  y = m->m_Grains[gnum]->centroidy;
  z = m->m_Grains[gnum]->centroidz;
  dia = m->m_Grains[gnum]->equivdiameter;
  for (size_t n = 1; n < m->m_Grains.size(); n++)
  {
    xn = m->m_Grains[n]->centroidx;
    yn = m->m_Grains[n]->centroidy;
    zn = m->m_Grains[n]->centroidz;
    dia2 = m->m_Grains[n]->equivdiameter;
    xdist = fabs(x - xn);
    ydist = fabs(y - yn);
    zdist = fabs(z - zn);
    totdist = (xdist * xdist) + (ydist * ydist) + (zdist * zdist);
    totdist = sqrt(totdist);
    if(totdist < (3 * (dia / 2.0)))
    {
      DoverR = int(totdist / (dia / 2.0));
      for (int iter = DoverR; iter < 3; iter++)
      {
        if(add > 0) m->m_Grains[n]->neighbordistfunc[iter]++;
        if(add < 0) m->m_Grains[n]->neighbordistfunc[iter] = m->m_Grains[n]->neighbordistfunc[iter] - 1;
      }
    }
    if(totdist < (3 * (dia2 / 2.0)))
    {
      DoverR = int(totdist / (dia2 / 2.0));
      for (int iter = DoverR; iter < 3; iter++)
      {
        if(add > 0) m->m_Grains[gnum]->neighbordistfunc[iter]++;
        if(add < 0) m->m_Grains[gnum]->neighbordistfunc[iter] = m->m_Grains[gnum]->neighbordistfunc[iter] - 1;
      }
    }
  }
}

float PackGrainsGen2::check_neighborhooderror(int gadd, int gremove)
{
  float neighborerror;
  float bhattdist;
  float dia;
  int nnum;
  int index;
  std::vector<int> count;
  int phase;
  for (size_t iter = 0; iter < m->neighbordist.size(); ++iter)
  {
    phase = m->primaryphases[iter];
    count.resize(m->simneighbordist[iter].size(), 0);
    for (size_t i = 0; i < m->simneighbordist[iter].size(); i++)
    {
      m->simneighbordist[iter][i][0] = 0;
      m->simneighbordist[iter][i][1] = 0;
      m->simneighbordist[iter][i][2] = 0;
    }
    if(gadd > 0 && m->m_Grains[gadd]->phase == phase)
    {
      determine_neighbors(gadd, 1);
    }
    if(gremove > 0 && m->m_Grains[gremove]->phase == phase)
    {
      determine_neighbors(gremove, -1);
    }
    for (size_t i = 1; i < m->m_Grains.size(); i++)
    {
      nnum = 0;
      index = i;
      if(index != gremove && m->m_Grains[index]->phase == phase)
      {
        dia = m->m_Grains[index]->equivdiameter;
        if(dia > m->maxdiameter[phase]) dia = m->maxdiameter[phase];
        if(dia < m->mindiameter[phase]) dia = m->mindiameter[phase];
        dia = int((dia - m->mindiameter[phase]) / m->binstepsize[phase]);
        for (int j = 0; j < 3; j++)
        {
          nnum = m->m_Grains[index]->neighbordistfunc[j];
          if(nnum > 0)
          {
            m->simneighbordist[iter][dia][j] = m->simneighbordist[iter][dia][j] + nnum;
          }
        }
        count[dia]++;
      }
    }
    if(gadd > 0 && m->m_Grains[gadd]->phase == phase)
    {
      dia = m->m_Grains[index]->equivdiameter;
      if(dia > m->maxdiameter[phase]) dia = m->maxdiameter[phase];
      if(dia < m->mindiameter[phase]) dia = m->mindiameter[phase];
      dia = int((dia - m->mindiameter[phase]) / m->binstepsize[phase]);
      for (int j = 0; j < 3; j++)
      {
        nnum = m->m_Grains[index]->neighbordistfunc[j];
        if(nnum > 0)
        {
          m->simneighbordist[iter][dia][j] = m->simneighbordist[iter][dia][j] + nnum;
        }
      }
      count[dia]++;
    }
    for (size_t i = 0; i < m->simneighbordist[iter].size(); i++)
    {
      for (size_t j = 0; j < 3; j++)
      {
        m->simneighbordist[iter][i][j] = m->simneighbordist[iter][i][j] / double(count[i]);
        if(count[i] == 0) m->simneighbordist[iter][i][j] = 0.0;
      }
    }
    if(gadd > 0 && m->m_Grains[gadd]->phase == phase)
    {
      determine_neighbors(gadd, -1);
    }
    if(gremove > 0 && m->m_Grains[gremove]->phase == phase)
    {
      determine_neighbors(gremove, 1);
    }
  }
  compare_3Ddistributions(m->simneighbordist, m->neighbordist, bhattdist);
  neighborerror = bhattdist;
  return neighborerror;
}

void PackGrainsGen2::compare_1Ddistributions(std::vector<float> array1, std::vector<float> array2, float &bhattdist)
{
  bhattdist = 0;
  for (size_t i = 0; i < array1.size(); i++)
  {
    bhattdist = bhattdist + sqrt((array1[i]*array2[i]));
  }
}
void PackGrainsGen2::compare_2Ddistributions(std::vector<std::vector<float> > array1, std::vector<std::vector<float> > array2, float &bhattdist)
{
  bhattdist = 0;
  for (size_t i = 0; i < array1.size(); i++)
  {
    for (size_t j = 0; j < array1[i].size(); j++)
    {
      bhattdist = bhattdist + sqrt((array1[i][j] * array2[i][j]));
    }
  }
}

void PackGrainsGen2::compare_3Ddistributions(std::vector<std::vector<std::vector<float> > > array1, std::vector<std::vector<std::vector<float> > > array2, float &bhattdist)
{
  bhattdist = 0;
  for (size_t i = 0; i < array1.size(); i++)
  {
    for (size_t j = 0; j < array1[i].size(); j++)
    {
      for (size_t k = 0; k < array1[i][j].size(); k++)
      {
        bhattdist = bhattdist + sqrt((array1[i][j][k]*array2[i][j][k]));
      }
    }
  }
}

float PackGrainsGen2::check_sizedisterror(int gadd, int gremove)
{
  float dia;
  float sizedisterror = 0;
  float bhattdist;
  int index;
  int count = 0;
  int phase;
  for (size_t iter = 0; iter < m->grainsizedist.size(); ++iter)
  {
    phase = m->primaryphases[iter];
    count = 0;
    for (size_t i = 0; i < m->grainsizedist[iter].size(); i++)
    {
      m->simgrainsizedist[iter][i] = 0.0;
    }
    for (size_t b = 1; b < m->m_Grains.size(); b++)
    {
      index = b;
      if(index != gremove && m->m_Grains[index]->phase == phase)
      {
        dia = m->m_Grains[index]->equivdiameter;
        dia = (dia - (m->mindiameter[phase] / 2.0)) / m->grainsizediststep[iter];
        if(dia < 0) dia = 0;
        if(dia > m->grainsizedist[iter].size() - 1) dia = m->grainsizedist[iter].size() - 1;
        m->simgrainsizedist[iter][int(dia)]++;
        count++;
      }
    }
    if(gadd > 0 && m->m_Grains[gadd]->phase == phase)
    {
      dia = m->m_Grains[gadd]->equivdiameter;
      dia = (dia - (m->mindiameter[phase] / 2.0)) / m->grainsizediststep[iter];
      if(dia < 0) dia = 0;
      if(dia > m->grainsizedist[iter].size() - 1) dia = m->grainsizedist[iter].size() - 1;
      m->simgrainsizedist[iter][int(dia)]++;
      count++;
    }
    for (size_t i = 0; i < m->grainsizedist[iter].size(); i++)
    {
      m->simgrainsizedist[iter][i] = m->simgrainsizedist[iter][i] / float(count);
      if(count == 0) m->simgrainsizedist[iter][i] = 0.0;
    }
  }
  compare_2Ddistributions(m->simgrainsizedist, m->grainsizedist, bhattdist);
  sizedisterror = bhattdist;
  return sizedisterror;
}

float PackGrainsGen2::check_fillingerror(int gadd, int gremove)
{
  float fillingerror = m->fillingerror * float(m->packingtotalpoints);
  int col, row, plane;
  if(gadd > 0)
  {
    size_t size = m->m_Grains[gadd]->columnlist->size();
    std::vector<int>& columnlist = *(m->m_Grains[gadd]->columnlist);
    std::vector<int>& rowlist = *(m->m_Grains[gadd]->rowlist);
    std::vector<int>& planelist = *(m->m_Grains[gadd]->planelist);

	float packquality = 0;
    for (size_t i = 0; i < size; i++)
    {
      col = columnlist[i];
      row = rowlist[i];
      plane = planelist[i];
      if(m->periodic_boundaries == true)
      {
        if(col < 0) col = col + m->packingxpoints;
        if(col > m->packingxpoints - 1) col = col - m->packingxpoints;
        if(row < 0) row = row + m->packingypoints;
        if(row > m->packingypoints - 1) row = row - m->packingypoints;
        if(plane < 0) plane = plane + m->packingzpoints;
        if(plane > m->packingzpoints - 1) plane = plane - m->packingzpoints;
        fillingerror = fillingerror + (2 * m->grainowners[col][row][plane] - 1);
		m->grainowners[col][row][plane]++;
		packquality = packquality + ((m->grainowners[col][row][plane]-1)*(m->grainowners[col][row][plane]-1));
      }
      if(m->periodic_boundaries == false)
      {
        if(col >= 0 && col <= m->packingxpoints - 1 && row >= 0 && row <= m->packingypoints - 1 && plane >= 0 && plane <= m->packingzpoints - 1)
        {
          fillingerror = fillingerror + (2 * m->grainowners[col][row][plane] - 1);
		  m->grainowners[col][row][plane]++;
		  packquality = packquality + ((m->grainowners[col][row][plane]-1)*(m->grainowners[col][row][plane]-1));
        }
      }
    }
	m->m_Grains[gadd]->packquality = packquality/float(size);
  }
  if(gremove > 0)
  {
    size_t size = m->m_Grains[gremove]->columnlist->size();
    std::vector<int>& columnlist = *(m->m_Grains[gremove]->columnlist);
    std::vector<int>& rowlist = *(m->m_Grains[gremove]->rowlist);
    std::vector<int>& planelist = *(m->m_Grains[gremove]->planelist);
    for (size_t i = 0; i < size; i++)
    {
      col = columnlist[i];
      row = rowlist[i];
      plane = planelist[i];
      if(m->periodic_boundaries == true)
      {
        if(col < 0) col = col + m->packingxpoints;
        if(col > m->packingxpoints - 1) col = col - m->packingxpoints;
        if(row < 0) row = row + m->packingypoints;
        if(row > m->packingypoints - 1) row = row - m->packingypoints;
        if(plane < 0) plane = plane + m->packingzpoints;
        if(plane > m->packingzpoints - 1) plane = plane - m->packingzpoints;
        fillingerror = fillingerror + (-2 * m->grainowners[col][row][plane] + 3);
		m->grainowners[col][row][plane] = m->grainowners[col][row][plane] - 1;
      }
      if(m->periodic_boundaries == false)
      {
        if(col >= 0 && col <= m->packingxpoints - 1 && row >= 0 && row <= m->packingypoints - 1 && plane >= 0 && plane <= m->packingzpoints - 1)
        {
          fillingerror = fillingerror + (-2 * m->grainowners[col][row][plane] + 3);
		  m->grainowners[col][row][plane] = m->grainowners[col][row][plane] - 1;
        }
      }
    }
  }
  fillingerror = fillingerror / float(m->packingtotalpoints);
  return fillingerror;
}

void PackGrainsGen2::insert_grain(size_t gnum)
{
  DREAM3D_RANDOMNG_NEW()

//  float dist;
  float inside = -1;
  int column, row, plane;
  int centercolumn, centerrow, centerplane;
  int xmin, xmax, ymin, ymax, zmin, zmax;
  float xc, yc, zc;
  float xp, yp, zp;
  float x, y, z;
  float volcur = m->m_Grains[gnum]->volume;
  float bovera = m->m_Grains[gnum]->radius2;
  float covera = m->m_Grains[gnum]->radius3;
  float omega3 = m->m_Grains[gnum]->omega3;
  float radcur1 = 1;
  if(NULL == m->m_Grains[gnum]->columnlist)
  {
    m->m_Grains[gnum]->columnlist = new std::vector<int>(0);
  }
  if(NULL == m->m_Grains[gnum]->rowlist)
  {
    m->m_Grains[gnum]->rowlist = new std::vector<int>(0);
  }
  if(NULL == m->m_Grains[gnum]->planelist)
  {
    m->m_Grains[gnum]->planelist = new std::vector<int>(0);
  }

  DREAM3D::SyntheticBuilder::ShapeType shapeclass = m->shapeTypes[m->m_Grains[gnum]->phase];

  // init any values for each of the Shape Ops
  for (std::map<DREAM3D::SyntheticBuilder::ShapeType, DREAM3D::ShapeOps*>::iterator ops = m->m_ShapeOps.begin(); ops != m->m_ShapeOps.end(); ++ops)
  {
    (*ops).second->init();
  }
  // Create our Argument Map
  std::map<DREAM3D::ShapeOps::ArgName, float> shapeArgMap;
  shapeArgMap[DREAM3D::ShapeOps::Omega3] = omega3;
  shapeArgMap[DREAM3D::ShapeOps::VolCur] = volcur;
  shapeArgMap[DREAM3D::ShapeOps::B_OverA] = bovera;
  shapeArgMap[DREAM3D::ShapeOps::C_OverA] = covera;

  radcur1 = m->m_ShapeOps[shapeclass]->radcur1(shapeArgMap);

  float radcur2 = (radcur1 * bovera);
  float radcur3 = (radcur1 * covera);
  float phi1 = m->m_Grains[gnum]->axiseuler1;
  float PHI = m->m_Grains[gnum]->axiseuler2;
  float phi2 = m->m_Grains[gnum]->axiseuler3;
  float ga[3][3];
  ga[0][0] = cosf(phi1) * cosf(phi2) - sinf(phi1) * sinf(phi2) * cosf(PHI);
  ga[0][1] = sinf(phi1) * cosf(phi2) + cosf(phi1) * sinf(phi2) * cosf(PHI);
  ga[0][2] = sinf(phi2) * sinf(PHI);
  ga[1][0] = -cosf(phi1) * sinf(phi2) - sinf(phi1) * cosf(phi2) * cosf(PHI);
  ga[1][1] = -sinf(phi1) * sinf(phi2) + cosf(phi1) * cosf(phi2) * cosf(PHI);
  ga[1][2] = cosf(phi2) * sinf(PHI);
  ga[2][0] = sinf(phi1) * sinf(PHI);
  ga[2][1] = -cosf(phi1) * sinf(PHI);
  ga[2][2] = cosf(PHI);
  xc = m->m_Grains[gnum]->centroidx;
  yc = m->m_Grains[gnum]->centroidy;
  zc = m->m_Grains[gnum]->centroidz;
  centercolumn = (xc - (m->packingresx / 2)) / m->packingresx;
  centerrow = (yc - (m->packingresy / 2)) / m->packingresy;
  centerplane = (zc - (m->packingresz / 2)) / m->packingresz;
  xmin = int(centercolumn - ((radcur1 / m->packingresx) + 1));
  xmax = int(centercolumn + ((radcur1 / m->packingresx) + 1));
  ymin = int(centerrow - ((radcur1 / m->packingresy) + 1));
  ymax = int(centerrow + ((radcur1 / m->packingresy) + 1));
  zmin = int(centerplane - ((radcur1 / m->packingresz) + 1));
  zmax = int(centerplane + ((radcur1 / m->packingresz) + 1));
  if(xmin < -m->packingxpoints) xmin = -m->packingxpoints;
  if(xmax > 2 * m->packingxpoints - 1) xmax = (2 * m->packingxpoints - 1);
  if(ymin < -m->packingypoints) ymin = -m->packingypoints;
  if(ymax > 2 * m->packingypoints - 1) ymax = (2 * m->packingypoints - 1);
  if(zmin < -m->packingzpoints) zmin = -m->packingzpoints;
  if(zmax > 2 * m->packingzpoints - 1) zmax = (2 * m->packingzpoints - 1);
  for (int iter1 = xmin; iter1 < xmax + 1; iter1++)
  {
    for (int iter2 = ymin; iter2 < ymax + 1; iter2++)
    {
      for (int iter3 = zmin; iter3 < zmax + 1; iter3++)
      {
        column = iter1;
        row = iter2;
        plane = iter3;
        x = float(column) * m->packingresx;
        y = float(row) * m->packingresy;
        z = float(plane) * m->packingresz;
        inside = -1;
		x = x - xc;
		y = y - yc;
		z = z - zc;
		xp = (x * ga[0][0]) + (y * ga[1][0]) + (z * ga[2][0]);
		yp = (x * ga[0][1]) + (y * ga[1][1]) + (z * ga[2][1]);
		zp = (x * ga[0][2]) + (y * ga[1][2]) + (z * ga[2][2]);
		float axis1comp = xp / radcur1;
		float axis2comp = yp / radcur2;
		float axis3comp = zp / radcur3;
		inside = m->m_ShapeOps[shapeclass]->inside(axis1comp, axis2comp, axis3comp);
		if(inside >= 0)
		{
			m->m_Grains[gnum]->columnlist->push_back(column);
			m->m_Grains[gnum]->rowlist->push_back(row);
			m->m_Grains[gnum]->planelist->push_back(plane);
		}
      }
    }
  }
}

void PackGrainsGen2::assign_voxels()
{
  int index;
  int neighpoints[6];
  neighpoints[0] = -(m->xpoints * m->ypoints);
  neighpoints[1] = -m->xpoints;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = m->xpoints;
  neighpoints[5] = (m->xpoints * m->ypoints);
  int oldname;
  int column, row, plane;
  float inside;
  float xc, yc, zc;
  float xp, yp, zp;
  float dist;
  float x, y, z;
  int xmin, xmax, ymin, ymax, zmin, zmax;

  m->gsizes.resize(m->m_Grains.size());

  for (size_t i = 1; i < m->m_Grains.size(); i++)
  {
    m->gsizes[i] = 0;
  }
  for (size_t i = 1; i < m->m_Grains.size(); i++)
  {
    float volcur = m->m_Grains[i]->volume;
    float bovera = m->m_Grains[i]->radius2;
    float covera = m->m_Grains[i]->radius3;
    float omega3 = m->m_Grains[i]->omega3;
    xc = m->m_Grains[i]->centroidx;
    yc = m->m_Grains[i]->centroidy;
    zc = m->m_Grains[i]->centroidz;
    float radcur1 = 0.0f;
    //Unbounded Check for the size of shapeTypes. We assume a 1:1 with phase
    DREAM3D::SyntheticBuilder::ShapeType shapeclass = m->shapeTypes[m->m_Grains[i]->phase];

    // init any values for each of the Shape Ops
    for (std::map<DREAM3D::SyntheticBuilder::ShapeType, DREAM3D::ShapeOps*>::iterator ops = m->m_ShapeOps.begin(); ops != m->m_ShapeOps.end(); ++ops )
    {
      (*ops).second->init();
    }
    // Create our Argument Map
    std::map<DREAM3D::ShapeOps::ArgName, float> shapeArgMap;
    shapeArgMap[DREAM3D::ShapeOps::Omega3] = omega3;
    shapeArgMap[DREAM3D::ShapeOps::VolCur] = volcur;
    shapeArgMap[DREAM3D::ShapeOps::B_OverA] = bovera;
    shapeArgMap[DREAM3D::ShapeOps::C_OverA] = covera;

    radcur1 = m->m_ShapeOps[shapeclass]->radcur1(shapeArgMap);

    float radcur2 = (radcur1 * bovera);
    float radcur3 = (radcur1 * covera);
    float phi1 = m->m_Grains[i]->axiseuler1;
    float PHI = m->m_Grains[i]->axiseuler2;
    float phi2 = m->m_Grains[i]->axiseuler3;
    float ga[3][3];
    ga[0][0] = cosf(phi1) * cosf(phi2) - sinf(phi1) * sinf(phi2) * cosf(PHI);
    ga[0][1] = sinf(phi1) * cosf(phi2) + cosf(phi1) * sinf(phi2) * cosf(PHI);
    ga[0][2] = sinf(phi2) * sinf(PHI);
    ga[1][0] = -cosf(phi1) * sinf(phi2) - sinf(phi1) * cosf(phi2) * cosf(PHI);
    ga[1][1] = -sinf(phi1) * sinf(phi2) + cosf(phi1) * cosf(phi2) * cosf(PHI);
    ga[1][2] = cosf(phi2) * sinf(PHI);
    ga[2][0] = sinf(phi1) * sinf(PHI);
    ga[2][1] = -cosf(phi1) * sinf(PHI);
    ga[2][2] = cosf(PHI);
    column = (xc - (m->resx / 2)) / m->resx;
    row = (yc - (m->resy / 2)) / m->resy;
    plane = (zc - (m->resz / 2)) / m->resz;
    xmin = int(column - ((radcur1 / m->resx) + 1));
    xmax = int(column + ((radcur1 / m->resx) + 1));
    ymin = int(row - ((radcur1 / m->resy) + 1));
    ymax = int(row + ((radcur1 / m->resy) + 1));
    zmin = int(plane - ((radcur1 / m->resz) + 1));
    zmax = int(plane + ((radcur1 / m->resz) + 1));
    if (m->periodic_boundaries == true)
    {
      if (xmin < -m->xpoints) xmin = -m->xpoints;
      if (xmax > 2 * m->xpoints - 1) xmax = (2 * m->xpoints - 1);
      if (ymin < -m->ypoints) ymin = -m->ypoints;
      if (ymax > 2 * m->ypoints - 1) ymax = (2 * m->ypoints - 1);
      if (zmin < -m->zpoints) zmin = -m->zpoints;
      if (zmax > 2 * m->zpoints - 1) zmax = (2 * m->zpoints - 1);
    }
    if (m->periodic_boundaries == false)
    {
      if (xmin < 0) xmin = 0;
      if (xmax > m->xpoints - 1) xmax = m->xpoints - 1;
      if (ymin < 0) ymin = 0;
      if (ymax > m->ypoints - 1) ymax = m->ypoints - 1;
      if (zmin < 0) zmin = 0;
      if (zmax > m->zpoints - 1) zmax = m->zpoints - 1;
    }
    for (int iter1 = xmin; iter1 < xmax + 1; iter1++)
    {
      for (int iter2 = ymin; iter2 < ymax + 1; iter2++)
      {
        for (int iter3 = zmin; iter3 < zmax + 1; iter3++)
        {
          column = iter1;
          row = iter2;
          plane = iter3;
          if (iter1 < 0) column = iter1 + m->xpoints;
          if (iter1 > m->xpoints - 1) column = iter1 - m->xpoints;
          if (iter2 < 0) row = iter2 + m->ypoints;
          if (iter2 > m->ypoints - 1) row = iter2 - m->ypoints;
          if (iter3 < 0) plane = iter3 + m->zpoints;
          if (iter3 > m->zpoints - 1) plane = iter3 - m->zpoints;
          index = (plane * m->xpoints * m->ypoints) + (row * m->xpoints) + column;
          inside = -1;
          x = float(column) * m->resx;
          y = float(row) * m->resy;
          z = float(plane) * m->resz;
          if (iter1 < 0) x = x - m->sizex;
          if (iter1 > m->xpoints - 1) x = x + m->sizex;
          if (iter2 < 0) y = y - m->sizey;
          if (iter2 > m->ypoints - 1) y = y + m->sizey;
          if (iter3 < 0) z = z - m->sizez;
          if (iter3 > m->zpoints - 1) z = z + m->sizez;
          dist = ((x - xc) * (x - xc)) + ((y - yc) * (y - yc)) + ((z - zc) * (z - zc));
          dist = sqrtf(dist);
          if (dist < radcur1)
          {
            x = x - xc;
            y = y - yc;
            z = z - zc;
            xp = (x * ga[0][0]) + (y * ga[1][0]) + (z * ga[2][0]);
            yp = (x * ga[0][1]) + (y * ga[1][1]) + (z * ga[2][1]);
            zp = (x * ga[0][2]) + (y * ga[1][2]) + (z * ga[2][2]);
            float axis1comp = xp / radcur1;
            float axis2comp = yp / radcur2;
            float axis3comp = zp / radcur3;
            inside = m->m_ShapeOps[shapeclass]->inside(axis1comp, axis2comp, axis3comp);
            if (inside >= 0)
            {
              int currentpoint = index;
              if (m->grain_indicies[currentpoint] > 0)
              {
                oldname = m->grain_indicies[currentpoint];
                m->gsizes[oldname] = m->gsizes[oldname] - 1;
                m->grain_indicies[currentpoint] = -1;
              }
              if (m->grain_indicies[currentpoint] == 0)
              {
                m->grain_indicies[currentpoint] = i;
                m->gsizes[i]++;
              }
            }
          }
        }
      }
    }
  }
  m->newnames.resize(m->m_Grains.size());
  int goodcount = 1;
  for (size_t i = 1; i < m->m_Grains.size(); i++)
  {
    m->newnames[i] = 0;
    if (m->gsizes[i] > 0)
    {
      m->m_Grains[goodcount] = m->m_Grains[i];
      m->newnames[i] = goodcount;
      goodcount++;
    }
  }
  for (int i = 0; i < m->totalpoints; i++)
  {
    if (m->grain_indicies[i] > 0)
    {
	  m->grain_indicies[i] = m->newnames[m->grain_indicies[i]];
    }
  }
  m->m_Grains.resize(goodcount);
}
