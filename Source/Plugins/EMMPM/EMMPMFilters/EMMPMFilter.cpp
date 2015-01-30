/*
 * Your License or Copyright Information can go here
 */

#include "EMMPMFilter.h"

#include <QtCore/QString>
#include <QRgb>

#include "EMMPM/EMMPMConstants.h"

#include "EMMPMLib/EMMPMLib.h"
#include "EMMPMLib/Common/EMTime.h"
#include "EMMPMLib/Common/EMMPM_Math.h"
#include "EMMPMLib/Core/EMMPM_Data.h"
#include "EMMPMLib/Core/EMMPM.h"
#include "EMMPMLib/Common/StatsDelegate.h"
#include "EMMPMLib/Core/InitializationFunctions.h"
#include "EMMPMLib/Core/EMMPMUtilities.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMFilter::EMMPMFilter() :
  AbstractFilter(),
  m_InputDataArrayPath(),
  m_NumClasses(4),
  m_ExchangeEnergy(0.5f),
  m_HistogramLoops(5),
  m_SegmentationLoops(5),
  m_UseSimulatedAnnealing(false),
  m_UseGradientPenalty(false),
  m_GradientPenalty(1.0f),
  m_UseCurvaturePenalty(false),
  m_CurvaturePenalty(1.0f),
  m_RMax(15.0f),
  m_EMLoopDelay(1),
  m_OutputDataArrayPath()
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMFilter::~EMMPMFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMFilter::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(FilterParameter::New("Select Input Array", "InputDataArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getInputDataArrayPath()));
  parameters.push_back(FilterParameter::New("Num Classes", "NumClasses", FilterParameterWidgetType::IntWidget, getNumClasses()));
  parameters.push_back(FilterParameter::New("Exchange Energy", "ExchangeEnergy", FilterParameterWidgetType::DoubleWidget, getExchangeEnergy()));
  parameters.push_back(FilterParameter::New("Histogram Loops (EM)", "HistogramLoops", FilterParameterWidgetType::IntWidget, getHistogramLoops()));
  parameters.push_back(FilterParameter::New("Segmentation Loops (MPM)", "SegmentationLoops", FilterParameterWidgetType::IntWidget, getSegmentationLoops()));

  parameters.push_back(FilterParameter::New("Use Simulated Annealing", "UseSimulatedAnnealing", FilterParameterWidgetType::BooleanWidget, getUseSimulatedAnnealing()));

  QStringList linkedProps("GradientPenalty");
  parameters.push_back(LinkedBooleanFilterParameter::New("Use Gradient Penalty", "UseGradientPenalty", getUseGradientPenalty(), linkedProps, false));
  parameters.push_back(FilterParameter::New("Gradient Penalty (Beta E)", "GradientPenalty", FilterParameterWidgetType::DoubleWidget, getGradientPenalty()));

  linkedProps.clear();
  linkedProps << "CurvaturePenalty" << "RMax" << "EMLoopDelay";
  parameters.push_back(LinkedBooleanFilterParameter::New("Use Curvature Penalty", "UseCurvaturePenalty", getUseCurvaturePenalty(), linkedProps));
  parameters.push_back(FilterParameter::New("Curvature Penalty (Beta C)", "CurvaturePenalty", FilterParameterWidgetType::DoubleWidget, getCurvaturePenalty()));
  parameters.push_back(FilterParameter::New("R Max", "RMax", FilterParameterWidgetType::DoubleWidget, getRMax()));
  parameters.push_back(FilterParameter::New("EM Loop Delay", "EMLoopDelay", FilterParameterWidgetType::IntWidget, getEMLoopDelay()));

  parameters.push_back(FilterParameter::New("Output Data", "", FilterParameterWidgetType::SeparatorWidget, "", false));
  parameters.push_back(FilterParameter::New("Created Data Array", "OutputDataArrayPath", FilterParameterWidgetType::DataArrayCreationWidget, getOutputDataArrayPath()));



  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputDataArrayPath(reader->readDataArrayPath("InputDataArrayPath", getInputDataArrayPath()));
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
  setOutputDataArrayPath(reader->readDataArrayPath("OutputDataArrayPath", getOutputDataArrayPath()));

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EMMPMFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);

  DREAM3D_FILTER_WRITE_PARAMETER(InputDataArrayPath)
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
  DREAM3D_FILTER_WRITE_PARAMETER(OutputDataArrayPath)

  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMFilter::dataCheck()
{
  setErrorCondition(0);

  QVector<size_t> cDims(1, 1); // We need a single component, gray scale image
  m_InputImagePtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(this, getInputDataArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_InputImagePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_InputImage = m_InputImagePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */


  m_OutputImagePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this,  getOutputDataArrayPath(), 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_OutputImagePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_OutputImage = m_OutputImagePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */


  if(getNumClasses() > 15)
  {
    setErrorCondition(-62000);
    QString ss = QObject::tr("The Maximum number of classes is 15");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMFilter::preflight()
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
void EMMPMFilter::execute()
{
  int err = 0;
  // typically run your dataCheck function to make sure you can get that far and all your variables are initialized
  dataCheck();
  // Check to make sure you made it through the data check. Errors would have been reported already so if something
  // happens to fail in the dataCheck() then we simply return
  if(getErrorCondition() < 0) { return; }
  setErrorCondition(0);

  // Copy all the variables from the filter into the EMmpm data structure.
  EMMPM_Data::Pointer data = EMMPM_Data::New();
  data->initVariables();
  data->initType = EMMPM_Basic;

  InitializationFunction::Pointer initFunction = BasicInitialization::New();

   // Set the initialization function based on the parameters
   switch(data->initType)
   {
     case EMMPM_ManualInit:
       initFunction = InitializationFunction::New();
       break;
     case EMMPM_UserInitArea:
       initFunction = UserDefinedAreasInitialization::New();
       break;
     default:
	   initFunction = BasicInitialization::New();
       break;
   }

   data->classes = getNumClasses();
   data->in_beta = getExchangeEnergy();
   data->emIterations = getHistogramLoops();
   data->mpmIterations = getSegmentationLoops();
   
   data->colorTable[0] = qRgb(255, 0, 255);
   data->colorTable[1] = qRgb(255, 255, 255);

   data->min_variance[0] = 4.5;
   data->min_variance[1] = 4.5;

   data->w_gamma[0] = 0;
   data->w_gamma[1] = 1;

   DataArrayPath dap = getInputDataArrayPath();
   AttributeMatrix::Pointer am = getDataContainerArray()->getAttributeMatrix(dap);
   QVector<size_t> tDims = am->getTupleDimensions();
   IDataArray::Pointer iDataArray = am->getAttributeArray(getInputDataArrayPath().getDataArrayName());
   QVector<size_t> cDims = iDataArray->getComponentDimensions();

   data->columns = tDims[0];
   data->rows = tDims[1];
   data->dims = 1; // We operate on a single channel | single component "image".
   data->inputImageChannels = cDims[0];
   
   data->simulatedAnnealing = (char)(getUseSimulatedAnnealing());
   data->useGradientPenalty = getUseGradientPenalty();
   data->beta_e = getGradientPenalty();
   data->useCurvaturePenalty = getUseCurvaturePenalty();
   data->beta_c = getCurvaturePenalty();
   data->r_max = getRMax();
   data->ccostLoopDelay = getEMLoopDelay();

   //Assign our data array allocated input and output images into the EMMPM_Data class
   data->inputImage = m_InputImage;
   data->xt =  m_OutputImage;

   // Allocate all the memory here
   data->allocateDataStructureMemory();

   // Create a new StatsDelegate so the EMMPM algorith has somewhere to write its statistics
   StatsDelegate::Pointer statsDelegate = StatsDelegate::New();

  // Start the EM/MPM process going
  EMMPM::Pointer emmpm = EMMPM::New();

  emmpm->setData(data);
  emmpm->setStatsDelegate(statsDelegate.get());
  emmpm->setInitializationFunction(initFunction);

  // Connect up the Error/Warning/Progress object so the filter can report those things
  connect(emmpm.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
			this, SLOT(broadcastPipelineMessage(const PipelineMessage&)));

  emmpm->execute();

  // We manually set the pointers to NULL so that the EMMPM_Data class does not try to free the memory
  data->inputImage = NULL;
  data->xt = NULL;
  //data->outputImage = NULL;

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
const QString EMMPMFilter::getCompiledLibraryName()
{
  return EMMPMPlugin::EMMPMBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EMMPMFilter::getGroupName()
{
  return "EMMPM";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EMMPMFilter::getHumanLabel()
{
  return "EMMPM Segmentation";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EMMPMFilter::getSubGroupName()
{
  return "Segmentation";
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer EMMPMFilter::newFilterInstance(bool copyFilterParameters)
{
  /*
  * write code to optionally copy the filter parameters from the current filter into the new instance
  */
  EMMPMFilter::Pointer filter = EMMPMFilter::New();
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

