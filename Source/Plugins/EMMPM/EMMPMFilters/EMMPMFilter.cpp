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
* SERVICES; LOSS OF USE, Data, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
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
#include "EMMPM/EMMPMLib/Common/EMMPM_Math.h"
#include "EMMPM/EMMPMLib/Common/EMTime.h"
#include "EMMPM/EMMPMLib/Common/StatsDelegate.h"
#include "EMMPM/EMMPMLib/Core/EMMPM.h"
#include "EMMPM/EMMPMLib/Core/EMMPMUtilities.h"
#include "EMMPM/EMMPMLib/Core/InitializationFunctions.h"
#include "EMMPM/EMMPMLib/EMMPMLib.h"
#include "EMMPM/FilterParameters/EMMPMFilterParameter.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/ConstrainedDoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/ConstrainedIntFilterParameter.h"
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
EMMPMFilter::EMMPMFilter()
: AbstractFilter(),
  m_InputDataArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::ImageData),
  m_UseOneBasedValues(true),
  m_NumClasses(2),
  m_ExchangeEnergy(0.5f),
  m_HistogramLoops(5),
  m_SegmentationLoops(5),
  m_UseSimulatedAnnealing(false),
  m_GradientBetaE(1.0f),
  m_CurvatureBetaC(1.0f),
  m_CurvatureRMax(15.0f),
  m_CurvatureEMLoopDelay(1),
  m_OutputDataArrayPath("", "", ""),
  m_EmmpmInitType(EMMPM_Basic),
  m_Data(EMMPM_Data::New())
{
  std::vector<std::vector<double> > tableData(2, std::vector<double>(4));
  tableData[0][0] = 0;
  tableData[0][1] = 4.5;
  tableData[0][2] = 128;
  tableData[0][3] = 20;
  tableData[1][0] = 0;
  tableData[1][1] = 4.5;
  tableData[1][2] = 128;
  tableData[1][3] = 20;
  m_EMMPMTableData.setTableData(tableData);

  QStringList rHeaders;
  rHeaders << "1" << "2";
  m_EMMPMTableData.setRowHeaders(rHeaders);

  QStringList cHeaders;
  cHeaders << "Chem. Pntl" << "Min Std Dev" << "Mean" << "Std Dev";
  m_EMMPMTableData.setColHeaders(cHeaders);

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

  {
    EMMPMFilterParameter::Pointer parameter = EMMPMFilterParameter::New();
    parameter->setHumanLabel("EMMPM Widget");
    parameter->setPropertyName("NumClasses");
    parameter->setCategory(FilterParameter::Parameter);
    parameter->setFilter(this);
    parameters.push_back(parameter);
  }

  parameters.push_back(SIMPL_NEW_BOOL_FP("Use 1-Based Values", UseOneBasedValues, FilterParameter::Parameter, EMMPMFilter));

  {
    QStringList linkedProps;
    linkedProps << "GradientBetaE";
    parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Use Gradient Penalty", UseGradientPenalty, FilterParameter::Parameter, EMMPMFilter, linkedProps));
  }
  parameters.push_back(SIMPL_NEW_CONSTRAINED_DOUBLE_FP("Beta E", GradientBetaE, FilterParameter::Parameter, EMMPMFilter));
  {
    QStringList linkedProps;
    linkedProps << "CurvatureBetaC" << "CurvatureRMax" << "CurvatureEMLoopDelay";
    parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Use Curvature Penalty", UseCurvaturePenalty, FilterParameter::Parameter, EMMPMFilter, linkedProps));
  }
  parameters.push_back(SIMPL_NEW_CONSTRAINED_DOUBLE_FP("Beta C", CurvatureBetaC, FilterParameter::Parameter, EMMPMFilter));
  parameters.push_back(SIMPL_NEW_CONSTRAINED_DOUBLE_FP("R Max", CurvatureRMax, FilterParameter::Parameter, EMMPMFilter));
  parameters.push_back(SIMPL_NEW_CONSTRAINED_INT_FP("EM Loop Delay", CurvatureEMLoopDelay, FilterParameter::Parameter, EMMPMFilter));

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt8, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Input Attribute Array", InputDataArrayPath, FilterParameter::RequiredArray, EMMPMFilter, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Output Attribute Array", OutputDataArrayPath, FilterParameter::CreatedArray, EMMPMFilter, req));
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
  setGradientBetaE(reader->readValue("GradientPenalty", getGradientBetaE()));
  setUseCurvaturePenalty(reader->readValue("UseCurvaturePenalty", getUseCurvaturePenalty()));
  setCurvatureBetaC(reader->readValue("CurvaturePenalty", getCurvatureBetaC()));
  setCurvatureRMax(reader->readValue("RMax", getCurvatureRMax()));
  setCurvatureEMLoopDelay(reader->readValue("EMLoopDelay", getCurvatureEMLoopDelay()));
  setOutputDataArrayPath(reader->readDataArrayPath("OutputDataArrayPath", getOutputDataArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMFilter::initialize()
{
  m_Data->initVariables();
  m_Data->dims = 1; // We operate on a single channel | single component "image".

  m_PreviousMu.resize(getNumClasses() * m_Data->dims);
  m_PreviousSigma.resize(getNumClasses() * m_Data->dims);

  // Store the values from the table as the previous
  DynamicTableData tableDataObj = getEMMPMTableData();
  std::vector<std::vector<double> > tableData = tableDataObj.getTableData();
  for(std::vector<float>::size_type i = 0; i < getNumClasses(); i++)
  {
    for(uint32_t d = 0; d < m_Data->dims; d++)
    {
      m_PreviousMu[i * m_Data->dims + d] = tableData[i][2];
      m_PreviousSigma[i * m_Data->dims + d] = tableData[i][3];
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMFilter::dataCheck()
{
  setErrorCondition(0);

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getInputDataArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 1); // We need a single component, gray scale image
  m_InputImagePtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(this, getInputDataArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_InputImagePtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_InputImage = m_InputImagePtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to Data from the DataArray<T> object */

  m_OutputImagePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(
      this, getOutputDataArrayPath(), 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_OutputImagePtr.lock().get())   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_OutputImage = m_OutputImagePtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to Data from the DataArray<T> object */

  if(getNumClasses() > 15)
  {
    setErrorCondition(-62000);
    QString ss = QObject::tr("The maximum number of classes is 15");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if(getNumClasses() < 2)
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
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMFilter::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }
  initialize();

  // This is the routine that sets up the EM/MPM to segment the image
  segment(getEmmpmInitType());

  if (m_UseOneBasedValues == true && m_OutputImagePtr.lock().get() != nullptr)
  {
    UInt8ArrayType::Pointer outputArray =  m_OutputImagePtr.lock();
    for (int i=0; i<outputArray->getNumberOfTuples(); i++)
    {
      uint8_t curVal = outputArray->getValue(i);
      uint8_t newVal = curVal + 1;
      outputArray->setValue(i, newVal);
    }
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMFilter::segment(EMMPM_InitializationType initType)
{
  // Copy all the variables from the filter into the EMmpm Data structure.
  m_Data->initType = initType;

  InitializationFunction::Pointer initFunction = BasicInitialization::New();

  // Set the initialization function based on the parameters
  switch(m_Data->initType)
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

  m_Data->classes = getNumClasses();
  m_Data->in_beta = getExchangeEnergy();
  m_Data->emIterations = getHistogramLoops();
  m_Data->mpmIterations = getSegmentationLoops();

  DynamicTableData tableDataObj = getEMMPMTableData();
  std::vector<std::vector<double> > tableData = tableDataObj.getTableData();
  for(int32_t i = 0; i < m_Data->classes; i++)
  {
    int32_t gray = 255 / (m_Data->classes - 1);
    // Generate a Gray Scale Color Table
    m_Data->colorTable[i] = qRgb(i * gray, i * gray, i * gray);
    // Hard code the minimum variance to 4.5; This could be a user option.
    m_Data->min_variance[i] = tableData[i][1];
    // Do we know what w_gamma is?
    m_Data->w_gamma[i] = tableData[i][0];
  }

  DataArrayPath dap = getInputDataArrayPath();
  AttributeMatrix::Pointer am = getDataContainerArray()->getAttributeMatrix(dap);
  QVector<size_t> tDims = am->getTupleDimensions();
  IDataArray::Pointer iDataArray = am->getAttributeArray(getInputDataArrayPath().getDataArrayName());
  QVector<size_t> cDims = iDataArray->getComponentDimensions();

  m_Data->columns = tDims[0];
  m_Data->rows = tDims[1];
  m_Data->inputImageChannels = cDims[0];

  m_Data->simulatedAnnealing = (char)(getUseSimulatedAnnealing());
  m_Data->useGradientPenalty = getUseGradientPenalty();
  m_Data->beta_e = getGradientBetaE();
  m_Data->useCurvaturePenalty = getUseCurvaturePenalty();
  m_Data->beta_c = getCurvatureBetaC();
  m_Data->r_max = getCurvatureRMax();
  m_Data->ccostLoopDelay = getCurvatureEMLoopDelay();

  // Assign our Data array allocated input and output images into the EMMPData class
  m_Data->inputImage = m_InputImage;
  m_Data->xt = m_OutputImage;

  // Allocate all the memory here
  m_Data->allocateDataStructureMemory();

  // If we are using the "Feedback" loop then we copy the previous Mu/Sigma values into the Mean/Variance
  // variables
  if(m_Data->initType == EMMPM_ManualInit)
  {
    for(int32_t i = 0; i < m_Data->classes; i++)
    {
      for(uint32_t d = 0; d < m_Data->dims; d++)
      {
        m_Data->mean[i * m_Data->dims + d] = m_PreviousMu[i * m_Data->dims + d];
        m_Data->variance[i * m_Data->dims + d] = m_PreviousSigma[i * m_Data->dims + d];
      }
    }
  }

  // Create a new StatsDelegate so the EMMPM algorith has somewhere to write its statistics
  StatsDelegate::Pointer statsDelegate = StatsDelegate::New();

  // Start the EM/MPM process going
  EMMPM::Pointer emmpm = EMMPM::New();

  emmpm->setData(m_Data);
  emmpm->setStatsDelegate(statsDelegate.get());
  emmpm->setInitializationFunction(initFunction);
  emmpm->setMessagePrefix(getMessagePrefix());

  // Connect up the Error/Warning/Progress object so the filter can report those things
  connect(emmpm.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)), this, SLOT(broadcastPipelineMessage(const PipelineMessage&)));

  emmpm->execute();

  // We manually set the pointers to nullptr so that the EMMPData class does not try to free the memory
  m_Data->inputImage = nullptr;
  m_Data->xt = nullptr;

  // Grab the Mu/Sigma values from the current finished segmented image and use those as inputs
  // into the initialization of the next Image to be Segmented
  m_PreviousMu.resize(getNumClasses() * m_Data->dims);
  m_PreviousSigma.resize(getNumClasses() * m_Data->dims);
  if(0)
  {
    std::cout << "--------------------------------------------------------------" << std::endl;
    for(std::vector<float>::size_type i = 0; i < getNumClasses(); i++)
    {
      std::cout << "Mu: " << m_Data->mean[i] << " Variance: " << sqrtf(m_Data->variance[i]) << std::endl;
      for(uint32_t d = 0; d < m_Data->dims; d++)
      {
        m_PreviousMu[i * m_Data->dims + d] = m_Data->mean[i * m_Data->dims + d];
        m_PreviousSigma[i * m_Data->dims + d] = m_Data->variance[i * m_Data->dims + d];
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
std::vector<float> EMMPMFilter::getPreviousMu()
{
  return m_PreviousMu;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMFilter::setPreviousMu(std::vector<float> prevMu)
{
  m_PreviousMu = prevMu;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<float> EMMPMFilter::getPreviousSigma()
{
  return m_PreviousSigma;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMFilter::setPreviousSigma(std::vector<float> prevSigma)
{
  m_PreviousSigma = prevSigma;
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
  vStream << EM_MPM::Version::Major() << "." << EM_MPM::Version::Minor() << "." << EM_MPM::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EMMPMFilter::getGroupName()
{
  return SIMPL::FilterGroups::ReconstructionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EMMPMFilter::getSubGroupName()
{
  return SIMPL::FilterSubGroups::SegmentationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EMMPMFilter::getHumanLabel()
{
  return "[EMMPM] EM/MPM";
}
