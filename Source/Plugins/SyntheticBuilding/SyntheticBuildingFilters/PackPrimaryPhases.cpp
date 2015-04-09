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
#include "DREAM3DLib/CoreFilters/DataContainerWriter.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/StatsData/PrimaryStatsData.h"
#include "SyntheticBuilding/ShapeOps/CubeOctohedronOps.h"
#include "SyntheticBuilding/ShapeOps/CylinderOps.h"
#include "SyntheticBuilding/ShapeOps/EllipsoidOps.h"
#include "SyntheticBuilding/ShapeOps/SuperEllipsoidOps.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "DREAM3DLib/Utilities/TimeUtilities.h"
#include "OrientationLib/Math/OrientationMath.h"

//// Macro to determine if we are going to show the Debugging Output files
#define PPP_SHOW_DEBUG_OUTPUTS 0

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
    int32_t* m_FeatureIds;
    float xc;
    float yc;
    float zc;
    ShapeOps* m_ShapeOps;
    float ga[3][3];
    int curFeature;
    Int32ArrayType::Pointer newownersPtr;
    FloatArrayType::Pointer ellipfuncsPtr;

  public:
    AssignVoxelsGapsImpl(DimType* dimensions, float* resolution, int32_t* featureIds, float* radCur,
                         float* xx, ShapeOps* shapeOps, float gA[3][3], float* size, int cur_feature,
                         Int32ArrayType::Pointer newowners, FloatArrayType::Pointer ellipfuncs) :
      m_FeatureIds(featureIds),
      m_ShapeOps(shapeOps),
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
    virtual ~AssignVoxelsGapsImpl() {}

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
      for(int i = 0; i < 3; i++)
      {
        for(int j = 0; j < 3; j++)
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
            inside = m_ShapeOps->inside(axis1comp, axis2comp, axis3comp);
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
PackPrimaryPhases::PackPrimaryPhases() :
  AbstractFilter(),
  m_OutputCellAttributeMatrixName(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, ""),
  m_OutputCellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_OutputCellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_NumFeaturesArrayName(DREAM3D::EnsembleData::NumFeatures),
  m_NeighborhoodsArrayName(DREAM3D::FeatureData::Neighborhoods),
  m_CentroidsArrayName(DREAM3D::FeatureData::Centroids),
  m_VolumesArrayName(DREAM3D::FeatureData::Volumes),
  m_AxisLengthsArrayName(DREAM3D::FeatureData::AxisLengths),
  m_AxisEulerAnglesArrayName(DREAM3D::FeatureData::AxisEulerAngles),
  m_Omega3sArrayName(DREAM3D::FeatureData::Omega3s),
  m_EquivalentDiametersArrayName(DREAM3D::FeatureData::EquivalentDiameters),
  m_InputStatsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::Statistics),
  m_InputPhaseTypesArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::PhaseTypes),
  m_InputShapeTypesArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::ShapeTypes),
  m_MaskArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::GoodVoxels),
  m_UseMask(false),
  m_HaveFeatures(false),
  m_FeatureInputFile(""),
  m_CsvOutputFile(""),
  m_PeriodicBoundaries(false),
  m_WriteGoalAttributes(false),
  m_ErrorOutputFile(""),
  m_VtkOutputFile(""),
  m_FeatureIds(NULL),
  m_CellPhases(NULL),
  m_MaskArrayName(DREAM3D::CellData::GoodVoxels),
  m_Mask(NULL),
  m_FeaturePhases(NULL),
  m_NumFeatures(NULL),
  m_Neighborhoods(NULL),
  m_Centroids(NULL),
  m_Volumes(NULL),
  m_AxisLengths(NULL),
  m_AxisEulerAngles(NULL),
  m_Omega3s(NULL),
  m_EquivalentDiameters(NULL),
  m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
  m_PhaseTypes(NULL),
  m_ShapeTypesArrayName(DREAM3D::EnsembleData::ShapeTypes),
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

  m_Seed = QDateTime::currentMSecsSinceEpoch();
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
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Periodic Boundaries", "PeriodicBoundaries", FilterParameterWidgetType::BooleanWidget, getPeriodicBoundaries(), false));
  QStringList linkedProps("MaskArrayPath");
  parameters.push_back(LinkedBooleanFilterParameter::New("Use Mask", "UseMask", getUseMask(), linkedProps, false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Mask Array", "MaskArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getMaskArrayPath(), true));
  parameters.push_back(FilterParameter::New("Statistics Array", "InputStatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getInputStatsArrayPath(), true));
  parameters.push_back(FilterParameter::New("Phase Types Array", "InputPhaseTypesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getInputPhaseTypesArrayPath(), true));
  parameters.push_back(FilterParameter::New("Shape Types Array", "InputShapeTypesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getInputShapeTypesArrayPath(), true));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Cell Attribute Matrix Name", "OutputCellAttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getOutputCellAttributeMatrixName(), true));
  parameters.push_back(FilterParameter::New("Cell Feature Attribute Matrix Name", "OutputCellFeatureAttributeMatrixName", FilterParameterWidgetType::StringWidget, getOutputCellFeatureAttributeMatrixName(), true));
  parameters.push_back(FilterParameter::New("Cell Ensemble Attribute Matrix Name", "OutputCellEnsembleAttributeMatrixName", FilterParameterWidgetType::StringWidget, getOutputCellEnsembleAttributeMatrixName(), true));
  parameters.push_back(FilterParameter::New("Feature Ids Array Name", "FeatureIdsArrayName", FilterParameterWidgetType::StringWidget, getFeatureIdsArrayName(), true));
  parameters.push_back(FilterParameter::New("Cell Phases Array Name", "CellPhasesArrayName", FilterParameterWidgetType::StringWidget, getCellPhasesArrayName(), true));
  parameters.push_back(FilterParameter::New("Feature Phases Array Name", "FeaturePhasesArrayName", FilterParameterWidgetType::StringWidget, getFeaturePhasesArrayName(), true));
  parameters.push_back(FilterParameter::New("Number of Features Array Name", "NumFeaturesArrayName", FilterParameterWidgetType::StringWidget, getNumFeaturesArrayName(), true));
  linkedProps.clear();
  linkedProps << "FeatureInputFile";
  parameters.push_back(LinkedBooleanFilterParameter::New("Already Have Features", "HaveFeatures", getHaveFeatures(), linkedProps, false));
  parameters.push_back(FileSystemFilterParameter::New("Feature Input File", "FeatureInputFile", FilterParameterWidgetType::InputFileWidget, getFeatureInputFile(), false, "", "*.txt", "Text File"));
  linkedProps.clear();
  linkedProps << "CsvOutputFile";
  parameters.push_back(LinkedBooleanFilterParameter::New("Write Goal Attributes", "WriteGoalAttributes", getWriteGoalAttributes(), linkedProps, true));
  parameters.push_back(FileSystemFilterParameter::New("Goal Attribute CSV File", "CsvOutputFile", FilterParameterWidgetType::OutputFileWidget, getCsvOutputFile(), false, "", "*.csv", "Comma Separated Data"));

#if PPP_SHOW_DEBUG_OUTPUTS
  parameters.push_back(FileSystemFilterParameter::New("Debug VTK File", "VtkOutputFile", FilterParameterWidgetType::InputFileWidget, getVtkOutputFile(), true, "", "*.vtk", "VTK File"));
  parameters.push_back(FileSystemFilterParameter::New("Debug Error File", "ErrorOutputFile", FilterParameterWidgetType::InputFileWidget, getErrorOutputFile(), false, "", "*.txt", "Text File"));
#endif

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOutputCellAttributeMatrixName( reader->readDataArrayPath("OutputCellAttributeMatrixName", getOutputCellAttributeMatrixName() ) );
  setOutputCellFeatureAttributeMatrixName( reader->readString("OutputCellFeatureAttributeMatrixName", getOutputCellFeatureAttributeMatrixName() ) );
  setOutputCellEnsembleAttributeMatrixName( reader->readString("OutputCellEnsembleAttributeMatrixName", getOutputCellEnsembleAttributeMatrixName() ) );
  setFeatureIdsArrayName( reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName() ) );
  setCellPhasesArrayName( reader->readString("CellPhasesArrayName", getCellPhasesArrayName() ) );
  setFeaturePhasesArrayName( reader->readString("FeaturePhasesArrayName", getFeaturePhasesArrayName() ) );
  setNumFeaturesArrayName( reader->readString("NumFeaturesArrayName", getNumFeaturesArrayName() ) );
  setPeriodicBoundaries( reader->readValue("PeriodicBoundaries", false) );
  setWriteGoalAttributes( reader->readValue("WriteGoalAttributes", false) );
  setUseMask( reader->readValue("UseMask", getUseMask()) );
  setHaveFeatures( reader->readValue("HaveFeatures", getHaveFeatures()) );
  setFeatureInputFile( reader->readString( "FeatureInputFile", getFeatureInputFile() ) );
  setCsvOutputFile( reader->readString( "CsvOutputFile", getCsvOutputFile() ) );
  setInputStatsArrayPath(reader->readDataArrayPath("InputStatsArrayPath", getInputStatsArrayPath() ) );
  setInputPhaseTypesArrayPath(reader->readDataArrayPath("InputPhaseTypesArrayPath", getInputPhaseTypesArrayPath() ) );
  setInputShapeTypesArrayPath(reader->readDataArrayPath("InputShapeTypesArrayPath", getInputShapeTypesArrayPath() ) );
  setMaskArrayPath(reader->readDataArrayPath("MaskArrayPath", getMaskArrayPath() ) );
  setVtkOutputFile( reader->readString( "VtkOutputFile", getVtkOutputFile() ) );
  setErrorOutputFile( reader->readString( "ErrorOutputFile", getErrorOutputFile() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PackPrimaryPhases::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputCellAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputCellFeatureAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputCellEnsembleAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellPhasesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(FeaturePhasesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(NumFeaturesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(PeriodicBoundaries)
  DREAM3D_FILTER_WRITE_PARAMETER(UseMask)
  DREAM3D_FILTER_WRITE_PARAMETER(HaveFeatures)
  DREAM3D_FILTER_WRITE_PARAMETER(WriteGoalAttributes)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureInputFile)
  DREAM3D_FILTER_WRITE_PARAMETER(CsvOutputFile)
  DREAM3D_FILTER_WRITE_PARAMETER(InputStatsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(InputPhaseTypesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(InputShapeTypesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MaskArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(VtkOutputFile)
  DREAM3D_FILTER_WRITE_PARAMETER(ErrorOutputFile)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::updateFeatureInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_NeighborhoodsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Neighborhoods = m_NeighborhoodsPtr.lock()->getPointer(0); }
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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);
  // This is for convenience

  // Make sure we have our input DataContainer with the proper Ensemble data
  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getOutputCellAttributeMatrixName().getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m.get()) { return; }

  ImageGeom::Pointer image = m->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0 || NULL == image.get()) { return; }

  //Input Ensemble Data That we require
  QVector<size_t> dims(1, 1);
  m_PhaseTypesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getInputPhaseTypesArrayPath(), dims);
  if( NULL != m_PhaseTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_ShapeTypesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getInputShapeTypesArrayPath(), dims);
  if( NULL != m_ShapeTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ShapeTypes = m_ShapeTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_StatsDataArray = getDataContainerArray()->getPrereqArrayFromPath<StatsDataArray, AbstractFilter>(this, getInputStatsArrayPath(), dims);
  if(m_StatsDataArray.lock() == NULL)
  {
    QString ss = QObject::tr("Stats Array Not Initialized correctly");
    setErrorCondition(-308);
    notifyErrorMessage(getHumanLabel(), ss, -308);
  }

  if(m_UseMask == true)
  {
    m_MaskPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getMaskArrayPath(), dims);
    if( NULL != m_MaskPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_Mask = m_MaskPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  dims[0] = 1;
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
  tDims[0] = m_PhaseTypesPtr.lock()->getNumberOfTuples();
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getOutputCellEnsembleAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellEnsemble);
  if(getErrorCondition() < 0) { return; }

  //Feature Data
  tempPath.update(getOutputCellAttributeMatrixName().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), getFeaturePhasesArrayName() );
  m_FeaturePhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getOutputCellAttributeMatrixName().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), getNeighborhoodsArrayName() );
  m_NeighborhoodsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, dims);
  if( NULL != m_NeighborhoodsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Neighborhoods = m_NeighborhoodsPtr.lock()->getPointer(0); }
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
  //Ensemble Data
  dims[0] = 1;
  tempPath.update(getOutputCellAttributeMatrixName().getDataContainerName(), getOutputCellEnsembleAttributeMatrixName(), getNumFeaturesArrayName() );
  m_NumFeaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NumFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NumFeatures = m_NumFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();

  if (m_WriteGoalAttributes == true && getCsvOutputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Csv Output File Set and it was not.").arg(ClassName());
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-387);
  }

  if(m_HaveFeatures == true && getFeatureInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr(": The Feature file must be set before executing this filter.");
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
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
  attrMat->removeAttributeArray(m_NeighborhoodsArrayName);
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::execute()
{
  int err = 0;
  setErrorCondition(err);
  DREAM3D_RANDOMNG_NEW()
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  if(m_HaveFeatures == false)
  {
    notifyStatusMessage(getHumanLabel(), "Packing Features - Initializing Volume");
    // this initializes the arrays to hold the details of the locations of all of the features during packing
    Int32ArrayType::Pointer featureOwnersPtr = initialize_packinggrid();

    notifyStatusMessage(getHumanLabel(), "Packing Features - Placing Features");
    place_features(featureOwnersPtr);
  }

  if(m_HaveFeatures == true)
  {
    notifyStatusMessage(getHumanLabel(), "Loading Features");
    load_features();
    if (getCancel() == true) { return; }
  }

  notifyStatusMessage(getHumanLabel(), "Packing Features - Assigning Voxels");
  assign_voxels();
  if (getCancel() == true) { return; }

  notifyStatusMessage(getHumanLabel(), "Packing Features - Assigning Gaps");
  assign_gaps_only();
  if (getCancel() == true) { return; }

  notifyStatusMessage(getHumanLabel(), "Packing Features - Cleaning Up Volume");
  //  cleanup_features();
  if (getCancel() == true) { return; }

  if(m_WriteGoalAttributes == true)
  {
    write_goal_attributes();
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixName().getDataContainerName());
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName);
  cellFeatureAttrMat->removeAttributeArray(m_EquivalentDiametersArrayName);
  cellFeatureAttrMat->removeAttributeArray(m_Omega3sArrayName);
  cellFeatureAttrMat->removeAttributeArray(m_AxisEulerAnglesArrayName);
  cellFeatureAttrMat->removeAttributeArray(m_AxisLengthsArrayName);
  cellFeatureAttrMat->removeAttributeArray(m_VolumesArrayName);
  cellFeatureAttrMat->removeAttributeArray(m_CentroidsArrayName);
  cellFeatureAttrMat->removeAttributeArray(m_NeighborhoodsArrayName);

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Packing Features Complete");

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PackPrimaryPhases::writeVtkFile(int32_t* featureOwners, int32_t* exclusionZones)
{
  size_t featureOwnersIdx = 0;
  std::ofstream outFile;
  outFile.open(m_VtkOutputFile.toLatin1().data(), std::ios_base::binary);
  if(outFile.is_open() == false)
  {
    qDebug() << "m_VtkOutputFile: " << m_VtkOutputFile << "\n";
    notifyErrorMessage(getHumanLabel(), "Could not open Vtk File for writing from PackFeatures", -1);
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
        featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * i) + (m_PackingPoints[0] * j) + k;
        int name = featureOwners[featureOwnersIdx];
        if(i % 20 == 0 && i > 0) { outFile << "\n"; }
        outFile << "     ";
        if(name < 100) { outFile << " "; }
        if(name < 10) { outFile << " "; }
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
        featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * i) + (m_PackingPoints[0] * j) + k;
        int32_t val = exclusionZones[featureOwnersIdx];
        if(i % 20 == 0 && i > 0) { outFile << "\n"; }
        outFile << "       ";
        outFile << val;
      }
    }
  }
  outFile.close();
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void  PackPrimaryPhases::load_features()
{

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixName().getDataContainerName());
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName);

  std::ifstream inFile;
  inFile.open(getFeatureInputFile().toLatin1().data(), std::ios_base::binary);
  if(!inFile)
  {
    QString ss = QObject::tr("Failed to open: %1").arg(getFeatureInputFile());
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, -1);
  }
  int numFeatures;
  inFile >> numFeatures;
  if (0 == numFeatures)
  {
    notifyErrorMessage(getHumanLabel(), "The number of features is Zero and should be greater than Zero", -600);
  }

  firstPrimaryFeature = 1;

  QVector<size_t> tDims(1, firstPrimaryFeature + numFeatures);
  cellFeatureAttrMat->setTupleDimensions(tDims);
  updateFeatureInstancePointers();

  int phase;
  float xC, yC, zC;
  float axisA, axisB, axisC;
  float vol, eqDiam;
  float omega3;
  float phi1, PHI, phi2;
  size_t currentFeature = firstPrimaryFeature;
  const float fourThirds = 4.0f / 3.0f;
  for(int i = 0; i < numFeatures; i++)
  {
    inFile >> phase >> xC >> yC >> zC >> axisA >> axisB >> axisC >> omega3 >> phi1 >> PHI >> phi2;
    vol = fourThirds * DREAM3D::Constants::k_Pi * axisA * axisB * axisC;
    eqDiam = 2.0f * powf((vol * (0.75f) * (DREAM3D::Constants::k_1OverPi)), (DREAM3D::Constants::k_1Over3));
    m_Centroids[3 * currentFeature + 0] = xC;
    m_Centroids[3 * currentFeature + 1] = yC;
    m_Centroids[3 * currentFeature + 2] = zC;
    m_Volumes[currentFeature] = vol;
    m_EquivalentDiameters[currentFeature] = eqDiam;
    m_AxisLengths[3 * currentFeature + 0] = axisA / axisA;
    m_AxisLengths[3 * currentFeature + 1] = axisB / axisA;
    m_AxisLengths[3 * currentFeature + 2] = axisC / axisA;
    m_AxisEulerAngles[3 * currentFeature + 0] = phi1;
    m_AxisEulerAngles[3 * currentFeature + 1] = PHI;
    m_AxisEulerAngles[3 * currentFeature + 2] = phi2;
    m_Omega3s[currentFeature] = omega3;
    m_FeaturePhases[currentFeature] = phase;
    currentFeature++;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::place_features(Int32ArrayType::Pointer featureOwnersPtr)
{
  bool writeErrorFile = false;
  std::ofstream outFile;
  if(m_ErrorOutputFile.isEmpty() == false)
  {
    outFile.open(m_ErrorOutputFile.toLatin1().data(), std::ios_base::binary);
    writeErrorFile = outFile.is_open();
  }

  int err = 0;
  setErrorCondition(err);
  m_Seed = QDateTime::currentMSecsSinceEpoch();
  DREAM3D_RANDOMNG_NEW_SEEDED(m_Seed);

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixName().getDataContainerName());

  size_t totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  // Get the number of input ensembles from one of the input arrays that are located in the Input Ensemble AttributeMatrix
  size_t totalEnsembles = m_PhaseTypesPtr.lock()->getNumberOfTuples();

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock().get());

  size_t udims[3] =
  { 0, 0, 0 };
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

  float xRes = m->getGeometryAs<ImageGeom>()->getXRes();
  float yRes = m->getGeometryAs<ImageGeom>()->getYRes();
  float zRes = m->getGeometryAs<ImageGeom>()->getZRes();
  sizex = dims[0] * m->getGeometryAs<ImageGeom>()->getXRes();
  sizey = dims[1] * m->getGeometryAs<ImageGeom>()->getYRes();
  sizez = dims[2] * m->getGeometryAs<ImageGeom>()->getZRes();
  totalvol = sizex * sizey * sizez;

  double totalprimaryvolTEMP = 0;
  //  float badcount = 0;
  //  size_t check = 0;
  size_t totalVox = static_cast<size_t>(dims[0] * dims[1] * dims[2]);
  for (size_t i = 0; i < totalVox; i++)
  {
    if(m_FeatureIds[i] <= 0) { totalprimaryvolTEMP++; }
  }
  float totalprimaryvol = static_cast<float>(totalprimaryvolTEMP);
  totalprimaryvol = totalprimaryvol * (m->getGeometryAs<ImageGeom>()->getXRes() * m->getGeometryAs<ImageGeom>()->getYRes() * m->getGeometryAs<ImageGeom>()->getZRes());

  // float change1, change2;
  float change = 0.0f;
  int phase = 0;
  int randomfeature;
  //   float random = 0.0f;
  //  int newfeature;
  // float check;
  float xc, yc, zc;
  float oldxc, oldyc, oldzc;
  oldfillingerror = 0;
  currentneighborhooderror = 0, oldneighborhooderror = 0;
  currentsizedisterror = 0, oldsizedisterror = 0;
  int acceptedmoves = 0;
  float totalprimaryfractions = 0.0;
  // find which phases are primary phases
  for (size_t i = 1; i < totalEnsembles; ++i)
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
        notifyErrorMessage(getHumanLabel(), ss, -666);
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

  QVector<size_t> dim(1, 1);
  Int32ArrayType::Pointer exclusionOwnersPtr = Int32ArrayType::CreateArray(featureOwnersPtr->getNumberOfTuples(), dim, "PackPrimaryFeatures::exclusions_owners");
  exclusionOwnersPtr->initializeWithValue(0);

  //This is the set that we are going to keep updated with the points that are not in an exclusion zone
  std::map<size_t, size_t> availablePoints;
  std::map<size_t, size_t> availablePointsInv;

  // Get a pointer to the Feature Owners that was just initialized in the initialize_packinggrid() method
  int32_t* featureOwners = featureOwnersPtr->getPointer(0);
  int32_t* exclusionOwners = exclusionOwnersPtr->getPointer(0);
  size_t featureOwnersIdx = 0;

  // initialize the sim and goal size distributions for the primary phases
  featuresizedist.resize(primaryphases.size());
  simfeaturesizedist.resize(primaryphases.size());
  featuresizediststep.resize(primaryphases.size());
  for (size_t i = 0; i < primaryphases.size(); i++)
  {
    phase = primaryphases[i];
    PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
    featuresizedist[i].resize(40);
    simfeaturesizedist[i].resize(40);
    featuresizediststep[i] = static_cast<float>(((2 * pp->getMaxFeatureDiameter()) - (pp->getMinFeatureDiameter() / 2.0)) / featuresizedist[i].size());
    float input = 0;
    float previoustotal = 0;
    VectorOfFloatArray GSdist = pp->getFeatureSizeDistribution();
    float avg = GSdist[0]->getValue(0);
    float stdev = GSdist[1]->getValue(0);
    float denominatorConst = 1.0 / sqrtf(2.0f * stdev * stdev); // Calculate it here rather than calculating the same thing multiple times below
    for (size_t j = 0; j < featuresizedist[i].size(); j++)
    {
      input = (float(j + 1) * featuresizediststep[i]) + (pp->getMinFeatureDiameter() / 2.0f);
      float logInput = logf(input);
      if(logInput <= avg)
      {
        featuresizedist[i][j] = 0.5f - 0.5f * (DREAM3DMath::erf((avg - logInput) * denominatorConst)) - previoustotal;
      }
      if(logInput > avg)
      {
        featuresizedist[i][j] = 0.5f + 0.5f * (DREAM3DMath::erf((logInput - avg) * denominatorConst)) - previoustotal;
      }
      previoustotal = previoustotal + featuresizedist[i][j];
    }
  }

  if (getCancel() == true)
  {
    std::cout << "PackPrimaryPhases: " << this << std::endl;
    QString ss = QObject::tr("Filter Cancelled.");
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
    return;
  }

  // generate the features and monitor the size distribution error while doing so. After features are generated, no new features can enter or leave the structure.
  Feature feature;

  // Estimate the total Number of features here
  int estNumFeatures = estimate_numfeatures((int)(udims[0]), (int)(udims[1]), (int)(udims[2]), xRes, yRes, zRes);
  QVector<size_t> tDims(1, estNumFeatures);
  m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->resizeAttributeArrays(tDims);
  //need to update pointers after resize, buut do not need to run full data check because pointers are still valid
  updateFeatureInstancePointers();

  int gid = 1;
  firstPrimaryFeature = gid;
  std::vector<float> curphasevol;
  curphasevol.resize(primaryphases.size());
  float factor = 1.0;
  size_t iter = 0;
  for (size_t j = 0; j < primaryphases.size(); ++j)
  {
    curphasevol[j] = 0;
    float curphasetotalvol = totalprimaryvol * primaryphasefractions[j];
    while (curphasevol[j] < (factor * curphasetotalvol))
    {
      iter++;
      m_Seed++;
      phase = primaryphases[j];
      generate_feature(phase, static_cast<int>(m_Seed), &feature, m_ShapeTypes[phase]);
      currentsizedisterror = check_sizedisterror(&feature);
      change = (currentsizedisterror) - (oldsizedisterror);
      if(change > 0 || currentsizedisterror > (1.0 - (float(iter) * 0.001)) || curphasevol[j] < (0.75 * factor * curphasetotalvol))
      {
        QString ss = QObject::tr("Packing Features (1/2) - Generating Feature #%1").arg(gid);
        notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
        if (gid + 1 >= static_cast<int>(m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumTuples()))
        {
          tDims[0] = gid + 1;
          m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->resizeAttributeArrays(tDims);
          //need to update pointers after resize, buut do not need to run full data check because pointers are still valid
          updateFeatureInstancePointers();
        }

        transfer_attributes(gid, &feature);
        oldsizedisterror = currentsizedisterror;
        curphasevol[j] = curphasevol[j] + m_Volumes[gid];
        iter = 0;
        gid++;
      }
      if (getCancel() == true)
      {
        QString ss = QObject::tr("Filter Cancelled.");
        notifyErrorMessage(getHumanLabel(), ss, -1);
        setErrorCondition(-1);
        return;
      }
    }
  }

  if(m_PeriodicBoundaries == false)
  {
    iter = 0;
    int xfeatures, yfeatures, zfeatures;
    xfeatures = int(powf((m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumTuples() * (sizex / sizey) * (sizex / sizez)), (1.0f / 3.0f)) + 1);
    yfeatures = int(xfeatures * (sizey / sizex) + 1);
    zfeatures = int(xfeatures * (sizez / sizex) + 1);
    factor = 0.25f * (1.0f - (float((xfeatures - 2) * (yfeatures - 2) * (zfeatures - 2)) / float(xfeatures * yfeatures * zfeatures)));
    for (size_t j = 0; j < primaryphases.size(); ++j)
    {
      float curphasetotalvol = totalprimaryvol * primaryphasefractions[j];
      while (curphasevol[j] < ((1 + factor) * curphasetotalvol))
      {
        iter++;
        m_Seed++;
        phase = primaryphases[j];
        generate_feature(phase, static_cast<int>(m_Seed), &feature, m_ShapeTypes[phase]);
        currentsizedisterror = check_sizedisterror(&feature);
        change = (currentsizedisterror) - (oldsizedisterror);
        if(change > 0 || currentsizedisterror > (1.0 - (iter * 0.001)) || curphasevol[j] < (0.75 * factor * curphasetotalvol))
        {
          QString ss = QObject::tr("Packing Features (2/2) - Generating Feature #%1").arg(gid);
          notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
          if (gid + 1 >= static_cast<int>(m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumTuples()) )
          {
            tDims[0] = gid + 1;
            m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->resizeAttributeArrays(tDims);
            //need to update pointers after resize, buut do not need to run full data check because pointers are still valid
            updateFeatureInstancePointers();
          }
          transfer_attributes(gid, &feature);
          oldsizedisterror = currentsizedisterror;
          curphasevol[j] = curphasevol[j] + m_Volumes[gid];
          iter = 0;
          gid++;
        }
        if (getCancel() == true)
        {
          QString ss = QObject::tr("Filter Cancelled.");
          notifyErrorMessage(getHumanLabel(), ss, -1);
          setErrorCondition(-1);
          return;
        }
      }
    }
  }

  tDims[0] = gid;
  m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->resizeAttributeArrays(tDims);
  totalFeatures = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumTuples();
  //need to update pointers after resize, buut do not need to run full data check because pointers are still valid
  updateFeatureInstancePointers();

  if (getCancel() == true)
  {
    QString ss = QObject::tr("Filter Cancelled.");
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Initializing Neighbor Distributions");

  // initialize the sim and goal neighbor distribution for the primary phases
  neighbordist.resize(primaryphases.size());
  simneighbordist.resize(primaryphases.size());
  neighbordiststep.resize(primaryphases.size());
  for (size_t i = 0; i < primaryphases.size(); i++)
  {
    phase = primaryphases[i];
    PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
    neighbordist[i].resize(pp->getBinNumbers()->getSize());
    simneighbordist[i].resize(pp->getBinNumbers()->getSize());
    VectorOfFloatArray Neighdist = pp->getFeatureSize_Neighbors();
    float normalizer = 0;
    for (size_t j = 0; j < neighbordist[i].size(); j++)
    {
      neighbordist[i][j].resize(40);
      float input = 0;
      float previoustotal = 0;
      float avg = Neighdist[0]->getValue(j);
      float stdev = Neighdist[1]->getValue(j);
      neighbordiststep[i] = 2;
      float denominatorConst = 1.0 / sqrtf(2.0f * stdev * stdev); // Calculate it here rather than calculating the same thing multiple times below
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
    normalizer = 1.0 / normalizer;
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
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
    return;
  }

  //  for each feature : select centroid, determine voxels in feature, monitor filling error and decide of the 10 placements which
  // is the most beneficial, then the feature is added and its neighbors are determined

  columnlist.resize(totalFeatures);
  rowlist.resize(totalFeatures);
  planelist.resize(totalFeatures);
  ellipfunclist.resize(totalFeatures);
  packqualities.resize(totalFeatures);
  fillingerror = 1;

  int count = 0;
  int column, row, plane;
  int progFeature = 0;
  int progFeatureInc = totalFeatures * .01;
  for (size_t i = firstPrimaryFeature; i < totalFeatures; i++)
  {
    if ((int)i > progFeature + progFeatureInc)
    {

      QString ss = QObject::tr("Placing Feature #%1/%2").arg(i).arg(totalFeatures);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      progFeature = i;
    }

    //we always put the feature in the center of the box to make sure the feature has the optimal chance to not touch the edge of the box
    //this is because in the insert function below we are just determining which voxels relative to the centroid are in the feature
    xc = sizex * 0.5f;
    yc = sizey * 0.5f;
    zc = sizez * 0.5f;
    m_Centroids[3 * i] = xc;
    m_Centroids[3 * i + 1] = yc;
    m_Centroids[3 * i + 2] = zc;
    insert_feature(i);
    count = 0;
    //now we randomly pick a place to try to place the feature
    xc = static_cast<float>(rg.genrand_res53() * sizex);
    yc = static_cast<float>(rg.genrand_res53() * sizey);
    zc = static_cast<float>(rg.genrand_res53() * sizez);
    column = static_cast<int>( (xc - (m_HalfPackingRes[0])) * m_OneOverPackingRes[0] );
    row = static_cast<int>( (yc - (m_HalfPackingRes[1])) * m_OneOverPackingRes[1] );
    plane = static_cast<int>( (zc - (m_HalfPackingRes[2])) * m_OneOverPackingRes[2] );
    featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + column;
    //now we walk til we find a point that is not in an exclusion zone
    while(exclusionOwners[featureOwnersIdx] > 0 && count < m_TotalPackingPoints)
    {
      featureOwnersIdx++;
      if(featureOwnersIdx >= m_TotalPackingPoints) { featureOwnersIdx = 0; }
      count++;
    }
    column = featureOwnersIdx % m_PackingPoints[0];
    row = int(featureOwnersIdx / m_PackingPoints[0]) % m_PackingPoints[1];
    plane = featureOwnersIdx / (m_PackingPoints[0] * m_PackingPoints[1]);
    xc = static_cast<float>((column * m_PackingRes[0]) + (m_PackingRes[0] * 0.5));
    yc = static_cast<float>((row * m_PackingRes[1]) + (m_PackingRes[1] * 0.5));
    zc = static_cast<float>((plane * m_PackingRes[2]) + (m_PackingRes[2] * 0.5));
    move_feature(i, xc, yc, zc);
    fillingerror = check_fillingerror(i, -1000, featureOwnersPtr, exclusionOwnersPtr);

    if (getCancel() == true)
    {
      QString ss = QObject::tr("Filter Cancelled.");
      notifyErrorMessage(getHumanLabel(), ss, -1);
      setErrorCondition(-1);
      return;
    }
  }

  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Determining Neighbors");
  progFeature = 0;
  progFeatureInc = totalFeatures * .01;
  uint64_t millis = QDateTime::currentMSecsSinceEpoch();
  uint64_t currentMillis = millis;
  uint64_t startMillis = millis;
  uint64_t estimatedTime = 0;
  float timeDiff = 0.0f;

  // determine neighborhoods and initial neighbor distribution errors
  for (size_t i = firstPrimaryFeature; i < totalFeatures; i++)
  {
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if (currentMillis - millis > 1000)
    {
      QString ss = QObject::tr("Determining Neighbors %1/%2").arg(i).arg(totalFeatures);
      timeDiff = ((float)i / (float)(currentMillis - startMillis));
      estimatedTime = (float)(totalFeatures - i) / timeDiff;
      ss = QObject::tr(" Est. Time Remain: %1").arg(DREAM3D::convertMillisToHrsMinSecs(estimatedTime));
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

      millis = QDateTime::currentMSecsSinceEpoch();
    }
    determine_neighbors(i, 1);
  }
  oldneighborhooderror = check_neighborhooderror(-1000, -1000);
  // begin swaping/moving/adding/removing features to try to improve packing
  int totalAdjustments = static_cast<int>(100 * (totalFeatures - 1));

  //determine initial set of available points
  availablePointsCount = 0;
  for(int i = 0; i < m_TotalPackingPoints; i++)
  {
    if(exclusionOwners[i] == 0)
    {
      availablePoints[i] = availablePointsCount;
      availablePointsInv[availablePointsCount] = i;
      availablePointsCount++;
    }
  }
  // and clear the pointsToRemove and pointsToAdd vectors from the initial packing
  pointsToRemove.clear();
  pointsToAdd.clear();

  millis = QDateTime::currentMSecsSinceEpoch();
  startMillis = millis;
  bool good;
  size_t key;
  float xshift, yshift, zshift;
  int lastIteration = 0;
  for (int iteration = 0; iteration < totalAdjustments; ++iteration)
  {
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if (currentMillis - millis > 5000)
    {
      QString ss = QObject::tr("Swapping/Moving/Adding/Removing Features Iteration %1/%2").arg(iteration).arg(totalAdjustments);
      timeDiff = ((float)iteration / (float)(currentMillis - startMillis));
      estimatedTime = (float)(totalAdjustments - iteration) / timeDiff;

      ss = QObject::tr(" || Est. Time Remain: %1 || Iterations/Sec: %2").arg(DREAM3D::convertMillisToHrsMinSecs(estimatedTime)).arg(timeDiff * 1000);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

      millis = QDateTime::currentMSecsSinceEpoch();
      lastIteration = iteration;
    }


    if (getCancel() == true)
    {
      QString ss = QObject::tr("Filter Cancelled.");
      notifyErrorMessage(getHumanLabel(), ss, -1);
      setErrorCondition(-1);
      return;
    }

    int option = iteration % 2;

    if(writeErrorFile == true && iteration % 25 == 0)
    {
      outFile << iteration << " " << fillingerror << "  " << availablePoints.size() << "  " << availablePointsCount << " " << totalFeatures << " " << acceptedmoves << "\n";
    }

    // JUMP - this option moves one feature to a random spot in the volume
    if(option == 0)
    {
      randomfeature = firstPrimaryFeature + int(rg.genrand_res53() * (totalFeatures - firstPrimaryFeature));
      good = false;
      count = 0;
      while(good == false && count < static_cast<int>((totalFeatures - firstPrimaryFeature)) )
      {
        xc = m_Centroids[3 * randomfeature];
        yc = m_Centroids[3 * randomfeature + 1];
        zc = m_Centroids[3 * randomfeature + 2];
        column = static_cast<int>( (xc - (m_HalfPackingRes[0])) * m_OneOverPackingRes[0] );
        row = static_cast<int>( (yc - (m_HalfPackingRes[1])) * m_OneOverPackingRes[1] );
        plane = static_cast<int>( (zc - (m_HalfPackingRes[2])) * m_OneOverPackingRes[2] );
        featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + column;
        if(featureOwners[featureOwnersIdx] > 1) { good = true; }
        else { randomfeature++; }
        if(static_cast<size_t>(randomfeature) >= totalFeatures) { randomfeature = firstPrimaryFeature; }
        count++;
      }
      m_Seed++;

      if(availablePoints.size() > 0)
      {
        key = static_cast<size_t>(rg.genrand_res53() * (availablePointsCount - 1));
        featureOwnersIdx = availablePointsInv[key];
      }
      else
      {
        featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPackingPoints);
      }

      //find the column row and plane of that point
      column = featureOwnersIdx % m_PackingPoints[0];
      row = int(featureOwnersIdx / m_PackingPoints[0]) % m_PackingPoints[1];
      plane = featureOwnersIdx / (m_PackingPoints[0] * m_PackingPoints[1]);
      xc = static_cast<float>((column * m_PackingRes[0]) + (m_PackingRes[0] * 0.5));
      yc = static_cast<float>((row * m_PackingRes[1]) + (m_PackingRes[1] * 0.5));
      zc = static_cast<float>((plane * m_PackingRes[2]) + (m_PackingRes[2] * 0.5));
      oldxc = m_Centroids[3 * randomfeature];
      oldyc = m_Centroids[3 * randomfeature + 1];
      oldzc = m_Centroids[3 * randomfeature + 2];
      oldfillingerror = fillingerror;
      fillingerror = check_fillingerror(-1000, static_cast<int>(randomfeature), featureOwnersPtr, exclusionOwnersPtr);
      move_feature(randomfeature, xc, yc, zc);
      fillingerror = check_fillingerror(static_cast<int>(randomfeature), -1000, featureOwnersPtr, exclusionOwnersPtr);
      currentneighborhooderror = check_neighborhooderror(-1000, randomfeature);
      if(fillingerror <= oldfillingerror)
      {
        oldneighborhooderror = currentneighborhooderror;
        update_availablepoints(availablePoints, availablePointsInv);
        acceptedmoves++;
      }
      else if(fillingerror > oldfillingerror)
      {
        fillingerror = check_fillingerror(-1000, static_cast<int>(randomfeature), featureOwnersPtr, exclusionOwnersPtr);
        move_feature(randomfeature, oldxc, oldyc, oldzc);
        fillingerror = check_fillingerror(static_cast<int>(randomfeature), -1000, featureOwnersPtr, exclusionOwnersPtr);
        pointsToRemove.clear();
        pointsToAdd.clear();
      }
    }
    // NUDGE - this option moves one feature to a spot close to its current centroid
    if(option == 1)
    {
      randomfeature = firstPrimaryFeature + int(rg.genrand_res53() * (totalFeatures - firstPrimaryFeature));
      good = false;
      count = 0;
      while(good == false && count < static_cast<int>((totalFeatures - firstPrimaryFeature)) )
      {
        xc = m_Centroids[3 * randomfeature];
        yc = m_Centroids[3 * randomfeature + 1];
        zc = m_Centroids[3 * randomfeature + 2];
        column = static_cast<int>( (xc - (m_HalfPackingRes[0])) * m_OneOverPackingRes[0] );
        row = static_cast<int>( (yc - (m_HalfPackingRes[1])) * m_OneOverPackingRes[1] );
        plane = static_cast<int>( (zc - (m_HalfPackingRes[2])) * m_OneOverPackingRes[2] );
        featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + column;
        if(featureOwners[featureOwnersIdx] > 1) { good = true; }
        else { randomfeature++; }
        if(static_cast<size_t>(randomfeature) >= totalFeatures) { randomfeature = firstPrimaryFeature; }
        count++;
      }
      m_Seed++;
      oldxc = m_Centroids[3 * randomfeature];
      oldyc = m_Centroids[3 * randomfeature + 1];
      oldzc = m_Centroids[3 * randomfeature + 2];
      xshift = static_cast<float>(((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * m_PackingRes[0])) );
      yshift = static_cast<float>(((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * m_PackingRes[1])) );
      zshift = static_cast<float>(((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * m_PackingRes[2])) );
      if((oldxc + xshift) < sizex && (oldxc + xshift) > 0) { xc = oldxc + xshift; }
      else { xc = oldxc; }
      if((oldyc + yshift) < sizey && (oldyc + yshift) > 0) { yc = oldyc + yshift; }
      else { yc = oldyc; }
      if((oldzc + zshift) < sizez && (oldzc + zshift) > 0) { zc = oldzc + zshift; }
      else { zc = oldzc; }
      oldfillingerror = fillingerror;
      fillingerror = check_fillingerror(-1000, static_cast<int>(randomfeature), featureOwnersPtr, exclusionOwnersPtr);
      move_feature(randomfeature, xc, yc, zc);
      fillingerror = check_fillingerror(static_cast<int>(randomfeature), -1000, featureOwnersPtr, exclusionOwnersPtr);
      currentneighborhooderror = check_neighborhooderror(-1000, randomfeature);
      //      change2 = (currentneighborhooderror * currentneighborhooderror) - (oldneighborhooderror * oldneighborhooderror);
      //      if(fillingerror <= oldfillingerror && currentneighborhooderror >= oldneighborhooderror)
      if(fillingerror <= oldfillingerror)
      {
        oldneighborhooderror = currentneighborhooderror;
        update_availablepoints(availablePoints, availablePointsInv);
        acceptedmoves++;
      }
      //      else if(fillingerror > oldfillingerror || currentneighborhooderror < oldneighborhooderror)
      else if(fillingerror > oldfillingerror)
      {
        fillingerror = check_fillingerror(-1000, static_cast<int>(randomfeature), featureOwnersPtr, exclusionOwnersPtr);
        move_feature(randomfeature, oldxc, oldyc, oldzc);
        fillingerror = check_fillingerror(static_cast<int>(randomfeature), -1000, featureOwnersPtr, exclusionOwnersPtr);
        pointsToRemove.clear();
        pointsToAdd.clear();
      }
    }
  }

  notifyStatusMessage(getHumanLabel(), "Packing Features - Feature Adjustment Complete");

  if(m_VtkOutputFile.isEmpty() == false)
  {
    err = writeVtkFile(featureOwnersPtr->getPointer(0), exclusionOwnersPtr->getPointer(0));
    if(err < 0)
    {
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Int32ArrayType::Pointer PackPrimaryPhases::initialize_packinggrid()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixName().getDataContainerName());

  m_PackingRes[0] = m->getGeometryAs<ImageGeom>()->getXRes() * 2.0f;
  m_PackingRes[1] = m->getGeometryAs<ImageGeom>()->getYRes() * 2.0f;
  m_PackingRes[2] = m->getGeometryAs<ImageGeom>()->getZRes() * 2.0f;

  m_HalfPackingRes[0] = m_PackingRes[0] * 0.5;
  m_HalfPackingRes[1] = m_PackingRes[1] * 0.5;
  m_HalfPackingRes[2] = m_PackingRes[2] * 0.5;

  m_OneOverHalfPackingRes[0] = 1.0f / m_HalfPackingRes[0];
  m_OneOverHalfPackingRes[1] = 1.0f / m_HalfPackingRes[1];
  m_OneOverHalfPackingRes[2] = 1.0f / m_HalfPackingRes[2];

  m_OneOverPackingRes[0] = 1.0f / m_PackingRes[0];
  m_OneOverPackingRes[1] = 1.0f / m_PackingRes[1];
  m_OneOverPackingRes[2] = 1.0f / m_PackingRes[2];

  m_PackingPoints[0] = m->getGeometryAs<ImageGeom>()->getXPoints() / 2;
  m_PackingPoints[1] = m->getGeometryAs<ImageGeom>()->getYPoints() / 2;
  m_PackingPoints[2] = m->getGeometryAs<ImageGeom>()->getZPoints() / 2;

  m_TotalPackingPoints = m_PackingPoints[0] * m_PackingPoints[1] * m_PackingPoints[2];

  Int32ArrayType::Pointer featureOwnersPtr = Int32ArrayType::CreateArray(m_TotalPackingPoints, "PackPrimaryFeatures::feature_owners");
  featureOwnersPtr->initializeWithZeros();

  return featureOwnersPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::generate_feature(int phase, int Seed, Feature* feature, unsigned int shapeclass)
{
  DREAM3D_RANDOMNG_NEW_SEEDED(Seed)

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock().get());

  float r1 = 1;
  float a2 = 0, a3 = 0;
  float b2 = 0, b3 = 0;
  float diam = 0;
  float vol = 0;
  int volgood = 0;
  float phi1, PHI, phi2;
  float fourThirdsPiOverEight =  static_cast<float>(((4.0f / 3.0f) * (DREAM3D::Constants::k_Pi)) / 8.0f);
  PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
  VectorOfFloatArray GSdist = pp->getFeatureSizeDistribution();
  float avg = GSdist[0]->getValue(0);
  float stdev = GSdist[1]->getValue(0);
  while (volgood == 0)
  {
    volgood = 1;
    diam = static_cast<float>(rg.genrand_norm(avg, stdev));
    diam = exp(diam);
    if(diam >= pp->getMaxFeatureDiameter()) { volgood = 0; }
    if(diam < pp->getMinFeatureDiameter()) { volgood = 0; }
    vol = fourThirdsPiOverEight * (diam * diam * diam);
  }
  int diameter = int((diam - pp->getMinFeatureDiameter()) / pp->getBinStepSize());
  float r2 = 0, r3 = 1;
  VectorOfFloatArray bovera = pp->getFeatureSize_BOverA();
  VectorOfFloatArray covera = pp->getFeatureSize_COverA();
  while (r2 < r3)
  {
    r2 = 0, r3 = 0;
    a2 = bovera[0]->getValue(diameter);
    b2 = bovera[1]->getValue(diameter);
    if(a2 == 0)
    {
      a2 = bovera[0]->getValue(diameter - 1);
      b2 = bovera[1]->getValue(diameter - 1);
    }
    r2 = static_cast<float>(rg.genrand_beta(a2, b2));
    a3 = covera[0]->getValue(diameter);
    b3 = covera[1]->getValue(diameter);
    if(a3 == 0)
    {
      a3 = covera[0]->getValue(diameter - 1);
      b3 = covera[1]->getValue(diameter - 1);
    }
    r3 = static_cast<float>( rg.genrand_beta(a3, b3) );
  }
  float random = static_cast<float>( rg.genrand_res53() );
  float totaldensity = 0;
  int bin = 0;
  FloatArrayType::Pointer axisodf = pp->getAxisOrientation();
  while (random > totaldensity && bin < static_cast<int>(axisodf->getSize()) )
  {
    totaldensity = totaldensity + axisodf->getValue(bin);
    bin++;
  }
  m_OrthoOps->determineEulerAngles(bin, phi1, PHI, phi2);
  VectorOfFloatArray omega3 = pp->getFeatureSize_Omegas();
  float mf = omega3[0]->getValue(diameter);
  float s = omega3[1]->getValue(diameter);
  float omega3f = static_cast<float>(rg.genrand_beta(mf, s));
  if(shapeclass == DREAM3D::ShapeType::EllipsoidShape) { omega3f = 1; }

  feature->m_Volumes = vol;
  feature->m_EquivalentDiameters = diam;
  feature->m_AxisLengths[0] = r1;
  feature->m_AxisLengths[1] = r2;
  feature->m_AxisLengths[2] = r3;
  feature->m_AxisEulerAngles[0] = phi1;
  feature->m_AxisEulerAngles[1] = PHI;
  feature->m_AxisEulerAngles[2] = phi2;
  feature->m_Omega3s = omega3f;
  feature->m_FeaturePhases = phase;
  feature->m_Neighborhoods = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::transfer_attributes(int gnum, Feature* feature)
{
  m_Volumes[gnum] = feature->m_Volumes;
  m_EquivalentDiameters[gnum] = feature->m_EquivalentDiameters;
  m_AxisLengths[3 * gnum + 0] = feature->m_AxisLengths[0];
  m_AxisLengths[3 * gnum + 1] = feature->m_AxisLengths[1];
  m_AxisLengths[3 * gnum + 2] = feature->m_AxisLengths[2];
  m_AxisEulerAngles[3 * gnum + 0] = feature->m_AxisEulerAngles[0];
  m_AxisEulerAngles[3 * gnum + 1] = feature->m_AxisEulerAngles[1];
  m_AxisEulerAngles[3 * gnum + 2] = feature->m_AxisEulerAngles[2];
  m_Omega3s[gnum] = feature->m_Omega3s;
  m_FeaturePhases[gnum] = feature->m_FeaturePhases;
  m_Neighborhoods[gnum] = feature->m_Neighborhoods;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::move_feature(size_t gnum, float xc, float yc, float zc)
{
  int occolumn, ocrow, ocplane;
  int nccolumn, ncrow, ncplane;
  int shiftcolumn, shiftrow, shiftplane;
  float oxc = m_Centroids[3 * gnum];
  float oyc = m_Centroids[3 * gnum + 1];
  float ozc = m_Centroids[3 * gnum + 2];
  occolumn = static_cast<int>( (oxc - (m_HalfPackingRes[0])) * m_OneOverPackingRes[0] );
  ocrow = static_cast<int>( (oyc - (m_HalfPackingRes[1])) * m_OneOverPackingRes[1] );
  ocplane = static_cast<int>( (ozc - (m_HalfPackingRes[2])) * m_OneOverPackingRes[2] );
  nccolumn = static_cast<int>( (xc - (m_HalfPackingRes[0])) * m_OneOverPackingRes[0] );
  ncrow = static_cast<int>( (yc - (m_HalfPackingRes[1])) * m_OneOverPackingRes[1] );
  ncplane = static_cast<int>( (zc - (m_HalfPackingRes[2])) * m_OneOverPackingRes[2] );
  shiftcolumn = nccolumn - occolumn;
  shiftrow = ncrow - ocrow;
  shiftplane = ncplane - ocplane;
  m_Centroids[3 * gnum] = xc;
  m_Centroids[3 * gnum + 1] = yc;
  m_Centroids[3 * gnum + 2] = zc;
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
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixName().getDataContainerName());

  float x, y, z;
  float xn, yn, zn;
  float dia, dia2;
  float dx, dy, dz;
  x = m_Centroids[3 * gnum];
  y = m_Centroids[3 * gnum + 1];
  z = m_Centroids[3 * gnum + 2];
  dia = m_EquivalentDiameters[gnum];
  size_t totalFeatures = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumTuples();
  int32_t increment = 0;
  if(add > 0) { increment = 1; }
  if(add < 0) { increment = -1; }
  for (size_t n = firstPrimaryFeature; n < totalFeatures; n++)
  {
    xn = m_Centroids[3 * n];
    yn = m_Centroids[3 * n + 1];
    zn = m_Centroids[3 * n + 2];
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
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixName().getDataContainerName());

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock().get());

  float neighborerror;
  float bhattdist;
  float dia;
  int nnum;
  size_t diabin = 0;
  size_t nnumbin = 0;
  int index = 0;

  int phase;
  typedef std::vector<std::vector<float> > VectOfVectFloat_t;
  for (size_t iter = 0; iter < simneighbordist.size(); ++iter)
  {
    phase = primaryphases[iter];
    PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
    VectOfVectFloat_t& curSimNeighborDist = simneighbordist[iter];
    size_t curSImNeighborDist_Size = curSimNeighborDist.size();
    float oneOverNeighborDistStep = 1.0f / neighbordiststep[iter];

    std::vector<int> count(curSImNeighborDist_Size, 0);
    for (size_t i = 0; i < curSImNeighborDist_Size; i++)
    {
      curSimNeighborDist[i].resize(40);
      for (size_t j = 0; j < 40; j++)
      {
        curSimNeighborDist[i][j] = 0;
      }
    }
    if(gadd > 0 && m_FeaturePhases[gadd] == phase)
    {
      determine_neighbors(gadd, 1);
    }
    if(gremove > 0 && m_FeaturePhases[gremove] == phase)
    {
      determine_neighbors(gremove, -1);
    }

    float maxFeatureDia = pp->getMaxFeatureDiameter();
    float minFeatureDia = pp->getMinFeatureDiameter();
    float oneOverBinStepSize = 1.0f / pp->getBinStepSize();

    size_t totalFeatures = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumTuples();
    for (size_t i = firstPrimaryFeature; i < totalFeatures; i++)
    {
      nnum = 0;
      index = i;
      if(index != gremove && m_FeaturePhases[index] == phase)
      {
        dia = m_EquivalentDiameters[index];
        if(dia > maxFeatureDia) { dia = maxFeatureDia; }
        if(dia < minFeatureDia) { dia = minFeatureDia; }
        diabin = static_cast<size_t>(((dia - minFeatureDia) * oneOverBinStepSize) );
        nnum = m_Neighborhoods[index];
        nnumbin = static_cast<size_t>( nnum * oneOverNeighborDistStep );
        if(nnumbin >= 40) { nnumbin = 39; }
        curSimNeighborDist[diabin][nnumbin]++;
        count[diabin]++;
      }
    }
    if(gadd > 0 && m_FeaturePhases[gadd] == phase)
    {
      dia = m_EquivalentDiameters[gadd];
      if(dia > maxFeatureDia) { dia = maxFeatureDia; }
      if(dia < minFeatureDia) { dia = minFeatureDia; }
      diabin = static_cast<size_t>(((dia - minFeatureDia) * oneOverBinStepSize) );
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

    if(gadd > 0 && m_FeaturePhases[gadd] == phase)
    {
      determine_neighbors(gadd, -1);
    }

    if(gremove > 0 && m_FeaturePhases[gremove] == phase)
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
void PackPrimaryPhases::compare_1Ddistributions(std::vector<float> array1, std::vector<float> array2, float& bhattdist)
{
  bhattdist = 0;
  for (size_t i = 0; i < array1.size(); i++)
  {
    bhattdist = bhattdist + sqrt((array1[i] * array2[i]));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::compare_2Ddistributions(std::vector<std::vector<float> > array1, std::vector<std::vector<float> > array2, float& bhattdist)
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
void PackPrimaryPhases::compare_3Ddistributions(std::vector<std::vector<std::vector<float> > > array1, std::vector<std::vector<std::vector<float> > > array2, float& bhattdist)
{
  bhattdist = 0;
  for (size_t i = 0; i < array1.size(); i++)
  {
    for (size_t j = 0; j < array1[i].size(); j++)
    {
      for (size_t k = 0; k < array1[i][j].size(); k++)
      {
        bhattdist = bhattdist + sqrt((array1[i][j][k] * array2[i][j][k]));
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float PackPrimaryPhases::check_sizedisterror(Feature* feature)
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixName().getDataContainerName());

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock().get());

  float dia;
  float sizedisterror = 0;
  float bhattdist;
  int index;
  int count = 0;
  int phase;
  size_t featureSizeDist_Size = featuresizedist.size();
  for (size_t iter = 0; iter < featureSizeDist_Size; ++iter)
  {
    phase = primaryphases[iter];
    PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
    count = 0;
    std::vector<float>& curFeatureSizeDist = featuresizedist[iter];
    std::vector<float>::size_type curFeatureSizeDistSize = curFeatureSizeDist.size();
    std::vector<float>& curSimFeatureSizeDist = simfeaturesizedist[iter];
    // Initialize all Values to Zero
    for (size_t i = 0; i < curFeatureSizeDistSize; i++)
    {
      curSimFeatureSizeDist[i] = 0.0f;
    }

    int64_t totalFeatures = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumTuples();
    float oneOverCurFeatureSizeDistStep = 1.0f / featuresizediststep[iter];
    float halfMinFeatureDiameter = pp->getMinFeatureDiameter() * 0.5f;
    for (int64_t b = firstPrimaryFeature; b < totalFeatures; b++)
    {
      index = b;
      if(m_FeaturePhases[index] == phase)
      {
        dia = m_EquivalentDiameters[index];
        dia = (dia - halfMinFeatureDiameter) * oneOverCurFeatureSizeDistStep;
        if(dia < 0) { dia = 0; }
        if(dia > curFeatureSizeDistSize - 1.0f) { dia = curFeatureSizeDistSize - 1.0f; }
        curSimFeatureSizeDist[int(dia)]++;
        count++;
      }
    }

    if(feature->m_FeaturePhases == phase)
    {
      dia = feature->m_EquivalentDiameters;
      dia = (dia - halfMinFeatureDiameter) * oneOverCurFeatureSizeDistStep;
      if(dia < 0) { dia = 0; }
      if(dia > curFeatureSizeDistSize - 1.0f) { dia = curFeatureSizeDistSize - 1.0f; }
      curSimFeatureSizeDist[int(dia)]++;
      count++;
    }
    float oneOverCount = 1.0f / count;

    if (count == 0)
    {
      for (size_t i = 0; i < curFeatureSizeDistSize; i++) { curSimFeatureSizeDist[i] = 0.0; }
    }
    else
    {
      for (size_t i = 0; i < curFeatureSizeDistSize; i++)
      {
        curSimFeatureSizeDist[i] = curSimFeatureSizeDist[i] * oneOverCount;
      }
    }
  }
  compare_2Ddistributions(simfeaturesizedist, featuresizedist, bhattdist);
  sizedisterror = bhattdist;
  return sizedisterror;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float PackPrimaryPhases::check_fillingerror(int gadd, int gremove, Int32ArrayType::Pointer featureOwnersPtr, Int32ArrayType::Pointer exclusionOwnersPtr)
{
  size_t featureOwnersIdx = 0;
  int32_t* featureOwners = featureOwnersPtr->getPointer(0);
  int32_t* exclusionOwners = exclusionOwnersPtr->getPointer(0);

  fillingerror = fillingerror * float(m_TotalPackingPoints);
  int col, row, plane;
  int k1 = 0, k2 = 0, k3 = 0;
  if(gadd > 0)
  {
    //size_t key, val;
    k1 = 2;
    k2 = -1;
    k3 = 1;
    size_t size = columnlist[gadd].size();
    std::vector<int>& cl = columnlist[gadd];
    std::vector<int>& rl = rowlist[gadd];
    std::vector<int>& pl = planelist[gadd];
    std::vector<float>& efl = ellipfunclist[gadd];
    float packquality = 0;
    for (size_t i = 0; i < size; i++)
    {
      col = cl[i];
      row = rl[i];
      plane = pl[i];
      if(m_PeriodicBoundaries == true)
      {
        if(col < 0) { col = col + m_PackingPoints[0]; }
        if(col > m_PackingPoints[0] - 1) { col = col - m_PackingPoints[0]; }
        if(row < 0) { row = row + m_PackingPoints[1]; }
        if(row > m_PackingPoints[1] - 1) { row = row - m_PackingPoints[1]; }
        if(plane < 0) { plane = plane + m_PackingPoints[2]; }
        if(plane > m_PackingPoints[2] - 1) { plane = plane - m_PackingPoints[2]; }
        featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + col;
        int currentFeatureOwner = featureOwners[featureOwnersIdx];
        if(exclusionOwners[featureOwnersIdx] > 0) {  }
        if(efl[i] > 0.1)
        {
          if(exclusionOwners[featureOwnersIdx] == 0)
          {
            pointsToRemove.push_back(featureOwnersIdx);
          }
          exclusionOwners[featureOwnersIdx]++;
        }
        fillingerror = fillingerror + ((k1 * currentFeatureOwner  + k2));
        //        fillingerror = fillingerror + (multiplier * (k1 * currentFeatureOwner  + k2));
        featureOwners[featureOwnersIdx] = currentFeatureOwner + k3;
        packquality = packquality + ((currentFeatureOwner) * (currentFeatureOwner));
      }
      else
      {
        if(col >= 0 && col < m_PackingPoints[0] && row >= 0 && row < m_PackingPoints[1] && plane >= 0 && plane < m_PackingPoints[2])
        {
          featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + col;
          int currentFeatureOwner = featureOwners[featureOwnersIdx];
          if(exclusionOwners[featureOwnersIdx] > 0) {  }
          if(efl[i] > 0.1)
          {
            if(exclusionOwners[featureOwnersIdx] == 0)
            {
              pointsToRemove.push_back(featureOwnersIdx);
            }
            exclusionOwners[featureOwnersIdx]++;
          }
          fillingerror = fillingerror + ((k1 * currentFeatureOwner  + k2));
          //        fillingerror = fillingerror + (multiplier * (k1 * currentFeatureOwner  + k2));
          featureOwners[featureOwnersIdx] = currentFeatureOwner + k3;
          packquality = packquality + ((currentFeatureOwner) * (currentFeatureOwner));
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
    std::vector<int>& cl = columnlist[gremove];
    std::vector<int>& rl = rowlist[gremove];
    std::vector<int>& pl = planelist[gremove];
    std::vector<float>& efl = ellipfunclist[gremove];
    for (size_t i = 0; i < size; i++)
    {
      col = cl[i];
      row = rl[i];
      plane = pl[i];

      if(m_PeriodicBoundaries == true)
      {
        if(col < 0) { col = col + m_PackingPoints[0]; }
        if(col > m_PackingPoints[0] - 1) { col = col - m_PackingPoints[0]; }
        if(row < 0) { row = row + m_PackingPoints[1]; }
        if(row > m_PackingPoints[1] - 1) { row = row - m_PackingPoints[1]; }
        if(plane < 0) { plane = plane + m_PackingPoints[2]; }
        if(plane > m_PackingPoints[2] - 1) { plane = plane - m_PackingPoints[2]; }
        featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + col;
        int currentFeatureOwner = featureOwners[featureOwnersIdx];
        if(exclusionOwners[featureOwnersIdx] > 1) { }
        if(efl[i] > 0.1)
        {
          exclusionOwners[featureOwnersIdx]--;
          if(exclusionOwners[featureOwnersIdx] == 0)
          {
            pointsToAdd.push_back(featureOwnersIdx);
          }
        }
        fillingerror = fillingerror + ((k1 * currentFeatureOwner  + k2));
        //        fillingerror = fillingerror + (multiplier * (k1 * currentFeatureOwner  + k2));
        featureOwners[featureOwnersIdx] = currentFeatureOwner + k3;
      }
      else
      {
        if(col >= 0 && col < m_PackingPoints[0] && row >= 0 && row < m_PackingPoints[1] && plane >= 0 && plane < m_PackingPoints[2])
        {
          featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + col;
          int currentFeatureOwner = featureOwners[featureOwnersIdx];
          if(exclusionOwners[featureOwnersIdx] > 1) { }
          if(efl[i] > 0.1)
          {
            exclusionOwners[featureOwnersIdx]--;
            if(exclusionOwners[featureOwnersIdx] == 0)
            {
              pointsToAdd.push_back(featureOwnersIdx);
            }
          }
          fillingerror = fillingerror + ((k1 * currentFeatureOwner  + k2));
          //          fillingerror = fillingerror + (multiplier * (k1 * currentFeatureOwner  + k2));
          featureOwners[featureOwnersIdx] = currentFeatureOwner + k3;
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
void PackPrimaryPhases::update_availablepoints(std::map<size_t, size_t>& availablePoints, std::map<size_t, size_t>& availablePointsInv)
{
  size_t removeSize = pointsToRemove.size();
  size_t addSize = pointsToAdd.size();
  size_t featureOwnersIdx;
  size_t key, val;
  for(size_t i = 0; i < removeSize; i++)
  {
    featureOwnersIdx = pointsToRemove[i];
    key = availablePoints[featureOwnersIdx];
    //  availablePoints.erase(featureOwnersIdx);
    val = availablePointsInv[availablePointsCount - 1];
    //  availablePointsInv.erase(availablePointsCount-1);
    if(key < availablePointsCount - 1)
    {
      availablePointsInv[key] = val;
      availablePoints[val] = key;
    }
    availablePointsCount--;
  }
  for(size_t i = 0; i < addSize; i++)
  {
    featureOwnersIdx = pointsToAdd[i];
    availablePoints[featureOwnersIdx] = availablePointsCount;
    availablePointsInv[availablePointsCount] = featureOwnersIdx;
    availablePointsCount++;
  }
  pointsToRemove.clear();
  pointsToAdd.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::insert_feature(size_t gnum)
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
  float bovera = m_AxisLengths[3 * gnum + 1];
  float covera = m_AxisLengths[3 * gnum + 2];
  float omega3 = m_Omega3s[gnum];
  float radcur1 = 1;
  unsigned int shapeclass = m_ShapeTypes[m_FeaturePhases[gnum]];

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
  float phi1 = m_AxisEulerAngles[3 * gnum];
  float PHI = m_AxisEulerAngles[3 * gnum + 1];
  float phi2 = m_AxisEulerAngles[3 * gnum + 2];
  float ga[3][3];
  float gaT[3][3];
  OrientationMath::EulertoMat(phi1, PHI, phi2, ga);
  xc = m_Centroids[3 * gnum];
  yc = m_Centroids[3 * gnum + 1];
  zc = m_Centroids[3 * gnum + 2];
  centercolumn = static_cast<int>( (xc - (m_PackingRes[0] * 0.5)) * m_OneOverPackingRes[0] );
  centerrow = static_cast<int>( (yc - (m_PackingRes[1] * 0.5)) * m_OneOverPackingRes[1] );
  centerplane = static_cast<int>( (zc - (m_PackingRes[2] * 0.5)) * m_OneOverPackingRes[2] );
  xmin = int(centercolumn - ((radcur1 * m_OneOverPackingRes[0]) + 1));
  xmax = int(centercolumn + ((radcur1 * m_OneOverPackingRes[0]) + 1));
  ymin = int(centerrow - ((radcur1 * m_OneOverPackingRes[1]) + 1));
  ymax = int(centerrow + ((radcur1 * m_OneOverPackingRes[1]) + 1));
  zmin = int(centerplane - ((radcur1 * m_OneOverPackingRes[2]) + 1));
  zmax = int(centerplane + ((radcur1 * m_OneOverPackingRes[2]) + 1));
  if(xmin < -m_PackingPoints[0]) { xmin = -m_PackingPoints[0]; }
  if(xmax > 2 * m_PackingPoints[0] - 1) { xmax = (2 * m_PackingPoints[0] - 1); }
  if(ymin < -m_PackingPoints[1]) { ymin = -m_PackingPoints[1]; }
  if(ymax > 2 * m_PackingPoints[1] - 1) { ymax = (2 * m_PackingPoints[1] - 1); }
  if(zmin < -m_PackingPoints[2]) { zmin = -m_PackingPoints[2]; }
  if(zmax > 2 * m_PackingPoints[2] - 1) { zmax = (2 * m_PackingPoints[2] - 1); }

  float OneOverRadcur1 = 1.0 / radcur1;
  float OneOverRadcur2 = 1.0 / radcur2;
  float OneOverRadcur3 = 1.0 / radcur3;
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
        MatrixMath::Transpose3x3(ga, gaT);
        MatrixMath::Multiply3x3with3x1(gaT, coords, coordsRotated);
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
  notifyStatusMessage(getHumanLabel(), "Assigning Voxels");

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixName().getDataContainerName());

  int64_t totalPoints = m->getAttributeMatrix(m_OutputCellAttributeMatrixName.getAttributeMatrixName())->getNumTuples();

  size_t udims[3] = {0, 0, 0};
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);

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

  float xRes = m->getGeometryAs<ImageGeom>()->getXRes();
  float yRes = m->getGeometryAs<ImageGeom>()->getYRes();
  float zRes = m->getGeometryAs<ImageGeom>()->getZRes();
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
  for (int64_t i = firstPrimaryFeature; i < totalFeatures; i++)
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
    //Unbounded Check for the size of shapeTypes. We assume a 1:1 with phase
    unsigned int shapeclass = m_ShapeTypes[m_FeaturePhases[i]];

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

    if (m_PeriodicBoundaries == true)
    {
      if (xmin < -dims[0]) { xmin = -dims[0]; }
      if (xmax > 2 * dims[0] - 1) { xmax = (2 * dims[0] - 1); }
      if (ymin < -dims[1]) { ymin = -dims[1]; }
      if (ymax > 2 * dims[1] - 1) { ymax = (2 * dims[1] - 1); }
      if (zmin < -dims[2]) { zmin = -dims[2]; }
      if (zmax > 2 * dims[2] - 1) { zmax = (2 * dims[2] - 1); }
    }
    else
    {
      if (xmin < 0) { xmin = 0; }
      if (xmax > dims[0] - 1) { xmax = dims[0] - 1; }
      if (ymin < 0) { ymin = 0; }
      if (ymax > dims[1] - 1) { ymax = dims[1] - 1; }
      if (zmin < 0) { zmin = 0; }
      if (zmax > dims[2] - 1) { zmax = dims[2] - 1; }
    }

    float radCur[3] = { radcur1, radcur2, radcur3 };
    float xx[3] = {xc, yc, zc };
    ShapeOps* shapeOps = m_ShapeOps[shapeclass];
    //#if 0
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range3d<int, int, int>(zmin, zmax + 1, ymin, ymax + 1, xmin, xmax + 1),
                        AssignVoxelsGapsImpl(dims, res, m_FeatureIds, radCur, xx, shapeOps, ga, size, i, newownersPtr, ellipfuncsPtr), tbb::auto_partitioner());

    }
    else
#endif
    {
      AssignVoxelsGapsImpl serial(dims, res, m_FeatureIds, radCur, xx, shapeOps, ga, size, i, newownersPtr, ellipfuncsPtr);
      serial.convert(zmin, zmax + 1, ymin, ymax + 1, xmin, xmax + 1);
    }


  }

  QVector<bool> activeObjects(totalFeatures, false);
  int gnum;
  for (size_t i = 0; i < static_cast<size_t>(totalPoints); i++)
  {
//    if(ellipfuncs[i] >= 0) { m_FeatureIds[i] = newowners[i]; }
    if(ellipfuncs[i] >= 0 && (m_UseMask == false || (m_UseMask == true && m_Mask[i] == true))) { m_FeatureIds[i] = newowners[i]; }
    if(m_UseMask == true && m_Mask[i] == false) { m_FeatureIds[i] = 0; }
    gnum = m_FeatureIds[i];
    if(gnum >= 0) { activeObjects[gnum] = true; }
    newowners[i] = -1;
    ellipfuncs[i] = -1.0;
  }

  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(getOutputCellFeatureAttributeMatrixName());
  cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());
  //need to update pointers after removing inactive objects
  updateFeatureInstancePointers();
  totalFeatures = cellFeatureAttrMat->getNumTuples();
  //counting the number of features for each phase
  for(int64_t i = 1; i < totalFeatures; i++)
  {
    int phase = m_FeaturePhases[i];
    if(phase >= 0) { m_NumFeatures[phase]++; }
  }

  //need to update pointers after resize, but do not need to run full data check because pointers are still valid
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
void PackPrimaryPhases::assign_gaps_only()
{
  notifyStatusMessage(getHumanLabel(), "Assigning Gaps");

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixName().getDataContainerName());

  int featurename, feature;
  int current = 0;
  int most = 0;
  int count = 1;
  int fixedCount = 1;
  int counter = 0;
  int neighpoint;
  int good;
  int neighbor;

  int xPoints = static_cast<int>(m->getGeometryAs<ImageGeom>()->getXPoints());
  int yPoints = static_cast<int>(m->getGeometryAs<ImageGeom>()->getYPoints());
  int zPoints = static_cast<int>(m->getGeometryAs<ImageGeom>()->getZPoints());
  size_t totalPoints = m->getAttributeMatrix(m_OutputCellAttributeMatrixName.getAttributeMatrixName())->getNumTuples();
  size_t totalFeatures = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumTuples();

  int neighpoints[6];
  neighpoints[0] = -xPoints * yPoints;
  neighpoints[1] = -xPoints;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xPoints;
  neighpoints[5] = xPoints * yPoints;

  Int32ArrayType::Pointer neighborsPtr = Int32ArrayType::CreateArray(m->getGeometryAs<ImageGeom>()->getNumberOfElements(), "Neighbors");
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
void PackPrimaryPhases::cleanup_features()
{
  notifyStatusMessage(getHumanLabel(), "Cleaning Up Features");

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixName().getDataContainerName());

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock().get());

  size_t totalPoints = m->getAttributeMatrix(m_OutputCellAttributeMatrixName.getAttributeMatrixName())->getNumTuples();
  size_t totalFeatures = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumTuples();
  size_t udims[3] = {0, 0, 0};
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
  vlists.resize(totalFeatures);
  QVector<int> currentvlist;
  QVector<bool> checked(totalPoints, false);
  QVector<bool> activeObjects(totalFeatures, true);
  size_t count;
  int touchessurface = 0;
  int good;
  int neighbor;
  DimType column, row, plane;
  int index;
  float minsize = 0;
  gsizes.resize(totalFeatures);
  for (size_t i = 1; i < totalFeatures; i++)
  {
    gsizes[i] = 0;
  }

  float resConst = m->getGeometryAs<ImageGeom>()->getXRes() * m->getGeometryAs<ImageGeom>()->getYRes() * m->getGeometryAs<ImageGeom>()->getZRes();
  const double k_PiOver6 = M_PI / 6.0;
  for (size_t i = 0; i < totalPoints; i++)
  {
    touchessurface = 0;
    if(checked[i] == false && m_FeatureIds[i] > firstPrimaryFeature)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[m_CellPhases[i]].get());
      minsize = static_cast<float>( pp->getMinFeatureDiameter() * pp->getMinFeatureDiameter() * pp->getMinFeatureDiameter() * k_PiOver6 );
      minsize = static_cast<float>( int(minsize / (resConst)) );
      currentvlist.push_back(i);
      count = 0;
      while (count < currentvlist.size())
      {
        index = currentvlist[count];
        column = index % xp;
        row = (index / xp) % yp;
        plane = index / (xp * yp);
        if(column == 0 || column == xp || row == 0 || row == yp || plane == 0 || plane == zp) { touchessurface = 1; }
        for (int j = 0; j < 6; j++)
        {
          good = 1;
          neighbor = static_cast<int>( index + neighpoints[j] );
          if(m_PeriodicBoundaries == false)
          {
            if(j == 0 && plane == 0) { good = 0; }
            if(j == 5 && plane == (zp - 1)) { good = 0; }
            if(j == 1 && row == 0) { good = 0; }
            if(j == 4 && row == (yp - 1)) { good = 0; }
            if(j == 2 && column == 0) { good = 0; }
            if(j == 3 && column == (xp - 1)) { good = 0; }
            if(good == 1 && m_FeatureIds[neighbor] == m_FeatureIds[index] && checked[neighbor] == false)
            {
              currentvlist.push_back(neighbor);
              checked[neighbor] = true;
            }
          }
          else if(m_PeriodicBoundaries == true)
          {
            if(j == 0 && plane == 0) { neighbor = static_cast<int>( neighbor + (xp * yp * zp) ); }
            if(j == 5 && plane == (zp - 1)) { neighbor = static_cast<int>( neighbor - (xp * yp * zp) ); }
            if(j == 1 && row == 0) { neighbor = static_cast<int>( neighbor + (xp * yp) ); }
            if(j == 4 && row == (yp - 1)) { neighbor = static_cast<int>( neighbor - (xp * yp) ); }
            if(j == 2 && column == 0) { neighbor = static_cast<int>( neighbor + (xp) ); }
            if(j == 3 && column == (xp - 1)) { neighbor = static_cast<int>( neighbor - (xp) ); }
            if(m_FeatureIds[neighbor] == m_FeatureIds[index] && checked[neighbor] == false)
            {
              currentvlist.push_back(neighbor);
              checked[neighbor] = true;
            }
          }
        }
        count++;
      }
      size_t size = vlists[m_FeatureIds[i]].size();
      if(size > 0)
      {
        if(size < currentvlist.size())
        {
          for (size_t k = 0; k < vlists[m_FeatureIds[i]].size(); k++)
          {
            m_FeatureIds[vlists[m_FeatureIds[i]][k]] = -1;
          }
          vlists[m_FeatureIds[i]].resize(currentvlist.size());
          vlists[m_FeatureIds[i]].swap(currentvlist);
        }
        else if(size >= currentvlist.size())
        {
          for (size_t k = 0; k < currentvlist.size(); k++)
          {
            m_FeatureIds[currentvlist[k]] = -1;
          }
        }
      }
      else if(size == 0)
      {
        if(currentvlist.size() >= minsize || touchessurface == 1)
        {
          vlists[m_FeatureIds[i]].resize(currentvlist.size());
          vlists[m_FeatureIds[i]].swap(currentvlist);
        }
        if(currentvlist.size() < minsize && touchessurface == 0)
        {
          for (size_t k = 0; k < currentvlist.size(); k++)
          {
            m_FeatureIds[currentvlist[k]] = -1;
          }
        }
      }
      currentvlist.clear();
    }
  }
  assign_gaps_only();
  for (int i = 0; i < totalPoints; i++)
  {
    if(m_FeatureIds[i] > 0) { gsizes[m_FeatureIds[i]]++; }
  }
  for (size_t i = firstPrimaryFeature; i < totalFeatures; i++)
  {
    if(gsizes[i] == 0) { activeObjects[i] = false; }
  }

  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(getOutputCellFeatureAttributeMatrixName());
  cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());
  updateFeatureInstancePointers();

  for (int i = 0; i < totalPoints; i++)
  {
    if(m_FeatureIds[i] > 0) { m_CellPhases[i] = m_FeaturePhases[m_FeatureIds[i]]; }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PackPrimaryPhases::estimate_numfeatures(int xpoints, int ypoints, int zpoints, float xres, float yres, float zres)
{
  //  int err = -1;

  float ptotalvol;
  int phase;

  ptotalvol = (xpoints * xres) * (ypoints * yres) * (zpoints * zres);
  if (ptotalvol == 0.0)
  {
    return 1;
  }

  // This is for convenience
  DataContainerArray::Pointer dca = getDataContainerArray();

  //DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixName().getDataContainerName());
  QVector<size_t> dims(1, 1);
  m_PhaseTypesPtr = dca->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getInputPhaseTypesArrayPath(), dims);
  DataArray<uint32_t>* phaseType = m_PhaseTypesPtr.lock().get();

  StatsDataArray::Pointer statsPtr = dca->getPrereqArrayFromPath<StatsDataArray, AbstractFilter>(this, getInputStatsArrayPath(), dims);
  m_StatsDataArray = boost::dynamic_pointer_cast<StatsDataArray>(statsPtr);
  if(m_StatsDataArray.lock().get() == NULL)
  {
    QString ss = QObject::tr("Stats Array Not Initialized correctly");
    setErrorCondition(-308);
    notifyErrorMessage(getHumanLabel(), ss, -308);
    return 1;
  }

#if 0

  IDataArray::Pointer iPtr = m->getAttributeMatrix(m_CellEnsembleAttributeMatrixName)->getAttributeArray(DREAM3D::EnsembleData::PhaseTypes);
  // Get the PhaseTypes - Remember there is a Dummy PhaseType in the first slot of the array
  DataArray<uint32_t>* phaseType = DataArray<uint32_t>::SafePointerDownCast(iPtr.get());

  iPtr = m->getAttributeMatrix(m_CellEnsembleAttributeMatrixName)->getAttributeArray(DREAM3D::EnsembleData::Statistics);
  StatsDataArray* statsDataArrayPtr = StatsDataArray::SafePointerDownCast(iPtr.get());
  if(NULL == statsDataArrayPtr)
  {
    return 1;
  }
#endif
  // Create a Reference Variable so we can use the [] syntax
  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock().get());

  DREAM3D_RANDOMNG_NEW()

  QVector<int> primaryPhasesLocal;
  QVector<double> primaryPhaseFractionsLocal;
  double totalprimaryfractions = 0.0;
  StatsData::Pointer statsData = StatsData::NullPointer();
  // find which phases are primary phases
  for (size_t i = 1; i < phaseType->getNumberOfTuples(); ++i)
  {
    if(phaseType->getValue(i) == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      primaryPhasesLocal.push_back(i);
      primaryPhaseFractionsLocal.push_back(pp->getPhaseFraction());
      totalprimaryfractions = totalprimaryfractions + pp->getPhaseFraction();
    }
  }
  // scale the primary phase fractions to total to 1
  for (size_t i = 0; i < primaryPhaseFractionsLocal.size(); i++)
  {
    primaryPhaseFractionsLocal[i] = primaryPhaseFractionsLocal[i] / totalprimaryfractions;
  }
  // generate the features
  int gid = 1;

  float currentvol = 0.0;
  float vol = 0.0f;
  float diam = 0.0f;
  int volgood = 0;
  for (size_t j = 0; j < primaryPhasesLocal.size(); ++j)
  {
    float curphasetotalvol = totalvol * primaryPhaseFractionsLocal[j];
    while (currentvol < (curphasetotalvol))
    {
      volgood = 0;
      phase = primaryPhasesLocal[j];
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
      while (volgood == 0)
      {
        volgood = 1;
        // u = rg.genrand_res53();
        if(pp->getFeatureSize_DistType() == DREAM3D::DistributionType::LogNormal)
        {
          float avgdiam = pp->getFeatureSizeDistribution().at(0)->getValue(0);
          float sddiam = pp->getFeatureSizeDistribution().at(1)->getValue(0);
          diam = rg.genrand_norm(avgdiam, sddiam);
          diam = exp(diam);
          if(diam >= pp->getMaxFeatureDiameter()) { volgood = 0; }
          if(diam < pp->getMinFeatureDiameter()) { volgood = 0; }
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
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixName().getDataContainerName());

  int64_t totalFeatures = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumTuples();

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(m_CsvOutputFile);
  QDir parentPath = fi.path();
  if(!parentPath.mkpath("."))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
    return;
  }

  QFile outFile(getCsvOutputFile());
  if (!outFile.open(QIODevice::WriteOnly))
  {
    QString msg = QObject::tr("CSV Output file could not be opened: %1").arg(getCsvOutputFile());
    setErrorCondition(-200);
    notifyErrorMessage(getHumanLabel(), "", getErrorCondition());
    return;
  }

  QTextStream dStream(&outFile);

  char space = ',';
  // Write the total number of features
  dStream << totalFeatures - firstPrimaryFeature;
  // Get all the names of the arrays from the Data Container
  QList<QString> headers = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getAttributeArrayNames();

  QVector<IDataArray::Pointer> data;

  //For checking if an array is a neighborlist
  NeighborList<int>::Pointer neighborlistPtr = NeighborList<int>::New();

  // Print the FeatureIds Header before the rest of the headers
  dStream << DREAM3D::FeatureData::FeatureID;
  // Loop throught the list and print the rest of the headers, ignoring those we don't want
  for(QList<QString>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
  {
    // Only get the array if the name does NOT match those listed
    IDataArray::Pointer p = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getAttributeArray(*iter);
    if(p->getNameOfClass().compare(neighborlistPtr->getNameOfClass()) != 0)
    {
      if (p->getNumberOfComponents() == 1)
      {
        dStream << space << (*iter);
      }
      else // There are more than a single component so we need to add multiple header values
      {
        for(int k = 0; k < p->getNumberOfComponents(); ++k)
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

  // Skip the first feature
  for(size_t i = firstPrimaryFeature; i < numTuples; ++i)
  {
    if (((float)i / numTuples) * 100.0f > threshold)
    {

      QString ss = QObject::tr("Writing Feature Data - %1% Complete").arg(((float)i / numTuples) * 100);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      threshold = threshold + 5.0f;
      if (threshold < ((float)i / numTuples) * 100.0f)
      {
        threshold = ((float)i / numTuples) * 100.0f;
      }
    }

    // Print the feature id
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer PackPrimaryPhases::newFilterInstance(bool copyFilterParameters)
{
  PackPrimaryPhases::Pointer filter = PackPrimaryPhases::New();
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
const QString PackPrimaryPhases::getCompiledLibraryName()
{ return SyntheticBuilding::SyntheticBuildingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PackPrimaryPhases::getGroupName()
{ return DREAM3D::FilterGroups::SyntheticBuildingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PackPrimaryPhases::getSubGroupName()
{ return DREAM3D::FilterSubGroups::PackingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PackPrimaryPhases::getHumanLabel()
{ return "Pack Primary Phases"; }

