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


#include "SPParksTextReader.h"

#include <QtCore/QFileInfo>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "IO/IOConstants.h"

// Include the MOC generated file for this class
#include "moc_SPParksTextReader.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SPParksTextReader::SPParksTextReader() :
  FileReader(),
  m_VolumeDataContainerName(DREAM3D::Defaults::ImageDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_InputFile(""),
  m_OneBasedArrays(false),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds)
{
  m_Origin.x = 0.0f;
  m_Origin.y = 0.0f;
  m_Origin.z = 0.0f;

  m_Resolution.x = 1.0f;
  m_Resolution.y = 1.0f;
  m_Resolution.z = 1.0f;

  m_Dims[0] = 0;
  m_Dims[1] = 0;
  m_Dims[2] = 0;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SPParksTextReader::~SPParksTextReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SPParksTextReader::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(InputFileFilterParameter::New("Input File", "InputFile", getInputFile(), FilterParameter::Parameter, "*.dump", "SPParks Dump File"));
  parameters.push_back(FloatVec3FilterParameter::New("Origin", "Origin", getOrigin(), FilterParameter::Parameter));
  parameters.push_back(FloatVec3FilterParameter::New("Resolution", "Resolution", getResolution(), FilterParameter::Parameter));
  parameters.push_back(BooleanFilterParameter::New("One Based Arrays", "OneBasedArrays", getOneBasedArrays(), FilterParameter::Parameter));
  parameters.push_back(StringFilterParameter::New("Data Container", "VolumeDataContainerName", getVolumeDataContainerName(), FilterParameter::CreatedArray));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Cell Attribute Matrix", "CellAttributeMatrixName", getCellAttributeMatrixName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Feature Ids", "FeatureIdsArrayName", getFeatureIdsArrayName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SPParksTextReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setVolumeDataContainerName(reader->readString("VolumeDataContainerName", getVolumeDataContainerName() ) );
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName() ) );
  setFeatureIdsArrayName(reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName() ) );
  setInputFile( reader->readString( "InputFile", getInputFile() ) );
  setOrigin( reader->readFloatVec3("Origin", getOrigin() ) );
  setResolution( reader->readFloatVec3("Resolution", getResolution() ) );
  setOneBasedArrays( reader->readValue("OneBasedArrays", getOneBasedArrays() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SPParksTextReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(VolumeDataContainerName)
  SIMPL_FILTER_WRITE_PARAMETER(CellAttributeMatrixName)
  SIMPL_FILTER_WRITE_PARAMETER(FeatureIdsArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(InputFile)
  SIMPL_FILTER_WRITE_PARAMETER(Origin)
  SIMPL_FILTER_WRITE_PARAMETER(Resolution)
  SIMPL_FILTER_WRITE_PARAMETER(OneBasedArrays)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SPParksTextReader::updateCellInstancePointers()
{
  setErrorCondition(0);

  //if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  //{ m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SPParksTextReader::dataCheck()
{
  setErrorCondition(0);

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getVolumeDataContainerName());
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> tDims(3, 0);
  m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }

  // Creating a Feature Ids array here in preflight so that it appears in the current data structure
  // This is a temporary array that will be overwritten by the correct array at the end of reading the file
  QVector<size_t> cDims(1, 1);
  DataArrayPath fIdsPath(getVolumeDataContainerName(), getCellAttributeMatrixName(), getFeatureIdsArrayName());
  getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, fIdsPath, 0, cDims);

  QFileInfo fi(getInputFile());
  if (getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("The input file must be set");
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if (fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(DREAM3D::Geometry::ImageGeometry);
  m->setGeometry(image);

  if (getInputFile().isEmpty() == false && fi.exists() == true)
  {
    // We need to read the header of the input file to get the dimensions
    m_InStream.setFileName(getInputFile());

    if (!m_InStream.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QString msg = QObject::tr("Input SPParks file could not be opened: %1").arg(getInputFile());
      setErrorCondition(-102);
      notifyErrorMessage(getHumanLabel(), msg, getErrorCondition());
    }
    else
    {
      int32_t error = readHeader();
      m_InStream.close();
      if (error < 0)
      {
        setErrorCondition(error);
        QString ss = QObject::tr("Error occurred trying to parse the dimensions from the input file");
        notifyErrorMessage(getHumanLabel(), ss, -48010);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SPParksTextReader::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SPParksTextReader::execute()
{
  int32_t err = 0;
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  m_InStream.setFileName(getInputFile());
  m_InStream.open(QFile::ReadOnly);

  err = readHeader();
  if (err < 0)
  {
    m_InStream.close();
    return;
  }
  err = readFile();
  m_InStream.close();
  if(err < 0)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t SPParksTextReader::readHeader()
{
  /*
  ITEM: TIMESTEP
  210    21000.6
  ITEM: NUMBER OF ATOMS
  106480
  ITEM: BOX BOUNDS
  0.5 44.5
  0.5 44.5
  0.5 55.5
  ITEM: ATOMS id type x y z
  */

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getVolumeDataContainerName());

  int32_t oneBase = 0;
  if (getOneBasedArrays()) { oneBase = 1; }

  bool ok = false; // Use this to verify that the comversions from string to numbers actually works.
  // We are going to reuse the 'buf' variable
  QByteArray buf = m_InStream.readLine(); //ITEM: TIMESTEP
  buf = m_InStream.readLine(); // 210    21000.6
  buf = m_InStream.readLine(); // ITEM: NUMBER OF ATOMS
  buf = m_InStream.readLine(); // 106480
  buf = buf.trimmed();
  int64_t numAtoms = buf.toInt(&ok); // Parse out the number of atoms
  buf = m_InStream.readLine(); // ITEM: BOX BOUNDS

  buf = m_InStream.readLine(); // 0.5 44.5
  buf = buf.trimmed();
  QList<QByteArray> tokens = buf.split(' ');
  float low = tokens[0].toFloat(&ok);
  float high = tokens[1].toFloat(&ok);
  int64_t nx = static_cast<int64_t>(floor(high - 0.01f) - ceil(low)) + oneBase;

  buf = m_InStream.readLine(); // 0.5 44.5
  buf = buf.trimmed();
  tokens = buf.split(' ');
  low = tokens[0].toFloat(&ok);
  high = tokens[1].toFloat(&ok);
  int64_t ny = static_cast<int64_t>(floor(high - 0.01f) - ceil(low)) + oneBase;

  buf = m_InStream.readLine(); // 0.5 55.5
  buf = buf.trimmed();
  tokens = buf.split(' ');
  low = tokens[0].toFloat(&ok);
  high = tokens[1].toFloat(&ok);
  int64_t nz = static_cast<int64_t>(floor(high - 0.01f) - ceil(low)) + oneBase;

  if (numAtoms != nx * ny * nz)
  {
    QString msg = QObject::tr("Number of sites does not match the calculated number of sites %1 != %2 * %3 * %4").arg(numAtoms).arg(nx).arg(ny).arg(nz);
    setErrorCondition(-101);
    notifyErrorMessage(getHumanLabel(), msg, getErrorCondition());
    return -100;
  }
  m->getGeometryAs<ImageGeom>()->setDimensions(nx, ny, nz);
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t SPParksTextReader::readFile()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getVolumeDataContainerName());
  // The readHeader() function should have set the dimensions correctly
  size_t totalPoints = m->getGeometryAs<ImageGeom>()->getNumberOfElements();

  QVector<size_t> tDims(3, 0);
  tDims[0] = m->getGeometryAs<ImageGeom>()->getXPoints();
  tDims[1] = m->getGeometryAs<ImageGeom>()->getYPoints();
  tDims[2] = m->getGeometryAs<ImageGeom>()->getZPoints();
  m->getAttributeMatrix(getCellAttributeMatrixName())->resizeAttributeArrays(tDims);
  updateCellInstancePointers();

  QByteArray buf = m_InStream.readLine(); // ITEM: ATOMS id type x y z
  buf = buf.trimmed();
  QList<QByteArray> tokens = buf.split(' '); // Tokenize the array with a tab
  Ebsd::NumType pType = Ebsd::UnknownNumType;

  int64_t xCol = 0;
  int64_t yCol = 0;
  int64_t zCol = 0;
  qint32 size = tokens.size();
  bool didAllocate = false;
  for (qint32 i = 2; i < size; ++i)
  {
    QString name = QString::fromLatin1(tokens[i]);
    pType = getPointerType(name);

    // We don't need to parse the X, Y & Z or id values into arrays.
    if (name.compare("x") == 0) { xCol = i - 2; continue;}
    if (name.compare("y") == 0) { yCol = i - 2; continue;}
    if (name.compare("z") == 0) { zCol = i - 2; continue;}
    if (name.compare("id") == 0) { zCol = i - 2; continue;}

    if (Ebsd::Int32 == pType)
    {
      Int32Parser::Pointer dparser = Int32Parser::New(NULL, totalPoints, name, i - 2);
      if ((didAllocate = dparser->allocateArray(totalPoints)) == true)
      {
        ::memset(dparser->getVoidPointer(), 0xAB, sizeof(int32_t) * totalPoints);
        m_NamePointerMap.insert(name, dparser);
      }
    }
    else if (Ebsd::Float == pType)
    {
      FloatParser::Pointer dparser = FloatParser::New(NULL, totalPoints, name, i - 2);
      if ((didAllocate = dparser->allocateArray(totalPoints)) == true)
      {
        ::memset(dparser->getVoidPointer(), 0xAB, sizeof(float) * totalPoints);
        m_NamePointerMap.insert(name, dparser);
      }
    }
    else
    {
      QString msg = QObject::tr("Column header %1 is not a recognized column for SPParks files. Please recheck your file and report this error to the DREAM.3D developers").arg(QString(tokens[i]));
      setErrorCondition(-107);
      notifyErrorMessage(getHumanLabel(), msg, getErrorCondition());
      return getErrorCondition();
    }

    if (didAllocate == false)
    {
      QString msg = QObject::tr("Unable to allocate memory for the data");
      setErrorCondition(-106);
      notifyErrorMessage(getHumanLabel(), msg, getErrorCondition());
      return getErrorCondition();
    }
  }

  // Now loop over all the points in the file. This could get REALLY slow if the file gets large.
  // Thank goodness we all like to save text files
  for (size_t n = 0; n < totalPoints; ++n)
  {
    buf = m_InStream.readLine(); // Read the line into a QByteArray including the newline
    buf = buf.trimmed(); // Remove leading and trailing whitespace

    if (m_InStream.atEnd() == true && buf.isEmpty() == true) // We have to have read to the end of the file AND the buffer is empty
    {
      // otherwise we read EXACTLY the last line and we still need to parse the line.
      break;
    }
    parseDataLine(buf, tDims, xCol, yCol, zCol);
  }

  DataParser::Pointer parser = m_NamePointerMap["type"];
  if (NULL != parser.get() )
  {
    QVector<size_t> cDims(1, 1);
    // Create a new DataArray that wraps the already allocated memory
    Int32ArrayType::Pointer typePtr = Int32ArrayType::WrapPointer(static_cast<int*>(parser->getVoidPointer()), totalPoints, cDims, getFeatureIdsArrayName(), true);
    // Release the DataParser from having to delete the memory
    parser->setManageMemory(false);

    AttributeMatrix::Pointer attrMat = m->getAttributeMatrix(getCellAttributeMatrixName());
    if (NULL != attrMat.get())
    {
      attrMat->addAttributeArray(typePtr->getName(), typePtr);
    }
  }

  // Now set the Resolution and Origin that the user provided on the GUI or as parameters
  m->getGeometryAs<ImageGeom>()->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  m->getGeometryAs<ImageGeom>()->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);

  notifyStatusMessage(getHumanLabel(), "Complete");
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SPParksTextReader::parseDataLine(QByteArray& line, QVector<size_t> dims, int64_t xCol, int64_t yCol, int64_t zCol)
{
  // Filter the line to convert European command style decimals to US/UK style points
  for (qint32 c = 0; c < line.size(); ++c)
  {
    if (line.at(c) == ',')
    {
      line[c] = '.';
    }
  }

  int64_t xIdx = 0, yIdx = 0, zIdx = 0;
  bool ok = false;
  QList<QByteArray> tokens = line.split(' ');

  int32_t oneBase = 0;
  if (getOneBasedArrays()) { oneBase = 1; }

  xIdx = tokens[xCol].toInt(&ok) - oneBase;
  yIdx = tokens[yCol].toInt(&ok) - oneBase;
  zIdx = tokens[zCol].toInt(&ok) - oneBase;

  // Calculate the offset into the actual array based on the x, y & z values from the data line we just read
  size_t offset = static_cast<size_t>((dims[1] * dims[0] * zIdx) + (dims[0] * yIdx) + xIdx);
  // BOOST_ASSERT(tokens.size() == m_NamePointerMap.size());

  QMapIterator<QString, DataParser::Pointer> iter(m_NamePointerMap);
  while (iter.hasNext())
  {
    iter.next();
    DataParser::Pointer dparser = iter.value();
    // Make sure we dont' parse the x, y, z or id columns since they are pretty much useless data. At some point
    // if the SPParks users actually wanted to read in the matching XYZ lattice site for the data then actually
    // parsing and storing the data _may_ be of interest to them
    if(dparser->getColumnName().compare("x") == 0
        || dparser->getColumnName().compare("y") == 0
        || dparser->getColumnName().compare("z") == 0
        || dparser->getColumnName().compare("id") == 0)
    {
      continue;
    }
    dparser->parse(tokens[dparser->getColumnIndex()], offset);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Ebsd::NumType SPParksTextReader::getPointerType(const QString& featureName)
{
  // text fields = id or site or x or y or z or
  //                  energy or propensity or iN or dN

  if (featureName.compare("id") == 0) { return Ebsd::Int32;}
  if (featureName.compare("type") == 0) { return Ebsd::Int32;}
  if (featureName.compare("x") == 0) { return Ebsd::Int32;}
  if (featureName.compare("y") == 0) { return Ebsd::Int32;}
  if (featureName.compare("z") == 0) { return Ebsd::Int32;}
  if (featureName.compare("energy") == 0) { return Ebsd::Float;}
  if (featureName.compare("propensity") == 0) { return Ebsd::Float;}
  if (featureName.startsWith("i")) { return Ebsd::Int32; } // Generic Integer site Value
  if (featureName.startsWith("d")) { return Ebsd::Float; } // Generic floating point site value

  // std::cout << "THIS IS NOT GOOD. Featurename: " << featureName << " was not found in the list" << std::endl;
  return Ebsd::UnknownNumType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t SPParksTextReader::getTypeSize(const QString& featureName)
{
  if (featureName.compare("id") == 0) { return 4;}
  if (featureName.compare("type") == 0) { return 4;}
  if (featureName.compare("x") == 0) { return 4;}
  if (featureName.compare("y") == 0) { return 4;}
  if (featureName.compare("z") == 0) { return 4;}
  if (featureName.compare("energy") == 0) { return 4;}
  if (featureName.compare("propensity") == 0) { return 4;}
  if (featureName.startsWith("i")) { return 4; }
  if (featureName.startsWith("d")) { return 4; }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer SPParksTextReader::newFilterInstance(bool copyFilterParameters)
{
  SPParksTextReader::Pointer filter = SPParksTextReader::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SPParksTextReader::getCompiledLibraryName()
{ return IOConstants::IOBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SPParksTextReader::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SPParksTextReader::getSubGroupName()
{ return DREAM3D::FilterSubGroups::InputFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SPParksTextReader::getHumanLabel()
{ return "Read SPParks Text File"; }
