/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "EstablishShapeTypes.h"

#include <QtCore/QFileInfo>

#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/HDF5ScopedFileSentinel.h"


#include "DREAM3DLib/IOFilters/DataContainerReader.h"

#include "SyntheticBuilding/SyntheticBuildingConstants.h"

#define INIT_SYNTH_VOLUME_CHECK(var, errCond) \
  if (m_##var <= 0) { QString ss = QObject::tr(":%1 must be a value > 0\n").arg( #var); notifyErrorMessage(getHumanLabel(), ss, errCond);}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EstablishShapeTypes::EstablishShapeTypes() :
  AbstractFilter(),
  m_InputPhaseTypesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::PhaseTypes),
  m_ShapeTypesArrayName(DREAM3D::EnsembleData::ShapeTypes)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EstablishShapeTypes::~EstablishShapeTypes()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishShapeTypes::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("Phase Types Array Name", "InputPhaseTypesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("Shape Types Array Name", "ShapeTypesArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  ShapeTypesFilterParameter::Pointer sType_parameter = ShapeTypesFilterParameter::New(
        "Shape Types", "ShapeTypeData", FilterParameterWidgetType::ShapeTypeSelectionWidget,
        "UInt32Vector_t", "PhaseCount", "InputPhaseTypesArrayPath", false);
  parameters.push_back(sType_parameter);
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void EstablishShapeTypes::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputPhaseTypesArrayPath(reader->readDataArrayPath("InputPhaseTypesArrayPath", getInputPhaseTypesArrayPath() ) );
  setShapeTypesArrayName(reader->readString("ShapeTypesArrayName", getShapeTypesArrayName() ) );
  QVector<uint32_t> data = getShapeTypeData().d;
  data = reader->readArray("ShapeTypeData", data);
  UInt32Vector_t vec;
  vec.d = data;
  setShapeTypeData(vec);
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EstablishShapeTypes::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("InputPhaseTypesArrayPath", getInputPhaseTypesArrayPath() );
  writer->writeValue("ShapeTypesArrayName", getShapeTypesArrayName() );
  writer->writeValue("ShapeTypeData", getShapeTypeData().d );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishShapeTypes::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  DataContainerArray::Pointer dca = getDataContainerArray();

  QVector<size_t> dims(1, 1);
  m_PhaseTypesPtr = dca->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getInputPhaseTypesArrayPath(), dims);
  if( NULL != m_PhaseTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Get the DataContainer first - same as phase types
  VolumeDataContainer* m = dca->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getInputPhaseTypesArrayPath().getDataContainerName());
  if(getErrorCondition() < 0) { return; }

  // Now get the AttributeMatrix that the user wants to use to store the ShapeTypes array - sme as phase types
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getInputPhaseTypesArrayPath().getAttributeMatrixName(), -990);
  if(getErrorCondition() < 0) { return; }
  // Now create the output Shape Types Array
  tempPath.update(getInputPhaseTypesArrayPath().getDataContainerName(), getInputPhaseTypesArrayPath().getAttributeMatrixName(), getShapeTypesArrayName() );
  m_ShapeTypesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<UInt32ArrayType, AbstractFilter>(this, tempPath, true, dims); /* Assigns the shared_ptr<>(this, tempPath, true, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ShapeTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ShapeTypes = m_ShapeTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishShapeTypes::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishShapeTypes::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  // Copy the data from the internal QVector into the actual ShapeTypes array from the data container
  for(qint32 i = 0; i < m_ShapeTypeData.d.size(); i++)
  {
    m_ShapeTypesPtr.lock()->setValue(i, m_ShapeTypeData.d[i]);
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EstablishShapeTypes::getPhaseCount()
{
  DataContainerArray::Pointer dca = getDataContainerArray();

  AttributeMatrix::Pointer inputAttrMat = dca->getAttributeMatrix(getInputPhaseTypesArrayPath());
  if (NULL == inputAttrMat.get() ) { return 0; }
  QVector<size_t> tupleDims = inputAttrMat->getTupleDimensions();
  size_t phaseCount = 0;
  for(qint32 i = 0; i < tupleDims.size(); i++)
  {
    phaseCount = phaseCount + tupleDims[i];
  }
  return phaseCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer EstablishShapeTypes::newFilterInstance(bool copyFilterParameters)
{
  EstablishShapeTypes::Pointer filter = EstablishShapeTypes::New();
  if(true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters() );

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

      if(parameter->isConditional() == true)
      {
        QVariant cond = property(parameter->getConditionalProperty().toLatin1().constData() );
        ok = filter->setProperty(parameter->getConditionalProperty().toLatin1().constData(), cond);
        if(false == ok)
        {
          QString ss = QString("%1::newFilterInstance()\nError occurred transferring the Filter Parameter '%2' in Filter '%3' to the filter instance. "
                              " The filter parameter has a conditional property '%4'. The transfer of this property from the old filter to the new filter failed."
                              " Please report this issue to the developers of this filter.").arg(filter->getNameOfClass())
                              .arg(parameter->getPropertyName())
                              .arg(filter->getHumanLabel())
                              .arg(parameter->getConditionalProperty());
          Q_ASSERT_X(ok, __FILE__, ss.toLatin1().constData());
        }
      }
    }
  }
  return filter;
}
