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

#include "AbaqusHexahedronWriter.h"

#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QDateTime>

#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/FilterParameters/FileSystemFilterParameter.h"
#include "DREAM3DLib/Utilities/TimeUtilities.h"

#include "IO/IOConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbaqusHexahedronWriter::AbaqusHexahedronWriter() :
  AbstractFilter(),
  m_OutputPath(""),
  m_FilePrefix("default"),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_HourglassStiffness(250),
  m_JobName(""),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbaqusHexahedronWriter::~AbaqusHexahedronWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbaqusHexahedronWriter::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FileSystemFilterParameter::New("Output Path", "OutputPath", FilterParameterWidgetType::OutputPathWidget, getOutputPath(), FilterParameter::Uncategorized));
  parameters.push_back(FilterParameter::New("Output File Prefix", "FilePrefix", FilterParameterWidgetType::StringWidget, getFilePrefix(), FilterParameter::Uncategorized));
  parameters.push_back(FilterParameter::New("Feature Ids", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureIdsArrayPath(), FilterParameter::Uncategorized));
  parameters.push_back(FilterParameter::New("Hourglass Stiffness", "HourglassStiffness", FilterParameterWidgetType::IntWidget, getHourglassStiffness(), FilterParameter::Uncategorized, "", 0));
  parameters.push_back(FilterParameter::New("Job Name", "JobName", FilterParameterWidgetType::StringWidget, getJobName(), FilterParameter::Uncategorized));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbaqusHexahedronWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setFilePrefix(reader->readString("FilePrefix", getFilePrefix()));
  setOutputPath(reader->readString("OutputPath", getOutputPath()));
  setHourglassStiffness(reader->readValue("HourglassStiffness", getHourglassStiffness()));
  setJobName(reader->readString("JobName", getJobName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbaqusHexahedronWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FilePrefix)
  DREAM3D_FILTER_WRITE_PARAMETER(HourglassStiffness)
  DREAM3D_FILTER_WRITE_PARAMETER(JobName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbaqusHexahedronWriter::dataCheck()
{
  setErrorCondition(0);
  if (m_OutputPath.isEmpty() == true)
  {
    setErrorCondition(-12001);
    QString ss = QObject::tr("The output path must be set before executing this filter.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QVector<size_t> cDims(1, 1); // The component dimensions of the needed array.
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  DataContainer::Pointer dc = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName(), false);
  if (getErrorCondition() < 0) { return; }

  ImageGeom::Pointer image = dc->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if (getErrorCondition() < 0) { return; }

  if(NULL != m_FeatureIdsPtr.lock().get() && NULL != dc )
  {
    size_t volDims[3] = { 0, 0, 0};
    image->getDimensions(volDims);
    size_t volTuples = volDims[0] * volDims[1] * volDims[2];

    if (volTuples != m_FeatureIdsPtr.lock()->getNumberOfTuples() )
    {
      setErrorCondition(-12000);
      QString ss = QObject::tr("The number of tuples for the selected volume '%1' does not match the number of tuples for the selected data array '%2'.\
                               Did you select an Attribute Matrix that holds data for each voxel? These AttributeMatrices are usually called 'CellData'.").arg(volTuples).arg(m_FeatureIdsPtr.lock()->getNumberOfTuples());
                                                                                                  notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbaqusHexahedronWriter::preflight()
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
void AbaqusHexahedronWriter::execute()
{
  int err;
  setErrorCondition(0);
  dataCheck();
  if (getErrorCondition() < 0) { return; }

  // Check Output Path
  QDir dir;
  if (!dir.mkpath(m_OutputPath))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(m_OutputPath);
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
    return;
  }

  DataContainer::Pointer r = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  size_t cDims[3] = { 0, 0, 0 };
  r->getGeometryAs<ImageGeom>()->getDimensions(cDims);
  size_t pDims[3] = { cDims[0] + 1, cDims[1] + 1, cDims[2] + 1 };
  float origin[3] = { 0.0f, 0.0f, 0.0f };
  r->getGeometryAs<ImageGeom>()->getOrigin(origin);
  float spacing[3] = { 0.0f, 0.0f, 0.0f };
  r->getGeometryAs<ImageGeom>()->getResolution(spacing);
  size_t totalPoints = r->getGeometryAs<ImageGeom>()->getXPoints() * r->getGeometryAs<ImageGeom>()->getYPoints() * r->getGeometryAs<ImageGeom>()->getZPoints();

  // Create file names
  QString nodesFile = m_OutputPath + QDir::separator() + m_FilePrefix + "_nodes.inp";
  QString elemsFile = m_OutputPath + QDir::separator() + m_FilePrefix + "_elems.inp";
  QString sectsFile = m_OutputPath + QDir::separator() + m_FilePrefix + "_sects.inp";
  QString elsetFile = m_OutputPath + QDir::separator() + m_FilePrefix + "_elset.inp";
  QString masterFile = m_OutputPath + QDir::separator() + m_FilePrefix + ".inp";
  QList<QString> fileNames;
  fileNames << nodesFile << elemsFile << sectsFile << elsetFile << masterFile;

  err = writeNodes(fileNames, cDims, origin, spacing); // Nodes file
  if (err < 0)
  {
    QString ss = QObject::tr("Error writing output nodes file '%1'\n ").arg(nodesFile);
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  else if (getCancel() == true)   // Filter has been cancelled
  {
    deleteFile(fileNames); // delete files
    return;
  }

  err = writeElems(fileNames, cDims, pDims); // Elements file
  if (err < 0)
  {
    QString ss = QObject::tr("Error writing output elems file '%1'\n ").arg(elemsFile);
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  else if (getCancel() == true)   // Filter has been cancelled
  {
    deleteFile(fileNames); // delete files
    return;
  }

  err = writeSects(sectsFile, totalPoints); // Sections file
  if (err < 0)
  {
    QString ss = QObject::tr("Error writing output sects file '%1'\n ").arg(sectsFile);
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  else if (getCancel() == true)   // Filter has been cancelled
  {
    deleteFile(fileNames); // delete files
    return;
  }

  err = writeElset(fileNames, totalPoints); // Element set file
  if (err < 0)
  {
    QString ss = QObject::tr("Error writing output elset file '%1'\n ").arg(elsetFile);
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  else if (getCancel() == true)   // Filter has been cancelled
  {
    deleteFile(fileNames); // delete files
    return;
  }

  err = writeMaster(masterFile); // Master file
  if (err < 0)
  {
    QString ss = QObject::tr("Error writing output master file '%1'\n ").arg(masterFile);
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  else if (getCancel() == true)   // Filter has been cancelled
  {
    deleteFile(fileNames); // delete files
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

/**
* @param cDims
* @param origin
* @param spacing
*/
int AbaqusHexahedronWriter::writeNodes(const QList<QString>& fileNames, size_t* cDims, float* origin, float* spacing)
{
  uint64_t millis = QDateTime::currentMSecsSinceEpoch();
  uint64_t currentMillis = millis;
  uint64_t startMillis = millis;
  uint64_t estimatedTime = 0;
  float timeDiff = 0.0f;
  QString buf;
  QTextStream ss(&buf);

  size_t pDims[3] = { cDims[0] + 1, cDims[1] + 1, cDims[2] + 1 };
  size_t nodeIndex = 1;
  size_t totalPoints = pDims[0] * pDims[1] * pDims[2];
  int increment = totalPoints * .01;
  if (increment == 0)  // check to prevent divide by 0
  {
    increment = 1;
  }

  int err = 0;
  FILE* f = NULL;
  f = fopen(fileNames.at(0).toLatin1().data(), "wb");
  if (NULL == f)
  {
    return -1;
  }

  fprintf(f, "** Generated by : %s\n", DREAM3DLib::Version::PackageComplete().toLatin1().data());
  fprintf(f, "** ----------------------------------------------------------------\n**\n*Node\n");

  for (size_t z = 0; z < pDims[2]; z++)
  {
    for (size_t y = 0; y < pDims[1]; y++)
    {
      for (size_t x = 0; x < pDims[0]; x++)
      {
        float xCoord = origin[0] + (x * spacing[0]);
        float yCoord = origin[1] + (y * spacing[1]);
        float zCoord = origin[2] + (z * spacing[2]);
        fprintf(f, "%lu, %f, %f, %f\n", nodeIndex, xCoord, yCoord, zCoord);
        if (nodeIndex % increment == 0)
        {
          currentMillis = QDateTime::currentMSecsSinceEpoch();
          if (currentMillis - millis > 1000)
          {
            buf.clear();
            ss << getMessagePrefix() << " Writing Nodes (File 1/5) " << static_cast<int>((float)(nodeIndex) / (float)(totalPoints) * 100) << "% Completed ";
            timeDiff = ((float)nodeIndex / (float)(currentMillis - startMillis));
            estimatedTime = (float)(totalPoints - nodeIndex) / timeDiff;
            ss << " || Est. Time Remain: " << DREAM3D::convertMillisToHrsMinSecs(estimatedTime);
            notifyStatusMessage(getHumanLabel(), buf);
            millis = QDateTime::currentMSecsSinceEpoch();
            if (getCancel() == true)   // Filter has been cancelled
            {
              fclose(f);
              return 1;
            }
          }
        }
        ++nodeIndex;
      }
    }
  }

  // Write the last node, which is a dummy node used for stress - strain curves.
  fprintf(f, "%d, %f, %f, %f\n", 999999, 0.0f, 0.0f, 0.0f);
  fprintf(f, "**\n** ----------------------------------------------------------------\n**\n");

  // Close the file
  notifyStatusMessage(getHumanLabel(), "Writing Nodes (File 1/5) Complete");
  fclose(f);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbaqusHexahedronWriter::writeElems(const QList<QString>& fileNames, size_t* cDims, size_t* pDims)
{
  uint64_t millis = QDateTime::currentMSecsSinceEpoch();
  uint64_t currentMillis = millis;
  uint64_t startMillis = millis;
  uint64_t estimatedTime = 0;
  float timeDiff = 0.0f;
  QString buf;
  QTextStream ss(&buf);
  size_t totalPoints = cDims[0] * cDims[1] * cDims[2];
  int increment = totalPoints * .01;
  if (increment == 0)  // check to prevent divide by 0
  {
    increment = 1;
  }

  int err = 0;
  FILE* f = NULL;
  f = fopen(fileNames.at(1).toLatin1().data(), "wb");
  if (NULL == f)
  {
    return -1;
  }

  size_t index = 1;
  fprintf(f, "** Generated by : %s\n", DREAM3DLib::Version::PackageComplete().toLatin1().data());
  fprintf(f, "** ----------------------------------------------------------------\n**\n*Element, type=C3D8\n");
  for (size_t z = 0; z < cDims[2]; z++)
  {
    for (size_t y = 0; y < cDims[1]; y++)
    {
      for (size_t x = 0; x < cDims[0]; x++)
      {
        std::vector<int> nodeId = getNodeIds(index, x, y, z, pDims);
        fprintf(f, "%lu, %d, %d, %d, %d, %d, %d, %d, %d\n", index, nodeId[5], nodeId[1], nodeId[0], nodeId[4], nodeId[7], nodeId[3], nodeId[2], nodeId[6]);
        if (index % increment == 0)
        {
          currentMillis = QDateTime::currentMSecsSinceEpoch();
          if (currentMillis - millis > 1000)
          {
            buf.clear();
            ss << getMessagePrefix() << " Writing Elements (File 2/5) " << static_cast<int>((float)(index) / (float)(totalPoints) * 100) << "% Completed ";
            timeDiff = ((float)index / (float)(currentMillis - startMillis));
            estimatedTime = (float)(totalPoints - index) / timeDiff;
            ss << " || Est. Time Remain: " << DREAM3D::convertMillisToHrsMinSecs(estimatedTime);
            notifyStatusMessage(getHumanLabel(), buf);
            millis = QDateTime::currentMSecsSinceEpoch();
            if (getCancel() == true)   // Filter has been cancelled
            {
              fclose(f);
              return 1;
            }
          }
        }
        ++index;
      }
    }
  }

  fprintf(f, "**\n** ----------------------------------------------------------------\n**\n");

  // Close the file
  notifyStatusMessage(getHumanLabel(), "Writing Elements (File 2/5) Complete");
  fclose(f);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbaqusHexahedronWriter::writeElset(const QList<QString>& fileNames, size_t totalPoints)
{
  uint64_t millis = QDateTime::currentMSecsSinceEpoch();
  uint64_t currentMillis = millis;
  uint64_t startMillis = millis;
  uint64_t estimatedTime = 0;
  float timeDiff = 0.0f;
  QString buf;
  QTextStream ss(&buf);

  int err = 0;
  FILE* f = NULL;
  f = fopen(fileNames.at(3).toLatin1().data(), "wb");
  if (NULL == f)
  {
    return -1;
  }

  fprintf(f, "** Generated by : %s\n", DREAM3DLib::Version::PackageComplete().toLatin1().data());
  fprintf(f, "** ----------------------------------------------------------------\n**\n** The element sets\n");
  fprintf(f, "*Elset, elset=cube, generate\n");
  fprintf(f, "1, %lu, 1\n", totalPoints);
  fprintf(f, "**\n** Each Grain is made up of multiple elements\n**");
  notifyStatusMessage(getHumanLabel(), (getMessagePrefix() + " Writing Element Sets (File 4/5) 1% Completed || Est. Time Remain: "));

  // find total number of Grain Ids
  size_t maxGrainId = 0;
  for (size_t i = 0; i < totalPoints; i++)   // find number of grainIds
  {
    if (m_FeatureIds[i] > maxGrainId)
    {
      maxGrainId = m_FeatureIds[i];
    }
  }

  int increment = maxGrainId * .1;
  if (increment == 0)  // check to prevent divide by 0
  {
    increment = 1;
  }

  size_t voxelId = 1;
  while (voxelId <= maxGrainId)
  {
    size_t elementPerLine = 0;
    fprintf(f, "\n*Elset, elset=Grain%lu_set\n", voxelId);

    for (size_t i = 0; i < totalPoints + 1; i++)
    {
      if (m_FeatureIds[i] == voxelId)
      {
        if (elementPerLine != 0) // no comma at start
        {
          if (elementPerLine % 16) // 16 per line
          {
            fprintf(f, ", ");
          }
          else
          {
            fprintf(f, ",\n");
          }
        }
        fprintf(f, "%lu", i + 1);
        elementPerLine++;
      }
    }
    if (voxelId % increment == 0)
    {
      currentMillis = QDateTime::currentMSecsSinceEpoch();
      if (currentMillis - millis > 1000)
      {
        buf.clear();
        ss << getMessagePrefix() << " Writing Element Sets (File 4/5) " << static_cast<int>((float)(voxelId) / (float)(maxGrainId) * 100) << "% Completed ";
        timeDiff = ((float)voxelId / (float)(currentMillis - startMillis));
        estimatedTime = (float)(maxGrainId - voxelId) / timeDiff;
        ss <<  " || Est. Time Remain: " << DREAM3D::convertMillisToHrsMinSecs(estimatedTime);
        notifyStatusMessage(getHumanLabel(), buf);
        millis = QDateTime::currentMSecsSinceEpoch();
        if (getCancel() == true)   // Filter has been cancelled
        {
          fclose(f);
          return 1;
        }
      }
    }
    voxelId++;
  }
  fprintf(f, "\n**\n** ----------------------------------------------------------------\n**\n");

  // Close the file
  notifyStatusMessage(getHumanLabel(), "Writing Element Sets (File 4/5) Complete");
  fclose(f);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbaqusHexahedronWriter::writeMaster(const QString& file)
{
  int err = 0;
  FILE* f = NULL;
  f = fopen(file.toLatin1().data(), "wb");
  if (NULL == f)
  {
    return -1;
  }

  fprintf(f, "*Heading\n");
  fprintf(f, "%s\n", m_JobName.toLatin1().data());
  fprintf(f, "** Job name : %s\n", m_JobName.toLatin1().data());
  fprintf(f, "** Generated by : %s\n", DREAM3DLib::Version::PackageComplete().toLatin1().data());
  fprintf(f, "*Preprint, echo = NO, model = NO, history = NO, contact = NO\n");
  fprintf(f, "**\n** ----------------------------Geometry----------------------------\n**\n");
  fprintf(f, "*Include, Input = %s\n", (m_FilePrefix + "_nodes.inp").toLatin1().data());
  fprintf(f, "*Include, Input = %s\n", (m_FilePrefix + "_elems.inp").toLatin1().data());
  fprintf(f, "*Include, Input = %s\n", (m_FilePrefix + "_elset.inp").toLatin1().data());
  fprintf(f, "*Include, Input = %s\n", (m_FilePrefix + "_sects.inp").toLatin1().data());
  fprintf(f, "**\n** ----------------------------------------------------------------\n**\n");

  // Close the file
  notifyStatusMessage(getHumanLabel(), "Writing Master (File 5/5) Complete");
  fclose(f);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbaqusHexahedronWriter::writeSects(const QString& file, size_t totalPoints)
{

  int err = 0;
  FILE* f = NULL;
  f = fopen(file.toLatin1().data(), "wb");
  if (NULL == f)
  {
    return -1;
  }
  fprintf(f, "** Generated by : %s\n", DREAM3DLib::Version::PackageComplete().toLatin1().data());
  fprintf(f, "** ----------------------------------------------------------------\n**\n** Each section is a separate grain\n");

  // find total number of Grain Ids
  size_t maxGrainId = 0;
  for (size_t i = 0; i < totalPoints; i++)
  {
    if (m_FeatureIds[i] > maxGrainId)
    {
      maxGrainId = m_FeatureIds[i];
    }
  }

  // We are now defining the sections, which is for each grain
  size_t grain = 1;
  while (grain <= maxGrainId)
  {
    fprintf(f, "** Section: Grain%lu\n", grain);
    fprintf(f, "*Solid Section, elset=Grain%lu_set, material=Grain_Mat%lu\n", grain, grain);
    fprintf(f, "*Hourglass Stiffness\n%d\n", m_HourglassStiffness);
    fprintf(f, "** --------------------------------------\n");
    grain++;
  }
  fprintf(f, "**\n** ----------------------------------------------------------------\n**\n");

  // Close the file
  notifyStatusMessage(getHumanLabel(), "Writing Sections (File 3/5) Complete");
  fclose(f);
  return err;
}

/**
* @param index Element index
* @param x Voxel
* @param y Voxel
* @param z Voxel
* @param pDims The point Dims, or the Voxel Dims + 1 in each direction
*/
std::vector<int> AbaqusHexahedronWriter::getNodeIds(size_t index, size_t x, size_t y, size_t z, size_t* pDims)
{
  std::vector<int> nodeId(8, 0);

  nodeId[0] = 1 + (pDims[0] * pDims[1] * z) + (pDims[0] * y) + x;
  nodeId[1] = 1 + (pDims[0] * pDims[1] * z) + (pDims[0] * y) + (x + 1);
  nodeId[2] = 1 + (pDims[0] * pDims[1] * z) + (pDims[0] * (y + 1)) + x;
  nodeId[3] = 1 + (pDims[0] * pDims[1] * z) + (pDims[0] * (y + 1)) + (x + 1);

  nodeId[4] = 1 + (pDims[0] * pDims[1] * (z + 1)) + (pDims[0] * y) + x;
  nodeId[5] = 1 + (pDims[0] * pDims[1] * (z + 1)) + (pDims[0] * y) + (x + 1);
  nodeId[6] = 1 + (pDims[0] * pDims[1] * (z + 1)) + (pDims[0] * (y + 1)) + x;
  nodeId[7] = 1 + (pDims[0] * pDims[1] * (z + 1)) + (pDims[0] * (y + 1)) + (x + 1);

  if (0)
  {
    printf("           %03d-------%03d  \n", nodeId[4], nodeId[5]);
    printf("            /|        /|   \n");
    printf("           / |       / |   \n");
    printf("          /  |      /  |   \n");
    printf("       %03d--------%03d  |   \n", nodeId[6], nodeId[7]);
    printf("         |   |      |  |   \n");
    printf("         | %03d------|-%03d  \n", nodeId[0], nodeId[1]);
    printf("         |  /       | /    \n");
    printf("         | /        |/     \n");
    printf("        %03d--------%03d     \n", nodeId[2], nodeId[3]);
  }
  return nodeId;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbaqusHexahedronWriter::deleteFile(const QList<QString>& fileNames)
{
  for (int i = 0; i < fileNames.size(); i++)
  {
    QFileInfo fi(fileNames.at(i));
    if (fi.exists())
    {
      QFile::remove(fileNames.at(i));
    }
  }
  notifyStatusMessage(getHumanLabel(), "Filter was canceled");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer AbaqusHexahedronWriter::newFilterInstance(bool copyFilterParameters)
{
  /*
  * write code to optionally copy the filter parameters from the current filter into the new instance
  */
  AbaqusHexahedronWriter::Pointer filter = AbaqusHexahedronWriter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbaqusHexahedronWriter::getCompiledLibraryName()
{
  return IOConstants::IOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbaqusHexahedronWriter::getGroupName()
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbaqusHexahedronWriter::getHumanLabel()
{
  return "Abaqus Hexahedron Writer";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString AbaqusHexahedronWriter::getSubGroupName()
{
  return "Output";
}



