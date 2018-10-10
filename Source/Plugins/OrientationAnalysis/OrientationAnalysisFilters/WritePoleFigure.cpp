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
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Utilities/ColorTable.h"

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

#include "EbsdLib/EbsdConstants.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#include "hpdf.h"
#include <setjmp.h>

jmp_buf env;

void error_handler  (HPDF_STATUS   error_no,
                HPDF_STATUS   detail_no,
                void* /* user_data */)
{
    printf ("ERROR: error_no=%04X, detail_no=%u\n", static_cast<HPDF_UINT>(error_no),
                static_cast<HPDF_UINT>(detail_no) );
    longjmp(env, 1);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WritePoleFigure::WritePoleFigure()
: m_ImagePrefix("")
, m_OutputPath("")
, m_ImageFormat(0)
, m_ImageSize(512)
, m_LambertSize(64)
, m_NumColors(32)
, m_ImageLayout(SIMPL::Layout::Square)
, m_CellEulerAnglesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::EulerAngles)
, m_CellPhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Phases)
, m_CrystalStructuresArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::CrystalStructures)
, m_GoodVoxelsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Mask)
, m_UseGoodVoxels(false)
, m_GenerationAlgorithm(0)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WritePoleFigure::~WritePoleFigure() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WritePoleFigure::setupFilterParameters()
{
  FilterParameterVector parameters;
  
  
   parameters.push_back(SIMPL_NEW_STRING_FP("Figure Title", Title, FilterParameter::Parameter, WritePoleFigure));

  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Pole Figure Type");
    parameter->setPropertyName("GenerationAlgorithm");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(WritePoleFigure, this, GenerationAlgorithm));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(WritePoleFigure, this, GenerationAlgorithm));

    parameter->setDefaultValue(0);

    QVector<QString> choices;
    choices.push_back("Lambert Square");
    choices.push_back("Discrete");

    parameter->setChoices(choices);
    QStringList linkedProps;
    linkedProps << "LambertSize"
                << "NumColors";
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  parameters.push_back(SIMPL_NEW_INTEGER_FP("Lambert Image Size (Pixels)", LambertSize, FilterParameter::Parameter, WritePoleFigure, 0));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Number of Colors", NumColors, FilterParameter::Parameter, WritePoleFigure, 0));

  // parameters.push_back(SIMPL_NEW_BOOL_FP("Generate Color Heat Map Style", UseDiscreteHeatMap, FilterParameter::Parameter, WritePoleFigure, 1));

  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Image Format");
    parameter->setPropertyName("ImageFormat");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(WritePoleFigure, this, ImageFormat));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(WritePoleFigure, this, ImageFormat));

    QVector<QString> choices;    
