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
#include "CombineAttributeMatrices.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

// Include the MOC generated file for this class
#include "moc_CombineAttributeMatrices.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CombineAttributeMatrices::CombineAttributeMatrices() :
AbstractFilter(),
m_FirstAttributeMatrixPath("", "", ""),
m_SecondAttributeMatrixPath("", "", ""),
m_FirstIndexArrayPath("", "", ""),
m_SecondIndexArrayPath("", "", ""),
m_CombinedAttributeMatrixName(""),
m_NewIndexArrayName(""),
m_FirstIndex(NULL),
m_SecondIndex(NULL),
m_NewIndex(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CombineAttributeMatrices::~CombineAttributeMatrices()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CombineAttributeMatrices::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Feature/Ensemble Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req;
    QVector<uint32_t> dcGeomTypes;
    QVector<uint32_t> amTypes;
    amTypes.push_back(DREAM3D::AttributeMatrixType::CellEnsemble);
    amTypes.push_back(DREAM3D::AttributeMatrixType::CellFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::EdgeEnsemble);
    amTypes.push_back(DREAM3D::AttributeMatrixType::EdgeFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::FaceEnsemble);
    amTypes.push_back(DREAM3D::AttributeMatrixType::FaceFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::VertexEnsemble);
    amTypes.push_back(DREAM3D::AttributeMatrixType::VertexFeature);
    dcGeomTypes.push_back(DREAM3D::Defaults::AnyGeometry);
    req.amTypes = amTypes;
    req.dcGeometryTypes = dcGeomTypes;
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("First Feature/Ensemble Attribute Matrix", "FirstAttributeMatrixPath", getFirstAttributeMatrixPath(), FilterParameter::RequiredArray, req));
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Second Feature/Ensemble Attribute Matrix", "SecondAttributeMatrixPath", getSecondAttributeMatrixPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    QVector<uint32_t> dcGeomTypes;
    req.dcGeometryTypes = dcGeomTypes;
    QVector<uint32_t> amTypes;
    amTypes.push_back(DREAM3D::AttributeMatrixType::Cell);
    amTypes.push_back(DREAM3D::AttributeMatrixType::CellFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::Edge);
    amTypes.push_back(DREAM3D::AttributeMatrixType::EdgeFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::Face);
    amTypes.push_back(DREAM3D::AttributeMatrixType::FaceFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::Vertex);
    amTypes.push_back(DREAM3D::AttributeMatrixType::VertexFeature);
    QVector<QString> daTypes;
    daTypes.push_back(DREAM3D::TypeNames::Int32);
    QVector<QVector<size_t> > compDims;
    compDims.resize(1);
    compDims[0].resize(1);
    compDims[0][0] = 1;
    req.dcGeometryTypes = dcGeomTypes;
    req.amTypes = amTypes;
    req.daTypes = daTypes;
    req.componentDimensions = compDims;
    parameters.push_back(DataArraySelectionFilterParameter::New("First Index Array", "FirstIndexArrayPath", getFirstIndexArrayPath(), FilterParameter::RequiredArray, req));
    parameters.push_back(DataArraySelectionFilterParameter::New("Second Index Array", "SecondIndexArrayPath", getSecondIndexArrayPath(), FilterParameter::RequiredArray, req));
  }

  parameters.push_back(SeparatorFilterParameter::New("Cell/Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("New Index Array", "NewIndexArrayName", getNewIndexArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(SeparatorFilterParameter::New("Feature/Ensemble Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Combined Attribute Matrix", "CombinedAttributeMatrixName", getCombinedAttributeMatrixName(), FilterParameter::CreatedArray));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CombineAttributeMatrices::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFirstAttributeMatrixPath(reader->readDataArrayPath("FirstAttributeMatrixPath", getFirstAttributeMatrixPath()));
  setSecondAttributeMatrixPath(reader->readDataArrayPath("SecondAttributeMatrixPath", getSecondAttributeMatrixPath()));
  setCombinedAttributeMatrixName(reader->readString("CombinedAttributeMatrixName", getCombinedAttributeMatrixName()));
  setFirstIndexArrayPath(reader->readDataArrayPath("FirstIndexArrayPath", getFirstIndexArrayPath()));
  setSecondIndexArrayPath(reader->readDataArrayPath("SecondIndexArrayPath", getSecondIndexArrayPath()));
  setNewIndexArrayName(reader->readString("NewIndexArrayName", getNewIndexArrayName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CombineAttributeMatrices::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(FirstAttributeMatrixPath)
  SIMPL_FILTER_WRITE_PARAMETER(SecondAttributeMatrixPath)
  SIMPL_FILTER_WRITE_PARAMETER(CombinedAttributeMatrixName)
  SIMPL_FILTER_WRITE_PARAMETER(FirstIndexArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(SecondIndexArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(NewIndexArrayName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CombineAttributeMatrices::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getFirstAttributeMatrixPath().getDataContainerName(), false);
  if (getErrorCondition() < 0 || NULL == m.get()) { return; }

  if (getFirstAttributeMatrixPath().getDataContainerName().compare(getSecondAttributeMatrixPath().getDataContainerName()) != 0)
  {
    QString ss = QObject::tr("The selected attribute matrices must be in the same data container and currently are not");
    setErrorCondition(-5557);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (getFirstAttributeMatrixPath().getAttributeMatrixName().compare(getSecondAttributeMatrixPath().getAttributeMatrixName()) == 0)
  {
    QString ss = QObject::tr("The selected attribute matrices must be different and currently are the same");
    setErrorCondition(-5558);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  AttributeMatrix::Pointer firstAttrMat = m->getPrereqAttributeMatrix(this, getFirstAttributeMatrixPath().getAttributeMatrixName(), -301);
  AttributeMatrix::Pointer secondAttrMat = m->getPrereqAttributeMatrix(this, getSecondAttributeMatrixPath().getAttributeMatrixName(), -301);
  if (getErrorCondition() < 0) { return; }

  if (firstAttrMat->getType() != secondAttrMat->getType())
  {
    QString ss = QObject::tr("The selected attribute matrices must be of the same type (ie Feature) and currently are not");
    setErrorCondition(-5559);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (getErrorCondition() < 0) { return; }

  //Note that the minus 1 in the totalTuples calculation is to account for the fact that the zeroth tuple in the two attribute matrices should only be counted once, not twice.
  //All Feature or Ensemble AMs should start from 1 and the zeroth tuple can be combined in the two AMs
  size_t totalTuples = firstAttrMat->getNumTuples() + secondAttrMat->getNumTuples() - 1;
  QVector<size_t> tDims(1, totalTuples);
  m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCombinedAttributeMatrixName(), tDims, firstAttrMat->getType());
  if (getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer combinedAttrMat = m->getAttributeMatrix(getCombinedAttributeMatrixName());

  QVector<size_t> cDims(1, 1);
  m_FirstIndexPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFirstIndexArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (NULL != m_FirstIndexPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
    m_FirstIndex = m_FirstIndexPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if (getErrorCondition() < 0) { return; }

  m_SecondIndexPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSecondIndexArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (NULL != m_SecondIndexPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
    m_SecondIndex = m_SecondIndexPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if (getErrorCondition() < 0) { return; }

  // Create arrays on the reference grid to hold data present on the sampling grid
  QList<QString> fArrayNames = firstAttrMat->getAttributeArrayNames();
  for (QList<QString>::iterator iter = fArrayNames.begin(); iter != fArrayNames.end(); ++iter)
  {
    tempPath.update(getFirstAttributeMatrixPath().getDataContainerName(), getCombinedAttributeMatrixName(), *iter);
    IDataArray::Pointer tmpDataArray = firstAttrMat->getPrereqIDataArray<IDataArray, AbstractFilter>(this, *iter, -90001);
    if (getErrorCondition() >= 0)
    {
      QVector<size_t> cDims = tmpDataArray->getComponentDimensions();
      TemplateHelpers::CreateNonPrereqArrayFromArrayType()(this, tempPath, cDims, tmpDataArray);
    }
  }
  QList<QString> sArrayNames = secondAttrMat->getAttributeArrayNames();
  for (QList<QString>::iterator iter = sArrayNames.begin(); iter != sArrayNames.end(); ++iter)
  {
    tempPath.update(getSecondAttributeMatrixPath().getDataContainerName(), getCombinedAttributeMatrixName(), *iter);
    IDataArray::Pointer tmpDataArray = secondAttrMat->getPrereqIDataArray<IDataArray, AbstractFilter>(this, *iter, -90001);
    if (getErrorCondition() >= 0)
    {
      if (fArrayNames.contains(*iter) == false)
      {
        QVector<size_t> cDims = tmpDataArray->getComponentDimensions();
        TemplateHelpers::CreateNonPrereqArrayFromArrayType()(this, tempPath, cDims, tmpDataArray);
      }
    }
  }

  tempPath.update(getFirstIndexArrayPath().getDataContainerName(), getFirstIndexArrayPath().getAttributeMatrixName(), getNewIndexArrayName());
  m_NewIndexPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (NULL != m_NewIndexPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
    m_NewIndex = m_NewIndexPtr.lock()->getPointer(0);
  }    /* Now assign the raw pointer to data from the DataArray<T> object */
  if (getErrorCondition() < 0) { return; }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CombineAttributeMatrices::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

template<typename T>
void copyData(IDataArray::Pointer fromData, IDataArray::Pointer toData, size_t location)
{
  typename DataArray<T>::Pointer fData = std::dynamic_pointer_cast<DataArray<T>>(fromData);
  typename DataArray<T>::Pointer tData = std::dynamic_pointer_cast<DataArray<T>>(toData);

  //only wanting to grab data from tuple 1 to numTuples of the fromData array,s ince the zeroth slot is a placeholder the first AM should already have
  T* src = fData->getPointer(1 * fromData->getNumberOfComponents());
  T* dest = tData->getPointer(location * toData->getNumberOfComponents());
  size_t bytes = sizeof(T) * (fromData->getNumberOfTuples() - 1) * fromData->getNumberOfComponents();
  ::memcpy(dest, src, bytes);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CombineAttributeMatrices::execute()
{
  setErrorCondition(0);
  dataCheck();
  if (getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getFirstAttributeMatrixPath().getDataContainerName());
  AttributeMatrix::Pointer firstAttrMat = m->getAttributeMatrix(getFirstAttributeMatrixPath().getAttributeMatrixName());
  AttributeMatrix::Pointer secondAttrMat = m->getAttributeMatrix(getSecondAttributeMatrixPath().getAttributeMatrixName());
  AttributeMatrix::Pointer combinedAttrMat = m->getAttributeMatrix(getCombinedAttributeMatrixName());
  size_t firstAttrMatNumTuples = firstAttrMat->getNumTuples();

  size_t totalTuples1 = m_SecondIndexPtr.lock()->getNumberOfTuples();
  size_t totalTuples2 = m_SecondIndexPtr.lock()->getNumberOfTuples();
  for (size_t i = 0; i < totalTuples1; i++)
  {
    if (m_FirstIndex > 0) { m_NewIndex[i] = m_FirstIndex[i]; }
  }
  for (size_t i = 0; i < totalTuples2; i++)
  {
    //subtract 1 from the index plus numTuples because the second index should be shifted to account for the zeroth tuple (all AMs above element start at tuple 1)
    if (m_SecondIndex[i] > 0 && m_NewIndex[i] == 0) m_NewIndex[i] = m_SecondIndex[i] + firstAttrMatNumTuples - 1;
    else if (m_SecondIndex[i] > 0 && m_NewIndex[i] != 0)
    {
      QString ss = QObject::tr("When copying the indices, the indices of the two attribute matrices overlapped.  The index of the first attribute matrix was kept.");
      notifyWarningMessage(getHumanLabel(), ss, -111);
    }
  }

  QList<QString> arrayNames = firstAttrMat->getAttributeArrayNames();
  size_t location = 0;
  for (QList<QString>::iterator iter = arrayNames.begin(); iter != arrayNames.end(); ++iter)
  {
    IDataArray::Pointer fromDataArray = firstAttrMat->getAttributeArray(*iter);
    IDataArray::Pointer toDataArray = combinedAttrMat->getAttributeArray(*iter);
    EXECUTE_FUNCTION_TEMPLATE(this, copyData, fromDataArray, fromDataArray, toDataArray, location);
  }

  arrayNames.clear();
  arrayNames = secondAttrMat->getAttributeArrayNames();
  location = firstAttrMatNumTuples;
  for (QList<QString>::iterator iter = arrayNames.begin(); iter != arrayNames.end(); ++iter)
  {
    IDataArray::Pointer fromDataArray = secondAttrMat->getAttributeArray(*iter);
    IDataArray::Pointer toDataArray = combinedAttrMat->getAttributeArray(*iter);
    EXECUTE_FUNCTION_TEMPLATE(this, copyData, fromDataArray, fromDataArray, toDataArray, location);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CombineAttributeMatrices::newFilterInstance(bool copyFilterParameters)
{
  CombineAttributeMatrices::Pointer filter = CombineAttributeMatrices::New();
  if (true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CombineAttributeMatrices::getCompiledLibraryName()
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CombineAttributeMatrices::getBrandingString()
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CombineAttributeMatrices::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CombineAttributeMatrices::getGroupName()
{
  return DREAM3D::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CombineAttributeMatrices::getSubGroupName()
{
  return DREAM3D::FilterSubGroups::MemoryManagementFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CombineAttributeMatrices::getHumanLabel()
{
  return "Combine Feature/Ensemble Attribute Matrices";
}
