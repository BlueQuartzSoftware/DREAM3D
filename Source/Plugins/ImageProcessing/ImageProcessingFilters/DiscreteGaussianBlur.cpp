/*
 * Your License or Copyright Information can go here
 */

#include "DiscreteGaussianBlur.h"

#include <QtCore/QString>

#include "ITKUtilities.h"
#include "itkDiscreteGaussianImageFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DiscreteGaussianBlur::DiscreteGaussianBlur() :
  AbstractFilter(),
  m_SelectedCellArrayPath("", "", ""),
  m_SaveAsNewArray(true),
  m_Stdev(2.0),
  m_SelectedCellArrayArrayName(""),
  m_SelectedCellArray(NULL),
  m_NewCellArrayName(""),
  m_NewCellArray(NULL)
  {
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DiscreteGaussianBlur::~DiscreteGaussianBlur()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DiscreteGaussianBlur::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Array to Process", "SelectedCellArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedCellArrayPath(), false, ""));
  QStringList linkedProps;
  linkedProps << "NewCellArrayName";
  parameters.push_back(FilterParameter::NewConditional("Save As New Array", "SaveAsNewArray", FilterParameterWidgetType::LinkedBooleanWidget, getSaveAsNewArray(), false, linkedProps));

  parameters.push_back(FilterParameter::New("Created Array Name", "NewCellArrayName", FilterParameterWidgetType::StringWidget, getNewCellArrayName(), false, ""));
  parameters.push_back(FilterParameter::New("Standard Deviation", "Stdev", FilterParameterWidgetType::DoubleWidget, getStdev(), false));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DiscreteGaussianBlur::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath( reader->readDataArrayPath( "SelectedCellArrayPath", getSelectedCellArrayPath() ) );
  setNewCellArrayName( reader->readString( "NewCellArrayName", getNewCellArrayName() ) );
  setSaveAsNewArray( reader->readValue( "SaveAsNewArray", getSaveAsNewArray() ) );
  setStdev( reader->readValue( "Stdev", getStdev() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DiscreteGaussianBlur::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedCellArrayPath", getSelectedCellArrayPath() );
  writer->writeValue("NewCellArrayName", getNewCellArrayName() );
  writer->writeValue("SaveAsNewArray", getSaveAsNewArray() );
  writer->writeValue("Stdev", getStdev() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DiscreteGaussianBlur::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  QVector<size_t> dims(1, 1);
  m_SelectedCellArrayPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<ImageProcessing::DefaultPixelType>, AbstractFilter>(this, getSelectedCellArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SelectedCellArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SelectedCellArray = m_SelectedCellArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_SaveAsNewArray == false) m_NewCellArrayName = "thisIsATempName";
  tempPath.update(getSelectedCellArrayPath().getDataContainerName(), getSelectedCellArrayPath().getAttributeMatrixName(), getNewCellArrayName() );
  m_NewCellArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<ImageProcessing::DefaultPixelType>, AbstractFilter, ImageProcessing::DefaultPixelType>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NewCellArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NewCellArray = m_NewCellArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DiscreteGaussianBlur::preflight()
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
void DiscreteGaussianBlur::execute()
{
  int err = 0;
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getSelectedCellArrayPath().getDataContainerName());
  QString attrMatName = getSelectedCellArrayPath().getAttributeMatrixName();

  //wrap m_RawImageData as itk::image
  ImageProcessing::DefaultImageType::Pointer inputImage=ITKUtilities::Dream3DtoITK(m, attrMatName, m_SelectedCellArray);

  //create guassian blur filter
  typedef itk::DiscreteGaussianImageFilter< ImageProcessing::DefaultImageType,  ImageProcessing::DefaultImageType > GaussianFilterType;
  GaussianFilterType::Pointer gaussianFilter = GaussianFilterType::New();
  gaussianFilter->SetInput(inputImage);
  gaussianFilter->SetVariance(m_Stdev*m_Stdev);

  //have filter write to dream3d array instead of creating its own buffer
  ITKUtilities::SetITKOutput(gaussianFilter->GetOutput(), m_NewCellArray, m_NewCellArrayPtr.lock()->getNumberOfTuples());

  //execute filter
  gaussianFilter->Update();

  //array name changing/cleanup
  if(m_SaveAsNewArray == false)
  {
    AttributeMatrix::Pointer attrMat = m->getAttributeMatrix(m_SelectedCellArrayPath.getAttributeMatrixName());
    attrMat->removeAttributeArray(m_SelectedCellArrayPath.getDataArrayName());
    bool check = attrMat->renameAttributeArray(m_NewCellArrayName, m_SelectedCellArrayPath.getDataArrayName());
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer DiscreteGaussianBlur::newFilterInstance(bool copyFilterParameters)
{
  DiscreteGaussianBlur::Pointer filter = DiscreteGaussianBlur::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

