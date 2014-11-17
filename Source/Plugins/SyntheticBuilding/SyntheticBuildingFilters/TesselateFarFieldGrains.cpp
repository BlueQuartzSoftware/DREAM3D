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

#include "TesselateFarFieldGrains.h"
#include <algorithm>


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range3d.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif


#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/StatsData/PrimaryStatsData.h"
#include "DREAM3DLib/ShapeOps/EllipsoidOps.h"
#include "DREAM3DLib/CoreFilters/DataContainerWriter.h"
#include "DREAM3DLib/Utilities/TimeUtilities.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "DREAM3DLib/Utilities/FilePathGenerator.h"

//// Macro to determine if we are going to show the Debugging Output files
#define PPP_SHOW_DEBUG_OUTPUTS 0

#if 0

#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

#define GG_INIT_DOUBLE_ARRAY(array, value, size)\
  for(size_t n = 0; n < size; ++n) { array[n] = (value); }

#endif

#if (CMP_SIZEOF_SIZE_T == 4)
typedef int32_t DimType;
#else
typedef int64_t DimType;
#endif




/**
 * @brief
 */
class AssignVoxelsImpl
{
    DimType dims[3];
    float Invradcur[3];
    float res[3];
    int32_t* m_FeatureIds;
    float xc;
    float yc;
    float zc;
    ShapeOps::Pointer m_EllipsoidOps;
    float ga[3][3];
    int curFeature;
    Int32ArrayType::Pointer newownersPtr;
    FloatArrayType::Pointer ellipfuncsPtr;

