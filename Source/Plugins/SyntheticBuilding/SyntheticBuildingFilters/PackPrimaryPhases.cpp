/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include "PackPrimaryPhases.h"

#include <fstream>

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range3d.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/ShapeType.h"
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/StatsData/PrimaryStatsData.h"
#include "SIMPLib/Utilities/SIMPLibRandom.h"
#include "SIMPLib/Utilities/TimeUtilities.h"

#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"

#include "SyntheticBuilding/SyntheticBuildingConstants.h"
#include "SyntheticBuilding/SyntheticBuildingVersion.h"

// Macro to determine if we are going to show the Debugging Output files
#define PPP_SHOW_DEBUG_OUTPUTS 0

/**
 * @brief The AssignVoxelsGapsImpl class implements a threaded algorithm that assigns all the voxels
 * in the volume to a unique Feature.
 */
class AssignVoxelsGapsImpl
{
  int64_t dims[3];
  float Invradcur[3];
  float res[3];
  int32_t* m_FeatureIds;
  float xc;
  float yc;
  float zc;
  ShapeOps* m_ShapeOps;
  float ga[3][3];
  int32_t curFeature;
  Int32ArrayType::Pointer newownersPtr;
  FloatArrayType::Pointer ellipfuncsPtr;

public:
  AssignVoxelsGapsImpl(int64_t* dimensions, float* resolution, int32_t* featureIds, float* radCur, float* xx, ShapeOps* shapeOps, float gA[3][3], float* size, int32_t cur_feature,
                       Int32ArrayType::Pointer newowners, FloatArrayType::Pointer ellipfuncs)
  : m_FeatureIds(featureIds)
  , m_ShapeOps(shapeOps)
  , curFeature(cur_feature)
  {
    size = 0;
    dims[0] = dimensions[0];
    dims[1] = dimensions[1];
    dims[2] = dimensions[2];
    Invradcur[0] = 1.0f / radCur[0];
    Invradcur[1] = 1.0f / radCur[1];
    Invradcur[2] = 1.0f / radCur[2];

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

  virtual ~AssignVoxelsGapsImpl()
  {
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void convert(int64_t zStart, int64_t zEnd, int64_t yStart, int64_t yEnd, int64_t xStart, int64_t xEnd) const
  {
    int64_t column = 0;
    int64_t row = 0;
    int64_t plane = 0;
    int64_t index = 0;
    float coords[3] = {0.0f, 0.0f, 0.0f};
    float inside = 0.0f;
    float coordsRotated[3] = {0.0f, 0.0f, 0.0f};
    int32_t* newowners = newownersPtr->getPointer(0);
    float* ellipfuncs = ellipfuncsPtr->getPointer(0);

    int64_t dim0_dim_1 = dims[0] * dims[1];
    for(int64_t iter1 = xStart; iter1 < xEnd; iter1++)
    {
      column = iter1;
      if(iter1 < 0)
      {
        column = iter1 + dims[0];
      }
      else if(iter1 > dims[0] - 1)
      {
        column = iter1 - dims[0];
      }

      for(int64_t iter2 = yStart; iter2 < yEnd; iter2++)
      {
        row = iter2;
        if(iter2 < 0)
        {
          row = iter2 + dims[1];
        }
        else if(iter2 > dims[1] - 1)
        {
          row = iter2 - dims[1];
        }
        size_t row_dim = row * dims[0];

        for(int64_t iter3 = zStart; iter3 < zEnd; iter3++)
        {
          plane = iter3;
          if(iter3 < 0)
          {
            plane = iter3 + dims[2];
          }
          else if(iter3 > dims[2] - 1)
          {
            plane = iter3 - dims[2];
          }

          index = static_cast<int64_t>((plane * dim0_dim_1) + (row_dim) + column);

          inside = -1.0f;
          coords[0] = float(iter1) * res[0];
          coords[1] = float(iter2) * res[1];
          coords[2] = float(iter3) * res[2];

          coords[0] = coords[0] - xc;
          coords[1] = coords[1] - yc;
          coords[2] = coords[2] - zc;
          MatrixMath::Multiply3x3with3x1(ga, coords, coordsRotated);
          float axis1comp = coordsRotated[0] * Invradcur[0];
          float axis2comp = coordsRotated[1] * Invradcur[1];
          float axis3comp = coordsRotated[2] * Invradcur[2];
          inside = m_ShapeOps->inside(axis1comp, axis2comp, axis3comp);
          // if (inside >= 0 && newowners[index] > 0)
          if(inside >= 0 && newowners[index] > 0 && inside > ellipfuncs[index])
          {
            newowners[index] = curFeature;
            ellipfuncs[index] = inside;
            // newowners[index] = -2;
            // ellipfuncs[index] = inside;
          }
          else if(inside >= 0 && newowners[index] == -1)
          {
            newowners[index] = curFeature;
            ellipfuncs[index] = inside;
          }
        }
      }
    }
  }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range3d<int64_t, int64_t, int64_t>& r) const
  {
    convert(r.pages().begin(), r.pages().end(), r.rows().begin(), r.rows().end(), r.cols().begin(), r.cols().end());
  }
#endif

private:
};

// Include the MOC generated file for this class
#include "moc_PackPrimaryPhases.cpp"

const QString PrimaryPhaseSyntheticShapeParametersName("Synthetic Shape Parameters (Primary Phase)");

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PackPrimaryPhases::PackPrimaryPhases()
: AbstractFilter()
, m_OutputCellAttributeMatrixPath(SIMPL::Defaults::SyntheticVolumeDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, "")
, m_OutputCellFeatureAttributeMatrixName(SIMPL::Defaults::CellFeatureAttributeMatrixName)
, m_OutputCellEnsembleAttributeMatrixName(SIMPL::Defaults::CellEnsembleAttributeMatrixName)
, m_FeatureIdsArrayName(SIMPL::CellData::FeatureIds)
, m_CellPhasesArrayName(SIMPL::CellData::Phases)
, m_FeaturePhasesArrayName(SIMPL::FeatureData::Phases)
, m_NumFeaturesArrayName(SIMPL::EnsembleData::NumFeatures)
, m_InputStatsArrayPath(SIMPL::Defaults::StatsGenerator, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::Statistics)
, m_InputPhaseTypesArrayPath(SIMPL::Defaults::StatsGenerator, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::PhaseTypes)
, m_InputPhaseNamesArrayPath(SIMPL::Defaults::StatsGenerator, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::PhaseName)
, m_InputShapeTypesArrayPath(SIMPL::Defaults::StatsGenerator, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::ShapeTypes)
, m_MaskArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Mask)
, m_UseMask(false)
, m_HaveFeatures(false)
, m_FeatureInputFile("")
, m_CsvOutputFile("")
, m_PeriodicBoundaries(false)
, m_WriteGoalAttributes(false)
, m_SaveGeometricDescriptions(false)
, m_NewAttributeMatrixPath(SIMPL::Defaults::SyntheticVolumeDataContainerName, PrimaryPhaseSyntheticShapeParametersName, "")
, m_NeighborhoodsArrayName(SIMPL::FeatureData::Neighborhoods)
, m_CentroidsArrayName(SIMPL::FeatureData::Centroids)
, m_VolumesArrayName(SIMPL::FeatureData::Volumes)
, m_AxisLengthsArrayName(SIMPL::FeatureData::AxisLengths)
, m_AxisEulerAnglesArrayName(SIMPL::FeatureData::AxisEulerAngles)
, m_Omega3sArrayName(SIMPL::FeatureData::Omega3s)
, m_EquivalentDiametersArrayName(SIMPL::FeatureData::EquivalentDiameters)
, m_Neighbors(nullptr)
, m_FeatureIds(nullptr)
, m_CellPhases(nullptr)
, m_Mask(nullptr)
, m_BoundaryCells(nullptr)
, m_FeaturePhases(nullptr)
, m_Neighborhoods(nullptr)
, m_Centroids(nullptr)
, m_Volumes(nullptr)
, m_AxisLengths(nullptr)
, m_AxisEulerAngles(nullptr)
, m_Omega3s(nullptr)
, m_EquivalentDiameters(nullptr)
, m_PhaseTypes(nullptr)
, m_ShapeTypes(nullptr)
, m_NumFeatures(nullptr)
, m_ErrorOutputFile("")
, m_VtkOutputFile("")
{

  initialize();
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
void PackPrimaryPhases::initialize()
{
  m_Neighbors = nullptr;
  m_BoundaryCells = nullptr;

  m_StatsDataArray = StatsDataArray::NullPointer();

  m_ShapeOps = ShapeOps::getShapeOpsQVector();
  m_UnknownShapeOps = ShapeOps::NullPointer();
  m_CubicOctohedronOps = ShapeOps::NullPointer();
  m_CylinderOps = ShapeOps::NullPointer();
  m_EllipsoidOps = ShapeOps::NullPointer();
  m_SuperEllipsoidOps = ShapeOps::NullPointer();
  m_OrthoOps = OrthoRhombicOps::New();

  m_ColumnList.clear();
  m_RowList.clear();
  m_PlaneList.clear();
  m_EllipFuncList.clear();

  m_PointsToAdd.clear();
  m_PointsToRemove.clear();
  m_Seed = QDateTime::currentMSecsSinceEpoch();
  m_FirstPrimaryFeature = 1;
  m_SizeX = m_SizeY = m_SizeZ = m_TotalVol = 0.0f;
  m_TotalVol = 1.0f;
  m_HalfPackingRes[0] = m_HalfPackingRes[1] = m_HalfPackingRes[2] = 1.0f;
  m_OneOverPackingRes[0] = m_OneOverPackingRes[1] = m_OneOverPackingRes[2] = 1.0f;
  m_OneOverHalfPackingRes[0] = m_OneOverHalfPackingRes[1] = m_OneOverHalfPackingRes[2] = 1.0f;
  m_PackingRes[0] = m_PackingRes[1] = m_PackingRes[2] = 1.0f;

  m_PackingPoints[0] = m_PackingPoints[1] = m_PackingPoints[2] = 1;
  m_TotalPackingPoints = 1;

  m_FeatureSizeDist.clear();
  m_SimFeatureSizeDist.clear();
  m_NeighborDist.clear();
  m_SimNeighborDist.clear();

  m_FeatureSizeDistStep.clear();
  m_NeighborDistStep.clear();

  m_PackQualities.clear();
  m_GSizes.clear();

  m_PrimaryPhases.clear();
  m_PrimaryPhaseFractions.clear();

  m_AvailablePointsCount = 1;
  m_FillingError = m_OldFillingError = 0.0f;
  m_CurrentNeighborhoodError = m_OldNeighborhoodError = 0.0f;
  m_CurrentSizeDistError = m_OldSizeDistError = 0.0f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_BOOL_FP("Periodic Boundaries", PeriodicBoundaries, FilterParameter::Parameter, PackPrimaryPhases));
  QStringList linkedProps("MaskArrayPath");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Use Mask", UseMask, FilterParameter::Parameter, PackPrimaryPhases, linkedProps));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Cell Attribute Matrix", OutputCellAttributeMatrixPath, FilterParameter::RequiredArray, PackPrimaryPhases, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Bool, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Mask", MaskArrayPath, FilterParameter::RequiredArray, PackPrimaryPhases, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::StatsDataArray, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Any);
    IGeometry::Types geomTypes;
    geomTypes.push_back(IGeometry::Type::Image);
    geomTypes.push_back(IGeometry::Type::Unknown);
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Statistics", InputStatsArrayPath, FilterParameter::RequiredArray, PackPrimaryPhases, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Any);
    IGeometry::Types geomTypes;
    geomTypes.push_back(IGeometry::Type::Image);
    geomTypes.push_back(IGeometry::Type::Unknown);
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phase Types", InputPhaseTypesArrayPath, FilterParameter::RequiredArray, PackPrimaryPhases, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::StringArray, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Any);
    IGeometry::Types geomTypes;
    geomTypes.push_back(IGeometry::Type::Image);
    geomTypes.push_back(IGeometry::Type::Unknown);
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phase Names", InputPhaseNamesArrayPath, FilterParameter::RequiredArray, PackPrimaryPhases, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Any);
    IGeometry::Types geomTypes;
    geomTypes.push_back(IGeometry::Type::Image);
    geomTypes.push_back(IGeometry::Type::Unknown);
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Shape Types", InputShapeTypesArrayPath, FilterParameter::RequiredArray, PackPrimaryPhases, req));
  }

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Feature Ids", FeatureIdsArrayName, FilterParameter::CreatedArray, PackPrimaryPhases));
  parameters.push_back(SIMPL_NEW_STRING_FP("Phases", CellPhasesArrayName, FilterParameter::CreatedArray, PackPrimaryPhases));

  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Feature Attribute Matrix", OutputCellFeatureAttributeMatrixName, FilterParameter::CreatedArray, PackPrimaryPhases));
  parameters.push_back(SIMPL_NEW_STRING_FP("Phases", FeaturePhasesArrayName, FilterParameter::CreatedArray, PackPrimaryPhases));

  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Ensemble Attribute Matrix", OutputCellEnsembleAttributeMatrixName, FilterParameter::CreatedArray, PackPrimaryPhases));
  parameters.push_back(SIMPL_NEW_STRING_FP("Number of Features", NumFeaturesArrayName, FilterParameter::CreatedArray, PackPrimaryPhases));

  linkedProps.clear();
  linkedProps << "FeatureInputFile";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Already Have Features", HaveFeatures, FilterParameter::Parameter, PackPrimaryPhases, linkedProps));
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Feature Input File", FeatureInputFile, FilterParameter::Parameter, PackPrimaryPhases, "*.txt", "Text File"));
  linkedProps.clear();
  linkedProps << "CsvOutputFile";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Write Goal Attributes", WriteGoalAttributes, FilterParameter::Parameter, PackPrimaryPhases, linkedProps));
  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Goal Attribute CSV File", CsvOutputFile, FilterParameter::Parameter, PackPrimaryPhases, "*.csv", "Comma Separated Data"));

  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Save Shape Description Arrays");
    parameter->setPropertyName("SaveGeometricDescriptions");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(PackPrimaryPhases, this, SaveGeometricDescriptions));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(PackPrimaryPhases, this, SaveGeometricDescriptions));

    QVector<QString> choices;
    choices.push_back("Do Not Save");
    choices.push_back("Save To New Attribute Matrix");
    choices.push_back("Append To Existing Attribute Matrix");
    parameter->setChoices(choices);
    QStringList linkedProps;
    linkedProps << "NewAttributeMatrixPath"
                << "SelectedAttributeMatrixPath";
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  {
    AttributeMatrixCreationFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_AM_CREATION_FP("New Attribute Matrix", NewAttributeMatrixPath, FilterParameter::Parameter, PackPrimaryPhases, req, 1));
  }

  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Selected Attribute Matrix", SelectedAttributeMatrixPath, FilterParameter::Parameter, PackPrimaryPhases, req, 2));
  }

#if PPP_SHOW_DEBUG_OUTPUTS
  parameters.push_back(InputFileFilterParameter::New("Debug VTK File", "VtkOutputFile", getVtkOutputFile(), FilterParameter::Parameter, "*.vtk", "VTK File"));
  parameters.push_back(InputFileFilterParameter::New("Debug Error File", "ErrorOutputFile", getErrorOutputFile(), FilterParameter::Parameter, "*.txt", "Text File"));
