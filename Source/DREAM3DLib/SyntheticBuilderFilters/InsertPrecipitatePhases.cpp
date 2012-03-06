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

#include "InsertPrecipitatePhases.h"

#include <map>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"
#include "DREAM3DLib/Common/DataContainerMacros.h"

#include "DREAM3DLib/ShapeOps/CubeOctohedronOps.h"
#include "DREAM3DLib/ShapeOps/CylinderOps.h"
#include "DREAM3DLib/ShapeOps/EllipsoidOps.h"
#include "DREAM3DLib/ShapeOps/SuperEllipsoidOps.h"

#include "DREAM3DLib/PrivateFilters/FindNeighbors.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InsertPrecipitatePhases::InsertPrecipitatePhases() :
AbstractFilter(),
m_PeriodicBoundaries(false),
m_GrainIds(NULL),
m_PhasesC(NULL),
m_SurfaceVoxels(NULL),
m_AxisEulerAngles(NULL),
m_Centroids(NULL),
m_AxisLengths(NULL),
m_Volumes(NULL),
m_Omega3s(NULL),
m_EquivalentDiameters(NULL),
m_Active(NULL),
m_PhasesF(NULL),
m_NumCells(NULL),
m_PhaseTypes(NULL),
m_ShapeTypes(NULL),
m_NumFields(NULL)
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

  setupFilterOptions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InsertPrecipitatePhases::~InsertPrecipitatePhases()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void InsertPrecipitatePhases::setupFilterOptions()
{
  std::vector<FilterOption::Pointer> options;
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Periodic Boundary");
    option->setPropertyName("PeriodicBoundaries");
    option->setWidgetType(FilterOption::BooleanWidget);
    option->setValueType("bool");
    options.push_back(option);
  }

  setFilterOptions(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  DataContainer* m = getDataContainer();

  // Cell Data
  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1);
  GET_PREREQ_DATA(m, DREAM3D, CellData, SurfaceVoxels, ss, -301, int8_t, Int8ArrayType, voxels, 1);
  CREATE_NON_PREREQ_DATA_SUFFIX(m, DREAM3D, CellData, Phases, C, ss, int32_t, Int32ArrayType,  voxels, 1);

  // Field Data
  GET_PREREQ_DATA_SUFFIX(m, DREAM3D, FieldData, Phases, F, ss, -303,  int32_t, Int32ArrayType, fields, 1);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, ss, -305, float, FloatArrayType, fields, 1);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, Omega3s, ss, -306, float, FloatArrayType, fields, 1);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, AxisEulerAngles, ss, -307, float, FloatArrayType, fields, 3);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, AxisLengths, ss, -308, float, FloatArrayType, fields, 3);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, Volumes, ss, -309, float, FloatArrayType, fields, 1);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, Centroids, ss, -310, float, FloatArrayType, fields, 3);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, -311, bool, BoolArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, NumCells, ss, int32_t, Int32ArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Neighborhoods, ss, int32_t, Int32ArrayType, fields, 1);

  //Ensemble Data
  typedef DataArray<unsigned int> PhaseTypeArrayType;
  typedef DataArray<unsigned int> ShapeTypeArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, PhaseTypes, ss, -301, unsigned int, PhaseTypeArrayType, ensembles, 1);
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, ShapeTypes, ss, -304, unsigned int, ShapeTypeArrayType, ensembles, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, EnsembleData, NumFields, ss, int32_t, Int32ArrayType, ensembles, 1);
  m_StatsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(m->getEnsembleData(DREAM3D::EnsembleData::Statistics).get());
  if(m_StatsDataArray == NULL)
  {
    ss << "Stats Array Not Initialized At Beginning of '" << getNameOfClass() << "' Filter" << std::endl;
    setErrorCondition(-308);
  }

  setErrorMessage(ss.str());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertPrecipitatePhases::execute()
{
  int err = 0;
  setErrorCondition(err);
  DREAM3D_RANDOMNG_NEW()
  DataContainer* m = getDataContainer();

  if(NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }

  int64_t totalPoints = m->getTotalPoints();
  int totalFields = m->getNumFieldTuples();

  dataCheck(false, totalPoints, totalFields, m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  sizex = m->getXPoints() * m->getXRes();
  sizey = m->getYPoints() * m->getYRes();
  sizez = m->getZPoints() * m->getZRes();

  totalvol = sizex * sizey * sizez;

  place_precipitates();
  fillin_precipitates();

  int numfields = m->getNumFieldTuples();
  for(size_t i = firstPrecipitateField; i < numfields; i++)
  {
	m_NumFields[m_PhasesF[i]]++;
  }

  // If there is an error set this to something negative and also set a message
  notify("InsertPrecipitatePhases Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//void InsertPrecipitatePhases::insert_precipitate(size_t gnum, float coatingthickness)
void InsertPrecipitatePhases::insert_precipitate(size_t gnum)
{
  DREAM3D_RANDOMNG_NEW()
  DataContainer* m = getDataContainer();
  size_t udims[3] =
  { 0, 0, 0 };
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif

  DimType dims[3] =
  {   static_cast<DimType>(udims[0]),
      static_cast<DimType>(udims[1]),
      static_cast<DimType>(udims[2]) };

  float dist;
  float inside = -1;
//  float coatinginside = -1;

  DimType index;
  DimType column, row, plane;
  DimType xmin, xmax, ymin, ymax, zmin, zmax;
  float xc, yc, zc;
  float xp, yp, zp;
  float x, y, z;
  float volcur = m_Volumes[gnum];
  float bovera = m_AxisLengths[3 * gnum + 1];
  float covera = m_AxisLengths[3 * gnum + 2];
  float omega3 = m_Omega3s[gnum];
  float radcur1, radcur2, radcur3;
//  float coatingradcur1, coatingradcur2, coatingradcur3;
  currentprecipvoxellist.resize(0);
//  currentcoatingvoxellist.resize(0);


  unsigned int shapeclass = m_ShapeTypes[m_PhasesF[gnum]];
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

  radcur2 = (radcur1 * bovera);
  radcur3 = (radcur1 * covera);
//  coatingradcur1 = radcur1 + coatingthickness;
//  coatingradcur2 = radcur2 + coatingthickness;
//  coatingradcur3 = radcur3 + coatingthickness;
  float phi1 = m_AxisEulerAngles[3 * gnum];
  float PHI = m_AxisEulerAngles[3 * gnum + 1];
  float phi2 = m_AxisEulerAngles[3 * gnum + 2];
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
  xc = m_Centroids[3 * gnum];
  yc = m_Centroids[3 * gnum + 1];
  zc = m_Centroids[3 * gnum + 2];
  column = (xc - (m->getXRes() / 2)) / m->getXRes();
  row = (yc - (m->getYRes() / 2)) / m->getYRes();
  plane = (zc - (m->getZRes() / 2)) / m->getZRes();
  xmin = int(column - ((radcur1 / m->getXRes()) + 1));
  xmax = int(column + ((radcur1 / m->getXRes()) + 1));
  ymin = int(row - ((radcur1 / m->getYRes()) + 1));
  ymax = int(row + ((radcur1 / m->getYRes()) + 1));
  zmin = int(plane - ((radcur1 / m->getZRes()) + 1));
  zmax = int(plane + ((radcur1 / m->getZRes()) + 1));
  if(m_PeriodicBoundaries == true)
  {
    if(xmin < -dims[0]) xmin = -dims[0];
    if(xmax > 2 * dims[0] - 1) xmax = (2 * dims[0] - 1);
    if(ymin < -dims[1]) ymin = -dims[1];
    if(ymax > 2 * dims[1] - 1) ymax = (2 * dims[1] - 1);
    if(zmin < -dims[2]) zmin = -dims[2];
    if(zmax > 2 * dims[2] - 1) zmax = (2 * dims[2] - 1);
  }
  if(m_PeriodicBoundaries == false)
  {
    if(xmin < 0) xmin = 0;
    if(xmax > dims[0] - 1) xmax = dims[0] - 1;
    if(ymin < 0) ymin = 0;
    if(ymax > dims[1] - 1) ymax = dims[1] - 1;
    if(zmin < 0) zmin = 0;
    if(zmax > dims[2] - 1) zmax = dims[2] - 1;
  }
  for (DimType iter1 = xmin; iter1 < xmax + 1; iter1++)
  {
    for (DimType iter2 = ymin; iter2 < ymax + 1; iter2++)
    {
      for (DimType iter3 = zmin; iter3 < zmax + 1; iter3++)
      {
        column = iter1;
        row = iter2;
        plane = iter3;
        if(iter1 < 0) column = iter1 + dims[0];
        if(iter1 > dims[0] - 1) column = iter1 - dims[0];
        if(iter2 < 0) row = iter2 + dims[1];
        if(iter2 > dims[1] - 1) row = iter2 - dims[1];
        if(iter3 < 0) plane = iter3 + dims[2];
        if(iter3 > dims[2] - 1) plane = iter3 - dims[2];
        index = (plane * dims[0] * dims[1]) + (row * dims[0]) + column;
        inside = -1;
//        coatinginside = -1;
        x = float(column) * m->getXRes();
        y = float(row) * m->getYRes();
        z = float(plane) * m->getZRes();
        if(iter1 < 0) x = x - sizex;
        if(iter1 > dims[0] - 1) x = x + sizex;
        if(iter2 < 0) y = y - sizey;
        if(iter2 > dims[1] - 1) y = y + sizey;
        if(iter3 < 0) z = z - sizez;
        if(iter3 > dims[2] - 1) z = z + sizez;
        dist = ((x - xc) * (x - xc)) + ((y - yc) * (y - yc)) + ((z - zc) * (z - zc));
        dist = sqrt(dist);
//        if(dist < coatingradcur1)
        if(dist < radcur1)
        {
          x = x - xc;
          y = y - yc;
          z = z - zc;
          xp = (x * ga[0][0]) + (y * ga[0][1]) + (z * ga[0][2]);
          yp = (x * ga[1][0]) + (y * ga[1][1]) + (z * ga[1][2]);
          zp = (x * ga[2][0]) + (y * ga[2][1]) + (z * ga[2][2]);
          float axis1comp = xp / radcur1;
          float axis2comp = yp / radcur2;
          float axis3comp = zp / radcur3;
//          float coatingaxis1comp = xp / coatingradcur1;
//          float coatingaxis2comp = yp / coatingradcur2;
//          float coatingaxis3comp = zp / coatingradcur3;
          inside = m_ShapeOps[shapeclass]->inside(axis1comp, axis2comp, axis3comp);
//          coatinginside = m_ShapeOps[shapeclass]->inside(coatingaxis1comp, coatingaxis2comp, coatingaxis3comp);
          if(inside >= 0)
          {
            int currentpoint = index;
            currentprecipvoxellist.push_back(currentpoint);
          }
//          if(inside < 0 && coatinginside >= 0)
//          {
//            int currentpoint = index;
//            currentcoatingvoxellist.push_back(currentpoint);
//          }
        }
      }
    }
  }
}




void  InsertPrecipitatePhases::fillin_precipitates()
{
  notify("Filling In Precipitates", 0, Observable::UpdateProgressMessage);
  DataContainer* m = getDataContainer();
  int64_t totalPoints = m->getTotalPoints();

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

  std::vector<int> neighs;
  std::vector<int> remove;
  std::vector<int> gsizes;
  std::vector<int> neighbors;
  int count = 1;
  int previouscount = 0;
  bool flag = false;
  int good = 1;
  float x, y, z;
  gsizes.resize(m->getNumFieldTuples(), 0);
  neighbors.resize(m->getTotalPoints(), 0);
  DimType neighpoint;
  DimType neighpoints[6];
  std::vector<int> n(m->getNumFieldTuples());
  neighpoints[0] = -dims[0] * dims[1];
  neighpoints[1] = -dims[0];
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = dims[0];
  neighpoints[5] = dims[0] * dims[1];
  while (count != 0)
  {
    if(previouscount == count && flag == false) { flag = true;}
    previouscount = count;
    count = 0;
    for (int64_t i = 0; i < totalPoints; i++)
    {
      int grainname = m_GrainIds[i];
      if(grainname <= 0)
      {
        count++;
        for (size_t c = 1; c < m->getNumFieldTuples(); c++)
        {
          n[c] = 0;
        }
        x = static_cast<float>(i % dims[0]);
        y = static_cast<float>((i / dims[0]) % dims[1]);
        z = static_cast<float>(i / (dims[0] * dims[1]));
        for (int j = 0; j < 6; j++)
        {
          good = 1;
          neighpoint = i + neighpoints[j];
          if(j == 0 && z == 0) good = 0;
          if(j == 5 && z == (dims[2] - 1)) good = 0;
          if(j == 1 && y == 0) good = 0;
          if(j == 4 && y == (dims[1] - 1)) good = 0;
          if(j == 2 && x == 0) good = 0;
          if(j == 3 && x == (dims[0] - 1)) good = 0;
          if(good == 1)
          {
            int grain = m_GrainIds[neighpoint];
            if(grain > 0 && (grain >= firstPrecipitateField || flag == true))
            {
              neighs.push_back(grain);
            }
          }
        }
        int current = 0;
        int most = 0;
        int curgrain = 0;
        int size = int(neighs.size());
        for (int k = 0; k < size; k++)
        {
          int neighbor = neighs[k];
          n[neighbor]++;
          current = n[neighbor];
          if(current > most)
          {
            most = current;
            curgrain = neighbor;
          }
        }
        if(size > 0)
        {
          neighbors[i] = curgrain;
          neighs.clear();
        }
      }
    }
    for (int64_t j = 0; j < totalPoints; j++)
    {
      int grainname = m_GrainIds[j];
      int neighbor = neighbors[j];
      if(grainname <= 0 && neighbor > 0 && (neighbor >= firstPrecipitateField || flag == true))
      {
        m_GrainIds[j] = neighbor;
        m_PhasesC[j] = m_PhasesF[neighbor];
      }
    }
  }
  gsizes.resize(m->getNumFieldTuples(), 0);

  for (int64_t i = 0; i < totalPoints; i++)
  {
    int name = m_GrainIds[i];
    gsizes[name]++;
  }
  for (size_t i = 1; i < m->getNumFieldTuples(); i++)
  {
    m_NumCells[i] = gsizes[i];
  }
  gsizes.clear();
}

void  InsertPrecipitatePhases::place_precipitates()
{
  notify("Placing Precipitates", 0, Observable::UpdateProgressMessage);
  DREAM3D_RANDOMNG_NEW()

  DataContainer* m = getDataContainer();

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

  // float xRes = m->getXRes();
  // float yRes = m->getYRes();
  // float zRes = m->getZRes();
  sizex = dims[0] * m->getXRes();
  sizey = dims[1] * m->getYRes();
  sizez = dims[2] * m->getZRes();
  totalvol = sizex*sizey*sizez;

  int64_t totalPoints = m->getTotalPoints();
  size_t precipvoxelcounter = 0;
//  float thickness = 0.25;
  size_t currentnumgrains = m->getNumFieldTuples();
  firstPrecipitateField = currentnumgrains;
  // size_t index;
  int phase;
  float precipboundaryfraction = 0.0;
  float random;
  int random2;
  float xc, yc, zc;
  double totalprecipitatefractions = 0.0;

  size_t numensembles = m->getNumEnsembleTuples();

  for (size_t i = 1; i < numensembles; ++i)
  {
    if(m_PhaseTypes[i] == DREAM3D::PhaseType::PrecipitatePhase)
    {
	  m_NumFields[i] = 0;
      precipitatephases.push_back(i);
      precipitatephasefractions.push_back(statsDataArray[i]->getPhaseFraction());
      totalprecipitatefractions = totalprecipitatefractions + statsDataArray[i]->getPhaseFraction();
    }
  }
  for (size_t i = 0; i < precipitatephases.size(); i++)
  {
    precipitatephasefractions[i] = precipitatephasefractions[i] / totalprecipitatefractions;
    if(i > 0) precipitatephasefractions[i] = precipitatephasefractions[i] + precipitatephasefractions[i - 1];
  }

  Field field;
  std::vector<float> curphasevol;
  curphasevol.resize(precipitatephases.size());
  float factor = 1.0;
  float iter = 0;
  for (size_t j = 0; j < precipitatephases.size(); ++j)
  {
	  curphasevol[j] = 0;
	  float curphasetotalvol = (totalvol*totalprecipitatefractions)*precipitatephasefractions[j];
	  while (curphasevol[j] < (factor * curphasetotalvol))
	  {
	    iter++;
	    Seed++;
	    phase = precipitatephases[j];
		PackPrimaryPhases::generate_grain(phase, Seed, &field, m_StatsDataArray, m_ShapeTypes[phase], m_OrthoOps);
		m->resizeFieldDataArrays(currentnumgrains + 1);
		dataCheck(false, totalPoints, m->getNumFieldTuples(), m->getNumEnsembleTuples());

		transfer_attributes(currentnumgrains, &field);
		precipboundaryfraction = statsDataArray[phase]->getPrecipBoundaryFraction();
		random = rg.genrand_res53();
		if(random <= precipboundaryfraction)
		{
	      random2 = int(rg.genrand_res53() * double(totalPoints - 1));
	      while (m_SurfaceVoxels[random2] == 0 || m_GrainIds[random2] > firstPrecipitateField)
	      {
	        random2++;
	        if(random2 >= totalPoints) random2 = random2 - totalPoints;
	      }
	    }
	    else if(random > precipboundaryfraction)
	    {
	      random2 = rg.genrand_res53() * (totalPoints - 1);
	      while (m_SurfaceVoxels[random2] != 0 || m_GrainIds[random2] > firstPrecipitateField)
	      {
	        random2++;
	        if(random2 >= totalPoints) random2 = random2 - totalPoints;
	      }
	    }
	    xc = find_xcoord(random2);
	    yc = find_ycoord(random2);
	    zc = find_zcoord(random2);
	    m_Centroids[3 * currentnumgrains] = xc;
	    m_Centroids[3 * currentnumgrains + 1] = yc;
	    m_Centroids[3 * currentnumgrains + 2] = zc;
//    insert_precipitate(currentnumgrains, thickness);
	    insert_precipitate(currentnumgrains);

	    m_Active[currentnumgrains] = true;
	    precipvoxelcounter = 0;
	    for (size_t jj = 0; jj < currentprecipvoxellist.size(); jj++)
	    {
	      if(m_GrainIds[currentprecipvoxellist[jj]] > 0 && m_GrainIds[currentprecipvoxellist[jj]] < firstPrecipitateField)
	      {
	        precipvoxelcounter++;
	      }
	    }
	    if(precipvoxelcounter == currentprecipvoxellist.size())
	    {
	      precipvoxelcounter = 0;
	      for (size_t jj = 0; jj < currentprecipvoxellist.size(); jj++)
	      {
	        m_GrainIds[currentprecipvoxellist[jj]] = currentnumgrains;
	        m_PhasesC[currentprecipvoxellist[jj]] = m_PhasesF[currentnumgrains];
	        precipvoxelcounter++;
	      }
//	      for (size_t j = 0; j < currentcoatingvoxellist.size(); j++)
//	      {
//	        if(m_GrainIds[currentcoatingvoxellist[j]] < numprimarygrains)
//	        {
//	          m_PhasesC[currentcoatingvoxellist[j]] = 3;
//	        }
//	      }
	      curphasevol[j] = curphasevol[j] + (precipvoxelcounter * m->getXRes() * m->getYRes() * m->getZRes());
	      currentnumgrains++;
	    }
	  }
  }
}

void InsertPrecipitatePhases::transfer_attributes(int gnum, Field* field)
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
  m_PhasesF[gnum] = field->m_PhasesF;
  m_Neighborhoods[gnum] = field->m_Neighborhoods;
}

float InsertPrecipitatePhases::find_xcoord(long long int index)
{
  DataContainer* m = getDataContainer();
  float x = m->getXRes()*float(index%m->getXPoints());
  return x;
}
float InsertPrecipitatePhases::find_ycoord(long long int index)
{
  DataContainer* m = getDataContainer();
  float y = m->getYRes()*float((index/m->getXPoints())%m->getYPoints());
  return y;
}
float InsertPrecipitatePhases::find_zcoord(long long int index)
{
  DataContainer* m = getDataContainer();
  float z = m->getZRes()*float(index/(m->getXPoints()*m->getYPoints()));
  return z;
}
