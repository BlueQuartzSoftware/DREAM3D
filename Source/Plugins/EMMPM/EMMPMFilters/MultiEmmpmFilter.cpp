/*
* Your License or Copyright Information can go here
*/

#include "MultiEmmpmFilter.h"

#include <QtCore/QString>
#include <QRgb>

#include "EMMPM/EMMPMConstants.h"

#include "EMMPM/EMMPMLib/EMMPMLib.h"
#include "EMMPM/EMMPMLib/Common/EMTime.h"
#include "EMMPM/EMMPMLib/Common/EMMPM_Math.h"
#include "EMMPM/EMMPMLib/Core/EMMPM_Data.h"
#include "EMMPM/EMMPMLib/Core/EMMPM.h"
#include "EMMPM/EMMPMLib/Common/StatsDelegate.h"
#include "EMMPM/EMMPMLib/Core/InitializationFunctions.h"
#include "EMMPM/EMMPMLib/Core/EMMPMUtilities.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiEmmpmFilter::MultiEmmpmFilter() :
EMMPMFilter(),
m_OutputAttributeMatrixName("EMMPM Output"),
m_OutputArrayPrefix("Segmented_"),
m_UsePreviousMuSigma(true)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiEmmpmFilter::~MultiEmmpmFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiEmmpmFilter::setupFilterParameters()
{
  FilterParameterVector parameters = getFilterParameters();
  parameters[0] = FilterParameter::New("Select Input AttributeMatrix", "InputAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getInputDataArrayPath());


  // Look for the OutputDataArrayPath and replace with our OutputAttributeMatrixName instead
  for ( qint32 i = 0; i < parameters.size(); i++ )
  {
    FilterParameter::Pointer& p = parameters[i];
    if ( p->getPropertyName().compare("OutputDataArrayPath") == 0 )
    {
        parameters[i] = FilterParameter::New("Output Attribute Matrix Name", "OutputAttributeMatrixName", FilterParameterWidgetType::StringWidget, getOutputAttributeMatrixName(), false);
    }
  }

  parameters.push_back(FilterParameter::New("Use Mu/Sigma from Previous Image as Initialization for Current Image", "UsePreviousMuSigma", FilterParameterWidgetType::BooleanWidget, getUsePreviousMuSigma(), false));
  parameters.push_back(FilterParameter::New("Output ArrayName Prefix", "OutputArrayPrefix", FilterParameterWidgetType::StringWidget, getOutputArrayPrefix(), false));

  // Set the new parameters back into the class
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiEmmpmFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputAttributeMatrixPath(reader->readDataArrayPath("InputAttributeMatrixPath", getInputAttributeMatrixPath()));
  setNumClasses(reader->readValue("NumClasses", getNumClasses()));
  setExchangeEnergy(reader->readValue("ExchangeEnergy", getExchangeEnergy()));
  setHistogramLoops(reader->readValue("HistogramLoops", getHistogramLoops()));
  setSegmentationLoops(reader->readValue("SegmentationLoops", getSegmentationLoops()));
  setUseSimulatedAnnealing(reader->readValue("UseSimulatedAnnealing", getUseSimulatedAnnealing()));
  setUseGradientPenalty(reader->readValue("UseGradientPenalty", getUseGradientPenalty()));
  setGradientPenalty(reader->readValue("GradientPenalty", getGradientPenalty()));
  setUseCurvaturePenalty(reader->readValue("UseCurvaturePenalty", getUseCurvaturePenalty()));
  setCurvaturePenalty(reader->readValue("CurvaturePenalty", getCurvaturePenalty()));
  setRMax(reader->readValue("RMax", getRMax()));
  setEMLoopDelay(reader->readValue("EMLoopDelay", getEMLoopDelay()));
  setOutputAttributeMatrixName(reader->readString("OutputAttributeMatrixName", getOutputAttributeMatrixName()));
  setUsePreviousMuSigma(reader->readValue("UsePreviousMuSigma", getUsePreviousMuSigma()));
  setOutputArrayPrefix(reader->readString("OutputArrayPrefix", getOutputArrayPrefix()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MultiEmmpmFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);

  DREAM3D_FILTER_WRITE_PARAMETER(InputAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(NumClasses)
  DREAM3D_FILTER_WRITE_PARAMETER(ExchangeEnergy)
  DREAM3D_FILTER_WRITE_PARAMETER(HistogramLoops)
  DREAM3D_FILTER_WRITE_PARAMETER(SegmentationLoops)
  DREAM3D_FILTER_WRITE_PARAMETER(UseSimulatedAnnealing)
  DREAM3D_FILTER_WRITE_PARAMETER(UseGradientPenalty)
  DREAM3D_FILTER_WRITE_PARAMETER(GradientPenalty)
  DREAM3D_FILTER_WRITE_PARAMETER(UseCurvaturePenalty)
  DREAM3D_FILTER_WRITE_PARAMETER(CurvaturePenalty)
  DREAM3D_FILTER_WRITE_PARAMETER(RMax)
  DREAM3D_FILTER_WRITE_PARAMETER(EMLoopDelay)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(UsePreviousMuSigma)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputArrayPrefix)

  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiEmmpmFilter::dataCheck()
{
  setErrorCondition(0);

  QVector<size_t> cDims(1, 1); // We need a single component, gray scale image

#if 0
  m_InputImagePtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(this, getInputDataArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (NULL != m_InputImagePtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
    m_InputImage = m_InputImagePtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if (getErrorCondition() < 0) { return; }

  ImageGeom::Pointer image = getDataContainerArray()->getDataContainer(getInputDataArrayPath().getDataContainerName())->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if (getErrorCondition() < 0 || NULL == image.get()) { return; }

  m_OutputImagePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, getOutputDataArrayPath(), 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (NULL != m_OutputImagePtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
    m_OutputImage = m_OutputImagePtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

 #endif

  if(getOutputArrayPrefix().isEmpty())
  {
    setErrorCondition(-62002);
    QString message = QObject::tr("Using a Prefix (even a single Alpha-Numeric Value) is needed so that the output XDMF files can be written correctly.");
    notifyErrorMessage(getHumanLabel(), message, getErrorCondition());
  }

  if(getInputAttributeMatrixPath().getDataContainerName().isEmpty())
  {
    setErrorCondition(-62003);
    QString message = QObject::tr("The DataContainer name is empty for the Input AttributeMatrix Path.");
    notifyErrorMessage(getHumanLabel(), message, getErrorCondition());
    return;
  }

  if(getInputAttributeMatrixPath().getAttributeMatrixName().isEmpty())
  {
    setErrorCondition(-62004);
    QString message = QObject::tr("The AttributeMatrix name is empty for the Input AttributeMatrix Path.");
    notifyErrorMessage(getHumanLabel(), message, getErrorCondition());
    return;
  }

  // Make sure the Data Container exists
  DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(getInputAttributeMatrixPath().getDataContainerName());
  if(NULL == dc.get())
  {
    return;
  }

  // Make sure the input AttributeMatrix exists
  AttributeMatrix::Pointer inAM = dc->getPrereqAttributeMatrix<AbstractFilter>(this, getInputAttributeMatrixPath().getAttributeMatrixName(), 10000);
  if(getErrorCondition() < 0)
  {
    return;
  }


  // Now create our output attributeMatrix which will contain all of our segmented images
  QVector<size_t> tDims = inAM->getTupleDimensions();
  AttributeMatrix::Pointer outAM = getDataContainerArray()->getDataContainer(getInputAttributeMatrixPath().getDataContainerName())->createNonPrereqAttributeMatrix<AbstractFilter>(this, getOutputAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0)
  {
    return;
  }


  // Get the list of the array names from the input attribute matrix
  QList<QString> arrayNames = inAM->getAttributeArrayNames();
  DataArrayPath path = getInputAttributeMatrixPath();
  path.setAttributeMatrixName(getOutputAttributeMatrixName());
  for ( int i = 0; i < arrayNames.size(); i++ )
  {
    path.setDataArrayName(arrayNames.at(i));
    outAM->createAndAddAttributeArray<UInt8ArrayType, AbstractFilter, uint8_t>(this, arrayNames.at(i), 0, cDims);
  }

  // The EM/MPM Library has a hard coded MAX Classes of 16
  if (getNumClasses() > 15)
  {
    setErrorCondition(-62000);
    QString ss = QObject::tr("The Maximum number of classes is 15");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  // It does not make any sense if we want anything less than 2 classes
  if (getNumClasses() < 2)
  {
    setErrorCondition(-62001);
    QString ss = QObject::tr("The Minimum number of classes is 2");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiEmmpmFilter::preflight()
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
void MultiEmmpmFilter::execute()
{
  int err = 0;
  // typically run your dataCheck function to make sure you can get that far and all your variables are initialized
  dataCheck();
  // Check to make sure you made it through the data check. Errors would have been reported already so if something
  // happens to fail in the dataCheck() then we simply return
  if (getErrorCondition() < 0) { return; }
  setErrorCondition(0);

  AttributeMatrix::Pointer am = getDataContainerArray()->getAttributeMatrix(getInputAttributeMatrixPath());
  QList<QString> arrayNames = am->getAttributeArrayNames();
  QListIterator<QString> iter(arrayNames);

  QString msgPrefix = getMessagePrefix();
  int i = 1;
  // This is the routine that sets up the EM/MPM to segment the image
  while (iter.hasNext())
  {
    DataArrayPath arrayPath = getInputAttributeMatrixPath();
    QString name = iter.next();

    arrayPath.setDataArrayName(name);
    setInputDataArrayPath(arrayPath);

    // Change the output AttributeMatrix
    arrayPath.setAttributeMatrixName(getOutputAttributeMatrixName());
    QString outName = getOutputArrayPrefix() + arrayPath.getDataArrayName();
    arrayPath.setDataArrayName(outName);
    setOutputDataArrayPath(arrayPath);

    QString prefix = QObject::tr("%1 (Array %2 of %3)").arg(msgPrefix).arg(i).arg(arrayNames.size());
    setMessagePrefix(prefix);

    if ( i == 2 && getUsePreviousMuSigma())
    {
      setEmmpmInitType(EMMPM_ManualInit);
    }
    else
    {
      setEmmpmInitType(EMMPM_Basic);
    }

    EMMPMFilter::execute();
    if ( getErrorCondition() < 0 ) { break; }
    i++;

    if ( getCancel() ) { break; }
  }

  /* If some error occurs this code snippet can report the error up the call chain*/
  if (getErrorCondition() < 0)
  {
    QString ss = QObject::tr("There was a generic error running the EMMPM Segmentation Filter. Please contact the developers.");
    setErrorCondition(-60009);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiEmmpmFilter::getCompiledLibraryName()
{
  return EMMPMPlugin::EMMPMBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiEmmpmFilter::getGroupName()
{
  return "Segmentation";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiEmmpmFilter::getHumanLabel()
{
  return "MultiArray EMMPM Segmentation";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiEmmpmFilter::getSubGroupName()
{
  return "EMMPM";
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MultiEmmpmFilter::newFilterInstance(bool copyFilterParameters)
{
  /*
  * write code to optionally copy the filter parameters from the current filter into the new instance
  */
  MultiEmmpmFilter::Pointer filter = MultiEmmpmFilter::New();
  if (true == copyFilterParameters)
  {
    DREAM3D_COPY_INSTANCEVAR(InputAttributeMatrixPath)
    DREAM3D_COPY_INSTANCEVAR(NumClasses)
    DREAM3D_COPY_INSTANCEVAR(ExchangeEnergy)
    DREAM3D_COPY_INSTANCEVAR(HistogramLoops)
    DREAM3D_COPY_INSTANCEVAR(SegmentationLoops)
    DREAM3D_COPY_INSTANCEVAR(UseSimulatedAnnealing)
    DREAM3D_COPY_INSTANCEVAR(UseGradientPenalty)
    DREAM3D_COPY_INSTANCEVAR(GradientPenalty)
    DREAM3D_COPY_INSTANCEVAR(UseCurvaturePenalty)
    DREAM3D_COPY_INSTANCEVAR(CurvaturePenalty)
    DREAM3D_COPY_INSTANCEVAR(RMax)
    DREAM3D_COPY_INSTANCEVAR(EMLoopDelay)
    DREAM3D_COPY_INSTANCEVAR(OutputAttributeMatrixName)

  }
  return filter;
}

