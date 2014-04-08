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

#include "FindFeatureReferenceMisorientations.h"

#include <limits>


#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/OrientationMath.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureReferenceMisorientations::FindFeatureReferenceMisorientations() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
/*[]*/m_FeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
/*[]*/m_CellPhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Phases),
/*[]*/m_CrystalStructuresArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
/*[]*/m_QuatsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Quats),
/*[]*/m_AvgQuatsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
/*[]*/m_GBEuclideanDistancesArrayPath(DREAM3D::Defaults::SomePath),
  m_FeatureAvgMisorientationsArrayName(DREAM3D::FeatureData::FeatureAvgMisorientations),
  m_FeatureReferenceMisorientationsArrayName(DREAM3D::CellData::FeatureReferenceMisorientations),
  m_ReferenceOrientation(0),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellPhases(NULL),
  m_GBEuclideanDistancesArrayName(DREAM3D::CellData::GBEuclideanDistances),
  m_GBEuclideanDistances(NULL),
  m_FeatureReferenceMisorientations(NULL),
  m_QuatsArrayName(DREAM3D::CellData::Quats),
  m_Quats(NULL),
  m_FeatureAvgMisorientations(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureReferenceMisorientations::~FindFeatureReferenceMisorientations()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureReferenceMisorientations::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Reference Orientation");
    parameter->setPropertyName("ReferenceOrientation");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    parameter->setValueType("unsigned int");
    QVector<QString> choices;
    choices.push_back("Feature's Average Orientation");
    choices.push_back("Orientation at Feature's Centroid");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
/*[]*/parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
/*[]*/parameters.push_back(FilterParameter::New("CellPhases", "CellPhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
/*[]*/parameters.push_back(FilterParameter::New("CrystalStructures", "CrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
/*[]*/parameters.push_back(FilterParameter::New("Quats", "QuatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
/*[]*/parameters.push_back(FilterParameter::New("AvgQuats", "AvgQuatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
/*[]*/parameters.push_back(FilterParameter::New("GBEuclideanDistances", "GBEuclideanDistancesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
/*##*/parameters.push_back(FilterParameter::New("FeatureAvgMisorientations", "FeatureAvgMisorientationsArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
/*##*/parameters.push_back(FilterParameter::New("FeatureReferenceMisorientations", "FeatureReferenceMisorientationsArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindFeatureReferenceMisorientations::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
/*[]*/setFeatureReferenceMisorientationsArrayName(reader->readString("FeatureReferenceMisorientationsArrayName", getFeatureReferenceMisorientationsArrayName() ) );
/*[]*/setFeatureAvgMisorientationsArrayName(reader->readString("FeatureAvgMisorientationsArrayName", getFeatureAvgMisorientationsArrayName() ) );
  setGBEuclideanDistancesArrayPath(reader->readDataArrayPath("GBEuclideanDistancesArrayPath", getGBEuclideanDistancesArrayPath() ) );
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath() ) );
  setQuatsArrayPath(reader->readDataArrayPath("QuatsArrayPath", getQuatsArrayPath() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setReferenceOrientation( reader->readValue("ReferenceOrientation", getReferenceOrientation()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindFeatureReferenceMisorientations::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
/*[]*/writer->writeValue("FeatureReferenceMisorientationsArrayName", getFeatureReferenceMisorientationsArrayName() );
/*[]*/writer->writeValue("FeatureAvgMisorientationsArrayName", getFeatureAvgMisorientationsArrayName() );
  writer->writeValue("GBEuclideanDistancesArrayPath", getGBEuclideanDistancesArrayPath() );
  writer->writeValue("AvgQuatsArrayPath", getAvgQuatsArrayPath() );
  writer->writeValue("QuatsArrayPath", getQuatsArrayPath() );
  writer->writeValue("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() );
  writer->writeValue("CellPhasesArrayPath", getCellPhasesArrayPath() );
  writer->writeValue("FeatureIdsArrayPath", getFeatureIdsArrayPath() );
  writer->writeValue("ReferenceOrientation", getReferenceOrientation() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureReferenceMisorientations::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || NULL == cellAttrMat.get() ) { return; }


  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_FeatureAvgMisorientationsPtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_FeatureAvgMisorientationsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getFeatureAvgMisorientationsArrayName() );
////==>MIKE_GROEBER_FIX m_FeatureAvgMisorientationsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureAvgMisorientationsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureAvgMisorientations = m_FeatureAvgMisorientationsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_FeatureReferenceMisorientationsPtr = cellAttrMat->createNonPrereqArray<DataArray<float>, AbstractFilter, float>(this, m_FeatureReferenceMisorientationsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getFeatureReferenceMisorientationsArrayName() );
////==>MIKE_GROEBER_FIX m_FeatureReferenceMisorientationsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureReferenceMisorientationsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureReferenceMisorientations = m_FeatureReferenceMisorientationsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), dims)
                           ; /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 4;
  m_QuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuatsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(m_ReferenceOrientation == 0)
  {
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  else if(m_ReferenceOrientation == 1)
  {
    dims[0] = 1;
  m_GBEuclideanDistancesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getGBEuclideanDistancesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_GBEuclideanDistancesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_GBEuclideanDistances = m_GBEuclideanDistancesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureReferenceMisorientations::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureReferenceMisorientations::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
  int64_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  QuatF q1;
  QuatF q2;
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  float w;
  float n1, n2, n3;
  float r1, r2, r3;
  unsigned int phase1 = Ebsd::CrystalStructure::UnknownCrystalStructure;
  unsigned int phase2 = Ebsd::CrystalStructure::UnknownCrystalStructure;
  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef uint32_t DimType;
  uint32_t maxUInt32 = std::numeric_limits<uint32_t>::max();
  // We have more points than can be allocated on a 32 bit machine. Assert Now.
  if(totalPoints > maxUInt32)
  {
    setErrorCondition(-666);
    notifyErrorMessage(getHumanLabel(), "More Points than can be help in memory on a 32 bit machine. Try reducing the size of the input volume.", -666);
    return;
  }
#else
  typedef int64_t DimType;
#endif

  size_t gnum;
  float dist;
  QVector<size_t> m_Centers(totalFeatures, 0);
  QVector<float> m_CenterDists(totalFeatures, 0);
  if(m_ReferenceOrientation == 1)
  {
    for (int64_t i = 0; i < totalPoints; i++)
    {
      gnum = m_FeatureIds[i];
      dist = m_GBEuclideanDistances[i];
      if(dist >= m_CenterDists[gnum])
      {
        m_CenterDists[gnum] = dist;
        m_Centers[gnum] = i;
      }
    }
  }


  FloatArrayType::Pointer avgMisoPtr = FloatArrayType::CreateArray(totalFeatures * 2, "AVERAGE MISORIENTATION INTERNAL_ARRAY");
  avgMisoPtr->initializeWithZeros();
  float* avgMiso = avgMisoPtr->getPointer(0);

  DimType xPoints = static_cast<DimType>(udims[0]);
  DimType yPoints = static_cast<DimType>(udims[1]);
  DimType zPoints = static_cast<DimType>(udims[2]);
  DimType point = 0;
  size_t idx = 0;
  for (DimType col = 0; col < xPoints; col++)
  {
    for (DimType row = 0; row < yPoints; row++)
    {
      for (DimType plane = 0; plane < zPoints; plane++)
      {
        point = (plane * xPoints * yPoints) + (row * xPoints) + col;
        if (m_FeatureIds[point] > 0 && m_CellPhases[point] > 0)
        {
          QuaternionMathF::Copy(quats[point], q1);
          phase1 = m_CrystalStructures[m_CellPhases[point]];
          if(m_ReferenceOrientation == 0)
          {
            QuaternionMathF::Copy(avgQuats[m_FeatureIds[point]], q2);
          }
          else if(m_ReferenceOrientation == 1)
          {
            gnum = m_FeatureIds[point];
            QuaternionMathF::Copy(quats[m_Centers[gnum]], q2);
            phase2 = m_CrystalStructures[m_CellPhases[m_Centers[gnum]]];
          }
          w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
          OrientationMath::AxisAngletoRod(w, n1, n2, n3, r1, r2, r3);
          m_OrientationOps[phase1]->getMDFFZRod(r1, r2, r3);
          w = w * (180.0f / DREAM3D::Constants::k_Pi);
          m_FeatureReferenceMisorientations[point] = w;
          idx = m_FeatureIds[point] * 2;
          avgMiso[idx + 0]++;
          avgMiso[idx + 1] = avgMiso[idx + 1] + w;
        }
        if (m_FeatureIds[point] == 0 || m_CellPhases[point] == 0)
        {
          m_FeatureReferenceMisorientations[point] = 0;
        }
      }
    }
  }

  for (int64_t i = 1; i < totalFeatures; i++)
  {
    idx = i * 2;
    m_FeatureAvgMisorientations[i] = avgMiso[idx + 1] / avgMiso[idx];
    if(avgMiso[idx] == 0.0f) { m_FeatureAvgMisorientations[i] = 0.0; }
  }

  notifyStatusMessage(getHumanLabel(), "FindFeatureReferenceMisorientations Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindFeatureReferenceMisorientations::newFilterInstance(bool copyFilterParameters)
{
  FindFeatureReferenceMisorientations::Pointer filter = FindFeatureReferenceMisorientations::New();
  if(true == copyFilterParameters)
  {
    //Loop over each Filter Parameter that is registered to the filter either through this class or a parent class
    // and copy the value from the current instance of the object into the "new" instance that was just created
    QVector<FilterParameter::Pointer> options = getFilterParameters(); // Get the current set of filter parameters
    for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
    {
      FilterParameter* parameter = (*iter).get();
      if (parameter->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0 )
      {
        continue; // Skip this type of filter parameter as it has nothing to do with anything in the filter.
      }
      // Get the property from the current instance of the filter
      QVariant var = property(parameter->getPropertyName().toLatin1().constData());
      bool ok = filter->setProperty(parameter->getPropertyName().toLatin1().constData(), var);
      if(false == ok)
      {
        QString ss = QString("Error occurred transferring the Filter Parameter '%1' in Filter '%2' to the filter instance. The pipeline may run but the underlying filter will NOT be using the values from the GUI."
                             " Please report this issue to the developers of this filter.").arg(parameter->getPropertyName()).arg(filter->getHumanLabel());
        Q_ASSERT_X(ok, __FILE__, ss.toLatin1().constData());
      }
    }
  }
  return filter;
}
