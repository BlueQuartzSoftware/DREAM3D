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

#include "IdentifyDislocationSegments.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/GeometryMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IdentifyDislocationSegments::IdentifyDislocationSegments() :
  AbstractFilter(),
  m_EdgeFeatureAttributeMatrixName(DREAM3D::Defaults::EdgeFeatureAttributeMatrixName),
  m_BurgersVectorsArrayPath(DREAM3D::Defaults::EdgeDataContainerName, DREAM3D::Defaults::EdgeAttributeMatrixName, DREAM3D::EdgeData::BurgersVectors),
  m_SlipPlaneNormalsArrayPath(DREAM3D::Defaults::EdgeDataContainerName, DREAM3D::Defaults::EdgeAttributeMatrixName, DREAM3D::EdgeData::SlipPlaneNormals),
  m_DislocationIdsArrayName(DREAM3D::EdgeData::DislocationIds),
  m_ActiveArrayName(DREAM3D::FeatureData::Active),
  m_BurgersVectorsArrayName(DREAM3D::EdgeData::BurgersVectors),
  m_BurgersVectors(NULL),
  m_SlipPlaneNormalsArrayName(DREAM3D::EdgeData::SlipPlaneNormals),
  m_SlipPlaneNormals(NULL),
  m_DislocationIds(NULL),
  m_Active(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IdentifyDislocationSegments::~IdentifyDislocationSegments()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyDislocationSegments::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("BurgersVectors", "BurgersVectorsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getBurgersVectorsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("SlipPlaneNormals", "SlipPlaneNormalsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSlipPlaneNormalsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Edge Feature Attribute Matrix Name", "EdgeFeatureAttributeMatrixName", FilterParameterWidgetType::StringWidget, getEdgeFeatureAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("DislocationIds", "DislocationIdsArrayName", FilterParameterWidgetType::StringWidget, getDislocationIdsArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("Active", "ActiveArrayName", FilterParameterWidgetType::StringWidget, getActiveArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyDislocationSegments::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setEdgeFeatureAttributeMatrixName(reader->readString("EdgeFeatureAttributeMatrixName", getEdgeFeatureAttributeMatrixName() ) );
  setActiveArrayName(reader->readString("ActiveArrayName", getActiveArrayName() ) );
  setDislocationIdsArrayName(reader->readString("DislocationIdsArrayName", getDislocationIdsArrayName() ) );
  setSlipPlaneNormalsArrayPath(reader->readDataArrayPath("SlipPlaneNormalsArrayPath", getSlipPlaneNormalsArrayPath() ) );
  setBurgersVectorsArrayPath(reader->readDataArrayPath("BurgersVectorsArrayPath", getBurgersVectorsArrayPath() ) );

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int IdentifyDislocationSegments::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(EdgeFeatureAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(ActiveArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(DislocationIdsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SlipPlaneNormalsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(BurgersVectorsArrayPath)

  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyDislocationSegments::updateEdgeFeatureInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyDislocationSegments::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  // Next check the existing DataContainer/AttributeMatrix
  EdgeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<EdgeDataContainer, AbstractFilter>(this, getBurgersVectorsArrayPath().getDataContainerName());
  if(getErrorCondition() < 0) { return; }
  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer edgeFeatureAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getEdgeFeatureAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::EdgeFeature);
  if(getErrorCondition() < 0) { return; }

  // We MUST have Vertices defined.
  if(m->getVertices().get() == NULL)
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "Edge DataContainer missing Vertices", getErrorCondition());
  }
  // We MUST have Edges defined also.
  if(m->getEdges().get() == NULL)
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "Edge DataContainer missing Edges", getErrorCondition());
  }

  //Get the name and create the array in the new data attrMat
  QVector<size_t> dims(1, 3);
  m_BurgersVectorsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this,  getBurgersVectorsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_BurgersVectorsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_BurgersVectors = m_BurgersVectorsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_SlipPlaneNormalsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this,  getSlipPlaneNormalsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SlipPlaneNormalsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SlipPlaneNormals = m_SlipPlaneNormalsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  tempPath.update(getBurgersVectorsArrayPath().getDataContainerName(), getEdgeFeatureAttributeMatrixName(), getDislocationIdsArrayName() );
  m_DislocationIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_DislocationIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_DislocationIds = m_DislocationIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(getBurgersVectorsArrayPath().getDataContainerName(), getEdgeFeatureAttributeMatrixName(), getActiveArrayName() );
  m_ActivePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, true, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IdentifyDislocationSegments::preflight()
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
void IdentifyDislocationSegments::execute()
{
  QString ss;
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  EdgeDataContainer* edc = getDataContainerArray()->getDataContainerAs<EdgeDataContainer>(getBurgersVectorsArrayPath().getDataContainerName());
  AttributeMatrix::Pointer edgeFeatureAttrMat = edc->getAttributeMatrix(getEdgeFeatureAttributeMatrixName());

  VertexArray::Pointer nodesPtr = edc->getVertices();
  //size_t numNodes = nodesPtr->getNumberOfTuples();
  //VertexArray::Vert_t* nodes = nodesPtr->getPointer(0);

  EdgeArray::Pointer edgesPtr = edc->getEdges();
  size_t numEdges = edgesPtr->getNumberOfTuples();
  EdgeArray::Edge_t* edges = edgesPtr->getPointer(0);

  edgesPtr->findEdgesContainingVert();
  Int32DynamicListArray::Pointer edgesContainingVert = edgesPtr->getEdgesContainingVert();

  int dnum = 0;
  size_t size = 0;
  QVector<size_t> tDims(1, dnum);
  int64_t initialEdgesListSize = 100;
  QVector<int64_t> edgeslist(initialEdgesListSize, -1);
  float refBV[3], refSPN[3];
  float neighBV[3], neighSPN[3];
  float angleBV, angleSPN;
  float angleTol = 1.0 * DREAM3D::Constants::k_Pi / 180.0f;
  for(size_t i = 0; i < numEdges; i++)
  {
    size = 0;
    if(m_DislocationIds[i] == 0)
    {
      dnum++;
      m_DislocationIds[i] = dnum;
      edgeslist[size] = i;
      size++;
      tDims[0] = dnum + 1;
      edgeFeatureAttrMat->resizeAttributeArrays(tDims);
      updateEdgeFeatureInstancePointers();
      m_Active[dnum] = true;
      while(size > 0)
      {
        int64_t currentEdge = edgeslist[size - 1];
        refBV[0] = m_BurgersVectors[3 * currentEdge + 0];
        refBV[1] = m_BurgersVectors[3 * currentEdge + 1];
        refBV[2] = m_BurgersVectors[3 * currentEdge + 2];
        refSPN[0] = m_SlipPlaneNormals[3 * currentEdge + 0];
        refSPN[1] = m_SlipPlaneNormals[3 * currentEdge + 1];
        refSPN[2] = m_SlipPlaneNormals[3 * currentEdge + 2];
        size -= 1;
        for(int iter = 0; iter < 2; iter++)
        {
          uint16_t eCount = edgesContainingVert->getNumberOfElements(edges[currentEdge].verts[iter]);
          int32_t* data = edgesContainingVert->getElementListPointer(edges[currentEdge].verts[iter]);
          for(uint16_t j = 0; j < eCount; j++)
          {
            if(m_DislocationIds[data[j]] == 0)
            {
              neighBV[0] = m_BurgersVectors[3 * data[j] + 0];
              neighBV[1] = m_BurgersVectors[3 * data[j] + 1];
              neighBV[2] = m_BurgersVectors[3 * data[j] + 2];
              neighSPN[0] = m_SlipPlaneNormals[3 * data[j] + 0];
              neighSPN[1] = m_SlipPlaneNormals[3 * data[j] + 1];
              neighSPN[2] = m_SlipPlaneNormals[3 * data[j] + 2];
              angleBV = GeometryMath::AngleBetweenVectors(refBV, neighBV);
              angleSPN = GeometryMath::AngleBetweenVectors(refSPN, neighSPN);
              if((angleBV < angleTol || (DREAM3D::Constants::k_Pi - angleBV) < angleTol) && (angleSPN < angleTol || (DREAM3D::Constants::k_Pi - angleSPN) < angleTol))
              {
                m_DislocationIds[data[j]] = dnum;
                edgeslist[size] = data[j];
                size++;
                if(size >= edgeslist.size())
                {
                  size = edgeslist.size();
                  edgeslist.resize(size + initialEdgesListSize);
                  for(qint32 j = size; j < edgeslist.size(); ++j) { edgeslist[j] = -1; }
                }
              }
            }
          }
        }
      }
    }
  }

  // Generate all the numbers up front
  const int rangeMin = 1;
  const int rangeMax = dnum - 1;
  typedef boost::uniform_int<int> NumberDistribution;
  typedef boost::mt19937 RandomNumberGenerator;
  typedef boost::variate_generator<RandomNumberGenerator&, NumberDistribution> Generator;

  NumberDistribution distribution(rangeMin, rangeMax);
  RandomNumberGenerator generator;
  Generator numberGenerator(generator, distribution);
  generator.seed(static_cast<boost::uint32_t>( QDateTime::currentMSecsSinceEpoch() )); // seed with the current time

  DataArray<int32_t>::Pointer rndNumbers = DataArray<int32_t>::CreateArray(dnum, "New FeatureIds");
  int32_t* gid = rndNumbers->getPointer(0);
  gid[0] = 0;
  QSet<int32_t> featureIdSet;
  featureIdSet.insert(0);
  for(size_t i = 1; i < dnum; ++i)
  {
    gid[i] = i; //numberGenerator();
    featureIdSet.insert(gid[i]);
  }

  size_t r;
  size_t temp;
  //--- Shuffle elements by randomly exchanging each with one other.
  for (size_t i = 1; i < dnum; i++)
  {
    r = numberGenerator(); // Random remaining position.
    if (r >= dnum)
    {
      continue;
    }
    temp = gid[i];
    gid[i] = gid[r];
    gid[r] = temp;
  }

  // Now adjust all the Feature Id values for each Voxel
  for(int64_t i = 0; i < numEdges; ++i)
  {
    m_DislocationIds[i] = gid[ m_DislocationIds[i] ];
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer IdentifyDislocationSegments::newFilterInstance(bool copyFilterParameters)
{
  IdentifyDislocationSegments::Pointer filter = IdentifyDislocationSegments::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifyDislocationSegments::getCompiledLibraryName()
{ return DDDAnalysisToolbox::DDDAnalysisToolboxBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifyDislocationSegments::getGroupName()
{ return DREAM3D::FilterGroups::DDDAnalysisFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifyDislocationSegments::getSubGroupName()
{ return DREAM3D::FilterSubGroups::FeatureIdentificationFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString IdentifyDislocationSegments::getHumanLabel()
{ return "Identify Dislocation Segments"; }

