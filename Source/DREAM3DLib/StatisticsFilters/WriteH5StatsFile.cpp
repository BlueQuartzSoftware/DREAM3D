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

#include "WriteH5StatsFile.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

using namespace std;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteH5StatsFile::WriteH5StatsFile()
{
  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_HexOps.get()));

  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_CubicOps.get()));

  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_OrthoOps.get()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteH5StatsFile::~WriteH5StatsFile()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteH5StatsFile::execute()
{
  DataContainer* m = getDataContainer();
  setErrorCondition(0);

  H5StatsWriter::Pointer h5io = H5StatsWriter::New(getH5StatsFile());

  if(m->zpoints > 1) write_h5statsfile(h5io, getBinStepSize());
  if(m->zpoints == 1) write_h5statsfile2D(h5io, getBinStepSize());
  notify("WriteH5StatsFile Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteH5StatsFile::write_h5statsfile(H5StatsWriter::Pointer h5io, float sizebinstepsize)
{
  DataContainer* m = getDataContainer();
  int retErr = 0;
  float actualgrains = 0;
  float avglogdiam = 0;
  size_t numgrains = m->m_Grains.size();
  float *mindiameter;
  float *maxdiameter;
  mindiameter = new float [m->crystruct.size()];
  maxdiameter = new float [m->crystruct.size()];
  for(unsigned long long i=1;i<m->crystruct.size();i++)
  {
	  mindiameter[i] = 0;
	  maxdiameter[i] = 0;
  }
  for(size_t iter=1;iter<numgrains;iter++)
  {
	if (m->m_Grains[iter]->outsideboundbox == false && m->m_Grains[iter]->active == true)
	{
	  int phase = m->m_Grains[iter]->phase;
	  float diam = m->m_Grains[iter]->equivdiameter;
	  if(diam < mindiameter[phase]) mindiameter[phase] = diam;
	  if(diam > maxdiameter[phase]) maxdiameter[phase] = diam;
	}
  }
  std::vector<std::vector<float> > neighborhood;
  std::vector<std::vector<float> > neighborhoodfit;
  std::vector<std::vector<float> > svbovera;
  std::vector<std::vector<float> > svcovera;
  std::vector<std::vector<float> > svcoverb;
  std::vector<std::vector<float> > svschmid;
  std::vector<std::vector<float> > svomega3;
  for(size_t iter=1;iter<m->crystruct.size();iter++)
  {
	  int numbins = int((maxdiameter[iter] - mindiameter[iter]) / sizebinstepsize) + 1;
	  neighborhood.resize(numbins);
	  neighborhoodfit.resize(numbins);
	  svbovera.resize(numbins);
	  svcovera.resize(numbins);
	  svcoverb.resize(numbins);
	  svschmid.resize(numbins);
	  svomega3.resize(numbins);
	  for (int temp = 0; temp < numbins; temp++)
	  {
		neighborhood[temp].resize(7, 0);
		neighborhoodfit[temp].resize(4, 0);
		svbovera[temp].resize(5, 0);
		svcovera[temp].resize(5, 0);
		svcoverb[temp].resize(5, 0);
		svschmid[temp].resize(5, 0);
		svomega3[temp].resize(5, 0);
	  }
	  for (size_t i = 1; i < numgrains; i++)
	  {
		if (m->m_Grains[i]->outsideboundbox == false && m->m_Grains[i]->active == true && m->m_Grains[i]->phase == static_cast<int>(iter) )
		{
		  actualgrains++;
		  float diam = m->m_Grains[i]->equivdiameter;
		  float logdiam = log(diam);
		  float bovera = m->m_Grains[i]->aspectratio1;
		  float covera = m->m_Grains[i]->aspectratio2;
		  float coverb = covera/bovera;
		  float schmid = m->m_Grains[i]->schmidfactor;
		  float omega3 = m->m_Grains[i]->omega3;
		  avglogdiam = avglogdiam + logdiam;
		  int diamint = int((diam - mindiameter[iter]) / sizebinstepsize);
		  neighborhood[diamint][0]++;
		  svbovera[diamint][0]++;
		  svcovera[diamint][0]++;
		  svcoverb[diamint][0]++;
		  svschmid[diamint][0]++;
		  svomega3[diamint][0]++;
		  svbovera[diamint][1] = svbovera[diamint][1] + bovera;
		  svcovera[diamint][1] = svcovera[diamint][1] + covera;
		  svcoverb[diamint][1] = svcoverb[diamint][1] + coverb;
		  svschmid[diamint][1] = svschmid[diamint][1] + schmid;
		  svomega3[diamint][1] = svomega3[diamint][1] + omega3;
		  neighborhood[diamint][0]++;
		  for (int k = 0; k < 3; k++)
		  {
			int nnum = m->m_Grains[i]->neighbordistfunc[k];
			neighborhood[diamint][((2 * k) + 1)] = neighborhood[diamint][((2 * k) + 1)] + nnum;
		  }
		}
	  }
	  for (int temp3 = 0; temp3 < numbins; temp3++)
	  {
		if (svbovera[temp3][0] > 1)
		{
		  neighborhood[temp3][1] = neighborhood[temp3][1] / neighborhood[temp3][0];
		  neighborhood[temp3][3] = neighborhood[temp3][3] / neighborhood[temp3][0];
		  neighborhood[temp3][5] = neighborhood[temp3][5] / neighborhood[temp3][0];
		  svbovera[temp3][1] = svbovera[temp3][1] / svbovera[temp3][0];
		  svcovera[temp3][1] = svcovera[temp3][1] / svcovera[temp3][0];
		  svcoverb[temp3][1] = svcoverb[temp3][1] / svcoverb[temp3][0];
		  svschmid[temp3][1] = svschmid[temp3][1] / svschmid[temp3][0];
		  svomega3[temp3][1] = svomega3[temp3][1] / svomega3[temp3][0];
		  neighborhoodfit[temp3][0] = neighborhood[temp3][0];
		  neighborhoodfit[temp3][1] = neighborhood[temp3][3] - neighborhood[temp3][1];
		  neighborhoodfit[temp3][2] = neighborhood[temp3][1];
		  neighborhoodfit[temp3][3] = log((neighborhood[temp3][5] - neighborhoodfit[temp3][2]) / neighborhoodfit[temp3][1]) / log(2.0f);
		}
	  }
	  avglogdiam = avglogdiam / actualgrains;
	  float sdlogdiam = 0;
	  for (size_t j = 1; j < numgrains; j++)
	  {
		if (m->m_Grains[j]->outsideboundbox == false && m->m_Grains[j]->active == true && m->m_Grains[j]->phase == static_cast<int>(iter) )
		{
		  float diam = m->m_Grains[j]->equivdiameter;
		  float logdiam = log(diam);
		  float bovera = m->m_Grains[j]->aspectratio1;
		  float covera = m->m_Grains[j]->aspectratio2;
		  float coverb = covera/bovera;
		  float schmid = m->m_Grains[j]->schmidfactor;
		  float omega3 = m->m_Grains[j]->omega3;
		  sdlogdiam = sdlogdiam + ((logdiam - avglogdiam) * (logdiam - avglogdiam));
		  int diamint = int((diam - mindiameter[iter]) / sizebinstepsize);
		  svbovera[diamint][2] = svbovera[diamint][2] + ((bovera - svbovera[diamint][1]) * (bovera - svbovera[diamint][1]));
		  svcovera[diamint][2] = svcovera[diamint][2] + ((covera - svcovera[diamint][1]) * (covera - svcovera[diamint][1]));
		  svcoverb[diamint][2] = svcoverb[diamint][2] + ((coverb - svcoverb[diamint][1]) * (coverb - svcoverb[diamint][1]));
		  svschmid[diamint][2] = svschmid[diamint][2] + ((schmid - svschmid[diamint][1]) * (schmid - svschmid[diamint][1]));
		  svomega3[diamint][2] = svomega3[diamint][2] + ((omega3 - svomega3[diamint][1]) * (omega3 - svomega3[diamint][1]));
		  for (int k = 0; k < 3; k++)
		  {
			int nnum = m->m_Grains[j]->neighbordistfunc[k];
			neighborhood[diamint][((2 * k) + 2)] = neighborhood[diamint][((2 * k) + 2)] + ((neighborhood[diamint][((2 * k) + 1)] - nnum)
				* (neighborhood[diamint][((2 * k) + 1)] - nnum));
		  }
		}
	  }
	  for (int temp4 = 0; temp4 < numbins; temp4++)
	  {
		if (svbovera[temp4][0] > 1)
		{
		  neighborhood[temp4][2] = neighborhood[temp4][2] / neighborhood[temp4][0];
		  neighborhood[temp4][4] = neighborhood[temp4][4] / neighborhood[temp4][0];
		  neighborhood[temp4][6] = neighborhood[temp4][6] / neighborhood[temp4][0];
		  svbovera[temp4][2] = svbovera[temp4][2] / svbovera[temp4][0];
		  svcovera[temp4][2] = svcovera[temp4][2] / svcovera[temp4][0];
		  svcoverb[temp4][2] = svcoverb[temp4][2] / svcoverb[temp4][0];
		  svschmid[temp4][2] = svschmid[temp4][2] / svschmid[temp4][0];
		  svomega3[temp4][2] = svomega3[temp4][2] / svomega3[temp4][0];
		  svbovera[temp4][3] = svbovera[temp4][1] * (((svbovera[temp4][1] * (1 - svbovera[temp4][1])) / svbovera[temp4][2]) - 1);
		  svbovera[temp4][4] = (1 - svbovera[temp4][1]) * (((svbovera[temp4][1] * (1 - svbovera[temp4][1])) / svbovera[temp4][2]) - 1);
		  svcovera[temp4][3] = svcovera[temp4][1] * (((svcovera[temp4][1] * (1 - svcovera[temp4][1])) / svcovera[temp4][2]) - 1);
		  svcovera[temp4][4] = (1 - svcovera[temp4][1]) * (((svcovera[temp4][1] * (1 - svcovera[temp4][1])) / svcovera[temp4][2]) - 1);
		  svcoverb[temp4][3] = svcoverb[temp4][1] * (((svcoverb[temp4][1] * (1 - svcoverb[temp4][1])) / svcoverb[temp4][2]) - 1);
		  svcoverb[temp4][4] = (1 - svcoverb[temp4][1]) * (((svcoverb[temp4][1] * (1 - svcoverb[temp4][1])) / svcoverb[temp4][2]) - 1);
		  svomega3[temp4][3] = svomega3[temp4][1] * (((svomega3[temp4][1] * (1 - svomega3[temp4][1])) / svomega3[temp4][2]) - 1);
		  svomega3[temp4][4] = (1 - svomega3[temp4][1]) * (((svomega3[temp4][1] * (1 - svomega3[temp4][1])) / svomega3[temp4][2]) - 1);
		  neighborhood[temp4][2] = sqrt(neighborhood[temp4][2]);
		  neighborhood[temp4][4] = sqrt(neighborhood[temp4][4]);
		  neighborhood[temp4][6] = sqrt(neighborhood[temp4][6]);
		  svbovera[temp4][2] = sqrt(svbovera[temp4][2]);
		  svcovera[temp4][2] = sqrt(svcovera[temp4][2]);
		  svcoverb[temp4][2] = sqrt(svcoverb[temp4][2]);
		  svschmid[temp4][2] = sqrt(svschmid[temp4][2]);
		  svomega3[temp4][2] = sqrt(svomega3[temp4][2]);
		}
	  }
	  sdlogdiam = sdlogdiam / actualgrains;
	  sdlogdiam = sqrt(sdlogdiam);
	  retErr = h5io->writeVolumeStats(iter, m->crystruct[iter], m->phaseType[iter], m->phasefraction[iter], m->pptFractions[iter],
	                                  maxdiameter[iter], mindiameter[iter], sizebinstepsize, avglogdiam, sdlogdiam, svbovera, svcovera, svcoverb, neighborhoodfit, svomega3);
	  if (retErr < 0)
	  {
		  //FIXME: What are we doing in case of Error?s
	  }
  }
}

void WriteH5StatsFile::write_h5statsfile2D(H5StatsWriter::Pointer h5io, float sizebinstepsize)
{
  DataContainer* m = getDataContainer();

  float actualgrains = 0;
  float avglogdiam = 0;
  size_t numgrains = m->m_Grains.size();
  float *mindiameter;
  float *maxdiameter;
  mindiameter = new float [m->crystruct.size()];
  maxdiameter = new float [m->crystruct.size()];
  for(unsigned long long i=1;i<m->crystruct.size();i++)
  {
	  mindiameter[i] = 0;
	  maxdiameter[i] = 0;
  }
  for(size_t iter=1;iter<numgrains;iter++)
  {
	if (m->m_Grains[iter]->outsideboundbox == false && m->m_Grains[iter]->active == true)
	{
	  int phase = m->m_Grains[iter]->phase;
	  float diam = m->m_Grains[iter]->equivdiameter;
	  if(diam < mindiameter[phase]) mindiameter[phase] = diam;
	  if(diam > maxdiameter[phase]) maxdiameter[phase] = diam;
	}
  }
  vector<vector<float> > neighborhood;
  vector<vector<float> > neighborhoodfit;
  vector<vector<float> > svbovera;
  vector<vector<float> > svcovera;
  vector<vector<float> > svcoverb;
  vector<vector<float> > svschmid;
  vector<vector<float> > svomega3;
  for (size_t iter = 1; iter < m->crystruct.size(); iter++)
  {
    int numbins = int((maxdiameter[iter] - mindiameter[iter]) / sizebinstepsize) + 1;
    neighborhood.resize(numbins);
    neighborhoodfit.resize(numbins);
    svbovera.resize(numbins);
    svcovera.resize(numbins);
    svcoverb.resize(numbins);
    svschmid.resize(numbins);
    svomega3.resize(numbins);
    for (int temp = 0; temp < numbins; temp++)
    {
      neighborhood[temp].resize(7, 0);
      neighborhoodfit[temp].resize(4, 0);
      svbovera[temp].resize(5, 0);
      svschmid[temp].resize(5, 0);
    }
    for (size_t i = 1; i < numgrains; i++)
    {
      if (m->m_Grains[i]->outsideboundbox == false && m->m_Grains[i]->active == true && m->m_Grains[i]->phase == static_cast<int>(iter) )
      {
        actualgrains++;
        float diam = m->m_Grains[i]->equivdiameter;
        float logdiam = log(diam);
        float rad1 = m->m_Grains[i]->radius1;
        float rad2 = m->m_Grains[i]->radius2;
        float bovera = rad2 / rad1;
        float schmid = m->m_Grains[i]->schmidfactor;
        avglogdiam = avglogdiam + logdiam;
        int diamint = int((diam - mindiameter[iter]) / sizebinstepsize);
        neighborhood[diamint][0]++;
        svbovera[diamint][0]++;
        svschmid[diamint][0]++;
        svbovera[diamint][1] = svbovera[diamint][1] + bovera;
        svschmid[diamint][1] = svschmid[diamint][1] + schmid;
        neighborhood[diamint][0]++;
        for (int k = 0; k < 3; k++)
        {
          int nnum = m->m_Grains[i]->neighbordistfunc[k];
          neighborhood[diamint][((2 * k) + 1)] = neighborhood[diamint][((2 * k) + 1)] + nnum;
        }
      }
    }
    for (int temp3 = 0; temp3 < numbins; temp3++)
    {
      if (svbovera[temp3][0] != 0)
      {
        neighborhood[temp3][1] = neighborhood[temp3][1] / neighborhood[temp3][0];
        neighborhood[temp3][3] = neighborhood[temp3][3] / neighborhood[temp3][0];
        neighborhood[temp3][5] = neighborhood[temp3][5] / neighborhood[temp3][0];
        svbovera[temp3][1] = svbovera[temp3][1] / svbovera[temp3][0];
        svschmid[temp3][1] = svschmid[temp3][1] / svschmid[temp3][0];
        neighborhoodfit[temp3][0] = neighborhood[temp3][0];
        neighborhoodfit[temp3][1] = neighborhood[temp3][3] - neighborhood[temp3][1];
        neighborhoodfit[temp3][2] = neighborhood[temp3][1];
        neighborhoodfit[temp3][3] = log((neighborhood[temp3][5] - neighborhoodfit[temp3][2]) / neighborhoodfit[temp3][1]) / log(2.0);
      }
    }
    avglogdiam = avglogdiam / actualgrains;
    float sdlogdiam = 0;
    for (size_t j = 1; j < numgrains; j++)
    {
      if (m->m_Grains[j]->outsideboundbox == false && m->m_Grains[j]->active == true && m->m_Grains[j]->phase == static_cast<int>(iter) )
      {
        float diam = m->m_Grains[j]->equivdiameter;
        float logdiam = log(diam);
        float rad1 = m->m_Grains[j]->radius1;
        float rad2 = m->m_Grains[j]->radius2;
        float bovera = rad2 / rad1;
        float schmid = m->m_Grains[j]->schmidfactor;
        sdlogdiam = sdlogdiam + ((logdiam - avglogdiam) * (logdiam - avglogdiam));
        int diamint = int((diam - mindiameter[iter]) / sizebinstepsize);
        svbovera[diamint][2] = svbovera[diamint][2] + ((bovera - svbovera[diamint][1]) * (bovera - svbovera[diamint][1]));
        svschmid[diamint][2] = svschmid[diamint][2] + ((schmid - svschmid[diamint][1]) * (schmid - svschmid[diamint][1]));
        for (int k = 0; k < 3; k++)
        {
          int nnum = m->m_Grains[j]->neighbordistfunc[k];
          neighborhood[diamint][((2 * k) + 2)] = neighborhood[diamint][((2 * k) + 2)] + ((neighborhood[diamint][((2 * k) + 1)] - nnum)
              * (neighborhood[diamint][((2 * k) + 1)] - nnum));
        }
      }
    }
    for (int temp4 = 0; temp4 < numbins; temp4++)
    {
      if (svbovera[temp4][0] != 0)
      {
        neighborhood[temp4][2] = neighborhood[temp4][2] / neighborhood[temp4][0];
        neighborhood[temp4][4] = neighborhood[temp4][4] / neighborhood[temp4][0];
        neighborhood[temp4][6] = neighborhood[temp4][6] / neighborhood[temp4][0];
        svbovera[temp4][2] = svbovera[temp4][2] / svbovera[temp4][0];
        svschmid[temp4][2] = svschmid[temp4][2] / svschmid[temp4][0];
        svbovera[temp4][3] = svbovera[temp4][1] * (((svbovera[temp4][1] * (1 - svbovera[temp4][1])) / svbovera[temp4][2]) - 1);
        svbovera[temp4][4] = (1 - svbovera[temp4][1]) * (((svbovera[temp4][1] * (1 - svbovera[temp4][1])) / svbovera[temp4][2]) - 1);
        neighborhood[temp4][2] = sqrt(neighborhood[temp4][2]);
        neighborhood[temp4][4] = sqrt(neighborhood[temp4][4]);
        neighborhood[temp4][6] = sqrt(neighborhood[temp4][6]);
        svbovera[temp4][2] = sqrt(svbovera[temp4][2]);
        svschmid[temp4][2] = sqrt(svschmid[temp4][2]);
      }
    }
    sdlogdiam = sdlogdiam / actualgrains;
    sdlogdiam = sqrt(sdlogdiam);

    int err = h5io->writeVolumeStats2D(iter, m->crystruct[iter], m->phaseType[iter], m->phasefraction[iter], m->pptFractions[iter],
                                   maxdiameter[iter], mindiameter[iter], 1.0, avglogdiam, sdlogdiam, svbovera, neighborhoodfit);
    if (err < 0)
    {
    	std::cout << "Error Writing the Volume Stats for 2D Case." << std::endl;
    }
  }
}