#endif

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOutputCellAttributeMatrixPath(reader->readDataArrayPath("OutputCellAttributeMatrixPath", getOutputCellAttributeMatrixPath()));
  setOutputCellFeatureAttributeMatrixName(reader->readString("OutputCellFeatureAttributeMatrixName", getOutputCellFeatureAttributeMatrixName()));
  setOutputCellEnsembleAttributeMatrixName(reader->readString("OutputCellEnsembleAttributeMatrixName", getOutputCellEnsembleAttributeMatrixName()));
  setFeatureIdsArrayName(reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName()));
  setCellPhasesArrayName(reader->readString("CellPhasesArrayName", getCellPhasesArrayName()));
  setFeaturePhasesArrayName(reader->readString("FeaturePhasesArrayName", getFeaturePhasesArrayName()));
  setNumFeaturesArrayName(reader->readString("NumFeaturesArrayName", getNumFeaturesArrayName()));
  setPeriodicBoundaries(reader->readValue("PeriodicBoundaries", false));
  setWriteGoalAttributes(reader->readValue("WriteGoalAttributes", false));
  setUseMask(reader->readValue("UseMask", getUseMask()));
  setHaveFeatures(reader->readValue("HaveFeatures", getHaveFeatures()));
  setFeatureInputFile(reader->readString("FeatureInputFile", getFeatureInputFile()));
  setCsvOutputFile(reader->readString("CsvOutputFile", getCsvOutputFile()));
  setInputStatsArrayPath(reader->readDataArrayPath("InputStatsArrayPath", getInputStatsArrayPath()));
  setInputPhaseTypesArrayPath(reader->readDataArrayPath("InputPhaseTypesArrayPath", getInputPhaseTypesArrayPath()));
  setInputShapeTypesArrayPath(reader->readDataArrayPath("InputShapeTypesArrayPath", getInputShapeTypesArrayPath()));
  setMaskArrayPath(reader->readDataArrayPath("MaskArrayPath", getMaskArrayPath()));
  //  setVtkOutputFile( reader->readString( "VtkOutputFile", getVtkOutputFile() ) );
  //  setErrorOutputFile( reader->readString( "ErrorOutputFile", getErrorOutputFile() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::readFilterParameters(QJsonObject& obj)
{
  AbstractFilter::readFilterParameters(obj);
  // setErrorOutputFile(obj["ErrorOutputFile"].toString());
  // setVtkOutputFile(obj["VtkOutputFile"].toString());
}

