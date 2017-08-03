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

#include "WritePoleFigure.h"

#include <QtCore/QDir>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "OrientationLib/LaueOps/CubicLowOps.h"
#include "OrientationLib/LaueOps/CubicOps.h"
#include "OrientationLib/LaueOps/HexagonalLowOps.h"
#include "OrientationLib/LaueOps/HexagonalOps.h"
#include "OrientationLib/LaueOps/MonoclinicOps.h"
#include "OrientationLib/LaueOps/OrthoRhombicOps.h"
#include "OrientationLib/LaueOps/TetragonalLowOps.h"
#include "OrientationLib/LaueOps/TetragonalOps.h"
#include "OrientationLib/LaueOps/TriclinicOps.h"
#include "OrientationLib/LaueOps/TrigonalLowOps.h"
#include "OrientationLib/LaueOps/TrigonalOps.h"

#include "OrientationLib/Utilities/PoleFigureImageUtilities.h"

#include "EbsdLib/EbsdConstants.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

// Include the MOC generated file for this class
#include "moc_WritePoleFigure.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WritePoleFigure::WritePoleFigure()
: AbstractFilter()
, m_ImagePrefix("")
, m_OutputPath("")
, m_ImageFormat(0)
, m_ImageSize(512)
, m_LambertSize(32)
, m_NumColors(32)
, m_ImageLayout(SIMPL::Layout::Square)
, m_CellEulerAnglesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::EulerAngles)
, m_CellPhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Phases)
, m_CrystalStructuresArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::CrystalStructures)
, m_GoodVoxelsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Mask)
, m_UseGoodVoxels(false)
, m_CellEulerAngles(nullptr)
, m_CellPhases(nullptr)
, m_CrystalStructures(nullptr)
, m_GoodVoxels(nullptr)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WritePoleFigure::~WritePoleFigure()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WritePoleFigure::setupFilterParameters()
{
  FilterParameterVector parameters;

  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Image Format");
    parameter->setPropertyName("ImageFormat");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(WritePoleFigure, this, ImageFormat));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(WritePoleFigure, this, ImageFormat));

    QVector<QString> choices;
    choices.push_back("tif");
    choices.push_back("bmp");
    choices.push_back("png");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Lambert Image Size (Pixels)", LambertSize, FilterParameter::Parameter, WritePoleFigure));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Number of Colors", NumColors, FilterParameter::Parameter, WritePoleFigure));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Image Layout");
    parameter->setPropertyName("ImageLayout");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(WritePoleFigure, this, ImageLayout));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(WritePoleFigure, this, ImageLayout));

    QVector<QString> choices;
    choices.push_back("Horizontal");
    choices.push_back("Vertical");
    choices.push_back("Square");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(SIMPL_NEW_STRING_FP("Image Prefix", ImagePrefix, FilterParameter::Parameter, WritePoleFigure));
  parameters.push_back(SIMPL_NEW_OUTPUT_PATH_FP("Output Path", OutputPath, FilterParameter::Parameter, WritePoleFigure));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Image Size (Square Pixels)", ImageSize, FilterParameter::Parameter, WritePoleFigure));
  QStringList linkedProps("GoodVoxelsArrayPath");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Use Mask Array", UseGoodVoxels, FilterParameter::Parameter, WritePoleFigure, linkedProps));

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Type::Any, IGeometry::Type::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Euler Angles", CellEulerAnglesArrayPath, FilterParameter::RequiredArray, WritePoleFigure, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Any, IGeometry::Type::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", CellPhasesArrayPath, FilterParameter::RequiredArray, WritePoleFigure, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Bool, 1, AttributeMatrix::Type::Any, IGeometry::Type::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Mask", GoodVoxelsArrayPath, FilterParameter::RequiredArray, WritePoleFigure, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, WritePoleFigure, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WritePoleFigure::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath()));
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath()));
  setCellEulerAnglesArrayPath(reader->readDataArrayPath("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath()));
  setImagePrefix(reader->readString("ImagePrefix", getImagePrefix()));
  setOutputPath(reader->readString("OutputPath", getOutputPath()));
  setImageFormat(reader->readValue("ImageFormat", getImageFormat()));
  setImageLayout(reader->readValue("ImageLayout", getImageLayout()));
  setImageSize(reader->readValue("ImageSize", getImageSize()));
  setLambertSize(reader->readValue("LambertSize", getLambertSize()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WritePoleFigure::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WritePoleFigure::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  QDir path(getOutputPath());

  if(m_OutputPath.isEmpty() == true)
  {
    setErrorCondition(-1003);
    notifyErrorMessage(getHumanLabel(), "The output directory must be set", getErrorCondition());
  }
  else if(path.exists() == false)
  {
    setWarningCondition(-1004);
    QString ss = QObject::tr("The directory path for the output file does not exist. DREAM.3D will attempt to create this path during execution of the filter");
    notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
  }

  QVector<DataArrayPath> dataArrayPaths;

  QVector<size_t> cDims(1, 3);
  m_CellEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellEulerAnglesArrayPath(),
                                                                                                           cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellEulerAnglesPtr.lock().get())                                                                 /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getCellEulerAnglesArrayPath());
  }

  cDims[0] = 1;
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellPhasesPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getCellPhasesArrayPath());
  }

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(),
                                                                                                                cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CrystalStructuresPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(getUseGoodVoxels() == true)
  {
    // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to create it
    cDims[0] = 1;
    m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(),
                                                                                                       cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_GoodVoxelsPtr.lock().get())                                                                /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0)
    {
      dataArrayPaths.push_back(getGoodVoxelsArrayPath());
    }
  }
  else
  {
    m_GoodVoxels = nullptr;
  }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WritePoleFigure::preflight()
{
  setInPreflight(true);
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename Ops> QVector<UInt8ArrayType::Pointer> makePoleFigures(PoleFigureConfiguration_t& config)
{
  Ops ops;
  return ops.generatePoleFigure(config);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString WritePoleFigure::generateImagePath(QString label)
{
  QString path = m_OutputPath + "/" + m_ImagePrefix + label;
  if(m_ImageFormat == TifImageType)
  {
    path.append(".tif");
  }
  else if(m_ImageFormat == BmpImageType)
  {
    path.append(".bmp");
  }
  else if(m_ImageFormat == PngImageType)
  {
    path.append(".png");
  }
  else if(m_ImageFormat == JpgImageType)
  {
    path.append(".jpg");
  }
  path = QDir::toNativeSeparators(path);
  QFileInfo fi(path);
  QDir parent(fi.absolutePath());
  if(parent.exists() == false)
  {
    parent.mkpath(fi.absolutePath());
  }

  return path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WritePoleFigure::writeImage(QImage image, QString label)
{
  QString filename = generateImagePath(label);
  QString ss = QObject::tr("Writing Image %1").arg(filename);
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
  bool saved = image.save(filename);
  if(!saved)
  {
    setErrorCondition(-90011);
    QString ss = QObject::tr("The Pole Figure image file '%1' was not saved").arg(filename);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WritePoleFigure::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QDir path(getOutputPath());

  if(!path.mkpath("."))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(path.absolutePath());
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_CellPhasesArrayPath.getDataContainerName());

  // Find the total number of angles we have based on the number of Tuples of the
  // Euler Angles array
  size_t numPoints = m_CellEulerAnglesPtr.lock()->getNumberOfTuples();
  // Find how many phases we have by getting the number of Crystal Structures
  size_t numPhases = m_CrystalStructuresPtr.lock()->getNumberOfTuples();

  // Loop over all the voxels gathering the Eulers for a specific phase into an array
  for(size_t phase = 1; phase < numPhases; ++phase)
  {
    size_t count = 0;
    // First find out how many voxels we are going to have. This is probably faster to loop twice than to
    // keep allocating memory everytime we find one.
    for(size_t i = 0; i < numPoints; ++i)
    {
      if(m_CellPhases[i] == phase)
      {
        if(m_UseGoodVoxels == false || m_GoodVoxels[i] == true)
        {
          count++;
        }
      }
    }
    QVector<size_t> eulerCompDim(1, 3);
    FloatArrayType::Pointer subEulers = FloatArrayType::CreateArray(count, eulerCompDim, "Eulers_Per_Phase");
    subEulers->initializeWithValue(std::numeric_limits<float>::signaling_NaN());
    float* eu = subEulers->getPointer(0);

    // Now loop through the eulers again and this time add them to the subEulers Array
    count = 0;
    for(size_t i = 0; i < numPoints; ++i)
    {
      if(m_CellPhases[i] == phase)
      {
        if(m_UseGoodVoxels == false || m_GoodVoxels[i] == true)
        {
          eu[count * 3] = m_CellEulerAngles[i * 3];
          eu[count * 3 + 1] = m_CellEulerAngles[i * 3 + 1];
          eu[count * 3 + 2] = m_CellEulerAngles[i * 3 + 2];
          count++;
        }
      }
    }
    if(subEulers->getNumberOfTuples() == 0)
    {
      continue;
    } // Skip because we have no Pole Figure data

    QVector<UInt8ArrayType::Pointer> figures;

    PoleFigureConfiguration_t config;
    config.eulers = subEulers.get();
    config.imageDim = getImageSize();
    config.lambertDim = getLambertSize();
    config.numColors = getNumColors();

    QString label("Phase_");
    label.append(QString::number(phase));

    QString ss = QObject::tr("Generating Pole Figures for Phase %1").arg(phase);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

    switch(m_CrystalStructures[phase])
    {
    case Ebsd::CrystalStructure::Cubic_High:
      figures = makePoleFigures<CubicOps>(config);
      break;
    case Ebsd::CrystalStructure::Cubic_Low:
      figures = makePoleFigures<CubicLowOps>(config);
      break;
    case Ebsd::CrystalStructure::Hexagonal_High:
      figures = makePoleFigures<HexagonalOps>(config);
      break;
    case Ebsd::CrystalStructure::Hexagonal_Low:
      figures = makePoleFigures<HexagonalLowOps>(config);
      break;
    case Ebsd::CrystalStructure::Trigonal_High:
      //   figures = makePoleFigures<TrigonalOps>(config);
      setWarningCondition(-1010);
      notifyWarningMessage(getHumanLabel(), "Trigonal High Symmetry is not supported for Pole figures. This phase will be omitted from results", getWarningCondition());
      break;
    case Ebsd::CrystalStructure::Trigonal_Low:
      //  figures = makePoleFigures<TrigonalLowOps>(config);
      setWarningCondition(-1010);
      notifyWarningMessage(getHumanLabel(), "Trigonal Low Symmetry is not supported for Pole figures. This phase will be omitted from results", getWarningCondition());
      break;
    case Ebsd::CrystalStructure::Tetragonal_High:
      //  figures = makePoleFigures<TetragonalOps>(config);
      setWarningCondition(-1010);
      notifyWarningMessage(getHumanLabel(), "Tetragonal High Symmetry is not supported for Pole figures. This phase will be omitted from results", getWarningCondition());
      break;
    case Ebsd::CrystalStructure::Tetragonal_Low:
      //  figures = makePoleFigures<TetragonalLowOps>(config);
      setWarningCondition(-1010);
      notifyWarningMessage(getHumanLabel(), "Tetragonal Low Symmetry is not supported for Pole figures. This phase will be omitted from results", getWarningCondition());
      break;
    case Ebsd::CrystalStructure::OrthoRhombic:
      figures = makePoleFigures<OrthoRhombicOps>(config);
      break;
    case Ebsd::CrystalStructure::Monoclinic:
      figures = makePoleFigures<MonoclinicOps>(config);
      break;
    case Ebsd::CrystalStructure::Triclinic:
      figures = makePoleFigures<TriclinicOps>(config);
      break;
    default:
      break;
    }

    if(figures.size() == 3)
    {
      QImage combinedImage = PoleFigureImageUtilities::Create3ImagePoleFigure(figures[0].get(), figures[1].get(), figures[2].get(), config, getImageLayout());
      writeImage(combinedImage, label);
    }
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer WritePoleFigure::newFilterInstance(bool copyFilterParameters)
{
  WritePoleFigure::Pointer filter = WritePoleFigure::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WritePoleFigure::getCompiledLibraryName()
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WritePoleFigure::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WritePoleFigure::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WritePoleFigure::getGroupName()
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WritePoleFigure::getSubGroupName()
{
  return SIMPL::FilterSubGroups::OutputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WritePoleFigure::getHumanLabel()
{
  return "Export Pole Figure Images";
}
