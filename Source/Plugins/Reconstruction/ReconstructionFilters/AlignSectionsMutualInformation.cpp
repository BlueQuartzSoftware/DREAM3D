/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "AlignSectionsMutualInformation.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/Utilities/SIMPLibRandom.h"

#include "Reconstruction/ReconstructionConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsMutualInformation::AlignSectionsMutualInformation() :
  AlignSections(),
  m_MisorientationTolerance(5.0f),
  m_UseGoodVoxels(true),
  m_QuatsArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Quats),
  m_CellPhasesArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Phases),
  m_GoodVoxelsArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Mask),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_Quats(NULL),
  m_CellPhases(NULL),
  m_GoodVoxels(NULL),
  m_CrystalStructures(NULL)
{
  Seed = QDateTime::currentMSecsSinceEpoch();

  m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();

  featurecounts = NULL;
  m_FeatureIds = NULL;

  // only setting up the child parameters because the parent constructor has already been called
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsMutualInformation::~AlignSectionsMutualInformation()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMutualInformation::setupFilterParameters()
{
  // getting the current parameters that were set by the parent and adding to it before resetting it
  FilterParameterVector parameters = getFilterParameters();
  parameters.push_front(DoubleFilterParameter::New("Misorientation Tolerance", "MisorientationTolerance", getMisorientationTolerance(), FilterParameter::Parameter));
  QStringList linkedProps("GoodVoxelsArrayPath");
  parameters.push_back(LinkedBooleanFilterParameter::New("Use Mask Array", "UseGoodVoxels", getUseGoodVoxels(), linkedProps, FilterParameter::Parameter));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Float, 4, DREAM3D::AttributeMatrixType::Cell, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Quaternions", "QuatsArrayPath", getQuatsArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixType::Cell, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Phases", "CellPhasesArrayPath", getCellPhasesArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Bool, 1, DREAM3D::AttributeMatrixType::Cell, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Mask", "GoodVoxelsArrayPath", getGoodVoxelsArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::UInt32, 1, DREAM3D::AttributeMatrixType::CellEnsemble, DREAM3D::GeometryType::ImageGeometry);

    parameters.push_back(DataArraySelectionFilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", getCrystalStructuresArrayPath(), FilterParameter::RequiredArray, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMutualInformation::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  AlignSections::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setUseGoodVoxels(reader->readValue("UseGoodVoxels", getUseGoodVoxels() ) );
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() ) );
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath() ) );
  setQuatsArrayPath(reader->readDataArrayPath("QuatsArrayPath", getQuatsArrayPath() ) );
  setMisorientationTolerance( reader->readValue("MisorientationTolerance", getMisorientationTolerance()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AlignSectionsMutualInformation::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  AlignSections::writeFilterParameters(writer, index);
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(GoodVoxelsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(UseGoodVoxels)
  SIMPL_FILTER_WRITE_PARAMETER(CellPhasesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(QuatsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(MisorientationTolerance)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMutualInformation::dataCheck()
{
  setErrorCondition(0);

  // Set the DataContainerName and AttributematrixName for the Parent Class (AlignSections) to Use.
  setDataContainerName(m_QuatsArrayPath.getDataContainerName());
  setCellAttributeMatrixName(m_QuatsArrayPath.getAttributeMatrixName());

  AlignSections::dataCheck();
  if(getErrorCondition() < 0) { return; }

  INIT_DataArray(m_FeatureCounts, int32_t);

  QVector<DataArrayPath> dataArrayPaths;

  QVector<size_t> cDims(1, 4);
  m_QuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuatsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getQuatsArrayPath()); }

  cDims[0] = 1;
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray< int32_t>, AbstractFilter>(this,  getCellPhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getCellPhasesArrayPath()); }
  if(m_UseGoodVoxels == true)
  {
    m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getGoodVoxelsArrayPath()); }
  }

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this,  getCrystalStructuresArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMutualInformation::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMutualInformation::find_shifts(std::vector<int64_t>& xshifts, std::vector<int64_t>& yshifts)
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
  Int32ArrayType::Pointer p = Int32ArrayType::CreateArray((totalPoints * 1), "_INTERNAL_USE_ONLY_MIFeatureIds");
  m_FeatureIds = p->getPointer(0);

  std::ofstream outFile;
  if (getWriteAlignmentShifts() == true)
  {
    outFile.open(getAlignmentShiftFileName().toLatin1().data());
  }

  size_t udims[3] = { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  float disorientation = 0.0f;
  float mindisorientation = std::numeric_limits<float>::max();
  float** mutualinfo12 = NULL;
  float* mutualinfo1 = NULL;
  float* mutualinfo2 = NULL;
  int32_t featurecount1 = 0, featurecount2 = 0;
  int64_t newxshift = 0;
  int64_t newyshift = 0;
  int64_t oldxshift = 0;
  int64_t oldyshift = 0;
  float count = 0.0f;
  DimType slice = 0;

  int32_t refgnum = 0, curgnum = 0;
  DimType refposition = 0;
  DimType curposition = 0;

  form_features_sections();

  std::vector<std::vector<float> >  misorients;
  misorients.resize(dims[0]);
  for (DimType a = 0; a < dims[0]; a++)
  {
    misorients[a].assign(dims[1], 0.0f);
  }

  for (DimType iter = 1; iter < dims[2]; iter++)
  {
    QString ss = QObject::tr("Aligning Sections || Determining Shifts || %1% Complete").arg(((float)iter / dims[2]) * 100);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    mindisorientation = std::numeric_limits<float>::max();
    slice = (dims[2] - 1) - iter;
    featurecount1 = featurecounts[slice];
    featurecount2 = featurecounts[slice + 1];
    mutualinfo12 = new float *[featurecount1];
    mutualinfo1 = new float[featurecount1];
    mutualinfo2 = new float[featurecount2];

    for (int32_t a = 0; a < featurecount1; a++)
    {
      mutualinfo1[a] = 0.0f;
      mutualinfo12[a] = new float[featurecount2];
      for (int32_t b = 0; b < featurecount2; b++)
      {
        mutualinfo12[a][b] = 0.0f;
        mutualinfo2[b] = 0.0f;
      }
    }
    oldxshift = -1;
    oldyshift = -1;
    newxshift = 0;
    newyshift = 0;
    for (DimType a = 0; a < dims[0]; a++)
    {
      for (DimType b = 0; b < dims[1]; b++)
      {
        misorients[a][b] = 0;
      }
    }
    while (newxshift != oldxshift || newyshift != oldyshift)
    {
      oldxshift = newxshift;
      oldyshift = newyshift;
      for (int32_t j = -3; j < 4; j++)
      {
        for (int32_t k = -3; k < 4; k++)
        {
          disorientation = 0;
          count = 0;
          if (misorients[k + oldxshift + dims[0] / 2][j + oldyshift + dims[1] / 2] == 0 && llabs(k + oldxshift) < (dims[0] / 2)
              && (j + oldyshift) < (dims[1] / 2))
          {
            for (DimType l = 0; l < dims[1]; l = l + 4)
            {
              for (DimType n = 0; n < dims[0]; n = n + 4)
              {
                if ((l + j + oldyshift) >= 0 && (l + j + oldyshift) < dims[1] && (n + k + oldxshift) >= 0 && (n + k + oldxshift) < dims[0])
                {
                  refposition = ((slice + 1) * dims[0] * dims[1]) + (l * dims[0]) + n;
                  curposition = (slice * dims[0] * dims[1]) + ((l + j + oldyshift) * dims[0]) + (n + k + oldxshift);
                  refgnum = m_FeatureIds[refposition];
                  curgnum = m_FeatureIds[curposition];
                  if (curgnum >= 0 && refgnum >= 0)
                  {
                    mutualinfo12[curgnum][refgnum]++;
                    mutualinfo1[curgnum]++;
                    mutualinfo2[refgnum]++;
                    count++;
                  }
                }
                else
                {
                  mutualinfo12[0][0]++;
                  mutualinfo1[0]++;
                  mutualinfo2[0]++;
                }
              }
            }
            float ha = 0.0f;
            float hb = 0.0f;
            float hab = 0.0f;
            for (int32_t b = 0; b < featurecount1; b++)
            {
              mutualinfo1[b] = mutualinfo1[b] / count;
              if (mutualinfo1[b] != 0) { ha = ha + mutualinfo1[b] * logf(mutualinfo1[b]); }
            }
            for (int32_t c = 0; c < featurecount2; c++)
            {
              mutualinfo2[c] = mutualinfo2[c] / float(count);
              if (mutualinfo2[c] != 0) { hb = hb + mutualinfo2[c] * logf(mutualinfo2[c]); }
            }
            for (int32_t b = 0; b < featurecount1; b++)
            {
              for (int32_t c = 0; c < featurecount2; c++)
              {
                mutualinfo12[b][c] = mutualinfo12[b][c] / count;
                if (mutualinfo12[b][c] != 0) { hab = hab + mutualinfo12[b][c] * logf(mutualinfo12[b][c]); }
                float value = 0.0f;
                if (mutualinfo1[b] > 0 && mutualinfo2[c] > 0) { value = (mutualinfo12[b][c] / (mutualinfo1[b] * mutualinfo2[c])); }
                if (value != 0) { disorientation = disorientation + (mutualinfo12[b][c] * logf(value)); }
              }
            }
            for (int32_t b = 0; b < featurecount1; b++)
            {
              for (int32_t c = 0; c < featurecount2; c++)
              {
                mutualinfo12[b][c] = 0.0f;
                mutualinfo1[b] = 0.0f;
                mutualinfo2[c] = 0.0f;
              }
            }
            disorientation = 1.0f / disorientation;
            misorients[k + oldxshift + dims[0] / 2][j + oldyshift + dims[1] / 2] = disorientation;
            if (disorientation < mindisorientation)
            {
              newxshift = k + oldxshift;
              newyshift = j + oldyshift;
              mindisorientation = disorientation;
            }
          }
        }
      }
    }
    xshifts[iter] = xshifts[iter - 1] + newxshift;
    yshifts[iter] = yshifts[iter - 1] + newyshift;
    if (getWriteAlignmentShifts() == true)
    {
      outFile << slice << "	" << slice + 1 << "	" << newxshift << "	" << newyshift << "	" << xshifts[iter] << "	" << yshifts[iter] << "\n";
    }
    delete[] mutualinfo1;
    delete[] mutualinfo2;
    for (int32_t i = 0; i < featurecount1; i++)
    {
      delete mutualinfo12[i];
    }
    delete[] mutualinfo12;
    mutualinfo1 = NULL;
    mutualinfo2 = NULL;
    mutualinfo12 = NULL;
  }

  m->getAttributeMatrix(getCellAttributeMatrixName())->removeAttributeArray(DREAM3D::CellData::FeatureIds);

  if (getWriteAlignmentShifts() == true)
  {
    outFile.close();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMutualInformation::form_features_sections()
{
  SIMPL_RANDOMNG_NEW()
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  size_t udims[3] = { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  DimType point = 0;
  DimType seed = 0;
  bool noseeds = false;
  int32_t featurecount = 1;
  DimType neighbor = 0;
  QuatF q1 = QuaternionMathF::New();
  QuatF q2 = QuaternionMathF::New();
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);
  float w = 0.0f;
  float n1 = 0.0f;
  float n2 = 0.0f;
  float n3 = 0.0f;
  DimType randx = 0;
  DimType randy = 0;
  bool good = false;
  DimType x = 0, y = 0, z = 0;
  DimType col = 0, row = 0;
  size_t size = 0;
  size_t initialVoxelsListSize = 1000;

  m_FeatureCounts->resize(dims[2]);
  featurecounts = m_FeatureCounts->getPointer(0);

  std::vector<DimType> voxelslist(initialVoxelsListSize, -1);
  DimType neighpoints[4] = { 0, 0, 0, 0 };
  neighpoints[0] = -dims[0];
  neighpoints[1] = -1;
  neighpoints[2] = 1;
  neighpoints[3] = dims[0];

  uint32_t phase1 = 0, phase2 = 0;

  for (DimType slice = 0; slice < dims[2]; slice++)
  {
    QString ss = QObject::tr("Aligning Sections || Identifying Features on Sections || %1% Complete").arg(((float)slice / dims[2]) * 100);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    featurecount = 1;
    noseeds = false;
    while (noseeds == false)
    {
      seed = -1;
      randx = DimType(float(rg.genrand_res53()) * float(dims[0]));
      randy = DimType(float(rg.genrand_res53()) * float(dims[1]));
      for (DimType j = 0; j < dims[1]; ++j)
      {
        for (DimType i = 0; i < dims[0]; ++i)
        {
          x = randx + i;
          y = randy + j;
          z = slice;
          if (x > dims[0] - 1) { x = x - dims[0]; }
          if (y > dims[1] - 1) { y = y - dims[1]; }
          point = (z * dims[0] * dims[1]) + (y * dims[0]) + x;
          if ((m_UseGoodVoxels == false || m_GoodVoxels[point] == true) && m_FeatureIds[point] == 0 && m_CellPhases[point] > 0)
          {
            seed = point;
          }
          if (seed > -1) { break; }
        }
        if( seed > -1) { break; }
      }
      if (seed == -1) { noseeds = true; }
      if (seed >= 0)
      {
        size = 0;
        m_FeatureIds[seed] = featurecount;
        voxelslist[size] = seed;
        size++;
        for (size_t j = 0; j < size; ++j)
        {
          DimType currentpoint = voxelslist[j];
          col = currentpoint % dims[0];
          row = (currentpoint / dims[0]) % dims[1];
          QuaternionMathF::Copy(quats[currentpoint], q1);
          phase1 = m_CrystalStructures[m_CellPhases[currentpoint]];
          for (int32_t i = 0; i < 4; i++)
          {
            good = true;
            neighbor = currentpoint + neighpoints[i];
            if ((i == 0) && row == 0) { good = false; }
            if ((i == 3) && row == (dims[1] - 1)) { good = false; }
            if ((i == 1) && col == 0) { good = false; }
            if ((i == 2) && col == (dims[0] - 1)) { good = false; }
            if (good == true && m_FeatureIds[neighbor] <= 0 && m_CellPhases[neighbor] > 0)
            {
              w = std::numeric_limits<float>::max();
              QuaternionMathF::Copy(quats[neighbor], q2);
              phase2 = m_CrystalStructures[m_CellPhases[neighbor]];
              if (phase1 == phase2)
              {
                w = m_OrientationOps[phase1]->getMisoQuat(q1, q2, n1, n2, n3);
              }
              if (w < m_MisorientationTolerance)
              {
                m_FeatureIds[neighbor] = featurecount;
                voxelslist[size] = neighbor;
                size++;
                if (std::vector<DimType>::size_type(size) >= voxelslist.size())
                {
                  size = voxelslist.size();
                  voxelslist.resize(size + initialVoxelsListSize);
                  for (std::vector<DimType>::size_type v = size; v < voxelslist.size(); ++v) { voxelslist[v] = -1; }
                }
              }
            }
          }
        }
        voxelslist.erase(std::remove(voxelslist.begin(), voxelslist.end(), -1), voxelslist.end());
        featurecount++;
        voxelslist.assign(initialVoxelsListSize, -1);
      }
    }
    featurecounts[slice] = featurecount;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMutualInformation::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  // Converting the user defined tolerance to radians.
  m_MisorientationTolerance = m_MisorientationTolerance * SIMPLib::Constants::k_Pi / 180.0f;

  AlignSections::execute();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AlignSectionsMutualInformation::newFilterInstance(bool copyFilterParameters)
{
  AlignSectionsMutualInformation::Pointer filter = AlignSectionsMutualInformation::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsMutualInformation::getCompiledLibraryName()
{ return ReconstructionConstants::ReconstructionBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsMutualInformation::getGroupName()
{ return DREAM3D::FilterGroups::ReconstructionFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsMutualInformation::getSubGroupName()
{return DREAM3D::FilterSubGroups::AlignmentFilters;}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AlignSectionsMutualInformation::getHumanLabel()
{ return "Align Sections (Mutual Information)"; }