// FP: Check why these values are not connected to a filter parameter!

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::writeFilterParameters(QJsonObject& obj)
{
  AbstractFilter::writeFilterParameters(obj);
  // obj["ErrorOutputFile"] = getErrorOutputFile();
  // obj["VtkOutputFile"] = getVtkOutputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::updateFeatureInstancePointers()
{
  setErrorCondition(0);
  if(nullptr != m_FeaturePhasesPtr.lock().get())
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  }
  if(nullptr != m_NeighborhoodsPtr.lock().get())
  {
    m_Neighborhoods = m_NeighborhoodsPtr.lock()->getPointer(0);
  }
  if(nullptr != m_EquivalentDiametersPtr.lock().get())
  {
    m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0);
  }
  if(nullptr != m_VolumesPtr.lock().get())
  {
    m_Volumes = m_VolumesPtr.lock()->getPointer(0);
  }
  if(nullptr != m_Omega3sPtr.lock().get())
  {
    m_Omega3s = m_Omega3sPtr.lock()->getPointer(0);
  }
  if(nullptr != m_CentroidsPtr.lock().get())
  {
    m_Centroids = m_CentroidsPtr.lock()->getPointer(0);
  }
  if(nullptr != m_AxisEulerAnglesPtr.lock().get())
  {
    m_AxisEulerAngles = m_AxisEulerAnglesPtr.lock()->getPointer(0);
  }
  if(nullptr != m_AxisLengthsPtr.lock().get())
  {
    m_AxisLengths = m_AxisLengthsPtr.lock()->getPointer(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  // Make sure we have our input DataContainer with the proper Ensemble data
  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getOutputCellAttributeMatrixPath().getDataContainerName());

  QVector<DataArrayPath> cellDataArrayPaths;
  QVector<DataArrayPath> ensembleDataArrayPaths;

  // Input Ensemble Data that we require
  QVector<size_t> cDims(1, 1);
  m_PhaseTypesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getInputPhaseTypesArrayPath(), cDims);
  if(nullptr != m_PhaseTypesPtr.lock().get())
  {
    m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0);
  }
  if(getErrorCondition() >= 0)
  {
    ensembleDataArrayPaths.push_back(getInputPhaseTypesArrayPath());
  }

  m_PhaseNamesPtr = getDataContainerArray()->getPrereqArrayFromPath<StringDataArray, AbstractFilter>(this, getInputPhaseNamesArrayPath(), cDims);
  if(getErrorCondition() >= 0)
  {
    ensembleDataArrayPaths.push_back(getInputPhaseNamesArrayPath());
  }

  m_ShapeTypesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getInputShapeTypesArrayPath(), cDims);
  if(nullptr != m_ShapeTypesPtr.lock().get())
  {
    m_ShapeTypes = m_ShapeTypesPtr.lock()->getPointer(0);
  }
  if(getErrorCondition() >= 0)
  {
    ensembleDataArrayPaths.push_back(getInputShapeTypesArrayPath());
  }

  m_StatsDataArray = getDataContainerArray()->getPrereqArrayFromPath<StatsDataArray, AbstractFilter>(this, getInputStatsArrayPath(), cDims);
  if(m_StatsDataArray.lock() == nullptr)
  {
    QString ss = QObject::tr("Statistics array is not initialized correctly. The path is %1").arg(getInputStatsArrayPath().serialize());
    setErrorCondition(-78000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if(getErrorCondition() >= 0)
  {
    ensembleDataArrayPaths.push_back(getInputStatsArrayPath());
  }

  if(m_UseMask == true)
  {
    m_MaskPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getMaskArrayPath(), cDims);
    if(nullptr != m_MaskPtr.lock().get())
    {
      m_Mask = m_MaskPtr.lock()->getPointer(0);
    }
    if(getErrorCondition() >= 0)
    {
      cellDataArrayPaths.push_back(getMaskArrayPath());
    }
  }

  cDims[0] = 1;
  // Cell Data
  tempPath.update(getOutputCellAttributeMatrixPath().getDataContainerName(), getOutputCellAttributeMatrixPath().getAttributeMatrixName(), getFeatureIdsArrayName());
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, -1, cDims); /* Assigns the shared_ptr<>(this, tempPath, -1, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock().get())
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  }

  tempPath.update(getOutputCellAttributeMatrixPath().getDataContainerName(), getOutputCellAttributeMatrixPath().getAttributeMatrixName(), getCellPhasesArrayName());
  m_CellPhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, cDims); /* Assigns the shared_ptr<>(this, tempPath, 0, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellPhasesPtr.lock().get())
  {
    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  }

  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixPath().getDataContainerName());

  QVector<size_t> tDims(1, 0);
  m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getOutputCellFeatureAttributeMatrixName(), tDims, AttributeMatrix::Type::CellFeature);

  PackPrimaryPhases::SaveMethod saveMethod = static_cast<PackPrimaryPhases::SaveMethod>(getSaveGeometricDescriptions());
  if(saveMethod == PackPrimaryPhases::SaveMethod::SaveToNew)
  {
    m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getNewAttributeMatrixPath().getAttributeMatrixName(), tDims, AttributeMatrix::Type::CellFeature);
  }
  else if(saveMethod == PackPrimaryPhases::SaveMethod::AppendToExisting)
  {
    int err = 0;
    m->getPrereqAttributeMatrix<AbstractFilter>(this, getSelectedAttributeMatrixPath().getAttributeMatrixName(), err);
  }

  AttributeMatrix::Pointer outEnsembleAttrMat = AttributeMatrix::NullPointer();
  if(m->doesAttributeMatrixExist(getOutputCellEnsembleAttributeMatrixName()))
  {
    outEnsembleAttrMat = m->getPrereqAttributeMatrix(this, getOutputCellEnsembleAttributeMatrixName(), -350);
  }
  else
  {
    tDims[0] = m_PhaseTypesPtr.lock()->getNumberOfTuples();
    outEnsembleAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getOutputCellEnsembleAttributeMatrixName(), tDims, AttributeMatrix::Type::CellEnsemble);
  }

  tempPath = getOutputCellAttributeMatrixPath();
  tempPath.setAttributeMatrixName(getOutputCellEnsembleAttributeMatrixName());
  tempPath.setDataArrayName(SIMPL::EnsembleData::PhaseName);
  m_PhaseNamesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<StringDataArray, AbstractFilter, QString>(this, tempPath, 0,
                                                                                                                    cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */

  // Feature Data
  tempPath.update(getOutputCellAttributeMatrixPath().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), getFeaturePhasesArrayName());
  m_FeaturePhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeaturePhasesPtr.lock().get())
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  }

  tempPath.update(getOutputCellAttributeMatrixPath().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), m_NeighborhoodsArrayName);
  m_NeighborhoodsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, cDims);
  if(nullptr != m_NeighborhoodsPtr.lock().get())
  {
    m_Neighborhoods = m_NeighborhoodsPtr.lock()->getPointer(0);
  }

  tempPath.update(getOutputCellAttributeMatrixPath().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), m_EquivalentDiametersArrayName);
  m_EquivalentDiametersPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_EquivalentDiametersPtr.lock().get())
  {
    m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0);
  }

  tempPath.update(getOutputCellAttributeMatrixPath().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), m_VolumesArrayName);
  m_VolumesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0,
                                                                                                                cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_VolumesPtr.lock().get())
  {
    m_Volumes = m_VolumesPtr.lock()->getPointer(0);
  }

  tempPath.update(getOutputCellAttributeMatrixPath().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), m_Omega3sArrayName);
  m_Omega3sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0,
                                                                                                                cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_Omega3sPtr.lock().get())
  {
    m_Omega3s = m_Omega3sPtr.lock()->getPointer(0);
  }

  cDims[0] = 3;
  tempPath.update(getOutputCellAttributeMatrixPath().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), m_CentroidsArrayName);
  m_CentroidsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0,
                                                                                                                  cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CentroidsPtr.lock().get())
  {
    m_Centroids = m_CentroidsPtr.lock()->getPointer(0);
  }

  tempPath.update(getOutputCellAttributeMatrixPath().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), m_AxisEulerAnglesArrayName);
  m_AxisEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AxisEulerAnglesPtr.lock().get())
  {
    m_AxisEulerAngles = m_AxisEulerAnglesPtr.lock()->getPointer(0);
  }

  tempPath.update(getOutputCellAttributeMatrixPath().getDataContainerName(), getOutputCellFeatureAttributeMatrixName(), m_AxisLengthsArrayName);
  m_AxisLengthsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0,
                                                                                                                    cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AxisLengthsPtr.lock().get())
  {
    m_AxisLengths = m_AxisLengthsPtr.lock()->getPointer(0);
  }

  // Ensemble Data
  cDims[0] = 1;
  tempPath.update(getOutputCellAttributeMatrixPath().getDataContainerName(), getOutputCellEnsembleAttributeMatrixName(), getNumFeaturesArrayName());
  m_NumFeaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_NumFeaturesPtr.lock().get())
  {
    m_NumFeatures = m_NumFeaturesPtr.lock()->getPointer(0);
  }

  if(m_WriteGoalAttributes == true)
  {
    if(getCsvOutputFile().isEmpty() == true)
    {
      QString ss = QObject::tr("The goal attribute output file must be set");
      setErrorCondition(-78001);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }

    QFileInfo fi(getCsvOutputFile());

    QDir parentPath = fi.path();
    if(parentPath.exists() == false)
    {
      QString ss = QObject::tr("The directory path for the GoalAttribute output file does not exist. The application will attempt to create this path during execution of the filter");
      notifyWarningMessage(getHumanLabel(), ss, -1);
    }
  }

  if(m_HaveFeatures == true)
  {
    QFileInfo fi(getFeatureInputFile());

    if(getFeatureInputFile().isEmpty() == true)
    {
      QString ss = QObject::tr("The input feature file must be set");
      setErrorCondition(-78003);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
    else if(fi.exists() == false)
    {
      QString ss = QObject::tr("The input feature file does not exist");
      setErrorCondition(-78004);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
  }
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

  DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixPath());
  if(dc == nullptr)
  {
    setInPreflight(false);
    return;
  }
  AttributeMatrix::Pointer attrMat = dc->getAttributeMatrix(getOutputCellFeatureAttributeMatrixName());
  if(attrMat == nullptr)
  {
    setInPreflight(false);
    return;
  }

  moveShapeDescriptions();

  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::execute()
{
  initialize();

  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  if(m_HaveFeatures == false)
  {
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Packing Features || Initializing Volume");
    // this initializes the arrays to hold the details of the locations of all of the features during packing
    Int32ArrayType::Pointer featureOwnersPtr = initializePackingGrid();
    if(getErrorCondition() < 0)
    {
      return;
    }
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Packing Features || Placing Features");
    placeFeatures(featureOwnersPtr);
    if(getErrorCondition() < 0)
    {
      return;
    }
    if(getCancel() == true)
    {
      return;
    }
  }

  if(m_HaveFeatures == true)
  {
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Loading Features");
    loadFeatures();
    if(getCancel() == true)
    {
      return;
    }
  }

  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Packing Features || Assigning Voxels");
  assignVoxels();
  if(getErrorCondition() < 0)
  {
    return;
  }
  if(getCancel() == true)
  {
    return;
  }

  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Packing Features || Assigning Gaps");
  assignGapsOnly();
  if(getCancel() == true)
  {
    return;
  }

  // notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Packing Features || Cleaning Up Volume");
  // cleanup_features();
  // if (getCancel() == true) { return; }

  if(m_WriteGoalAttributes == true)
  {
    writeGoalAttributes();
  }
  if(getErrorCondition() < 0)
  {
    return;
  }

  moveShapeDescriptions();

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixPath().getDataContainerName());

  AttributeMatrix::Pointer ensembleAttrMat = getDataContainerArray()->getAttributeMatrix(getInputPhaseNamesArrayPath());
  IDataArray::Pointer inputPhaseNames = ensembleAttrMat->getAttributeArray(getInputPhaseNamesArrayPath().getDataArrayName());
  if(inputPhaseNames.get() != nullptr)
  {
    AttributeMatrix::Pointer cellEnsembleAttrMat = m->getAttributeMatrix(m_OutputCellEnsembleAttributeMatrixName);
    IDataArray::Pointer outputPhaseNames = inputPhaseNames->deepCopy();
    cellEnsembleAttrMat->addAttributeArray(outputPhaseNames->getName(), outputPhaseNames);
  }
  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t PackPrimaryPhases::writeVtkFile(int32_t* featureOwners, int32_t* exclusionZones)
{
  size_t featureOwnersIdx = 0;
  std::ofstream outFile;
  outFile.open(m_VtkOutputFile.toLatin1().data(), std::ios_base::binary);
  if(outFile.is_open() == false)
  {
    qDebug() << "m_VtkOutputFile: " << m_VtkOutputFile << "\n";
    notifyErrorMessage(getHumanLabel(), "Could not open Vtk File for writing from PackFeatures", -1);
    setErrorCondition(-78005);
    return -1;
  }
  outFile << "# vtk DataFile Version 2.0"
          << "\n";
  outFile << "DREAM.3D Generated from PackPrimaryPhases Filter"
          << "\n";
  outFile << "ASCII"
          << "\n";
  outFile << "DATASET STRUCTURED_POINTS"
          << "\n";
  outFile << "DIMENSIONS " << m_PackingPoints[0] << " " << m_PackingPoints[1] << " " << m_PackingPoints[2] << "\n";
  outFile << "ORIGIN 0.0 0.0 0.0"
          << "\n";
  outFile << "SPACING " << m_PackingRes[0] << " " << m_PackingRes[1] << " " << m_PackingRes[2] << "\n";
  outFile << "POINT_DATA " << m_PackingPoints[0] * m_PackingPoints[1] * m_PackingPoints[2] << "\n";
  outFile << "\n";
  outFile << "\n";
  outFile << "SCALARS NumOwners int  1"
          << "\n";
  outFile << "LOOKUP_TABLE default"
          << "\n";
  for(int64_t i = 0; i < (m_PackingPoints[2]); i++)
  {
    for(int64_t j = 0; j < (m_PackingPoints[1]); j++)
    {
      for(int64_t k = 0; k < (m_PackingPoints[0]); k++)
      {
        featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * i) + (m_PackingPoints[0] * j) + k;
        int32_t name = featureOwners[featureOwnersIdx];
        if(i % 20 == 0 && i > 0)
        {
          outFile << "\n";
        }
        outFile << "     ";
        if(name < 100)
        {
          outFile << " ";
        }
        if(name < 10)
        {
          outFile << " ";
        }
        outFile << name;
      }
    }
  }
  outFile << "SCALARS ExclusionZone int  1"
          << "\n";
  outFile << "LOOKUP_TABLE default"
          << "\n";
  for(int64_t i = 0; i < (m_PackingPoints[2]); i++)
  {
    for(int64_t j = 0; j < (m_PackingPoints[1]); j++)
    {
      for(int64_t k = 0; k < (m_PackingPoints[0]); k++)
      {
        featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * i) + (m_PackingPoints[0] * j) + k;
        int32_t val = exclusionZones[featureOwnersIdx];
        if(i % 20 == 0 && i > 0)
        {
          outFile << "\n";
        }
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
void PackPrimaryPhases::loadFeatures()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixPath().getDataContainerName());
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName);

  std::ifstream inFile;
  inFile.open(getFeatureInputFile().toLatin1().data(), std::ios_base::binary);
  if(!inFile)
  {
    QString ss = QObject::tr("Failed to open: %1").arg(getFeatureInputFile());
    setErrorCondition(-78006);
    notifyErrorMessage(getHumanLabel(), ss, -1);
  }
  int32_t numFeatures = 0;
  inFile >> numFeatures;
  if(0 == numFeatures)
  {
    notifyErrorMessage(getHumanLabel(), "The number of Features is 0 and should be greater than 0", -600);
  }

  m_FirstPrimaryFeature = 1;

  QVector<size_t> tDims(1, m_FirstPrimaryFeature + numFeatures);
  cellFeatureAttrMat->setTupleDimensions(tDims);
  updateFeatureInstancePointers();

  int32_t phase = 1;
  float xC = 0.0f, yC = 0.0f, zC = 0.0f;
  float axisA = 0.0f, axisB = 0.0f, axisC = 0.0f;
  float vol = 0.0f, eqDiam = 0.0f;
  float omega3 = 0.0f;
  float phi1 = 0.0f, PHI = 0.0f, phi2 = 0.0f;
  size_t currentFeature = m_FirstPrimaryFeature;
  const float fourThirds = 4.0f / 3.0f;
  for(int32_t i = 0; i < numFeatures; i++)
  {
    inFile >> phase >> xC >> yC >> zC >> axisA >> axisB >> axisC >> omega3 >> phi1 >> PHI >> phi2;
    vol = fourThirds * SIMPLib::Constants::k_Pi * axisA * axisB * axisC;
    eqDiam = 2.0f * powf((vol * (0.75f) * (SIMPLib::Constants::k_1OverPi)), (SIMPLib::Constants::k_1Over3));
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
void PackPrimaryPhases::placeFeatures(Int32ArrayType::Pointer featureOwnersPtr)
{
  bool writeErrorFile = false;
  std::ofstream outFile;
  if(m_ErrorOutputFile.isEmpty() == false)
  {
    outFile.open(m_ErrorOutputFile.toLatin1().data(), std::ios_base::binary);
    writeErrorFile = outFile.is_open();
  }

  m_Seed = QDateTime::currentMSecsSinceEpoch();
  SIMPL_RANDOMNG_NEW_SEEDED(m_Seed);

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixPath().getDataContainerName());

  size_t totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  // Get the number of input ensembles from one of the input arrays that are located in the Input Ensemble AttributeMatrix
  size_t totalEnsembles = m_PhaseTypesPtr.lock()->getNumberOfTuples();

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock().get());

  size_t udims[3] = {0, 0, 0};
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]),
  };

  float xRes = m->getGeometryAs<ImageGeom>()->getXRes();
  float yRes = m->getGeometryAs<ImageGeom>()->getYRes();
  float zRes = m->getGeometryAs<ImageGeom>()->getZRes();
  m_SizeX = static_cast<float>(dims[0] * m->getGeometryAs<ImageGeom>()->getXRes());
  m_SizeY = static_cast<float>(dims[1] * m->getGeometryAs<ImageGeom>()->getYRes());
  m_SizeZ = static_cast<float>(dims[2] * m->getGeometryAs<ImageGeom>()->getZRes());
  m_TotalVol = m_SizeX * m_SizeY * m_SizeZ;

  // Making a double to prevent float overflow on incrementing
  double totalprimaryvolTEMP = 0;
  size_t totalVox = static_cast<size_t>(dims[0] * dims[1] * dims[2]);
  for(size_t i = 0; i < totalVox; i++)
  {
    if(m_FeatureIds[i] <= 0)
    {
      totalprimaryvolTEMP++;
    }
  }
  float totalprimaryvol = static_cast<float>(totalprimaryvolTEMP);
  totalprimaryvol = totalprimaryvol * (m->getGeometryAs<ImageGeom>()->getXRes() * m->getGeometryAs<ImageGeom>()->getYRes() * m->getGeometryAs<ImageGeom>()->getZRes());

  float change = 0.0f;
  int32_t phase = 0;
  int32_t randomfeature = 0;
  float xc = 0.0f, yc = 0.0f, zc = 0.0f;
  float oldxc = 0.0f, oldyc = 0.0f, oldzc = 0.0f;
  m_OldFillingError = 0.0f;
  m_CurrentNeighborhoodError = 0.0f, m_OldNeighborhoodError = 0.0f;
  m_CurrentSizeDistError = 0.0f, m_OldSizeDistError = 0.0f;
  int32_t acceptedmoves = 0;
  float totalprimaryfractions = 0.0f;

  // find which phases are primary phases
  for(size_t i = 1; i < totalEnsembles; ++i)
  {
    if(m_PhaseTypes[i] == static_cast<PhaseType::EnumType>(PhaseType::Type::Primary))
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      if(nullptr == pp)
      {
        QString ss = QObject::tr("Tried to cast a statsDataArray[%1].get() to a PrimaryStatsData* "
                                 "pointer but this resulted in a nullptr pointer. The value at m_PhaseTypes[%2] = %3 does not match up "
                                 "with the type of pointer stored in the StatsDataArray (PrimaryStatsData)\n")
                         .arg(i)
                         .arg(i)
                         .arg(m_PhaseTypes[i]);
        setErrorCondition(-78007);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
      m_PrimaryPhases.push_back(static_cast<int32_t>(i));
      m_PrimaryPhaseFractions.push_back(pp->getPhaseFraction());
      totalprimaryfractions = totalprimaryfractions + pp->getPhaseFraction();
    }
  }
  // scale the primary phase fractions to total to 1
  for(size_t i = 0; i < m_PrimaryPhaseFractions.size(); i++)
  {
    m_PrimaryPhaseFractions[i] = m_PrimaryPhaseFractions[i] / totalprimaryfractions;
  }

  QVector<size_t> cDim(1, 1);
  Int32ArrayType::Pointer exclusionOwnersPtr = Int32ArrayType::CreateArray(m_TotalPackingPoints, cDim, "_INTERNAL_USE_ONLY_PackPrimaryFeatures::exclusions_owners");
  exclusionOwnersPtr->initializeWithValue(0);

  // This is the set that we are going to keep updated with the points that are not in an exclusion zone
  std::map<size_t, size_t> availablePoints;
  std::map<size_t, size_t> availablePointsInv;

  // Get a pointer to the Feature Owners that was just initialized in the initialize_packinggrid() method
  int32_t* featureOwners = featureOwnersPtr->getPointer(0);
  int32_t* exclusionOwners = exclusionOwnersPtr->getPointer(0);
  int64_t featureOwnersIdx = 0;

  // determine initial set of available points
  m_AvailablePointsCount = 0;
  for(int64_t i = 0; i < m_TotalPackingPoints; i++)
  {
    if((exclusionOwners[i] == 0 && m_UseMask == false) || (exclusionOwners[i] == 0 && m_UseMask == true && m_Mask[i] == true))
    {
      availablePoints[i] = m_AvailablePointsCount;
      availablePointsInv[m_AvailablePointsCount] = i;
      m_AvailablePointsCount++;
    }
  }
  // and clear the pointsToRemove and pointsToAdd vectors from the initial packing
  m_PointsToRemove.clear();
  m_PointsToAdd.clear();

  // initialize the sim and goal size distributions for the primary phases
  m_FeatureSizeDist.resize(m_PrimaryPhases.size());
  m_SimFeatureSizeDist.resize(m_PrimaryPhases.size());
  m_FeatureSizeDistStep.resize(m_PrimaryPhases.size());
  size_t numPrimaryPhases = m_PrimaryPhases.size();
  for(size_t i = 0; i < numPrimaryPhases; i++)
  {
    phase = m_PrimaryPhases[i];
    PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
    m_FeatureSizeDist[i].resize(40);
    m_SimFeatureSizeDist[i].resize(40);
    m_FeatureSizeDistStep[i] = static_cast<float>(((2 * pp->getMaxFeatureDiameter()) - (pp->getMinFeatureDiameter() / 2.0f)) / m_FeatureSizeDist[i].size());
    float input = 0.0f;
    float previoustotal = 0.0f;
    VectorOfFloatArray GSdist = pp->getFeatureSizeDistribution();
    float avg = GSdist[0]->getValue(0);
    float stdev = GSdist[1]->getValue(0);
    float denominatorConst = 1.0f / sqrtf(2.0f * stdev * stdev); // Calculate it here rather than calculating the same thing multiple times below
    size_t numFeatureSizeDist = m_FeatureSizeDist[i].size();
    for(size_t j = 0; j < numFeatureSizeDist; j++)
    {
      input = (float(j + 1) * m_FeatureSizeDistStep[i]) + (pp->getMinFeatureDiameter() / 2.0f);
      float logInput = logf(input);
      if(logInput <= avg)
      {
        m_FeatureSizeDist[i][j] = 0.5f - 0.5f * (SIMPLibMath::erf((avg - logInput) * denominatorConst)) - previoustotal;
      }
      if(logInput > avg)
      {
        m_FeatureSizeDist[i][j] = 0.5f + 0.5f * (SIMPLibMath::erf((logInput - avg) * denominatorConst)) - previoustotal;
      }
      previoustotal = previoustotal + m_FeatureSizeDist[i][j];
    }
  }

  if(getCancel() == true)
  {
    return;
  }

  // generate the features and monitor the size distribution error while doing so. After features are generated, no new features can enter or leave the structure.
  Feature_t feature;

  // Estimate the total Number of features here
  int32_t estNumFeatures = estimateNumFeatures(udims[0], udims[1], udims[2], xRes, yRes, zRes);
  QVector<size_t> tDims(1, estNumFeatures);
  m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->resizeAttributeArrays(tDims);
  // need to update pointers after resize, buut do not need to run full data check because pointers are still valid
  updateFeatureInstancePointers();

  int32_t gid = 1;
  m_FirstPrimaryFeature = gid;
  std::vector<float> curphasevol;
  curphasevol.resize(m_PrimaryPhases.size());
  float factor = 1.0f;
  size_t iter = 0;
  for(size_t j = 0; j < numPrimaryPhases; ++j)
  {
    curphasevol[j] = 0;
    float curphasetotalvol = totalprimaryvol * m_PrimaryPhaseFractions[j];
    while(curphasevol[j] < (factor * curphasetotalvol))
    {
      iter++;
      m_Seed++;
      phase = m_PrimaryPhases[j];
      generateFeature(phase, &feature, m_ShapeTypes[phase]);
      m_CurrentSizeDistError = checkSizeDistError(&feature);
      change = (m_CurrentSizeDistError) - (m_OldSizeDistError);
      if(change > 0.0f || m_CurrentSizeDistError > (1.0f - (float(iter) * 0.001f)) || curphasevol[j] < (0.75f * factor * curphasetotalvol))
      {
        if(gid % 100 == 0)
        {
          QString ss = QObject::tr("Packing Features (1/2) || Generating Feature #%1").arg(gid);
          notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
        }
        if(gid + 1 >= static_cast<int32_t>(m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumberOfTuples()))
        {
          tDims[0] = static_cast<size_t>(gid + 1);
          m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->resizeAttributeArrays(tDims);
          // need to update pointers after resize, buut do not need to run full data check because pointers are still valid
          updateFeatureInstancePointers();
        }

        transferAttributes(gid, &feature);
        m_OldSizeDistError = m_CurrentSizeDistError;
        curphasevol[j] = curphasevol[j] + m_Volumes[gid];
        iter = 0;
        gid++;
      }
      if(getCancel() == true)
      {
        return;
      }
    }
  }

  if(m_PeriodicBoundaries == false)
  {
    iter = 0;
    int32_t xfeatures = 0, yfeatures = 0, zfeatures = 0;
    xfeatures = static_cast<int32_t>(powf((m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumberOfTuples() * (m_SizeX / m_SizeY) * (m_SizeX / m_SizeZ)), (1.0f / 3.0f)) + 1);
    yfeatures = static_cast<int32_t>(xfeatures * (m_SizeY / m_SizeX) + 1);
    zfeatures = static_cast<int32_t>(xfeatures * (m_SizeZ / m_SizeX) + 1);
    factor = 0.25f * (1.0f - (float((xfeatures - 2) * (yfeatures - 2) * (zfeatures - 2)) / float(xfeatures * yfeatures * zfeatures)));
    for(size_t j = 0; j < numPrimaryPhases; ++j)
    {
      float curphasetotalvol = totalprimaryvol * m_PrimaryPhaseFractions[j];
      while(curphasevol[j] < ((1 + factor) * curphasetotalvol))
      {
        iter++;
        m_Seed++;
        phase = m_PrimaryPhases[j];
        generateFeature(phase, &feature, m_ShapeTypes[phase]);
        m_CurrentSizeDistError = checkSizeDistError(&feature);
        change = (m_CurrentSizeDistError) - (m_OldSizeDistError);
        if(change > 0 || m_CurrentSizeDistError > (1.0f - (iter * 0.001f)) || curphasevol[j] < (0.75f * factor * curphasetotalvol))
        {
          QString ss = QObject::tr("Packing Features (2/2) || Generating Feature #%1").arg(gid);
          notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
          if(gid + 1 >= static_cast<int32_t>(m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumberOfTuples()))
          {
            tDims[0] = static_cast<size_t>(gid + 1);
            m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->resizeAttributeArrays(tDims);
            // need to update pointers after resize, buut do not need to run full data check because pointers are still valid
            updateFeatureInstancePointers();
          }
          transferAttributes(gid, &feature);
          m_OldSizeDistError = m_CurrentSizeDistError;
          curphasevol[j] = curphasevol[j] + m_Volumes[gid];
          iter = 0;
          gid++;
        }
        if(getCancel() == true)
        {
          return;
        }
      }
    }
  }

  QString ss = QObject::tr("Packing Features || Starting Feature Placement...");
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

  tDims[0] = static_cast<size_t>(gid);
  m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->resizeAttributeArrays(tDims);
  totalFeatures = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumberOfTuples();
  // need to update pointers after resize, buut do not need to run full data check because pointers are still valid
  updateFeatureInstancePointers();

  if(getCancel() == true)
  {
    return;
  }

  // initialize the sim and goal neighbor distribution for the primary phases
  m_NeighborDist.resize(m_PrimaryPhases.size());
  m_SimNeighborDist.resize(m_PrimaryPhases.size());
  m_NeighborDistStep.resize(m_PrimaryPhases.size());
  for(size_t i = 0; i < numPrimaryPhases; i++)
  {
    phase = m_PrimaryPhases[i];
    PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
    m_NeighborDist[i].resize(pp->getBinNumbers()->getSize());
    m_SimNeighborDist[i].resize(pp->getBinNumbers()->getSize());
    VectorOfFloatArray Neighdist = pp->getFeatureSize_Neighbors();
    float normalizer = 0.0f;
    size_t numNeighborDistBins = m_NeighborDist[i].size();
    for(size_t j = 0; j < numNeighborDistBins; j++)
    {
      m_NeighborDist[i][j].resize(40);
      float input = 0.0f;
      float previoustotal = 0.0f;
      float avg = Neighdist[0]->getValue(j);
      float stdev = Neighdist[1]->getValue(j);
      m_NeighborDistStep[i] = 2.0f;
      float denominatorConst = 1.0f / sqrtf(2.0f * stdev * stdev); // Calculate it here rather than calculating the same thing multiple times below
      for(size_t k = 0; k < 40; k++)
      {
        input = (float(k + 1) * m_NeighborDistStep[i]);
        float logInput = logf(input);
        if(logInput <= avg)
        {
          m_NeighborDist[i][j][k] = 0.5f - 0.5f * (SIMPLibMath::erf((avg - logInput) * denominatorConst)) - previoustotal;
        }
        if(logInput > avg)
        {
          m_NeighborDist[i][j][k] = 0.5f + 0.5f * (SIMPLibMath::erf((logInput - avg) * denominatorConst)) - previoustotal;
        }
        previoustotal = previoustotal + m_NeighborDist[i][j][k];
      }
      normalizer = normalizer + previoustotal;
    }
    normalizer = 1.0f / normalizer;
    for(size_t j = 0; j < numNeighborDistBins; j++)
    {
      for(size_t k = 0; k < 40; k++)
      {
        m_NeighborDist[i][j][k] = m_NeighborDist[i][j][k] * normalizer;
      }
    }
  }

  if(getCancel() == true)
  {
    return;
  }

  m_ColumnList.resize(totalFeatures);
  m_RowList.resize(totalFeatures);
  m_PlaneList.resize(totalFeatures);
  m_EllipFuncList.resize(totalFeatures);
  m_PackQualities.resize(totalFeatures);
  m_FillingError = 1.0f;

  int64_t count = 0;
  int64_t column = 0, row = 0, plane = 0;
  int32_t progFeature = 0;
  int32_t progFeatureInc = static_cast<int32_t>(totalFeatures * 0.01f);
  for(size_t i = m_FirstPrimaryFeature; i < totalFeatures; i++)
  {
    if(getCancel() == true)
    {
      return;
    }

    if((int32_t)i > progFeature + progFeatureInc)
    {
      QString ss = QObject::tr("Placing Feature #%1/%2").arg(i).arg(totalFeatures);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      progFeature = i;
    }

    if(i == (totalFeatures - 1))
    {
      QString ss = QObject::tr("Placing Feature #%1/%2").arg(i + 1).arg(totalFeatures);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    }

    // we always put the feature in the center of the box to make sure the feature has the optimal chance to not touch the edge of the box
    // this is because in the insert function below we are just determining which voxels relative to the centroid are in the feature
    xc = m_SizeX * 0.5f;
    yc = m_SizeY * 0.5f;
    zc = m_SizeZ * 0.5f;
    m_Centroids[3 * i] = xc;
    m_Centroids[3 * i + 1] = yc;
    m_Centroids[3 * i + 2] = zc;
    insertFeature(i);
    if(getErrorCondition() < 0)
    {
      return;
    }
    count = 0;
    // now we randomly pick a place to try to place the feature
    xc = static_cast<float>(rg.genrand_res53() * m_SizeX);
    yc = static_cast<float>(rg.genrand_res53() * m_SizeY);
    zc = static_cast<float>(rg.genrand_res53() * m_SizeZ);
    column = static_cast<int64_t>((xc - (m_HalfPackingRes[0])) * m_OneOverPackingRes[0]);
    row = static_cast<int64_t>((yc - (m_HalfPackingRes[1])) * m_OneOverPackingRes[1]);
    plane = static_cast<int64_t>((zc - (m_HalfPackingRes[2])) * m_OneOverPackingRes[2]);
    featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + column;
    // now we walk til we find a point that is not in an exclusion zone
    while(exclusionOwners[featureOwnersIdx] > 0 && count < m_TotalPackingPoints)
    {
      featureOwnersIdx++;
      if(featureOwnersIdx >= m_TotalPackingPoints)
      {
        featureOwnersIdx = 0;
      }
      count++;
    }
    column = static_cast<int64_t>(featureOwnersIdx % m_PackingPoints[0]);
    row = static_cast<int64_t>(featureOwnersIdx / m_PackingPoints[0]) % m_PackingPoints[1];
    plane = static_cast<int64_t>(featureOwnersIdx / (m_PackingPoints[0] * m_PackingPoints[1]));
    xc = static_cast<float>((column * m_PackingRes[0]) + (m_PackingRes[0] * 0.5));
    yc = static_cast<float>((row * m_PackingRes[1]) + (m_PackingRes[1] * 0.5));
    zc = static_cast<float>((plane * m_PackingRes[2]) + (m_PackingRes[2] * 0.5));
    moveFeature(i, xc, yc, zc);
    m_FillingError = checkFillingError(i, -1000, featureOwnersPtr, exclusionOwnersPtr);
  }

  progFeature = 0;
  progFeatureInc = static_cast<int32_t>(totalFeatures * 0.01f);
  uint64_t millis = QDateTime::currentMSecsSinceEpoch();
  uint64_t currentMillis = millis;
  uint64_t startMillis = millis;
  uint64_t estimatedTime = 0;
  float timeDiff = 0.0f;

  // determine neighborhoods and initial neighbor distribution errors
  for(size_t i = m_FirstPrimaryFeature; i < totalFeatures; i++)
  {
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if(currentMillis - millis > 1000)
    {
      QString ss = QObject::tr("Determining Neighbors Feature %1/%2").arg(i).arg(totalFeatures);
      timeDiff = ((float)i / (float)(currentMillis - startMillis));
      estimatedTime = (float)(totalFeatures - i) / timeDiff;
      ss += QObject::tr(" || Est. Time Remain: %1 || Iterations/Sec: %2").arg(DREAM3D::convertMillisToHrsMinSecs(estimatedTime)).arg(timeDiff * 1000);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

      millis = QDateTime::currentMSecsSinceEpoch();
    }
    determineNeighbors(i, true);
  }
  m_OldNeighborhoodError = checkNeighborhoodError(-1000, -1000);

  // begin swaping/moving/adding/removing features to try to improve packing
  int32_t totalAdjustments = static_cast<int32_t>(100 * (totalFeatures - 1));

  // determine initial set of available points
  m_AvailablePointsCount = 0;
  for(int64_t i = 0; i < m_TotalPackingPoints; i++)
  {
    if((exclusionOwners[i] == 0 && m_UseMask == false) || (exclusionOwners[i] == 0 && m_UseMask == true && m_Mask[i] == true))
    {
      availablePoints[i] = m_AvailablePointsCount;
      availablePointsInv[m_AvailablePointsCount] = i;
      m_AvailablePointsCount++;
    }
  }

  // and clear the pointsToRemove and pointsToAdd vectors from the initial packing
  m_PointsToRemove.clear();
  m_PointsToAdd.clear();

  millis = QDateTime::currentMSecsSinceEpoch();
  startMillis = millis;
  bool good = false;
  size_t key = 0;
  float xshift = 0.0f, yshift = 0.0f, zshift = 0.0f;
  int32_t lastIteration = 0;
  for(int32_t iteration = 0; iteration < totalAdjustments; ++iteration)
  {
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if(currentMillis - millis > 1000)
    {
      QString ss = QObject::tr("Swapping/Moving/Adding/Removing Features Iteration %1/%2").arg(iteration).arg(totalAdjustments);
      timeDiff = ((float)iteration / (float)(currentMillis - startMillis));
      estimatedTime = (float)(totalAdjustments - iteration) / timeDiff;

      ss += QObject::tr(" || Est. Time Remain: %1 || Iterations/Sec: %2").arg(DREAM3D::convertMillisToHrsMinSecs(estimatedTime)).arg(timeDiff * 1000);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

      millis = QDateTime::currentMSecsSinceEpoch();
      lastIteration = iteration;
    }

    if(getCancel() == true)
    {
      return;
    }

    int32_t option = iteration % 2;

    if(writeErrorFile == true && iteration % 25 == 0)
    {
      outFile << iteration << " " << m_FillingError << "  " << availablePoints.size() << "  " << m_AvailablePointsCount << " " << totalFeatures << " " << acceptedmoves << "\n";
    }

    // JUMP - this option moves one feature to a random spot in the volume
    if(option == 0)
    {
      randomfeature = m_FirstPrimaryFeature + int32_t(rg.genrand_res53() * (totalFeatures - m_FirstPrimaryFeature));
      good = false;
      count = 0;
      while(good == false && count < static_cast<int32_t>((totalFeatures - m_FirstPrimaryFeature)))
      {
        xc = m_Centroids[3 * randomfeature];
        yc = m_Centroids[3 * randomfeature + 1];
        zc = m_Centroids[3 * randomfeature + 2];
        column = static_cast<int64_t>((xc - (m_HalfPackingRes[0])) * m_OneOverPackingRes[0]);
        row = static_cast<int64_t>((yc - (m_HalfPackingRes[1])) * m_OneOverPackingRes[1]);
        plane = static_cast<int64_t>((zc - (m_HalfPackingRes[2])) * m_OneOverPackingRes[2]);
        featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + column;
        if(featureOwners[featureOwnersIdx] > 1)
        {
          good = true;
        }
        else
        {
          randomfeature++;
        }
        if(static_cast<size_t>(randomfeature) >= totalFeatures)
        {
          randomfeature = m_FirstPrimaryFeature;
        }
        count++;
      }
      m_Seed++;

      if(availablePoints.size() > 0)
      {
        key = static_cast<size_t>(rg.genrand_res53() * (m_AvailablePointsCount - 1));
        featureOwnersIdx = availablePointsInv[key];
      }
      else
      {
        featureOwnersIdx = static_cast<size_t>(rg.genrand_res53() * m_TotalPackingPoints);
      }

      // find the column row and plane of that point
      column = static_cast<int64_t>(featureOwnersIdx % m_PackingPoints[0]);
      row = static_cast<int64_t>(featureOwnersIdx / m_PackingPoints[0]) % m_PackingPoints[1];
      plane = static_cast<int64_t>(featureOwnersIdx / (m_PackingPoints[0] * m_PackingPoints[1]));
      xc = static_cast<float>((column * m_PackingRes[0]) + (m_PackingRes[0] * 0.5));
      yc = static_cast<float>((row * m_PackingRes[1]) + (m_PackingRes[1] * 0.5));
      zc = static_cast<float>((plane * m_PackingRes[2]) + (m_PackingRes[2] * 0.5));
      oldxc = m_Centroids[3 * randomfeature];
      oldyc = m_Centroids[3 * randomfeature + 1];
      oldzc = m_Centroids[3 * randomfeature + 2];
      m_OldFillingError = m_FillingError;
      m_FillingError = checkFillingError(-1000, static_cast<int32_t>(randomfeature), featureOwnersPtr, exclusionOwnersPtr);
      moveFeature(randomfeature, xc, yc, zc);
      m_FillingError = checkFillingError(static_cast<int32_t>(randomfeature), -1000, featureOwnersPtr, exclusionOwnersPtr);
      m_CurrentNeighborhoodError = checkNeighborhoodError(-1000, randomfeature);
      if(m_FillingError <= m_OldFillingError)
      {
        m_OldNeighborhoodError = m_CurrentNeighborhoodError;
        updateAvailablePoints(availablePoints, availablePointsInv);
        acceptedmoves++;
      }
      else if(m_FillingError > m_OldFillingError)
      {
        m_FillingError = checkFillingError(-1000, static_cast<int32_t>(randomfeature), featureOwnersPtr, exclusionOwnersPtr);
        moveFeature(randomfeature, oldxc, oldyc, oldzc);
        m_FillingError = checkFillingError(static_cast<int32_t>(randomfeature), -1000, featureOwnersPtr, exclusionOwnersPtr);
        m_PointsToRemove.clear();
        m_PointsToAdd.clear();
      }
    }

    // NUDGE - this option moves one feature to a spot close to its current centroid
    if(option == 1)
    {
      randomfeature = m_FirstPrimaryFeature + int32_t(rg.genrand_res53() * (totalFeatures - m_FirstPrimaryFeature));
      good = false;
      count = 0;
      while(good == false && count < static_cast<int32_t>((totalFeatures - m_FirstPrimaryFeature)))
      {
        xc = m_Centroids[3 * randomfeature];
        yc = m_Centroids[3 * randomfeature + 1];
        zc = m_Centroids[3 * randomfeature + 2];
        column = static_cast<int64_t>((xc - (m_HalfPackingRes[0])) * m_OneOverPackingRes[0]);
        row = static_cast<int64_t>((yc - (m_HalfPackingRes[1])) * m_OneOverPackingRes[1]);
        plane = static_cast<int64_t>((zc - (m_HalfPackingRes[2])) * m_OneOverPackingRes[2]);
        featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + column;
        if(featureOwners[featureOwnersIdx] > 1)
        {
          good = true;
        }
        else
        {
          randomfeature++;
        }
        if(static_cast<size_t>(randomfeature) >= totalFeatures)
        {
          randomfeature = m_FirstPrimaryFeature;
        }
        count++;
      }
      m_Seed++;
      oldxc = m_Centroids[3 * randomfeature];
      oldyc = m_Centroids[3 * randomfeature + 1];
      oldzc = m_Centroids[3 * randomfeature + 2];
      xshift = static_cast<float>(((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * m_PackingRes[0])));
      yshift = static_cast<float>(((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * m_PackingRes[1])));
      zshift = static_cast<float>(((2.0f * (rg.genrand_res53() - 0.5f)) * (2.0f * m_PackingRes[2])));
      if((oldxc + xshift) < m_SizeX && (oldxc + xshift) > 0)
      {
        xc = oldxc + xshift;
      }
      else
      {
        xc = oldxc;
      }
      if((oldyc + yshift) < m_SizeY && (oldyc + yshift) > 0)
      {
        yc = oldyc + yshift;
      }
      else
      {
        yc = oldyc;
      }
      if((oldzc + zshift) < m_SizeZ && (oldzc + zshift) > 0)
      {
        zc = oldzc + zshift;
      }
      else
      {
        zc = oldzc;
      }
      m_OldFillingError = m_FillingError;
      m_FillingError = checkFillingError(-1000, static_cast<int32_t>(randomfeature), featureOwnersPtr, exclusionOwnersPtr);
      moveFeature(randomfeature, xc, yc, zc);
      m_FillingError = checkFillingError(static_cast<int32_t>(randomfeature), -1000, featureOwnersPtr, exclusionOwnersPtr);
      m_CurrentNeighborhoodError = checkNeighborhoodError(-1000, randomfeature);
      //      change2 = (currentneighborhooderror * currentneighborhooderror) - (oldneighborhooderror * oldneighborhooderror);
      //      if(fillingerror <= oldfillingerror && currentneighborhooderror >= oldneighborhooderror)
      if(m_FillingError <= m_OldFillingError)
      {
        m_OldNeighborhoodError = m_CurrentNeighborhoodError;
        updateAvailablePoints(availablePoints, availablePointsInv);
        acceptedmoves++;
      }
      //      else if(fillingerror > oldfillingerror || currentneighborhooderror < oldneighborhooderror)
      else if(m_FillingError > m_OldFillingError)
      {
        m_FillingError = checkFillingError(-1000, static_cast<int>(randomfeature), featureOwnersPtr, exclusionOwnersPtr);
        moveFeature(randomfeature, oldxc, oldyc, oldzc);
        m_FillingError = checkFillingError(static_cast<int>(randomfeature), -1000, featureOwnersPtr, exclusionOwnersPtr);
        m_PointsToRemove.clear();
        m_PointsToAdd.clear();
      }
    }
  }

  if(m_VtkOutputFile.isEmpty() == false)
  {
    int32_t err = writeVtkFile(featureOwnersPtr->getPointer(0), exclusionOwnersPtr->getPointer(0));
    if(err < 0)
    {
      QString ss = QObject::tr("Error writing Vtk file");
      setErrorCondition(-78008);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Int32ArrayType::Pointer PackPrimaryPhases::initializePackingGrid()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixPath().getDataContainerName());

  m_PackingRes[0] = m->getGeometryAs<ImageGeom>()->getXRes() * 2.0f;
  m_PackingRes[1] = m->getGeometryAs<ImageGeom>()->getYRes() * 2.0f;
  m_PackingRes[2] = m->getGeometryAs<ImageGeom>()->getZRes() * 2.0f;

  m_HalfPackingRes[0] = m_PackingRes[0] * 0.5f;
  m_HalfPackingRes[1] = m_PackingRes[1] * 0.5f;
  m_HalfPackingRes[2] = m_PackingRes[2] * 0.5f;

  m_OneOverHalfPackingRes[0] = 1.0f / m_HalfPackingRes[0];
  m_OneOverHalfPackingRes[1] = 1.0f / m_HalfPackingRes[1];
  m_OneOverHalfPackingRes[2] = 1.0f / m_HalfPackingRes[2];

  m_OneOverPackingRes[0] = 1.0f / m_PackingRes[0];
  m_OneOverPackingRes[1] = 1.0f / m_PackingRes[1];
  m_OneOverPackingRes[2] = 1.0f / m_PackingRes[2];

  m_PackingPoints[0] = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getXPoints() / 2);
  m_PackingPoints[1] = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getYPoints() / 2);
  m_PackingPoints[2] = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getZPoints() / 2);

  for(auto&& packingValue : m_PackingPoints)
  {
    if(packingValue == 0)
    {
      packingValue = 1;
    }
  }

  m_TotalPackingPoints = m_PackingPoints[0] * m_PackingPoints[1] * m_PackingPoints[2];

  Int32ArrayType::Pointer featureOwnersPtr = Int32ArrayType::CreateArray(m_TotalPackingPoints, "_INTERNAL_USE_ONLY_PackPrimaryFeatures::feature_owners");
  featureOwnersPtr->initializeWithZeros();

  return featureOwnersPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::generateFeature(int32_t phase, Feature_t* feature, uint32_t shapeclass)
{
  SIMPL_RANDOMNG_NEW_SEEDED(m_Seed)

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock().get());

  float r1 = 1.0f;
  float a2 = 0.0f, a3 = 0.0f;
  float b2 = 0.0f, b3 = 0.0f;
  float diam = 0.0f;
  float vol = 0.0f;
  bool volgood = false;
  float fourThirdsPiOverEight = static_cast<float>(((4.0f / 3.0f) * (SIMPLib::Constants::k_Pi)) / 8.0f);
  PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
  VectorOfFloatArray GSdist = pp->getFeatureSizeDistribution();
  float avg = GSdist[0]->getValue(0);
  float stdev = GSdist[1]->getValue(0);
  while(volgood == false)
  {
    volgood = true;
    diam = static_cast<float>(rg.genrand_norm(avg, stdev));
    diam = expf(diam);
    if(diam >= pp->getMaxFeatureDiameter())
    {
      volgood = false;
    }
    if(diam < pp->getMinFeatureDiameter())
    {
      volgood = false;
    }
    vol = fourThirdsPiOverEight * (diam * diam * diam);
  }
  int32_t diameter = int32_t((diam - pp->getMinFeatureDiameter()) / pp->getBinStepSize());
  float r2 = 0.0f, r3 = 1.0f;
  VectorOfFloatArray bovera = pp->getFeatureSize_BOverA();
  VectorOfFloatArray covera = pp->getFeatureSize_COverA();
  if(diameter >= static_cast<int32_t>(bovera[0]->getSize()))
  {
    diameter = static_cast<int32_t>(bovera[0]->getSize()) - 1;
  }
  while(r2 < r3)
  {
    r2 = 0.0f, r3 = 0.0f;
    a2 = bovera[0]->getValue(diameter);
    b2 = bovera[1]->getValue(diameter);
    a3 = covera[0]->getValue(diameter);
    b3 = covera[1]->getValue(diameter);
    int32_t tmpDiameter = diameter;
    int32_t increment = -1;
    while(a2 == 0 || b2 == 0 || a3 == 0 || b3 == 0)
    {
      tmpDiameter += increment;
      if(tmpDiameter < 0)
      {
        tmpDiameter = diameter + 1;
        increment = 1;
      }
      if(tmpDiameter >= static_cast<int32_t>(bovera[0]->getSize()))
      {
        a2 = 1.0f;
        b2 = 0.0f;
        a3 = 1.0f;
        b3 = 0.0f;
        break;
      }
      a2 = bovera[0]->getValue(tmpDiameter);
      b2 = bovera[1]->getValue(tmpDiameter);
      a3 = covera[0]->getValue(tmpDiameter);
      b3 = covera[1]->getValue(tmpDiameter);
    }
    r2 = static_cast<float>(rg.genrand_beta(a2, b2));
    r3 = static_cast<float>(rg.genrand_beta(a3, b3));
  }
  FloatArrayType::Pointer axisodf = pp->getAxisOrientation();
  int32_t numbins = axisodf->getNumberOfTuples();
  float random = static_cast<float>(rg.genrand_res53());
  float totaldensity = 0.0f;
  int32_t bin = 0;
  for(int32_t j = 0; j < numbins; j++)
  {
    float density = axisodf->getValue(j);
    float td1 = totaldensity;
    totaldensity = totaldensity + density;
    if(random < totaldensity && random >= td1)
    {
      bin = j;
      break;
    }
  }
  FOrientArrayType eulers = m_OrthoOps->determineEulerAngles(m_Seed, bin);
  VectorOfFloatArray omega3 = pp->getFeatureSize_Omegas();
  float mf = omega3[0]->getValue(diameter);
  float s = omega3[1]->getValue(diameter);
  float omega3f = static_cast<float>(rg.genrand_beta(mf, s));
  if(shapeclass == static_cast<ShapeType::EnumType>(ShapeType::Type::Ellipsoid))
  {
    omega3f = 1;
  }
  feature->m_Volumes = vol;
  feature->m_EquivalentDiameters = diam;
  feature->m_AxisLengths[0] = r1;
  feature->m_AxisLengths[1] = r2;
  feature->m_AxisLengths[2] = r3;
  feature->m_AxisEulerAngles[0] = eulers[0];
  feature->m_AxisEulerAngles[1] = eulers[1];
  feature->m_AxisEulerAngles[2] = eulers[2];
  feature->m_Omega3s = omega3f;
  feature->m_FeaturePhases = phase;
  feature->m_Neighborhoods = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::transferAttributes(int32_t gnum, Feature_t* feature)
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
void PackPrimaryPhases::moveFeature(size_t gnum, float xc, float yc, float zc)
{
  int64_t occolumn = 0, ocrow = 0, ocplane = 0;
  int64_t nccolumn = 0, ncrow = 0, ncplane = 0;
  int64_t shiftcolumn = 0, shiftrow = 0, shiftplane = 0;
  float oxc = m_Centroids[3 * gnum];
  float oyc = m_Centroids[3 * gnum + 1];
  float ozc = m_Centroids[3 * gnum + 2];
  occolumn = static_cast<int64_t>((oxc - (m_HalfPackingRes[0])) * m_OneOverPackingRes[0]);
  ocrow = static_cast<int64_t>((oyc - (m_HalfPackingRes[1])) * m_OneOverPackingRes[1]);
  ocplane = static_cast<int64_t>((ozc - (m_HalfPackingRes[2])) * m_OneOverPackingRes[2]);
  nccolumn = static_cast<int64_t>((xc - (m_HalfPackingRes[0])) * m_OneOverPackingRes[0]);
  ncrow = static_cast<int64_t>((yc - (m_HalfPackingRes[1])) * m_OneOverPackingRes[1]);
  ncplane = static_cast<int64_t>((zc - (m_HalfPackingRes[2])) * m_OneOverPackingRes[2]);
  shiftcolumn = nccolumn - occolumn;
  shiftrow = ncrow - ocrow;
  shiftplane = ncplane - ocplane;
  m_Centroids[3 * gnum] = xc;
  m_Centroids[3 * gnum + 1] = yc;
  m_Centroids[3 * gnum + 2] = zc;
  size_t size = m_ColumnList[gnum].size();

  for(size_t i = 0; i < size; i++)
  {
    int64_t& cl = m_ColumnList[gnum][i];
    cl += shiftcolumn;
    int64_t& rl = m_RowList[gnum][i];
    rl += shiftrow;
    int64_t& pl = m_PlaneList[gnum][i];
    pl += shiftplane;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::determineNeighbors(size_t gnum, bool add)
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixPath().getDataContainerName());

  float x = 0.0f, y = 0.0f, z = 0.0f;
  float xn = 0.0f, yn = 0.0f, zn = 0.0f;
  float dia = 0.0f, dia2 = 0.0f;
  float dx = 0.0f, dy = 0.0f, dz = 0.0f;
  x = m_Centroids[3 * gnum];
  y = m_Centroids[3 * gnum + 1];
  z = m_Centroids[3 * gnum + 2];
  dia = m_EquivalentDiameters[gnum];
  size_t totalFeatures = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumberOfTuples();
  int32_t increment = 0;
  if(add == true)
  {
    increment = 1;
  }
  if(add == false)
  {
    increment = -1;
  }
  for(size_t n = m_FirstPrimaryFeature; n < totalFeatures; n++)
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
float PackPrimaryPhases::checkNeighborhoodError(int32_t gadd, int32_t gremove)
{
  // Optimized Code
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixPath().getDataContainerName());

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock().get());

  float neighborerror = 0.0f;
  float bhattdist = 0.0f;
  float dia = 0.0f;
  int32_t nnum = 0;
  size_t diabin = 0;
  size_t nnumbin = 0;
  int32_t index = 0;
  int32_t phase = 0;

  typedef std::vector<std::vector<float>> VectOfVectFloat_t;
  size_t numPhases = m_SimNeighborDist.size();
  for(size_t iter = 0; iter < numPhases; ++iter)
  {
    phase = m_PrimaryPhases[iter];
    PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
    VectOfVectFloat_t& curSimNeighborDist = m_SimNeighborDist[iter];
    size_t curSImNeighborDist_Size = curSimNeighborDist.size();
    float oneOverNeighborDistStep = 1.0f / m_NeighborDistStep[iter];

    std::vector<int32_t> count(curSImNeighborDist_Size, 0);
    for(size_t i = 0; i < curSImNeighborDist_Size; i++)
    {
      curSimNeighborDist[i].resize(40);
      for(size_t j = 0; j < 40; j++)
      {
        curSimNeighborDist[i][j] = 0;
      }
    }
    if(gadd > 0 && m_FeaturePhases[gadd] == phase)
    {
      determineNeighbors(gadd, true);
    }
    if(gremove > 0 && m_FeaturePhases[gremove] == phase)
    {
      determineNeighbors(gremove, false);
    }

    float maxFeatureDia = pp->getMaxFeatureDiameter();
    float minFeatureDia = pp->getMinFeatureDiameter();
    float oneOverBinStepSize = 1.0f / pp->getBinStepSize();

    size_t totalFeatures = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumberOfTuples();
    for(size_t i = m_FirstPrimaryFeature; i < totalFeatures; i++)
    {
      nnum = 0;
      index = static_cast<int32_t>(i);
      if(index != gremove && m_FeaturePhases[index] == phase)
      {
        dia = m_EquivalentDiameters[index];
        if(dia > maxFeatureDia)
        {
          dia = maxFeatureDia;
        }
        if(dia < minFeatureDia)
        {
          dia = minFeatureDia;
        }
        diabin = static_cast<size_t>(((dia - minFeatureDia) * oneOverBinStepSize));
        if(diabin >= curSImNeighborDist_Size)
        {
          diabin = curSImNeighborDist_Size - 1;
        }
        nnum = m_Neighborhoods[index];
        nnumbin = static_cast<size_t>(nnum * oneOverNeighborDistStep);
        if(nnumbin >= 40)
        {
          nnumbin = 39;
        }
        curSimNeighborDist[diabin][nnumbin]++;
        count[diabin]++;
      }
    }
    if(gadd > 0 && m_FeaturePhases[gadd] == phase)
    {
      dia = m_EquivalentDiameters[gadd];
      if(dia > maxFeatureDia)
      {
        dia = maxFeatureDia;
      }
      if(dia < minFeatureDia)
      {
        dia = minFeatureDia;
      }
      diabin = static_cast<size_t>(((dia - minFeatureDia) * oneOverBinStepSize));
      if(diabin >= curSImNeighborDist_Size)
      {
        diabin = curSImNeighborDist_Size - 1;
      }
      nnum = m_Neighborhoods[gadd];
      nnumbin = static_cast<size_t>(nnum * oneOverNeighborDistStep);
      if(nnumbin >= 40)
      {
        nnumbin = 39;
      }
      curSimNeighborDist[diabin][nnumbin]++;
      count[diabin]++;
    }
    float runningtotal = 0.0f;

    for(size_t i = 0; i < curSImNeighborDist_Size; i++)
    {
      if(count[i] == 0)
      {
        for(size_t j = 0; j < 40; j++)
        {
          curSimNeighborDist[i][j] = 0.0f;
        }
      }
      else
      {
        float oneOverCount = 1.0f / (float)(count[i]);
        for(size_t j = 0; j < 40; j++)
        {
          curSimNeighborDist[i][j] = curSimNeighborDist[i][j] * oneOverCount;
          runningtotal = runningtotal + curSimNeighborDist[i][j];
        }
      }
    }

    runningtotal = 1.0f / runningtotal; // Flip this so that we have a multiply instead of a divide.
    for(size_t i = 0; i < curSImNeighborDist_Size; i++)
    {
      for(size_t j = 0; j < 40; j++)
      {
        curSimNeighborDist[i][j] = curSimNeighborDist[i][j] * runningtotal;
      }
    }

    if(gadd > 0 && m_FeaturePhases[gadd] == phase)
    {
      determineNeighbors(gadd, false);
    }

    if(gremove > 0 && m_FeaturePhases[gremove] == phase)
    {
      determineNeighbors(gremove, true);
    }
  }
  compare3dDistributions(m_SimNeighborDist, m_NeighborDist, bhattdist);
  neighborerror = bhattdist;
  return neighborerror;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::compare1dDistributions(std::vector<float> array1, std::vector<float> array2, float& bhattdist)
{
  bhattdist = 0.0f;
  size_t array1Size = array1.size();
  for(size_t i = 0; i < array1Size; i++)
  {
    bhattdist = bhattdist + sqrtf((array1[i] * array2[i]));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::compare2dDistributions(std::vector<std::vector<float>> array1, std::vector<std::vector<float>> array2, float& bhattdist)
{
  bhattdist = 0.0f;
  size_t array1Size = array1.size();
  for(size_t i = 0; i < array1Size; i++)
  {
    size_t array2Size = array1[i].size();
    for(size_t j = 0; j < array2Size; j++)
    {
      bhattdist = bhattdist + sqrtf((array1[i][j] * array2[i][j]));
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::compare3dDistributions(std::vector<std::vector<std::vector<float>>> array1, std::vector<std::vector<std::vector<float>>> array2, float& bhattdist)
{
  bhattdist = 0.0f;
  size_t array1Size = array1.size();
  for(size_t i = 0; i < array1Size; i++)
  {
    size_t array2Size = array1[i].size();
    for(size_t j = 0; j < array2Size; j++)
    {
      size_t array3Size = array1[i][j].size();
      for(size_t k = 0; k < array3Size; k++)
      {
        bhattdist = bhattdist + sqrtf((array1[i][j][k] * array2[i][j][k]));
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float PackPrimaryPhases::checkSizeDistError(Feature_t* feature)
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixPath().getDataContainerName());

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock().get());

  float dia = 0.0f;
  float sizedisterror = 0.0f;
  float bhattdist = 0.0f;
  int32_t index = 0;
  int32_t count = 0;
  int32_t phase = 0;
  size_t featureSizeDist_Size = m_FeatureSizeDist.size();
  for(size_t iter = 0; iter < featureSizeDist_Size; ++iter)
  {
    phase = m_PrimaryPhases[iter];
    PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
    count = 0;
    std::vector<float>& curFeatureSizeDist = m_FeatureSizeDist[iter];
    std::vector<float>::size_type curFeatureSizeDistSize = curFeatureSizeDist.size();
    std::vector<float>& curSimFeatureSizeDist = m_SimFeatureSizeDist[iter];
    // Initialize all Values to Zero
    for(size_t i = 0; i < curFeatureSizeDistSize; i++)
    {
      curSimFeatureSizeDist[i] = 0.0f;
    }
    int64_t totalFeatures = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumberOfTuples();
    float oneOverCurFeatureSizeDistStep = 1.0f / m_FeatureSizeDistStep[iter];
    float halfMinFeatureDiameter = pp->getMinFeatureDiameter() * 0.5f;
    for(int32_t b = m_FirstPrimaryFeature; b < totalFeatures; b++)
    {
      index = b;
      if(m_FeaturePhases[index] == phase)
      {
        dia = m_EquivalentDiameters[index];
        dia = (dia - halfMinFeatureDiameter) * oneOverCurFeatureSizeDistStep;
        if(dia < 0)
        {
          dia = 0.0f;
        }
        if(dia > curFeatureSizeDistSize - 1.0f)
        {
          dia = curFeatureSizeDistSize - 1.0f;
        }
        curSimFeatureSizeDist[int32_t(dia)]++;
        count++;
      }
    }

    if(feature->m_FeaturePhases == phase)
    {
      dia = feature->m_EquivalentDiameters;
      dia = (dia - halfMinFeatureDiameter) * oneOverCurFeatureSizeDistStep;
      if(dia < 0)
      {
        dia = 0.0f;
      }
      if(dia > curFeatureSizeDistSize - 1.0f)
      {
        dia = curFeatureSizeDistSize - 1.0f;
      }
      curSimFeatureSizeDist[int32_t(dia)]++;
      count++;
    }
    float oneOverCount = 1.0f / count;

    if(count == 0)
    {
      for(size_t i = 0; i < curFeatureSizeDistSize; i++)
      {
        curSimFeatureSizeDist[i] = 0.0;
      }
    }
    else
    {
      for(size_t i = 0; i < curFeatureSizeDistSize; i++)
      {
        curSimFeatureSizeDist[i] = curSimFeatureSizeDist[i] * oneOverCount;
      }
    }
  }
  compare2dDistributions(m_SimFeatureSizeDist, m_FeatureSizeDist, bhattdist);
  sizedisterror = bhattdist;
  return sizedisterror;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float PackPrimaryPhases::checkFillingError(int32_t gadd, int32_t gremove, Int32ArrayType::Pointer featureOwnersPtr, Int32ArrayType::Pointer exclusionOwnersPtr)
{
  size_t featureOwnersIdx = 0;
  int32_t* featureOwners = featureOwnersPtr->getPointer(0);
  int32_t* exclusionOwners = exclusionOwnersPtr->getPointer(0);

  m_FillingError = m_FillingError * float(m_TotalPackingPoints);
  int64_t col = 0, row = 0, plane = 0;
  int32_t k1 = 0, k2 = 0, k3 = 0;
  if(gadd > 0)
  {
    k1 = 2;
    k2 = -1;
    k3 = 1;
    size_t numVoxelsForCurrentGrain = m_ColumnList[gadd].size();
    std::vector<int64_t>& cl = m_ColumnList[gadd];
    std::vector<int64_t>& rl = m_RowList[gadd];
    std::vector<int64_t>& pl = m_PlaneList[gadd];
    std::vector<float>& efl = m_EllipFuncList[gadd];
    float packquality = 0;
    for(size_t i = 0; i < numVoxelsForCurrentGrain; i++)
    {
      col = cl[i];
      row = rl[i];
      plane = pl[i];
      if(m_PeriodicBoundaries == true)
      {
        // Perform mod arithmetic to ensure we are within the packing points range
        col = col % m_PackingPoints[0];
        row = row % m_PackingPoints[1];
        plane = plane % m_PackingPoints[2];

        if(col < 0)
        {
          col = col + m_PackingPoints[0];
        }
        if(col > m_PackingPoints[0] - 1)
        {
          col = col - m_PackingPoints[0];
        }
        if(row < 0)
        {
          row = row + m_PackingPoints[1];
        }
        if(row > m_PackingPoints[1] - 1)
        {
          row = row - m_PackingPoints[1];
        }
        if(plane < 0)
        {
          plane = plane + m_PackingPoints[2];
        }
        if(plane > m_PackingPoints[2] - 1)
        {
          plane = plane - m_PackingPoints[2];
        }
        featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + col;
        int32_t currentFeatureOwner = featureOwners[featureOwnersIdx];
        if(efl[i] > 0.1f)
        {
          if(exclusionOwners[featureOwnersIdx] == 0)
          {
            m_PointsToRemove.push_back(featureOwnersIdx);
          }
          exclusionOwners[featureOwnersIdx]++;
        }
        m_FillingError = static_cast<float>(m_FillingError + ((k1 * currentFeatureOwner + k2)));
        //        fillingerror = fillingerror + (multiplier * (k1 * currentFeatureOwner  + k2));
        featureOwners[featureOwnersIdx] = currentFeatureOwner + k3;
        packquality = static_cast<float>(packquality + ((currentFeatureOwner) * (currentFeatureOwner)));
      }
      else
      {
        if(col >= 0 && col < m_PackingPoints[0] && row >= 0 && row < m_PackingPoints[1] && plane >= 0 && plane < m_PackingPoints[2])
        {
          featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + col;
          int32_t currentFeatureOwner = featureOwners[featureOwnersIdx];
          if(exclusionOwners[featureOwnersIdx] > 0)
          {
          }
          if(efl[i] > 0.1f)
          {
            if(exclusionOwners[featureOwnersIdx] == 0)
            {
              m_PointsToRemove.push_back(featureOwnersIdx);
            }
            exclusionOwners[featureOwnersIdx]++;
          }
          m_FillingError = static_cast<float>(m_FillingError + ((k1 * currentFeatureOwner + k2)));
          //        fillingerror = fillingerror + (multiplier * (k1 * currentFeatureOwner  + k2));
          featureOwners[featureOwnersIdx] = currentFeatureOwner + k3;
          packquality = static_cast<float>(packquality + ((currentFeatureOwner) * (currentFeatureOwner)));
        }
      }
    }
    m_PackQualities[gadd] = static_cast<int64_t>(packquality / float(numVoxelsForCurrentGrain));
  }
  if(gremove > 0)
  {
    k1 = -2;
    k2 = 3;
    k3 = -1;
    size_t size = m_ColumnList[gremove].size();
    std::vector<int64_t>& cl = m_ColumnList[gremove];
    std::vector<int64_t>& rl = m_RowList[gremove];
    std::vector<int64_t>& pl = m_PlaneList[gremove];
    std::vector<float>& efl = m_EllipFuncList[gremove];
    for(size_t i = 0; i < size; i++)
    {
      col = cl[i];
      row = rl[i];
      plane = pl[i];
      if(m_PeriodicBoundaries == true)
      {
        // Perform mod arithmetic to ensure we are within the packing points range
        col = col % m_PackingPoints[0];
        row = row % m_PackingPoints[1];
        plane = plane % m_PackingPoints[2];

        if(col < 0)
        {
          col = col + m_PackingPoints[0];
        }
        if(col > m_PackingPoints[0] - 1)
        {
          col = col - m_PackingPoints[0];
        }
        if(row < 0)
        {
          row = row + m_PackingPoints[1];
        }
        if(row > m_PackingPoints[1] - 1)
        {
          row = row - m_PackingPoints[1];
        }
        if(plane < 0)
        {
          plane = plane + m_PackingPoints[2];
        }
        if(plane > m_PackingPoints[2] - 1)
        {
          plane = plane - m_PackingPoints[2];
        }
        featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + col;
        int32_t currentFeatureOwner = featureOwners[featureOwnersIdx];
        if(efl[i] > 0.1f)
        {
          exclusionOwners[featureOwnersIdx]--;
          if(exclusionOwners[featureOwnersIdx] == 0)
          {
            m_PointsToAdd.push_back(featureOwnersIdx);
          }
        }
        m_FillingError = static_cast<float>(m_FillingError + ((k1 * currentFeatureOwner + k2)));
        //        fillingerror = fillingerror + (multiplier * (k1 * currentFeatureOwner  + k2));
        featureOwners[featureOwnersIdx] = currentFeatureOwner + k3;
      }
      else
      {
        if(col >= 0 && col < m_PackingPoints[0] && row >= 0 && row < m_PackingPoints[1] && plane >= 0 && plane < m_PackingPoints[2])
        {
          featureOwnersIdx = (m_PackingPoints[0] * m_PackingPoints[1] * plane) + (m_PackingPoints[0] * row) + col;
          int32_t currentFeatureOwner = featureOwners[featureOwnersIdx];
          if(efl[i] > 0.1f)
          {
            exclusionOwners[featureOwnersIdx]--;
            if(exclusionOwners[featureOwnersIdx] == 0)
            {
              m_PointsToAdd.push_back(featureOwnersIdx);
            }
          }
          m_FillingError = static_cast<float>(m_FillingError + ((k1 * currentFeatureOwner + k2)));
          //          fillingerror = fillingerror + (multiplier * (k1 * currentFeatureOwner  + k2));
          featureOwners[featureOwnersIdx] = currentFeatureOwner + k3;
        }
      }
    }
  }
  m_FillingError = m_FillingError / float(m_TotalPackingPoints);
  return m_FillingError;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::updateAvailablePoints(std::map<size_t, size_t>& availablePoints, std::map<size_t, size_t>& availablePointsInv)
{
  size_t removeSize = m_PointsToRemove.size();
  size_t addSize = m_PointsToAdd.size();
  size_t featureOwnersIdx = 0;
  size_t key = 0, val = 0;
  for(size_t i = 0; i < removeSize; i++)
  {
    featureOwnersIdx = m_PointsToRemove[i];
    key = availablePoints[featureOwnersIdx];
    //  availablePoints.erase(featureOwnersIdx);
    val = availablePointsInv[m_AvailablePointsCount - 1];
    //  availablePointsInv.erase(availablePointsCount-1);
    if(key < m_AvailablePointsCount - 1)
    {
      availablePointsInv[key] = val;
      availablePoints[val] = key;
    }
    m_AvailablePointsCount--;
  }
  for(size_t i = 0; i < addSize; i++)
  {
    featureOwnersIdx = m_PointsToAdd[i];
    availablePoints[featureOwnersIdx] = m_AvailablePointsCount;
    availablePointsInv[m_AvailablePointsCount] = featureOwnersIdx;
    m_AvailablePointsCount++;
  }
  m_PointsToRemove.clear();
  m_PointsToAdd.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::insertFeature(size_t gnum)
{
  SIMPL_RANDOMNG_NEW();

  float inside = -1.0f;
  int64_t column = 0, row = 0, plane = 0;
  int64_t centercolumn = 0, centerrow = 0, centerplane = 0;
  int64_t xmin = 0, xmax = 0, ymin = 0, ymax = 0, zmin = 0, zmax = 0;
  float xc = 0.0f, yc = 0.0f, zc = 0.0f;
  float coordsRotated[3] = {0.0f, 0.0f, 0.0f};
  float coords[3] = {0.0f, 0.0f, 0.0f};
  float volcur = m_Volumes[gnum];
  float bovera = m_AxisLengths[3 * gnum + 1];
  float covera = m_AxisLengths[3 * gnum + 2];
  float omega3 = m_Omega3s[gnum];
  float radcur1 = 1.0f;
  uint32_t shapeclass = m_ShapeTypes[m_FeaturePhases[gnum]];

  // Bail if the shapeclass is not one of our enumerated types
  if(shapeclass >= static_cast<ShapeType::EnumType>(ShapeType::Type::ShapeTypeEnd))
  {
    QString ss = QObject::tr("Undefined shape class in shape types array with path %1").arg(m_InputShapeTypesArrayPath.serialize());
    setErrorCondition(-78009);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  // init any values for each of the Shape Ops
  for(size_t iter = 0; iter < m_ShapeOps.size(); iter++)
  {
    m_ShapeOps[iter]->init();
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
  float ga[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  FOrientArrayType om(9, 0.0);
  FOrientTransformsType::eu2om(FOrientArrayType(phi1, PHI, phi2), om);
  om.toGMatrix(ga);

  xc = m_Centroids[3 * gnum];
  yc = m_Centroids[3 * gnum + 1];
  zc = m_Centroids[3 * gnum + 2];
  centercolumn = static_cast<int64_t>((xc - (m_PackingRes[0] * 0.5f)) * m_OneOverPackingRes[0]);
  centerrow = static_cast<int64_t>((yc - (m_PackingRes[1] * 0.5f)) * m_OneOverPackingRes[1]);
  centerplane = static_cast<int64_t>((zc - (m_PackingRes[2] * 0.5f)) * m_OneOverPackingRes[2]);
  xmin = static_cast<int64_t>(centercolumn - ((radcur1 * m_OneOverPackingRes[0]) + 1));
  xmax = static_cast<int64_t>(centercolumn + ((radcur1 * m_OneOverPackingRes[0]) + 1));
  ymin = static_cast<int64_t>(centerrow - ((radcur1 * m_OneOverPackingRes[1]) + 1));
  ymax = static_cast<int64_t>(centerrow + ((radcur1 * m_OneOverPackingRes[1]) + 1));
  zmin = static_cast<int64_t>(centerplane - ((radcur1 * m_OneOverPackingRes[2]) + 1));
  zmax = static_cast<int64_t>(centerplane + ((radcur1 * m_OneOverPackingRes[2]) + 1));
  if(xmin < -m_PackingPoints[0])
  {
    xmin = -m_PackingPoints[0];
  }
  if(xmax > 2 * m_PackingPoints[0] - 1)
  {
    xmax = (2 * m_PackingPoints[0] - 1);
  }
  if(ymin < -m_PackingPoints[1])
  {
    ymin = -m_PackingPoints[1];
  }
  if(ymax > 2 * m_PackingPoints[1] - 1)
  {
    ymax = (2 * m_PackingPoints[1] - 1);
  }
  if(zmin < -m_PackingPoints[2])
  {
    zmin = -m_PackingPoints[2];
  }
  if(zmax > 2 * m_PackingPoints[2] - 1)
  {
    zmax = (2 * m_PackingPoints[2] - 1);
  }

  float OneOverRadcur1 = 1.0f / radcur1;
  float OneOverRadcur2 = 1.0f / radcur2;
  float OneOverRadcur3 = 1.0f / radcur3;
  for(int64_t iter1 = xmin; iter1 < xmax + 1; iter1++)
  {
    for(int64_t iter2 = ymin; iter2 < ymax + 1; iter2++)
    {
      for(int64_t iter3 = zmin; iter3 < zmax + 1; iter3++)
      {
        column = iter1;
        row = iter2;
        plane = iter3;
        coords[0] = float(column) * m_PackingRes[0];
        coords[1] = float(row) * m_PackingRes[1];
        coords[2] = float(plane) * m_PackingRes[2];
        inside = -1.0f;
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
          m_ColumnList[gnum].push_back(column);
          m_RowList[gnum].push_back(row);
          m_PlaneList[gnum].push_back(plane);
          m_EllipFuncList[gnum].push_back(inside);
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::assignVoxels()
{

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixPath().getDataContainerName());

  size_t totalPoints = m->getAttributeMatrix(m_OutputCellAttributeMatrixPath.getAttributeMatrixName())->getNumberOfTuples();

  size_t udims[3] = {0, 0, 0};
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]),
  };

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  int64_t column = 0, row = 0, plane = 0;
  float xc = 0.0f, yc = 0.0f, zc = 0.0f;
  float size[3] = {m_SizeX, m_SizeY, m_SizeZ};

  int64_t xmin = 0, xmax = 0, ymin = 0, ymax = 0, zmin = 0, zmax = 0;

  float xRes = m->getGeometryAs<ImageGeom>()->getXRes();
  float yRes = m->getGeometryAs<ImageGeom>()->getYRes();
  float zRes = m->getGeometryAs<ImageGeom>()->getZRes();
  float res[3] = {xRes, yRes, zRes};

  Int32ArrayType::Pointer newownersPtr = Int32ArrayType::CreateArray(totalPoints, "_INTERNAL_USE_ONLY_newowners");
  newownersPtr->initializeWithValue(-1);
  int32_t* newowners = newownersPtr->getPointer(0);

  FloatArrayType::Pointer ellipfuncsPtr = FloatArrayType::CreateArray(totalPoints, "_INTERNAL_USE_ONLY_ellipfuncs");
  ellipfuncsPtr->initializeWithValue(-1);
  float* ellipfuncs = ellipfuncsPtr->getPointer(0);

  float featuresPerTime = 0;
  uint64_t millis = QDateTime::currentMSecsSinceEpoch();
  uint64_t currentMillis = millis;

  int64_t totalFeatures = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumberOfTuples();
  for(int64_t i = m_FirstPrimaryFeature; i < totalFeatures; i++)
  {
    featuresPerTime++;
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if(currentMillis - millis > 1000)
    {
      float rate = featuresPerTime / ((float)(currentMillis - millis)) * 1000.0f;

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
    // Unbounded Check for the size of shapeTypes. We assume a 1:1 with phase
    uint32_t shapeclass = m_ShapeTypes[m_FeaturePhases[i]];

    if(shapeclass != 0 && shapeclass != 1 && shapeclass != 2 && shapeclass != 3)
    {
      QString ss = QObject::tr("Undefined shape class in shape types array with path %1").arg(m_InputShapeTypesArrayPath.serialize());
      setErrorCondition(-78010);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    // init any values for each of the Shape Ops
    // init any values for each of the Shape Ops
    for(size_t iter = 0; iter < m_ShapeOps.size(); iter++)
    {
      m_ShapeOps[iter]->init();
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
    float ga[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    FOrientArrayType om(9, 0.0);
    FOrientTransformsType::eu2om(FOrientArrayType(phi1, PHI, phi2), om);
    om.toGMatrix(ga);
    column = static_cast<int64_t>(xc / xRes);
    row = static_cast<int64_t>(yc / yRes);
    plane = static_cast<int64_t>(zc / zRes);
    xmin = static_cast<int64_t>(column - ((radcur1 / xRes) + 1));
    xmax = static_cast<int64_t>(column + ((radcur1 / xRes) + 1));
    ymin = static_cast<int64_t>(row - ((radcur1 / yRes) + 1));
    ymax = static_cast<int64_t>(row + ((radcur1 / yRes) + 1));
    zmin = static_cast<int64_t>(plane - ((radcur1 / zRes) + 1));
    zmax = static_cast<int64_t>(plane + ((radcur1 / zRes) + 1));

    if(m_PeriodicBoundaries == true)
    {
      if(xmin < -dims[0])
      {
        xmin = -dims[0];
      }
      if(xmax > 2 * dims[0] - 1)
      {
        xmax = (2 * dims[0] - 1);
      }
      if(ymin < -dims[1])
      {
        ymin = -dims[1];
      }
      if(ymax > 2 * dims[1] - 1)
      {
        ymax = (2 * dims[1] - 1);
      }
      if(zmin < -dims[2])
      {
        zmin = -dims[2];
      }
      if(zmax > 2 * dims[2] - 1)
      {
        zmax = (2 * dims[2] - 1);
      }
    }
    else
    {
      if(xmin < 0)
      {
        xmin = 0;
      }
      if(xmax > dims[0] - 1)
      {
        xmax = dims[0] - 1;
      }
      if(ymin < 0)
      {
        ymin = 0;
      }
      if(ymax > dims[1] - 1)
      {
        ymax = dims[1] - 1;
      }
      if(zmin < 0)
      {
        zmin = 0;
      }
      if(zmax > dims[2] - 1)
      {
        zmax = dims[2] - 1;
      }
    }

    float radCur[3] = {radcur1, radcur2, radcur3};
    float xx[3] = {xc, yc, zc};
    ShapeOps* shapeOps = m_ShapeOps[shapeclass].get();
//#if 0
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    if(doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range3d<int64_t, int64_t, int64_t>(zmin, zmax + 1, ymin, ymax + 1, xmin, xmax + 1),
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
  int32_t gnum = 0;
  for(size_t i = 0; i < totalPoints; i++)
  {
    //    if(ellipfuncs[i] >= 0) { m_FeatureIds[i] = newowners[i]; }
    if(ellipfuncs[i] >= 0 && (m_UseMask == false || (m_UseMask == true && m_Mask[i] == true)))
    {
      m_FeatureIds[i] = newowners[i];
    }
    if(m_UseMask == true && m_Mask[i] == false)
    {
      m_FeatureIds[i] = 0;
    }
    gnum = m_FeatureIds[i];
    if(gnum >= 0)
    {
      activeObjects[gnum] = true;
    }
    newowners[i] = -1;
    ellipfuncs[i] = -1.0f;
  }

  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(getOutputCellFeatureAttributeMatrixName());
  cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());
  // need to update pointers after removing inactive objects
  updateFeatureInstancePointers();
  totalFeatures = cellFeatureAttrMat->getNumberOfTuples();
  // counting the number of features for each phase
  for(int64_t i = 1; i < totalFeatures; i++)
  {
    int32_t phase = m_FeaturePhases[i];
    if(phase >= 0)
    {
      m_NumFeatures[phase]++;
    }
  }

  // need to update pointers after resize, but do not need to run full data check because pointers are still valid
  updateFeatureInstancePointers();
  if(getCancel() == true)
  {
    return;
  }

  for(size_t i = 0; i < totalPoints; i++)
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
void PackPrimaryPhases::assignGapsOnly()
{

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixPath().getDataContainerName());

  int32_t featurename = 0, feature = 0;
  int32_t current = 0;
  int32_t most = 0;
  int64_t gapVoxelCount = 1;
  int64_t previousGapVoxelCount = 0;
  int32_t iterationCounter = 0;
  int64_t neighpoint;
  bool good = false;
  int32_t neighbor = 0;

  int64_t xPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getXPoints());
  int64_t yPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getYPoints());
  int64_t zPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getZPoints());
  size_t totalPoints = m->getAttributeMatrix(m_OutputCellAttributeMatrixPath.getAttributeMatrixName())->getNumberOfTuples();
  size_t totalFeatures = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumberOfTuples();

  int64_t neighpoints[6] = {0, 0, 0, 0, 0, 0};
  neighpoints[0] = -xPoints * yPoints;
  neighpoints[1] = -xPoints;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xPoints;
  neighpoints[5] = xPoints * yPoints;

  Int64ArrayType::Pointer neighborsPtr = Int64ArrayType::CreateArray(m->getGeometryAs<ImageGeom>()->getNumberOfElements(), "_INTERNAL_USE_ONLY_Neighbors");
  neighborsPtr->initializeWithValue(-1);
  m_Neighbors = neighborsPtr->getPointer(0);

  std::vector<int32_t> n(totalFeatures + 1, 0);

  while(gapVoxelCount != 0 && gapVoxelCount != previousGapVoxelCount)
  {
    iterationCounter++;
    previousGapVoxelCount = gapVoxelCount;
    gapVoxelCount = 0;
    int64_t zStride, yStride;
    for(int64_t i = 0; i < zPoints; i++)
    {
      zStride = i * xPoints * yPoints;
      for(int64_t j = 0; j < yPoints; j++)
      {
        yStride = j * xPoints;
        for(int64_t k = 0; k < xPoints; k++)
        {
          featurename = m_FeatureIds[zStride + yStride + k];
          if(featurename < 0)
          {
            gapVoxelCount++;
            current = 0;
            most = 0;
            for(int32_t l = 0; l < 6; l++)
            {
              good = true;
              neighpoint = zStride + yStride + k + neighpoints[l];
              if(l == 0 && i == 0)
              {
                good = false;
              }
              if(l == 5 && i == (zPoints - 1))
              {
                good = false;
              }
              if(l == 1 && j == 0)
              {
                good = false;
              }
              if(l == 4 && j == (yPoints - 1))
              {
                good = false;
              }
              if(l == 2 && k == 0)
              {
                good = false;
              }
              if(l == 3 && k == (xPoints - 1))
              {
                good = false;
              }
              if(good == true)
              {
                feature = m_FeatureIds[neighpoint];
                if(feature > 0)
                {
                  n[feature]++;
                  current = n[feature];
                  if(current > most)
                  {
                    most = current;
                    m_Neighbors[zStride + yStride + k] = neighpoint;
                  }
                }
              }
            }
            for(int32_t l = 0; l < 6; l++)
            {
              good = true;
              neighpoint = zStride + yStride + k + neighpoints[l];
              if(l == 0 && i == 0)
              {
                good = false;
              }
              if(l == 5 && i == (zPoints - 1))
              {
                good = false;
              }
              if(l == 1 && j == 0)
              {
                good = false;
              }
              if(l == 4 && j == (yPoints - 1))
              {
                good = false;
              }
              if(l == 2 && k == 0)
              {
                good = false;
              }
              if(l == 3 && k == (xPoints - 1))
              {
                good = false;
              }
              if(good == true)
              {
                feature = m_FeatureIds[neighpoint];
                if(feature > 0)
                {
                  n[feature] = 0;
                }
              }
            }
          }
        }
      }
    }
    for(size_t j = 0; j < totalPoints; j++)
    {
      featurename = m_FeatureIds[j];
      neighbor = m_Neighbors[j];
      if(featurename < 0 && neighbor != -1 && m_FeatureIds[neighbor] > 0)
      {
        m_FeatureIds[j] = m_FeatureIds[neighbor];
        m_CellPhases[j] = m_FeaturePhases[m_FeatureIds[neighbor]];
      }
    }
    if(iterationCounter >= 1)
    {
      QString ss = QObject::tr("Assign Gaps || Cycle#: %1 || Remaining Unassigned Voxel Count: %2").arg(iterationCounter).arg(gapVoxelCount);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    }
    if(getCancel() == true)
    {
      return;
    }
  }
  if(gapVoxelCount != 0)
  {
    for(size_t j = 0; j < totalPoints; j++)
    {
      if(m_FeatureIds[j] < 0)
      {
        m_FeatureIds[j] = 0;
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::cleanupFeatures()
{

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixPath().getDataContainerName());

  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock().get());

  size_t totalPoints = m->getAttributeMatrix(m_OutputCellAttributeMatrixPath.getAttributeMatrixName())->getNumberOfTuples();
  size_t totalFeatures = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumberOfTuples();
  size_t udims[3] = {0, 0, 0};
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]),
  };

  int64_t neighpoints[6] = {0, 0, 0, 0, 0, 0};
  int64_t xp = dims[0];
  int64_t yp = dims[1];
  int64_t zp = dims[2];

  neighpoints[0] = -(xp * yp);
  neighpoints[1] = -xp;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xp;
  neighpoints[5] = (xp * yp);
  std::vector<std::vector<int64_t>> vlists;
  vlists.resize(totalFeatures);
  std::vector<int64_t> currentvlist;
  std::vector<bool> checked(totalPoints, false);
  QVector<bool> activeObjects(totalFeatures, true);
  size_t count;
  bool touchessurface = false;
  bool good = false;
  int64_t neighbor = 0;
  int64_t column = 0, row = 0, plane = 0;
  int64_t index = 0;
  float minsize = 0.0f;
  m_GSizes.resize(totalFeatures);
  for(size_t i = 1; i < totalFeatures; i++)
  {
    m_GSizes[i] = 0;
  }

  float resConst = m->getGeometryAs<ImageGeom>()->getXRes() * m->getGeometryAs<ImageGeom>()->getYRes() * m->getGeometryAs<ImageGeom>()->getZRes();
  const double k_PiOver6 = M_PI / 6.0;
  for(size_t i = 0; i < totalPoints; i++)
  {
    touchessurface = false;
    if(checked[i] == false && m_FeatureIds[i] > m_FirstPrimaryFeature)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[m_CellPhases[i]].get());
      minsize = static_cast<float>(pp->getMinFeatureDiameter() * pp->getMinFeatureDiameter() * pp->getMinFeatureDiameter() * k_PiOver6);
      minsize = static_cast<float>(int32_t(minsize / (resConst)));
      currentvlist.push_back(i);
      count = 0;
      while(count < currentvlist.size())
      {
        index = currentvlist[count];
        column = index % xp;
        row = (index / xp) % yp;
        plane = index / (xp * yp);
        if(column == 0 || column == xp || row == 0 || row == yp || plane == 0 || plane == zp)
        {
          touchessurface = true;
        }
        for(int32_t j = 0; j < 6; j++)
        {
          good = true;
          neighbor = index + neighpoints[j];
          if(m_PeriodicBoundaries == false)
          {
            if(j == 0 && plane == 0)
            {
              good = false;
            }
            if(j == 5 && plane == (zp - 1))
            {
              good = false;
            }
            if(j == 1 && row == 0)
            {
              good = false;
            }
            if(j == 4 && row == (yp - 1))
            {
              good = false;
            }
            if(j == 2 && column == 0)
            {
              good = false;
            }
            if(j == 3 && column == (xp - 1))
            {
              good = false;
            }
            if(good == true && m_FeatureIds[neighbor] == m_FeatureIds[index] && checked[neighbor] == false)
            {
              currentvlist.push_back(neighbor);
              checked[neighbor] = true;
            }
          }
          else if(m_PeriodicBoundaries == true)
          {
            if(j == 0 && plane == 0)
            {
              neighbor = neighbor + (xp * yp * zp);
            }
            if(j == 5 && plane == (zp - 1))
            {
              neighbor = neighbor - (xp * yp * zp);
            }
            if(j == 1 && row == 0)
            {
              neighbor = neighbor + (xp * yp);
            }
            if(j == 4 && row == (yp - 1))
            {
              neighbor = neighbor - (xp * yp);
            }
            if(j == 2 && column == 0)
            {
              neighbor = neighbor + (xp);
            }
            if(j == 3 && column == (xp - 1))
            {
              neighbor = neighbor - (xp);
            }
            if(m_FeatureIds[neighbor] == m_FeatureIds[index] && checked[neighbor] == false)
            {
              currentvlist.push_back(neighbor);
              checked[neighbor] = true;
            }
          }
        }
        count++;
      }
      int32_t size = vlists[m_FeatureIds[i]].size();
      if(size > 0)
      {
        if(size < currentvlist.size())
        {
          for(size_t k = 0; k < vlists[m_FeatureIds[i]].size(); k++)
          {
            m_FeatureIds[vlists[m_FeatureIds[i]][k]] = -1;
          }
          vlists[m_FeatureIds[i]].resize(currentvlist.size());
          vlists[m_FeatureIds[i]].swap(currentvlist);
        }
        else if(size >= currentvlist.size())
        {
          for(size_t k = 0; k < currentvlist.size(); k++)
          {
            m_FeatureIds[currentvlist[k]] = -1;
          }
        }
      }
      else if(size == 0)
      {
        if(currentvlist.size() >= minsize || touchessurface == true)
        {
          vlists[m_FeatureIds[i]].resize(currentvlist.size());
          vlists[m_FeatureIds[i]].swap(currentvlist);
        }
        if(currentvlist.size() < minsize && touchessurface == false)
        {
          for(size_t k = 0; k < currentvlist.size(); k++)
          {
            m_FeatureIds[currentvlist[k]] = -1;
          }
        }
      }
      currentvlist.clear();
    }
  }
  assignGapsOnly();
  if(getCancel() == true)
  {
    return;
  }
  for(size_t i = 0; i < totalPoints; i++)
  {
    if(m_FeatureIds[i] > 0)
    {
      m_GSizes[m_FeatureIds[i]]++;
    }
  }
  for(size_t i = m_FirstPrimaryFeature; i < totalFeatures; i++)
  {
    if(m_GSizes[i] == 0)
    {
      activeObjects[i] = false;
    }
  }

  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(getOutputCellFeatureAttributeMatrixName());
  cellFeatureAttrMat->removeInactiveObjects(activeObjects, m_FeatureIdsPtr.lock());
  updateFeatureInstancePointers();

  for(size_t i = 0; i < totalPoints; i++)
  {
    if(m_FeatureIds[i] > 0)
    {
      m_CellPhases[i] = m_FeaturePhases[m_FeatureIds[i]];
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t PackPrimaryPhases::estimateNumFeatures(size_t xpoints, size_t ypoints, size_t zpoints, float xres, float yres, float zres)
{
  float ptotalvol = 0.0f;
  int32_t phase = 0;

  ptotalvol = static_cast<float>((xpoints * xres) * (ypoints * yres) * (zpoints * zres));
  if(ptotalvol == 0.0)
  {
    return 1;
  }

  // This is for convenience
  DataContainerArray::Pointer dca = getDataContainerArray();

  // DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixPath().getDataContainerName());
  QVector<size_t> cDims(1, 1);
  m_PhaseTypesPtr = dca->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getInputPhaseTypesArrayPath(), cDims);
  DataArray<uint32_t>* phaseType = m_PhaseTypesPtr.lock().get();

  StatsDataArray::Pointer statsPtr = dca->getPrereqArrayFromPath<StatsDataArray, AbstractFilter>(this, getInputStatsArrayPath(), cDims);
  m_StatsDataArray = std::dynamic_pointer_cast<StatsDataArray>(statsPtr);
  if(m_StatsDataArray.lock().get() == nullptr)
  {
    QString ss = QObject::tr("Stats Array Not Initialized correctly");
    setErrorCondition(-78011);
    notifyErrorMessage(getHumanLabel(), ss, -308);
    return 1;
  }

#if 0

  IDataArray::Pointer iPtr = m->getAttributeMatrix(m_CellEnsembleAttributeMatrixName)->getAttributeArray(SIMPL::EnsembleData::PhaseTypes);
  // Get the PhaseTypes - Remember there is a Dummy PhaseType in the first slot of the array
  DataArray<uint32_t>* phaseType = DataArray<uint32_t>::SafePointerDownCast(iPtr.get());

  iPtr = m->getAttributeMatrix(m_CellEnsembleAttributeMatrixName)->getAttributeArray(SIMPL::EnsembleData::Statistics);
  StatsDataArray* statsDataArrayPtr = StatsDataArray::SafePointerDownCast(iPtr.get());
  if(nullptr == statsDataArrayPtr)
  {
    return 1;
  }
#endif
  // Create a Reference Variable so we can use the [] syntax
  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock().get());

  SIMPL_RANDOMNG_NEW()

  std::vector<int32_t> primaryPhasesLocal;
  std::vector<double> primaryPhaseFractionsLocal;
  double totalprimaryfractions = 0.0;
  // find which phases are primary phases
  size_t numPhases = phaseType->getNumberOfTuples();
  for(size_t i = 1; i < numPhases; ++i)
  {
    if(phaseType->getValue(i) == static_cast<PhaseType::EnumType>(PhaseType::Type::Primary))
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      primaryPhasesLocal.push_back(i);
      primaryPhaseFractionsLocal.push_back(pp->getPhaseFraction());
      totalprimaryfractions = totalprimaryfractions + pp->getPhaseFraction();
    }
  }

  // scale the primary phase fractions to total to 1
  for(size_t i = 0; i < primaryPhaseFractionsLocal.size(); i++)
  {
    primaryPhaseFractionsLocal[i] = primaryPhaseFractionsLocal[i] / totalprimaryfractions;
  }

  // generate the features
  int32_t gid = 1;

  float currentvol = 0.0f;
  float vol = 0.0f;
  float diam = 0.0f;
  bool volgood = false;
  size_t numLocalPrimaryPhases = primaryPhasesLocal.size();

  for(size_t j = 0; j < numLocalPrimaryPhases; ++j)
  {
    float curphasetotalvol = m_TotalVol * primaryPhaseFractionsLocal[j];
    while(currentvol < curphasetotalvol)
    {
      volgood = 0;
      phase = primaryPhasesLocal[j];
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
      while(volgood == false)
      {
        volgood = true;
        if(pp->getFeatureSize_DistType() == SIMPL::DistributionType::LogNormal)
        {
          float avgdiam = pp->getFeatureSizeDistribution().at(0)->getValue(0);
          float sddiam = pp->getFeatureSizeDistribution().at(1)->getValue(0);
          diam = rg.genrand_norm(avgdiam, sddiam);
          diam = expf(diam);
          if(diam >= pp->getMaxFeatureDiameter())
          {
            volgood = false;
          }
          if(diam < pp->getMinFeatureDiameter())
          {
            volgood = false;
          }
          vol = (4.0f / 3.0f) * (M_PI) * ((diam * 0.5f) * (diam * 0.5f) * (diam * 0.5f));
        }
      }
      currentvol = currentvol + vol;
      gid++;
    }
  }
  return gid;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PackPrimaryPhases::writeGoalAttributes()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixPath().getDataContainerName());

  size_t totalFeatures = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getNumberOfTuples();

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(m_CsvOutputFile);
  QDir parentPath = fi.path();
  if(!parentPath.mkpath("."))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    setErrorCondition(-78013);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QFile outFile(getCsvOutputFile());
  if(!outFile.open(QIODevice::WriteOnly))
  {
    QString msg = QObject::tr("CSV Output file could not be opened: %1").arg(getCsvOutputFile());
    setErrorCondition(-78014);
    notifyErrorMessage(getHumanLabel(), msg, getErrorCondition());
    return;
  }

  QTextStream dStream(&outFile);

  char space = ',';
  // Write the total number of features
  dStream << totalFeatures - m_FirstPrimaryFeature;
  // Get all the names of the arrays from the Data Container
  QList<QString> headers = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getAttributeArrayNames();

  QVector<IDataArray::Pointer> data;

  // For checking if an array is a neighborlist
  NeighborList<int32_t>::Pointer neighborlistPtr = NeighborList<int32_t>::New();

  // Print the FeatureIds Header before the rest of the headers
  dStream << SIMPL::FeatureData::FeatureID;
  // Loop throught the list and print the rest of the headers, ignoring those we don't want
  for(QList<QString>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
  {
    // Only get the array if the name does NOT match those listed
    IDataArray::Pointer p = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName)->getAttributeArray(*iter);
    if(p->getNameOfClass().compare(neighborlistPtr->getNameOfClass()) != 0)
    {
      if(p->getNumberOfComponents() == 1)
      {
        dStream << space << (*iter);
      }
      else // There are more than a single component so we need to add multiple header values
      {
        for(int32_t k = 0; k < p->getNumberOfComponents(); ++k)
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
  for(size_t i = m_FirstPrimaryFeature; i < numTuples; ++i)
  {
    if(((float)i / numTuples) * 100.0f > threshold)
    {

      QString ss = QObject::tr("Writing Feature Data || %1% Complete").arg(((float)i / numTuples) * 100);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      threshold = threshold + 5.0f;
      if(threshold < ((float)i / numTuples) * 100.0f)
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
void PackPrimaryPhases::moveShapeDescriptions()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getOutputCellAttributeMatrixPath().getDataContainerName());

  QStringList names;
  names << m_EquivalentDiametersArrayName << m_Omega3sArrayName << m_AxisEulerAnglesArrayName << m_AxisLengthsArrayName << m_VolumesArrayName << m_CentroidsArrayName << m_NeighborhoodsArrayName;

  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(m_OutputCellFeatureAttributeMatrixName);
  QVector<size_t> tDims(1, 0);

  QList<IDataArray::Pointer> attrArrays;
  foreach(const QString name, names)
  {
    IDataArray::Pointer arrayPtr = cellFeatureAttrMat->removeAttributeArray(name);
    if(arrayPtr != IDataArray::NullPointer())
    {
      attrArrays.push_back(arrayPtr);
    }
  }

  PackPrimaryPhases::SaveMethod saveMethod = static_cast<PackPrimaryPhases::SaveMethod>(getSaveGeometricDescriptions());
  if(saveMethod == PackPrimaryPhases::SaveMethod::SaveToNew)
  {
    AttributeMatrix::Pointer newAM = getDataContainerArray()->getAttributeMatrix(getNewAttributeMatrixPath());
    if(newAM != AttributeMatrix::NullPointer())
    {
      if(attrArrays.size() > 0)
      {
        size_t incomingArrayTupleCount = attrArrays[0]->getNumberOfTuples();
        size_t newAMTupleCount = newAM->getTupleDimensions()[0];
        tDims[0] = incomingArrayTupleCount + newAMTupleCount;
        newAM->resizeAttributeArrays(tDims);
      }

      foreach(IDataArray::Pointer incomingArray, attrArrays)
      {
        newAM->addAttributeArray(incomingArray->getName(), incomingArray);
      }
    }
  }
  else if(saveMethod == PackPrimaryPhases::SaveMethod::AppendToExisting)
  {
    AttributeMatrix::Pointer existingAM = getDataContainerArray()->getAttributeMatrix(getSelectedAttributeMatrixPath());
    if(existingAM != AttributeMatrix::NullPointer())
    {
      if(attrArrays.size() > 0)
      {
        size_t incomingArrayTupleCount = attrArrays[0]->getNumberOfTuples();
        size_t existingAMTupleCount = existingAM->getTupleDimensions()[0];
        tDims[0] = incomingArrayTupleCount + existingAMTupleCount;
        existingAM->resizeAttributeArrays(tDims);
      }

      foreach(IDataArray::Pointer incomingArray, attrArrays)
      {
        int err = 0;
        IDataArray::Pointer existingArray = existingAM->getPrereqIDataArray<IDataArray, AbstractFilter>(this, incomingArray->getName(), err);
        if(existingArray != IDataArray::NullPointer())
        {
          existingArray->copyFromArray(tDims[0], incomingArray);
        }
      }
    }
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
{
  return SyntheticBuildingConstants::SyntheticBuildingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PackPrimaryPhases::getBrandingString()
{
  return "SyntheticBuilding";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PackPrimaryPhases::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SyntheticBuilding::Version::Major() << "." << SyntheticBuilding::Version::Minor() << "." << SyntheticBuilding::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PackPrimaryPhases::getGroupName()
{
  return SIMPL::FilterGroups::SyntheticBuildingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PackPrimaryPhases::getSubGroupName()
{
  return SIMPL::FilterSubGroups::PackingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PackPrimaryPhases::getHumanLabel()
{
  return "Pack Primary Phases";
}
