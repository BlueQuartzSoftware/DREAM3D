/*
 * Your License or Copyright Information can go here
 */

#include "RawBinaryWriter.h"

#include <QtCore/QString>

#include "IO/IOConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RawBinaryWriter::RawBinaryWriter() :
  AbstractFilter()
  /* DO NOT FORGET TO INITIALIZE ALL YOUR DREAM3D Filter Parameters HERE */
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RawBinaryWriter::~RawBinaryWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RawBinaryWriter::setupFilterParameters()
{
  FilterParameterVector parameters;
  /* There are several types of FilterParameter classes to choose from and several
  * options for each class type. The programmer can put the entire invocation into
  * a single line if they want. For example:
  *
  *   parameters.push_back(FloatVec3FilterParameter::New("Reference Direction", "ReferenceDir", getReferenceDir(), FilterParameter::Uncategorized));
  * or the programmer can create a FilterParameter like usual C++ codes:
  * {
  *  FilterParameter::Pointer parameter = FilterParameter::New();
  *  parameter->setHumanLabel("Eulers Array");
  *  parameter->setPropertyName("CellEulerAnglesArrayName");
  *
  *  parameter->setUnits("");
  *  parameters.push_back(parameter);
  * }
  */
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RawBinaryWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int RawBinaryWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  /* Place code that will write the inputs values into a file. reference the AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RawBinaryWriter::dataCheck()
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
void RawBinaryWriter::preflight()
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
const QString RawBinaryWriter::getCompiledLibraryName()
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RawBinaryWriter::getGroupName()
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RawBinaryWriter::getHumanLabel()
{
  return "RawBinaryWriter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RawBinaryWriter::getSubGroupName()
{
  return "Misc";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RawBinaryWriter::execute()
{
  int err = 0;
  // typically run your dataCheck function to make sure you can get that far and all your variables are initialized
  dataCheck();
  // Check to make sure you made it through the data check. Errors would have been reported already so if something
  // happens to fail in the dataCheck() then we simply return
  if(getErrorCondition() < 0)
  {
    return;
  }
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
AbstractFilter::Pointer RawBinaryWriter::newFilterInstance(bool copyFilterParameters)
{
  /*
  * write code to optionally copy the filter parameters from the current filter into the new instance
  */
  RawBinaryWriter::Pointer filter = RawBinaryWriter::New();
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
    //    SIMPL_COPY_INSTANCEVAR(OutputFile)
    //    SIMPL_COPY_INSTANCEVAR(ZStartIndex)
    //    SIMPL_COPY_INSTANCEVAR(ZEndIndex)
    //    SIMPL_COPY_INSTANCEVAR(ZResolution)
  }
  return filter;
}

