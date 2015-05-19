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


#include "ImportVectorImageStack.h"

#include <string.h>

#include <limits>

#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtGui/QImage>
#include <QtGui/QImageReader>

#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/Utilities/FilePathGenerator.h"

#include "ImageImport/ImageImportConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportVectorImageStack::ImportVectorImageStack() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::DataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_StartIndex(0),
  m_EndIndex(0),
  m_InputPath(""),
  m_FilePrefix(""),
  m_Separator(""),
  m_FileSuffix(""),
  m_FileExtension(""),
  m_PaddingDigits(0),
  m_RefFrameZDir(Ebsd::RefFrameZDir::LowtoHigh),
  m_VectorDataArrayName(DREAM3D::CellData::VectorData),
  m_VectorData(NULL)
{

  m_Origin.x = 0.0;
  m_Origin.y = 0.0;
  m_Origin.z = 0.0;

  m_Resolution.x = 1.0;
  m_Resolution.y = 1.0;
  m_Resolution.z = 1.0;


  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportVectorImageStack::~ImportVectorImageStack()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStack::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;

  parameters.push_back(FilterParameter::New("Import Image Data", "ImageVector", FilterParameterWidgetType::ImportVectorImageStackWidget, getImageVector(), false));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Data Container Name", "DataContainerName", FilterParameterWidgetType::StringWidget, getDataContainerName(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Attribute Matrix Name", "CellAttributeMatrixName", FilterParameterWidgetType::StringWidget, getCellAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("VectorData", "VectorDataArrayName", FilterParameterWidgetType::StringWidget, getVectorDataArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStack::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName() ) );
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName() ) );
  setVectorDataArrayName( reader->readString("VectorDataArrayName", getVectorDataArrayName()) );
  setStartIndex( reader->readValue("StartIndex", getStartIndex()) );
  setEndIndex( reader->readValue("EndIndex", getEndIndex()) );
  setStartComp( reader->readValue("StartComp", getStartComp()) );
  setEndComp( reader->readValue("EndComp", getEndComp()) );
  setPaddingDigits( reader->readValue("PaddingDigits", getPaddingDigits()) );
  setRefFrameZDir( reader->readValue("RefFrameZDir", getRefFrameZDir()) );
  setInputPath( reader->readString("InputPath", getInputPath()) );
  setFilePrefix( reader->readString("FilePrefix", getFilePrefix()) );
  setSeparator( reader->readString("Separator", getSeparator()) );
  setFileSuffix( reader->readString("FileSuffix", getFileSuffix()) );
  setFileExtension( reader->readString("FileExtension", getFileExtension()) );
  setOrigin( reader->readFloatVec3("Origin", getOrigin()) );
  setResolution( reader->readFloatVec3("Resolution", getResolution()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImportVectorImageStack::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(DataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(VectorDataArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(StartIndex)
  DREAM3D_FILTER_WRITE_PARAMETER(EndIndex)
  DREAM3D_FILTER_WRITE_PARAMETER(StartComp)
  DREAM3D_FILTER_WRITE_PARAMETER(EndComp)
  DREAM3D_FILTER_WRITE_PARAMETER(PaddingDigits)
  DREAM3D_FILTER_WRITE_PARAMETER(RefFrameZDir)
  DREAM3D_FILTER_WRITE_PARAMETER(InputPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FilePrefix)
  DREAM3D_FILTER_WRITE_PARAMETER(Separator)
  DREAM3D_FILTER_WRITE_PARAMETER(FileSuffix)
  DREAM3D_FILTER_WRITE_PARAMETER(FileExtension)
  DREAM3D_FILTER_WRITE_PARAMETER(Origin)
  DREAM3D_FILTER_WRITE_PARAMETER(Resolution)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStack::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);
  QString ss;

  if(m_InputPath.isEmpty() == true)
  {
    ss = QObject::tr("The Input Directory must be set before executing this filter.");
    notifyErrorMessage(getHumanLabel(), ss, -13);
    setErrorCondition(-13);
  }

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0) { return; }

  bool hasMissingFiles = false;
  bool stackLowToHigh = false;

  if( Ebsd::RefFrameZDir::LowtoHigh == m_RefFrameZDir) { stackLowToHigh = true; }
  else if (Ebsd::RefFrameZDir::HightoLow == m_RefFrameZDir) { stackLowToHigh = false; }

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList = FilePathGenerator::GenerateVectorFileList(m_StartIndex, m_EndIndex, m_StartComp, m_EndComp,
                              hasMissingFiles, stackLowToHigh, m_InputPath,
                              m_FilePrefix, m_Separator, m_FileSuffix, m_FileExtension,
                              m_PaddingDigits);
  if (fileList.size() == 0)
  {
    QString ss = QObject::tr("No files have been selected for import. Have you set the input directory?");
    setErrorCondition(-11);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  else
  {
    // If we have RGB or RGBA Images then we are going to have to change things a bit.
    // We should read the file and see what we have? Of course Qt is going to read it up into
    // an RGB array by default
    int err = 0;
    QImage image(fileList[0]);
    int64_t dims[3] = {image.width(), image.height(), ((m_EndIndex - m_StartIndex) + 1)};
    int pixelBytes = 0;
    //int test = image.format();
    if (image.format() == QImage::Format_Indexed8)
    {
      pixelBytes = 1;
    }
    else if (image.format() == QImage::Format_RGB32 || image.format() == QImage::Format_ARGB32)
    {
      pixelBytes = 4;
    }
    /* Sanity check what we are trying to load to make sure it can fit in our address space.
      * Note that this does not guarantee the user has enough left, just that the
      * size of the volume can fit in the address space of the program
      */
#if   (CMP_SIZEOF_SSIZE_T==4)
    int64_t max = std::numeric_limits<size_t>::max();
#else
    int64_t max = std::numeric_limits<int64_t>::max();
#endif
    if(dims[0] * dims[1] * dims[2] > max)
    {
      err = -1;
      QString ss = QObject::tr("The total number of elements '%1' is greater than this program can hold. Try the 64 bit version.").arg((dims[0] * dims[1] * dims[2]));
      setErrorCondition(err);
      notifyErrorMessage(getHumanLabel(), ss, err);
    }

    if(dims[0] > max || dims[1] > max || dims[2] > max)
    {
      err = -1;
      QString ss = QObject::tr("One of the dimensions is greater than the max index for this sysem. Try the 64 bit version."
                               " dim[0]=%1  dim[1]=%2  dim[2]=%3").arg(dims[0]).arg(dims[1]).arg(dims[2]);
      setErrorCondition(err);
      notifyErrorMessage(getHumanLabel(), ss, err);
    }
    /* ************ End Sanity Check *************************** */

    m->getGeometryAs<ImageGeom>()->setDimensions(static_cast<size_t>(dims[0]), static_cast<size_t>(dims[1]), static_cast<size_t>(dims[2]));
    m->getGeometryAs<ImageGeom>()->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
    m->getGeometryAs<ImageGeom>()->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);

    QVector<size_t> tDims(3, 0);
    for(int i = 0; i < 3; i++)
    {
      tDims[i] = dims[i];
    }
    AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
    if(getErrorCondition() < 0) { return; }

    //set up component dimensions for the vector image array
    QVector<size_t> arraydims(2);
    arraydims[0] = ((m_EndComp - m_StartComp) + 1);
    arraydims[1] = pixelBytes;

    // This would be for a gray scale image
    tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), getVectorDataArrayName() );
    m_VectorDataPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, tempPath, 0, arraydims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_VectorDataPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_VectorData = m_VectorDataPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
// This is code for adding to an existing data container --- kept only for reference in case we want to implement this later
//  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, ImportVectorImageStack>(this, getDataContainerName());
//  if(getErrorCondition() < 0) { return; }

//  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), DREAM3D::AttributeMatrixType::Cell);
//  if(getErrorCondition() < 0) { return; }

//  QVector<size_t> tDims = cellAttrMat->getTupleDimensions();
//  if(tDims.size() != 3)
//  {
//    QString ss = QObject::tr("Existing DataContainer has different number of tuple dimensions than images being read");
//    setErrorCondition(-12);
//    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
//    return;
//  }
//  for(int i = 0; i < tDims.size(); i++)
//  {
//    if(tDims[i] != dims[i])
//    {
//      QString ss = QObject::tr("Existing DataContainer has a tuple dimension different than images being read");
//      setErrorCondition(-12);
//      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
//      return;
//    }
//  }

//  QVector<size_t> arraydims(1, fileList.size());
//  // This would be for a gray scale image
//  tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), getVectorDataArrayName() );
//  m_VectorDataPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, tempPath, 0, arraydims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
//  if( NULL != m_VectorDataPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
//  { m_VectorData = m_VectorDataPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportVectorImageStack::preflight()
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
void ImportVectorImageStack::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());

  QVector<size_t> tDims = cellAttrMat->getTupleDimensions();
  size_t imageWidth = tDims[0];
  size_t imageHeight = tDims[1];
  //size_t numVecImages = tDims[2];

  size_t numComps = m_VectorDataPtr.lock()->getNumberOfComponents();
  QVector<size_t> cDims = m_VectorDataPtr.lock()->getComponentDimensions();
  //size_t vecDim = cDims[0];
  size_t pixDepth = cDims[1];

  size_t numCompsPerVecImage = imageHeight * imageWidth * numComps;

  int64_t comp = m_StartComp;
  size_t compSpot;
  int64_t imageNum = m_StartIndex;
  size_t imageSpot;
  size_t imageCompShift;

  uint8_t pixVal = 0;

  bool hasMissingFiles = false;
  bool stackLowToHigh = false;

  if( Ebsd::RefFrameZDir::LowtoHigh == m_RefFrameZDir) { stackLowToHigh = true; }
  else if (Ebsd::RefFrameZDir::HightoLow == m_RefFrameZDir) { stackLowToHigh = false; }

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList = FilePathGenerator::GenerateVectorFileList(m_StartIndex, m_EndIndex,
                              m_StartComp, m_EndComp, hasMissingFiles, stackLowToHigh, m_InputPath,
                              m_FilePrefix, m_Separator, m_FileSuffix, m_FileExtension,
                              m_PaddingDigits);


  for (QVector<QString>::iterator filepath = fileList.begin(); filepath != fileList.end(); ++filepath)
  {
    QString imageFName = *filepath;
    QString ss = QObject::tr("Importing file %1").arg(imageFName);
    notifyStatusMessage(getHumanLabel(), ss);

    QImage image(imageFName);
    if (image.isNull() == true)
    {
      setErrorCondition(-14000);
      notifyErrorMessage(getHumanLabel(), "Failed to load Image file", getErrorCondition());
    }

    compSpot = (comp - m_StartComp);
    imageSpot = (imageNum - m_StartIndex);
    imageCompShift = numCompsPerVecImage * imageSpot;
    size_t compStride = (compSpot * pixDepth);
    size_t src = 0;
    size_t dst = 0;
    size_t hStride;
    for(size_t i = 0; i < imageHeight; ++i)
    {
      uint8_t* source = image.scanLine(i);
      src = 0;
      hStride = i * imageWidth;
      for(size_t j = 0; j < imageWidth; ++j)
      {
        dst = imageCompShift + (numComps * (hStride + j)) + compStride;
        src = j * pixDepth;
        for(size_t k = 0; k < pixDepth; ++k)
        {
          pixVal = source[src + k];
          m_VectorData[dst + k] = pixVal;
        }
      }
    }

    ++comp;
    if(comp > m_EndComp)
    {
      comp = m_StartComp;
      imageNum++;
    }
    if(getCancel() == true)
    {
      notifyStatusMessage(getHumanLabel(), "Conversion was Canceled");
      return;
    }
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ImportVectorImageStack::newFilterInstance(bool copyFilterParameters)
{
  ImportVectorImageStack::Pointer filter = ImportVectorImageStack::New();
  if(true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters() );
    // We are going to hand copy all of the parameters because the other way of copying the parameters are going to
    // miss some of them because we are not enumerating all of them.
    DREAM3D_COPY_INSTANCEVAR(DataContainerName)
    DREAM3D_COPY_INSTANCEVAR(CellAttributeMatrixName)
    DREAM3D_COPY_INSTANCEVAR(StartIndex)
    DREAM3D_COPY_INSTANCEVAR(EndIndex)
    DREAM3D_COPY_INSTANCEVAR(StartComp)
    DREAM3D_COPY_INSTANCEVAR(EndComp)
    DREAM3D_COPY_INSTANCEVAR(Resolution)
    DREAM3D_COPY_INSTANCEVAR(Origin)
    DREAM3D_COPY_INSTANCEVAR(InputPath)
    DREAM3D_COPY_INSTANCEVAR(FilePrefix)
    DREAM3D_COPY_INSTANCEVAR(Separator)
    DREAM3D_COPY_INSTANCEVAR(FileSuffix)
    DREAM3D_COPY_INSTANCEVAR(FileExtension)
    DREAM3D_COPY_INSTANCEVAR(PaddingDigits)
    DREAM3D_COPY_INSTANCEVAR(RefFrameZDir)
    DREAM3D_COPY_INSTANCEVAR(ImageVector)
    DREAM3D_COPY_INSTANCEVAR(VectorDataArrayName)
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportVectorImageStack::getCompiledLibraryName()
{ return ImageImportConstants::ImageImportBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportVectorImageStack::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportVectorImageStack::getSubGroupName()
{ return DREAM3D::FilterSubGroups::InputFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportVectorImageStack::getHumanLabel()
{ return "Import Images (3D Vector Stack)"; }

