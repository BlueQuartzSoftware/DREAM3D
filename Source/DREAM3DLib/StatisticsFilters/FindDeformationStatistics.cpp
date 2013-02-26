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
m_GrainAvgMisorientations(NULL),
m_Poles(NULL),
m_Schmids(NULL),
m_GBEuclideanDistances(NULL),
m_TJEuclideanDistances(NULL),
m_QPEuclideanDistances(NULL)
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
  GET_PREREQ_DATA(m, DREAM3D, CellData, GBEuclideanDistances, ss, -300, float, FloatArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, TJEuclideanDistances, ss, -300, float, FloatArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, QPEuclideanDistances, ss, -300, float, FloatArrayType, voxels, 1)

  GET_PREREQ_DATA(m, DREAM3D, FieldData, Schmids, ss, -305, float, FloatArrayType, fields, 1)
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
  float kam, gbdist, tjdist, qpdist, sf, sf2, sfmm, grm, mprime;
  int gname, gname2;
  float q1[5], q2[5];
  int kmdist[25];
  int gamdist[25];
  float kmvgb[25][2];
  float gamvgb[25][2];
  float kmvtj[25][2];
  float gamvtj[25][2];
  float kmvqp[25][2];
  float gamvqp[25][2];
  float kmvsf[25][2];
  float gamvsf[25][2];
  float kmvsfmm[25][2];
  float gamvsfmm[25][2];
  float kmvmprime[25][2];
  float gamvmprime[25][2];
  float kmvdis[25][2];
  float gamvdis[25][2];
  float kmvsfdistthresh[25][10][2];
  float gamvsfdistthresh[25][10][2];
  float kmvsfmmdistthresh[25][10][2];
  float gamvsfmmdistthresh[25][10][2];
  float kmvmprimedistthresh[25][10][2];
  float gamvmprimedistthresh[25][10][2];
  float kmvdisdistthresh[25][10][2];
  float gamvdisdistthresh[25][10][2];
  float kmvsfmmmprimethresh[25][10][2];
  float gamvsfmmmprimethresh[25][10][2];
  int kambin, grmbin;
  int gbbin, tjbin, qpbin;
  int sfbin, mprimebin, sfmmbin, disbin;
 // int actualpoints = 0;
  for (int h = 0; h < 25; h++)
  {
    kmdist[h] = 0;
    gamdist[h] = 0;
  }
  for (int h = 0; h < 10; h++)
  {
      for (int i = 0; i < 25; i++)
      {
        for (int j = 0; j < 2; j++)
        {
          kmvsfmmmprimethresh[h][i][j] = 0;
          gamvsfmmmprimethresh[h][i][j] = 0;
          kmvsfdistthresh[h][i][j] = 0;
          gamvsfdistthresh[h][i][j] = 0;
          kmvmprimedistthresh[h][i][j] = 0;
          gamvmprimedistthresh[h][i][j] = 0;
          kmvmprimedistthresh[h][i][j] = 0;
          gamvmprimedistthresh[h][i][j] = 0;
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
              kmvmprime[i][j] = 0;
              gamvmprime[i][j] = 0;
              kmvdis[i][j] = 0;
              gamvdis[i][j] = 0;
          }
        }
    }
  }

  float avgKAM = 0;
  float avgGRM = 0;
  float avgGBdist = 0;
  float avgTJdist = 0;
  float avgQPdist = 0;
  float avgSF = 0;
  float avgSFMM = 0;
  float avgmprime = 0;
  float avgDIS = 0;
  double counter = 0;
  float LD[3];
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
      gname2 = m_NearestNeighbors[i * 3 + 0];
    avgSFMM = avgSFMM + (m_Schmids[gname]/m_Schmids[gname2]);
      for (int j = 0; j < 5; j++)
      {
        q1[j] = m_AvgQuats[5 * gname + j];
        q2[j] = m_AvgQuats[5 * gname2 + j];
      }
      if(crystruct->GetValue(m_FieldPhases[gname]) == crystruct->GetValue(m_FieldPhases[gname2])
          && m_FieldPhases[gname] > 0)
      {
        w = m_OrientationOps[crystruct->GetValue(m_FieldPhases[gname])]->getMisoQuat(q1, q2, n1, n2, n3);
        w = w *(180.0f/m_pi);
    m_OrientationOps[crystruct->GetValue(m_FieldPhases[gname])]->getmPrime(q1, q2, LD, mprime);
      }
      else
      {
        w = 0;
    mprime = 0;
      }
    avgmprime = avgmprime + mprime;
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
  avgSFMM = avgSFMM/counter;
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
      sf2 = m_Schmids[gname2];
      sfmm = sf / sf2;
      for (int j = 0; j < 5; j++)
      {
        q1[j] = m_AvgQuats[5 * gname + j];
        q2[j] = m_AvgQuats[5 * gname2 + j];
      }
      if(crystruct->GetValue(m_FieldPhases[gname]) == crystruct->GetValue(m_FieldPhases[gname2])
          && m_FieldPhases[gname] > 0)
      {
        w = m_OrientationOps[crystruct->GetValue(m_FieldPhases[gname])]->getMisoQuat(q1, q2, n1, n2, n3);
        w = w *(180.0f/m_pi);
        m_OrientationOps[crystruct->GetValue(m_FieldPhases[gname])]->getmPrime(q1, q2, LD, mprime);
      }
      else
      {
        w = 0;
    mprime = 0;
      }
      kambin = int((kam/avgKAM)/0.1);
      grmbin = int((grm/avgGRM)/0.1);
      gbbin = int(gbdist);
      tjbin = int(tjdist);
      qpbin = int(qpdist);
      sfbin = int((sf - 0.25) / 0.025);
      if(sfmm >= 1) sfmmbin = int((sfmm - 1.0) / 0.2) + 5;
      if(sfmm < 1) sfmmbin = 4 - int(((1.0 / sfmm) - 1.0) / 0.2);
      mprimebin = int((mprime - 0.4) / 0.06);
      disbin = int((w) / 10.0);
      if(kambin < 0) kambin = 0;
      if(kambin > 24) kambin = 24;
      if(grmbin < 0) grmbin = 0;
      if(grmbin > 24) grmbin = 24;
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
      if(mprimebin < 0) mprimebin = 0;
      if(mprimebin > 9) mprimebin = 9;
      if(disbin < 0) disbin = 0;
      if(disbin > 9) disbin = 9;
      kmdist[kambin]++;
      gamdist[grmbin]++;
      kmvgb[kambin][0]++;
      kmvgb[kambin][1] = kmvgb[kambin][1] + (gbdist/avgGBdist);
      gamvgb[grmbin][0]++;
      gamvgb[grmbin][1] = gamvgb[grmbin][1] + (gbdist/avgGBdist);
      kmvtj[kambin][0]++;
      kmvtj[kambin][1] = kmvtj[kambin][1] + (tjdist/avgTJdist);
      gamvtj[grmbin][0]++;
      gamvtj[grmbin][1] = gamvtj[grmbin][1] + (tjdist/avgTJdist);
      kmvqp[kambin][0]++;
      kmvqp[kambin][1] = kmvqp[kambin][1] + (qpdist/avgQPdist);
      gamvqp[grmbin][0]++;
      gamvqp[grmbin][1] = gamvqp[grmbin][1] + (qpdist/avgQPdist);
      distance = int(m_GBEuclideanDistances[i]);
      if(distance > 9) distance = 9;
      if(distance <= 3)
      {
        kmvsf[kambin][0]++;
        kmvsf[kambin][1] = kmvsf[kambin][1] + (sf/avgSF);
        gamvsf[grmbin][0]++;
        gamvsf[grmbin][1] = gamvsf[grmbin][1] + (sf/avgSF);
        kmvsfmm[kambin][0]++;
        kmvsfmm[kambin][1] = kmvsfmm[kambin][1] + (sfmm/avgSFMM);
        gamvsfmm[grmbin][0]++;
        gamvsfmm[grmbin][1] = gamvsfmm[grmbin][1] + (sfmm/avgSFMM);
        kmvmprime[kambin][0]++;
        kmvmprime[kambin][1] = kmvmprime[kambin][1] + (mprime/avgmprime);
        gamvmprime[grmbin][0]++;
        gamvmprime[grmbin][1] = gamvmprime[grmbin][1] + (mprime/avgmprime);
        kmvdis[kambin][0]++;
        kmvdis[kambin][1] = kmvdis[kambin][1] + (w/avgDIS);
        gamvdis[grmbin][0]++;
        gamvdis[grmbin][1] = gamvdis[grmbin][1] + (w/avgDIS);
      }
      kmvsfdistthresh[distance][sfbin][0]++;
      kmvsfdistthresh[distance][sfbin][1] = kmvsfdistthresh[distance][sfbin][1] + kam;
      gamvsfdistthresh[distance][sfbin][0]++;
      gamvsfdistthresh[distance][sfbin][1] = gamvsfdistthresh[distance][sfbin][1] + grm;
      kmvsfmmdistthresh[distance][sfmmbin][0]++;
      kmvsfmmdistthresh[distance][sfmmbin][1] = kmvsfmmdistthresh[distance][sfmmbin][1] + kam;
      gamvsfmmdistthresh[distance][sfmmbin][0]++;
      gamvsfmmdistthresh[distance][sfmmbin][1] = gamvsfmmdistthresh[distance][sfmmbin][1] + grm;
      kmvmprimedistthresh[distance][mprimebin][0]++;
      kmvmprimedistthresh[distance][mprimebin][1] = kmvmprimedistthresh[distance][mprimebin][1] + kam;
      gamvmprimedistthresh[distance][mprimebin][0]++;
      gamvmprimedistthresh[distance][mprimebin][1] = gamvmprimedistthresh[distance][mprimebin][1] + grm;
      kmvdisdistthresh[distance][disbin][0]++;
      kmvdisdistthresh[distance][disbin][1] = kmvdisdistthresh[distance][disbin][1] + kam;
      gamvdisdistthresh[distance][disbin][0]++;
      gamvdisdistthresh[distance][disbin][1] = gamvdisdistthresh[distance][disbin][1] + grm;
      kmvsfmmmprimethresh[sfmmbin][mprimebin][0]++;
      kmvsfmmmprimethresh[sfmmbin][mprimebin][1] = kmvsfmmmprimethresh[sfmmbin][mprimebin][1] + kam;
      gamvsfmmmprimethresh[sfmmbin][mprimebin][0]++;
      gamvsfmmmprimethresh[sfmmbin][mprimebin][1] = gamvsfmmmprimethresh[sfmmbin][mprimebin][1] + grm;
    }
  }
  outFile << "Kernel Misorientation Data" << std::endl;
  outFile << "GB		TJ		QP		SF		SFMM		mprime		DIS" << std::endl;
  for (int i = 0; i < 25; i++)
  {
    if (kmvgb[i][0] > 0) kmvgb[i][1] = kmvgb[i][1] / kmvgb[i][0];
    if (kmvtj[i][0] > 0) kmvtj[i][1] = kmvtj[i][1] / kmvtj[i][0];
    if (kmvqp[i][0] > 0) kmvqp[i][1] = kmvqp[i][1] / kmvqp[i][0];
    if (kmvsf[i][0] > 0) kmvsf[i][1] = kmvsf[i][1] / kmvsf[i][0];
    if (kmvsfmm[i][0] > 0) kmvsfmm[i][1] = kmvsfmm[i][1] / kmvsfmm[i][0];
    if (kmvmprime[i][0] > 0) kmvmprime[i][1] = kmvmprime[i][1] / kmvmprime[i][0];
    if (kmvdis[i][0] > 0) kmvdis[i][1] = kmvdis[i][1] / kmvdis[i][0];
    outFile << kmvgb[i][0] << "	" << kmvgb[i][1] << "	"
        << kmvtj[i][0] << "	" << kmvtj[i][1] << "	"
        << kmvqp[i][0] << "	" << kmvqp[i][1] << "	"
        << kmvsf[i][0] << "	" << kmvsf[i][1] << "	"
        << kmvsfmm[i][0] << "	" << kmvsfmm[i][1] << "	"
        << kmvmprime[i][0] << "	" << kmvmprime[i][1] << "	"
        << kmvdis[i][0] << "	" << kmvdis[i][1] << std::endl;
  }
  outFile << std::endl;
  outFile << std::endl;
  outFile << "Grain Average Misorientation Data" << std::endl;
  outFile << "GB		TJ		QP		SF		SFMM		mprime		DIS" << std::endl;
  for (int i = 0; i < 25; i++)
  {
    if (gamvgb[i][0] > 0) gamvgb[i][1] = gamvgb[i][1] / gamvgb[i][0];
    if (gamvtj[i][0] > 0) gamvtj[i][1] = gamvtj[i][1] / gamvtj[i][0];
    if (gamvqp[i][0] > 0) gamvqp[i][1] = gamvqp[i][1] / gamvqp[i][0];
    if (gamvsf[i][0] > 0) gamvsf[i][1] = gamvsf[i][1] / gamvsf[i][0];
    if (gamvsfmm[i][0] > 0) gamvsfmm[i][1] = gamvsfmm[i][1] / gamvsfmm[i][0];
    if (gamvmprime[i][0] > 0) gamvmprime[i][1] = gamvmprime[i][1] / gamvmprime[i][0];
    if (gamvdis[i][0] > 0) gamvdis[i][1] = gamvdis[i][1] / gamvdis[i][0];
    outFile << gamvgb[i][0] << "	" << gamvgb[i][1] << "	"
        << gamvtj[i][0] << "	" << gamvtj[i][1] << "	"
        << gamvqp[i][0] << "	" << gamvqp[i][1] << "	"
        << gamvsf[i][0] << "	" << gamvsf[i][1] << "	"
        << gamvsfmm[i][0] << "	" << gamvsfmm[i][1] << "	"
        << gamvmprime[i][0] << "	" << gamvmprime[i][1] << "	"
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
  outFile << "mprime KAM" << std::endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << std::endl;
  for (int i = 0; i < 10; i++)
  {
      for (int j = 0; j < 10; j++)
      {
        if (kmvmprimedistthresh[j][i][0] > 0) kmvmprimedistthresh[j][i][1] = kmvmprimedistthresh[j][i][1] / kmvmprimedistthresh[j][i][0];
        outFile << kmvmprimedistthresh[j][i][0] << "	" << kmvmprimedistthresh[j][i][1] << "	";
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
  outFile << "mprime GAM" << std::endl;
  outFile << "0		1		2		3		4		5		6		7		8		9" << std::endl;
  for (int i = 0; i < 10; i++)
  {
      for (int j = 0; j < 10; j++)
      {
        if (gamvmprimedistthresh[j][i][0] > 0) gamvmprimedistthresh[j][i][1] = gamvmprimedistthresh[j][i][1] / gamvmprimedistthresh[j][i][0];
        outFile << gamvmprimedistthresh[j][i][0] << "	" << gamvmprimedistthresh[j][i][1] << "	";
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
  outFile << "SFMM mprime KAM" << std::endl;
  outFile << "0.43		0.49		0.55		0.61		0.67		0.73		0.79		0.85		0.91		0.97" << std::endl;
  for (int i = 0; i < 10; i++)
  {
      for (int j = 0; j < 10; j++)
      {
        if (kmvsfmmmprimethresh[i][j][0] > 0) kmvsfmmmprimethresh[i][j][1] = kmvsfmmmprimethresh[i][j][1] / kmvsfmmmprimethresh[i][j][0];
        outFile << kmvsfmmmprimethresh[i][j][0] << "	" << kmvsfmmmprimethresh[i][j][1] << "	";
      }
      outFile << std::endl;
  }
  outFile << "SFMM mprime GAM" << std::endl;
  outFile << "0.43		0.49		0.55		0.61		0.67		0.73		0.79		0.85		0.91		0.97" << std::endl;
  for (int i = 0; i < 10; i++)
  {
      for (int j = 0; j < 10; j++)
      {
        if (gamvsfmmmprimethresh[i][j][0] > 0) gamvsfmmmprimethresh[i][j][1] = gamvsfmmmprimethresh[i][j][1] / gamvsfmmmprimethresh[i][j][0];
        outFile << gamvsfmmmprimethresh[i][j][0] << "	" << gamvsfmmmprimethresh[i][j][1] << "	";
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
