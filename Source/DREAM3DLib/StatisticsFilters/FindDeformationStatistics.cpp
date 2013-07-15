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
m_F1ArrayName(DREAM3D::FieldData::F1),
m_F1sptArrayName(DREAM3D::FieldData::F1spt),
m_F7ArrayName(DREAM3D::FieldData::F7),
m_mPrimeArrayName(DREAM3D::FieldData::mPrime),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_GrainReferenceMisorientationsArrayName(DREAM3D::CellData::GrainReferenceMisorientations),
m_KernelAverageMisorientationsArrayName(DREAM3D::CellData::KernelAverageMisorientations),
m_GBEuclideanDistancesArrayName(DREAM3D::CellData::GBEuclideanDistances),
m_TJEuclideanDistancesArrayName(DREAM3D::CellData::TJEuclideanDistances),
m_QPEuclideanDistancesArrayName(DREAM3D::CellData::QPEuclideanDistances),
m_NearestNeighborsArrayName(DREAM3D::CellData::NearestNeighbors),
m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
m_GrainAvgMisorientationsArrayName(DREAM3D::FieldData::GrainAvgMisorientations),
m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
m_PolesArrayName(DREAM3D::FieldData::Poles),
m_SchmidsArrayName(DREAM3D::FieldData::Schmids),
m_GrainIds(NULL),
m_FieldPhases(NULL),
m_NearestNeighbors(NULL),
m_GrainReferenceMisorientations(NULL),
m_KernelAverageMisorientations(NULL),
m_AvgQuats(NULL),
m_F1(NULL),
m_F1spt(NULL),
m_F7(NULL),
m_mPrime(NULL),
m_GrainAvgMisorientations(NULL),
m_Poles(NULL),
m_Schmids(NULL),
m_GBEuclideanDistances(NULL),
m_TJEuclideanDistances(NULL),
m_QPEuclideanDistances(NULL),
m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationMath::getOrientationOpsVector();
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
void FindDeformationStatistics::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindDeformationStatistics::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(index);
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

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, KernelAverageMisorientations, ss, -300, float, FloatArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainReferenceMisorientations, ss, -300, float, FloatArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, NearestNeighbors, ss, -300, int32_t, Int32ArrayType, voxels, 3)
  GET_PREREQ_DATA(m, DREAM3D, CellData, GBEuclideanDistances, ss, -300, float, FloatArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, TJEuclideanDistances, ss, -300, float, FloatArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, QPEuclideanDistances, ss, -300, float, FloatArrayType, voxels, 1)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, Schmids, ss, -305, float, FloatArrayType, fields, 1)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -301, float, FloatArrayType, fields, 5)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -302, int32_t, Int32ArrayType, fields, 1)


  GET_PREREQ_DATA(m, DREAM3D, FieldData, Poles, ss, -306, int32_t, Int32ArrayType, fields, 3)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, GrainAvgMisorientations, ss, -306, float, FloatArrayType, fields, 1)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, F1, ss, -307, float, FloatArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, F1spt, ss, -308, float, FloatArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, F7, ss, -309, float, FloatArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, mPrime, ss, -310, float, FloatArrayType, voxels, 1)

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -305, unsigned int, XTalStructArrayType, ensembles, 1)
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

//  std::string filename = m_OutputFile1;
  std::ofstream outFile;
  outFile.open(m_DeformationStatisticsFile.c_str(), std::ios_base::binary);
  float w, n1, n2, n3;
  int distance;
  float kam, gbdist, tjdist, qpdist, sf, grm, mprime, F1, F1spt, F7;
  int gname, gname2;
  float q1[5], q2[5];
  int kmdist[20];
  int gamdist[20];
  float kmvgb[20][2];
  float gamvgb[20][2];
  float kmvtj[20][2];
  float gamvtj[20][2];
  float kmvqp[20][2];
  float gamvqp[20][2];
  float kmvsf[20][2];
  float gamvsf[20][2];
  float kmvF1[20][2];
  float gamvF1[20][2];
  float kmvF1spt[20][2];
  float gamvF1spt[20][2];
  float kmvF7[20][2];
  float gamvF7[20][2];
  float kmvmprime[20][2];
  float gamvmprime[20][2];
  float kmvdis[20][2];
  float gamvdis[20][2];
