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

#include "INLWriter.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "EbsdLib/TSL/AngConstants.h"

#include "IO/IOConstants.h"

// Include the MOC generated file for this class
#include "moc_INLWriter.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
INLWriter::INLWriter() :
  FileWriter(),
  m_MaterialNameArrayPath(DREAM3D::EnsembleData::MaterialName),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_CellPhasesArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Phases),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_NumFeaturesArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::NumFeatures),
  m_CellEulerAnglesArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::EulerAngles),
  m_FeatureIds(NULL),
  m_CellPhases(NULL),
  m_CellEulerAngles(NULL),
  m_CrystalStructures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
INLWriter::~INLWriter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void INLWriter::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(OutputFileFilterParameter::New("Output File", "OutputFile", getOutputFile(), FilterParameter::Parameter, "*.txt", "INL Format"));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixType::Cell, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Feature Ids", "FeatureIdsArrayPath", getFeatureIdsArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixType::Cell, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Phases", "CellPhasesArrayPath", getCellPhasesArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Float, 3, DREAM3D::AttributeMatrixType::Cell, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Euler Angles", "CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::UInt32, 1, DREAM3D::AttributeMatrixType::CellEnsemble, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", getCrystalStructuresArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::Defaults::AnyPrimitive, 1, DREAM3D::AttributeMatrixType::CellEnsemble, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Material Names", "MaterialNameArrayPath", getMaterialNameArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixType::CellEnsemble, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Number of Features", "NumFeaturesArrayPath", getNumFeaturesArrayPath(), FilterParameter::RequiredArray, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void INLWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellEulerAnglesArrayPath(reader->readDataArrayPath("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setNumFeaturesArrayPath(reader->readDataArrayPath("NumFeaturesArrayPath", getNumFeaturesArrayPath() ) );
  setMaterialNameArrayPath(reader->readDataArrayPath("MaterialNameArrayPath", getMaterialNameArrayPath()));
  setOutputFile( reader->readString( "OutputFile", getOutputFile() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int INLWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(CellEulerAnglesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(CellPhasesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(OutputFile)
  SIMPL_FILTER_WRITE_PARAMETER(NumFeaturesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(MaterialNameArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void INLWriter::dataCheck()
{
  setErrorCondition(0);

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  if (getOutputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("The output file must be set");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QFileInfo fi(getOutputFile());
  QDir parentPath = fi.path();
  if (parentPath.exists() == false)
  {
    QString ss = QObject::tr("The directory path for the output file does not exist. DREAM.3D will attempt to create this path during execution of the filter");
    notifyWarningMessage(getHumanLabel(), ss, -1);
  }

  QVector<DataArrayPath> cellDataArrayPaths;
  QVector<DataArrayPath> ensembleDataArrayPaths;

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { cellDataArrayPaths.push_back(getFeatureIdsArrayPath()); }

  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { cellDataArrayPaths.push_back(getCellPhasesArrayPath()); }

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this,  getCrystalStructuresArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { ensembleDataArrayPaths.push_back(getCrystalStructuresArrayPath()); }

  m_NumFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this,  getNumFeaturesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NumFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NumFeatures = m_NumFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { ensembleDataArrayPaths.push_back(getNumFeaturesArrayPath()); }

  m_MaterialNamePtr = getDataContainerArray()->getPrereqArrayFromPath<StringDataArray, AbstractFilter>(this,  getMaterialNameArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(getErrorCondition() >= 0) { ensembleDataArrayPaths.push_back(getMaterialNameArrayPath()); }

  cDims[0] = 3;
  m_CellEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellEulerAnglesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { cellDataArrayPaths.push_back(getCellEulerAnglesArrayPath()); }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, cellDataArrayPaths);
  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, ensembleDataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void INLWriter::preflight()
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
int32_t INLWriter::writeHeader()
{
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint32_t mapCrystalSymmetryToTslSymmetry(uint32_t symmetry)
{
  switch(symmetry)
  {
    case Ebsd::CrystalStructure::Cubic_High:
      return Ebsd::Ang::PhaseSymmetry::Cubic;
    case Ebsd::CrystalStructure::Cubic_Low:
      return Ebsd::Ang::PhaseSymmetry::Tetrahedral;
    case  Ebsd::CrystalStructure::Tetragonal_High:
      return Ebsd::Ang::PhaseSymmetry::DiTetragonal;
    case Ebsd::CrystalStructure::Tetragonal_Low:
      return Ebsd::Ang::PhaseSymmetry::Tetragonal;
    case Ebsd::CrystalStructure::OrthoRhombic:
      return Ebsd::Ang::PhaseSymmetry::Orthorhombic;
    case Ebsd::CrystalStructure::Monoclinic:
      return Ebsd::Ang::PhaseSymmetry::Monoclinic_c;
      return Ebsd::Ang::PhaseSymmetry::Monoclinic_b;
      return Ebsd::Ang::PhaseSymmetry::Monoclinic_a;
    case Ebsd::CrystalStructure::Triclinic:
      return Ebsd::Ang::PhaseSymmetry::Triclinic;
    case Ebsd::CrystalStructure::Hexagonal_High:
      return Ebsd::Ang::PhaseSymmetry::DiHexagonal;
    case Ebsd::CrystalStructure::Hexagonal_Low:
      return Ebsd::Ang::PhaseSymmetry::Hexagonal;
    case Ebsd::CrystalStructure::Trigonal_High:
      return Ebsd::Ang::PhaseSymmetry::DiTrigonal;
    case Ebsd::CrystalStructure::Trigonal_Low:
      return Ebsd::Ang::PhaseSymmetry::Trigonal;
    default:
      return Ebsd::CrystalStructure::UnknownCrystalStructure;
  }
  return Ebsd::CrystalStructure::UnknownCrystalStructure;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t INLWriter::writeFile()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return getErrorCondition(); }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName());

  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();

  int32_t err = 0;
  size_t dims[3] = { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(dims);
  float res[3] = { 0.0f, 0.0f, 0.0f };
  m->getGeometryAs<ImageGeom>()->getResolution(res);
  float origin[3] = { 0.0f, 0.0f, 0.0f };
  m->getGeometryAs<ImageGeom>()->getOrigin(origin);

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(getOutputFile());
  QDir dir(fi.path());
  if (!dir.mkpath("."))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(fi.path());
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }

  FILE* f = fopen(getOutputFile().toLatin1().data(), "wb");
  if (NULL == f)
  {
    QString ss = QObject::tr("Error opening output file '%1'").arg(getOutputFile());
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }

  // Write the header, Each line starts with a "#" symbol
  fprintf(f, "# File written from %s\r\n", SIMPLib::Version::PackageComplete().toLatin1().data());
  fprintf(f, "# DateTime: %s\r\n", QDateTime::currentDateTime().toString().toLatin1().data());
  fprintf(f, "# X_STEP: %f\r\n", res[0]);
  fprintf(f, "# Y_STEP: %f\r\n", res[1]);
  fprintf(f, "# Z_STEP: %f\r\n", res[2]);
  fprintf(f, "#\r\n");
  fprintf(f, "# X_MIN: %f\r\n", origin[0]);
  fprintf(f, "# Y_MIN: %f\r\n", origin[1]);
  fprintf(f, "# Z_MIN: %f\r\n", origin[2]);
  fprintf(f, "#\r\n");
  fprintf(f, "# X_MAX: %f\r\n", origin[0] + (dims[0]*res[0]));
  fprintf(f, "# Y_MAX: %f\r\n", origin[1] + (dims[1]*res[1]));
  fprintf(f, "# Z_MAX: %f\r\n", origin[2] + (dims[2]*res[2]));
  fprintf(f, "#\r\n");
  fprintf(f, "# X_DIM: %llu\r\n", static_cast<long long unsigned int>(dims[0]));
  fprintf(f, "# Y_DIM: %llu\r\n", static_cast<long long unsigned int>(dims[1]));
  fprintf(f, "# Z_DIM: %llu\r\n", static_cast<long long unsigned int>(dims[2]));
  fprintf(f, "#\r\n");

  StringDataArray* materialNames = m_MaterialNamePtr.lock().get();

#if 0
  -------------------------------------------- -
# Phase_1: MOX with 30% Pu
# Symmetry_1: 43
# Features_1: 4
#
# Phase_2: Brahman
# Symmetry_2: 62
# Features_2: 6
#
# Phase_3: Void
# Symmetry_3: 22
# Features_3: 1
#
# Total_Features: 11
  -------------------------------------------- -
#endif


  uint32_t symmetry = 0;
  int32_t count = static_cast<int32_t>(materialNames->getNumberOfTuples());
  for (int32_t i = 1; i < count; ++i)
  {
    QString matName = materialNames->getValue(i);
    fprintf(f, "# Phase_%d: %s\r\n", i, matName.toLatin1().data());
    symmetry = m_CrystalStructures[i];
    symmetry = mapCrystalSymmetryToTslSymmetry(symmetry);
    fprintf(f, "# Symmetry_%d: %u\r\n", i, symmetry);
    fprintf(f, "# Features_%d: %d\r\n", i, m_NumFeatures[i]);
    fprintf(f, "#\r\n");
  }

  std::set<int32_t> uniqueFeatureIds;
  for (size_t i = 0; i < totalPoints; ++i)
  {
    uniqueFeatureIds.insert(m_FeatureIds[i]);
  }
  count = static_cast<int32_t>(uniqueFeatureIds.size());
  fprintf(f, "# Num_Features: %d \r\n", count);
  fprintf(f, "#\r\n");

  //  fprintf(f, "# Column 1-3: phi1, PHI, phi2 (orientation of point in radians)\r\n");
  //  fprintf(f, "# Column 4-6: x, y, z (coordinates of point in microns)\r\n");
  //  fprintf(f, "# Column 7: Feature ID\r\n");
  //  fprintf(f, "# Column 8: Phase ID\r\n");

  fprintf(f, "# phi1 PHI phi2 x y z FeatureId PhaseId Symmetry\r\n");

  float phi1 = 0.0f, phi = 0.0f, phi2 = 0.0f;
  float xPos = 0.0f, yPos = 0.0f, zPos = 0.0f;
  int32_t featureId = 0;
  int32_t phaseId = 0;

  size_t index = 0;
  for (size_t z = 0; z < dims[2]; ++z)
  {
    for (size_t y = 0; y < dims[1]; ++y)
    {
      for (size_t x = 0; x < dims[0]; ++x)
      {
        index = (z * dims[0] * dims[1]) + (dims[0] * y) + x;
        phi1 = m_CellEulerAngles[index * 3];
        phi = m_CellEulerAngles[index * 3 + 1];
        phi2 = m_CellEulerAngles[index * 3 + 2];
        xPos = origin[0] + (x * res[0]);
        yPos = origin[1] + (y * res[1]);
        zPos = origin[2] + (z * res[2]);
        featureId = m_FeatureIds[index];
        phaseId = m_CellPhases[index];
        symmetry = m_CrystalStructures[phaseId];
        if (phaseId > 0)
        {
          if (symmetry == Ebsd::CrystalStructure::Cubic_High)
          {
            symmetry = Ebsd::Ang::PhaseSymmetry::Cubic;
          }
          else if (symmetry == Ebsd::CrystalStructure::Hexagonal_High)
          {
            symmetry = Ebsd::Ang::PhaseSymmetry::DiHexagonal;
          }
          else
          {
            symmetry = Ebsd::Ang::PhaseSymmetry::UnknownSymmetry;
          }
        }
        else
        {
          symmetry = Ebsd::Ang::PhaseSymmetry::UnknownSymmetry;
        }

        fprintf(f, "%f %f %f %f %f %f %d %d %d\r\n", phi1, phi, phi2, xPos, yPos, zPos, featureId, phaseId, symmetry);
      }
    }
  }

  fclose(f);

  notifyStatusMessage(getHumanLabel(), "Complete");
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer INLWriter::newFilterInstance(bool copyFilterParameters)
{
  INLWriter::Pointer filter = INLWriter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString INLWriter::getCompiledLibraryName()
{
  return IOConstants::IOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString INLWriter::getBrandingString()
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString INLWriter::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString INLWriter::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString INLWriter::getSubGroupName()
{ return DREAM3D::FilterSubGroups::OutputFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString INLWriter::getHumanLabel()
{ return "Write INL File"; }
