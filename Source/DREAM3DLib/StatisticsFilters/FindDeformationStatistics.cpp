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
#include "DREAM3DLib/GenericFilters/FindGrainPhases.h"


const static float m_pi = static_cast<float>(M_PI);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindDeformationStatistics::FindDeformationStatistics() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_GrainReferenceMisorientationsArrayName(DREAM3D::CellData::GrainReferenceMisorientations),
m_KernelAverageMisorientationsArrayName(DREAM3D::CellData::KernelAverageMisorientations),
m_NearestNeighborDistancesArrayName(DREAM3D::CellData::NearestNeighborDistances),
m_NearestNeighborsArrayName(DREAM3D::CellData::NearestNeighbors),
m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
m_GrainAvgMisorientationsArrayName(DREAM3D::FieldData::GrainAvgMisorientations),
m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
m_PolesArrayName(DREAM3D::FieldData::Poles),
m_SchmidsArrayName(DREAM3D::FieldData::Schmids),
m_SlipSystemsArrayName(DREAM3D::FieldData::SlipSystems),
m_GrainIds(NULL),
m_FieldPhases(NULL),
m_NearestNeighbors(NULL),
m_SlipSystems(NULL),
m_GrainReferenceMisorientations(NULL),
m_KernelAverageMisorientations(NULL),
m_AvgQuats(NULL),
m_GrainAvgMisorientations(NULL),
m_Poles(NULL),
m_Schmids(NULL),
m_NearestNeighborDistances(NULL)
{
  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_HexOps.get()));

  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_CubicOps.get()));

  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*> (m_OrthoOps.get()));
  setupFilterParameters();
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
void FindDeformationStatistics::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Deformation Statistics File");
    option->setPropertyName("DeformationStatisticsFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("VTK Output File");
    option->setPropertyName("VtkOutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void FindDeformationStatistics::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("DeformationStatisticsFile", getDeformationStatisticsFile() );
  writer->writeValue("VtkOutputFile", getVtkOutputFile() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDeformationStatistics::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  int err = 0;

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, KernelAverageMisorientations, ss, -300, float, FloatArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainReferenceMisorientations, ss, -300, float, FloatArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, NearestNeighbors, ss, -300, int32_t, Int32ArrayType, voxels, 3)
  GET_PREREQ_DATA(m, DREAM3D, CellData, NearestNeighborDistances, ss, -300, float, FloatArrayType, voxels, 3)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, Schmids, ss, -305, float, FloatArrayType, fields, 1)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, SlipSystems, ss, -306, int32_t, Int32ArrayType, fields, 1)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -301, float, FloatArrayType, fields, 5)
  TEST_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, err, -302, int32_t, Int32ArrayType, fields, 1)
  if(err == -302)
  {
    setErrorCondition(0);
    FindGrainPhases::Pointer find_grainphases = FindGrainPhases::New();
    find_grainphases->setObservers(this->getObservers());
    find_grainphases->setVoxelDataContainer(getVoxelDataContainer());
    if(preflight == true) find_grainphases->preflight();
    if(preflight == false) find_grainphases->execute();
  }
  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -302, int32_t, Int32ArrayType, fields, 1)


  GET_PREREQ_DATA(m, DREAM3D, FieldData, Poles, ss, -306, int32_t, Int32ArrayType, fields, 3)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, GrainAvgMisorientations, ss, -306, float, FloatArrayType, fields, 1)

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDeformationStatistics::preflight()
{
  dataCheck(true, 1,1 ,1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDeformationStatistics::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  int64_t totalPoints = m->getTotalPoints();
  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  typedef DataArray<unsigned int> XTalType;
  XTalType* crystruct
      = XTalType::SafeObjectDownCast<IDataArray*, XTalType*>(m->getEnsembleData(DREAM3D::EnsembleData::CrystalStructures).get());

//  size_t numXTals = crystruct->GetNumberOfTuples();


//  std::string filename = m_OutputFile1;
  std::ofstream outFile;
  outFile.open(m_DeformationStatisticsFile.c_str(), std::ios_base::binary);
  float w, n1, n2, n3;
  int distance;
  float km, gbdist, tjdist, qpdist, sf, sf2, sfmm, gam, ssap;
  int nearestneighbor, gname, gname2, ss1;
  float q1[5], q2[5];
  int kmdist[25];
  int gamdist[25];
  float kmvgb[10][2];
  float gamvgb[10][2];
  float kmvtj[10][2];
  float gamvtj[10][2];
  float kmvqp[10][2];
  float gamvqp[10][2];
  float kmvsf[10][2];
  float gamvsf[10][2];
  float kmvsfmm[10][2];
  float gamvsfmm[10][2];
  float kmvssap[10][2];
  float gamvssap[10][2];
  float kmvdis[10][2];
  float gamvdis[10][2];
  float kmvsfdistthresh[10][10][2];
  float gamvsfdistthresh[10][10][2];
  float kmvsfmmdistthresh[10][10][2];
  float gamvsfmmdistthresh[10][10][2];
  float kmvssapdistthresh[10][10][2];
  float gamvssapdistthresh[10][10][2];
  float kmvdisdistthresh[10][10][2];
  float gamvdisdistthresh[10][10][2];
  float kmvsfmmssapthresh[10][10][2];
  float gamvsfmmssapthresh[10][10][2];
  int kmbin, gambin;
  int gbbin, tjbin, qpbin;
  int sfbin, ssapbin, sfmmbin, disbin;
 // int actualpoints = 0;
  for (int h = 0; h < 25; h++)
  {
    kmdist[h] = 0;
    gamdist[h] = 0;
  }
  for (int h = 0; h < 10; h++)
  {
      for (int i = 0; i < 10; i++)
      {
        for (int j = 0; j < 2; j++)
        {
          kmvsfmmssapthresh[h][i][j] = 0;
          gamvsfmmssapthresh[h][i][j] = 0;
          kmvsfdistthresh[h][i][j] = 0;
          gamvsfdistthresh[h][i][j] = 0;
          kmvssapdistthresh[h][i][j] = 0;
          gamvssapdistthresh[h][i][j] = 0;
          kmvssapdistthresh[h][i][j] = 0;
          gamvssapdistthresh[h][i][j] = 0;
          kmvsfmmdistthresh[h][i][j] = 0;
          gamvsfmmdistthresh[h][i][j] = 0;
          kmvdisdistthresh[h][i][j] = 0;
          gamvdisdistthresh[h][i][j] = 0;
          if(h == 0)
          {
              kmvgb[i][j] = 0;
              gamvgb[i][j] = 0;
              kmvtj[i][j] = 0;
              gamvtj[i][j] = 0;
              kmvqp[i][j] = 0;
              gamvqp[i][j] = 0;
              kmvsf[i][j] = 0;
              gamvsf[i][j] = 0;
              kmvsfmm[i][j] = 0;
              gamvsfmm[i][j] = 0;
              kmvssap[i][j] = 0;
              gamvssap[i][j] = 0;
              kmvdis[i][j] = 0;
              gamvdis[i][j] = 0;
          }
        }
    }
  }

  for (int i = 0; i < totalPoints; i++)
  {
    gname = m_GrainIds[i];
    if(gname > 0)
    {
      km = m_KernelAverageMisorientations[i];
      gam = m_GrainReferenceMisorientations[i];
      gbdist = m_NearestNeighborDistances[i * 3 + 0];
      tjdist = m_NearestNeighborDistances[i * 3 + 1];
      qpdist = m_NearestNeighborDistances[i * 3 + 2];
      nearestneighbor = m_NearestNeighbors[i * 3 + 0];
      gname2 = m_GrainIds[nearestneighbor];
      sf = m_Schmids[gname];
      sf2 = m_Schmids[gname2];
      sfmm = sf / sf2;
      ss1 = m_SlipSystems[gname];
      //	  ss2 = m_SlipSystems[gname2];
      for (int j = 0; j < 5; j++)
      {
        q1[j] = m_AvgQuats[5 * gname + j];
        q2[j] = m_AvgQuats[5 * gname2 + j];
      }
      OrientationMath::getSlipMisalignment(ss1, q1, q2, ssap);
      if(crystruct->GetValue(m_FieldPhases[gname]) == crystruct->GetValue(m_FieldPhases[gname2])
          && m_FieldPhases[gname] > 0)
      {
        w = m_OrientationOps[crystruct->GetValue(m_FieldPhases[gname])]->getMisoQuat(q1, q2, n1, n2, n3);
        w = w *(180.0f/m_pi);
      }
      else
      {
        w = 0;
      }
      kmbin = int(km / 0.2);
      gambin = int(gam / 0.8);
      gbbin = int(gbdist);
      tjbin = int(tjdist);
      qpbin = int(qpdist);
      sfbin = int((sf - 0.25) / 0.025);
      if(sfmm >= 1) sfmmbin = int((sfmm - 1.0) / 0.2) + 5;
      if(sfmm < 1) sfmmbin = 4 - int(((1.0 / sfmm) - 1.0) / 0.2);
      ssapbin = int((ssap - 0.4) / 0.06);
      disbin = int((w) / 10.0);
      if(kmbin < 0) kmbin = 0;
      if(kmbin > 24) kmbin = 24;
      if(gambin < 0) gambin = 0;
      if(gambin > 24) gambin = 24;
      if(gbbin < 0) gbbin = 0;
      if(gbbin > 9) gbbin = 9;
      if(tjbin < 0) tjbin = 0;
      if(tjbin > 9) tjbin = 9;
      if(qpbin < 0) qpbin = 0;
      if(qpbin > 9) qpbin = 9;
      if(sfbin < 0) sfbin = 0;
      if(sfbin > 9) sfbin = 9;
      if(sfmmbin < 0) sfmmbin = 0;
      if(sfmmbin > 9) sfmmbin = 9;
      if(ssapbin < 0) ssapbin = 0;
      if(ssapbin > 9) ssapbin = 9;
      if(disbin < 0) disbin = 0;
      if(disbin > 9) disbin = 9;
      kmdist[kmbin]++;
      gamdist[gambin]++;
      kmvgb[gbbin][0]++;
      kmvgb[gbbin][1] = kmvgb[gbbin][1] + km;
      gamvgb[gbbin][0]++;
      gamvgb[gbbin][1] = gamvgb[gbbin][1] + gam;
      kmvtj[tjbin][0]++;
      kmvtj[tjbin][1] = kmvtj[tjbin][1] + km;
      gamvtj[tjbin][0]++;
      gamvtj[tjbin][1] = gamvtj[tjbin][1] + gam;
      kmvqp[qpbin][0]++;
      kmvqp[qpbin][1] = kmvqp[qpbin][1] + km;
      gamvqp[qpbin][0]++;
      gamvqp[qpbin][1] = gamvqp[qpbin][1] + gam;
      distance = int(m_NearestNeighborDistances[i * 3 + 0]);
      if(distance > 9) distance = 9;
      if(distance <= 5)
      {
        kmvsf[sfbin][0]++;
        kmvsf[sfbin][1] = kmvsf[sfbin][1] + km;
        gamvsf[sfbin][0]++;
        gamvsf[sfbin][1] = gamvsf[sfbin][1] + gam;
        kmvsfmm[sfmmbin][0]++;
        kmvsfmm[sfmmbin][1] = kmvsfmm[sfmmbin][1] + km;
        gamvsfmm[sfmmbin][0]++;
        gamvsfmm[sfmmbin][1] = gamvsfmm[sfmmbin][1] + gam;
        kmvssap[ssapbin][0]++;
        kmvssap[ssapbin][1] = kmvssap[ssapbin][1] + km;
        gamvssap[ssapbin][0]++;
        gamvssap[ssapbin][1] = gamvssap[ssapbin][1] + gam;
        kmvdis[disbin][0]++;
        kmvdis[disbin][1] = kmvdis[disbin][1] + km;
        gamvdis[disbin][0]++;
        gamvdis[disbin][1] = gamvdis[disbin][1] + gam;
      }
      kmvsfdistthresh[distance][sfbin][0]++;
      kmvsfdistthresh[distance][sfbin][1] = kmvsfdistthresh[distance][sfbin][1] + km;
      gamvsfdistthresh[distance][sfbin][0]++;
      gamvsfdistthresh[distance][sfbin][1] = gamvsfdistthresh[distance][sfbin][1] + gam;
      kmvsfmmdistthresh[distance][sfmmbin][0]++;
      kmvsfmmdistthresh[distance][sfmmbin][1] = kmvsfmmdistthresh[distance][sfmmbin][1] + km;
      gamvsfmmdistthresh[distance][sfmmbin][0]++;
      gamvsfmmdistthresh[distance][sfmmbin][1] = gamvsfmmdistthresh[distance][sfmmbin][1] + gam;
      kmvssapdistthresh[distance][ssapbin][0]++;
      kmvssapdistthresh[distance][ssapbin][1] = kmvssapdistthresh[distance][ssapbin][1] + km;
      gamvssapdistthresh[distance][ssapbin][0]++;
      gamvssapdistthresh[distance][ssapbin][1] = gamvssapdistthresh[distance][ssapbin][1] + gam;
      kmvdisdistthresh[distance][disbin][0]++;
      kmvdisdistthresh[distance][disbin][1] = kmvdisdistthresh[distance][disbin][1] + km;
      gamvdisdistthresh[distance][disbin][0]++;
      gamvdisdistthresh[distance][disbin][1] = gamvdisdistthresh[distance][disbin][1] + gam;
      kmvsfmmssapthresh[sfmmbin][ssapbin][0]++;
      kmvsfmmssapthresh[sfmmbin][ssapbin][1] = kmvsfmmssapthresh[sfmmbin][ssapbin][1] + km;
      gamvsfmmssapthresh[sfmmbin][ssapbin][0]++;
      gamvsfmmssapthresh[sfmmbin][ssapbin][1] = gamvsfmmssapthresh[sfmmbin][ssapbin][1] + gam;
    }
  }
  outFile << "Kernel Misorientation Data" << std::endl;
  outFile << "GB		TJ		QP		SF		SFMM		SSAP		DIS" << std::endl;
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
        << kmvdis[i][0] << "	" << kmvdis[i][1] << std::endl;
  }
  outFile << std::endl;
  outFile << std::endl;
  outFile << "Grain Average Misorientation Data" << std::endl;
  outFile << "GB		TJ		QP		SF		SFMM		SSAP		DIS" << std::endl;
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
        << gamvdis[i][0] << "	" << gamvdis[i][1] << std::endl;
  }
  outFile << std::endl;
  outFile << std::endl;
  outFile << "Thresheld Data" << std::endl;
  outFile << "SF KAM" << std::endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << std::endl;
  for (int i = 0; i < 10; i++)
  {
      for (int j = 0; j < 10; j++)
      {
        if (kmvsfdistthresh[j][i][0] > 0) kmvsfdistthresh[j][i][1] = kmvsfdistthresh[j][i][1] / kmvsfdistthresh[j][i][0];
        outFile << kmvsfdistthresh[j][i][0] << "	" << kmvsfdistthresh[j][i][1] << "	";
      }
      outFile << std::endl;
  }
  outFile << "SFMM KAM" << std::endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << std::endl;
  for (int i = 0; i < 10; i++)
  {
      for (int j = 0; j < 10; j++)
      {
        if (kmvsfmmdistthresh[j][i][0] > 0) kmvsfmmdistthresh[j][i][1] = kmvsfmmdistthresh[j][i][1] / kmvsfmmdistthresh[j][i][0];
        outFile << kmvsfmmdistthresh[j][i][0] << "	" << kmvsfmmdistthresh[j][i][1] << "	";
      }
      outFile << std::endl;
  }
  outFile << "SSAP KAM" << std::endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << std::endl;
  for (int i = 0; i < 10; i++)
  {
      for (int j = 0; j < 10; j++)
      {
        if (kmvssapdistthresh[j][i][0] > 0) kmvssapdistthresh[j][i][1] = kmvssapdistthresh[j][i][1] / kmvssapdistthresh[j][i][0];
        outFile << kmvssapdistthresh[j][i][0] << "	" << kmvssapdistthresh[j][i][1] << "	";
      }
      outFile << std::endl;
  }
  outFile << "DIS KAM" << std::endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << std::endl;
  for (int i = 0; i < 10; i++)
  {
      for (int j = 0; j < 10; j++)
      {
        if (kmvdisdistthresh[j][i][0] > 0) kmvdisdistthresh[j][i][1] = kmvdisdistthresh[j][i][1] / kmvdisdistthresh[j][i][0];
        outFile << kmvdisdistthresh[j][i][0] << "	" << kmvdisdistthresh[j][i][1] << "	";
      }
      outFile << std::endl;
  }
  outFile << "SF GAM" << std::endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << std::endl;
  for (int i = 0; i < 10; i++)
  {
      for (int j = 0; j < 10; j++)
      {
        if (gamvsfdistthresh[j][i][0] > 0) gamvsfdistthresh[j][i][1] = gamvsfdistthresh[j][i][1] / gamvsfdistthresh[j][i][0];
        outFile << gamvsfdistthresh[j][i][0] << "	" << gamvsfdistthresh[j][i][1] << "	";
      }
      outFile << std::endl;
  }
  outFile << "SFMM GAM" << std::endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << std::endl;
  for (int i = 0; i < 10; i++)
  {
      for (int j = 0; j < 10; j++)
      {
        if (gamvsfmmdistthresh[j][i][0] > 0) gamvsfmmdistthresh[j][i][1] = gamvsfmmdistthresh[j][i][1] / gamvsfmmdistthresh[j][i][0];
        outFile << gamvsfmmdistthresh[j][i][0] << "	" << gamvsfmmdistthresh[j][i][1] << "	";
      }
      outFile << std::endl;
  }
  outFile << "SSAP GAM" << std::endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << std::endl;
  for (int i = 0; i < 10; i++)
  {
      for (int j = 0; j < 10; j++)
      {
        if (gamvssapdistthresh[j][i][0] > 0) gamvssapdistthresh[j][i][1] = gamvssapdistthresh[j][i][1] / gamvssapdistthresh[j][i][0];
        outFile << gamvssapdistthresh[j][i][0] << "	" << gamvssapdistthresh[j][i][1] << "	";
      }
      outFile << std::endl;
  }
  outFile << "DIS GAM" << std::endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << std::endl;
  for (int i = 0; i < 10; i++)
  {
      for (int j = 0; j < 10; j++)
      {
        if (gamvdisdistthresh[j][i][0] > 0) gamvdisdistthresh[j][i][1] = gamvdisdistthresh[j][i][1] / gamvdisdistthresh[j][i][0];
        outFile << gamvdisdistthresh[j][i][0] << "	" << gamvdisdistthresh[j][i][1] << "	";
      }
      outFile << std::endl;
  }
  outFile << "SFMM SSAP KAM" << std::endl;
  outFile << "0.43		0.49		0.55		0.61		0.67		0.73		0.79		0.85		0.91		0.97" << std::endl;
  for (int i = 0; i < 10; i++)
  {
      for (int j = 0; j < 10; j++)
      {
        if (kmvsfmmssapthresh[i][j][0] > 0) kmvsfmmssapthresh[i][j][1] = kmvsfmmssapthresh[i][j][1] / kmvsfmmssapthresh[i][j][0];
        outFile << kmvsfmmssapthresh[i][j][0] << "	" << kmvsfmmssapthresh[i][j][1] << "	";
      }
      outFile << std::endl;
  }
  outFile << "SFMM SSAP GAM" << std::endl;
  outFile << "0.43		0.49		0.55		0.61		0.67		0.73		0.79		0.85		0.91		0.97" << std::endl;
  for (int i = 0; i < 10; i++)
  {
      for (int j = 0; j < 10; j++)
      {
        if (gamvsfmmssapthresh[i][j][0] > 0) gamvsfmmssapthresh[i][j][1] = gamvsfmmssapthresh[i][j][1] / gamvsfmmssapthresh[i][j][0];
        outFile << gamvsfmmssapthresh[i][j][0] << "	" << gamvsfmmssapthresh[i][j][1] << "	";
      }
      outFile << std::endl;
  }
  outFile << "KAM DIST		GAM DIST" << std::endl;
  for (int i = 0; i < 25; i++)
  {
        outFile << float(i)*0.2+0.1 << "	" << kmdist[i] << "	" << float(i)*0.8+0.4 << "	" << gamdist[i] << std::endl;
  }
  outFile.close();

 // std::string filename2 = m_OutputFile2;
  FILE* vtkFile = NULL;
  vtkFile = fopen(m_VtkOutputFile.c_str(), "wb");
  if (NULL == vtkFile)
  {
    std::cout << "Error Creating VTK Visualization File '" << m_VtkOutputFile << "'" << std::endl;
    return;
  }
  fprintf(vtkFile, "# vtk DataFile Version 2.0\n");
  fprintf(vtkFile,  "DREAM3D Generated Data Set: Deformation Statistics\n");
  fprintf(vtkFile,  "ASCII\n");
  fprintf(vtkFile,  "DATASET UNSTRUCTURED_GRID\n");
  fprintf(vtkFile,  "POINTS %ld float\n", m->getNumFieldTuples()-1);


  size_t size = m->getNumFieldTuples();

  for(size_t i=1;i<size;i++)
  {
        float x = static_cast<float>( m_Poles[3*i] - (m_Poles[3*i] * (m_Poles[3*i+2] / (m_Poles[3*i+2] + 1.0))) );
        float y = static_cast<float>( m_Poles[3*i+1] - (m_Poles[3*i+1] * (m_Poles[3*i+2] / (m_Poles[3*i+2] + 1.0))) );
        float z = 0.0;
        fprintf(vtkFile, "%f %f %f\n", x, y, z);
  }

  fprintf(vtkFile, "CELLS %ld %ld\n", m->getNumFieldTuples()-1, ((m->getNumFieldTuples()-1)*2));
//  Store the Grain Ids so we don't have to re-read the triangles file again
  for(size_t i=1;i<size;i++)
  {
       fprintf(vtkFile, "1 %ld\n", (i-1));
  }

  // Write the CELL_TYPES into the file
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "CELL_TYPES %ld\n", m->getNumFieldTuples()-1);
  for(size_t i=1;i<size;i++)
  {
    fprintf(vtkFile, "1\n");
  }


  // Write the GrainId Data to teh file
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "CELL_DATA %ld\n", m->getNumFieldTuples()-1);
  fprintf(vtkFile, "SCALARS Misorientation float\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");
  for (size_t i = 1; i < size; i++)
  {
      float miso = m_GrainAvgMisorientations[i];
      fprintf(vtkFile, "%f\n", miso);
  }
  fclose(vtkFile);

 notifyStatusMessage("Completed");
}