//  float kmvsfdistthresh[20][2];
//  float gamvsfdistthresh[20][2];
//  float kmvF1distthresh[20][2];
//  float gamvF1distthresh[20][2];
//  float kmvF1sptdistthresh[20][2];
//  float gamvF1sptdistthresh[20][2];
//  float kmvF7distthresh[20][2];
//  float gamvF7distthresh[20][2];
//  float kmvmprimedistthresh[20][2];
//  float gamvmprimedistthresh[20][2];
//  float kmvdisdistthresh[20][2];
//  float gamvdisdistthresh[20][2];
//  float kmvsfmmmprimethresh[20][2];
//  float gamvsfmmmprimethresh[20][2];
  int kambin, grmbin, kambin2, grmbin2;
  int gbbin, tjbin, qpbin;
  int sfbin, mprimebin, F1bin, F1sptbin, F7bin, disbin;
 // int actualpoints = 0;
  for (int h = 0; h < 20; h++)
  {
    kmdist[h] = 0;
    gamdist[h] = 0;
  }
  for (int i = 0; i < 20; i++)
  {
     for (int j = 0; j < 2; j++)
     {
        kmvgb[i][j] = 0;
        gamvgb[i][j] = 0;
        kmvtj[i][j] = 0;
        gamvtj[i][j] = 0;
        kmvqp[i][j] = 0;
        gamvqp[i][j] = 0;
        kmvsf[i][j] = 0;
        gamvsf[i][j] = 0;
        kmvF1[i][j] = 0;
        gamvF1[i][j] = 0;
        kmvF1spt[i][j] = 0;
        gamvF1spt[i][j] = 0;
        kmvF7[i][j] = 0;
        gamvF7[i][j] = 0;
        kmvmprime[i][j] = 0;
        gamvmprime[i][j] = 0;
        kmvdis[i][j] = 0;
        gamvdis[i][j] = 0;
    }
  }

  float avgKAM = 0;
  float avgGRM = 0;
  float avgGBdist = 0;
  float avgTJdist = 0;
  float avgQPdist = 0;
  float avgSF = 0;
  float avgF1 = 0;
  float avgF1spt = 0;
  float avgF7 = 0;
  float avgmprime = 0;
  float avgDIS = 0;
  double counter = 0;
  float LD[3];
  LD[0] = 0;
  LD[1] = 0;
  LD[2] = 1;
  for (int i = 0; i < totalPoints; i++)
  {
    gname = m_GrainIds[i];
    if(gname > 0)
    {
      avgKAM = avgKAM + m_KernelAverageMisorientations[i];
      avgGRM = avgGRM + m_GrainReferenceMisorientations[i];
      avgGBdist = avgGBdist + m_GBEuclideanDistances[i];
      avgTJdist = avgTJdist + m_TJEuclideanDistances[i];
      avgQPdist = avgQPdist + m_QPEuclideanDistances[i];
      avgSF = avgSF + m_Schmids[gname];
      avgF1 = avgF1 + m_F1[i];
      avgF1spt = avgF1spt + m_F1spt[i];
      avgF7 = avgF7 + m_F7[i];
    avgmprime = avgmprime + m_mPrime[i];
      gname2 = m_NearestNeighbors[i * 3 + 0];
      for (int j = 0; j < 5; j++)
      {
        q1[j] = m_AvgQuats[5 * gname + j];
        q2[j] = m_AvgQuats[5 * gname2 + j];
      }
      if(m_CrystalStructures[m_FieldPhases[gname]] == m_CrystalStructures[m_FieldPhases[gname2]] && m_FieldPhases[gname] > 0)
      {
        w = m_OrientationOps[m_CrystalStructures[m_FieldPhases[gname]]]->getMisoQuat(q1, q2, n1, n2, n3);
        w = w *(180.0f/m_pi);
      }
      else
      {
        w = 0;
      }
      avgDIS = avgDIS + w;
      counter++;
    }
  }
  avgKAM = avgKAM/counter;
  avgGRM = avgGRM/counter;
  avgGBdist = avgGBdist/counter;
  avgTJdist = avgTJdist/counter;
  avgQPdist = avgQPdist/counter;
  avgSF = avgSF/counter;
  avgF1 = avgF1/counter;
  avgF1spt = avgF1spt/counter;
  avgF7 = avgF7/counter;
  avgmprime = avgmprime/counter;
  avgDIS = avgDIS/counter;
  for (int i = 0; i < totalPoints; i++)
  {
    gname = m_GrainIds[i];
    if(gname > 0)
    {
      kam = m_KernelAverageMisorientations[i];
      grm = m_GrainReferenceMisorientations[i];
      gbdist = m_GBEuclideanDistances[i];
      tjdist = m_TJEuclideanDistances[i];
      qpdist = m_QPEuclideanDistances[i];
      gname2 = m_NearestNeighbors[i * 3 + 0];
      sf = m_Schmids[gname];
    F1 = m_F1[i];
    F1spt = m_F1spt[i];
    F7 = m_F7[i];
  mprime = m_mPrime[i];
      for (int j = 0; j < 5; j++)
      {
        q1[j] = m_AvgQuats[5 * gname + j];
        q2[j] = m_AvgQuats[5 * gname2 + j];
      }
      if(m_CrystalStructures[m_FieldPhases[gname]] == m_CrystalStructures[m_FieldPhases[gname2]] && m_FieldPhases[gname] > 0)
      {
        w = m_OrientationOps[m_CrystalStructures[m_FieldPhases[gname]]]->getMisoQuat(q1, q2, n1, n2, n3);
        w = w *(180.0f/m_pi);
      }
      else
      {
        w = 0;
      }
      if((kam/avgKAM) >= 1) kambin = 10+int(((kam/avgKAM)-1.0)/0.25);
    else kambin = 9-int(((avgKAM/kam)-1.0)/0.25);
      if((grm/avgGRM) >= 1) grmbin = 10+int(((grm/avgGRM)-1.0)/0.25);
    else grmbin = 9-int(((avgGRM/grm)-1.0)/0.25);
    kambin2 = int(kam/0.25);
    grmbin2 = int(grm/0.5);
      gbbin = int(gbdist);
      tjbin = int(tjdist);
      qpbin = int(qpdist);
      if((sf/avgSF) >= 1) sfbin = 10+int(((sf/avgSF)-1.0)/0.25);
    else sfbin = 9-int(((avgSF/sf)-1.0)/0.25);
      if((w/avgDIS) >= 1) disbin = 10+int(((w/avgDIS)-1.0)/0.25);
    else disbin = 9-int(((avgDIS/w)-1.0)/0.25);
      if((mprime/avgmprime) >= 1) mprimebin = 10+int(((mprime/avgmprime)-1.0)/0.25);
    else mprimebin = 9-int(((avgmprime/mprime)-1.0)/0.25);
      if((F1/avgF1) >= 1) F1bin = 10+int(((F1/avgF1)-1.0)/0.25);
    else F1bin = 9-int(((avgF1/F1)-1.0)/0.25);
      if((F1spt/avgF1) >= 1) F1sptbin = 10+int(((F1spt/avgF1spt)-1.0)/0.25);
    else F1sptbin = 9-int(((avgF1spt/F1spt)-1.0)/0.25);
      if((F7/avgF7) >= 1) F7bin = 10+int(((F7/avgF7)-1.0)/0.25);
    else F7bin = 9-int(((avgF7/F7)-1.0)/0.25);
      if(kambin < 0) kambin = 0;
      if(kambin > 19) kambin = 19;
      if(grmbin < 0) grmbin = 0;
      if(grmbin > 19) grmbin = 19;
      if(gbbin < 0) gbbin = 0;
      if(gbbin > 19) gbbin = 19;
      if(tjbin < 0) tjbin = 0;
      if(tjbin > 19) tjbin = 19;
      if(qpbin < 0) qpbin = 0;
      if(qpbin > 19) qpbin = 19;
      if(sfbin < 0) sfbin = 0;
      if(sfbin > 19) sfbin = 19;
      if(F1bin < 0) F1bin = 0;
      if(F1bin > 19) F1bin = 19;
      if(F1sptbin < 0) F1sptbin = 0;
      if(F1sptbin > 19) F1sptbin = 19;
      if(F7bin < 0) F7bin = 0;
      if(F7bin > 19) F7bin = 19;
      if(mprimebin < 0) mprimebin = 0;
      if(mprimebin > 19) mprimebin = 19;
      if(disbin < 0) disbin = 0;
      if(disbin > 19) disbin = 19;
      kmdist[kambin2]++;
      gamdist[grmbin2]++;
      kmvgb[gbbin][0]++;
      kmvgb[gbbin][1] = kmvgb[gbbin][1] + (kam/avgKAM);
      gamvgb[gbbin][0]++;
      gamvgb[gbbin][1] = gamvgb[gbbin][1] + (grm/avgGRM);
      kmvtj[tjbin][0]++;
      kmvtj[tjbin][1] = kmvtj[tjbin][1] + (kam/avgKAM);
      gamvtj[tjbin][0]++;
      gamvtj[tjbin][1] = gamvtj[tjbin][1] + (grm/avgGRM);
      kmvqp[qpbin][0]++;
      kmvqp[qpbin][1] = kmvqp[qpbin][1] + (kam/avgKAM);
      gamvqp[qpbin][0]++;
      gamvqp[qpbin][1] = gamvqp[qpbin][1] + (grm/avgGRM);
      distance = int(m_GBEuclideanDistances[i]);
      if(distance > 19) distance = 19;
      if(distance <= 5)
      {
        kmvsf[kambin][0]++;
        kmvsf[kambin][1] = kmvsf[kambin][1] + (sf/avgSF);
        gamvsf[grmbin][0]++;
        gamvsf[grmbin][1] = gamvsf[grmbin][1] + (sf/avgSF);
        kmvF1[kambin][0]++;
        kmvF1[kambin][1] = kmvF1[kambin][1] + (F1/avgF1);
        gamvF1[grmbin][0]++;
        gamvF1[grmbin][1] = gamvF1[grmbin][1] + (F1/avgF1);
        kmvF1spt[kambin][0]++;
        kmvF1spt[kambin][1] = kmvF1spt[kambin][1] + (F1spt/avgF1spt);
        gamvF1spt[grmbin][0]++;
        gamvF1spt[grmbin][1] = gamvF1spt[grmbin][1] + (F1spt/avgF1spt);
        kmvF7[kambin][0]++;
        kmvF7[kambin][1] = kmvF7[kambin][1] + (F7/avgF7);
        gamvF7[grmbin][0]++;
        gamvF7[grmbin][1] = gamvF7[grmbin][1] + (F7/avgF7);
        kmvmprime[kambin][0]++;
        kmvmprime[kambin][1] = kmvmprime[kambin][1] + (mprime/avgmprime);
        gamvmprime[grmbin][0]++;
        gamvmprime[grmbin][1] = gamvmprime[grmbin][1] + (mprime/avgmprime);
        kmvdis[kambin][0]++;
        kmvdis[kambin][1] = kmvdis[kambin][1] + (w/avgDIS);
        gamvdis[grmbin][0]++;
        gamvdis[grmbin][1] = gamvdis[grmbin][1] + (w/avgDIS);
      }
    }
  }
  outFile << "Kernel Misorientation Data" << std::endl;
  outFile << "GB		TJ		QP		SF		F1		F1spt		F7		mprime		DIS" << std::endl;
  outFile << avgGBdist << "		" << avgTJdist << "		" << avgQPdist << "		" << avgSF << "		" << avgF1 << "		" << avgF1spt << "		" << avgF7 << "		" << avgmprime << "		" << avgDIS <<std::endl;
  for (int i = 0; i < 20; i++)
  {
    if (kmvgb[i][0] > 0) kmvgb[i][1] = kmvgb[i][1] / kmvgb[i][0];
    if (kmvtj[i][0] > 0) kmvtj[i][1] = kmvtj[i][1] / kmvtj[i][0];
    if (kmvqp[i][0] > 0) kmvqp[i][1] = kmvqp[i][1] / kmvqp[i][0];
    if (kmvsf[i][0] > 0) kmvsf[i][1] = kmvsf[i][1] / kmvsf[i][0];
    if (kmvF1[i][0] > 0) kmvF1[i][1] = kmvF1[i][1] / kmvF1[i][0];
    if (kmvF1spt[i][0] > 0) kmvF1spt[i][1] = kmvF1spt[i][1] / kmvF1spt[i][0];
    if (kmvF7[i][0] > 0) kmvF7[i][1] = kmvF7[i][1] / kmvF7[i][0];
    if (kmvmprime[i][0] > 0) kmvmprime[i][1] = kmvmprime[i][1] / kmvmprime[i][0];
    if (kmvdis[i][0] > 0) kmvdis[i][1] = kmvdis[i][1] / kmvdis[i][0];
    outFile << kmvgb[i][0] << "	" << kmvgb[i][1] << "	"
        << kmvtj[i][0] << "	" << kmvtj[i][1] << "	"
        << kmvqp[i][0] << "	" << kmvqp[i][1] << "	"
        << kmvsf[i][0] << "	" << kmvsf[i][1] << "	"
        << kmvF1[i][0] << "	" << kmvF1[i][1] << "	"
        << kmvF1spt[i][0] << "	" << kmvF1spt[i][1] << "	"
        << kmvF7[i][0] << "	" << kmvF7[i][1] << "	"
        << kmvmprime[i][0] << "	" << kmvmprime[i][1] << "	"
        << kmvdis[i][0] << "	" << kmvdis[i][1] << std::endl;
  }
  outFile << std::endl;
  outFile << std::endl;
  outFile << "Grain Average Misorientation Data" << std::endl;
  outFile << "GB		TJ		QP		SF		F1		F1spt		F7		mprime		DIS" << std::endl;
  outFile << avgGBdist << "		" << avgTJdist << "		" << avgQPdist << "		" << avgSF << "		" << avgF1 << "		" << avgF1spt << "		" << avgF7 << "		" << avgmprime << "		" << avgDIS <<std::endl;
  for (int i = 0; i < 20; i++)
  {
    if (gamvgb[i][0] > 0) gamvgb[i][1] = gamvgb[i][1] / gamvgb[i][0];
    if (gamvtj[i][0] > 0) gamvtj[i][1] = gamvtj[i][1] / gamvtj[i][0];
    if (gamvqp[i][0] > 0) gamvqp[i][1] = gamvqp[i][1] / gamvqp[i][0];
    if (gamvsf[i][0] > 0) gamvsf[i][1] = gamvsf[i][1] / gamvsf[i][0];
    if (gamvF1[i][0] > 0) gamvF1[i][1] = gamvF1[i][1] / gamvF1[i][0];
    if (gamvF1spt[i][0] > 0) gamvF1spt[i][1] = gamvF1spt[i][1] / gamvF1spt[i][0];
    if (gamvF7[i][0] > 0) gamvF7[i][1] = gamvF7[i][1] / gamvF7[i][0];
    if (gamvmprime[i][0] > 0) gamvmprime[i][1] = gamvmprime[i][1] / gamvmprime[i][0];
    if (gamvdis[i][0] > 0) gamvdis[i][1] = gamvdis[i][1] / gamvdis[i][0];
    outFile << gamvgb[i][0] << "	" << gamvgb[i][1] << "	"
        << gamvtj[i][0] << "	" << gamvtj[i][1] << "	"
        << gamvqp[i][0] << "	" << gamvqp[i][1] << "	"
        << gamvsf[i][0] << "	" << gamvsf[i][1] << "	"
        << gamvF1[i][0] << "	" << gamvF1[i][1] << "	"
        << gamvF1spt[i][0] << "	" << gamvF1spt[i][1] << "	"
        << gamvF7[i][0] << "	" << gamvF7[i][1] << "	"
        << gamvmprime[i][0] << "	" << gamvmprime[i][1] << "	"
        << gamvdis[i][0] << "	" << gamvdis[i][1] << std::endl;
  }
  outFile << std::endl;
  outFile << std::endl;
  outFile << "KAM DIST		GAM DIST" << std::endl;
  for (int i = 0; i < 20; i++)
  {
        outFile << float(i)*0.25+0.125 << "	" << kmdist[i] << "	" << float(i)*0.5+0.25 << "	" << gamdist[i] << std::endl;
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

  float x, y, z;
  float xtemp, ytemp, ztemp;
  float xFZ, yFZ, zFZ;
  for(size_t i=1;i<size;i++)
  {
    xtemp = m_Poles[3*i];
    ytemp = m_Poles[3*i+1];
    ztemp = m_Poles[3*i+2];

    if(ztemp<0) xtemp = -xtemp, ytemp = -ytemp, ztemp = -ztemp;
    if(xtemp >= ytemp && xtemp >= ztemp)
    {
      z = xtemp;
      if(ytemp >= ztemp) x = ytemp, y = ztemp;
      else x = ztemp, y = ytemp;
    }
    if(ytemp >= xtemp && ytemp >= ztemp)
    {
      z = ytemp;
      if(xtemp >= ztemp) x = xtemp, y = ztemp;
      else x = ztemp, y = xtemp;
    }
    if(ztemp >= xtemp && ztemp >= ytemp)
    {
      z = ztemp;
      if(xtemp >= ytemp) x = xtemp, y = ytemp;
      else x = ytemp, y = xtemp;
    }

    xFZ = static_cast<float>( x - (x * (z / (z + 1.0))) );
      yFZ = static_cast<float>( y - (y * (z / (z + 1.0))) );
      zFZ = 0.0;
      fprintf(vtkFile, "%f %f %f\n", xFZ, yFZ, zFZ);
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
