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

#include "AlignSectionsMutualInformation.h"

#include <QtCore/QtDebug>
#include <fstream>
#include <sstream>

#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1


#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsMutualInformation::AlignSectionsMutualInformation() :
  AlignSections(),
  m_MisorientationTolerance(5.0f),
  m_UseGoodVoxels(true),
  m_QuatsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Quats),
  m_CellPhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Phases),
  m_GoodVoxelsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::GoodVoxels),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_Quats(NULL),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellPhases(NULL),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_GoodVoxels(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL)
{
  Seed = QDateTime::currentMSecsSinceEpoch();

  m_OrientationOps = OrientationOps::getOrientationOpsVector();

  featurecounts = NULL;
  INIT_DataArray(m_FeatureCounts, int);
  //only setting up the child parameters because the parent constructor has already been called
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
  //getting the current parameters that were set by the parent and adding to it before resetting it
  FilterParameterVector parameters = getFilterParameters();
  parameters.push_front(FilterParameter::New("Misorientation Tolerance", "MisorientationTolerance", FilterParameterWidgetType::DoubleWidget, getMisorientationTolerance(), false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("Quats", "QuatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget,  getQuatsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Phases", "CellPhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget,  getCellPhasesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCrystalStructuresArrayPath(), true, ""));
  QStringList linkedProps("GoodVoxelsArrayPath");
  parameters.push_back(LinkedBooleanFilterParameter::New("Use GoodVoxels Array", "UseGoodVoxels", getUseGoodVoxels(), linkedProps, false));
  parameters.push_back(FilterParameter::New("GoodVoxels", "GoodVoxelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getGoodVoxelsArrayPath(), true, ""));

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
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(GoodVoxelsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(UseGoodVoxels)
  DREAM3D_FILTER_WRITE_PARAMETER(CellPhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(QuatsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MisorientationTolerance)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMutualInformation::dataCheck()
{
  setErrorCondition(0);

  //Set the DataContainerName and AttributematrixName for the Parent Class (AlignSections) to Use.
  setDataContainerName(m_QuatsArrayPath.getDataContainerName());
  setCellAttributeMatrixName(m_QuatsArrayPath.getAttributeMatrixName());

  if(true == getWriteAlignmentShifts() && getAlignmentShiftFileName().isEmpty() == true)
  {
    QString ss = QObject::tr("The Alignment Shift file name must be set before executing this filter.");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QVector<size_t> dims(1, 4);
  m_QuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuatsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 1;
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray< int32_t>, AbstractFilter>(this,  getCellPhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(m_UseGoodVoxels == true)
  {
    m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

 //typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this,  getCrystalStructuresArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
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
void AlignSectionsMutualInformation::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  //Converting the user defined tolerance to radians.
  m_MisorientationTolerance = m_MisorientationTolerance * DREAM3D::Constants::k_Pi / 180.0f;

  AlignSections::execute();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Aligning Sections Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMutualInformation::find_shifts(std::vector<int> &xshifts, std::vector<int> &yshifts)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
  Int32ArrayType::Pointer p = Int32ArrayType::CreateArray((totalPoints * 1), "MI FeatureIds");
  m_FeatureIds = p->getPointer(0);

  std::ofstream outFile;
  if (getWriteAlignmentShifts() == true)
  {
    outFile.open(getAlignmentShiftFileName().toLatin1().data());
  }

  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
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

  float disorientation = 0;
  float mindisorientation = 100000000;
  float** mutualinfo12 = NULL;
  float* mutualinfo1 = NULL;
  float* mutualinfo2 = NULL;
  int featurecount1, featurecount2;
  int newxshift = 0;
  int newyshift = 0;
  int oldxshift = 0;
  int oldyshift = 0;
  float count = 0;
  int slice = 0;

  int refgnum, curgnum;
  int refposition = 0;
  int curposition = 0;

  form_features_sections();

  std::vector<std::vector<float> >  misorients;
  misorients.resize(dims[0]);
  for (DimType a = 0; a < dims[0]; a++)
  {
    misorients[a].assign(dims[1], 0.0);
  }
  for (DimType iter = 1; iter < dims[2]; iter++)
  {
    QString ss = QObject::tr("Aligning Sections - Determining Shifts - %1 Percent Complete").arg(((float)iter / dims[2]) * 100);
    //  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    mindisorientation = 100000000;
    slice = static_cast<int>( (dims[2] - 1) - iter );
    featurecount1 = featurecounts[slice];
    featurecount2 = featurecounts[slice + 1];
    mutualinfo12 = new float *[featurecount1];
    mutualinfo1 = new float[featurecount1];
    mutualinfo2 = new float[featurecount2];

    for (int a = 0; a < featurecount1; a++)
    {
      mutualinfo1[a] = 0.0f;
      mutualinfo12[a] = new float[featurecount2];
      for (int b = 0; b < featurecount2; b++)
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
      for (int j = -3; j < 4; j++)
      {
        for (int k = -3; k < 4; k++)
        {
          disorientation = 0;
          count = 0;
          if(misorients[k + oldxshift + size_t(dims[0] / 2)][j + oldyshift + (size_t)(dims[1] / 2)] == 0 && abs(k + oldxshift) < (dims[0] / 2)
              && (j + oldyshift) < (dims[1] / 2))
          {
            for (DimType l = 0; l < dims[1]; l = l + 4)
            {
              for (DimType n = 0; n < dims[0]; n = n + 4)
              {
                if((l + j + oldyshift) >= 0 && (l + j + oldyshift) < dims[1] && (n + k + oldxshift) >= 0 && (n + k + oldxshift) < dims[0])
                {
                  refposition = static_cast<int>( ((slice + 1) * dims[0] * dims[1]) + (l * dims[0]) + n );
                  curposition = static_cast<int>( (slice * dims[0] * dims[1]) + ((l + j + oldyshift) * dims[0]) + (n + k + oldxshift) );
                  refgnum = m_FeatureIds[refposition];
                  curgnum = m_FeatureIds[curposition];
                  if(curgnum >= 0 && refgnum >= 0)
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
            float ha = 0;
            float hb = 0;
            float hab = 0;
            for (int b = 0; b < featurecount1; b++)
            {
              mutualinfo1[b] = mutualinfo1[b] / float(count);
              if(mutualinfo1[b] != 0) { ha = ha + mutualinfo1[b] * log(mutualinfo1[b]); }
            }
            for (int c = 0; c < featurecount2; c++)
            {
              mutualinfo2[c] = mutualinfo2[c] / float(count);
              if(mutualinfo2[c] != 0) { hb = hb + mutualinfo2[c] * log(mutualinfo2[c]); }
            }
            for (int b = 0; b < featurecount1; b++)
            {
              for (int c = 0; c < featurecount2; c++)
              {
                mutualinfo12[b][c] = mutualinfo12[b][c] / float(count);
                if(mutualinfo12[b][c] != 0) { hab = hab + mutualinfo12[b][c] * log(mutualinfo12[b][c]); }
                float value = 0;
                if(mutualinfo1[b] > 0 && mutualinfo2[c] > 0) { value = (mutualinfo12[b][c] / (mutualinfo1[b] * mutualinfo2[c])); }
                if(value != 0) { disorientation = disorientation + (mutualinfo12[b][c] * log(value)); }
              }
            }
            for (int b = 0; b < featurecount1; b++)
            {
              for (int c = 0; c < featurecount2; c++)
              {
                mutualinfo12[b][c] = 0;
                mutualinfo1[b] = 0;
                mutualinfo2[c] = 0;
              }
            }
            disorientation = static_cast<float>( 1.0 / disorientation );
            misorients[k + oldxshift + int(dims[0] / 2)][j + oldyshift + int(dims[1] / 2)] = disorientation;
            if(disorientation < mindisorientation)
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
    if(getWriteAlignmentShifts() == true)
    {
      outFile << slice << "	" << slice + 1 << "	" << newxshift << "	" << newyshift << "	" << xshifts[iter] << "	" << yshifts[iter] << "\n";
    }
    delete[] mutualinfo1;
    delete[] mutualinfo2;
    for (int i = 0; i < featurecount1; i++)
    {
      delete mutualinfo12[i];
    }
    delete[] mutualinfo12;
    mutualinfo1 = NULL;
    mutualinfo2 = NULL;
    mutualinfo12 = NULL;
  }

  m->getAttributeMatrix(getCellAttributeMatrixName())->removeAttributeArray(DREAM3D::CellData::FeatureIds);

  if(getWriteAlignmentShifts() == true)
  {
    outFile.close();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsMutualInformation::form_features_sections()
{
  DREAM3D_RANDOMNG_NEW()
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
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

  // int64_t totalPoints = m->totalPoints();


  int point = 0;
  int seed = 0;
  int noseeds = 0;
  int featurecount = 1;
  int neighbor;
  // int currentpoint;
  QuatF q1;
  QuatF q2;
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);
  //  float qs[5];
  float w;
  float n1;
  float n2;
  float n3;
  DimType randx = 0;
  DimType randy = 0;
  DimType good = 0;
  DimType x, y, z;
  DimType col, row;
  size_t size = 0;
  size_t initialVoxelsListSize = 1000;

  m_FeatureCounts->resize(dims[2]);
  featurecounts = m_FeatureCounts->getPointer(0);

  std::vector<int> voxelslist(initialVoxelsListSize, -1);
  DimType neighpoints[4];
  neighpoints[0] = -dims[0];
  neighpoints[1] = -1;
  neighpoints[2] = 1;
  neighpoints[3] = dims[0];

  unsigned int phase1, phase2;
  for (DimType slice = 0; slice < dims[2]; slice++)
  {

    QString ss = QObject::tr("Aligning Sections - Identifying Features on Sections - %1 Percent Complete").arg(((float)slice / dims[2]) * 100);
    featurecount = 1;
    noseeds = 0;
    while (noseeds == 0)
    {
      seed = -1;
      randx = int(float(rg.genrand_res53()) * float(dims[0]));
      randy = int(float(rg.genrand_res53()) * float(dims[1]));
      for (DimType j = 0; j < dims[1]; ++j)
      {
        for (DimType i = 0; i < dims[0]; ++i)
        {
          x = randx + i;
          y = randy + j;
          z = slice;
          if(x > dims[0] - 1) { x = x - dims[0]; }
          if(y > dims[1] - 1) { y = y - dims[1]; }
          point = static_cast<int>( (z * dims[0] * dims[1]) + (y * dims[0]) + x );
          if((m_UseGoodVoxels == false || m_GoodVoxels[point] == true) && m_FeatureIds[point] == 0 && m_CellPhases[point] > 0)
          {
            seed = point;
          }
          if(seed > -1) { break; }
        }
        if(seed > -1) { break; }
      }
      if(seed == -1) { noseeds = 1; }
      if(seed >= 0)
      {
        size = 0;
        m_FeatureIds[seed] = featurecount;
        voxelslist[size] = seed;
        size++;
        for (size_t j = 0; j < size; ++j)
        {
          int currentpoint = voxelslist[j];
          col = currentpoint % dims[0];
          row = (currentpoint / dims[0]) % dims[1];
          QuaternionMathF::Copy(quats[currentpoint], q1);
          phase1 = m_CrystalStructures[m_CellPhases[currentpoint]];
          for (int i = 0; i < 4; i++)
          {
            good = 1;
            neighbor = static_cast<int>( currentpoint + neighpoints[i] );
            if((i == 0) && row == 0) { good = 0; }
            if((i == 3) && row == (dims[1] - 1)) { good = 0; }
            if((i == 1) && col == 0) { good = 0; }
            if((i == 2) && col == (dims[0] - 1)) { good = 0; }
            if(good == 1 && m_FeatureIds[neighbor] <= 0 && m_CellPhases[neighbor] > 0)
            {
              w = 10000.0f;
              QuaternionMathF::Copy(quats[neighbor], q2);
              phase2 = m_CrystalStructures[m_CellPhases[neighbor]];
              if(phase1 == phase2)
              {
                w = m_OrientationOps[phase1]->getMisoQuat(q1, q2, n1, n2, n3);
              }
              if(w < m_MisorientationTolerance)
              {
                m_FeatureIds[neighbor] = featurecount;
                voxelslist[size] = neighbor;
                size++;
                if(size >= voxelslist.size())
                {
                  size = voxelslist.size();
                  voxelslist.resize(size + initialVoxelsListSize);
                  for(std::vector<int>::size_type v = size; v < voxelslist.size(); ++v) { voxelslist[v] = -1; }
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
{ return Reconstruction::ReconstructionBaseName; }


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

