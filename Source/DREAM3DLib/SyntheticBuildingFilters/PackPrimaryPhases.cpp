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

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DataContainerMacros.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"
#include "DREAM3DLib/Common/PrimaryStatsData.h"
#include "DREAM3DLib/ShapeOps/CubeOctohedronOps.h"
#include "DREAM3DLib/ShapeOps/CylinderOps.h"
#include "DREAM3DLib/ShapeOps/EllipsoidOps.h"
#include "DREAM3DLib/ShapeOps/SuperEllipsoidOps.h"
#include "DREAM3DLib/StatisticsFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/RenumberGrains.h"
#include "DREAM3DLib/IOFilters/FieldDataCSVWriter.h"
#include "DREAM3DLib/IOFilters/DataContainerWriter.h"
#include "DREAM3DLib/Utilities/TimeUtilities.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range3d.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif


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
    float Invradcur[3];
    float res[3];
    int32_t* m_GrainIds;
    float xc;
    float yc;
    float zc;
    ShapeOps* m_ShapeOps;
    float ga[3][3];
    int curGrain;
    Int32ArrayType::Pointer newownersPtr;
    FloatArrayType::Pointer ellipfuncsPtr;

  public:
    AssignVoxelsGapsImpl(DimType* dimensions, float* resolution, int32_t* grainIds, float* radCur,
                         float* xx, ShapeOps* shapeOps, float gA[3][3], float* size, int cur_grain,
    Int32ArrayType::Pointer newowners, FloatArrayType::Pointer ellipfuncs) :
      m_GrainIds(grainIds),
      m_ShapeOps(shapeOps),
      curGrain(cur_grain)
    {
      dims[0] = dimensions[0];
      dims[1] = dimensions[1];
      dims[2] = dimensions[2];
      Invradcur[0] = 1.0/radCur[0];
      Invradcur[1] = 1.0/radCur[1];
      Invradcur[2] = 1.0/radCur[2];

      res[0] = resolution[0];
      res[1] = resolution[1];
      res[2] = resolution[2];

      xc = xx[0];
      yc = xx[1];
      zc = xx[2];

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
   //   float dist = 0.0f;
   //   float radcur1squared = 1.0/(Invradcur[0]*Invradcur[0]);
      float coordsRotated[3] = {0.0f, 0.0f, 0.0f};
      int32_t* newowners = newownersPtr->getPointer(0);
      float* ellipfuncs = ellipfuncsPtr->getPointer(0);
      DimType dim0_dim_1 = dims[0] * dims[1];
      for (DimType iter1 = xStart; iter1 < xEnd; iter1++)
      {
        column = iter1;
        if (iter1 < 0) column = iter1 + dims[0];
        else if (iter1 > dims[0] - 1) column = iter1 - dims[0];

        for (DimType iter2 = yStart; iter2 < yEnd; iter2++)
        {
          row = iter2;
          if (iter2 < 0) row = iter2 + dims[1];
          else if (iter2 > dims[1] - 1) row = iter2 - dims[1];
          size_t row_dim = row * dims[0];

          for (DimType iter3 = zStart; iter3 < zEnd; iter3++)
          {
            plane = iter3;
            if (iter3 < 0) plane = iter3 + dims[2];
            else if (iter3 > dims[2] - 1) plane = iter3 - dims[2];

            index = static_cast<int>( (plane * dim0_dim_1) + (row_dim) + column );

            if(m_GrainIds[index] <= 0)
            {
              inside = -1;
              coords[0] = float(iter1) * res[0];
              coords[1] = float(iter2) * res[1];
              coords[2] = float(iter3) * res[2];

//              dist = ((coords[0] - xc) * (coords[0] - xc)) + ((coords[1] - yc) * (coords[1] - yc)) + ((coords[2] - zc) * (coords[2] - zc));
//              if (dist < radcur1squared)
//              {
                coords[0] = coords[0] - xc;
                coords[1] = coords[1] - yc;
                coords[2] = coords[2] - zc;
                MatrixMath::Multiply3x3with3x1(ga, coords, coordsRotated);
                float axis1comp = coordsRotated[0] * Invradcur[0];
                float axis2comp = coordsRotated[1] * Invradcur[1];
                float axis3comp = coordsRotated[2] * Invradcur[2];
                inside = m_ShapeOps->inside(axis1comp, axis2comp, axis3comp);
                if (inside >= 0 && newowners[index] > 0)
                //if (inside >= 0 && newowners[index] > 0 && inside > ellipfuncs[index])
                {
                  //                    newowners[index] = curGrain;
                  //                    ellipfuncs[index] = inside;
                  newowners[index] = -2;
                  ellipfuncs[index] = inside;
                }
                else if (inside >= 0 && newowners[index] == -1)
                {
                  newowners[index] = curGrain;
                  ellipfuncs[index] = inside;
                }
//              }
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
  m_CsvOutputFile(""),
  m_PeriodicBoundaries(false),
  m_WriteGoalAttributes(false),
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

  m_OrthoOps = OrthoRhombicOps::New();

  m_HalfPackingRes[0] = m_HalfPackingRes[1] = m_HalfPackingRes[2] = 1.0f;
  m_OneOverHalfPackingRes[0] = m_OneOverHalfPackingRes[1] = m_OneOverHalfPackingRes[2] = 1.0f;

  Seed = QDateTime::currentMSecsSinceEpoch();
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
  QVector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Periodic Boundary");
    option->setPropertyName("PeriodicBoundaries");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Goal Attributes");
    option->setPropertyName("WriteGoalAttributes");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Goal Attribute CSV File");
    option->setPropertyName("CsvOutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("*.csv");
    option->setFileType("Comma Separated Data");
    option->setValueType("string");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void PackPrimaryPhases::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setPeriodicBoundaries( reader->readValue("PeriodicBoundaries", false) );
  setWriteGoalAttributes( reader->readValue("WriteGoalAttributes", false) );
  setCsvOutputFile( reader->readValue( "CsvOutputFile", getCsvOutputFile() ) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PackPrimaryPhases::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("PeriodicBoundaries", getPeriodicBoundaries() );
  writer->writeValue("WriteGoalAttributes", getWriteGoalAttributes() );
  writer->writeValue("CsvOutputFile", getCsvOutputFile() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getVolumeDataContainer();

  //Cell Data
  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, -301, int32_t, Int32ArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, -302, int32_t, Int32ArrayType, voxels, 1)

  //Field Data
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFieldData, FieldPhases, int32_t, Int32ArrayType, 0, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFieldData, EquivalentDiameters, float, FloatArrayType, 0, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFieldData, Omega3s, float, FloatArrayType, 0, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFieldData, AxisEulerAngles, float, FloatArrayType, 0, fields, 3)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFieldData, AxisLengths, float, FloatArrayType, 0, fields, 3)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFieldData, Volumes, float, FloatArrayType, 0, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFieldData, Centroids, float, FloatArrayType, 0, fields, 3)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFieldData, Active, bool, BoolArrayType, true, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellFieldData, Neighborhoods, int32_t, Int32ArrayType, 0, fields, 1)

  //Ensemble Data
  typedef DataArray<unsigned int> PhaseTypeArrayType;
  typedef DataArray<unsigned int> ShapeTypeArrayType;
  GET_PREREQ_DATA(m, DREAM3D, CellEnsembleData, PhaseTypes, -302, unsigned int, PhaseTypeArrayType, ensembles, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellEnsembleData, ShapeTypes, -305, unsigned int, ShapeTypeArrayType, ensembles, 1)
  m_StatsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(m->getCellEnsembleData(DREAM3D::EnsembleData::Statistics).get());
  if(m_StatsDataArray == NULL)
  {
    QString ss = QObject::tr("Stats Array Not Initialized correctly");
    setErrorCondition(-308);
    addErrorMessage(getHumanLabel(), ss, -308);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::preflight()
{
  dataCheck(true, 1, 1, 1);

  if (m_WriteGoalAttributes == true && getCsvOutputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Csv Output File Set and it was not.").arg(ClassName());
    addErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-387);
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::execute()
{
  VolumeDataContainer* m = getVolumeDataContainer();

  bool writeErrorFile = true;
  std::ofstream outFile;
  if(m_ErrorOutputFile.isEmpty() == false)
  {
    outFile.open(m_ErrorOutputFile.toLatin1().data(), std::ios_base::binary);
    writeErrorFile = true;
  }

  int err = 0;
  setErrorCondition(err);
  unsigned long long int Seed = QDateTime::currentMSecsSinceEpoch();
  DREAM3D_RANDOMNG_NEW_SEEDED(Seed);

  int64_t totalPoints = m->getTotalPoints();
  size_t totalFields = m->getNumCellFieldTuples();
  if(totalFields == 0) totalFields = 1;
  dataCheck(false, totalPoints, totalFields, m->getNumCellEnsembleTuples());
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

  size_t numensembles = m->getNumCellEnsembleTuples();


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
        QString ss = QObject::tr("Tried to cast a statsDataArray[%1].get() to a PrimaryStatsData* "
        "pointer but this resulted in a NULL pointer. The value at m_PhaseTypes[%2] = %3 does not match up "
        "with the type of pointer stored in the StatsDataArray (PrimaryStatsData)\n")
        .arg(i).arg(i).arg(m_PhaseTypes[i]);
        PipelineMessage em (getHumanLabel(), ss, -666);
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
  }

  notifyStatusMessage("Packing Grains - Initializing Volume");
  // this initializes the arrays to hold the details of the locations of all of the grains during packing
  initialize_packinggrid();

  Int32ArrayType::Pointer grainOwnersPtr = Int32ArrayType::CreateArray(m_TotalPackingPoints, 1, "PackPrimaryGrains::grain_owners");
  grainOwnersPtr->initializeWithZeros();
  BoolArrayType::Pointer exclusionZonesPtr = BoolArrayType::CreateArray(m_TotalPackingPoints, 1, "PackPrimaryGrains::exclusions_zones");
  exclusionZonesPtr->initializeWithValues(false);

  // Get a pointer to the Grain Owners that was just initialized in the initialize_packinggrid() method
  int32_t* grainOwners = grainOwnersPtr->getPointer(0);
  bool* exclusionZones = exclusionZonesPtr->getPointer(0);
  size_t grainOwnersIdx = 0;

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
    float denominatorConst = 1.0/sqrtf(2.0f * stdev * stdev); // Calculate it here rather than calculating the same thing multiple times below
    for (size_t j = 0; j < grainsizedist[i].size(); j++)
    {
      input = (float(j + 1) * grainsizediststep[i]) + (pp->getMinGrainDiameter() / 2.0f);
      float logInput = logf(input);
      if(logInput <= avg)
      {
        grainsizedist[i][j] = 0.5f - 0.5f * (DREAM3DMath::erf((avg - logInput) * denominatorConst)) - previoustotal;
      }
      if(logInput > avg)
      {
        grainsizedist[i][j] = 0.5f + 0.5f * (DREAM3DMath::erf((logInput - avg) * denominatorConst)) - previoustotal;
      }
      previoustotal = previoustotal + grainsizedist[i][j];
    }
  }

  if (getCancel() == true)
  {
    QString ss = QObject::tr("Filter Cancelled.");
    notifyWarningMessage(ss, -1);
    setErrorCondition(-1);
    return;
  }

  // generate the grains and monitor the size distribution error while doing so. After grains are generated, no new grains can enter or leave the structure.
  Field field;

  // Estimate the total Number of grains here
  int estNumGrains = estimate_numgrains((int)(udims[0]), (int)(udims[1]), (int)(udims[2]), xRes, yRes, zRes);
  m->resizeCellFieldDataArrays(estNumGrains);
  dataCheck(false, totalPoints, estNumGrains, m->getNumCellEnsembleTuples());

  int gid = 1;
  firstPrimaryField = gid;
  QVector<float> curphasevol;
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
      generate_grain(phase, static_cast<int>(Seed), &field, m_ShapeTypes[phase]);
      currentsizedisterror = check_sizedisterror(&field);
      change = (currentsizedisterror) - (oldsizedisterror);
      if(change > 0 || currentsizedisterror > (1.0 - (float(iter) * 0.001)) || curphasevol[j] < (0.75* factor * curphasetotalvol))
      {

        QString ss = QObject::tr("Packing Grains (1/2) - Generating Grain #%1").arg(gid);
        notifyStatusMessage(ss);
        if (gid + 1 >= static_cast<int>(m->getNumCellFieldTuples()))
        {
          m->resizeCellFieldDataArrays(gid + 1);
          dataCheck(false, totalPoints, gid + 1, m->getNumCellEnsembleTuples());
        }

        m_Active[gid] = true;
        transfer_attributes(gid, &field);
        oldsizedisterror = currentsizedisterror;
        curphasevol[j] = curphasevol[j] + m_Volumes[gid];
        iter = 0;
        gid++;
      }
        if (getCancel() == true)
        {
          QString ss = QObject::tr("Filter Cancelled.");
          notifyWarningMessage(ss, -1);
          setErrorCondition(-1);
          return;
        }

    }
  }


  if(m_PeriodicBoundaries == false)
  {
    iter = 0;
    int xgrains, ygrains, zgrains;
    xgrains = int(powf((m->getNumCellFieldTuples()*(sizex/sizey)*(sizex/sizez)),(1.0f/3.0f))+1);
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
        generate_grain(phase, static_cast<int>(Seed), &field, m_ShapeTypes[phase]);
        currentsizedisterror = check_sizedisterror(&field);
        change = (currentsizedisterror) - (oldsizedisterror);
        if(change > 0 || currentsizedisterror > (1.0 - (iter * 0.001)) || curphasevol[j] < (0.75* factor * curphasetotalvol))
        {

          QString ss = QObject::tr("Packing Grains (2/2) - Generating Grain #").arg(gid);
          notifyStatusMessage(ss);
          if (gid + 1 >= static_cast<int>(m->getNumCellFieldTuples()) )
          {
            m->resizeCellFieldDataArrays(gid + 1);
            dataCheck(false, totalPoints, gid + 1, m->getNumCellEnsembleTuples());
          }

          m_Active[gid] = true;
          transfer_attributes(gid, &field);
          oldsizedisterror = currentsizedisterror;
          curphasevol[j] = curphasevol[j] + m_Volumes[gid];
          iter = 0;
          gid++;
        }
        if (getCancel() == true)
        {
          QString ss = QObject::tr("Filter Cancelled.");
          notifyWarningMessage(ss, -1);
          setErrorCondition(-1);
          return;
        }
      }
    }
  }

  m->resizeCellFieldDataArrays(gid);
  dataCheck(false, totalPoints, m->getNumCellFieldTuples(), m->getNumCellEnsembleTuples());

  if (getCancel() == true)
  {

    QString ss = QObject::tr("Filter Cancelled.");
    notifyWarningMessage(ss, -1);
    setErrorCondition(-1);
    return;
  }

  notifyStatusMessage("Initializing Neighbor Distributions");

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
      float denominatorConst = 1.0/sqrtf(2.0f * stdev * stdev); // Calculate it here rather than calculating the same thing multiple times below
      for (size_t k = 0; k < neighbordist[i][j].size(); k++)
      {
        input = (float(k + 1) * neighbordiststep[i]);
        float logInput = logf(input);
        if(logInput <= avg)
        {
          neighbordist[i][j][k] = 0.5f - 0.5f * (DREAM3DMath::erf((avg - logInput) * denominatorConst )) - previoustotal;
        }
        if(logInput > avg)
        {
          neighbordist[i][j][k] = 0.5f + 0.5f * (DREAM3DMath::erf((logInput - avg) * denominatorConst)) - previoustotal;
        }
        previoustotal = previoustotal + neighbordist[i][j][k];
      }
      normalizer = normalizer + previoustotal;
    }
    normalizer = 1.0/normalizer;
    for (size_t j = 0; j < neighbordist[i].size(); j++)
    {
      for (size_t k = 0; k < neighbordist[i][j].size(); k++)
      {
        neighbordist[i][j][k] = neighbordist[i][j][k] * normalizer;
      }
    }
  }

  if (getCancel() == true)
  {

    QString ss = QObject::tr("Filter Cancelled.");
    notifyWarningMessage(ss, -1);
    setErrorCondition(-1);
    return;
  }

  //  for each grain : select centroid, determine voxels in grain, monitor filling error and decide of the 10 placements which
  // is the most beneficial, then the grain is added and its neighbors are determined

  size_t numgrains = m->getNumCellFieldTuples();

  columnlist.resize(numgrains);
  rowlist.resize(numgrains);
  planelist.resize(numgrains);
  ellipfunclist.resize(numgrains);
  packqualities.resize(numgrains);
  fillingerror = 1;

  int count = 0;
  int column, row, plane;
  int progGrain = 0;
  int progGrainInc = numgrains * .01;
  for (size_t i = firstPrimaryField; i < numgrains; i++)
  {
    if ((int)i > progGrain + progGrainInc)
    {

      QString ss = QObject::tr("Placing Grain #%1/%2").arg(i).arg(numgrains);
      notifyStatusMessage(ss);
      progGrain = i;
    }


    xc = sizex * 0.5f;
    yc = sizey * 0.5f;
    zc = sizez * 0.5f;
    m_Centroids[3 * i] = xc;
    m_Centroids[3 * i + 1] = yc;
    m_Centroids[3 * i + 2] = zc;
    insert_grain(i);
    count = 0;
    column = static_cast<int>( (xc - (m_HalfPackingRes[0])) * m_OneOverPackingRes[0] );
    row = static_cast<int>( (yc - (m_HalfPackingRes[1])) * m_OneOverPackingRes[1] );
    plane = static_cast<int>( (zc - (m_HalfPackingRes[2])) * m_OneOverPackingRes[2] );
    grainOwnersIdx = (m_PackingPoints[0]*m_PackingPoints[1]*plane) + (m_PackingPoints[0]*row) + column;
    while(exclusionZones[grainOwnersIdx] == true && count < m_TotalPackingPoints)
    {
      column++;
      if(column >= m_PackingPoints[0])
      {
        column = 0;
        row++;
        if(row >= m_PackingPoints[1])
        {
          row = 0;
          plane++;
          if(plane >= m_PackingPoints[2])
          {
            plane = 0;
          }
        }
      }
      grainOwnersIdx = (m_PackingPoints[0]*m_PackingPoints[1]*plane) + (m_PackingPoints[0]*row) + column;
      count++;
    }
    xc = static_cast<float>((column*m_PackingRes[0]) + (m_PackingRes[0]*0.5));
    yc = static_cast<float>((row*m_PackingRes[1]) + (m_PackingRes[1]*0.5));
    zc = static_cast<float>((plane*m_PackingRes[2]) + (m_PackingRes[2]*0.5));
    move_grain(i, xc, yc, zc);
    fillingerror = check_fillingerror(i, -1000, grainOwnersPtr, exclusionZonesPtr);

    if (getCancel() == true)
    {

      QString ss = QObject::tr("Filter Cancelled.");
      notifyWarningMessage(ss, -1);
      setErrorCondition(-1);
      return;
    }
  }

  notifyStatusMessage("Determining Neighbors");
  progGrain = 0;
  progGrainInc = numgrains * .01;
  uint64_t millis = QDateTime::currentMSecsSinceEpoch();
  uint64_t currentMillis = millis;
  uint64_t startMillis = millis;
  uint64_t estimatedTime = 0;
  float timeDiff = 0.0f;

  // determine neighborhoods and initial neighbor distribution errors
  for (size_t i = firstPrimaryField; i < numgrains; i++)
  {
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if (currentMillis - millis > 1000)
    {

      QString ss = QObject::tr("Determining Neighbors %1/%2").arg(i).arg(numgrains);
      timeDiff = ((float)i / (float)(currentMillis - startMillis));
      estimatedTime = (float)(numgrains - i) / timeDiff;
      ss = QObject::tr(" Est. Time Remain: %1").arg(DREAM3D::convertMillisToHrsMinSecs(estimatedTime));
      notifyStatusMessage(ss);

      millis = QDateTime::currentMSecsSinceEpoch();
    }
    determine_neighbors(i, 1);
  }
  oldneighborhooderror = check_neighborhooderror(-1000, -1000);
  // begin swaping/moving/adding/removing grains to try to improve packing
  int totalAdjustments = static_cast<int>(10 * (numgrains-1));

  millis = QDateTime::currentMSecsSinceEpoch();
  startMillis = millis;
  bool good;
  float xshift, yshift, zshift;
  int lastIteration = 0;
  int numIterationsPerTime = 0;
  for (int iteration = 0; iteration < totalAdjustments; ++iteration)
  {
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if (currentMillis - millis > 1000)
    {

      QString ss = QObject::tr("Swapping/Moving/Adding/Removing Grains Iteration %1/%2").arg(iteration).arg(totalAdjustments);
      timeDiff = ((float)iteration / (float)(currentMillis - startMillis));
      estimatedTime = (float)(totalAdjustments - iteration) / timeDiff;

      ss = QObject::tr(" || Est. Time Remain: %1 || Iterations/Sec: %2").arg(DREAM3D::convertMillisToHrsMinSecs(estimatedTime)).arg(timeDiff * 1000);
      notifyStatusMessage(ss);

      millis = QDateTime::currentMSecsSinceEpoch();

      numIterationsPerTime = iteration - lastIteration;
      lastIteration = iteration;
    }


    if (getCancel() == true)
    {

      QString ss = QObject::tr("Filter Cancelled.");
      notifyWarningMessage(ss, -1);
      setErrorCondition(-1);
      return;
    }

    int option = iteration % 2;

    if(writeErrorFile == true && iteration % 25 == 0)
    {
      outFile << iteration << " " << fillingerror << "  " << oldsizedisterror << "  " << oldneighborhooderror << "  " << numgrains << " " << acceptedmoves
              << "\n";
    }

    // JUMP - this option moves one grain to a random spot in the volume
    if(option == 0)
    {
      randomgrain = firstPrimaryField + int(rg.genrand_res53() * (numgrains-firstPrimaryField));
      good = false;
      count = 0;
      while(good == false && count < static_cast<int>((numgrains-firstPrimaryField)) )
      {
        xc = m_Centroids[3*randomgrain];
        yc = m_Centroids[3*randomgrain+1];
        zc = m_Centroids[3*randomgrain+2];
        column = static_cast<int>( (xc - (m_HalfPackingRes[0])) * m_OneOverPackingRes[0] );
        row = static_cast<int>( (yc - (m_HalfPackingRes[1])) * m_OneOverPackingRes[1] );
        plane = static_cast<int>( (zc - (m_HalfPackingRes[2])) * m_OneOverPackingRes[2] );
        grainOwnersIdx = (m_PackingPoints[0]*m_PackingPoints[1]*plane) + (m_PackingPoints[0]*row) + column;
        if(grainOwners[grainOwnersIdx] > 1) good = true;
        else randomgrain++;
        if(static_cast<size_t>(randomgrain) >= numgrains) randomgrain = firstPrimaryField;
        count++;
      }
      Seed++;

      count = 0;
      xc = static_cast<float>(rg.genrand_res53() * sizex);
      yc = static_cast<float>(rg.genrand_res53() * sizey);
      zc = static_cast<float>(rg.genrand_res53() * sizez);
      column = static_cast<int>( (xc - (m_HalfPackingRes[0])) * m_OneOverPackingRes[0] );
      row = static_cast<int>( (yc - (m_HalfPackingRes[1])) * m_OneOverPackingRes[1] );
      plane = static_cast<int>( (zc - (m_HalfPackingRes[2])) * m_OneOverPackingRes[2] );

      while(exclusionZones[grainOwnersIdx] == true && count < m_TotalPackingPoints)
      {
        grainOwnersIdx = (m_PackingPoints[0]*m_PackingPoints[1]*plane) + (m_PackingPoints[0]*row) + column;
        if(column >= m_PackingPoints[0])
        {
          column = 0;
          row++;
          if(row >= m_PackingPoints[1])
          {
            row = 0;
            plane++;
            if(plane >= m_PackingPoints[2])
            {
              plane = 0;
            }
          }
        }
        count++;
      }
      xc = static_cast<float>((column*m_PackingRes[0]) + (m_PackingRes[0]*0.5));
      yc = static_cast<float>((row*m_PackingRes[1]) + (m_PackingRes[1]*0.5));
      zc = static_cast<float>((plane*m_PackingRes[2]) + (m_PackingRes[2]*0.5));
      oldxc = m_Centroids[3 * randomgrain];
      oldyc = m_Centroids[3 * randomgrain + 1];
      oldzc = m_Centroids[3 * randomgrain + 2];
      oldfillingerror = fillingerror;
      fillingerror = check_fillingerror(-1000, static_cast<int>(randomgrain), grainOwnersPtr, exclusionZonesPtr);
      move_grain(randomgrain, xc, yc, zc);
      fillingerror = check_fillingerror(static_cast<int>(randomgrain), -1000, grainOwnersPtr, exclusionZonesPtr);
      currentneighborhooderror = check_neighborhooderror(-1000, randomgrain);
      if(fillingerror <= oldfillingerror)
      {
        oldneighborhooderror = currentneighborhooderror;
        acceptedmoves++;
      }
      else if(fillingerror > oldfillingerror)
      {
        fillingerror = check_fillingerror(-1000, static_cast<int>(randomgrain), grainOwnersPtr, exclusionZonesPtr);
        move_grain(randomgrain, oldxc, oldyc, oldzc);
        fillingerror = check_fillingerror(static_cast<int>(randomgrain), -1000, grainOwnersPtr, exclusionZonesPtr);
      }
    }
    // NUDGE - this option moves one grain to a spot close to its current centroid
    if(option == 1)
    {
      randomgrain = firstPrimaryField + int(rg.genrand_res53() * (numgrains-firstPrimaryField));
      good = false;
      count = 0;
      while(good == false && count < static_cast<int>((numgrains-firstPrimaryField)) )
      {
        xc = m_Centroids[3*randomgrain];
        yc = m_Centroids[3*randomgrain+1];
        zc = m_Centroids[3*randomgrain+2];
        column = static_cast<int>( (xc - (m_HalfPackingRes[0])) * m_OneOverPackingRes[0] );
        row = static_cast<int>( (yc - (m_HalfPackingRes[1])) * m_OneOverPackingRes[1] );
        plane = static_cast<int>( (zc - (m_HalfPackingRes[2])) * m_OneOverPackingRes[2] );
        grainOwnersIdx = (m_PackingPoints[0]*m_PackingPoints[1]*plane) + (m_PackingPoints[0]*row) + column;
        if(grainOwners[grainOwnersIdx] > 1) good = true;
        else randomgrain++;
        if(static_cast<size_t>(randomgrain) >= numgrains) randomgrain = firstPrimaryField;
        count++;
      }
      Seed++;
      oldxc = m_Centroids[3 * randomgrain];
      oldyc = m_Centroids[3 * randomgrain + 1];
      oldzc = m_Centroids[3 * randomgrain + 2];
      xshift = static_cast<float>(((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * m_PackingRes[0])) );
      yshift = static_cast<float>(((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * m_PackingRes[1])) );
      zshift = static_cast<float>(((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * m_PackingRes[2])) );
      if((oldxc+xshift) < sizex && (oldxc+xshift) > 0) xc = oldxc + xshift;
      else xc = oldxc;
      if((oldyc+yshift) < sizey && (oldyc+yshift) > 0) yc = oldyc + yshift;
      else yc = oldyc;
      if((oldzc+zshift) < sizez && (oldzc+zshift) > 0) zc = oldzc + zshift;
      else zc = oldzc;
      oldfillingerror = fillingerror;
      fillingerror = check_fillingerror(-1000, static_cast<int>(randomgrain), grainOwnersPtr, exclusionZonesPtr);
      move_grain(randomgrain, xc, yc, zc);
      fillingerror = check_fillingerror(static_cast<int>(randomgrain), -1000, grainOwnersPtr, exclusionZonesPtr);
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
        fillingerror = check_fillingerror(-1000, static_cast<int>(randomgrain), grainOwnersPtr, exclusionZonesPtr);
        move_grain(randomgrain, oldxc, oldyc, oldzc);
        fillingerror = check_fillingerror(static_cast<int>(randomgrain), -1000, grainOwnersPtr, exclusionZonesPtr);
      }
    }
  }

  notifyStatusMessage("Packing Grains - Grain Adjustment Complete");

  if(m_VtkOutputFile.isEmpty() == false)
  {
    err = writeVtkFile(grainOwnersPtr->getPointer(0), exclusionZonesPtr->getPointer(0));
    if(err < 0)
    {
      return;
    }
  }

  notifyStatusMessage("Packing Grains - Assigning Voxels");
  assign_voxels();
  if (getCancel() == true) { return; }

  notifyStatusMessage("Packing Grains - Assigning Gaps");
  assign_gaps_only();
  if (getCancel() == true) { return; }

  notifyStatusMessage("Packing Grains - Cleaning Up Volume");
  //  cleanup_grains();
  if (getCancel() == true) { return; }

  notifyStatusMessage("Packing Grains - Renumbering Grains");
  RenumberGrains::Pointer renumber_grains2 = RenumberGrains::New();
  renumber_grains2->setObservers(this->getObservers());
  renumber_grains2->setVolumeDataContainer(m);
  renumber_grains2->execute();
  err = renumber_grains2->getErrorCondition();
  if (err < 0)
  {
    setErrorCondition(renumber_grains2->getErrorCondition());
    addErrorMessages(renumber_grains2->getPipelineMessages());
    return;
  }

  if(m_WriteGoalAttributes == true)
  {
    write_goal_attributes();
  }

  m->removeCellFieldData(m_EquivalentDiametersArrayName);
  m->removeCellFieldData(m_Omega3sArrayName);
  m->removeCellFieldData(m_AxisEulerAnglesArrayName);
  m->removeCellFieldData(m_AxisLengthsArrayName);
  m->removeCellFieldData(m_VolumesArrayName);
  m->removeCellFieldData(m_CentroidsArrayName);
  m->removeCellFieldData(m_NeighborhoodsArrayName);

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("Packing Grains Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PackPrimaryPhases::writeVtkFile(int32_t* grainOwners, bool* exclusionZones)
{
  size_t grainOwnersIdx = 0;
  std::ofstream outFile;
  outFile.open(m_VtkOutputFile.toLatin1().data(), std::ios_base::binary);
  if(outFile.is_open() == false)
  {
    qDebug() << "m_VtkOutputFile: " << m_VtkOutputFile << "\n";
    PipelineMessage em (getHumanLabel(), "Could not open Vtk File for writing from PackGrains", -1);
    addErrorMessage(em);
    setErrorCondition(-55);
    return -1;
  }
  outFile << "# vtk DataFile Version 2.0" << "\n";
  outFile << "DREAM.3D Generated from PackPrimaryPhases Filter" << "\n";
  outFile << "ASCII" << "\n";
  outFile << "DATASET STRUCTURED_POINTS" << "\n";
  outFile << "DIMENSIONS " << m_PackingPoints[0] << " " << m_PackingPoints[1] << " " << m_PackingPoints[2] << "\n";
  outFile << "ORIGIN 0.0 0.0 0.0" << "\n";
  outFile << "SPACING " << m_PackingRes[0] << " " << m_PackingRes[1] << " " << m_PackingRes[2] << "\n";
  outFile << "POINT_DATA " << m_PackingPoints[0] * m_PackingPoints[1] * m_PackingPoints[2] << "\n";
  outFile << "\n";
  outFile << "\n";
  outFile << "SCALARS NumOwners int  1" << "\n";
  outFile << "LOOKUP_TABLE default" << "\n";
  for (int i = 0; i < (m_PackingPoints[2]); i++)
  {
    for (int j = 0; j < (m_PackingPoints[1]); j++)
    {
      for (int k = 0; k < (m_PackingPoints[0]); k++)
      {
        grainOwnersIdx = (m_PackingPoints[0]*m_PackingPoints[1]*i) + (m_PackingPoints[0]*j) + k;
        int name = grainOwners[grainOwnersIdx];
        if(i % 20 == 0 && i > 0) outFile << "\n";
        outFile << "     ";
        if(name < 100) outFile << " ";
        if(name < 10) outFile << " ";
        outFile << name;
      }
    }
  }
  outFile << "SCALARS ExclusionZone int  1" << "\n";
  outFile << "LOOKUP_TABLE default" << "\n";
  for (int i = 0; i < (m_PackingPoints[2]); i++)
  {
    for (int j = 0; j < (m_PackingPoints[1]); j++)
    {
      for (int k = 0; k < (m_PackingPoints[0]); k++)
      {
        grainOwnersIdx = (m_PackingPoints[0]*m_PackingPoints[1]*i) + (m_PackingPoints[0]*j) + k;
        bool val = exclusionZones[grainOwnersIdx];
        if(i % 20 == 0 && i > 0) outFile << "\n";
        outFile << "       ";
        if(val == true) outFile << 1;
        else outFile << 0;
      }
    }
  }
  outFile.close();
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::initialize_packinggrid()
{
  VolumeDataContainer* m = getVolumeDataContainer();

  m_PackingRes[0] = m->getXRes() * 2.0f;
  m_PackingRes[1] = m->getYRes() * 2.0f;
  m_PackingRes[2] = m->getZRes() * 2.0f;

  m_HalfPackingRes[0] = m_PackingRes[0]*0.5;
  m_HalfPackingRes[1] = m_PackingRes[1]*0.5;
  m_HalfPackingRes[2] = m_PackingRes[2]*0.5;

  m_OneOverHalfPackingRes[0] = 1.0f/m_HalfPackingRes[0];
  m_OneOverHalfPackingRes[1] = 1.0f/m_HalfPackingRes[1];
  m_OneOverHalfPackingRes[2] = 1.0f/m_HalfPackingRes[2];

  m_OneOverPackingRes[0] = 1.0f/m_PackingRes[0];
  m_OneOverPackingRes[1] = 1.0f/m_PackingRes[1];
  m_OneOverPackingRes[2] = 1.0f/m_PackingRes[2];

  m_PackingPoints[0] = m->getXPoints()/2;
  m_PackingPoints[1] = m->getYPoints()/2;
  m_PackingPoints[2] = m->getZPoints()/2;

  m_TotalPackingPoints = m_PackingPoints[0] * m_PackingPoints[1] * m_PackingPoints[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::generate_grain(int phase, int Seed, Field* field, unsigned int shapeclass)
{
  DREAM3D_RANDOMNG_NEW_SEEDED(Seed)

      StatsDataArray& statsDataArray = *m_StatsDataArray;

  float r1 = 1;
  float a2 = 0, a3 = 0;
  float b2 = 0, b3 = 0;
  float diam = 0;
  float vol = 0;
  int volgood = 0;
  float phi1, PHI, phi2;
  float fourThirdsPiOverEight =  static_cast<float>(((4.0f / 3.0f) * (DREAM3D::Constants::k_Pi))/8.0f);
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
    vol = fourThirdsPiOverEight * (diam * diam * diam);
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
  m_OrthoOps->determineEulerAngles(bin, phi1, PHI, phi2);
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::move_grain(size_t gnum, float xc, float yc, float zc)
{
  int occolumn, ocrow, ocplane;
  int nccolumn, ncrow, ncplane;
  int shiftcolumn, shiftrow, shiftplane;
  float oxc = m_Centroids[3*gnum];
  float oyc = m_Centroids[3*gnum+1];
  float ozc = m_Centroids[3*gnum+2];
  occolumn = static_cast<int>( (oxc - (m_HalfPackingRes[0])) * m_OneOverPackingRes[0] );
  ocrow = static_cast<int>( (oyc - (m_HalfPackingRes[1])) * m_OneOverPackingRes[1] );
  ocplane = static_cast<int>( (ozc - (m_HalfPackingRes[2])) * m_OneOverPackingRes[2] );
  nccolumn = static_cast<int>( (xc - (m_HalfPackingRes[0])) * m_OneOverPackingRes[0] );
  ncrow = static_cast<int>( (yc - (m_HalfPackingRes[1])) * m_OneOverPackingRes[1] );
  ncplane = static_cast<int>( (zc - (m_HalfPackingRes[2])) * m_OneOverPackingRes[2] );
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::determine_neighbors(size_t gnum, int add)
{
  VolumeDataContainer* m = getVolumeDataContainer();
  float x, y, z;
  float xn, yn, zn;
  float dia, dia2;
  float dx, dy, dz;
  x = m_Centroids[3*gnum];
  y = m_Centroids[3*gnum+1];
  z = m_Centroids[3*gnum+2];
  dia = m_EquivalentDiameters[gnum];
  size_t numFieldTuples = m->getNumCellFieldTuples();
  int32_t increment = 0;
  if(add > 0) { increment = 1; }
  if(add < 0) { increment = -1; }
  for (size_t n = firstPrimaryField; n < numFieldTuples; n++)
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
      m_Neighborhoods[gnum] = m_Neighborhoods[gnum] + increment;
    }
    if(dx < dia2 && dy < dia2 && dz < dia2)
    {
      m_Neighborhoods[n] = m_Neighborhoods[n] + increment;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float PackPrimaryPhases::check_neighborhooderror(int gadd, int gremove)
{
  // Optimized Code
  VolumeDataContainer* m = getVolumeDataContainer();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  float neighborerror;
  float bhattdist;
  float dia;
  int nnum;
  size_t diabin = 0;
  size_t nnumbin = 0;
  int index = 0;

  int phase;
  typedef QVector<QVector<float> > VectOfVectFloat_t;
  for (size_t iter = 0; iter < simneighbordist.size(); ++iter)
  {
    phase = primaryphases[iter];
    PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
    VectOfVectFloat_t& curSimNeighborDist = simneighbordist[iter];
    size_t curSImNeighborDist_Size = curSimNeighborDist.size();
    float oneOverNeighborDistStep = 1.0f/neighbordiststep[iter];

    QVector<int> count(curSImNeighborDist_Size, 0);
    for (size_t i = 0; i < curSImNeighborDist_Size; i++)
    {
      curSimNeighborDist[i].resize(40);
      for (size_t j = 0; j < 40; j++)
      {
        curSimNeighborDist[i][j] = 0;
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

    float maxGrainDia = pp->getMaxGrainDiameter();
    float minGrainDia = pp->getMinGrainDiameter();
    float oneOverBinStepSize = 1.0f/pp->getBinStepSize();


    for (size_t i = firstPrimaryField; i < m->getNumCellFieldTuples(); i++)
    {
      nnum = 0;
      index = i;
      if(index != gremove && m_FieldPhases[index] == phase)
      {
        dia = m_EquivalentDiameters[index];
        if(dia > maxGrainDia) { dia = maxGrainDia; }
        if(dia < minGrainDia) { dia = minGrainDia; }
        diabin = static_cast<size_t>(((dia - minGrainDia) * oneOverBinStepSize) );
        nnum = m_Neighborhoods[index];
        nnumbin = static_cast<size_t>( nnum * oneOverNeighborDistStep );
        if(nnumbin >= 40) { nnumbin = 39; }
        curSimNeighborDist[diabin][nnumbin]++;
        count[diabin]++;
      }
    }
    if(gadd > 0 && m_FieldPhases[gadd] == phase)
    {
      dia = m_EquivalentDiameters[gadd];
      if(dia > maxGrainDia) { dia = maxGrainDia; }
      if(dia < minGrainDia) { dia = minGrainDia; }
      diabin = static_cast<size_t>(((dia - minGrainDia) * oneOverBinStepSize) );
      nnum = m_Neighborhoods[gadd];
      nnumbin = static_cast<size_t>( nnum * oneOverNeighborDistStep );
      if(nnumbin >= 40) { nnumbin = 39; }
      curSimNeighborDist[diabin][nnumbin]++;
      count[diabin]++;
    }
    float runningtotal = 0.0f;

    for (size_t i = 0; i < curSImNeighborDist_Size; i++)
    {
      if (count[i] == 0)
      {
        for (size_t j = 0; j < 40; j++)
        {
          curSimNeighborDist[i][j] = 0.0f;
          runningtotal = runningtotal + 0.0f;
        }
      }
      else
      {
        float oneOverCount = 1.0f / (float)(count[i]);
        for (size_t j = 0; j < 40; j++)
        {
          curSimNeighborDist[i][j] = curSimNeighborDist[i][j] * oneOverCount;
          runningtotal = runningtotal + curSimNeighborDist[i][j];
        }
      }
    }

    runningtotal = 1.0f / runningtotal; // Flip this so that we have a multiply instead of a divide.
    for (size_t i = 0; i < curSImNeighborDist_Size; i++)
    {
      for (size_t j = 0; j < 40; j++)
      {
        curSimNeighborDist[i][j] = curSimNeighborDist[i][j] * runningtotal;
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::compare_1Ddistributions(QVector<float> array1, QVector<float> array2, float &bhattdist)
{
  bhattdist = 0;
  for (size_t i = 0; i < array1.size(); i++)
  {
    bhattdist = bhattdist + sqrt((array1[i]*array2[i]));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::compare_2Ddistributions(QVector<QVector<float> > array1, QVector<QVector<float> > array2, float &bhattdist)
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::compare_3Ddistributions(QVector<QVector<QVector<float> > > array1, QVector<QVector<QVector<float> > > array2, float &bhattdist)
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float PackPrimaryPhases::check_sizedisterror(Field* field)
{
  VolumeDataContainer* m = getVolumeDataContainer();

  StatsDataArray& statsDataArray = *m_StatsDataArray;

  float dia;
  float sizedisterror = 0;
  float bhattdist;
  int index;
  int count = 0;
  int phase;
  size_t grainSizeDist_Size = grainsizedist.size();
  for (size_t iter = 0; iter < grainSizeDist_Size; ++iter)
  {
    phase = primaryphases[iter];
    PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
    count = 0;
    QVector<float>& curGrainSizeDist = grainsizedist[iter];
    QVector<float>::size_type curGrainSizeDistSize = curGrainSizeDist.size();
    QVector<float>& curSimGrainSizeDist = simgrainsizedist[iter];
    // Initialize all Values to Zero
    for (size_t i = 0; i < curGrainSizeDistSize; i++)
    {
      curSimGrainSizeDist[i] = 0.0f;
    }

    size_t nFieldTuples = m->getNumCellFieldTuples();
    float oneOverCurGrainSizeDistStep = 1.0f/grainsizediststep[iter];
    float halfMinGrainDiameter = pp->getMinGrainDiameter() * 0.5f;
    for (size_t b = firstPrimaryField; b < nFieldTuples; b++)
    {
      index = b;
      if(m_FieldPhases[index] == phase)
      {
        dia = m_EquivalentDiameters[index];
        dia = (dia - halfMinGrainDiameter) * oneOverCurGrainSizeDistStep;
        if(dia < 0) { dia = 0; }
        if(dia > curGrainSizeDistSize - 1.0f) { dia = curGrainSizeDistSize - 1.0f; }
        curSimGrainSizeDist[int(dia)]++;
        count++;
      }
    }

    if(field->m_FieldPhases == phase)
    {
      dia = field->m_EquivalentDiameters;
      dia = (dia - halfMinGrainDiameter) * oneOverCurGrainSizeDistStep;
      if(dia < 0) { dia = 0; }
      if(dia > curGrainSizeDistSize - 1.0f) dia = curGrainSizeDistSize - 1.0f;
      curSimGrainSizeDist[int(dia)]++;
      count++;
    }
    float oneOverCount = 1.0f/count;

    if (count == 0)
    {
      for (size_t i = 0; i < curGrainSizeDistSize; i++) { curSimGrainSizeDist[i] = 0.0; }
    }
    else
    {
      for (size_t i = 0; i < curGrainSizeDistSize; i++)
      {
        curSimGrainSizeDist[i] = curSimGrainSizeDist[i] * oneOverCount;
      }
    }
  }
  compare_2Ddistributions(simgrainsizedist, grainsizedist, bhattdist);
  sizedisterror = bhattdist;
  return sizedisterror;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float PackPrimaryPhases::check_fillingerror(int gadd, int gremove, Int32ArrayType::Pointer grainOwnersPtr, BoolArrayType::Pointer exclusionZonesPtr)
{
  size_t grainOwnersIdx = 0;
  int32_t* grainOwners = grainOwnersPtr->getPointer(0);
  bool* exclusionZones = exclusionZonesPtr->getPointer(0);

  fillingerror = fillingerror * float(m_TotalPackingPoints);
  int col, row, plane;
  int k1 = 0, k2 = 0, k3 = 0;
  if(gadd > 0)
  {
    k1 = 2;
    k2 = -1;
    k3 = 1;
    size_t size = columnlist[gadd].size();
    QVector<int>& cl = columnlist[gadd];
    QVector<int>& rl = rowlist[gadd];
    QVector<int>& pl = planelist[gadd];
    QVector<float>& efl = ellipfunclist[gadd];
    float packquality = 0;
    for (size_t i = 0; i < size; i++)
    {
      col = cl[i];
      row = rl[i];
      plane = pl[i];
      if(m_PeriodicBoundaries == true)
      {
        if(col < 0) col = col + m_PackingPoints[0];
        if(col > m_PackingPoints[0] - 1) col = col - m_PackingPoints[0];
        if(row < 0) row = row + m_PackingPoints[1];
        if(row > m_PackingPoints[1] - 1) row = row - m_PackingPoints[1];
        if(plane < 0) plane = plane + m_PackingPoints[2];
        if(plane > m_PackingPoints[2] - 1) plane = plane - m_PackingPoints[2];
        grainOwnersIdx = (m_PackingPoints[0]*m_PackingPoints[1]*plane) + (m_PackingPoints[0]*row) + col;
        int currentGrainOwner = grainOwners[grainOwnersIdx];
        fillingerror = fillingerror + (k1 * currentGrainOwner  + k2);
        grainOwners[grainOwnersIdx] = currentGrainOwner + k3;
        if(efl[i] > 0.25) exclusionZones[grainOwnersIdx] = true;
        packquality = packquality + ((currentGrainOwner) * (currentGrainOwner));
      }
      else
      {
        if(col >= 0 && col < m_PackingPoints[0] && row >= 0 && row < m_PackingPoints[1] && plane >= 0 && plane < m_PackingPoints[2])
        {
          grainOwnersIdx = (m_PackingPoints[0]*m_PackingPoints[1]*plane) + (m_PackingPoints[0]*row) + col;
          int currentGrainOwner = grainOwners[grainOwnersIdx];
          fillingerror = fillingerror + (k1 * currentGrainOwner + k2);
          grainOwners[grainOwnersIdx] = currentGrainOwner + k3;
          if(efl[i] > 0.25) exclusionZones[grainOwnersIdx] = true;
          packquality = packquality + ((currentGrainOwner) * (currentGrainOwner));
        }
      }
    }
    packqualities[gadd] = static_cast<int>( packquality / float(size) );
  }
  if(gremove > 0)
  {
    k1 = -2;
    k2 = 3;
    k3 = -1;
    size_t size = columnlist[gremove].size();
    QVector<int>& cl = columnlist[gremove];
    QVector<int>& rl = rowlist[gremove];
    QVector<int>& pl = planelist[gremove];
    QVector<float>& efl = ellipfunclist[gremove];
    for (size_t i = 0; i < size; i++)
    {
      col = cl[i];
      row = rl[i];
      plane = pl[i];
      if(m_PeriodicBoundaries == true)
      {
        if(col < 0) col = col + m_PackingPoints[0];
        if(col > m_PackingPoints[0] - 1) col = col - m_PackingPoints[0];
        if(row < 0) row = row + m_PackingPoints[1];
        if(row > m_PackingPoints[1] - 1) row = row - m_PackingPoints[1];
        if(plane < 0) plane = plane + m_PackingPoints[2];
        if(plane > m_PackingPoints[2] - 1) plane = plane - m_PackingPoints[2];
        grainOwnersIdx = (m_PackingPoints[0]*m_PackingPoints[1]*plane) + (m_PackingPoints[0]*row) + col;
        int currentGrainOwner = grainOwners[grainOwnersIdx];
        fillingerror = fillingerror + ( k1 * currentGrainOwner + k2);
        grainOwners[grainOwnersIdx] = currentGrainOwner + k3;
        if(efl[i] > 0.25 && grainOwners[grainOwnersIdx] == 0) exclusionZones[grainOwnersIdx] = false;
      }
      else
      {
        if(col >= 0 && col < m_PackingPoints[0] && row >= 0 && row < m_PackingPoints[1] && plane >= 0 && plane < m_PackingPoints[2])
        {
          grainOwnersIdx = (m_PackingPoints[0]*m_PackingPoints[1]*plane) + (m_PackingPoints[0]*row) + col;
          int currentGrainOwner = grainOwners[grainOwnersIdx];
          fillingerror = fillingerror + (k1 * currentGrainOwner + k2);
          grainOwners[grainOwnersIdx] = currentGrainOwner + k3;
          if(efl[i] > 0.25 && grainOwners[grainOwnersIdx] == 0) exclusionZones[grainOwnersIdx] = false;
        }
      }
    }
  }
  fillingerror = fillingerror / float(m_TotalPackingPoints);
  return fillingerror;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::insert_grain(size_t gnum)
{
  DREAM3D_RANDOMNG_NEW();

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
  for (QMap<unsigned int, ShapeOps*>::iterator ops = m_ShapeOps.begin(); ops != m_ShapeOps.end(); ++ops)
  {
    ops.value()->init();
  }
  // Create our Argument Map
  QMap<ShapeOps::ArgName, float> shapeArgMap;
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
  OrientationMath::EulertoMat(phi1, PHI, phi2, ga);
  xc = m_Centroids[3*gnum];
  yc = m_Centroids[3*gnum+1];
  zc = m_Centroids[3*gnum+2];
  centercolumn = static_cast<int>( (xc - (m_PackingRes[0] * 0.5)) * m_OneOverPackingRes[0] );
  centerrow = static_cast<int>( (yc - (m_PackingRes[1] * 0.5)) * m_OneOverPackingRes[1] );
  centerplane = static_cast<int>( (zc - (m_PackingRes[2] * 0.5)) * m_OneOverPackingRes[2] );
  xmin = int(centercolumn - ((radcur1 * m_OneOverPackingRes[0]) + 1));
  xmax = int(centercolumn + ((radcur1 * m_OneOverPackingRes[0]) + 1));
  ymin = int(centerrow - ((radcur1 * m_OneOverPackingRes[1]) + 1));
  ymax = int(centerrow + ((radcur1 * m_OneOverPackingRes[1]) + 1));
  zmin = int(centerplane - ((radcur1 * m_OneOverPackingRes[2]) + 1));
  zmax = int(centerplane + ((radcur1 * m_OneOverPackingRes[2]) + 1));
  if(xmin < -m_PackingPoints[0]) xmin = -m_PackingPoints[0];
  if(xmax > 2 * m_PackingPoints[0] - 1) xmax = (2 * m_PackingPoints[0] - 1);
  if(ymin < -m_PackingPoints[1]) ymin = -m_PackingPoints[1];
  if(ymax > 2 * m_PackingPoints[1] - 1) ymax = (2 * m_PackingPoints[1] - 1);
  if(zmin < -m_PackingPoints[2]) zmin = -m_PackingPoints[2];
  if(zmax > 2 * m_PackingPoints[2] - 1) zmax = (2 * m_PackingPoints[2] - 1);

  float OneOverRadcur1 = 1.0/radcur1;
  float OneOverRadcur2 = 1.0/radcur2;
  float OneOverRadcur3 = 1.0/radcur3;
  for (int iter1 = xmin; iter1 < xmax + 1; iter1++)
  {
    for (int iter2 = ymin; iter2 < ymax + 1; iter2++)
    {
      for (int iter3 = zmin; iter3 < zmax + 1; iter3++)
      {
        column = iter1;
        row = iter2;
        plane = iter3;
        coords[0] = float(column) * m_PackingRes[0];
        coords[1] = float(row) * m_PackingRes[1];
        coords[2] = float(plane) * m_PackingRes[2];
        inside = -1;
        coords[0] = coords[0] - xc;
        coords[1] = coords[1] - yc;
        coords[2] = coords[2] - zc;
        MatrixMath::Multiply3x3with3x1(ga, coords, coordsRotated);
        float axis1comp = coordsRotated[0] * OneOverRadcur1;
        float axis2comp = coordsRotated[1] * OneOverRadcur2;
        float axis3comp = coordsRotated[2] * OneOverRadcur3;
        inside = m_ShapeOps[shapeclass]->inside(axis1comp, axis2comp, axis3comp);
        if(inside >= 0)
        {
          columnlist[gnum].push_back(column);
          rowlist[gnum].push_back(row);
          planelist[gnum].push_back(plane);
          ellipfunclist[gnum].push_back(inside);
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::assign_voxels()
{
  notifyStatusMessage("Assigning Voxels");

  VolumeDataContainer* m = getVolumeDataContainer();
  int64_t totpoints = m->getTotalPoints();



  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);

  DimType dims[3] = {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  DimType column, row, plane;
  float xc, yc, zc;
  float size[3] = {sizex, sizey, sizez};

  DimType xmin, xmax, ymin, ymax, zmin, zmax;

  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();
  float res[3] = {xRes, yRes, zRes};

  Int32ArrayType::Pointer newownersPtr = Int32ArrayType::CreateArray(totpoints, "newowners");
  int32_t* newowners = newownersPtr->getPointer(0);
  newownersPtr->initializeWithValues(-1);

  FloatArrayType::Pointer ellipfuncsPtr = FloatArrayType::CreateArray(totpoints, "ellipfuncs");
  float* ellipfuncs = ellipfuncsPtr->getPointer(0);
  ellipfuncsPtr->initializeWithValues(-1);

  float grainsPerTime = 0;
  uint64_t millis = QDateTime::currentMSecsSinceEpoch();
  uint64_t currentMillis = millis;

  for (size_t i = firstPrimaryField; i < m->getNumCellFieldTuples(); i++)
  {
    grainsPerTime++;
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if (currentMillis - millis > 1000)
    {
      float rate = grainsPerTime / ( (float)(currentMillis-millis) ) * 1000.0f;

      QString ss = QObject::tr("Assign Voxels & Gaps|| Grains Checked: %1 || Grains/Second: %2").arg(i).arg((int)rate);
      notifyStatusMessage(ss);
      grainsPerTime = 0;
      millis = QDateTime::currentMSecsSinceEpoch();
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
    for (QMap<unsigned int, ShapeOps*>::iterator ops = m_ShapeOps.begin(); ops != m_ShapeOps.end(); ++ops )
    {
      ops.value()->init();
    }
    // Create our Argument Map
    QMap<ShapeOps::ArgName, float> shapeArgMap;
    shapeArgMap[ShapeOps::Omega3] = omega3;
    shapeArgMap[ShapeOps::VolCur] = volcur;
    shapeArgMap[ShapeOps::B_OverA] = bovera;
    shapeArgMap[ShapeOps::C_OverA] = covera;

    radcur1 = m_ShapeOps[shapeclass]->radcur1(shapeArgMap);

    float radcur2 = (radcur1 * bovera);
    float radcur3 = (radcur1 * covera);
    float phi1 = m_AxisEulerAngles[3*i];
    float PHI = m_AxisEulerAngles[3*i+1];
    float phi2 = m_AxisEulerAngles[3*i+2];
    float ga[3][3];
    OrientationMath::EulertoMat(phi1, PHI, phi2, ga);
    column = static_cast<DimType>( (xc - (xRes * 0.5)) / xRes );
    row = static_cast<DimType>( (yc - (yRes * 0.5)) / yRes );
    plane = static_cast<DimType>( (zc - (zRes * 0.5)) / zRes );
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

    float radCur[3] = { radcur1, radcur2, radcur3 };
    float xx[3] = {xc, yc, zc };
    ShapeOps* shapeOps = m_ShapeOps[shapeclass];
    //#if 0
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range3d<int, int, int>(zmin, zmax+1, ymin, ymax+1, xmin, xmax+1),
                        AssignVoxelsGapsImpl(dims, res, m_GrainIds, radCur, xx, shapeOps, ga, size, i, newownersPtr, ellipfuncsPtr), tbb::auto_partitioner());

    }
    else
#endif
    {
      AssignVoxelsGapsImpl serial(dims, res, m_GrainIds, radCur, xx, shapeOps, ga, size, i, newownersPtr, ellipfuncsPtr);
      serial.convert(zmin, zmax+1, ymin, ymax+1, xmin, xmax+1);
    }


  }

  int gnum;
  for (size_t i = firstPrimaryField; i < m->getNumCellFieldTuples(); i++)
  {
    m_Active[i] = false;
  }
  for (size_t i = 0; i < static_cast<size_t>(totpoints); i++)
  {
    if(ellipfuncs[i] >= 0) m_GrainIds[i] = newowners[i];
    gnum = m_GrainIds[i];
    if(gnum >= 0) m_Active[gnum] = true;
    newowners[i] = -1;
    ellipfuncs[i] = -1.0;
  }

  notifyStatusMessage("Assigning Voxels - Removing Included Grains");
  RenumberGrains::Pointer renumber_grains1 = RenumberGrains::New();
  renumber_grains1->setObservers(this->getObservers());
  renumber_grains1->setVolumeDataContainer(m);
  renumber_grains1->execute();
  int err = renumber_grains1->getErrorCondition();
  if (err < 0)
  {
    setErrorCondition(renumber_grains1->getErrorCondition());
    addErrorMessages(renumber_grains1->getPipelineMessages());
    return;
  }

  dataCheck(false, m->getTotalPoints(), m->getNumCellFieldTuples(), m->getNumCellEnsembleTuples());
  if (getCancel() == true)
  {

    QString ss = QObject::tr("Filter Cancelled.");
    notifyWarningMessage(ss, -1);
    setErrorCondition(-1);
    return;
  }
  for (size_t i = firstPrimaryField; i < m->getNumCellFieldTuples(); i++)
  {
    m_Active[i] = false;
  }
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


void PackPrimaryPhases::assign_gaps_only()
{
  notifyStatusMessage("Assigning Gaps");



  VolumeDataContainer* m = getVolumeDataContainer();

  int grainname, grain;
  int current = 0;
  int most = 0;
  int count = 1;
  int counter = 0;
  int neighpoint;
  int good;
  int neighbor;

  int xPoints = static_cast<int>(m->getXPoints());
  int yPoints = static_cast<int>(m->getYPoints());
  int zPoints = static_cast<int>(m->getZPoints());
  int totalPoints = xPoints*yPoints*zPoints;

  int neighpoints[6];
  neighpoints[0] = -xPoints*yPoints;
  neighpoints[1] = -xPoints;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xPoints;
  neighpoints[5] = xPoints*yPoints;

  Int32ArrayType::Pointer neighborsPtr = Int32ArrayType::CreateArray(m->getTotalPoints(), "Neighbors");
  m_Neighbors = neighborsPtr->getPointer(0);
  neighborsPtr->initializeWithValues(-1);

  QVector<int > n(m->getNumCellFieldTuples() + 1,0);
  uint64_t millis = QDateTime::currentMSecsSinceEpoch();
  uint64_t currentMillis = millis;

  while (count != 0)
  {
    counter++;
    count = 0;
    int zStride, yStride;
    for(int i=0;i<zPoints;i++)
    {
      zStride = i*xPoints*yPoints;
      for (int j=0;j<yPoints;j++)
      {
        yStride = j*xPoints;
        for(int k=0;k<xPoints;k++)
        {
          grainname = m_GrainIds[zStride+yStride+k];
          if (grainname < 0)
          {
            count++;
            currentMillis = QDateTime::currentMSecsSinceEpoch();
            if (currentMillis - millis > 1000)
            {
              QString ss = QObject::tr("Assign Gaps|| Cycle#: %1 || Remaining Unassigned Voxel Count: %2").arg(counter).arg(count);
              notifyStatusMessage(ss);
              millis = QDateTime::currentMSecsSinceEpoch();
            }
            current = 0;
            most = 0;
            for (int l = 0; l < 6; l++)
            {
              good = 1;
              neighpoint = zStride+yStride+k + neighpoints[l];
              if (l == 0 && i == 0) good = 0;
              if (l == 5 && i == (zPoints - 1)) good = 0;
              if (l == 1 && j == 0) good = 0;
              if (l == 4 && j == (yPoints - 1)) good = 0;
              if (l == 2 && k == 0) good = 0;
              if (l == 3 && k == (xPoints - 1)) good = 0;
              if (good == 1)
              {
                grain = m_GrainIds[neighpoint];
                if (grain > 0)
                {
                  n[grain]++;
                  current = n[grain];
                  if (current > most)
                  {
                    most = current;
                    m_Neighbors[zStride+yStride+k] = neighpoint;
                  }
                }
              }
            }
            for (int l = 0; l < 6; l++)
            {
              good = 1;
              neighpoint = zStride+yStride+k + neighpoints[l];
              if (l == 0 && i == 0) good = 0;
              if (l == 5 && i == (zPoints - 1)) good = 0;
              if (l == 1 && j == 0) good = 0;
              if (l == 4 && j == (yPoints - 1)) good = 0;
              if (l == 2 && k == 0) good = 0;
              if (l == 3 && k == (xPoints - 1)) good = 0;
              if (good == 1)
              {
                grain = m_GrainIds[neighpoint];
                if(grain > 0) n[grain] = 0;
              }
            }
          }
        }
      }
    }
    for (size_t j = 0; j < totalPoints; j++)
    {
      grainname = m_GrainIds[j];
      neighbor = m_Neighbors[j];
      if (grainname < 0 && neighbor != -1 && m_GrainIds[neighbor] > 0)
      {
        m_GrainIds[j] = m_GrainIds[neighbor];
        m_CellPhases[j] = m_FieldPhases[m_GrainIds[neighbor]];
      }
    }
    if(counter == 1)
    {
       QString ss = QObject::tr("Assign Gaps|| Cycle#: %1 || Remaining Unassigned Voxel Count: %2").arg(counter).arg(count);
       notifyStatusMessage(ss);
    }
  }
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::cleanup_grains()
{
  notifyStatusMessage("Cleaning Up Grains");

  VolumeDataContainer* m = getVolumeDataContainer();

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
  QVector<QVector<int> > vlists;
  vlists.resize(m->getNumCellFieldTuples());
  QVector<int> currentvlist;
  QVector<bool> checked(totpoints,false);
  size_t count;
  int touchessurface = 0;
  int good;
  int neighbor;
  DimType column, row, plane;
  int index;
  float minsize = 0;
  gsizes.resize(m->getNumCellFieldTuples());
  for (size_t i = 1; i < m->getNumCellFieldTuples(); i++)
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
  assign_gaps_only();
  for (int i = 0; i < totpoints; i++)
  {
    if(m_GrainIds[i] > 0) gsizes[m_GrainIds[i]]++;
  }
  for (size_t i = firstPrimaryField; i < m->getNumCellFieldTuples(); i++)
  {
    if(gsizes[i] == 0) m_Active[i] = false;
  }
  for (int i = 0; i < totpoints; i++)
  {
    if(m_GrainIds[i] > 0) { m_CellPhases[i] = m_FieldPhases[m_GrainIds[i]]; }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PackPrimaryPhases::estimate_numgrains(int xpoints, int ypoints, int zpoints, float xres, float yres, float zres)
{
  //  int err = -1;

  float totalvol;
  int phase;

  totalvol = (xpoints * xres) * (ypoints * yres) * (zpoints * zres);
  if (totalvol == 0.0)
  {
    return 1;
  }
  VolumeDataContainer* m = getVolumeDataContainer();

  IDataArray::Pointer iPtr = m->getCellEnsembleData(DREAM3D::EnsembleData::PhaseTypes);
  // Get the PhaseTypes - Remember there is a Dummy PhaseType in the first slot of the array
  DataArray<uint32_t>* phaseType = DataArray<uint32_t>::SafePointerDownCast(iPtr.get());

  iPtr = m->getCellEnsembleData(DREAM3D::EnsembleData::Statistics);
  StatsDataArray* statsDataArrayPtr = StatsDataArray::SafePointerDownCast(iPtr.get());
  if(NULL == statsDataArrayPtr)
  {
    return 1;
  }

  // Create a Reference Variable so we can use the [] syntax
  StatsDataArray& statsDataArray = *statsDataArrayPtr;

  DREAM3D_RANDOMNG_NEW()

      QVector<int> primaryphases;
  QVector<double> primaryphasefractions;
  double totalprimaryfractions = 0.0;
  StatsData::Pointer statsData = StatsData::NullPointer();
  // find which phases are primary phases
  for (size_t i = 1; i < phaseType->getNumberOfTuples(); ++i)
  {
    if(phaseType->GetValue(i) == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      primaryphases.push_back(i);
      primaryphasefractions.push_back(pp->getPhaseFraction());
      totalprimaryfractions = totalprimaryfractions + pp->getPhaseFraction();
    }
  }
  // scale the primary phase fractions to total to 1
  for (size_t i = 0; i < primaryphasefractions.size(); i++)
  {
    primaryphasefractions[i] = primaryphasefractions[i] / totalprimaryfractions;
  }
  // generate the grains
  int gid = 1;

  float currentvol = 0.0;
  float vol;
  float diam;
  int volgood = 0;
  for (size_t j = 0; j < primaryphases.size(); ++j)
  {
    float curphasetotalvol = totalvol * primaryphasefractions[j];
    while (currentvol < (curphasetotalvol))
    {
      volgood = 0;
      phase = primaryphases[j];
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
      while (volgood == 0)
      {
        volgood = 1;
        // u = rg.genrand_res53();
        if(pp->getGrainSize_DistType() == DREAM3D::DistributionType::LogNormal)
        {
          float avgdiam = pp->getGrainSizeDistribution().at(0)->GetValue(0);
          float sddiam = pp->getGrainSizeDistribution().at(1)->GetValue(0);
          diam = rg.genrand_norm(avgdiam, sddiam);
          diam = exp(diam);
          if(diam >= pp->getMaxGrainDiameter()) volgood = 0;
          if(diam < pp->getMinGrainDiameter()) volgood = 0;
          vol = (4.0f / 3.0f) * (M_PI) * ((diam * 0.5f) * (diam * 0.5f) * (diam * 0.5f));
        }
      }
      currentvol = currentvol + vol;
      gid++;
    }
  }
  return gid;
}

void PackPrimaryPhases::write_goal_attributes()
{
  int err = 0;
  setErrorCondition(err);
  VolumeDataContainer* m = getVolumeDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

// Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(m_CsvOutputFile);
  QDir parentPath = fi.path();
  if(!parentPath.mkpath("."))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    notifyErrorMessage(ss, -1);
    setErrorCondition(-1);
    return;
  }

    QFile outFile(getCsvOutputFile());
  if (!outFile.open(QIODevice::WriteOnly))
  {
    QString msg = QObject::tr("CSV Output file could not be opened: %1").arg(getCsvOutputFile());
    setErrorCondition(-200);
    notifyErrorMessage(msg, getErrorCondition());
    return;
  }

  QTextStream dStream(&outFile);

 char space = DREAM3D::GrainData::Delimiter;
  // Write the total number of grains
  dStream << m->getNumCellFieldTuples()-firstPrimaryField;
  // Get all the names of the arrays from the Data Container
  QList<QString> headers = m->getCellFieldArrayNameList();

  QVector<IDataArray::Pointer> data;

  //For checking if an array is a neighborlist
  NeighborList<int>::Pointer neighborlistPtr = NeighborList<int>::New();

  // Print the GrainIds Header before the rest of the headers
  dStream << DREAM3D::GrainData::GrainID;
  // Loop throught the list and print the rest of the headers, ignoring those we don't want
  for(QList<QString>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
  {
    // Only get the array if the name does NOT match those listed
    IDataArray::Pointer p = m->getCellFieldData(*iter);
    if(p->getNameOfClass().compare(neighborlistPtr->getNameOfClass()) != 0)
    {
      if (p->GetNumberOfComponents() == 1) {
        dStream << space << (*iter);
      }
      else // There are more than a single component so we need to add multiple header values
      {
        for(int k = 0; k < p->GetNumberOfComponents(); ++k)
        {
          dStream << space << (*iter) << "_" << k;
        }
      }
      // Get the IDataArray from the DataContainer
      data.push_back(p);
    }
  }
  dStream << "\n";

  // Get the number of tuples in the arrays
  size_t numTuples = data[0]->getNumberOfTuples();

  float threshold = 0.0f;

  // Skip the first grain
  for(size_t i = firstPrimaryField; i < numTuples; ++i)
  {
    if (((float)i / numTuples) * 100.0f > threshold) {

      QString ss = QObject::tr("Writing Field Data - %1% Complete").arg(((float)i / numTuples) * 100);
      notifyStatusMessage(ss);
      threshold = threshold + 5.0f;
      if (threshold < ((float)i / numTuples) * 100.0f) {
        threshold = ((float)i / numTuples) * 100.0f;
      }
    }

    // Print the grain id
    dStream << i;
    // Print a row of data
    for(qint32 p = 0; p < data.size(); ++p)
    {
      dStream << space;
      data[p]->printTuple(dStream, i, space);
    }
    dStream << "\n";
  }
}
