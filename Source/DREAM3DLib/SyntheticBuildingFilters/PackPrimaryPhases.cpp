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

#include "PackPrimaryPhases.h"


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range3d.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif




#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DataContainerMacros.h"
#include "DREAM3DLib/Common/MatrixMath.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"
#include "DREAM3DLib/Common/PrimaryStatsData.h"
#include "DREAM3DLib/ShapeOps/CubeOctohedronOps.h"
#include "DREAM3DLib/ShapeOps/CylinderOps.h"
#include "DREAM3DLib/ShapeOps/EllipsoidOps.h"
#include "DREAM3DLib/ShapeOps/SuperEllipsoidOps.h"
#include "DREAM3DLib/GenericFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/RenumberGrains.h"
#include "DREAM3DLib/IOFilters/FieldDataCSVWriter.h"
#include "DREAM3DLib/IOFilters/DataContainerWriter.h"

const static float m_pi = static_cast<float>(M_PI);


#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

#define GG_INIT_DOUBLE_ARRAY(array, value, size)\
  for(size_t n = 0; n < size; ++n) { array[n] = (value); }

#if (CMP_SIZEOF_SIZE_T == 4)
typedef int32_t DimType;
#else
typedef int64_t DimType;
#endif

/**
 * @brief
 */
class AssignVoxelsGapsImpl
{
    DimType dims[3];
    float radcur[3];
    float res[3];
    float size[3];
    int32_t* m_GrainIds;
    float xc;
    float yc;
    float zc;
    int32_t iteration;
    ShapeOps* m_ShapeOps;
    float ga[3][3];
    int curGrain;
    Int32ArrayType::Pointer newownersPtr;
    FloatArrayType::Pointer ellipfuncsPtr;

