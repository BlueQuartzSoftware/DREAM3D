/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include "EMMPMFilter.h"

#include "EMMPM/EMMPMConstants.h"
#include "EMMPM/EMMPMLib/EMMPMLib.h"
#include "EMMPM/EMMPMLib/Common/EMTime.h"
#include "EMMPM/EMMPMLib/Common/EMMPM_Math.h"
#include "EMMPM/EMMPMLib/Core/EMMPM_Data.h"
#include "EMMPM/EMMPMLib/Core/EMMPM.h"
#include "EMMPM/EMMPMLib/Common/StatsDelegate.h"
#include "EMMPM/EMMPMLib/Core/InitializationFunctions.h"
#include "EMMPM/EMMPMLib/Core/EMMPMUtilities.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/SIMPLibVersion.h"

#include "EMMPM/EMMPMVersion.h"

// Include the MOC generated file for this class
#include "moc_EMMPMFilter.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMFilter::EMMPMFilter() :
  AbstractFilter(),
  m_InputDataArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::ImageData),
  m_NumClasses(4),
  m_ExchangeEnergy(0.5f),
  m_HistogramLoops(5),
  m_SegmentationLoops(5),
  m_UseSimulatedAnnealing(false),
  m_GradientPenalty(1.0f),
  m_CurvaturePenalty(1.0f),
  m_RMax(15.0f),
  m_EMLoopDelay(1),
  m_OutputDataArrayPath("", "", ""),
  m_EmmpmInitType(EMMPM_Basic)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMFilter::~EMMPMFilter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMFilter::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(IntFilterParameter::New("Number of Classes", "NumClasses", getNumClasses(), FilterParameter::Parameter));
  parameters.push_back(DoubleFilterParameter::New("Exchange Energy", "ExchangeEnergy", getExchangeEnergy(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Histogram Loops (EM)", "HistogramLoops", getHistogramLoops(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Segmentation Loops (MPM)", "SegmentationLoops", getSegmentationLoops(), FilterParameter::Parameter));
  parameters.push_back(BooleanFilterParameter::New("Use Simulated Annealing", "UseSimulatedAnnealing", getUseSimulatedAnnealing(), FilterParameter::Parameter));
  QStringList linkedProps("GradientPenalty");
  parameters.push_back(LinkedBooleanFilterParameter::New("Use Gradient Penalty", "UseGradientPenalty", getUseGradientPenalty(), linkedProps, FilterParameter::Parameter));
  parameters.push_back(DoubleFilterParameter::New("Gradient Penalty (Beta E)", "GradientPenalty", getGradientPenalty(), FilterParameter::Parameter));
  linkedProps.clear();
  linkedProps << "CurvaturePenalty" << "RMax" << "EMLoopDelay";
  parameters.push_back(LinkedBooleanFilterParameter::New("Use Curvature Penalty", "UseCurvaturePenalty", getUseCurvaturePenalty(), linkedProps, FilterParameter::Parameter));
  parameters.push_back(DoubleFilterParameter::New("Curvature Penalty (Beta C)", "CurvaturePenalty", getCurvaturePenalty(), FilterParameter::Parameter));
  parameters.push_back(DoubleFilterParameter::New("R Max", "RMax", getRMax(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("EM Loop Delay", "EMLoopDelay", getEMLoopDelay(), FilterParameter::Parameter));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt8, 1, SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Input Attribute Array", "InputDataArrayPath", getInputDataArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArrayCreationFilterParameter::New("Output Attribute Array", "OutputDataArrayPath", getOutputDataArrayPath(), FilterParameter::CreatedArray, req));
  }
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
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(InputDataArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(NumClasses)
  SIMPL_FILTER_WRITE_PARAMETER(ExchangeEnergy)
  SIMPL_FILTER_WRITE_PARAMETER(HistogramLoops)
  SIMPL_FILTER_WRITE_PARAMETER(SegmentationLoops)
  SIMPL_FILTER_WRITE_PARAMETER(UseSimulatedAnnealing)
  SIMPL_FILTER_WRITE_PARAMETER(UseGradientPenalty)
  SIMPL_FILTER_WRITE_PARAMETER(GradientPenalty)
  SIMPL_FILTER_WRITE_PARAMETER(UseCurvaturePenalty)
  SIMPL_FILTER_WRITE_PARAMETER(CurvaturePenalty)
  SIMPL_FILTER_WRITE_PARAMETER(RMax)
  SIMPL_FILTER_WRITE_PARAMETER(EMLoopDelay)
  SIMPL_FILTER_WRITE_PARAMETER(OutputDataArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMFilter::initialize()
{
  m_PreviousMu.clear();
  m_PreviousSigma.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMFilter::dataCheck()
{
  setErrorCondition(0);

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getInputDataArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 1); // We need a single component, gray scale image
  m_InputImagePtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(this, getInputDataArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(NULL != m_InputImagePtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_InputImage = m_InputImagePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_OutputImagePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, getOutputDataArrayPath(), 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(NULL != m_OutputImagePtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_OutputImage = m_OutputImagePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if (getNumClasses() > 15)
  {
    setErrorCondition(-62000);
    QString ss = QObject::tr("The maximum number of classes is 15");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getNumClasses() < 2)
  {
    setErrorCondition(-62001);
    QString ss = QObject::tr("The minimum number of classes is 2");
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
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }
  initialize();

  // This is the routine that sets up the EM/MPM to segment the image
  segment(getEmmpmInitType());

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMFilter::segment(EMMPM_InitializationType initType)
{
  // Copy all the variables from the filter into the EMmpm data structure.
  EMMPM_Data::Pointer data = EMMPM_Data::New();
  data->initVariables();
  data->initType = initType;

  InitializationFunction::Pointer initFunction = BasicInitialization::New();

  // Set the initialization function based on the parameters
  switch (data->initType)
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

  for (int32_t i = 0; i < data->classes; i++)
  {
    int32_t gray = 255 / (data->classes - 1);
    // Generate a Gray Scale Color Table
    data->colorTable[i] = qRgb(i * gray, i * gray, i * gray);
    // Hard code the minimum variance to 4.5; This could be a user option.
    data->min_variance[i] = 4.5;
    // Do we know what w_gamma is?
    data->w_gamma[i] = i;
  }

  DataArrayPath dap = getInputDataArrayPath();
  AttributeMatrix::Pointer am = getDataContainerArray()->getAttributeMatrix(dap);
  QVector<size_t> tDims = am->getTupleDimensions();
  IDataArray::Pointer iDataArray = am->getAttributeArray(getInputDataArrayPath().getDataArrayName());
  QVector<size_t> cDims = iDataArray->getComponentDimensions();

  data->columns = tDims[0];
  data->rows = tDims[1];
  data->dims = 1; // We operate on a single channel | single component "image".
  data->inputImageChannels = cDims[0];

  data->simulatedAnnealing = (char)( getUseSimulatedAnnealing() );
  data->useGradientPenalty = getUseGradientPenalty();
  data->beta_e = getGradientPenalty();
  data->useCurvaturePenalty = getUseCurvaturePenalty();
  data->beta_c = getCurvaturePenalty();
  data->r_max = getRMax();
  data->ccostLoopDelay = getEMLoopDelay();

  // Assign our data array allocated input and output images into the EMMPM_Data class
  data->inputImage = m_InputImage;
  data->xt = m_OutputImage;

  // Allocate all the memory here
  data->allocateDataStructureMemory();

  // If we are using the "Feedback" loop then we copy the previous Mu/Sigma values into the Mean/Variance
  // variables
  if (data->initType == EMMPM_ManualInit)
  {
    for (int32_t i = 0; i < data->classes; i++)
    {
      for (uint32_t d = 0; d < data->dims; d++)
      {
        data->mean[i * data->dims + d] = m_PreviousMu[i * data->dims + d];
        data->variance[i * data->dims + d] = m_PreviousSigma[i * data->dims + d];
      }
    }
  }

  // Create a new StatsDelegate so the EMMPM algorith has somewhere to write its statistics
  StatsDelegate::Pointer statsDelegate = StatsDelegate::New();

  // Start the EM/MPM process going
  EMMPM::Pointer emmpm = EMMPM::New();

  emmpm->setData(data);
  emmpm->setStatsDelegate(statsDelegate.get());
  emmpm->setInitializationFunction(initFunction);
  emmpm->setMessagePrefix(getMessagePrefix());

  // Connect up the Error/Warning/Progress object so the filter can report those things
  connect(emmpm.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
          this, SLOT(broadcastPipelineMessage(const PipelineMessage&)));

  emmpm->execute();

  // We manually set the pointers to NULL so that the EMMPM_Data class does not try to free the memory
  data->inputImage = NULL;
  data->xt = NULL;

  // Grab the Mu/Sigma values from the current finished segmented image and use those as inputs
  // into the initialization of the next Image to be Segmented
  m_PreviousMu.resize(getNumClasses() * data->dims);
  m_PreviousSigma.resize(getNumClasses() * data->dims);
  if (0)
  {
    std::cout << "--------------------------------------------------------------" << std::endl;
    for(std::vector<float>::size_type i = 0; i < getNumClasses(); i++)
    {
      std::cout << "Mu: " << data->mean[i] << " Variance: " << sqrtf(data->variance[i]) << std::endl;
      for (uint32_t d = 0; d < data->dims; d++)
      {
        m_PreviousMu[i * data->dims + d] = data->mean[i * data->dims + d];
        m_PreviousSigma[i * data->dims + d] = data->variance[i * data->dims + d];
      }
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer EMMPMFilter::newFilterInstance(bool copyFilterParameters)
{
  EMMPMFilter::Pointer filter = EMMPMFilter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EMMPMFilter::getCompiledLibraryName()
{
  return EMMPMConstants::EMMPMBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EMMPMFilter::getBrandingString()
{
  return "EMMPM";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EMMPMFilter::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  EM_MPM::Version::Major() << "." << EM_MPM::Version::Minor() << "." << EM_MPM::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EMMPMFilter::getGroupName()
{ return SIMPL::FilterGroups::ReconstructionFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EMMPMFilter::getSubGroupName()
{ return SIMPL::FilterSubGroups::SegmentationFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EMMPMFilter::getHumanLabel()
{ return "EM/MPM"; }
