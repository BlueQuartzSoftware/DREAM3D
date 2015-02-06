/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2013 Dr. Joseph C. Tucker (UES, Inc.)
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *                   FA8650-07-D-5800 and FA8650-10-D-5226
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FindFeatureClustering.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/RadialDistributionFunction.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureClustering::FindFeatureClustering() :
  AbstractFilter(),
  m_ErrorOutputFile(),
  m_NumberOfBins(1),
  m_PhaseNumber(1),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, ""),
  m_RemoveBiasedFeatures(false),
  m_EquivalentDiametersArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::EquivalentDiameters),
  m_FeaturePhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_CentroidsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Centroids),
  m_ClusteringListArrayName(DREAM3D::FeatureData::ClusteringList),
  m_NewEnsembleArrayArrayName("RDF"),
  m_MaxMinArrayName(getNewEnsembleArrayArrayName() + "MaxMinDistances"),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL),
  m_CentroidsArrayName(DREAM3D::FeatureData::Centroids),
  m_Centroids(NULL),
  m_EquivalentDiametersArrayName(DREAM3D::FeatureData::EquivalentDiameters),
  m_EquivalentDiameters(NULL),
  m_NewEnsembleArray(NULL),
  m_BiasedFeaturesArrayName(DREAM3D::FeatureData::BiasedFeatures),
  m_BiasedFeatures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureClustering::~FindFeatureClustering()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureClustering::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Number Of Bins for RDF", "NumberOfBins", FilterParameterWidgetType::IntWidget, getNumberOfBins(), false, ""));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("EquivalentDiameters", "EquivalentDiametersArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getEquivalentDiametersArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("FeaturePhases", "FeaturePhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeaturePhasesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Centroids", "CentroidsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCentroidsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("BiasedFeatures", "BiasedFeaturesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getBiasedFeaturesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Ensemble Attribute Matrix Name", "CellEnsembleAttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellEnsembleAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Clustering List Array Name", "ClusteringListArrayName", FilterParameterWidgetType::StringWidget, getClusteringListArrayName(), true));
  //parameters.push_back(FilterParameter::New("Max and Min Distances Array Name", "MaxMinArrayName", FilterParameterWidgetType::StringWidget, getMaxMinArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("NewEnsembleArray", "NewEnsembleArrayArrayName", FilterParameterWidgetType::StringWidget, getNewEnsembleArrayArrayName(), true, ""));

  parameters.push_back(FilterParameter::New("Phase Number", "PhaseNumber", FilterParameterWidgetType::IntWidget, getPhaseNumber(), false, ""));
  QStringList linkedProps("BiasedFeaturesArrayPath");
  parameters.push_back(LinkedBooleanFilterParameter::New("Remove Biased Features", "RemoveBiasedFeatures", getRemoveBiasedFeatures(), linkedProps, false));




  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindFeatureClustering::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCellEnsembleAttributeMatrixName(reader->readDataArrayPath("CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName()));
  setNumberOfBins( reader->readValue( "NumberOfBins", getNumberOfBins() ) );
  setNewEnsembleArrayArrayName(reader->readString("NewEnsembleArrayArrayName", getNewEnsembleArrayArrayName() ) );
  setMaxMinArrayName(reader->readString("MaxMinArrayName", getMaxMinArrayName()));
  setClusteringListArrayName(reader->readString("ClusteringListArrayName", getClusteringListArrayName() ) );
  setCentroidsArrayPath(reader->readDataArrayPath("CentroidsArrayPath", getCentroidsArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setEquivalentDiametersArrayPath(reader->readDataArrayPath("EquivalentDiametersArrayPath", getEquivalentDiametersArrayPath() ) );
  setPhaseNumber( reader->readValue("PhaseNumber", getPhaseNumber() ) );
  setBiasedFeaturesArrayPath(reader->readDataArrayPath("BiasedFeaturesArrayPath", getBiasedFeaturesArrayPath() ) );
  setRemoveBiasedFeatures( reader->readValue( "RemoveBiasedFeatures", getRemoveBiasedFeatures() ) );



  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindFeatureClustering::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(CellEnsembleAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(NumberOfBins)
  DREAM3D_FILTER_WRITE_PARAMETER(ClusteringListArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(CentroidsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(EquivalentDiametersArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(NewEnsembleArrayArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(MaxMinArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(PhaseNumber)
  DREAM3D_FILTER_WRITE_PARAMETER(RemoveBiasedFeatures)
  DREAM3D_FILTER_WRITE_PARAMETER(BiasedFeaturesArrayPath)

  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureClustering::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  QVector<size_t> dims(1, 1);
  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getClusteringListArrayName() );
  m_ClusteringList = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */



  m_EquivalentDiametersPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getEquivalentDiametersArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_EquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  m_CentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCentroidsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CentroidsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Centroids = m_CentroidsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */


  int numComp = m_NumberOfBins;
  m_NewEnsembleArrayArrayName = m_SelectedFeatureArrayPath.getDataArrayName() + QString("RDF");
  dims[0] = numComp;
  tempPath.update(getCellEnsembleAttributeMatrixName().getDataContainerName(), getCellEnsembleAttributeMatrixName().getAttributeMatrixName(), getNewEnsembleArrayArrayName() );
  m_NewEnsembleArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, tempPath, 0, dims); /* Assigns the shared_ptr<>(this, tempPath, 0, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NewEnsembleArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NewEnsembleArray = m_NewEnsembleArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 2;
  m_MaxMinArrayName = getNewEnsembleArrayArrayName() + QString("MaxMinDistances");
  tempPath.update(getCellEnsembleAttributeMatrixName().getDataContainerName(), getCellEnsembleAttributeMatrixName().getAttributeMatrixName(), getMaxMinArrayName() );
  m_MaxMinArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, tempPath, 0, dims);
  if( NULL != m_MaxMinArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_MaxMinArray = m_MaxMinArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */


  if(m_RemoveBiasedFeatures == true)
  {
    dims[0] = 1;
    m_BiasedFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getBiasedFeaturesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_BiasedFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_BiasedFeatures = m_BiasedFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureClustering::preflight()
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
void FindFeatureClustering::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  find_clustering();
  notifyStatusMessage(getHumanLabel(), "FindFeatureClustering Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureClustering::find_clustering()
{
  bool writeErrorFile = true;
  std::ofstream outFile;
  if(m_ErrorOutputFile.isEmpty() == false)
  {
    outFile.open(m_ErrorOutputFile.toLatin1().data(), std::ios_base::binary);
    writeErrorFile = true;
  }

  float x, y, z;
  float xn, yn, zn;
  float r;


  int32_t bin;
  int32_t ensemble;
  int32_t totalPPTfeatures = 0;
  float min = 1000000.0f;
  float max = 0.0f;
  float value;
  float sizex, sizey, sizez, totalvol, totalpoints;
  float normFactor;


  std::vector<std::vector<float> > clusteringlist;
  std::vector<float> oldcount(m_NumberOfBins);
  std::vector<float> randomRDF;




  size_t totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_EquivalentDiametersArrayPath.getDataContainerName());

  size_t udims[3] =
  { 0, 0, 0 };
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  { static_cast<DimType>(udims[0]), static_cast<DimType>(udims[1]), static_cast<DimType>(udims[2]), };

  sizex = dims[0] * m->getXRes();
  sizey = dims[1] * m->getYRes();
  sizez = dims[2] * m->getZRes();
  totalvol = sizex * sizey * sizez;
  totalpoints = dims[0] * dims[1] * dims[2];


  //initialize boxdims and boxres vectors
  std::vector<float> boxdims(3);
  boxdims[0] = sizex;
  boxdims[1] = sizey;
  boxdims[2] = sizez;

  std::vector<float> boxres(3);
  boxres[0] = m->getXRes();
  boxres[1] = m->getYRes();
  boxres[2] = m->getZRes();



  for (size_t i = 1; i < totalFeatures; i++)
  {
    if (m_FeaturePhases[i] == m_PhaseNumber) {totalPPTfeatures++;}
  }


  clusteringlist.resize(totalFeatures);



  for (size_t i = 1; i < totalFeatures; i++)
  {
    if (i % 1000 == 0)
    {

      QString ss = QObject::tr("Working On Feature %1 of %2").arg(i).arg(totalFeatures);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    }
    x = m_Centroids[3 * i];
    y = m_Centroids[3 * i + 1];
    z = m_Centroids[3 * i + 2];

    for (size_t j = i + 1; j < totalFeatures; j++)
    {
      if (m_FeaturePhases[i] == m_FeaturePhases[j] && m_FeaturePhases[i] == m_PhaseNumber)
      {
        xn = m_Centroids[3 * j];
        yn = m_Centroids[3 * j + 1];
        zn = m_Centroids[3 * j + 2];

        r = sqrtf((x - xn) * (x - xn) + (y - yn) * (y - yn) + (z - zn) * (z - zn));

        clusteringlist[i].push_back(r);
        clusteringlist[j].push_back(r);


        if(writeErrorFile == true && m_FeaturePhases[j] == 2)
        {
          outFile << r << "\n" << r << "\n";
        }
      }
    }
  }


  for (size_t i = 1; i < totalFeatures; i++)
  {
    for (size_t j = 0; j < clusteringlist[i].size(); j++)
    {
      if (m_FeaturePhases[i] == m_PhaseNumber)
      {
        value = clusteringlist[i][j];
        if(value > max) { max = value; }
        if(value < min) { min = value; }
      }
    }
  }

  float stepsize = (max - min) / m_NumberOfBins;

  m_MaxMinArray[(m_PhaseNumber * 2)] = max;
  m_MaxMinArray[(m_PhaseNumber * 2) + 1] = min;

  for (size_t i = 1; i < totalFeatures; i++)
  {
    for (size_t j = 0; j < clusteringlist[i].size(); j++)
    {
      if (m_FeaturePhases[i] == m_PhaseNumber)
      {
        if(m_RemoveBiasedFeatures == false || m_BiasedFeatures[i] == false)
        {
          ensemble = m_FeaturePhases[i];
          bin = (clusteringlist[i][j] - min) / stepsize;
          if(bin >= m_NumberOfBins) { bin = m_NumberOfBins - 1; }
          m_NewEnsembleArray[(m_NumberOfBins * ensemble) + bin]++;
        }
      }
    }
  }






//Generate random distribution based on same box size and same stepsize
  float max_box_distance = sqrtf((sizex * sizex) + (sizey * sizey) + (sizez * sizez));
  int32_t current_num_bins = ceil((max_box_distance - min) / (stepsize));

  randomRDF.resize(current_num_bins + 1);
//Call this function to generate the random distribution, which is normalized by the total number of distances
  randomRDF = RadialDistributionFunction::GenerateRandomDistribution(min, max, m_NumberOfBins, boxdims, boxres);

//Scale the random distribution by the number of distances in this particular instance
  normFactor = totalPPTfeatures * (totalPPTfeatures - 1);
  for (size_t i = 0; i < randomRDF.size(); i++)
  {
    randomRDF[i] = randomRDF[i] * normFactor;
  }


  for (size_t i = 0; i < m_NumberOfBins; i++)
  {
    oldcount[i] = m_NewEnsembleArray[(m_NumberOfBins * m_PhaseNumber) + i];
    m_NewEnsembleArray[(m_NumberOfBins * m_PhaseNumber) + i] = oldcount[i] / randomRDF[i + 1];
  }





//    std::ofstream testFile3;
//    testFile3.open("/Users/Shared/Data/PW_Work/OUTFILE/normalized_target.txt");
//    for (size_t i = 0; i < m_NumberOfBins; i++)
//    {
//    testFile3 << "\n" << m_NewEnsembleArray[(m_NumberOfBins*m_PhaseNumber) + i];
//    }
//    testFile3.close();

//    std::ofstream testFile4;
//    testFile4.open("/Users/Shared/Data/PW_Work/OUTFILE/randomrdf_target.txt");
//    for (size_t i = 0; i < randomRDF.size(); i++)
//    {
//    testFile4 << "\n" << randomRDF[i];
//    }
//    testFile4.close();

//    std::ofstream testFile7;
//    testFile7.open("/Users/Shared/Data/PW_Work/OUTFILE/targetRaw.txt");
//    for (size_t i = 0; i < oldcount.size(); i++)
//    {
//    testFile7 << "\n" << oldcount[i];
//    }
//    testFile7.close();




  for (size_t i = 1; i < totalFeatures; i++)
  {
    // Set the vector for each list into the Clustering Object
    NeighborList<float>::SharedVectorType sharedClustLst(new std::vector<float>);
    sharedClustLst->assign(clusteringlist[i].begin(), clusteringlist[i].end());
    m_ClusteringList.lock()->setList(static_cast<int>(i), sharedClustLst);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindFeatureClustering::newFilterInstance(bool copyFilterParameters)
{
  FindFeatureClustering::Pointer filter = FindFeatureClustering::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureClustering::getCompiledLibraryName()
{ return Statistics::StatisticsBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureClustering::getGroupName()
{ return DREAM3D::FilterGroups::StatisticsFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureClustering::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MorphologicalFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindFeatureClustering::getHumanLabel()
{ return "Find Feature Clustering"; }