//    choices.push_back("tif");
//    choices.push_back("bmp");
//    choices.push_back("png");
    choices.push_back("pdf");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
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

  parameters.push_back(SIMPL_NEW_OUTPUT_PATH_FP("Output Path", OutputPath, FilterParameter::Parameter, WritePoleFigure));
  parameters.push_back(SIMPL_NEW_STRING_FP("Image Prefix", ImagePrefix, FilterParameter::Parameter, WritePoleFigure));
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
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::StringArray, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Material Name", MaterialNameArrayPath, FilterParameter::RequiredArray, WritePoleFigure, req));
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

  if(m_OutputPath.isEmpty())
  {
    setErrorCondition(-1003);
    notifyErrorMessage(getHumanLabel(), "The output directory must be set", getErrorCondition());
  }
  else if(!path.exists())
  {
    setWarningCondition(-1004);
    QString ss = QObject::tr("The directory path for the output file does not exist. DREAM.3D will attempt to create this path during execution of the filter");
    notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
  }

  QVector<DataArrayPath> dataArrayPaths;

  QVector<size_t> cDims(1, 3);
  m_CellEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellEulerAnglesArrayPath(),
                                                                                                           cDims); 
  if(nullptr != m_CellEulerAnglesPtr.lock())                                                                       
  {
    m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
  } 
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getCellEulerAnglesArrayPath());
  }

  cDims[0] = 1;
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(),
                                                                                                        cDims); 
  if(nullptr != m_CellPhasesPtr.lock())                                                                         
  {
    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  } 
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getCellPhasesArrayPath());
  }

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(),
                                                                                                                cDims); 
  if(nullptr != m_CrystalStructuresPtr.lock()) 
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  }
  
  cDims[0] = 1;
  m_MaterialNames = getDataContainerArray()->getPrereqArrayFromPath<StringDataArray, AbstractFilter>(this, getMaterialNameArrayPath(), cDims);
  
  if(getUseGoodVoxels())
  {
    // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to create it
    cDims[0] = 1;
    m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(),
                                                                                                       cDims); 
    if(nullptr != m_GoodVoxelsPtr.lock())                                                                      
    {
      m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0);
    } 
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
QString WritePoleFigure::generateImagePath(const QString &label)
{
  QString path = m_OutputPath + "/" + m_ImagePrefix + label;
//  if(m_ImageFormat == TifImageType)
//  {
//    path.append(".tif");
//  }
//  else if(m_ImageFormat == BmpImageType)
//  {
//    path.append(".bmp");
//  }
//  else if(m_ImageFormat == PngImageType)
//  {
//    path.append(".png");
//  }
//  else if(m_ImageFormat == JpgImageType)
//  {
//    path.append(".jpg");
//  }
//  else if(m_ImageFormat == PdfImageType)
  {
    path.append(".pdf");
  }
  path = QDir::toNativeSeparators(path);
  QFileInfo fi(path);
  QDir parent(fi.absolutePath());
  if(!parent.exists())
  {
    parent.mkpath(fi.absolutePath());
  }

  return path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#if 0
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
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UInt8ArrayType::Pointer flipAndMirrorPoleFigure(UInt8ArrayType* src, const PoleFigureConfiguration_t config)
{
  UInt8ArrayType::Pointer converted = UInt8ArrayType::CreateArray(static_cast<size_t>(config.imageDim * config.imageDim), 
                                                                  QVector<size_t>(1,3), 
                                                                  src->getName());
  // We need to flip the image "vertically", which means the bottom row becomes
  // the top row and convert from BGRA to RGB ordering (This is a Little Endian code)
  // If this is ever compiled on a BIG ENDIAN machine the colors will be off.
  for(int y = 0; y < config.imageDim; y++)
  {
    int destY = config.imageDim - 1 - y;
    for(int x = 0; x < config.imageDim; x++)
    {
      size_t indexSrc = static_cast<size_t>(y * config.imageDim + x);
      size_t indexDest = static_cast<size_t>(destY * config.imageDim + x);
      
      uint8_t* argb = src->getTuplePointer(indexSrc);
      uint8_t* dest = converted->getTuplePointer(indexDest);
      
      dest[0] = argb[2];
      dest[1] = argb[1];
      dest[2] = argb[0];
    }
  }
  return converted;
}

#define FLIP_Y(y,height)\
(height)-(y)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void drawScalarBar(HPDF_Page page, const PoleFigureConfiguration_t &config, 
                          const std::pair<HPDF_REAL, HPDF_REAL> &position,
                          float margins,
                          float fontPtSize,
                          HPDF_Font font,
                          int32_t phaseNum,
                          const QString& laueGroupName,
                          const QString& materialName)
{

  int numColors = config.numColors;

  // Get all the colors that we will need
  QVector<SIMPL::Rgb> colorTable(numColors);
  QVector<float> colors(3 * numColors, 0.0);
  SIMPLColorTable::GetColorTable(numColors, colors); // Generate the color table values
  float r = 0.0, g = 0.0, b = 0.0;
  for(int i = 0; i < numColors; i++) // Convert them to QRgbColor values
  {
    r = colors[3 * i];
    g = colors[3 * i + 1];
    b = colors[3 * i + 2];
    colorTable[i] = RgbColor::dRgb(r * 255, g * 255, b * 255, 255);
  }

  // Now start from the bottom and draw colored lines up the scale bar
  // A Slight Indentation for the scalar bar
  float scaleBarRelativeWidth = 0.10f;
  
  int imageHeight = config.imageDim;
  int imageWidth = config.imageDim;
  float colorHeight = (static_cast<float>(imageHeight)) / static_cast<float>(numColors);
  
  using RectF = std::pair<float, float>;

  RectF rect = std::make_pair(imageWidth * scaleBarRelativeWidth, colorHeight*1.00000f);  
  
  
  // Draw the Max Value
  QString maxStr = QString::number(config.maxScale, 'f', 3);
  HPDF_Page_SetRGBStroke (page, 0.0f, 0.0f, 0.0f);
  HPDF_Page_SetGrayStroke (page, 0.00f);
  HPDF_Page_BeginText (page);
  HPDF_Page_SetFontAndSize (page, font, fontPtSize);
  HPDF_Page_MoveTextPos (page, position.first + margins + rect.first + margins, position.second + margins + fontPtSize + (numColors-2)*rect.second);
  HPDF_Page_ShowText (page, maxStr.toLatin1());
  HPDF_Page_EndText (page);
  // Draw the Min value
  QString minStr = QString::number(config.minScale, 'f', 3);
  HPDF_Page_SetRGBStroke (page, 0.0f, 0.0f, 0.0f);
  HPDF_Page_SetGrayStroke (page, 0.00f);
  HPDF_Page_BeginText (page);
  HPDF_Page_SetFontAndSize (page, font, fontPtSize);
  HPDF_Page_MoveTextPos (page, position.first + margins + rect.first + margins, position.second + margins + fontPtSize);
  HPDF_Page_ShowText (page, minStr.toLatin1());
  HPDF_Page_EndText (page);
  
  
  
  QString label;  
  
  // Draw the Number of Samples
  label = QString("Phase Num: ") + QString::number(phaseNum);
  HPDF_Page_SetRGBStroke (page, 0.0f, 0.0f, 0.0f);
  HPDF_Page_SetGrayStroke (page, 0.00f);
  HPDF_Page_BeginText (page);
  HPDF_Page_SetFontAndSize (page, font, fontPtSize);
  HPDF_Page_MoveTextPos (page, position.first + margins + rect.first + margins, position.second + margins + fontPtSize + rect.second*numColors/1.5);
  HPDF_Page_ShowText (page, label.toLatin1());
  HPDF_Page_EndText (page);

  label = QString("Phase Name: ") + materialName;
  HPDF_Page_SetRGBStroke (page, 0.0f, 0.0f, 0.0f);
  HPDF_Page_SetGrayStroke (page, 0.00f);
  HPDF_Page_BeginText (page);
  HPDF_Page_SetFontAndSize (page, font, fontPtSize);
  HPDF_Page_MoveTextPos (page, position.first + margins + rect.first + margins, position.second + margins + fontPtSize + rect.second*numColors/1.5 - (fontPtSize*1.10f) );
  HPDF_Page_ShowText (page, label.toLatin1());
  HPDF_Page_EndText (page);

  label = QString("Laue Group: ") + laueGroupName;
  HPDF_Page_SetRGBStroke (page, 0.0f, 0.0f, 0.0f);
  HPDF_Page_SetGrayStroke (page, 0.00f);
  HPDF_Page_BeginText (page);
  HPDF_Page_SetFontAndSize (page, font, fontPtSize);
  HPDF_Page_MoveTextPos (page, position.first + margins + rect.first + margins, position.second + margins + fontPtSize + rect.second*numColors/1.5- (fontPtSize*2.20f) );
  HPDF_Page_ShowText (page, label.toLatin1());
  HPDF_Page_EndText (page);
  
  label = QString("Upper & Lower");
  HPDF_Page_SetRGBStroke (page, 0.0f, 0.0f, 0.0f);
  HPDF_Page_SetGrayStroke (page, 0.00f);
  HPDF_Page_BeginText (page);
  HPDF_Page_SetFontAndSize (page, font, fontPtSize);
  HPDF_Page_MoveTextPos (page, position.first + margins + rect.first + margins, position.second + margins + fontPtSize + rect.second*numColors/1.5 - (fontPtSize*3.30f) );
  HPDF_Page_ShowText (page, label.toLatin1());
  HPDF_Page_EndText (page);
  
  label = QString("Samples: ") + QString::number(config.eulers->getNumberOfTuples());
  HPDF_Page_SetRGBStroke (page, 0.0f, 0.0f, 0.0f);
  HPDF_Page_SetGrayStroke (page, 0.00f);
  HPDF_Page_BeginText (page);
  HPDF_Page_SetFontAndSize (page, font, fontPtSize);
  HPDF_Page_MoveTextPos (page, position.first + margins + rect.first + margins, position.second + margins + fontPtSize + rect.second*numColors/1.5 - (fontPtSize*4.40f) );
  HPDF_Page_ShowText (page, label.toLatin1());
  HPDF_Page_EndText (page);  
  
  label = QString("Lambert Sq. Dim: ") + QString::number(config.lambertDim);
  HPDF_Page_SetRGBStroke (page, 0.0f, 0.0f, 0.0f);
  HPDF_Page_SetGrayStroke (page, 0.00f);
  HPDF_Page_BeginText (page);
  HPDF_Page_SetFontAndSize (page, font, fontPtSize);
  HPDF_Page_MoveTextPos (page, position.first + margins + rect.first + margins, position.second + margins + fontPtSize + rect.second*numColors/1.5 - (fontPtSize*5.50f) );
  HPDF_Page_ShowText (page, label.toLatin1());
  HPDF_Page_EndText (page);  
  
  HPDF_Page_SetGrayStroke (page, 0.30f);
  for(int i = 0; i < numColors; i++)
  {
    SIMPL::Rgb c = colorTable[i];
    HPDF_REAL r, g, b;
    std::tie(r, g, b) = RgbColor::fRgb(c);
    
    HPDF_REAL x = position.first + margins;
    HPDF_REAL y = position.second + margins + fontPtSize + (i * colorHeight);
    
    
    HPDF_Page_SetRGBFill(page, r, g, b);
    HPDF_Page_Rectangle(page, x, y, rect.first, rect.second);
    HPDF_Page_Fill(page);
    
    HPDF_Page_SetRGBStroke(page, r, g, b);
    HPDF_Page_Rectangle(page, x, y, rect.first, rect.second);
    HPDF_Page_Stroke(page);
    
  }
  
  HPDF_Page_SetGrayStroke (page, 0.0f);
  HPDF_Page_MoveTo   (page, position.first + margins, position.second + margins + fontPtSize);
  HPDF_Page_Rectangle(page, position.first + margins, position.second + margins + fontPtSize, 
                      rect.first, numColors * rect.second);
  HPDF_Page_Stroke(page);
  
  
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void drawDiscreteInfoArea(HPDF_Page page, const PoleFigureConfiguration_t &config, 
                          const std::pair<HPDF_REAL, HPDF_REAL> &position,
                          float margins,
                          float fontPtSize,
                          HPDF_Font font,
                          int32_t phaseNum,
                          const QString& laueGroupName,
                          const QString& materialName)
{

  int numColors = config.numColors;

  // Now start from the bottom and draw colored lines up the scale bar
  // A Slight Indentation for the scalar bar
  float scaleBarRelativeWidth = 0.01f;
  
  int imageHeight = config.imageDim;
  int imageWidth = config.imageDim;
  float colorHeight = (static_cast<float>(imageHeight)) / static_cast<float>(numColors);
  
  using RectF = std::pair<float, float>;

  RectF rect = std::make_pair(imageWidth * scaleBarRelativeWidth, colorHeight*1.00000f);  

  QString label;  
  
  // Draw the Number of Samples
  label = QString("Phase Num: ") + QString::number(phaseNum);
  HPDF_Page_SetRGBStroke (page, 0.0f, 0.0f, 0.0f);
  HPDF_Page_SetGrayStroke (page, 0.00f);
  HPDF_Page_BeginText (page);
  HPDF_Page_SetFontAndSize (page, font, fontPtSize);
  HPDF_Page_MoveTextPos (page, position.first + margins + rect.first + margins, position.second + margins + fontPtSize + rect.second*numColors);
  HPDF_Page_ShowText (page, label.toLatin1());
  HPDF_Page_EndText (page);

  label = QString("Phase Name: ") + materialName;
  HPDF_Page_SetRGBStroke (page, 0.0f, 0.0f, 0.0f);
  HPDF_Page_SetGrayStroke (page, 0.00f);
  HPDF_Page_BeginText (page);
  HPDF_Page_SetFontAndSize (page, font, fontPtSize);
  HPDF_Page_MoveTextPos (page, position.first + margins + rect.first + margins, position.second + margins + fontPtSize + rect.second*numColors - (fontPtSize*1.10f) );
  HPDF_Page_ShowText (page, label.toLatin1());
  HPDF_Page_EndText (page);

  label = QString("Laue Group: ") + laueGroupName;
  HPDF_Page_SetRGBStroke (page, 0.0f, 0.0f, 0.0f);
  HPDF_Page_SetGrayStroke (page, 0.00f);
  HPDF_Page_BeginText (page);
  HPDF_Page_SetFontAndSize (page, font, fontPtSize);
  HPDF_Page_MoveTextPos (page, position.first + margins + rect.first + margins, position.second + margins + fontPtSize + rect.second*numColors- (fontPtSize*2.20f) );
  HPDF_Page_ShowText (page, label.toLatin1());
  HPDF_Page_EndText (page);
  
  label = QString("Upper & Lower");
  HPDF_Page_SetRGBStroke (page, 0.0f, 0.0f, 0.0f);
  HPDF_Page_SetGrayStroke (page, 0.00f);
  HPDF_Page_BeginText (page);
  HPDF_Page_SetFontAndSize (page, font, fontPtSize);
  HPDF_Page_MoveTextPos (page, position.first + margins + rect.first + margins, position.second + margins + fontPtSize + rect.second*numColors - (fontPtSize*3.30f) );
  HPDF_Page_ShowText (page, label.toLatin1());
  HPDF_Page_EndText (page);
  
  label = QString("Samples: ") + QString::number(config.eulers->getNumberOfTuples());
  HPDF_Page_SetRGBStroke (page, 0.0f, 0.0f, 0.0f);
  HPDF_Page_SetGrayStroke (page, 0.00f);
  HPDF_Page_BeginText (page);
  HPDF_Page_SetFontAndSize (page, font, fontPtSize);
  HPDF_Page_MoveTextPos (page, position.first + margins + rect.first + margins, position.second + margins + fontPtSize + rect.second*numColors - (fontPtSize*4.40f) );
  HPDF_Page_ShowText (page, label.toLatin1());
  HPDF_Page_EndText (page);  
  
  
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
        if(!m_UseGoodVoxels || m_GoodVoxels[i])
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
        if(!m_UseGoodVoxels || m_GoodVoxels[i])
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
    if(static_cast<WritePoleFigure::Algorithm>(getGenerationAlgorithm()) == WritePoleFigure::Algorithm::LambertProjection)
    {
      config.discrete = false;
    }
    else
    {
      config.discrete = true;
    }

    config.discreteHeatMap = m_UseDiscreteHeatMap;

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
      figures = makePoleFigures<TrigonalOps>(config);
      //   setWarningCondition(-1010);
      //   notifyWarningMessage(getHumanLabel(), "Trigonal High Symmetry is not supported for Pole figures. This phase will be omitted from results", getWarningCondition());
      break;
    case Ebsd::CrystalStructure::Trigonal_Low:
      figures = makePoleFigures<TrigonalLowOps>(config);
      //  setWarningCondition(-1010);
      //  notifyWarningMessage(getHumanLabel(), "Trigonal Low Symmetry is not supported for Pole figures. This phase will be omitted from results", getWarningCondition());
      break;
    case Ebsd::CrystalStructure::Tetragonal_High:
      figures = makePoleFigures<TetragonalOps>(config);
    //  setWarningCondition(-1010);
    //  notifyWarningMessage(getHumanLabel(), "Tetragonal High Symmetry is not supported for Pole figures. This phase will be omitted from results", getWarningCondition());
      break;
    case Ebsd::CrystalStructure::Tetragonal_Low:
      figures = makePoleFigures<TetragonalLowOps>(config);
      // setWarningCondition(-1010);
      // notifyWarningMessage(getHumanLabel(), "Tetragonal Low Symmetry is not supported for Pole figures. This phase will be omitted from results", getWarningCondition());
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
      QString filename = generateImagePath(label);

      HPDF_Doc  pdf = HPDF_New (error_handler, nullptr);
      
      HPDF_SetCompressionMode (pdf, HPDF_COMP_ALL);
      
      /* create default-font */
      HPDF_Font font = HPDF_GetFont (pdf, "Helvetica", nullptr);
      
      HPDF_BYTE buf[1] = { 'X' };
      
      HPDF_TextWidth textWidth = HPDF_Font_TextWidth(font, buf, 1);
      
      HPDF_REAL pageWidth = 0.0f;
      HPDF_REAL pageHeight = 0.0f;

      HPDF_UINT imageWidth = static_cast<HPDF_UINT>(config.imageDim);
      HPDF_UINT imageHeight = static_cast<HPDF_UINT>(config.imageDim);
      HPDF_REAL fontPtSize = imageHeight / 16.0f;
      HPDF_REAL margins = imageHeight / 32.0f;

      HPDF_REAL xCharWidth = textWidth.width * fontPtSize / 1000;
      
      // Each Pole Figure gets it's own Square mini canvas to draw into.
      HPDF_REAL subCanvasWidth = margins + imageWidth + xCharWidth + margins;
      HPDF_REAL subCanvasHeight = margins + fontPtSize + imageHeight + fontPtSize*2 + margins*2;

      
      QVector<std::pair<HPDF_REAL, HPDF_REAL>> imagePositions(4);
      if(m_ImageLayout == SIMPL::Layout::Horizontal)
      {
        pageWidth = subCanvasWidth * 4;
        pageHeight = subCanvasHeight;
        imagePositions[0] = std::make_pair(0, 0);
        imagePositions[1] = std::make_pair(subCanvasWidth, 0);
        imagePositions[2] = std::make_pair(subCanvasWidth * 2, 0);
        imagePositions[3] = std::make_pair(subCanvasWidth * 3, 0);
      }
      else if(m_ImageLayout == SIMPL::Layout::Vertical)
      {
        pageWidth = subCanvasWidth;
        pageHeight = subCanvasHeight * 4.0f;
        imagePositions[0] = std::make_pair(0, subCanvasHeight * 3);
        imagePositions[1] = std::make_pair(0, subCanvasHeight * 2);
        imagePositions[2] = std::make_pair(0, subCanvasHeight * 1);
        imagePositions[3] = std::make_pair(0, 0);
      }
      else if(m_ImageLayout == SIMPL::Layout::Square)
      {
        pageWidth =subCanvasWidth * 2;
        pageHeight = subCanvasHeight * 2;
        imagePositions[0] = std::make_pair(0, subCanvasHeight); // Upper Left
        imagePositions[1] = std::make_pair(subCanvasWidth, subCanvasHeight); // Upper Right
        imagePositions[2] = std::make_pair(0, 0); // Lower Left
        imagePositions[3] = std::make_pair(subCanvasWidth, 0); // Lower Right
      }
      
      pageHeight = pageHeight + margins + fontPtSize;

      /* add a new page object. */
      HPDF_Page page = HPDF_AddPage (pdf);
      HPDF_Page_SetWidth (page, pageWidth);
      HPDF_Page_SetHeight (page, pageHeight);
      
      QVector<HPDF_Image> pdfImages(figures.size());
      HPDF_ColorSpace colorSpace = HPDF_CS_DEVICE_RGB;
      for (int a = 0; a < figures.size(); a++)
      {
        figures[a] = flipAndMirrorPoleFigure(figures[a].get(), config);
        HPDF_Image image = HPDF_LoadRawImageFromMem(pdf,
                                                    figures[a]->getPointer(0),
                                                    static_cast<HPDF_UINT>(config.imageDim),
                                                    static_cast<HPDF_UINT>(config.imageDim),
                                                    colorSpace,
                                                    8);
        pdfImages[a] = image;                                            
      }
      
      
      // Create the Scalar Bar image
      //QImage scalarBar = PoleFigureImageUtilities::GenerateScalarBar(img0.width(), imageHeight, config); 
      
      for(int i = 0; i < 3; i++)
      {
          HPDF_REAL x = 0.0f;
          HPDF_REAL y = 0.0f;

          std::tie(x, y) = imagePositions[i];

          /* Draw image to the canvas. (normal-mode with actual size.)*/
          HPDF_Page_DrawImage (page, pdfImages[i], x + margins, y+ margins+fontPtSize, imageWidth, imageHeight);

          QString text = figures[i]->getName();
          std::vector<uint8_t> buffer;
          QString modText;
          int k = 0;
          while(k < text.length())
          {
              if(text.at(k) == '-')
              {
                  HPDF_TextWidth tw = HPDF_Font_TextWidth(font, buffer.data(), buffer.size());
                  int32_t scaledOffset = (tw.width * fontPtSize / 1000);

                  buffer.push_back(static_cast<uint8_t>(text.at(k+1).toLatin1()));
                  tw = HPDF_Font_TextWidth(font, buffer.data(), buffer.size());
                  int32_t scaledOffset2 = (tw.width * fontPtSize / 1000);
                  buffer.pop_back();

                  int32_t diff = (scaledOffset2 - scaledOffset);

                  buf[0] = '-';
                  tw = HPDF_Font_TextWidth(font, buf, 1);
                  int32_t charWidth = (tw.width * fontPtSize / 1000);
                  scaledOffset = scaledOffset + (diff - charWidth)/2;
                  // Draw the bar at the correct location...
                  std::string underScore("-");
                  HPDF_Page_BeginText (page);
                  HPDF_Page_SetFontAndSize (page, font, fontPtSize);
                  HPDF_Page_MoveTextPos (page, x + margins + scaledOffset, y+margins+fontPtSize+imageHeight+fontPtSize*1.60+ margins);
                  HPDF_Page_ShowText (page, underScore.c_str());
                  HPDF_Page_EndText (page);

                  k++;
              }
              // Keep building up the string to finally print.
              if(text.at(k) != '-')
              {
                  modText = modText + text.at(k);
                  buffer.push_back(static_cast<uint8_t>(text.at(k).toLatin1()));
              }
              k++;
          }

        HPDF_Page_BeginText (page);
        HPDF_Page_SetFontAndSize (page, font, fontPtSize);

        HPDF_Page_MoveTextPos (page, x + margins, y+margins+fontPtSize+imageHeight+fontPtSize+ margins);
        HPDF_Page_ShowText (page, modText.toLatin1());
        HPDF_Page_EndText (page);

        HPDF_REAL lineWidth = imageWidth / 512.0f;
        if(lineWidth < 1.0f)
        {
          lineWidth = 1.0f;
        }

        HPDF_Page_SetLineWidth (page, lineWidth);
        HPDF_Page_SetRGBStroke (page, 0.0f, 0.0f, 0.0f);
        HPDF_Page_SetGrayStroke (page, 0.30f);

        // Draw
        HPDF_Page_Circle (page, x + margins + imageWidth/2.0f, y+ margins + fontPtSize + imageWidth/2.0f, imageWidth/2.0f);
        // Draw the Horizontal Axis
        HPDF_Page_MoveTo (page, x + margins, y+ margins + fontPtSize + imageWidth/2.0f);
        HPDF_Page_LineTo (page, x + margins + imageWidth, y+ margins + fontPtSize + imageWidth/2.0f);
        // Draw the Vertical Axis
        HPDF_Page_MoveTo (page, x + margins + imageWidth/2.0f, y + margins + fontPtSize);
        HPDF_Page_LineTo (page, x + margins + imageWidth/2.0f, y + margins + fontPtSize + imageHeight);    
        HPDF_Page_Stroke (page);
        
        // Label the X Axis
        HPDF_Page_BeginText (page);
        HPDF_Page_SetFontAndSize (page, font, fontPtSize);
        HPDF_Page_MoveTextPos (page, x + margins + imageWidth, y+margins+fontPtSize+imageHeight/2.0f - fontPtSize*.33f);
        HPDF_Page_ShowText (page, " X");
        HPDF_Page_EndText (page);
        
        // Label the Y Axis
        HPDF_Page_BeginText (page);
        HPDF_Page_SetFontAndSize (page, font, fontPtSize);
        HPDF_Page_MoveTextPos (page, x + margins + imageWidth/2.0f - xCharWidth/2.0f, y+margins + fontPtSize + imageHeight + fontPtSize*0.10f);
        HPDF_Page_ShowText (page, "Y");
        HPDF_Page_EndText (page);
      }
      
      std::vector<QString> laueNames = LaueOps::GetLaueNames();
      uint32_t laueIndex = m_CrystalStructures[phase];
      // Draw the title onto the canvas
      QString materialName = m_MaterialNames->getValue(phase);
      QString fullTitle = QString("%1").arg(getTitle());
      HPDF_Page_BeginText (page);
      HPDF_Page_SetFontAndSize (page, font, imageHeight / 12.0f);
      HPDF_Page_MoveTextPos (page, margins, pageHeight - margins/2.0f - fontPtSize);
      HPDF_Page_ShowText (page, fullTitle.toLatin1());
      HPDF_Page_EndText (page);  
      
      // Now draw the Color Scalar Bar if needed.
      if(config.discrete)
      {
        drawDiscreteInfoArea(page, config, imagePositions[3], margins, imageHeight / 20.0f, font, phase, laueNames[laueIndex], materialName);
      }
      else
      {
        drawScalarBar(page, config, imagePositions[3], margins, imageHeight / 20.0f, font, phase, laueNames[laueIndex], materialName);
      }
      
      /* save the document to a file */
      HPDF_SaveToFile (pdf, filename.toLatin1());
      
      /* clean up */
      HPDF_Free (pdf);
    }
  }
  
  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer WritePoleFigure::newFilterInstance(bool copyFilterParameters) const
{
  WritePoleFigure::Pointer filter = WritePoleFigure::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WritePoleFigure::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WritePoleFigure::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WritePoleFigure::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WritePoleFigure::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid WritePoleFigure::getUuid()
{
  return QUuid("{a10bb78e-fcff-553d-97d6-830a43c85385}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WritePoleFigure::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::OutputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WritePoleFigure::getHumanLabel() const
{
  return "Export Pole Figure Images";
}