  public:
    AssignVoxelsImpl(DimType* dimensions, float* resolution, int32_t* featureIds, float* radCur,
                         float* xx, ShapeOps::Pointer ellipsoidOps, float gA[3][3], float* size, int cur_feature,
    Int32ArrayType::Pointer newowners, FloatArrayType::Pointer ellipfuncs) :
      m_FeatureIds(featureIds),
      m_EllipsoidOps(ellipsoidOps),
      curFeature(cur_feature)
    {
      dims[0] = dimensions[0];
      dims[1] = dimensions[1];
      dims[2] = dimensions[2];
      Invradcur[0] = 1.0 / radCur[0];
      Invradcur[1] = 1.0 / radCur[1];
      Invradcur[2] = 1.0 / radCur[2];

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
    virtual ~AssignVoxelsImpl() {}

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void convert(int zStart, int zEnd, int yStart, int yEnd, int xStart, int xEnd) const
    {

      int column = 0;
      int row = 0;
      int plane = 0;
      int index = 0;
      float gaCopy[3][3];
      float gaT[3][3];
      float coords[3] = {0.0f, 0.0f, 0.0f};
      float inside = 0.0f;
      //   float dist = 0.0f;
      //   float radcur1squared = 1.0/(Invradcur[0]*Invradcur[0]);
      float coordsRotated[3] = {0.0f, 0.0f, 0.0f};
      int32_t* newowners = newownersPtr->getPointer(0);
      float* ellipfuncs = ellipfuncsPtr->getPointer(0);

      //making a copy because the transpose function used later can't deal with the const nature of ga
      for(int i=0;i<3;i++)
      {
        for(int j=0;j<3;j++)
        {
          gaCopy[i][j] = ga[i][j];
        }
      }

      DimType dim0_dim_1 = dims[0] * dims[1];
      for (DimType iter1 = xStart; iter1 < xEnd; iter1++)
      {
        column = iter1;
        if (iter1 < 0) { column = iter1 + dims[0]; }
        else if (iter1 > dims[0] - 1) { column = iter1 - dims[0]; }

        for (DimType iter2 = yStart; iter2 < yEnd; iter2++)
        {
          row = iter2;
          if (iter2 < 0) { row = iter2 + dims[1]; }
          else if (iter2 > dims[1] - 1) { row = iter2 - dims[1]; }
          size_t row_dim = row * dims[0];

          for (DimType iter3 = zStart; iter3 < zEnd; iter3++)
          {
            plane = iter3;
            if (iter3 < 0) { plane = iter3 + dims[2]; }
            else if (iter3 > dims[2] - 1) { plane = iter3 - dims[2]; }

            index = static_cast<int>( (plane * dim0_dim_1) + (row_dim) + column );

            inside = -1;
            coords[0] = float(iter1) * res[0];
            coords[1] = float(iter2) * res[1];
            coords[2] = float(iter3) * res[2];

            coords[0] = coords[0] - xc;
            coords[1] = coords[1] - yc;
            coords[2] = coords[2] - zc;
            MatrixMath::Transpose3x3(gaCopy, gaT);
            MatrixMath::Multiply3x3with3x1(gaT, coords, coordsRotated);
            float axis1comp = coordsRotated[0] * Invradcur[0];
            float axis2comp = coordsRotated[1] * Invradcur[1];
            float axis3comp = coordsRotated[2] * Invradcur[2];
            inside = m_EllipsoidOps->inside(axis1comp, axis2comp, axis3comp);
            //if (inside >= 0 && newowners[index] > 0)
            if (inside >= 0 && newowners[index] > 0 && inside > ellipfuncs[index])
            {
              newowners[index] = curFeature;
              ellipfuncs[index] = inside;
              //newowners[index] = -2;
              //ellipfuncs[index] = inside;
            }
            else if (inside >= 0 && newowners[index] == -1)
            {
              newowners[index] = curFeature;
              ellipfuncs[index] = inside;
            }
          }
        }
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range3d<int, int, int>& r) const
    {
      convert(r.pages().begin(), r.pages().end(), r.rows().begin(), r.rows().end(), r.cols().begin(), r.cols().end());
    }
#endif

  private:

};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TesselateFarFieldGrains::TesselateFarFieldGrains() :
  AbstractFilter(),
  m_OutputCellAttributeMatrixName(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, ""),
  m_OutputCellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_OutputCellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeatureEulerAnglesArrayName(DREAM3D::FeatureData::EulerAngles),
  m_ElasticStrainsArrayName(DREAM3D::FeatureData::ElasticStrains),
  m_CentroidsArrayName(DREAM3D::FeatureData::Centroids),
  m_VolumesArrayName(DREAM3D::FeatureData::Volumes),
  m_AxisLengthsArrayName(DREAM3D::FeatureData::AxisLengths),
  m_AxisEulerAnglesArrayName(DREAM3D::FeatureData::AxisEulerAngles),
  m_Omega3sArrayName(DREAM3D::FeatureData::Omega3s),
  m_EquivalentDiametersArrayName(DREAM3D::FeatureData::EquivalentDiameters),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_MaskArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::GoodVoxels),
  m_FeatureIds(NULL),
  m_CellPhases(NULL),
  m_MaskArrayName(DREAM3D::CellData::GoodVoxels),
  m_Mask(NULL),
  m_FeaturePhases(NULL),
  m_Centroids(NULL),
  m_Volumes(NULL),
  m_AxisLengths(NULL),
  m_AxisEulerAngles(NULL),
  m_Omega3s(NULL),
  m_EquivalentDiameters(NULL),
  m_CrystalStructures(NULL)
{
  m_EllipsoidOps = EllipsoidOps::New();

  m_OrthoOps = OrthoRhombicOps::New();

  m_FeatureInputFileListInfo.StartIndex = 0;
  m_FeatureInputFileListInfo.EndIndex = 0;
  m_FeatureInputFileListInfo.PaddingDigits = 0;
  m_FeatureInputFileListInfo.Ordering = 0;
  m_FeatureInputFileListInfo.FileExtension = "";
  m_FeatureInputFileListInfo.FilePrefix = "";
  m_FeatureInputFileListInfo.FileSuffix = "";
  m_FeatureInputFileListInfo.InputPath = "";

  m_Seed = QDateTime::currentMSecsSinceEpoch();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TesselateFarFieldGrains::~TesselateFarFieldGrains()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TesselateFarFieldGrains::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FileListInfoFilterParameter::New("Feature Input File List", "FeatureInputFileListInfo", getFeatureInputFileListInfo(), false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Mask Array", "MaskArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getMaskArrayPath(), true));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Cell Attribute Matrix Name", "OutputCellAttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getOutputCellAttributeMatrixName(), true));
  parameters.push_back(FilterParameter::New("Cell Feature Attribute Matrix Name", "OutputCellFeatureAttributeMatrixName", FilterParameterWidgetType::StringWidget, getOutputCellFeatureAttributeMatrixName(), true));
  parameters.push_back(FilterParameter::New("Cell Ensemble Attribute Matrix Name", "OutputCellEnsembleAttributeMatrixName", FilterParameterWidgetType::StringWidget, getOutputCellEnsembleAttributeMatrixName(), true));
  parameters.push_back(FilterParameter::New("Feature Ids Array Name", "FeatureIdsArrayName", FilterParameterWidgetType::StringWidget, getFeatureIdsArrayName(), true));
  parameters.push_back(FilterParameter::New("Cell Phases Array Name", "CellPhasesArrayName", FilterParameterWidgetType::StringWidget, getCellPhasesArrayName(), true));
  parameters.push_back(FilterParameter::New("Feature Phases Array Name", "FeaturePhasesArrayName", FilterParameterWidgetType::StringWidget, getFeaturePhasesArrayName(), true));
  parameters.push_back(FilterParameter::New("Feature Euler Angles Array Name", "FeatureEulerAnglesArrayName", FilterParameterWidgetType::StringWidget, getFeatureEulerAnglesArrayName(), true));
  parameters.push_back(FilterParameter::New("Feature Elastic Strains Array Name", "ElasticStrainsArrayName", FilterParameterWidgetType::StringWidget, getElasticStrainsArrayName(), true));
  parameters.push_back(FilterParameter::New("Crystal Structures Array Name", "CrystalStructuresArrayName", FilterParameterWidgetType::StringWidget, getCrystalStructuresArrayName(), true));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TesselateFarFieldGrains::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOutputCellAttributeMatrixName( reader->readDataArrayPath("OutputCellAttributeMatrixName", getOutputCellAttributeMatrixName() ) );
  setOutputCellFeatureAttributeMatrixName( reader->readString("OutputCellFeatureAttributeMatrixName", getOutputCellFeatureAttributeMatrixName() ) );
  setOutputCellEnsembleAttributeMatrixName( reader->readString("OutputCellEnsembleAttributeMatrixName", getOutputCellEnsembleAttributeMatrixName() ) );
  setFeatureInputFileListInfo( reader->readFileListInfo("FeatureInputFileListInfo", getFeatureInputFileListInfo() ) );
  setFeatureIdsArrayName( reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName() ) );
  setCellPhasesArrayName( reader->readString("CellPhasesArrayName", getCellPhasesArrayName() ) );
  setFeaturePhasesArrayName( reader->readString("FeaturePhasesArrayName", getFeaturePhasesArrayName() ) );
  setFeatureEulerAnglesArrayName( reader->readString("FeatureEulerAnglesArrayName", getFeatureEulerAnglesArrayName() ) );
  setElasticStrainsArrayName( reader->readString("ElasticStrainsArrayName", getElasticStrainsArrayName() ) );
  setCrystalStructuresArrayName( reader->readString("CrystalStructuresArrayName", getCrystalStructuresArrayName() ) );
  setMaskArrayPath(reader->readDataArrayPath("MaskArrayPath", getMaskArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TesselateFarFieldGrains::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(OutputCellAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputCellFeatureAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputCellEnsembleAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureInputFileListInfo)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellPhasesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(FeaturePhasesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureEulerAnglesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(ElasticStrainsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(MaskArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TesselateFarFieldGrains::updateFeatureInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_EquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_VolumesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Volumes = m_VolumesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_Omega3sPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Omega3s = m_Omega3sPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_CentroidsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Centroids = m_CentroidsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_AxisEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AxisEulerAngles = m_AxisEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_AxisLengthsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AxisLengths = m_AxisLengthsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_ElasticStrainsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ElasticStrains = m_ElasticStrainsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_FeatureEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TesselateFarFieldGrains::updateEnsembleInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TesselateFarFieldGrains::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);
  // This is for convenience

  // Make sure we have our input DataContainer with the proper Ensemble data
  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getOutputCellAttributeMatrixName().getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  //Input Ensemble Data That we require

  QVector<size_t> dims(1, 1);
  m_MaskPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getMaskArrayPath(), dims);
  if( NULL != m_MaskPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Mask = m_MaskPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  //Cell Data
  tempPath.update(getOutputCellAttributeMatrixName().getDataContainerName(), getOutputCellAttributeMatrixName().getAttributeMatrixName(), getFeatureIdsArrayName() );
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, -1, dims); /* Assigns the shared_ptr<>(this, tempPath, -1, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getOutputCellAttributeMatrixName().getDataContainerName(), getOutputCellAttributeMatrixName().getAttributeMatrixName(), getCellPhasesArrayName() );
  m_CellPhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<>(this, tempPath, 0, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer cellFeatureAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getOutputCellFeatureAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellFeature);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getOutputCellEnsembleAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellEnsemble);
  if(getErrorCondition() < 0) { return; }

  //Feature Data
  tempPath.update(getOutputCellAttributeMatrixName().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), getFeaturePhasesArrayName() );
  m_FeaturePhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getOutputCellAttributeMatrixName().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), getEquivalentDiametersArrayName() );
  m_EquivalentDiametersPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_EquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getOutputCellAttributeMatrixName().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), getVolumesArrayName() );
  m_VolumesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_VolumesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Volumes = m_VolumesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getOutputCellAttributeMatrixName().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), getOmega3sArrayName() );
  m_Omega3sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_Omega3sPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Omega3s = m_Omega3sPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  tempPath.update(getOutputCellAttributeMatrixName().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), getCentroidsArrayName() );
  m_CentroidsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CentroidsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Centroids = m_CentroidsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getOutputCellAttributeMatrixName().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), getAxisEulerAnglesArrayName() );
  m_AxisEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AxisEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AxisEulerAngles = m_AxisEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getOutputCellAttributeMatrixName().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), getAxisLengthsArrayName() );
  m_AxisLengthsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AxisLengthsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AxisLengths = m_AxisLengthsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getOutputCellAttributeMatrixName().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), getFeatureEulerAnglesArrayName() );
  m_FeatureEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims.resize(2);
  dims[0] = 3;
  dims[1] = 3;
  tempPath.update(getOutputCellAttributeMatrixName().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), getElasticStrainsArrayName() );
  m_ElasticStrainsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ElasticStrainsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ElasticStrains = m_ElasticStrainsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  //Ensemble Data
  dims.resize(1);
  dims[0] = 1;
  tempPath.update(getOutputCellAttributeMatrixName().getDataContainerName(), getOutputCellEnsembleAttributeMatrixName(), getCrystalStructuresArrayName() );
  m_CrystalStructuresPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter, unsigned int>(this,  tempPath, Ebsd::CrystalStructure::UnknownCrystalStructure, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TesselateFarFieldGrains::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();

  bool hasMissingFiles = false;
  bool orderAscending = false;

  if(m_FeatureInputFileListInfo.Ordering == 0) { orderAscending = true; }
  else if (m_FeatureInputFileListInfo.Ordering == 1) { orderAscending = false; }

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList = FilePathGenerator::GenerateFileList(m_FeatureInputFileListInfo.StartIndex,
                              m_FeatureInputFileListInfo.EndIndex, hasMissingFiles, orderAscending,
                              m_FeatureInputFileListInfo.InputPath, m_FeatureInputFileListInfo.FilePrefix,
                              m_FeatureInputFileListInfo.FileSuffix, m_FeatureInputFileListInfo.FileExtension,
                              m_FeatureInputFileListInfo.PaddingDigits);
  if (fileList.size() == 0)
  {
    QString ss = QObject::tr("No files have been selected for import. Have you set the input directory?");
    setErrorCondition(-11);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }


  DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixName());
  if(dc == NULL) { setInPreflight(false); return; }
  AttributeMatrix::Pointer attrMat = dc->getAttributeMatrix(getOutputCellFeatureAttributeMatrixName());
  if(attrMat == NULL) { setInPreflight(false); return; }

  attrMat->removeAttributeArray(m_EquivalentDiametersArrayName);
  attrMat->removeAttributeArray(m_Omega3sArrayName);
  attrMat->removeAttributeArray(m_AxisEulerAnglesArrayName);
  attrMat->removeAttributeArray(m_AxisLengthsArrayName);
  attrMat->removeAttributeArray(m_VolumesArrayName);
  attrMat->removeAttributeArray(m_CentroidsArrayName);
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TesselateFarFieldGrains::execute()
{
  int err = 0;
  setErrorCondition(err);
  DREAM3D_RANDOMNG_NEW()
      dataCheck();
  if(getErrorCondition() < 0) { return; }

  notifyStatusMessage(getHumanLabel(), "Loading Features");
  load_features();
  if (getCancel() == true) { return; }

  notifyStatusMessage(getHumanLabel(), "Assigning Voxels");
  assign_voxels();
  if (getCancel() == true) { return; }

  notifyStatusMessage(getHumanLabel(), "Assigning Gaps");
  assign_gaps_only();
  if (getCancel() == true) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getOutputCellAttributeMatrixName().getDataContainerName());
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName);
  cellFeatureAttrMat->removeAttributeArray(m_EquivalentDiametersArrayName);
  cellFeatureAttrMat->removeAttributeArray(m_Omega3sArrayName);
  cellFeatureAttrMat->removeAttributeArray(m_AxisEulerAnglesArrayName);
  cellFeatureAttrMat->removeAttributeArray(m_AxisLengthsArrayName);
  cellFeatureAttrMat->removeAttributeArray(m_VolumesArrayName);
  cellFeatureAttrMat->removeAttributeArray(m_CentroidsArrayName);

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Tesselating Features Complete");

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void  TesselateFarFieldGrains::load_features()
{

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getOutputCellAttributeMatrixName().getDataContainerName());
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName);
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->getAttributeMatrix(m_OutputCellEnsembleAttributeMatrixName);

  bool hasMissingFiles = false;
  bool orderAscending = false;

  if(m_FeatureInputFileListInfo.Ordering == 0) { orderAscending = true; }
  else if (m_FeatureInputFileListInfo.Ordering == 1) { orderAscending = false; }

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList = FilePathGenerator::GenerateFileList(m_FeatureInputFileListInfo.StartIndex,
                              m_FeatureInputFileListInfo.EndIndex, hasMissingFiles, orderAscending,
                              m_FeatureInputFileListInfo.InputPath, m_FeatureInputFileListInfo.FilePrefix,
                              m_FeatureInputFileListInfo.FileSuffix, m_FeatureInputFileListInfo.FileExtension,
                              m_FeatureInputFileListInfo.PaddingDigits);  std::ifstream inFile;

  size_t currentFeature = 1;
  for (QVector<QString>::iterator filepath = fileList.begin(); filepath != fileList.end(); ++filepath)
  {
    QString fName = *filepath;
    QString ss = QObject::tr("Importing file %1").arg(fName);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

    inFile.open(fName.toLatin1().data(), std::ios_base::binary);
    if(!inFile)
    {
      QString ss = QObject::tr("Failed to open: %1").arg(fName);
      setErrorCondition(-1);
      notifyErrorMessage(getHumanLabel(), ss, -1);
    }

    // variable for holding meta data
    int numPhases = 1;
    int numFeatures;
    float beamCenter, beamThickness, globalZPos;
    std::string dummyStr, phaseName, crystruct;
    float aRef, bRef, cRef, alphaRef, betaRef, gammaRef;

    inFile >> dummyStr >> numFeatures;
    if (0 == numFeatures)
    {
      notifyErrorMessage(getHumanLabel(), "The number of features is Zero and should be greater than Zero", -600);
    }
    QVector<size_t> tDims(1, currentFeature + numFeatures);
    cellFeatureAttrMat->setTupleDimensions(tDims);
    updateFeatureInstancePointers();

    inFile >> dummyStr >> beamCenter;
    inFile >> dummyStr >> beamThickness;
    inFile >> dummyStr >> globalZPos;

    bool keepGoing = true;
    while(keepGoing == true)
    {
      keepGoing = false;
      inFile >> dummyStr >> phaseName >> crystruct >> aRef >> bRef >> cRef >> alphaRef >> betaRef >> gammaRef;
      numPhases++;
      tDims[0] = numPhases;
      cellEnsembleAttrMat->setTupleDimensions(tDims);
      updateEnsembleInstancePointers();
      m_CrystalStructures[numPhases=1] = Ebsd::CrystalStructure::Cubic_High;
    }

    size_t fId;
    int phase = 1;
    float xC, yC, zC;
    float a, b, c, alpha, beta, gamma;
    float vol, eqRad;
    float conf;
    float phi1, PHI, phi2;
    float dummy1, dummy2, dummy3;
    float mat[3][3];
    float rt[3][3];
    float rtInv[3][3];
    float rtMult[3][3];
    float rtAvg[3][3];
    float eps[3][3];
    float epsT[3][3];
    float epsAdd[3][3];
    float epsMult[3][3];
    float identity[3][3];
    float flst[3][3];
    const float fourThirds = 4.0f / 3.0f;

    alphaRef *= DREAM3D::Constants::k_PiOver180;
    betaRef *= DREAM3D::Constants::k_PiOver180;
    gammaRef *= DREAM3D::Constants::k_PiOver180;
    OrientationMath::RootTensorFromLatticeParameters(aRef, bRef, cRef, alphaRef, betaRef, gammaRef, rtAvg);
    MatrixMath::Identity3x3(identity);
    for(int i = 0; i < numFeatures; i++)
    {
      inFile >> fId >> phase >> mat[0][0] >> mat[0][1] >> mat[0][2] >> mat[1][0] >> mat[1][1] >> mat[1][2] >> mat[2][0] >> mat[2][1] >> mat[2][2] >> xC >> yC >> zC >> a >> b >> c >> alpha >> beta >> gamma >> dummy1 >> dummy2 >> dummy3 >> eqRad >> conf;

      if(fabs(zC-beamCenter) <= (beamThickness/2.0))
      {
        m_Centroids[3* currentFeature + 0] = xC;
        m_Centroids[3* currentFeature + 1] = yC;
        m_Centroids[3* currentFeature + 2] = zC + globalZPos;

        vol = fourThirds * DREAM3D::Constants::k_Pi * eqRad * eqRad * eqRad;
        m_Volumes[currentFeature] = vol;
        m_EquivalentDiameters[currentFeature] = eqRad*2.0;
        m_AxisLengths[3 * currentFeature + 0] = 1.0;
        m_AxisLengths[3 * currentFeature + 1] = 1.0;
        m_AxisLengths[3 * currentFeature + 2] = 1.0;
        m_AxisEulerAngles[3 * currentFeature + 0] = 0.0;
        m_AxisEulerAngles[3 * currentFeature + 1] = 0.0;
        m_AxisEulerAngles[3 * currentFeature + 2] = 0.0;
        m_Omega3s[currentFeature] = 1.0;

        m_FeaturePhases[currentFeature] = phase;

        OrientationMath::MattoEuler(mat, phi1, PHI, phi2);
        m_FeatureEulerAngles[3*currentFeature + 0] = phi1;
        m_FeatureEulerAngles[3*currentFeature + 1] = PHI;
        m_FeatureEulerAngles[3*currentFeature + 2] = phi2;

        alpha *= DREAM3D::Constants::k_PiOver180;
        beta *= DREAM3D::Constants::k_PiOver180;
        gamma *= DREAM3D::Constants::k_PiOver180;
        OrientationMath::RootTensorFromLatticeParameters(a, b, c, alpha, beta, gamma, rt);
        MatrixMath::Invert3x3(rt, rtInv);
        MatrixMath::Multiply3x3with3x3(rtInv, rtAvg, rtMult);
        MatrixMath::Subtract3x3s(rtMult, identity, eps);
        MatrixMath::Transpose3x3(eps, epsT);
        MatrixMath::Multiply3x3with3x3(epsT, eps, epsMult);
        MatrixMath::Add3x3s(eps, epsT, epsAdd);
        MatrixMath::Add3x3s(epsAdd, epsMult, flst);
        MatrixMath::Multiply3x3withConstant(flst, 0.5);

        m_ElasticStrains[9*currentFeature + 0] = flst[0][0];
        m_ElasticStrains[9*currentFeature + 1] = flst[0][1];
        m_ElasticStrains[9*currentFeature + 2] = flst[0][2];
        m_ElasticStrains[9*currentFeature + 3] = flst[1][0];
        m_ElasticStrains[9*currentFeature + 4] = flst[1][1];
        m_ElasticStrains[9*currentFeature + 5] = flst[1][2];
        m_ElasticStrains[9*currentFeature + 6] = flst[2][0];
        m_ElasticStrains[9*currentFeature + 7] = flst[2][1];
        m_ElasticStrains[9*currentFeature + 8] = flst[2][2];
        currentFeature++;
      }
    }
    inFile.close();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TesselateFarFieldGrains::assign_voxels()
{
  notifyStatusMessage(getHumanLabel(), "Assigning Voxels");

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getOutputCellAttributeMatrixName().getDataContainerName());

  int64_t totalPoints = m->getAttributeMatrix(m_OutputCellAttributeMatrixName.getAttributeMatrixName())->getNumTuples();

  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);

  DimType dims[3] =
  {
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

  Int32ArrayType::Pointer newownersPtr = Int32ArrayType::CreateArray(totalPoints, "newowners");
  newownersPtr->initializeWithValue(-1);
  int32_t* newowners = newownersPtr->getPointer(0);

  FloatArrayType::Pointer ellipfuncsPtr = FloatArrayType::CreateArray(totalPoints, "ellipfuncs");
  ellipfuncsPtr->initializeWithValue(-1);
  float* ellipfuncs = ellipfuncsPtr->getPointer(0);

  float featuresPerTime = 0;
  uint64_t millis = QDateTime::currentMSecsSinceEpoch();
  uint64_t currentMillis = millis;

  int64_t totalFeatures = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumTuples();
  for (int64_t i = 1; i < totalFeatures; i++)
  {
    featuresPerTime++;
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if (currentMillis - millis > 1000)
    {
      float rate = featuresPerTime / ( (float)(currentMillis - millis) ) * 1000.0f;

      QString ss = QObject::tr("Assign Voxels & Gaps|| Features Checked: %1 || Features/Second: %2").arg(i).arg((int)rate);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      featuresPerTime = 0;
      millis = QDateTime::currentMSecsSinceEpoch();
    }
    float volcur = m_Volumes[i];
    float bovera = m_AxisLengths[3 * i + 1];
    float covera = m_AxisLengths[3 * i + 2];
    float omega3 = m_Omega3s[i];
    xc = m_Centroids[3 * i];
    yc = m_Centroids[3 * i + 1];
    zc = m_Centroids[3 * i + 2];
    float radcur1 = 0.0f;

    // init any values for each of the Shape Ops
    m_EllipsoidOps->init();

    // Create our Argument Map
    QMap<ShapeOps::ArgName, float> shapeArgMap;
    shapeArgMap[ShapeOps::Omega3] = omega3;
    shapeArgMap[ShapeOps::VolCur] = volcur;
    shapeArgMap[ShapeOps::B_OverA] = bovera;
    shapeArgMap[ShapeOps::C_OverA] = covera;

    radcur1 = m_EllipsoidOps->radcur1(shapeArgMap);

    float radcur2 = (radcur1 * bovera);
    float radcur3 = (radcur1 * covera);
    float phi1 = m_AxisEulerAngles[3 * i];
    float PHI = m_AxisEulerAngles[3 * i + 1];
    float phi2 = m_AxisEulerAngles[3 * i + 2];
    float ga[3][3];
    OrientationMath::EulertoMat(phi1, PHI, phi2, ga);
    column = static_cast<DimType>( xc / xRes );
    row = static_cast<DimType>( yc / yRes );
    plane = static_cast<DimType>( zc / zRes );
    xmin = int(column - ((radcur1 / xRes) + 1));
    xmax = int(column + ((radcur1 / xRes) + 1));
    ymin = int(row - ((radcur1 / yRes) + 1)); // <======================
    ymax = int(row + ((radcur1 / yRes) + 1)); // <======================
    zmin = int(plane - ((radcur1 / zRes) + 1)); // <======================
    zmax = int(plane + ((radcur1 / zRes) + 1)); // <======================

    if (xmin < 0) { xmin = 0; }
    if (xmax > dims[0] - 1) { xmax = dims[0] - 1; }
    if (ymin < 0) { ymin = 0; }
    if (ymax > dims[1] - 1) { ymax = dims[1] - 1; }
    if (zmin < 0) { zmin = 0; }
    if (zmax > dims[2] - 1) { zmax = dims[2] - 1; }

    float radCur[3] = { radcur1, radcur2, radcur3 };
    float xx[3] = {xc, yc, zc };

    //#if 0
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range3d<int, int, int>(zmin, zmax + 1, ymin, ymax + 1, xmin, xmax + 1),
        AssignVoxelsImpl(dims, res, m_FeatureIds, radCur, xx, m_EllipsoidOps, ga, size, i, newownersPtr, ellipfuncsPtr), tbb::auto_partitioner());

    }
    else
#endif
    {
      AssignVoxelsImpl serial(dims, res, m_FeatureIds, radCur, xx, m_EllipsoidOps, ga, size, i, newownersPtr, ellipfuncsPtr);
      serial.convert(zmin, zmax + 1, ymin, ymax + 1, xmin, xmax + 1);
    }


  }

  QVector<bool> activeObjects(totalFeatures, false);
  int gnum;
  for (size_t i = 0; i < static_cast<size_t>(totalPoints); i++)
  {
//    if(ellipfuncs[i] >= 0) { m_FeatureIds[i] = newowners[i]; }
    if(ellipfuncs[i] >= 0 && m_Mask[i] == true) { m_FeatureIds[i] = newowners[i]; }
    if(m_Mask[i] == false) m_FeatureIds[i] = 0;
    gnum = m_FeatureIds[i];
    if(gnum >= 0) { activeObjects[gnum] = true; }
    newowners[i] = -1;
    ellipfuncs[i] = -1.0;
  }

  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(getOutputCellFeatureAttributeMatrixName());
  cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());
  //need to update pointers after removing inactive objects
  updateFeatureInstancePointers();

  if (getCancel() == true)
  {
    QString ss = QObject::tr("Filter Cancelled.");
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
    return;
  }

  for(int64_t i = 0; i < totalPoints; i++)
  {
    gnum = m_FeatureIds[i];
    if(gnum >= 0)
    {
      m_CellPhases[i] = m_FeaturePhases[gnum];
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TesselateFarFieldGrains::assign_gaps_only()
{
  notifyStatusMessage(getHumanLabel(), "Assigning Gaps");

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getOutputCellAttributeMatrixName().getDataContainerName());

  int featurename, feature;
  int current = 0;
  int most = 0;
  int count = 1;
  int fixedCount = 1;
  int counter = 0;
  int neighpoint;
  int good;
  int neighbor;

  int xPoints = static_cast<int>(m->getXPoints());
  int yPoints = static_cast<int>(m->getYPoints());
  int zPoints = static_cast<int>(m->getZPoints());
  size_t totalPoints = m->getAttributeMatrix(m_OutputCellAttributeMatrixName.getAttributeMatrixName())->getNumTuples();
  size_t totalFeatures = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumTuples();

  int neighpoints[6];
  neighpoints[0] = -xPoints * yPoints;
  neighpoints[1] = -xPoints;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xPoints;
  neighpoints[5] = xPoints * yPoints;

  Int32ArrayType::Pointer neighborsPtr = Int32ArrayType::CreateArray(m->getTotalPoints(), "Neighbors");
  neighborsPtr->initializeWithValue(-1);
  m_Neighbors = neighborsPtr->getPointer(0);

  QVector<int > n(totalFeatures + 1, 0);
  //uint64_t millis = QDateTime::currentMSecsSinceEpoch();
  //uint64_t currentMillis = millis;

  while (count != 0)
  {
    counter++;
    count = 0;
    fixedCount = 0;
    int zStride, yStride;
    for(int i = 0; i < zPoints; i++)
    {
      zStride = i * xPoints * yPoints;
      for (int j = 0; j < yPoints; j++)
      {
        yStride = j * xPoints;
        for(int k = 0; k < xPoints; k++)
        {
          featurename = m_FeatureIds[zStride + yStride + k];
          if (featurename < 0)
          {
            count++;
            current = 0;
            most = 0;
            for (int l = 0; l < 6; l++)
            {
              good = 1;
              neighpoint = zStride + yStride + k + neighpoints[l];
              if (l == 0 && i == 0) { good = 0; }
              if (l == 5 && i == (zPoints - 1)) { good = 0; }
              if (l == 1 && j == 0) { good = 0; }
              if (l == 4 && j == (yPoints - 1)) { good = 0; }
              if (l == 2 && k == 0) { good = 0; }
              if (l == 3 && k == (xPoints - 1)) { good = 0; }
              if (good == 1)
              {
                feature = m_FeatureIds[neighpoint];
                if (feature > 0)
                {
                  n[feature]++;
                  current = n[feature];
                  if (current > most)
                  {
                    most = current;
                    m_Neighbors[zStride + yStride + k] = neighpoint;
                  }
                }
              }
            }
            for (int l = 0; l < 6; l++)
            {
              good = 1;
              neighpoint = zStride + yStride + k + neighpoints[l];
              if (l == 0 && i == 0) { good = 0; }
              if (l == 5 && i == (zPoints - 1)) { good = 0; }
              if (l == 1 && j == 0) { good = 0; }
              if (l == 4 && j == (yPoints - 1)) { good = 0; }
              if (l == 2 && k == 0) { good = 0; }
              if (l == 3 && k == (xPoints - 1)) { good = 0; }
              if (good == 1)
              {
                feature = m_FeatureIds[neighpoint];
                if(feature > 0) { n[feature] = 0; }
              }
            }
          }
        }
      }
    }
    for (size_t j = 0; j < totalPoints; j++)
    {
      featurename = m_FeatureIds[j];
      neighbor = m_Neighbors[j];
      if (featurename < 0 && neighbor != -1 && m_FeatureIds[neighbor] > 0)
      {
        m_FeatureIds[j] = m_FeatureIds[neighbor];
        m_CellPhases[j] = m_FeaturePhases[m_FeatureIds[neighbor]];
        fixedCount++;
      }
    }
    if(counter >= 1)
    {
      QString ss = QObject::tr("Assign Gaps|| Cycle#: %1 || Remaining Unassigned Voxel Count: %2").arg(counter).arg(count);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer TesselateFarFieldGrains::newFilterInstance(bool copyFilterParameters)
{
  TesselateFarFieldGrains::Pointer filter = TesselateFarFieldGrains::New();
  if(true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters());
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TesselateFarFieldGrains::getCompiledLibraryName()
{ return SyntheticBuilding::SyntheticBuildingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TesselateFarFieldGrains::getGroupName()
{ return DREAM3D::FilterGroups::SyntheticBuildingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TesselateFarFieldGrains::getSubGroupName()
{ return DREAM3D::FilterSubGroups::PackingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TesselateFarFieldGrains::getHumanLabel()
{ return "Tesselate Far Field Grains"; }

