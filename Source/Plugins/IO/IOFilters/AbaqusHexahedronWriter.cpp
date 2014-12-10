/* ============================================================================
 * Copyright (c) 2014 Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson, BlueQuartz Software nor the names of
 * its contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "AbaqusHexahedronWriter.h"

#include <QtCore/QDir>
#include <QtCore/QString>

#include "IO/IOConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbaqusHexahedronWriter::AbaqusHexahedronWriter() :
AbstractFilter(),
m_FilePrefix("default"),
m_OutputPath(""),
m_HourglassStiffness(0),
m_JobName(""),
m_FeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
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
  parameters.push_back(FileSystemFilterParameter::New("Output Path", "OutputPath", FilterParameterWidgetType::OutputPathWidget, getOutputPath(), false));
  parameters.push_back(FilterParameter::New("Output Prefix", "FilePrefix", FilterParameterWidgetType::StringWidget, getFilePrefix(), false));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureIdsArrayPath(), false));
  parameters.push_back(FilterParameter::New("Hourglass Stiffness", "HourglassStiffness", FilterParameterWidgetType::IntWidget, getHourglassStiffness(), false, "", 0));
  parameters.push_back(FilterParameter::New("Job Name", "JobName", FilterParameterWidgetType::StringWidget, getJobName(), false));
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
    QString ss = QObject::tr("The output path must be set before executing this filter.");
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
  }

  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
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

  VolumeDataContainer* r = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureIdsArrayPath.getDataContainerName());
  int elementX = r->getXPoints();
  float step = r->getXRes();

  // Create file names
  QString nodesFile = m_OutputPath + QDir::separator() + m_FilePrefix + "_nodes.inp";
  QString elemsFile = m_OutputPath + QDir::separator() + m_FilePrefix + "_elems.inp";
  QString sectsFile = m_OutputPath + QDir::separator() + m_FilePrefix + "_sects.inp";
  QString elsetFile = m_OutputPath + QDir::separator() + m_FilePrefix + "_elset.inp";
  QString micronsFile = m_OutputPath + QDir::separator() +  m_FilePrefix + ".inp";

  err = writeNodes(nodesFile, elementX, step);
  if (err < 0)
  {
    QString ss = QObject::tr("Error writing output nodes file '%1'\n ").arg(nodesFile);
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  err = writeElems(elemsFile, elementX);
  if (err < 0)
  {
    QString ss = QObject::tr("Error writing output elems file '%1'\n ").arg(elemsFile);
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  err = writeSects(sectsFile, elementX);
  if (err < 0)
  {
    QString ss = QObject::tr("Error writing output sects file '%1'\n ").arg(sectsFile);
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  err = writeElset(elsetFile, elementX);
  if (err < 0)
  {
    QString ss = QObject::tr("Error writing output elset file '%1'\n ").arg(elsetFile);
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  err = writeMicrons(micronsFile);
  if (err < 0)
  {
    QString ss = QObject::tr("Error writing output microns file '%1'\n ").arg(micronsFile);
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbaqusHexahedronWriter::writeNodes(const QString& file, int elementX, float step)
{
  float min = 0.0f - step * 0.5f;
  float x_factor, x_coord, y_factor, y_coord, z_factor, z_coord;
  int index = 0;
  float distance = (((index + 1)*step + min) - (index*step + min)) / 1000.0f; // Length in millimeters(mm)
  float sidelength = distance * elementX; // sidelength of an individual element(in mm's)
  int nodeX = elementX + 1;
  int nodeTotal = nodeX * nodeX * nodeX;

  int err = 0;
  FILE* f = NULL;
  f = fopen(file.toLatin1().data(), "wb");
  if (NULL == f)
  {
    return -1;
  }

  fprintf(f, "** Generated by : %s\n", DREAM3DLib::Version::PackageComplete().toLatin1().data());
  fprintf(f, "** ----------------------------------------------------------------\n**\n*Node\n");

  // Loop through the nodes and assign the x - , y - , and z - coordinates.
  // This code starts at(0, 0, 0) and increments up to(L, L, L) by iterating through x, then y, then z
  int node = 1;
  while (node <= nodeTotal) {
    x_factor = (node - 1) % nodeX;
    x_coord = (x_factor * distance);
    y_factor = ((node - 1) / nodeX) % nodeX;
    y_coord = (y_factor * distance);
    z_factor = (node - 1) / (nodeTotal / nodeX);
    z_coord = (z_factor * distance);
    fprintf(f, "%d, %4.2f, %4.2f, %4.2f\n", node, x_coord, y_coord, z_coord);
    node = node + 1;
  }

  // Write the last node, which is a dummy node used for stress - strain curves.
  fprintf(f, "%d, %4.2f, %4.2f, %4.2f\n", 999999, 0.0f, sidelength, 0.0f);
  fprintf(f, "**\n** ----------------------------------------------------------------\n**\n");

  if (err < 0)
  {
    QString ss = QObject::tr("Error writing nodes file '%1'\n ").arg(file);
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  // Close the file
  fclose(f);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbaqusHexahedronWriter::writeElems(const QString& file, int elementX)
{
  int n1, n2, n3, n4, n5, n6, n7, n8;
  int nodeX = elementX + 1;
  int elementTotal = elementX * elementX * elementX;

  int err = 0;
  FILE* f = NULL;
  f = fopen(file.toLatin1().data(), "wb");
  if (NULL == f)
  {
    return -1;
  }

  // Write the element file header
  fprintf(f, "** Generated by : %s\n", DREAM3DLib::Version::PackageComplete().toLatin1().data());
  fprintf(f, "** ----------------------------------------------------------------\n**\n*Element, type=C3D8\n");

  // Loop through the elements and assign the 8 nodes for each element.
  // The nodes in the elements were reordered below to account for the new numbering system of nodes from(0, 0, 0) to(L, L, L)
  int element = 1;
  while (element <= elementTotal) {
    n4 = element + (nodeX * nodeX) + 0           + ((element - 1) / elementX) + (nodeX * ((element - 1) / (elementX * elementX)));
    n1 = element + (nodeX * nodeX) + 1           + ((element - 1) / elementX) + (nodeX * ((element - 1) / (elementX * elementX)));
    n5 = element + (nodeX * nodeX) + (nodeX + 1) + ((element - 1) / elementX) + (nodeX * ((element - 1) / (elementX * elementX)));
    n8 = element + (nodeX * nodeX) + nodeX       + ((element - 1) / elementX) + (nodeX * ((element - 1) / (elementX * elementX)));
    n3 = element +                   0           + ((element - 1) / elementX) + (nodeX * ((element - 1) / (elementX * elementX)));
    n2 = element +                   1           + ((element - 1) / elementX) + (nodeX * ((element - 1) / (elementX * elementX)));
    n6 = element +                   (nodeX + 1) + ((element - 1) / elementX) + (nodeX * ((element - 1) / (elementX * elementX)));
    n7 = element +                   nodeX       + ((element - 1) / elementX) + (nodeX * ((element - 1) / (elementX * elementX)));
    fprintf(f, "%d, %d, %d, %d, %d, %d, %d, %d, %d\n", element, n1, n2, n3, n4, n5, n6, n7, n8);
    element++;
  }
  fprintf(f, "**\n** ----------------------------------------------------------------\n**\n");

  if (err < 0)
  {
    QString ss = QObject::tr("Error writing elems file '%1'\n ").arg(file);
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  // Close the file
  fclose(f);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbaqusHexahedronWriter::writeElset(const QString& file, int elementX)
{
	size_t totalPoints = elementX * elementX * elementX;

  int err = 0;
  FILE* f = NULL;
  f = fopen(file.toLatin1().data(), "wb");
  if (NULL == f)
  {
    return -1;
  }

  fprintf(f, "** Generated by : %s\n", DREAM3DLib::Version::PackageComplete().toLatin1().data());
  fprintf(f, "** ----------------------------------------------------------------\n**\n** The element sets\n");
  fprintf(f, "*Elset, elset=cube, generate\n");
  fprintf(f, "%d, %d, %d\n", 1, totalPoints, 1);
  fprintf(f, "**\n** Each Grain is made up of multiple elements\n**");

  // find total number of Grain Ids
	size_t maxGrainId = 0;
  for (int i = 0; i < totalPoints; i++) {
    if (m_FeatureIds[i] > maxGrainId)
    {
      maxGrainId = m_FeatureIds[i];
    }
  }

	size_t voxelId = 1;
  while (voxelId <= maxGrainId) {
    int elementPerLine = 0;
    fprintf(f, "\n*Elset, elset=Grain%d_set\n", voxelId);
		for (size_t i = 0; i < totalPoints + 1; i++)
    {
      if (m_FeatureIds[i] == voxelId)
      {
        if (elementPerLine != 0 && (elementPerLine % 16)){ // no comma at beginning and after last element
            fprintf(f, ", ");
        }
        fprintf(f, "%d", i+1);
        elementPerLine++;
        if ((elementPerLine % 16) == 0) // 16 elements per line
        {
          fprintf(f, ",\n");
        }
      }
    }
    voxelId++;
  }
  fprintf(f, "\n**\n** ----------------------------------------------------------------\n**\n");

  if (err < 0)
  {
    QString ss = QObject::tr("Error writing element set file '%1'\n ").arg(file);
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  // Close the file
  fclose(f);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbaqusHexahedronWriter::writeMicrons(const QString& file)
{
  int err = 0;
  FILE* f = NULL;
  f = fopen(file.toLatin1().data(), "wb");
  if (NULL == f)
  {
    return -1;
  }

  fprintf(f, "*Heading\n");
//  fprintf(f, "Cyclic Loading of Polycrystalline Ti - 64 (1 element grains) (Power Flow Rule)\n");
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

  if (err < 0)
  {
    QString ss = QObject::tr("Error writing microns file '%1'\n ").arg(file);
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  // Close the file
  fclose(f);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbaqusHexahedronWriter::writeSects(const QString& file, int elementX)
{
  int totalPoints = elementX * elementX * elementX;

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
  int maxGrainId = 0;
  for (int i = 0; i < totalPoints; i++) {
    if (m_FeatureIds[i] > maxGrainId)
    {
      maxGrainId = m_FeatureIds[i];
    }
  }

  // We are now defining the sections, which is for each grain
  int grain = 1;
  while (grain <= maxGrainId) {
    fprintf(f, "** Section: Grain%d\n", grain);
    fprintf(f, "*Solid Section, elset=Grain%d_set, material=Grain_Mat%d\n", grain, grain);
    fprintf(f, "*Hourglass Stiffness\n%d\n", m_HourglassStiffness);
    fprintf(f, "** --------------------------------------\n");
    grain++;
  }
  fprintf(f, "**\n** ----------------------------------------------------------------\n**\n");
  if (err < 0)
  {
    QString ss = QObject::tr("Error writing sects file '%1'\n ").arg(file);
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  // Close the file
  fclose(f);
  return err;
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
    return IO::IOBaseName;
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
