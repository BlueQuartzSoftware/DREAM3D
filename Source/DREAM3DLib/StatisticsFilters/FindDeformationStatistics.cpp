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

#include "FindDeformationStatistics.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

using namespace std;

const static float m_pi = M_PI;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindDeformationStatistics::FindDeformationStatistics()
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
FindDeformationStatistics::~FindDeformationStatistics()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDeformationStatistics::execute()
{
  setErrorCondition(0);

  DataContainer* m = getDataContainer();
  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }
  int64_t totalPoints = m->totalPoints();
  int32_t* grain_indicies = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, totalPoints, this);
  if(NULL == grain_indicies)
  {
    return;
  }
  int32_t* nearestneighbors = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::NearestNeighbors, (totalPoints * 3), this);
  if(NULL == nearestneighbors)
  {
    return;
  }
  float* nearestneighbordistances = m->getVoxelDataSizeCheck<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::NearestNeighborDistances, (totalPoints * 3), this);
  if(NULL == nearestneighbordistances)
  {
    return;
  }
  float* kernelmisorientations = m->getVoxelDataSizeCheck<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::KernelAverageMisorientations, totalPoints, this);
  if(NULL == kernelmisorientations)
  {
    return;
  }
  float* grainmisorientations = m->getVoxelDataSizeCheck<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainMisorientations, totalPoints, this);
  if(NULL == grainmisorientations)
  {
    return;
  }
  float* misorientationgradients = m->getVoxelDataSizeCheck<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::MisorientationGradients, totalPoints, this);
  if(NULL == misorientationgradients)
  {
    return;
  }
  std::string filename = m_OutputFile1;
  ofstream outFile;
  outFile.open(filename.c_str(), std::ios_base::binary);
  float w, n1, n2, n3;
  int distance;
  float km, gbdist, tjdist, qpdist, sf, sf2, sfmm, gam, lmg, ssap;
  int nearestneighbor, gname, gname2, ss1;
  float q1[5], q2[5];
  int kmdist[25];
  int gamdist[25];
  int lmgdist[25];
  float kmvgb[10][2];
  float gamvgb[10][2];
  float lmgvgb[10][2];
  float kmvtj[10][2];
  float gamvtj[10][2];
  float lmgvtj[10][2];
  float kmvqp[10][2];
  float gamvqp[10][2];
  float lmgvqp[10][2];
  float kmvsf[10][2];
  float gamvsf[10][2];
  float lmgvsf[10][2];
  float kmvsfmm[10][2];
  float gamvsfmm[10][2];
  float lmgvsfmm[10][2];
  float kmvssap[10][2];
  float gamvssap[10][2];
  float lmgvssap[10][2];
  float kmvdis[10][2];
  float gamvdis[10][2];
  float lmgvdis[10][2];
  float kmvsfdistthresh[10][10][2];
  float gamvsfdistthresh[10][10][2];
  float lmgvsfdistthresh[10][10][2];
  float kmvsfmmdistthresh[10][10][2];
  float gamvsfmmdistthresh[10][10][2];
  float lmgvsfmmdistthresh[10][10][2];
  float kmvssapdistthresh[10][10][2];
  float gamvssapdistthresh[10][10][2];
  float lmgvssapdistthresh[10][10][2];
  float kmvdisdistthresh[10][10][2];
  float gamvdisdistthresh[10][10][2];
  float lmgvdisdistthresh[10][10][2];
  float kmvsfmmssapthresh[10][10][2];
  float gamvsfmmssapthresh[10][10][2];
  float lmgvsfmmssapthresh[10][10][2];
  int kmbin, gambin, lmgbin;
  int gbbin, tjbin, qpbin;
  int sfbin, ssapbin, sfmmbin, disbin;
 // int actualpoints = 0;
  for (int h = 0; h < 25; h++)
  {
	kmdist[h] = 0;
	gamdist[h] = 0;
	lmgdist[h] = 0;
  }
  for (int h = 0; h < 10; h++)
  {
	  for (int i = 0; i < 10; i++)
	  {
	    for (int j = 0; j < 2; j++)
	    {
		  kmvsfmmssapthresh[h][i][j] = 0;
		  gamvsfmmssapthresh[h][i][j] = 0;
		  lmgvsfmmssapthresh[h][i][j] = 0;
		  kmvsfdistthresh[h][i][j] = 0;
		  gamvsfdistthresh[h][i][j] = 0;
		  lmgvsfdistthresh[h][i][j] = 0;
		  kmvssapdistthresh[h][i][j] = 0;
		  gamvssapdistthresh[h][i][j] = 0;
		  lmgvssapdistthresh[h][i][j] = 0;
		  kmvssapdistthresh[h][i][j] = 0;
		  gamvssapdistthresh[h][i][j] = 0;
		  lmgvssapdistthresh[h][i][j] = 0;
		  kmvsfmmdistthresh[h][i][j] = 0;
		  gamvsfmmdistthresh[h][i][j] = 0;
		  lmgvsfmmdistthresh[h][i][j] = 0;
		  kmvdisdistthresh[h][i][j] = 0;
		  gamvdisdistthresh[h][i][j] = 0;
		  lmgvdisdistthresh[h][i][j] = 0;
		  if(h == 0)
		  {
			  kmvgb[i][j] = 0;
			  gamvgb[i][j] = 0;
			  lmgvgb[i][j] = 0;
			  kmvtj[i][j] = 0;
			  gamvtj[i][j] = 0;
			  lmgvtj[i][j] = 0;
			  kmvqp[i][j] = 0;
			  gamvqp[i][j] = 0;
			  lmgvqp[i][j] = 0;
			  kmvsf[i][j] = 0;
			  gamvsf[i][j] = 0;
			  lmgvsf[i][j] = 0;
			  kmvsfmm[i][j] = 0;
			  gamvsfmm[i][j] = 0;
			  lmgvsfmm[i][j] = 0;
			  kmvssap[i][j] = 0;
			  gamvssap[i][j] = 0;
			  lmgvssap[i][j] = 0;
			  kmvdis[i][j] = 0;
			  gamvdis[i][j] = 0;
			  lmgvdis[i][j] = 0;
		  }
		}
    }
  }

  for (int i = 0; i < totalPoints; i++)
  {
    gname = grain_indicies[i];
	if(gname > 0)
	{
		  km = kernelmisorientations[i];
		  gam = grainmisorientations[i];
		  lmg = misorientationgradients[i];
		  gbdist = nearestneighbordistances[i*3 + 0];
		  tjdist = nearestneighbordistances[i*3 + 1];
		  qpdist = nearestneighbordistances[i*3 + 2];
		  nearestneighbor = nearestneighbors[i*3 + 0];
		  gname2 = grain_indicies[nearestneighbor];
		  sf = m->m_Grains[gname]->schmidfactor;
		  sf2 = m->m_Grains[gname2]->schmidfactor;
		  sfmm = sf / sf2;
		  ss1 = m->m_Grains[gname]->slipsystem;
	//	  ss2 = m->m_Grains[gname2]->slipsystem;
		  for(int j=0;j<5;j++)
		  {
			q1[j] = m->m_Grains[gname]->avg_quat[j]/m->m_Grains[gname]->avg_quat[0];
			q2[j] = m->m_Grains[gname2]->avg_quat[j]/m->m_Grains[gname2]->avg_quat[0];
		  }
		  OrientationMath::getSlipMisalignment(ss1, q1, q2, ssap);
		  if (m->crystruct[m->m_Grains[gname]->phase] == m->crystruct[m->m_Grains[gname2]->phase] && m->m_Grains[gname]->phase > 0)
		  {
			w = m_OrientationOps[m->crystruct[m->m_Grains[gname]->phase]]->getMisoQuat(q1, q2, n1, n2, n3);
		  }
		  else
		  {
			w = 0;
		  }
		  kmbin = int(km/0.2);
		  gambin = int(gam/0.8);
		  lmgbin = int(lmg/0.1);
		  gbbin = int(gbdist);
		  tjbin = int(tjdist);
		  qpbin = int(qpdist);
		  sfbin = int((sf-0.25) / 0.025);
		  if(sfmm >= 1) sfmmbin = int((sfmm-1.0)/0.2)+5;
		  if(sfmm < 1) sfmmbin = 4-int(((1.0/sfmm)-1.0)/0.2);
		  ssapbin = int((ssap-0.4) / 0.06);
		  disbin = int((w) / 10.0);
		  if (kmbin < 0) kmbin = 0;
		  if (kmbin > 24) kmbin = 24;
		  if (gambin < 0) gambin = 0;
		  if (gambin > 24) gambin = 24;
		  if (lmgbin < 0) lmgbin = 0;
		  if (lmgbin > 24) lmgbin = 24;
		  if (gbbin < 0) gbbin = 0;
		  if (gbbin > 9) gbbin = 9;
		  if (tjbin < 0) tjbin = 0;
		  if (tjbin > 9) tjbin = 9;
		  if (qpbin < 0) qpbin = 0;
		  if (qpbin > 9) qpbin = 9;
		  if (sfbin < 0) sfbin = 0;
		  if (sfbin > 9) sfbin = 9;
		  if (sfmmbin < 0) sfmmbin = 0;
		  if (sfmmbin > 9) sfmmbin = 9;
		  if (ssapbin < 0) ssapbin = 0;
		  if (ssapbin > 9) ssapbin = 9;
		  if (disbin < 0) disbin = 0;
		  if (disbin > 9) disbin = 9;
		  kmdist[kmbin]++;
		  gamdist[gambin]++;
		  lmgdist[lmgbin]++;
		  kmvgb[gbbin][0]++;
		  kmvgb[gbbin][1] = kmvgb[gbbin][1] + km;
		  gamvgb[gbbin][0]++;
		  gamvgb[gbbin][1] = gamvgb[gbbin][1] + gam;
		  lmgvgb[gbbin][0]++;
		  lmgvgb[gbbin][1] = lmgvgb[gbbin][1] + lmg;
		  kmvtj[tjbin][0]++;
		  kmvtj[tjbin][1] = kmvtj[tjbin][1] + km;
		  gamvtj[tjbin][0]++;
		  gamvtj[tjbin][1] = gamvtj[tjbin][1] + gam;
		  lmgvtj[tjbin][0]++;
		  lmgvtj[tjbin][1] = lmgvtj[tjbin][1] + lmg;
		  kmvqp[qpbin][0]++;
		  kmvqp[qpbin][1] = kmvqp[qpbin][1] + km;
		  gamvqp[qpbin][0]++;
		  gamvqp[qpbin][1] = gamvqp[qpbin][1] + gam;
		  lmgvqp[qpbin][0]++;
		  lmgvqp[qpbin][1] = lmgvqp[qpbin][1] + lmg;
		  distance = int(nearestneighbordistances[i*3 + 0]);
		  if(distance > 9) distance = 9;
		  if(distance <= 5)
		  {
			  kmvsf[sfbin][0]++;
			  kmvsf[sfbin][1] = kmvsf[sfbin][1] + km;
			  gamvsf[sfbin][0]++;
			  gamvsf[sfbin][1] = gamvsf[sfbin][1] + gam;
			  lmgvsf[sfbin][0]++;
			  lmgvsf[sfbin][1] = lmgvsf[sfbin][1] + lmg;
			  kmvsfmm[sfmmbin][0]++;
			  kmvsfmm[sfmmbin][1] = kmvsfmm[sfmmbin][1] + km;
			  gamvsfmm[sfmmbin][0]++;
			  gamvsfmm[sfmmbin][1] = gamvsfmm[sfmmbin][1] + gam;
			  lmgvsfmm[sfmmbin][0]++;
			  lmgvsfmm[sfmmbin][1] = lmgvsfmm[sfmmbin][1] + lmg;
			  kmvssap[ssapbin][0]++;
			  kmvssap[ssapbin][1] = kmvssap[ssapbin][1] + km;
			  gamvssap[ssapbin][0]++;
			  gamvssap[ssapbin][1] = gamvssap[ssapbin][1] + gam;
			  lmgvssap[ssapbin][0]++;
			  lmgvssap[ssapbin][1] = lmgvssap[ssapbin][1] + lmg;
			  kmvdis[disbin][0]++;
			  kmvdis[disbin][1] = kmvdis[disbin][1] + km;
			  gamvdis[disbin][0]++;
			  gamvdis[disbin][1] = gamvdis[disbin][1] + gam;
			  lmgvdis[disbin][0]++;
			  lmgvdis[disbin][1] = lmgvdis[disbin][1] + lmg;
		  }
		  kmvsfdistthresh[distance][sfbin][0]++;
		  kmvsfdistthresh[distance][sfbin][1] = kmvsfdistthresh[distance][sfbin][1] + km;
		  gamvsfdistthresh[distance][sfbin][0]++;
		  gamvsfdistthresh[distance][sfbin][1] = gamvsfdistthresh[distance][sfbin][1] + gam;
		  lmgvsfdistthresh[distance][sfbin][0]++;
		  lmgvsfdistthresh[distance][sfbin][1] = lmgvsfdistthresh[distance][sfbin][1] + lmg;
		  kmvsfmmdistthresh[distance][sfmmbin][0]++;
		  kmvsfmmdistthresh[distance][sfmmbin][1] = kmvsfmmdistthresh[distance][sfmmbin][1] + km;
		  gamvsfmmdistthresh[distance][sfmmbin][0]++;
		  gamvsfmmdistthresh[distance][sfmmbin][1] = gamvsfmmdistthresh[distance][sfmmbin][1] + gam;
		  lmgvsfmmdistthresh[distance][sfmmbin][0]++;
		  lmgvsfmmdistthresh[distance][sfmmbin][1] = lmgvsfmmdistthresh[distance][sfmmbin][1] + lmg;
		  kmvssapdistthresh[distance][ssapbin][0]++;
		  kmvssapdistthresh[distance][ssapbin][1] = kmvssapdistthresh[distance][ssapbin][1] + km;
		  gamvssapdistthresh[distance][ssapbin][0]++;
		  gamvssapdistthresh[distance][ssapbin][1] = gamvssapdistthresh[distance][ssapbin][1] + gam;
		  lmgvssapdistthresh[distance][ssapbin][0]++;
		  lmgvssapdistthresh[distance][ssapbin][1] = lmgvssapdistthresh[distance][ssapbin][1] + lmg;
		  kmvdisdistthresh[distance][disbin][0]++;
		  kmvdisdistthresh[distance][disbin][1] = kmvdisdistthresh[distance][disbin][1] + km;
		  gamvdisdistthresh[distance][disbin][0]++;
		  gamvdisdistthresh[distance][disbin][1] = gamvdisdistthresh[distance][disbin][1] + gam;
		  lmgvdisdistthresh[distance][disbin][0]++;
		  lmgvdisdistthresh[distance][disbin][1] = lmgvdisdistthresh[distance][disbin][1] + lmg;
		  kmvsfmmssapthresh[sfmmbin][ssapbin][0]++;
		  kmvsfmmssapthresh[sfmmbin][ssapbin][1] = kmvsfmmssapthresh[sfmmbin][ssapbin][1] + km;
		  gamvsfmmssapthresh[sfmmbin][ssapbin][0]++;
		  gamvsfmmssapthresh[sfmmbin][ssapbin][1] = gamvsfmmssapthresh[sfmmbin][ssapbin][1] + gam;
		  lmgvsfmmssapthresh[sfmmbin][ssapbin][0]++;
		  lmgvsfmmssapthresh[sfmmbin][ssapbin][1] = lmgvsfmmssapthresh[sfmmbin][ssapbin][1] + lmg;
	}
  }
  outFile << "Kernel Misorientation Data" << endl;
  outFile << "GB		TJ		QP		SF		SFMM		SSAP		DIS" << endl;
  for (int i = 0; i < 10; i++)
  {
    if (kmvgb[i][0] > 0) kmvgb[i][1] = kmvgb[i][1] / kmvgb[i][0];
    if (kmvtj[i][0] > 0) kmvtj[i][1] = kmvtj[i][1] / kmvtj[i][0];
    if (kmvqp[i][0] > 0) kmvqp[i][1] = kmvqp[i][1] / kmvqp[i][0];
    if (kmvsf[i][0] > 0) kmvsf[i][1] = kmvsf[i][1] / kmvsf[i][0];
    if (kmvsfmm[i][0] > 0) kmvsfmm[i][1] = kmvsfmm[i][1] / kmvsfmm[i][0];
    if (kmvssap[i][0] > 0) kmvssap[i][1] = kmvssap[i][1] / kmvssap[i][0];
    if (kmvdis[i][0] > 0) kmvdis[i][1] = kmvdis[i][1] / kmvdis[i][0];
    outFile << kmvgb[i][0] << "	" << kmvgb[i][1] << "	"
		<< kmvtj[i][0] << "	" << kmvtj[i][1] << "	"
		<< kmvqp[i][0] << "	" << kmvqp[i][1] << "	"
		<< kmvsf[i][0] << "	" << kmvsf[i][1] << "	"
		<< kmvsfmm[i][0] << "	" << kmvsfmm[i][1] << "	"
		<< kmvssap[i][0] << "	" << kmvssap[i][1] << "	"
		<< kmvdis[i][0] << "	" << kmvdis[i][1] << endl;
  }
  outFile << endl;
  outFile << endl;
  outFile << "Grain Average Misorientation Data" << endl;
  outFile << "GB		TJ		QP		SF		SFMM		SSAP		DIS" << endl;
  for (int i = 0; i < 10; i++)
  {
    if (gamvgb[i][0] > 0) gamvgb[i][1] = gamvgb[i][1] / gamvgb[i][0];
    if (gamvtj[i][0] > 0) gamvtj[i][1] = gamvtj[i][1] / gamvtj[i][0];
    if (gamvqp[i][0] > 0) gamvqp[i][1] = gamvqp[i][1] / gamvqp[i][0];
    if (gamvsf[i][0] > 0) gamvsf[i][1] = gamvsf[i][1] / gamvsf[i][0];
    if (gamvsfmm[i][0] > 0) gamvsfmm[i][1] = gamvsfmm[i][1] / gamvsfmm[i][0];
    if (gamvssap[i][0] > 0) gamvssap[i][1] = gamvssap[i][1] / gamvssap[i][0];
    if (gamvdis[i][0] > 0) gamvdis[i][1] = gamvdis[i][1] / gamvdis[i][0];
    outFile << gamvgb[i][0] << "	" << gamvgb[i][1] << "	"
		<< gamvtj[i][0] << "	" << gamvtj[i][1] << "	"
		<< gamvqp[i][0] << "	" << gamvqp[i][1] << "	"
		<< gamvsf[i][0] << "	" << gamvsf[i][1] << "	"
		<< gamvsfmm[i][0] << "	" << gamvsfmm[i][1] << "	"
		<< gamvssap[i][0] << "	" << gamvssap[i][1] << "	"
		<< gamvdis[i][0] << "	" << gamvdis[i][1] << endl;
  }
  outFile << endl;
  outFile << endl;
  outFile << "Misorientation Gradient Data" << endl;
  outFile << "GB		TJ		QP		SF		SFMM		SSAP		DIS" << endl;
  for (int i = 0; i < 10; i++)
  {
    if (lmgvgb[i][0] > 0) lmgvgb[i][1] = lmgvgb[i][1] / lmgvgb[i][0];
    if (lmgvtj[i][0] > 0) lmgvtj[i][1] = lmgvtj[i][1] / lmgvtj[i][0];
    if (lmgvqp[i][0] > 0) lmgvqp[i][1] = lmgvqp[i][1] / lmgvqp[i][0];
    if (lmgvsf[i][0] > 0) lmgvsf[i][1] = lmgvsf[i][1] / lmgvsf[i][0];
    if (lmgvsfmm[i][0] > 0) lmgvsfmm[i][1] = lmgvsfmm[i][1] / lmgvsfmm[i][0];
    if (lmgvssap[i][0] > 0) lmgvssap[i][1] = lmgvssap[i][1] / lmgvssap[i][0];
    if (lmgvdis[i][0] > 0) lmgvdis[i][1] = lmgvdis[i][1] / lmgvdis[i][0];
    outFile << lmgvgb[i][0] << "	" << lmgvgb[i][1] << "	"
		<< lmgvtj[i][0] << "	" << lmgvtj[i][1] << "	"
		<< lmgvqp[i][0] << "	" << lmgvqp[i][1] << "	"
		<< lmgvsf[i][0] << "	" << lmgvsf[i][1] << "	"
		<< lmgvsfmm[i][0] << "	" << lmgvsfmm[i][1] << "	"
		<< lmgvssap[i][0] << "	" << lmgvssap[i][1] << "	"
		<< lmgvdis[i][0] << "	" << lmgvdis[i][1] << endl;
  }
  outFile << endl;
  outFile << endl;
  outFile << "Thresheld Data" << endl;
  outFile << "SF KAM" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (kmvsfdistthresh[j][i][0] > 0) kmvsfdistthresh[j][i][1] = kmvsfdistthresh[j][i][1] / kmvsfdistthresh[j][i][0];
	    outFile << kmvsfdistthresh[j][i][0] << "	" << kmvsfdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SFMM KAM" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (kmvsfmmdistthresh[j][i][0] > 0) kmvsfmmdistthresh[j][i][1] = kmvsfmmdistthresh[j][i][1] / kmvsfmmdistthresh[j][i][0];
	    outFile << kmvsfmmdistthresh[j][i][0] << "	" << kmvsfmmdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SSAP KAM" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (kmvssapdistthresh[j][i][0] > 0) kmvssapdistthresh[j][i][1] = kmvssapdistthresh[j][i][1] / kmvssapdistthresh[j][i][0];
	    outFile << kmvssapdistthresh[j][i][0] << "	" << kmvssapdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "DIS KAM" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (kmvdisdistthresh[j][i][0] > 0) kmvdisdistthresh[j][i][1] = kmvdisdistthresh[j][i][1] / kmvdisdistthresh[j][i][0];
	    outFile << kmvdisdistthresh[j][i][0] << "	" << kmvdisdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SF GAM" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (gamvsfdistthresh[j][i][0] > 0) gamvsfdistthresh[j][i][1] = gamvsfdistthresh[j][i][1] / gamvsfdistthresh[j][i][0];
	    outFile << gamvsfdistthresh[j][i][0] << "	" << gamvsfdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SFMM GAM" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (gamvsfmmdistthresh[j][i][0] > 0) gamvsfmmdistthresh[j][i][1] = gamvsfmmdistthresh[j][i][1] / gamvsfmmdistthresh[j][i][0];
	    outFile << gamvsfmmdistthresh[j][i][0] << "	" << gamvsfmmdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SSAP GAM" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (gamvssapdistthresh[j][i][0] > 0) gamvssapdistthresh[j][i][1] = gamvssapdistthresh[j][i][1] / gamvssapdistthresh[j][i][0];
	    outFile << gamvssapdistthresh[j][i][0] << "	" << gamvssapdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "DIS GAM" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (gamvdisdistthresh[j][i][0] > 0) gamvdisdistthresh[j][i][1] = gamvdisdistthresh[j][i][1] / gamvdisdistthresh[j][i][0];
	    outFile << gamvdisdistthresh[j][i][0] << "	" << gamvdisdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SF LMG" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (lmgvsfdistthresh[j][i][0] > 0) lmgvsfdistthresh[j][i][1] = lmgvsfdistthresh[j][i][1] / lmgvsfdistthresh[j][i][0];
	    outFile << lmgvsfdistthresh[j][i][0] << "	" << lmgvsfdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SFMM LMG" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (lmgvsfmmdistthresh[j][i][0] > 0) lmgvsfmmdistthresh[j][i][1] = lmgvsfmmdistthresh[j][i][1] / lmgvsfmmdistthresh[j][i][0];
	    outFile << lmgvsfmmdistthresh[j][i][0] << "	" << lmgvsfmmdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SSAP LMG" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (lmgvssapdistthresh[j][i][0] > 0) lmgvssapdistthresh[j][i][1] = lmgvssapdistthresh[j][i][1] / lmgvssapdistthresh[j][i][0];
	    outFile << lmgvssapdistthresh[j][i][0] << "	" << lmgvssapdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "DIS LMG" << endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (lmgvdisdistthresh[j][i][0] > 0) lmgvdisdistthresh[j][i][1] = lmgvdisdistthresh[j][i][1] / lmgvdisdistthresh[j][i][0];
	    outFile << lmgvdisdistthresh[j][i][0] << "	" << lmgvdisdistthresh[j][i][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SFMM SSAP KAM" << endl;
  outFile << "0.43		0.49		0.55		0.61		0.67		0.73		0.79		0.85		0.91		0.97" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (kmvsfmmssapthresh[i][j][0] > 0) kmvsfmmssapthresh[i][j][1] = kmvsfmmssapthresh[i][j][1] / kmvsfmmssapthresh[i][j][0];
	    outFile << kmvsfmmssapthresh[i][j][0] << "	" << kmvsfmmssapthresh[i][j][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SFMM SSAP GAM" << endl;
  outFile << "0.43		0.49		0.55		0.61		0.67		0.73		0.79		0.85		0.91		0.97" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (gamvsfmmssapthresh[i][j][0] > 0) gamvsfmmssapthresh[i][j][1] = gamvsfmmssapthresh[i][j][1] / gamvsfmmssapthresh[i][j][0];
	    outFile << gamvsfmmssapthresh[i][j][0] << "	" << gamvsfmmssapthresh[i][j][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "SFMM SSAP LMG" << endl;
  outFile << "0.43		0.49		0.55		0.61		0.67		0.73		0.79		0.85		0.91		0.97" << endl;
  for (int i = 0; i < 10; i++)
  {
	  for (int j = 0; j < 10; j++)
	  {
	    if (lmgvsfmmssapthresh[i][j][0] > 0) lmgvsfmmssapthresh[i][j][1] = lmgvsfmmssapthresh[i][j][1] / lmgvsfmmssapthresh[i][j][0];
	    outFile << lmgvsfmmssapthresh[i][j][0] << "	" << lmgvsfmmssapthresh[i][j][1] << "	";
	  }
	  outFile << endl;
  }
  outFile << "KAM DIST		GAM DIST		LMG DIST" << endl;
  for (int i = 0; i < 25; i++)
  {
	    outFile << float(i)*0.2+0.1 << "	" << kmdist[i] << "	" << float(i)*0.8+0.4 << "	" << gamdist[i] << "	" << float(i)*0.1+0.05 << "	" << lmgdist[i] << endl;
  }
  outFile.close();

  std::string filename2 = m_OutputFile2;
  FILE* vtkFile = NULL;
  vtkFile = fopen(filename2.c_str(), "wb");
  if (NULL == vtkFile)
  {
	std::cout << "Error Creating VTK Visualization File '" << filename << "'" << std::endl;
	return;
  }
  fprintf(vtkFile, "# vtk DataFile Version 2.0\n");
  fprintf(vtkFile,  "data set from FFT2dx_GB\n");
  fprintf(vtkFile,  "ASCII\n");
  fprintf(vtkFile,  "DATASET UNSTRUCTURED_GRID\n");
  fprintf(vtkFile,  "POINTS %ld float\n", m->m_Grains.size()-1);

  std::vector<Field::Pointer>::size_type size = m->m_Grains.size();

  for(size_t i=1;i<size;i++)
  {
		float x = m->m_Grains[i]->IPF[0] - (m->m_Grains[i]->IPF[0] * (m->m_Grains[i]->IPF[2] / (m->m_Grains[i]->IPF[2] + 1)));;
		float y = m->m_Grains[i]->IPF[1] - (m->m_Grains[i]->IPF[1] * (m->m_Grains[i]->IPF[2] / (m->m_Grains[i]->IPF[2] + 1)));;
		float z = 0.0;
		fprintf(vtkFile, "%f %f %f\n", x, y, z);
  }

  fprintf(vtkFile, "CELLS %ld %ld\n", m->m_Grains.size()-1, ((m->m_Grains.size()-1)*2));
//  Store the Grain Ids so we don't have to re-read the triangles file again
  for(size_t i=1;i<size;i++)
  {
	   fprintf(vtkFile, "1 %ld\n", (i-1));
  }

  // Write the CELL_TYPES into the file
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "CELL_TYPES %ld\n", m->m_Grains.size()-1);
  for(size_t i=1;i<size;i++)
  {
	fprintf(vtkFile, "1\n");
  }


  // Write the GrainId Data to teh file
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "CELL_DATA %ld\n", m->m_Grains.size()-1);
  fprintf(vtkFile, "SCALARS Misorientation float\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");
  for (size_t i = 1; i < size; i++)
  {
	  float miso = m->m_Grains[i]->averagemisorientation;
	  fprintf(vtkFile, "%f\n", miso);
  }
  fclose(vtkFile);

  notify("FindDeformationStatistics Completed", 0, Observable::UpdateProgressMessage);
}
