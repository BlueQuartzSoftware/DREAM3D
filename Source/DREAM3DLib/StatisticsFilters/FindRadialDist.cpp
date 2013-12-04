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

#include "FindRadialDist.h"


#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QtDebug>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/GenericFilters/FindFeaturePhases.h"
#include "DREAM3DLib/GenericFilters/FindSurfaceFeatures.h"
#include "DREAM3DLib/GenericFilters/FindFeatureCentroids.h"
#include "DREAM3DLib/StatisticsFilters/FindSizes.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindRadialDist::FindRadialDist() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_OutputFile(""),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL),
  m_EquivalentDiametersArrayName(DREAM3D::FeatureData::EquivalentDiameters),
  m_EquivalentDiameters(NULL),
  m_VolumesArrayName(DREAM3D::FeatureData::Volumes),
  m_Volumes(NULL),
  m_CentroidsArrayName(DREAM3D::FeatureData::Centroids),
  m_Centroids(NULL),
  m_SurfaceFeaturesArrayName(DREAM3D::FeatureData::SurfaceFeatures),
  m_SurfaceFeatures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindRadialDist::~FindRadialDist()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindRadialDist::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("*.csv");
    option->setFileType("Comma Separated Values");
    option->setValueType("string");
    parameters.push_back(option);
  }
}
// -----------------------------------------------------------------------------
void FindRadialDist::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindRadialDist::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputFile", getOutputFile());
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindRadialDist::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, FindRadialDist>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }

  QVector<int> dims(1, 1);
  m_FeaturePhasesPtr = attrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeaturePhasesArrayName, -304, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_SurfaceFeaturesPtr = attrMat->getPrereqArray<DataArray<bool>, AbstractFilter>(this, m_SurfaceFeaturesArrayName, -302, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_VolumesPtr = attrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_VolumesArrayName, -302, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_VolumesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Volumes = m_VolumesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_EquivalentDiametersPtr = attrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_EquivalentDiametersArrayName, -302, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_EquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  m_CentroidsPtr = attrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_CentroidsArrayName, -305, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CentroidsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Centroids = m_CentroidsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if (getOutputFile().isEmpty() == true)
  {
    QString ss = QObject::tr(": The output file must be set before executing this filter.");
    addErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
  }

  QFileInfo fi(getOutputFile());
  QDir parentPath(fi.path());
  if (parentPath.exists() == false)
  {
    QString ss = QObject::tr("The directory path for the output file does not exist.");
    addWarningMessage(getHumanLabel(), ss, -1);
  }

  if (fi.suffix().compare("") == 0)
  {
    setOutputFile(getOutputFile().append(".csv"));
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindRadialDist::preflight()
{

  dataCheck();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindRadialDist::execute()
{
  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  setErrorCondition(0);

  size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  dataCheck(false, 0, totalFeatures, 0);
  if (getErrorCondition() < 0)
  {
    return;
  }

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(m_OutputFile);
  QString parentPath = fi.path();
  QDir dir;
  if(!dir.mkpath(parentPath))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
    notifyErrorMessage(ss, -1);
    setErrorCondition(-1);
    return;
  }


  find_boundingbox();
  find_radialdist();


  notifyStatusMessage("Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindRadialDist::find_radialdist()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  float x, y, z;
  float xn, yn, zn;
  float dist;
  size_t numfeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  bool writeFile = !m_OutputFile.isEmpty();


  std::ofstream outFile;
  if (writeFile)
  {
    outFile.open(m_OutputFile.toLatin1().data());
  }
  size_t udims[3] =
  { 0, 0, 0 };
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif


  int numbins = 40;
  int ESDStepSize = 10;
  int number = 0;
  float totalvolume = 0;
  float largestESD = 0;
  float largestDistToSurface = 0;
  QVector<float> distToSurface(numfeatures, -1);
  for (size_t i = 1; i < numfeatures; i++)
  {

    if(m_SurfaceFeatures[i] == false)
    {
      number++;
      totalvolume = totalvolume + m_Volumes[i];
      if(m_EquivalentDiameters[i] > largestESD) { largestESD = m_EquivalentDiameters[i]; }
      dist = (m_Centroids[3 * i] - boundbox[1]);
      if((boundbox[2] - m_Centroids[3 * i]) < dist) { dist = (boundbox[2] - m_Centroids[3 * i]); }
      if((m_Centroids[3 * i + 1] - boundbox[3]) < dist) { dist = (m_Centroids[3 * i + 1] - boundbox[3]); }
      if((boundbox[4] - m_Centroids[3 * i + 1]) < dist) { dist = (boundbox[4] - m_Centroids[3 * i + 1]); }
      if((m_Centroids[3 * i + 2] - boundbox[5]) < dist) { dist = (m_Centroids[3 * i + 2] - boundbox[5]); }
      if((boundbox[6] - m_Centroids[3 * i + 2]) < dist) { dist = (boundbox[6] - m_Centroids[3 * i + 2]); }
      distToSurface[i] = dist;
      if(dist > largestDistToSurface) { largestDistToSurface = dist; }
    }

  }
  float binSize = largestDistToSurface / float(numbins);
  float avgDensity = float(number) / totalvolume;
  int sizebins = (largestESD / ESDStepSize) + 1;
  QVector<QVector<float> > count(sizebins);
  QVector<QVector<float> > volume(sizebins);
  for (size_t i = 0; i < count.size(); i++)
  {
    count[i].fill(0.0f, numbins);
    volume[i].fill(0.0f, numbins);
  }
  for (size_t i = 1; i < numfeatures; i++)
  {
    if(m_SurfaceFeatures[i] == false)
    {
      for(int j = 0; j < numbins; j++)
      {
        if(j < int(distToSurface[i] / binSize))
        {
          volume[int(m_EquivalentDiameters[i] / ESDStepSize)][j] = volume[int(m_EquivalentDiameters[i] / ESDStepSize)][j] + ((4.0 / 3.0) * DREAM3D::Constants::k_Pi * float((j + 1) * binSize) * float((j + 1) * binSize) * float((j + 1) * binSize)) - ((4.0 / 3.0) * DREAM3D::Constants::k_Pi * float(j * binSize) * float(j * binSize) * float(j * binSize));
        }
      }
      x = m_Centroids[3 * i];
      y = m_Centroids[3 * i + 1];
      z = m_Centroids[3 * i + 2];
      for (size_t j = 1; j < numfeatures; j++)
      {
        if(m_SurfaceFeatures[j] == false && i != j)
        {
          xn = m_Centroids[3 * j];
          yn = m_Centroids[3 * j + 1];
          zn = m_Centroids[3 * j + 2];
          dist = ((x - xn) * (x - xn)) + ((y - yn) * (y - yn)) + ((z - zn) * (z - zn));
          dist = sqrt(dist);
          if(int(dist / binSize) < int(distToSurface[i] / binSize))
          {
            count[int(m_EquivalentDiameters[i] / ESDStepSize)][int(dist / binSize)]++;
          }
        }
      }
    }
  }

  if (outFile.is_open() == true)
  {
    for (int i = 0; i < numbins; i++)
    {
      outFile << float(i)*binSize << " ";
    }
    outFile << "\n";
    for (size_t i = 0; i < count.size(); i++)
    {
      for (int j = 0; j < numbins; j++)
      {
        float value = (count[i][j] / volume[i][j]) / avgDensity;
        if(count[i][j] == 0) { value = 0; }
        outFile << value << " ";
      }
      outFile << "\n";
    }
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindRadialDist::find_boundingbox()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  size_t size = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  float coords[7];
  float x, y, z;
  float dist[7];
  float mindist;
  int sidetomove, move;
  boundbox[1] = 0;
  boundbox[2] = m->getXPoints() * m->getXRes();
  boundbox[3] = 0;
  boundbox[4] = m->getYPoints() * m->getYRes();
  boundbox[5] = 0;
  boundbox[6] = m->getZPoints() * m->getZRes();
  for (size_t i = 1; i < size; i++)
  {
    if(m_SurfaceFeatures[i] == true)
    {
      move = 1;
      mindist = 10000000000.0;
      x = m_Centroids[3 * i];
      y = m_Centroids[3 * i + 1];
      z = m_Centroids[3 * i + 2];
      coords[1] = x;
      coords[2] = x;
      coords[3] = y;
      coords[4] = y;
      coords[5] = z;
      coords[6] = z;
      for(int j = 1; j < 7; j++)
      {
        dist[j] = 10000000000.0;
        if(j % 2 == 1)
        {
          if(coords[j] > boundbox[j]) { dist[j] = (coords[j] - boundbox[j]); }
          if(coords[j] <= boundbox[j]) { move = 0; }
        }
        if(j % 2 == 0)
        {
          if(coords[j] < boundbox[j]) { dist[j] = (boundbox[j] - coords[j]); }
          if(coords[j] >= boundbox[j]) { move = 0; }
        }
        if(dist[j] < mindist) { mindist = dist[j], sidetomove = j; }
      }
      if(move == 1)
      {
        boundbox[sidetomove] = coords[sidetomove];
      }
    }
  }

}

