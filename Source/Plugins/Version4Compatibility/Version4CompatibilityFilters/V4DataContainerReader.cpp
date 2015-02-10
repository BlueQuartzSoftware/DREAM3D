/*
 * Your License or Copyright Information can go here
 */

#include "V4DataContainerReader.h"

#include <QtCore/QString>

#include "Version4Compatibility/Version4CompatibilityConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
V4DataContainerReader::V4DataContainerReader() :
  AbstractFilter()
/* DO NOT FORGET TO INITIALIZE ALL YOUR DREAM3D Filter Parameters HERE */
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
V4DataContainerReader::~V4DataContainerReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void V4DataContainerReader::setupFilterParameters()
{
  FilterParameterVector parameters;
  /* There are several types of FilterParameter classes to choose from and several
  * options for each class type. The programmer can put the entire invocation into
  * a single line if they want. For example:
  *
  *   parameters.push_back(FilterParameter::New("Reference Direction", "ReferenceDir", FilterParameterWidgetType::FloatVec3Widget, getReferenceDir(), false));
  * or the programmer can create a FilterParameter like usual C++ codes:
  * {
  *  FilterParameter::Pointer parameter = FilterParameter::New();
  *  parameter->setHumanLabel("Eulers Array");
  *  parameter->setPropertyName("CellEulerAnglesArrayName");
  *  parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
  *  parameter->setUnits("");
  *  parameters.push_back(parameter);
  * }
  */
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void V4DataContainerReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /*
   Place code in here that will read the parameters from a file
   setOutputFile( reader->readValue("OutputFile", getOutputFile() ) );
   */
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int V4DataContainerReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  /* Place code that will write the inputs values into a file. reference the AbstractFilterParametersWriter class for the proper API to use. */
  /*  DREAM3D_FILTER_WRITE_PARAMETER(OutputFile) */
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void V4DataContainerReader::dataCheck()
{
  setErrorCondition(0);

  /* Example code for preflighting looking for a valid string for the output file
   * but not necessarily the fact that the file exists: Example code to make sure
   * we have something in a string before proceeding.*/
  /*
  if (m_OutputFile.empty() == true)
  {
    QString ss = QObject::tr("Output file name was not set").arg(getHumanLabel());
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, -1);
    return;
  }
  * We can also check for the availability of REQUIRED ARRAYS:
  * QVector<size_t> dims(1, 1);
  * // Assigns the shared_ptr<> to an instance variable that is a weak_ptr<>
  * m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), dims);
  *  // Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object
  * if( NULL != m_CellPhasesPtr.lock().get() )
  * {
  *   // Now assign the raw pointer to data from the DataArray<T> object
  *   m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  * }
  *
  * We can also CREATE a new array to dump new data into
  *   tempPath.update(m_CellEulerAnglesArrayPath.getDataContainerName(), m_CellEulerAnglesArrayPath.getAttributeMatrixName(), getCellIPFColorsArrayName() );
  * // Assigns the shared_ptr<> to an instance variable that is a weak_ptr<>
  * m_CellIPFColorsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, tempPath, 0, dims);
  * // Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object
  * if( NULL != m_CellIPFColorsPtr.lock().get() )
  * {
  * // Now assign the raw pointer to data from the DataArray<T> object
  * m_CellIPFColors = m_CellIPFColorsPtr.lock()->getPointer(0);
  * }
  */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void V4DataContainerReader::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.

  /* *** THIS FILTER NEEDS TO BE CHECKED *** */
  setErrorCondition(0xABABABAB);
  QString ss = QObject::tr("Filter is NOT updated for IGeometry Redesign. A Programmer needs to check this filter. Please report this to the DREAM3D developers.");
  notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  /* *** THIS FILTER NEEDS TO BE CHECKED *** */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString V4DataContainerReader::getCompiledLibraryName()
{
  return Version4Compatibility::Version4CompatibilityBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString V4DataContainerReader::getGroupName()
{
  return "Version4Compatibility";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString V4DataContainerReader::getHumanLabel()
{
  return "V4DataContainerReader";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString V4DataContainerReader::getSubGroupName()
{
  return "Misc";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void V4DataContainerReader::execute()
{
  int err = 0;
  // typically run your dataCheck function to make sure you can get that far and all your variables are initialized
  dataCheck();
  // Check to make sure you made it through the data check. Errors would have been reported already so if something
  // happens to fail in the dataCheck() then we simply return
  if(getErrorCondition() < 0) { return; }
  setErrorCondition(0);

  /* Place all your code to execute your filter here. */

  /* If some error occurs this code snippet can report the error up the call chain*/
  if (err < 0)
  {
    QString ss = QObject::tr("Some error message");
    setErrorCondition(-99999999);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer V4DataContainerReader::newFilterInstance(bool copyFilterParameters)
{
  /*
  * write code to optionally copy the filter parameters from the current filter into the new instance
  */
  V4DataContainerReader::Pointer filter = V4DataContainerReader::New();
  if(true == copyFilterParameters)
  {
    /* If the filter uses all the standard Filter Parameter Widgets you can probabaly get
     * away with using this method to copy the filter parameters from the current instance
     * into the new instance
     */
    copyFilterParameterInstanceVariables(filter.get());
    /* If your filter is using a lot of custom FilterParameterWidgets @see ReadH5Ebsd then you
     * may need to copy each filter parameter explicitly plus any other instance variables that
     * are needed into the new instance. Here is some example code from ReadH5Ebsd
     */
    //    DREAM3D_COPY_INSTANCEVAR(OutputFile)
    //    DREAM3D_COPY_INSTANCEVAR(ZStartIndex)
    //    DREAM3D_COPY_INSTANCEVAR(ZEndIndex)
    //    DREAM3D_COPY_INSTANCEVAR(ZResolution)
  }
  return filter;
}