  public:
    AssignVoxelsGapsImpl(int32_t cycle, DimType* dimensions, float* resolution, int32_t* grainIds, float* radCur,
                         float* xx, ShapeOps* shapeOps, float gA[3][3], float* size, int cur_grain,
    Int32ArrayType::Pointer newowners, FloatArrayType::Pointer ellipfuncs) :
      m_GrainIds(grainIds),
      m_ShapeOps(shapeOps),
      curGrain(cur_grain)
    {
      dims[0] = dimensions[0];
      dims[1] = dimensions[1];
      dims[2] = dimensions[2];
      radcur[0] = radCur[0];
      radcur[1] = radCur[1];
      radcur[2] = radCur[2];

      res[0] = resolution[0];
      res[1] = resolution[1];
      res[2] = resolution[2];

      xc = xx[0];
      yc = xx[1];
      zc = xx[2];

      iteration = cycle;

      ga[0][0] = gA[0][0];
      ga[0][1] = gA[0][1];
      ga[0][2] = gA[0][2];
      ga[1][0] = gA[1][0];
      ga[1][1] = gA[1][1];
      ga[1][2] = gA[1][2];
      ga[2][0] = gA[2][0];
      ga[2][1] = gA[2][1];
      ga[2][2] = gA[2][2];

      newownersPtr = newowners;
      ellipfuncsPtr = ellipfuncs;

    }
    virtual ~AssignVoxelsGapsImpl(){}

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void convert(int zStart, int zEnd, int yStart, int yEnd, int xStart, int xEnd) const
    {

      int column = 0;
      int row = 0;
      int plane = 0;
      int index = 0;
      float coords[3] = {0.0f, 0.0f, 0.0f};
      float inside = 0.0f;
      float dist = 0.0f;
      float radcur1squared = radcur[0]*radcur[0];
      float coordsRotated[3] = {0.0f, 0.0f, 0.0f};
      int32_t* newowners = newownersPtr->GetPointer(0);
      float* ellipfuncs = ellipfuncsPtr->GetPointer(0);
      DimType dim0_dim_1 = dims[0] * dims[1];
      for (DimType iter1 = xStart; iter1 < xEnd; iter1++)
      {
        column = iter1;
        if (iter1 < 0) column = iter1 + dims[0];
        else if (iter1 > dims[0] - 1) column = iter1 - dims[0];

        for (DimType iter2 = yStart; iter2 < yEnd; iter2++)
        {
          row = iter2;
          size_t row_dim = row * dims[0];
          if (iter2 < 0) row = iter2 + dims[1];
          else if (iter2 > dims[1] - 1) row = iter2 - dims[1];

          for (DimType iter3 = zStart; iter3 < zEnd; iter3++)
          {
            plane = iter3;
            if (iter3 < 0) plane = iter3 + dims[2];
            else if (iter3 > dims[2] - 1) plane = iter3 - dims[2];

            index = static_cast<int>( (plane * dim0_dim_1) + (row_dim) + column );

            if(m_GrainIds[index] <= 0)
            {
              inside = -1;
              coords[0] = float(column) * res[0];
              coords[1] = float(row) * res[1];
              coords[2] = float(plane) * res[2];
              if (iter1 < 0) coords[0] = coords[0] - size[0];
              else if (iter1 > dims[0] - 1) coords[0] = coords[0] + size[0];
              if (iter2 < 0) coords[1] = coords[1] - size[1];
              else if (iter2 > dims[1] - 1) coords[1] = coords[1] + size[1];
              if (iter3 < 0) coords[2] = coords[2] - size[2];
              else if (iter3 > dims[2] - 1) coords[2] = coords[2] + size[2];

              dist = ((coords[0] - xc) * (coords[0] - xc)) + ((coords[1] - yc) * (coords[1] - yc)) + ((coords[2] - zc) * (coords[2] - zc));
              if (dist < radcur1squared)
              {
                coords[0] = coords[0] - xc;
                coords[1] = coords[1] - yc;
                coords[2] = coords[2] - zc;
                MatrixMath::multiply3x3with3x1(ga, coords, coordsRotated);
                float axis1comp = coordsRotated[0] / radcur[0];
                float axis2comp = coordsRotated[1] / radcur[1];
                float axis3comp = coordsRotated[2] / radcur[2];
                inside = m_ShapeOps->inside(axis1comp, axis2comp, axis3comp);
                if(iteration == 0)
                {
                  if (inside >= 0 && newowners[index] > 0)
                  {
                    newowners[index] = -2;
                    ellipfuncs[index] = inside;
                  }
                  else if (inside >= 0 && newowners[index] == -1)
                  {
                    newowners[index] = curGrain;
                    ellipfuncs[index] = inside;
                  }
                }
                else
                {
                  if (inside >= 0 && inside > ellipfuncs[index])
                  {
                    newowners[index] = curGrain;
                    ellipfuncs[index] = inside;
                  }
                }
              }
            }

          }
        }
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range3d<int, int, int> &r) const
    {
      convert(r.pages().begin(), r.pages().end(), r.rows().begin(), r.rows().end(), r.cols().begin(), r.cols().end());
    }
#endif

  private:

};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PackPrimaryPhases::PackPrimaryPhases() :
  AbstractFilter(),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_ActiveArrayName(DREAM3D::FieldData::Active),
  m_AxisEulerAnglesArrayName(DREAM3D::FieldData::AxisEulerAngles),
  m_AxisLengthsArrayName(DREAM3D::FieldData::AxisLengths),
  m_CentroidsArrayName(DREAM3D::FieldData::Centroids),
  m_EquivalentDiametersArrayName(DREAM3D::FieldData::EquivalentDiameters),
  m_NeighborhoodsArrayName(DREAM3D::FieldData::Neighborhoods),
  m_Omega3sArrayName(DREAM3D::FieldData::Omega3s),
  m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
  m_VolumesArrayName(DREAM3D::FieldData::Volumes),
  m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
  m_ShapeTypesArrayName(DREAM3D::EnsembleData::ShapeTypes),
  m_ErrorOutputFile(""),
  m_VtkOutputFile(""),
  m_PeriodicBoundaries(false),
  m_NeighborhoodErrorWeight(1.0f),
  m_GrainIds(NULL),
  m_CellPhases(NULL),
  m_SurfaceVoxels(NULL),
  m_Active(NULL),
  m_FieldPhases(NULL),
  m_Neighborhoods(NULL),
  m_Centroids(NULL),
  m_Volumes(NULL),
  m_AxisLengths(NULL),
  m_AxisEulerAngles(NULL),
  m_Omega3s(NULL),
  m_EquivalentDiameters(NULL),
  m_PhaseTypes(NULL),
  m_ShapeTypes(NULL)
{
  m_EllipsoidOps = EllipsoidOps::New();
  m_ShapeOps[DREAM3D::ShapeType::EllipsoidShape] = m_EllipsoidOps.get();
  m_SuperEllipsoidOps = SuperEllipsoidOps::New();
  m_ShapeOps[DREAM3D::ShapeType::SuperEllipsoidShape] = m_SuperEllipsoidOps.get();
  m_CubicOctohedronOps = CubeOctohedronOps::New();
  m_ShapeOps[DREAM3D::ShapeType::CubeOctahedronShape] = m_CubicOctohedronOps.get();
  m_CylinderOps = CylinderOps::New();
  m_ShapeOps[DREAM3D::ShapeType::CylinderShape] = m_CylinderOps.get();
  m_UnknownShapeOps = ShapeOps::New();
  m_ShapeOps[DREAM3D::ShapeType::UnknownShapeType] = m_UnknownShapeOps.get();

  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(m_HexOps.get());
  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(m_CubicOps.get());
  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(m_OrthoOps.get());

  Seed = MXA::getMilliSeconds();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PackPrimaryPhases::~PackPrimaryPhases()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Neighborhood Error Weight");
    option->setPropertyName("NeighborhoodErrorWeight");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Periodic Boundary");
    option->setPropertyName("PeriodicBoundaries");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void PackPrimaryPhases::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("NeighborhoodErrorWeight", getNeighborhoodErrorWeight() );
  writer->writeValue("PeriodicBoundaries", getPeriodicBoundaries() );
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  //Cell Data
  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -301, int32_t, Int32ArrayType, voxels, 1)
      GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, -302, int32_t, Int32ArrayType, voxels, 1)

      //Field Data
      CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, int32_t, Int32ArrayType, 0, fields, 1)
      CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, ss, float, FloatArrayType, 0, fields, 1)
      CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Omega3s, ss, float, FloatArrayType, 0, fields, 1)
      CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, AxisEulerAngles, ss, float, FloatArrayType, 0, fields, 3)
      CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, AxisLengths, ss, float, FloatArrayType, 0, fields, 3)
      CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Volumes, ss, float, FloatArrayType, 0, fields, 1)
      CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Centroids, ss, float, FloatArrayType, 0, fields, 3)
      CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, true, fields, 1)
      CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Neighborhoods, ss, int32_t, Int32ArrayType, 0, fields, 1)

      //Ensemble Data
      typedef DataArray<unsigned int> PhaseTypeArrayType;
  typedef DataArray<unsigned int> ShapeTypeArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, PhaseTypes, ss, -302, unsigned int, PhaseTypeArrayType, ensembles, 1)
      GET_PREREQ_DATA(m, DREAM3D, EnsembleData, ShapeTypes, ss, -305, unsigned int, ShapeTypeArrayType, ensembles, 1)
      m_StatsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(m->getEnsembleData(DREAM3D::EnsembleData::Statistics).get());
  if(m_StatsDataArray == NULL)
  {
    ss.str("");
    ss << "Stats Array Not Initialized correctly" << std::endl;
    setErrorCondition(-308);
    addErrorMessage(getHumanLabel(), ss.str(), -308);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();

  bool writeErrorFile = true;
  std::ofstream outFile;
  if(m_ErrorOutputFile.empty() == false)
  {
    outFile.open(m_ErrorOutputFile.c_str(), std::ios_base::binary);
    writeErrorFile = true;
  }

  int err = 0;
  setErrorCondition(err);
  unsigned long long int Seed = MXA::getMilliSeconds();
  DREAM3D_RANDOMNG_NEW_SEEDED(Seed);

  int64_t totalPoints = m->getTotalPoints();
  //size_t totalFields = m->getNumFieldTuples();
  dataCheck(false, totalPoints, 1, m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  size_t udims[3] =
  { 0, 0, 0 };
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  { static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]), };

  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();
  sizex = dims[0] * m->getXRes();
  sizey = dims[1] * m->getYRes();
  sizez = dims[2] * m->getZRes();
  totalvol = sizex*sizey*sizez;

  double totalprimaryvolTEMP = 0;
  //  float badcount = 0;
  //  size_t check = 0;
  size_t totalVox = static_cast<size_t>(dims[0] * dims[1] * dims[2]);
  for (size_t i = 0; i < totalVox; i++)
  {
    if(m_GrainIds[i] <= 0) totalprimaryvolTEMP++;
  }
  float totalprimaryvol = static_cast<float>(totalprimaryvolTEMP);
  totalprimaryvol = totalprimaryvol*(m->getXRes()*m->getYRes()*m->getZRes());

  size_t numensembles = m->getNumEnsembleTuples();
  std::stringstream ss;

  // float change1, change2;
  float change = 0.0f;
  int phase = 0;
  int randomgrain;
  //   float random = 0.0f;
  //  int newgrain;
  // float check;
  float xc, yc, zc;
  float oldxc, oldyc, oldzc;
  oldfillingerror = 0;
  currentneighborhooderror = 0, oldneighborhooderror = 0;
  currentsizedisterror = 0, oldsizedisterror = 0;
  int acceptedmoves = 0;
  float totalprimaryfractions = 0.0;
  // find which phases are primary phases
  for (size_t i = 1; i < numensembles; ++i)
  {

    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      if (NULL == pp)
      {
        ss << "Tried to cast a statsDataArray[" << i << "].get() to a PrimaryStatsData* ";
        ss << "pointer but this resulted in a NULL pointer. The value at m_PhaseTypes[" << i << "] = " << m_PhaseTypes[i] <<  " does not match up ";
        ss << "with the type of pointer stored in the StatsDataArray (PrimaryStatsData)\n";
        PipelineMessage em (getHumanLabel(), ss.str(), -666);
        addErrorMessage(em);
        setErrorCondition(-666);
        return;
      }
      primaryphases.push_back(static_cast<int>(i) );
      primaryphasefractions.push_back(pp->getPhaseFraction());
      totalprimaryfractions = totalprimaryfractions + pp->getPhaseFraction();
    }
  }
  // scale the primary phase fractions to total to 1
  for (size_t i = 0; i < primaryphasefractions.size(); i++)
  {
    primaryphasefractions[i] = primaryphasefractions[i] / totalprimaryfractions;
    if(i > 0) primaryphasefractions[i] = primaryphasefractions[i] + primaryphasefractions[i - 1];
  }

  notifyStatusMessage("Packing Grains - Initializing Volume");
  // this initializes the arrays to hold the details of the locations of all of the grains during packing
  initialize_packinggrid();

  // initialize the sim and goal size distributions for the primary phases
  grainsizedist.resize(primaryphases.size());
  simgrainsizedist.resize(primaryphases.size());
  grainsizediststep.resize(primaryphases.size());
  for (size_t i = 0; i < primaryphases.size(); i++)
  {
    phase = primaryphases[i];
    PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
    grainsizedist[i].resize(40);
    simgrainsizedist[i].resize(40);
    grainsizediststep[i] = static_cast<float>(((2 * pp->getMaxGrainDiameter()) - (pp->getMinGrainDiameter() / 2.0)) / grainsizedist[i].size());
    float input = 0;
    float previoustotal = 0;
    VectorOfFloatArray GSdist = pp->getGrainSizeDistribution();
    float avg = GSdist[0]->GetValue(0);
    float stdev = GSdist[1]->GetValue(0);
    float denominatorConst = sqrtf(2.0f * stdev * stdev); // Calculate it here rather than calculating the same thing multiple times below
    for (size_t j = 0; j < grainsizedist[i].size(); j++)
    {
      input = (float(j + 1) * grainsizediststep[i]) + (pp->getMinGrainDiameter() / 2.0f);
      float logInput = logf(input);
      if(logInput <= avg)
      {
        grainsizedist[i][j] = 0.5f - 0.5f * (DREAM3DMath::erf((avg - logInput) / denominatorConst)) - previoustotal;
      }
      if(logInput > avg)
      {
        grainsizedist[i][j] = 0.5f + 0.5f * (DREAM3DMath::erf((logInput - avg) / denominatorConst)) - previoustotal;
      }
      previoustotal = previoustotal + grainsizedist[i][j];
    }
  }

  if (getCancel() == true)
  {
    ss.str("");
    ss << "Filter Cancelled.";
    notifyWarningMessage(ss.str(), -1);
    setErrorCondition(-1);
    return;
  }

  // generate the grains and monitor the size distribution error while doing so. After grains are generated, no new grains can enter or leave the structure.
  Field field;
  int gid = static_cast<int>(m->getNumFieldTuples());
  if(gid == 0)
  {
    m->resizeFieldDataArrays(1);
    dataCheck(false, totalPoints, 1, m->getNumEnsembleTuples());
    gid = 1;
  }
  firstPrimaryField = gid;
  std::vector<float> curphasevol;
  curphasevol.resize(primaryphases.size());
  float factor = 1.0;
  size_t iter = 0;
  for (size_t j = 0; j < primaryphases.size(); ++j)
  {
    curphasevol[j] = 0;
    float curphasetotalvol = totalprimaryvol*primaryphasefractions[j];
    while (curphasevol[j] < (factor * curphasetotalvol))
    {
      iter++;
      Seed++;
      phase = primaryphases[j];
      generate_grain(phase, static_cast<int>(Seed), &field, m_ShapeTypes[phase], m_OrthoOps);
      currentsizedisterror = check_sizedisterror(&field);
      change = (currentsizedisterror) - (oldsizedisterror);
      if(change > 0 || currentsizedisterror > (1.0 - (float(iter) * 0.001)) || curphasevol[j] < (0.75* factor * curphasetotalvol))
      {
        std::stringstream ss;
        ss << "Packing Grains - Generating Grain #" << gid;
        notifyStatusMessage(ss.str());

        m->resizeFieldDataArrays(gid + 1);
        dataCheck(false, totalPoints, gid + 1, m->getNumEnsembleTuples());
        m_Active[gid] = true;
        transfer_attributes(gid, &field);
        oldsizedisterror = currentsizedisterror;
        curphasevol[j] = curphasevol[j] + m_Volumes[gid];
        //FIXME: Initialize the Grain with some sort of default data
        iter = 0;
        gid++;
      }
    }
  }

  if (getCancel() == true)
  {
    ss.str("");
    ss << "Filter Cancelled.";
    notifyWarningMessage(ss.str(), -1);
    setErrorCondition(-1);
    return;
  }

  if(m_PeriodicBoundaries == false)
  {
    iter = 0;
    int xgrains, ygrains, zgrains;
    xgrains = int(powf((m->getNumFieldTuples()*(sizex/sizey)*(sizex/sizez)),(1.0f/3.0f))+1);
    ygrains = int(xgrains*(sizey/sizex)+1);
    zgrains = int(xgrains*(sizez/sizex)+1);
    factor = 0.25f * (1.0f - (float((xgrains-2)*(ygrains-2)*(zgrains-2))/float(xgrains*ygrains*zgrains)));
    for (size_t j = 0; j < primaryphases.size(); ++j)
    {
      float curphasetotalvol = totalprimaryvol*primaryphasefractions[j];
      while (curphasevol[j] < ((1+factor) * curphasetotalvol))
      {
        iter++;
        Seed++;
        phase = primaryphases[j];
        generate_grain(phase, static_cast<int>(Seed), &field, m_ShapeTypes[phase], m_OrthoOps);
        currentsizedisterror = check_sizedisterror(&field);
        change = (currentsizedisterror) - (oldsizedisterror);
        if(change > 0 || currentsizedisterror > (1.0 - (iter * 0.001)) || curphasevol[j] < (0.75* factor * curphasetotalvol))
        {
          std::stringstream ss;
          ss << "Packing Grains - Generating Grain #" << gid;
          notifyStatusMessage(ss.str());

          m->resizeFieldDataArrays(gid + 1);
          dataCheck(false, totalPoints, gid + 1, m->getNumEnsembleTuples());
          m_Active[gid] = true;
          transfer_attributes(gid, &field);
          oldsizedisterror = currentsizedisterror;
          curphasevol[j] = curphasevol[j] + m_Volumes[gid];
          //FIXME: Initialize the Grain with some sort of default data
          iter = 0;
          gid++;
        }
      }
    }
  }

  if (getCancel() == true)
  {
    ss.str("");
    ss << "Filter Cancelled.";
    notifyWarningMessage(ss.str(), -1);
    setErrorCondition(-1);
    return;
  }
  notifyStatusMessage("Packing Grains - Grain Generation Complete");

  // initialize the sim and goal neighbor distribution for the primary phases
  neighbordist.resize(primaryphases.size());
  simneighbordist.resize(primaryphases.size());
  neighbordiststep.resize(primaryphases.size());
  for (size_t i = 0; i < primaryphases.size(); i++)
  {
    phase = primaryphases[i];
    PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
    neighbordist[i].resize(pp->getBinNumbers()->GetSize());
    simneighbordist[i].resize(pp->getBinNumbers()->GetSize());
    VectorOfFloatArray Neighdist = pp->getGrainSize_Neighbors();
    float normalizer = 0;
    for (size_t j = 0; j < neighbordist[i].size(); j++)
    {
      neighbordist[i][j].resize(40);
      float input = 0;
      float previoustotal = 0;
      float avg = Neighdist[0]->GetValue(j);
      float stdev = Neighdist[1]->GetValue(j);
      neighbordiststep[i] = 2;
      float denominatorConst = sqrtf(2.0f * stdev * stdev); // Calculate it here rather than calculating the same thing multiple times below
      for (size_t k = 0; k < neighbordist[i][j].size(); k++)
      {
        input = (float(k + 1) * neighbordiststep[i]);
        float logInput = logf(input);
        if(logInput <= avg)
        {
          neighbordist[i][j][k] = 0.5f - 0.5f * (DREAM3DMath::erf((avg - logInput) / denominatorConst )) - previoustotal;
        }
        if(logInput > avg)
        {
          neighbordist[i][j][k] = 0.5f + 0.5f * (DREAM3DMath::erf((logInput - avg) / denominatorConst)) - previoustotal;
        }
        previoustotal = previoustotal + neighbordist[i][j][k];
      }
      normalizer = normalizer + previoustotal;
    }
    for (size_t j = 0; j < neighbordist[i].size(); j++)
    {
      for (size_t k = 0; k < neighbordist[i][j].size(); k++)
      {
        neighbordist[i][j][k] = neighbordist[i][j][k]/normalizer;
      }
    }
  }

  if (getCancel() == true)
  {
    ss.str("");
    ss << "Filter Cancelled.";
    notifyWarningMessage(ss.str(), -1);
    setErrorCondition(-1);
    return;
  }


  //  for each grain : select centroid, determine voxels in grain, monitor filling error and decide of the 10 placements which
  // is the most beneficial, then the grain is added and its neighbors are determined

  size_t numgrains = m->getNumFieldTuples();

  columnlist.resize(numgrains);
  rowlist.resize(numgrains);
  planelist.resize(numgrains);
  packqualities.resize(numgrains);
  fillingerror = 1;

  int progGrain = 0;
  int progGrainInc = numgrains * .01;
  for (size_t i = firstPrimaryField; i < numgrains; i++)
  {
    if ((int)i > progGrain + progGrainInc)
    {
      ss.str("");
      ss << "Packing Grains - Placing Grain #" << i << "/" << numgrains;
      notifyStatusMessage(ss.str());
      progGrain = i;
    }

    xc = sizex / 2.0f;
    yc = sizey / 2.0f;
    zc = sizez / 2.0f;
    m_Centroids[3 * i] = xc;
    m_Centroids[3 * i + 1] = yc;
    m_Centroids[3 * i + 2] = zc;
    insert_grain(i);
    fillingerror = check_fillingerror(i, -1000);
    for (int iter = 0; iter < 10; iter++)
    {
      xc = static_cast<float>(rg.genrand_res53() * (dims[0] * xRes));
      yc = static_cast<float>(rg.genrand_res53() * (dims[1] * yRes));
      zc = static_cast<float>(rg.genrand_res53() * (dims[2] * zRes));
      oldxc = m_Centroids[3 * i];
      oldyc = m_Centroids[3 * i + 1];
      oldzc = m_Centroids[3 * i + 2];
      oldfillingerror = fillingerror;
      fillingerror = check_fillingerror(-1000, i);
      move_grain(i, xc, yc, zc);
      fillingerror = check_fillingerror(i, -1000);
      if(fillingerror > oldfillingerror)
      {
        fillingerror = check_fillingerror(-1000, i);
        move_grain(i, oldxc, oldyc, oldzc);
        fillingerror = check_fillingerror(i, -1000);
      }
    }
  }

  notifyStatusMessage("Packing Grains - Initial Grain Placement Complete");

  // determine neighborhoods and initial neighbor distribution errors
  for (size_t i = firstPrimaryField; i < numgrains; i++)
  {
    determine_neighbors(i, 1);
  }
  oldneighborhooderror = check_neighborhooderror(-1000, -1000);
  // begin swaping/moving/adding/removing grains to try to improve packing
  int totalAdjustments = static_cast<int>(100 * (numgrains-1));
  progGrain = 0;
  progGrainInc = totalAdjustments * .01;
  uint64_t millis = MXA::getMilliSeconds();
  uint64_t currentMillis = millis;
  uint64_t startMillis = millis;
  uint64_t estimatedTime = 0;
  float timeDiff = 0.0f;

  for (int iteration = 0; iteration < totalAdjustments; ++iteration)
  {
    currentMillis = MXA::getMilliSeconds();
    if (currentMillis - millis > 1000)
    {
      ss.str("");
      ss << "Packing Grains - Swapping/Moving/Adding/Removing Grains Iteration " << iteration << "/" << totalAdjustments;
      timeDiff = ((float)iteration / (float)(currentMillis - startMillis));
      estimatedTime = (float)(totalAdjustments - iteration) / timeDiff;

      ss << " Est. Time Remain: " << MXA::convertMillisToHrsMinSecs(estimatedTime);
      notifyStatusMessage(ss.str());
      millis = MXA::getMilliSeconds();
    }


    if (getCancel() == true)
    {
      ss.str("");
      ss << "Filter Cancelled.";
      notifyWarningMessage(ss.str(), -1);
      setErrorCondition(-1);
      return;
    }

    int option = iteration % 2;

    if(writeErrorFile == true && iteration % 25 == 0)
    {
      outFile << iteration << " " << fillingerror << "  " << oldsizedisterror << "  " << oldneighborhooderror << "  " << numgrains << " " << acceptedmoves
              << std::endl;
    }

    // JUMP - this option moves one grain to a random spot in the volume
    if(option == 0)
    {
      randomgrain = firstPrimaryField + int(rg.genrand_res53() * (numgrains-firstPrimaryField));
      if(randomgrain < static_cast<int>(firstPrimaryField)) {
        randomgrain = static_cast<int>(firstPrimaryField);
      }

      if(randomgrain >= static_cast<int>(numgrains))
      {
        randomgrain = static_cast<int>(numgrains) - 1;
      }
      Seed++;

      xc = static_cast<float>(rg.genrand_res53() * (dims[0] * xRes));
      yc = static_cast<float>(rg.genrand_res53() * (dims[1] * yRes));
      zc = static_cast<float>(rg.genrand_res53() * (dims[2] * zRes));
      oldxc = m_Centroids[3 * randomgrain];
      oldyc = m_Centroids[3 * randomgrain + 1];
      oldzc = m_Centroids[3 * randomgrain + 2];
      oldfillingerror = fillingerror;
      fillingerror = check_fillingerror(-1000, static_cast<int>(randomgrain)  );
      move_grain(randomgrain, xc, yc, zc);
      fillingerror = check_fillingerror(static_cast<int>(randomgrain), -1000);
      currentneighborhooderror = check_neighborhooderror(-1000, randomgrain);
      if(fillingerror <= oldfillingerror)
      {
        oldneighborhooderror = currentneighborhooderror;
        acceptedmoves++;
      }
      else if(fillingerror > oldfillingerror)
      {
        fillingerror = check_fillingerror(-1000, static_cast<int>(randomgrain));
        move_grain(randomgrain, oldxc, oldyc, oldzc);
        fillingerror = check_fillingerror(static_cast<int>(randomgrain), -1000);
      }
    }
    // NUDGE - this option moves one grain to a spot close to its current centroid
    if(option == 1)
    {
      randomgrain = firstPrimaryField + int(rg.genrand_res53() * (numgrains-firstPrimaryField));
      if(randomgrain < firstPrimaryField) {
        randomgrain = firstPrimaryField;
      }
      if(randomgrain >= static_cast<int>(numgrains))
      {
        randomgrain = static_cast<int>(numgrains) - 1;
      }
      Seed++;
      oldxc = m_Centroids[3 * randomgrain];
      oldyc = m_Centroids[3 * randomgrain + 1];
      oldzc = m_Centroids[3 * randomgrain + 2];
      xc = static_cast<float>( oldxc + ((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * packingresx)) );
      yc = static_cast<float>( oldyc + ((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * packingresy)) );
      zc = static_cast<float>( oldzc + ((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * packingresz)) );
      oldfillingerror = fillingerror;
      fillingerror = check_fillingerror(-1000, static_cast<int>(randomgrain));
      move_grain(randomgrain, xc, yc, zc);
      fillingerror = check_fillingerror(static_cast<int>(randomgrain), -1000);
      currentneighborhooderror = check_neighborhooderror(-1000, randomgrain);
      //      change2 = (currentneighborhooderror * currentneighborhooderror) - (oldneighborhooderror * oldneighborhooderror);
      //      if(fillingerror <= oldfillingerror && currentneighborhooderror >= oldneighborhooderror)
      if(fillingerror <= oldfillingerror)
      {
        oldneighborhooderror = currentneighborhooderror;
        acceptedmoves++;
      }
      //      else if(fillingerror > oldfillingerror || currentneighborhooderror < oldneighborhooderror)
      else if(fillingerror > oldfillingerror)
      {
        fillingerror = check_fillingerror(-1000, static_cast<int>(randomgrain));
        move_grain(randomgrain, oldxc, oldyc, oldzc);
        fillingerror = check_fillingerror(static_cast<int>(randomgrain), -1000);
      }
    }
  }

  notifyStatusMessage("Packing Grains - Grain Adjustment Complete");

  if(m_VtkOutputFile.empty() == false)
  {
    err = writeVtkFile();
    if(err < 0)
    {
      return;
    }
  }

  notifyStatusMessage("Packing Grains - Assigning Voxels");
  assign_voxels_and_gaps();
  if (getCancel() == true) { return; }

  notifyStatusMessage("Packing Grains - Cleaning Up Volume");
  cleanup_grains();
  if (getCancel() == true) { return; }

  notifyStatusMessage("Packing Grains - Renumbering Grains");
  RenumberGrains::Pointer renumber_grains2 = RenumberGrains::New();
  renumber_grains2->setObservers(this->getObservers());
  renumber_grains2->setVoxelDataContainer(m);
  renumber_grains2->execute();
  err = renumber_grains2->getErrorCondition();
  if (err < 0)
  {
    setErrorCondition(renumber_grains2->getErrorCondition());
    addErrorMessages(renumber_grains2->getPipelineMessages());
    return;
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("Packing Grains Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PackPrimaryPhases::writeVtkFile()
{
  //  ofstream outFile;
  std::ofstream outFile;
  outFile.open(m_VtkOutputFile.c_str(), std::ios_base::binary);
  if(outFile.is_open() == false)
  {
    std::cout << "m_VtkOutputFile: " << m_VtkOutputFile << std::endl;
    PipelineMessage em (getHumanLabel(), "Could not open Vtk File for writing from PackGrains", -1);
    addErrorMessage(em);
    setErrorCondition(-55);
    return -1;
  }
  outFile << "# vtk DataFile Version 2.0" << std::endl;
  outFile << "DREAM.3D Generated from PackPrimaryPhases Filter" << std::endl;
  outFile << "ASCII" << std::endl;
  outFile << "DATASET STRUCTURED_POINTS" << std::endl;
  outFile << "DIMENSIONS " << packingxpoints << " " << packingypoints << " " << packingzpoints << std::endl;
  outFile << "ORIGIN 0.0 0.0 0.0" << std::endl;
  outFile << "SPACING " << packingresx << " " << packingresy << " " << packingresz << std::endl;
  outFile << "POINT_DATA " << packingxpoints * packingypoints * packingzpoints << std::endl;
  outFile << std::endl;
  outFile << std::endl;
  outFile << "SCALARS GrainID int  1" << std::endl;
  outFile << "LOOKUP_TABLE default" << std::endl;
  for (int i = 0; i < (packingzpoints); i++)
  {
    for (int j = 0; j < (packingypoints); j++)
    {
      for (int k = 0; k < (packingxpoints); k++)
      {
        int name = grainowners[k][j][i];
        if(i % 20 == 0 && i > 0) outFile << std::endl;
        outFile << "     ";
        if(name < 100) outFile << " ";
        if(name < 10) outFile << " ";
        outFile << name;
      }
    }
  }
  outFile.close();
  return 0;
}

void PackPrimaryPhases::initialize_packinggrid()
{
  VoxelDataContainer* m = getVoxelDataContainer();

  packingresx = m->getXRes() * 2.0f;
  packingresy = m->getYRes() * 2.0f;
  packingresz = m->getZRes() * 2.0f;
  packingxpoints = int(sizex / packingresx);
  packingypoints = int(sizey / packingresy);
  packingzpoints = int(sizez / packingresz);
  packingtotalpoints = packingxpoints * packingypoints * packingzpoints;
  grainowners.resize(packingxpoints);
  for (int i = 0; i < packingxpoints; i++)
  {
    grainowners[i].resize(packingypoints);
    for (int j = 0; j < packingypoints; j++)
    {
      grainowners[i][j].resize(packingzpoints, 0);
    }
  }
}


void PackPrimaryPhases::generate_grain(int phase, int Seed, Field* field, unsigned int shapeclass, OrientationMath::Pointer OrthoOps)
{
  DREAM3D_RANDOMNG_NEW_SEEDED(Seed)

      //DataContainer* m = getVoxelDataContainer();

      StatsDataArray& statsDataArray = *m_StatsDataArray;

  float r1 = 1;
  float a2 = 0, a3 = 0;
  float b2 = 0, b3 = 0;
  float diam = 0;
  float vol = 0;
  int volgood = 0;
  float phi1, PHI, phi2;
  float fourThirdsPi =  static_cast<float>((4.0f / 3.0f) * (m_pi));
  PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
  VectorOfFloatArray GSdist = pp->getGrainSizeDistribution();
  float avg = GSdist[0]->GetValue(0);
  float stdev = GSdist[1]->GetValue(0);
  while (volgood == 0)
  {
    volgood = 1;
    diam = static_cast<float>(rg.genrand_norm(avg, stdev));
    diam = exp(diam);
    if(diam >= pp->getMaxGrainDiameter()) volgood = 0;
    if(diam < pp->getMinGrainDiameter()) volgood = 0;
    vol = fourThirdsPi * ((diam / 2.0f) * (diam / 2.0f) * (diam / 2.0f));
  }
  int diameter = int((diam - pp->getMinGrainDiameter()) / pp->getBinStepSize());
  float r2 = 0, r3 = 1;
  VectorOfFloatArray bovera = pp->getGrainSize_BOverA();
  VectorOfFloatArray covera = pp->getGrainSize_COverA();
  while (r2 < r3)
  {
    r2 = 0, r3 = 0;
    a2 = bovera[0]->GetValue(diameter);
    b2 = bovera[1]->GetValue(diameter);
    if(a2 == 0)
    {
      a2 = bovera[0]->GetValue(diameter-1);
      b2 = bovera[1]->GetValue(diameter-1);
    }
    r2 = static_cast<float>(rg.genrand_beta(a2, b2));
    a3 = covera[0]->GetValue(diameter);
    b3 = covera[1]->GetValue(diameter);
    if(a3 == 0)
    {
      a3 = covera[0]->GetValue(diameter-1);
      b3 = covera[1]->GetValue(diameter-1);
    }
    r3 = static_cast<float>( rg.genrand_beta(a3, b3) );
  }
  float random = static_cast<float>( rg.genrand_res53() );
  float totaldensity = 0;
  int bin = 0;
  FloatArrayType::Pointer axisodf = pp->getAxisOrientation();
  while (random > totaldensity && bin < static_cast<int>(axisodf->GetSize()) )
  {
    totaldensity = totaldensity + axisodf->GetValue(bin);
    bin++;
  }
  OrthoOps->determineEulerAngles(bin, phi1, PHI, phi2);
  VectorOfFloatArray omega3 = pp->getGrainSize_Omegas();
  float mf = omega3[0]->GetValue(diameter);
  float s = omega3[1]->GetValue(diameter);
  float omega3f = static_cast<float>(rg.genrand_beta(mf, s));
  if(shapeclass == DREAM3D::ShapeType::EllipsoidShape) omega3f = 1;

  field->m_Volumes = vol;
  field->m_EquivalentDiameters = diam;
  field->m_AxisLengths[0] = r1;
  field->m_AxisLengths[1] = r2;
  field->m_AxisLengths[2] = r3;
  field->m_AxisEulerAngles[0] = phi1;
  field->m_AxisEulerAngles[1] = PHI;
  field->m_AxisEulerAngles[2] = phi2;
  field->m_Omega3s = omega3f;
  field->m_FieldPhases = phase;
  field->m_Neighborhoods = 0;
}

void PackPrimaryPhases::transfer_attributes(int gnum, Field* field)
{
  m_Volumes[gnum] = field->m_Volumes;
  m_EquivalentDiameters[gnum] = field->m_EquivalentDiameters;
  m_AxisLengths[3*gnum+0] = field->m_AxisLengths[0];
  m_AxisLengths[3*gnum+1] = field->m_AxisLengths[1];
  m_AxisLengths[3*gnum+2] = field->m_AxisLengths[2];
  m_AxisEulerAngles[3*gnum+0] = field->m_AxisEulerAngles[0];
  m_AxisEulerAngles[3*gnum+1] = field->m_AxisEulerAngles[1];
  m_AxisEulerAngles[3*gnum+2] = field->m_AxisEulerAngles[2];
  m_Omega3s[gnum] = field->m_Omega3s;
  m_FieldPhases[gnum] = field->m_FieldPhases;
  m_Neighborhoods[gnum] = field->m_Neighborhoods;
}

void PackPrimaryPhases::move_grain(size_t gnum, float xc, float yc, float zc)
{
  int occolumn, ocrow, ocplane;
  int nccolumn, ncrow, ncplane;
  int shiftcolumn, shiftrow, shiftplane;
  float oxc = m_Centroids[3*gnum];
  float oyc = m_Centroids[3*gnum+1];
  float ozc = m_Centroids[3*gnum+2];
  occolumn = static_cast<int>( (oxc - (packingresx / 2.0f)) / packingresx );
  ocrow = static_cast<int>( (oyc - (packingresy / 2.0f)) / packingresy );
  ocplane = static_cast<int>( (ozc - (packingresz / 2.0f)) / packingresz );
  nccolumn = static_cast<int>( (xc - (packingresx / 2.0f)) / packingresx );
  ncrow = static_cast<int>( (yc - (packingresy / 2.0f)) / packingresy );
  ncplane = static_cast<int>( (zc - (packingresz / 2.0f)) / packingresz );
  shiftcolumn = nccolumn - occolumn;
  shiftrow = ncrow - ocrow;
  shiftplane = ncplane - ocplane;
  m_Centroids[3*gnum] = xc;
  m_Centroids[3*gnum+1] = yc;
  m_Centroids[3*gnum+2] = zc;
  size_t size = columnlist[gnum].size();

  for (size_t i = 0; i < size; i++)
  {
    int& cl = columnlist[gnum][i];
    cl += shiftcolumn;
    int& rl = rowlist[gnum][i];
    rl += shiftrow;
    int& pl = planelist[gnum][i];
    pl += shiftplane;
  }
}


void PackPrimaryPhases::determine_neighbors(size_t gnum, int add)
{
  VoxelDataContainer* m = getVoxelDataContainer();
  float x, y, z;
  float xn, yn, zn;
  float dia, dia2;
  float dx, dy, dz;
  //  int nnum = 0;
  //  nnum = 0;
  x = m_Centroids[3*gnum];
  y = m_Centroids[3*gnum+1];
  z = m_Centroids[3*gnum+2];
  dia = m_EquivalentDiameters[gnum];
  for (size_t n = firstPrimaryField; n < m->getNumFieldTuples(); n++)
  {
    xn = m_Centroids[3*n];
    yn = m_Centroids[3*n+1];
    zn = m_Centroids[3*n+2];
    dia2 = m_EquivalentDiameters[n];
    dx = fabs(x - xn);
    dy = fabs(y - yn);
    dz = fabs(z - zn);
    if(dx < dia && dy < dia && dz < dia)
    {
      if(add > 0) m_Neighborhoods[gnum]++;
      if(add < 0) m_Neighborhoods[gnum] = m_Neighborhoods[gnum] - 1;
    }
    if(dx < dia2 && dy < dia2 && dz < dia2)
    {
      if(add > 0) m_Neighborhoods[n]++;
      if(add < 0) m_Neighborhoods[n] = m_Neighborhoods[n] - 1;
    }
  }
}

float PackPrimaryPhases::check_neighborhooderror(int gadd, int gremove)
{
  VoxelDataContainer* m = getVoxelDataContainer();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  float neighborerror;
  float bhattdist;
  float dia;
  int nnum;
  size_t diabin = 0;
  size_t nnumbin = 0;
  int index = 0;
  std::vector<int> count;
  int counter = 0;
  int phase;
  for (size_t iter = 0; iter < simneighbordist.size(); ++iter)
  {
    phase = primaryphases[iter];
    PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
    count.resize(simneighbordist[iter].size(), 0);
    for (size_t i = 0; i < simneighbordist[iter].size(); i++)
    {
      simneighbordist[iter][i].resize(40);
      for (size_t j = 0; j < 40; j++)
      {
        simneighbordist[iter][i][j] = 0;
      }
    }
    if(gadd > 0 && m_FieldPhases[gadd] == phase)
    {
      determine_neighbors(gadd, 1);
    }
    if(gremove > 0 && m_FieldPhases[gremove] == phase)
    {
      determine_neighbors(gremove, -1);
    }
    for (size_t i = firstPrimaryField; i < m->getNumFieldTuples(); i++)
    {
      nnum = 0;
      index = i;
      if(index != gremove && m_FieldPhases[index] == phase)
      {
        dia = m_EquivalentDiameters[index];
        if(dia > pp->getMaxGrainDiameter()) dia = pp->getMaxGrainDiameter();
        if(dia < pp->getMinGrainDiameter()) dia = pp->getMinGrainDiameter();
        diabin = static_cast<size_t>(((dia - pp->getMinGrainDiameter()) / pp->getBinStepSize()) );
        nnum = m_Neighborhoods[index];
        nnumbin = static_cast<size_t>( nnum/neighbordiststep[iter] );
        if(nnumbin >= 40) nnumbin = 39;
        simneighbordist[iter][diabin][nnumbin]++;
        count[diabin]++;
        counter++;
      }
    }
    if(gadd > 0 && m_FieldPhases[gadd] == phase)
    {
      dia = m_EquivalentDiameters[gadd];
      if(dia > pp->getMaxGrainDiameter()) dia = pp->getMaxGrainDiameter();
      if(dia < pp->getMinGrainDiameter()) dia = pp->getMinGrainDiameter();
      diabin = static_cast<size_t>(((dia - pp->getMinGrainDiameter()) / pp->getBinStepSize()) );
      nnum = m_Neighborhoods[gadd];
      nnumbin = static_cast<size_t>( nnum/neighbordiststep[iter] );
      if(nnumbin >= 40) nnumbin = 39;
      simneighbordist[iter][diabin][nnumbin]++;
      count[diabin]++;
      counter++;
    }
    float runningtotal = 0;
    for (size_t i = 0; i < simneighbordist[iter].size(); i++)
    {
      for (size_t j = 0; j < 40; j++)
      {
        simneighbordist[iter][i][j] = static_cast<float>( simneighbordist[iter][i][j] / double(count[i]) );
        if(count[i] == 0) simneighbordist[iter][i][j] = 0.0;
        runningtotal = runningtotal + simneighbordist[iter][i][j];
      }
    }
    for (size_t i = 0; i < simneighbordist[iter].size(); i++)
    {
      for (size_t j = 0; j < 40; j++)
      {
        simneighbordist[iter][i][j] = static_cast<float>( simneighbordist[iter][i][j] / runningtotal );
      }
    }
    if(gadd > 0 && m_FieldPhases[gadd] == phase)
    {
      determine_neighbors(gadd, -1);
    }
    if(gremove > 0 && m_FieldPhases[gremove] == phase)
    {
      determine_neighbors(gremove, 1);
    }
  }
  compare_3Ddistributions(simneighbordist, neighbordist, bhattdist);
  neighborerror = bhattdist;
  return neighborerror;
}

void PackPrimaryPhases::compare_1Ddistributions(std::vector<float> array1, std::vector<float> array2, float &bhattdist)
{
  bhattdist = 0;
  for (size_t i = 0; i < array1.size(); i++)
  {
    bhattdist = bhattdist + sqrt((array1[i]*array2[i]));
  }
}
void PackPrimaryPhases::compare_2Ddistributions(std::vector<std::vector<float> > array1, std::vector<std::vector<float> > array2, float &bhattdist)
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

void PackPrimaryPhases::compare_3Ddistributions(std::vector<std::vector<std::vector<float> > > array1, std::vector<std::vector<std::vector<float> > > array2, float &bhattdist)
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

float PackPrimaryPhases::check_sizedisterror(Field* field)
{
  VoxelDataContainer* m = getVoxelDataContainer();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  float dia;
  float sizedisterror = 0;
  float bhattdist;
  int index;
  int count = 0;
  int phase;
  for (size_t iter = 0; iter < grainsizedist.size(); ++iter)
  {
    phase = primaryphases[iter];
    PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
    count = 0;
    for (size_t i = 0; i < grainsizedist[iter].size(); i++)
    {
      simgrainsizedist[iter][i] = 0.0f;
    }
    for (size_t b = firstPrimaryField; b < m->getNumFieldTuples(); b++)
    {
      index = b;
      if(m_FieldPhases[index] == phase)
      {
        dia = m_EquivalentDiameters[index];
        dia = (dia - (pp->getMinGrainDiameter() / 2.0f)) / grainsizediststep[iter];
        if(dia < 0) dia = 0;
        if(dia > grainsizedist[iter].size() - 1.0f) dia = grainsizedist[iter].size() - 1.0f;
        simgrainsizedist[iter][int(dia)]++;
        count++;
      }
    }
    if(field->m_FieldPhases == phase)
    {
      dia = field->m_EquivalentDiameters;
      dia = (dia - (pp->getMinGrainDiameter() / 2.0f)) / grainsizediststep[iter];
      if(dia < 0) dia = 0;
      if(dia > grainsizedist[iter].size() - 1.0f) dia = grainsizedist[iter].size() - 1.0f;
      simgrainsizedist[iter][int(dia)]++;
      count++;
    }
    for (size_t i = 0; i < grainsizedist[iter].size(); i++)
    {
      simgrainsizedist[iter][i] = simgrainsizedist[iter][i] / float(count);
      if(count == 0) simgrainsizedist[iter][i] = 0.0;
    }
  }
  compare_2Ddistributions(simgrainsizedist, grainsizedist, bhattdist);
  sizedisterror = bhattdist;
  return sizedisterror;
}

float PackPrimaryPhases::check_fillingerror(int gadd, int gremove)
{
  fillingerror = fillingerror * float(packingtotalpoints);
  int col, row, plane;
  if(gadd > 0)
  {
    size_t size = columnlist[gadd].size();
    std::vector<int>& cl_gadd = columnlist[gadd];
    std::vector<int>& rl_gadd = rowlist[gadd];
    std::vector<int>& pl_gadd = planelist[gadd];
    float packquality = 0;
    for (size_t i = 0; i < size; i++)
    {
      col = cl_gadd[i];
      row = rl_gadd[i];
      plane = pl_gadd[i];

      if(m_PeriodicBoundaries == true)
      {
        if(col < 0) col = col + packingxpoints;
        if(col > packingxpoints - 1) col = col - packingxpoints;
        if(row < 0) row = row + packingypoints;
        if(row > packingypoints - 1) row = row - packingypoints;
        if(plane < 0) plane = plane + packingzpoints;
        if(plane > packingzpoints - 1) plane = plane - packingzpoints;
        int& currentGrainOwner = grainowners[col][row][plane];
        fillingerror = fillingerror + (2 * currentGrainOwner - 1);
        packquality = packquality + ((currentGrainOwner) * (currentGrainOwner));
        ++currentGrainOwner;
      }
      else
      {
        if(col >= 0 && col <= packingxpoints - 1 && row >= 0 && row <= packingypoints - 1 && plane >= 0 && plane <= packingzpoints - 1)
        {
          int& currentGrainOwner = grainowners[col][row][plane];
          fillingerror = fillingerror + (2 * currentGrainOwner - 1);
          packquality = packquality + ((currentGrainOwner) * (currentGrainOwner));
          ++currentGrainOwner;
        }
      }
    }
    packqualities[gadd] = static_cast<int>( packquality / float(size) );
  }
  if(gremove > 0)
  {
    size_t size = columnlist[gremove].size();
    std::vector<int>& cl_gremove = columnlist[gremove];
    std::vector<int>& rl_gremove = rowlist[gremove];
    std::vector<int>& pl_gremove = planelist[gremove];
    for (size_t i = 0; i < size; i++)
    {
      col = cl_gremove[i];
      row = rl_gremove[i];
      plane = pl_gremove[i];
      if(m_PeriodicBoundaries == true)
      {
        if(col < 0) col = col + packingxpoints;
        if(col > packingxpoints - 1) col = col - packingxpoints;
        if(row < 0) row = row + packingypoints;
        if(row > packingypoints - 1) row = row - packingypoints;
        if(plane < 0) plane = plane + packingzpoints;
        if(plane > packingzpoints - 1) plane = plane - packingzpoints;
        int& currentGrainOwner = grainowners[col][row][plane];
        fillingerror = fillingerror + (-2 * currentGrainOwner + 3);
        currentGrainOwner = currentGrainOwner - 1;
      }
      else
      {
        if(col >= 0 && col <= packingxpoints - 1 && row >= 0 && row <= packingypoints - 1 && plane >= 0 && plane <= packingzpoints - 1)
        {
          int& currentGrainOwner = grainowners[col][row][plane];
          fillingerror = fillingerror + (-2 * currentGrainOwner + 3);
          currentGrainOwner = currentGrainOwner - 1;
        }
      }
    }
  }
  fillingerror = fillingerror / float(packingtotalpoints);
  return fillingerror;
}

void PackPrimaryPhases::insert_grain(size_t gnum)
{
  DREAM3D_RANDOMNG_NEW()
      //   DataContainer* m = getVoxelDataContainer();
      //  float dist;
      float inside = -1;
  int column, row, plane;
  int centercolumn, centerrow, centerplane;
  int xmin, xmax, ymin, ymax, zmin, zmax;
  float xc, yc, zc;
  float coordsRotated[3];
  float coords[3];
  float volcur = m_Volumes[gnum];
  float bovera = m_AxisLengths[3*gnum+1];
  float covera = m_AxisLengths[3*gnum+2];
  float omega3 = m_Omega3s[gnum];
  float radcur1 = 1;
  unsigned int shapeclass = m_ShapeTypes[m_FieldPhases[gnum]];

  // init any values for each of the Shape Ops
  for (std::map<unsigned int, ShapeOps*>::iterator ops = m_ShapeOps.begin(); ops != m_ShapeOps.end(); ++ops)
  {
    (*ops).second->init();
  }
  // Create our Argument Map
  std::map<ShapeOps::ArgName, float> shapeArgMap;
  shapeArgMap[ShapeOps::Omega3] = omega3;
  shapeArgMap[ShapeOps::VolCur] = volcur;
  shapeArgMap[ShapeOps::B_OverA] = bovera;
  shapeArgMap[ShapeOps::C_OverA] = covera;

  radcur1 = m_ShapeOps[shapeclass]->radcur1(shapeArgMap);

  float radcur2 = (radcur1 * bovera);
  float radcur3 = (radcur1 * covera);
  float phi1 = m_AxisEulerAngles[3*gnum];
  float PHI = m_AxisEulerAngles[3*gnum+1];
  float phi2 = m_AxisEulerAngles[3*gnum+2];
  float ga[3][3];
  OrientationMath::eulertoMat(phi1, PHI, phi2, ga);
  xc = m_Centroids[3*gnum];
  yc = m_Centroids[3*gnum+1];
  zc = m_Centroids[3*gnum+2];
  centercolumn = static_cast<int>( (xc - (packingresx / 2)) / packingresx );
  centerrow = static_cast<int>( (yc - (packingresy / 2)) / packingresy );
  centerplane = static_cast<int>( (zc - (packingresz / 2)) / packingresz );
  xmin = int(centercolumn - ((radcur1 / packingresx) + 1));
  xmax = int(centercolumn + ((radcur1 / packingresx) + 1));
  ymin = int(centerrow - ((radcur1 / packingresy) + 1));
  ymax = int(centerrow + ((radcur1 / packingresy) + 1));
  zmin = int(centerplane - ((radcur1 / packingresz) + 1));
  zmax = int(centerplane + ((radcur1 / packingresz) + 1));
  if(xmin < -packingxpoints) xmin = -packingxpoints;
  if(xmax > 2 * packingxpoints - 1) xmax = (2 * packingxpoints - 1);
  if(ymin < -packingypoints) ymin = -packingypoints;
  if(ymax > 2 * packingypoints - 1) ymax = (2 * packingypoints - 1);
  if(zmin < -packingzpoints) zmin = -packingzpoints;
  if(zmax > 2 * packingzpoints - 1) zmax = (2 * packingzpoints - 1);
  for (int iter1 = xmin; iter1 < xmax + 1; iter1++)
  {
    for (int iter2 = ymin; iter2 < ymax + 1; iter2++)
    {
      for (int iter3 = zmin; iter3 < zmax + 1; iter3++)
      {
        column = iter1;
        row = iter2;
        plane = iter3;
        coords[0] = float(column) * packingresx;
        coords[1] = float(row) * packingresy;
        coords[2] = float(plane) * packingresz;
        inside = -1;
        coords[0] = coords[0] - xc;
        coords[1] = coords[1] - yc;
        coords[2] = coords[2] - zc;
        MatrixMath::multiply3x3with3x1(ga, coords, coordsRotated);
        float axis1comp = coordsRotated[0] / radcur1;
        float axis2comp = coordsRotated[1] / radcur2;
        float axis3comp = coordsRotated[2] / radcur3;
        inside = m_ShapeOps[shapeclass]->inside(axis1comp, axis2comp, axis3comp);
        if(inside >= 0)
        {
          columnlist[gnum].push_back(column);
          rowlist[gnum].push_back(row);
          planelist[gnum].push_back(plane);
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::assign_voxels_and_gaps()
{
  notifyStatusMessage("Assigning Voxels and Gaps");

  VoxelDataContainer* m = getVoxelDataContainer();
  int64_t totpoints = m->getTotalPoints();

  std::stringstream ss;

  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);

  DimType dims[3] = {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };


  //int index;
  int timestep = 50;

  int unassignedcount = 1;
  DimType column, row, plane;
  //  float inside;
  float xc, yc, zc;
  //  float coordsRotated[3];
  //  float dist;
  //  float coords[3];
  float size[3] = {sizex, sizey, sizez};

  DimType xmin, xmax, ymin, ymax, zmin, zmax;

  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();
  float res[3] = {xRes, yRes, zRes};

  Int32ArrayType::Pointer newownersPtr = Int32ArrayType::CreateArray(totpoints, "newowners");
  int32_t* newowners = newownersPtr->GetPointer(0);
  newownersPtr->initializeWithZeros();

  FloatArrayType::Pointer ellipfuncsPtr = FloatArrayType::CreateArray(totpoints, "ellipfuncs");
  float* ellipfuncs = ellipfuncsPtr->GetPointer(0);
  ellipfuncsPtr->initializeWithValues(-1);

  float grainsPerTime = 0;
  uint64_t millis = MXA::getMilliSeconds();
  uint64_t currentMillis = millis;
  int cycle = 0;
  int lastUnassignedCount = 0;
  bool sanityFailed = false;

  while (unassignedcount != 0)
  {
    unassignedcount = 0;
    timestep = timestep + 50;
    cycle++;
    for (size_t i = firstPrimaryField; i < m->getNumFieldTuples(); i++)
    {
      grainsPerTime++;
      currentMillis = MXA::getMilliSeconds();
      if (currentMillis - millis > 1000)
      {
        float rate = grainsPerTime / ( (float)(currentMillis-millis) ) * 1000.0f;
        ss.str("");
        ss << "Cycle: " << cycle << "  Grains Checked: " << i << "  Grains/Second: " << (int)rate << "  Unassigned Count: " << lastUnassignedCount;
        notifyStatusMessage(ss.str());
        grainsPerTime = 0;
        millis = MXA::getMilliSeconds();
      }
      float volcur = m_Volumes[i];
      float bovera = m_AxisLengths[3*i+1];
      float covera = m_AxisLengths[3*i+2];
      float omega3 = m_Omega3s[i];
      xc = m_Centroids[3*i];
      yc = m_Centroids[3*i+1];
      zc = m_Centroids[3*i+2];
      float radcur1 = 0.0f;
      //Unbounded Check for the size of shapeTypes. We assume a 1:1 with phase
      unsigned int shapeclass = m_ShapeTypes[m_FieldPhases[i]];

      // init any values for each of the Shape Ops
      for (std::map<unsigned int, ShapeOps*>::iterator ops = m_ShapeOps.begin(); ops != m_ShapeOps.end(); ++ops )
      {
        (*ops).second->init();
      }
      // Create our Argument Map
      std::map<ShapeOps::ArgName, float> shapeArgMap;
      shapeArgMap[ShapeOps::Omega3] = omega3;
      shapeArgMap[ShapeOps::VolCur] = volcur;
      shapeArgMap[ShapeOps::B_OverA] = bovera;
      shapeArgMap[ShapeOps::C_OverA] = covera;

      radcur1 = m_ShapeOps[shapeclass]->radcur1(shapeArgMap);

      float radcur2 = (radcur1 * bovera);
      float radcur3 = (radcur1 * covera);
      radcur1 = static_cast<float>( (float(timestep)/100.0)*radcur1 );
      radcur2 = static_cast<float>( (float(timestep)/100.0)*radcur2 );
      radcur3 = static_cast<float>( (float(timestep)/100.0)*radcur3 );
      float phi1 = m_AxisEulerAngles[3*i];
      float PHI = m_AxisEulerAngles[3*i+1];
      float phi2 = m_AxisEulerAngles[3*i+2];
      float ga[3][3];
      OrientationMath::eulertoMat(phi1, PHI, phi2, ga);
      column = static_cast<DimType>( (xc - (xRes / 2.0f)) / xRes );
      row = static_cast<DimType>( (yc - (yRes / 2.0f)) / yRes );
      plane = static_cast<DimType>( (zc - (zRes / 2.0f)) / zRes );
      xmin = int(column - ((radcur1 / xRes) + 1));
      xmax = int(column + ((radcur1 / xRes) + 1));
      ymin = int(row - ((radcur1 / yRes) + 1)); // <======================
      ymax = int(row + ((radcur1 / yRes) + 1)); // <======================
      zmin = int(plane - ((radcur1 / zRes) + 1)); // <======================
      zmax = int(plane + ((radcur1 / zRes) + 1)); // <======================

      if (m_PeriodicBoundaries == true)
      {
        if (xmin < -dims[0]) xmin = -dims[0];
        if (xmax > 2 * dims[0] - 1) xmax = (2 *dims[0] - 1);
        if (ymin < -dims[1]) ymin = -dims[1];
        if (ymax > 2 * dims[1] - 1) ymax = (2 * dims[1] - 1);
        if (zmin < -dims[2]) zmin = -dims[2];
        if (zmax > 2 * dims[2] - 1) zmax = (2 * dims[2] - 1);
      }
      else
      {
        if (xmin < 0) xmin = 0;
        if (xmax > dims[0] - 1) xmax = dims[0] - 1;
        if (ymin < 0) ymin = 0;
        if (ymax > dims[1] - 1) ymax = dims[1] - 1;
        if (zmin < 0) zmin = 0;
        if (zmax > dims[2] - 1) zmax = dims[2] - 1;
      }
#if 0
      if (i == 1)
      {
        std::cout << xmin << " " << xmax << " " << ymin << " " << ymax << " " << zmin << " " << zmax << std::endl;
      }
#endif
      // Sanity Check the size of the grains:
      // Just check the first grain
      if (xmin == 0 && xmax == dims[0] - 1
          && ymin == 0 && ymax == dims[1] - 1
          && zmin == 0 && zmax == dims[2] - 1 )
      {
        //        sanityFailed = true;
        //        break;
      }

      float radCur[3] = { radcur1, radcur2, radcur3 };
      float xx[3] = {xc, yc, zc };
      ShapeOps* shapeOps = m_ShapeOps[shapeclass];
      //#if 0
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
      tbb::parallel_for(tbb::blocked_range3d<int, int, int>(zmin, zmax+1, ymin, ymax+1, xmin, xmax+1),
                        AssignVoxelsGapsImpl(cycle, dims, res, m_GrainIds, radCur, xx, shapeOps, ga, size, i, newownersPtr, ellipfuncsPtr), tbb::auto_partitioner());

#else
      AssignVoxelsGapsImpl serial(cycle, dims, res, m_GrainIds, radCur, xx, shapeOps, ga, size, i, newownersPtr, ellipfuncsPtr);
      serial.convert(zmin, zmax+1, ymin, ymax+1, xmin, xmax+1);
#endif


    }
    // END OF THE GRAIN LOOP
    if (sanityFailed == true)
    {
      setErrorCondition(-6666);
      notifyErrorMessage("Packing Primary Phases Failed to set all the grains correctly.", getErrorCondition());
      break;
    }

    int gnum;
    for (size_t i = firstPrimaryField; i < m->getNumFieldTuples(); i++)
    {
      m_Active[i] = false;
    }
    for (size_t i = 0; i < static_cast<size_t>(totpoints); i++)
    {
      if(ellipfuncs[i] >= 0) m_GrainIds[i] = newowners[i];
      gnum = m_GrainIds[i];
      if(gnum <= 0) unassignedcount++;
      if(gnum >= 0) m_Active[gnum] = true;
      newowners[i] = -1;
      ellipfuncs[i] = -1.0;
    }

    if(cycle == 0)
    {
      notifyStatusMessage("Assigning Voxels and Gaps - Removing Included Grains");
      RenumberGrains::Pointer renumber_grains1 = RenumberGrains::New();
      renumber_grains1->setObservers(this->getObservers());
      renumber_grains1->setVoxelDataContainer(m);
      renumber_grains1->execute();
      int err = renumber_grains1->getErrorCondition();
      if (err < 0)
      {
        setErrorCondition(renumber_grains1->getErrorCondition());
        addErrorMessages(renumber_grains1->getPipelineMessages());
        return;
      }

      dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
    }


    lastUnassignedCount = unassignedcount;

    if (getCancel() == true)
    {
      unassignedcount = 0;
      ss.str("");
      ss << "Filter Cancelled.";
      notifyWarningMessage(ss.str(), -1);
      setErrorCondition(-1);
      return;
    }
  }
  for (size_t i = firstPrimaryField; i < m->getNumFieldTuples(); i++)
  {
    m_Active[i] = false;
  }
  int gnum;
  for(int64_t i=0;i<totpoints;i++)
  {
    gnum = m_GrainIds[i];
    if(gnum >= 0)
    {
      m_Active[gnum] = true;
      m_CellPhases[i] = m_FieldPhases[gnum];
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::cleanup_grains()
{
  notifyStatusMessage("Cleaning Up Grains");

  VoxelDataContainer* m = getVoxelDataContainer();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  int64_t totpoints = m->getTotalPoints();
  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] = {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  DimType neighpoints[6];
  DimType xp = dims[0];
  DimType yp = dims[1];
  DimType zp = dims[2];

  neighpoints[0] = -(xp * yp);
  neighpoints[1] = -xp;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xp;
  neighpoints[5] = (xp * yp);
  std::vector<std::vector<int> > vlists;
  vlists.resize(m->getNumFieldTuples());
  std::vector<int> currentvlist;
  std::vector<bool> checked;
  checked.resize(totpoints,false);
  size_t count;
  int touchessurface = 0;
  int good;
  int neighbor;
  DimType column, row, plane;
  int index;
  float minsize = 0;
  gsizes.resize(m->getNumFieldTuples());
  for (size_t i = 1; i < m->getNumFieldTuples(); i++)
  {
    gsizes[i] = 0;
    m_Active[i] = true;
  }

  float resConst = m->getXRes() * m->getYRes() * m->getZRes();
  for (int i = 0; i < totpoints; i++)
  {
    touchessurface = 0;
    if(checked[i] == false && m_GrainIds[i] > firstPrimaryField)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[m_CellPhases[i]].get());
      minsize = static_cast<float>( pp->getMinGrainDiameter() * pp->getMinGrainDiameter() * pp->getMinGrainDiameter() * M_PI / 6.0f );
      minsize = static_cast<float>( int(minsize / (resConst)) );
      currentvlist.push_back(i);
      count = 0;
      while (count < currentvlist.size())
      {
        index = currentvlist[count];
        column = index % xp;
        row = (index / xp) % yp;
        plane = index / (xp * yp);
        if(column == 0 || column == xp || row == 0 || row == yp || plane == 0 || plane == zp) touchessurface = 1;
        for (int j = 0; j < 6; j++)
        {
          good = 1;
          neighbor = static_cast<int>( index + neighpoints[j] );
          if(m_PeriodicBoundaries == false)
          {
            if(j == 0 && plane == 0) good = 0;
            if(j == 5 && plane == (zp - 1)) good = 0;
            if(j == 1 && row == 0) good = 0;
            if(j == 4 && row == (yp - 1)) good = 0;
            if(j == 2 && column == 0) good = 0;
            if(j == 3 && column == (xp - 1)) good = 0;
            if(good == 1 && m_GrainIds[neighbor] == m_GrainIds[index] && checked[neighbor] == false)
            {
              currentvlist.push_back(neighbor);
              checked[neighbor] = true;
            }
          }
          else if(m_PeriodicBoundaries == true)
          {
            if(j == 0 && plane == 0) neighbor = static_cast<int>( neighbor + (xp * yp * zp) );
            if(j == 5 && plane == (zp - 1)) neighbor = static_cast<int>( neighbor - (xp * yp * zp) );
            if(j == 1 && row == 0) neighbor = static_cast<int>( neighbor + (xp * yp) );
            if(j == 4 && row == (yp - 1)) neighbor = static_cast<int>( neighbor - (xp * yp) );
            if(j == 2 && column == 0) neighbor = static_cast<int>( neighbor + (xp) );
            if(j == 3 && column == (xp - 1)) neighbor = static_cast<int>( neighbor - (xp) );
            if(m_GrainIds[neighbor] == m_GrainIds[index] && checked[neighbor] == false)
            {
              currentvlist.push_back(neighbor);
              checked[neighbor] = true;
            }
          }
        }
        count++;
      }
      size_t size = vlists[m_GrainIds[i]].size();
      if(size > 0)
      {
        if(size < currentvlist.size())
        {
          for (size_t k = 0; k < vlists[m_GrainIds[i]].size(); k++)
          {
            m_GrainIds[vlists[m_GrainIds[i]][k]] = -1;
          }
          vlists[m_GrainIds[i]].resize(currentvlist.size());
          vlists[m_GrainIds[i]].swap(currentvlist);
        }
        else if(size >= currentvlist.size())
        {
          for (size_t k = 0; k < currentvlist.size(); k++)
          {
            m_GrainIds[currentvlist[k]] = -1;
          }
        }
      }
      else if(size == 0)
      {
        if(currentvlist.size() >= minsize || touchessurface == 1)
        {
          vlists[m_GrainIds[i]].resize(currentvlist.size());
          vlists[m_GrainIds[i]].swap(currentvlist);
        }
        if(currentvlist.size() < minsize && touchessurface == 0)
        {
          for (size_t k = 0; k < currentvlist.size(); k++)
          {
            m_GrainIds[currentvlist[k]] = -1;
          }
        }
      }
      currentvlist.clear();
    }
  }
  assign_voxels_and_gaps();
  for (int i = 0; i < totpoints; i++)
  {
    if(m_GrainIds[i] > 0) gsizes[m_GrainIds[i]]++;
  }
  for (size_t i = firstPrimaryField; i < m->getNumFieldTuples(); i++)
  {
    if(gsizes[i] == 0) m_Active[i] = false;
  }
  for (int i = 0; i < totpoints; i++)
  {
    if(m_GrainIds[i] > 0) { m_CellPhases[i] = m_FieldPhases[m_GrainIds[i]]; }
  }
}

