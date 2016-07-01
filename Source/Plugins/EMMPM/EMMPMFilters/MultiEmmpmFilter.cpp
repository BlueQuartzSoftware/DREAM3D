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
#include "MultiEmmpmFilter.h"

#include "EMMPM/EMMPMConstants.h"
#include "EMMPM/EMMPMLib/EMMPMLib.h"
#include "EMMPM/EMMPMLib/Common/EMTime.h"
#include "EMMPM/EMMPMLib/Common/EMMPM_Math.h"
#include "EMMPM/EMMPMLib/Core/EMMPM_Data.h"
#include "EMMPM/EMMPMLib/Core/EMMPM.h"
#include "EMMPM/EMMPMLib/Common/StatsDelegate.h"
#include "EMMPM/EMMPMLib/Core/InitializationFunctions.h"
#include "EMMPM/EMMPMLib/Core/EMMPMUtilities.h"

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

// Include the MOC generated file for this class
#include "moc_MultiEmmpmFilter.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiEmmpmFilter::MultiEmmpmFilter() :
  EMMPMFilter(),
  m_OutputAttributeMatrixName("MultiArrayEMMPMOutput"),
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

  parameters.push_back(BooleanFilterParameter::New("Use Mu/Sigma from Previous Image as Initialization for Current Image", "UsePreviousMuSigma", getUsePreviousMuSigma(), FilterParameter::Parameter));
  parameters.push_back(StringFilterParameter::New("Output Array Prefix", "OutputArrayPrefix", getOutputArrayPrefix(), FilterParameter::Parameter));


  for ( qint32 i = 0; i < parameters.size(); i++ )
  {
    FilterParameter::Pointer& p = parameters[i];
    if ( p->getPropertyName().compare("InputDataArrayPath") == 0 )
    {
      {
        MultiDataArraySelectionFilterParameter::RequirementType req = MultiDataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt8, 1, SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
        parameters[i] = MultiDataArraySelectionFilterParameter::New("Input Attribute Arrays", "InputDataArrayVector", getInputDataArrayVector(), FilterParameter::RequiredArray, req);
      }
    }
  }

  // Look for the OutputDataArrayPath and replace with our OutputAttributeMatrixName instead
  for ( qint32 i = 0; i < parameters.size(); i++ )
  {
    FilterParameter::Pointer& p = parameters[i];
    if ( p->getPropertyName().compare("OutputDataArrayPath") == 0 )
    {
      parameters[i] = StringFilterParameter::New("Output Cell Attribute Matrix", "OutputAttributeMatrixName", getOutputAttributeMatrixName(), FilterParameter::CreatedArray);
    }
  }

  // Set the new parameters back into the class
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiEmmpmFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputDataArrayVector(reader->readDataArrayPathVector("InputDataArrayVector", getInputDataArrayVector()));
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
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(InputDataArrayVector)
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
  SIMPL_FILTER_WRITE_PARAMETER(OutputAttributeMatrixName)
  SIMPL_FILTER_WRITE_PARAMETER(UsePreviousMuSigma)
  SIMPL_FILTER_WRITE_PARAMETER(OutputArrayPrefix)

  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiEmmpmFilter::initialize()
{
  m_PreviousMu.clear();
  m_PreviousSigma.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiEmmpmFilter::dataCheck()
{
  setErrorCondition(0);

  if (DataArrayPath::ValidateVector(getInputDataArrayVector()) == false)
  {
    setErrorCondition(-62000);
    QString ss = QObject::tr("All Attribute Arrays must belong to the same Data Container and Attribute Matrix");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

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

  if (getOutputArrayPrefix().isEmpty())
  {
    setErrorCondition(-62002);
    QString message = QObject::tr("Using a prefix (even a single alphanumeric value) is required so that the output Xdmf files can be written correctly");
    notifyErrorMessage(getHumanLabel(), message, getErrorCondition());
  }

  if (getInputDataArrayVector().isEmpty())
  {
    setErrorCondition(-62003);
    QString message = QObject::tr("At least one Attribute Array must be selected");
    notifyErrorMessage(getHumanLabel(), message, getErrorCondition());
    return;
  }

  DataArrayPath inputAMPath = DataArrayPath::GetAttributeMatrixPath(getInputDataArrayVector());

  AttributeMatrix::Pointer inAM = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, inputAMPath, -301);
  if(getErrorCondition() < 0 || NULL == inAM.get()) { return; }

  // Now create our output attributeMatrix which will contain all of our segmented images
  QVector<size_t> tDims = inAM->getTupleDimensions();
  AttributeMatrix::Pointer outAM = getDataContainerArray()->getDataContainer(inputAMPath.getDataContainerName())->createNonPrereqAttributeMatrix<AbstractFilter>(this, getOutputAttributeMatrixName(), tDims, SIMPL::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0 || NULL == outAM.get()) { return; }

  // Get the list of checked array names from the input data arrays list
  QList<QString> arrayNames = DataArrayPath::GetDataArrayNames(getInputDataArrayVector());

  for (int32_t i = 0; i < arrayNames.size(); i++ )
  {
    QString daName = arrayNames.at(i);
    QString newName = getOutputArrayPrefix() + arrayNames.at(i);
    inputAMPath.setDataArrayName(daName);

    getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(this, inputAMPath, cDims);
    if (getErrorCondition() < 0) { return; }

    outAM->createAndAddAttributeArray<UInt8ArrayType, AbstractFilter, uint8_t>(this, newName, 0, cDims);
  }

  // The EM/MPM Library has a hard coded MAX Classes of 16
  if (getNumClasses() > 15)
  {
    setErrorCondition(-62000);
    QString ss = QObject::tr("The maximum number of classes is 15");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  // It does not make any sense if we want anything less than 2 classes
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
  setErrorCondition(0);
  dataCheck();
  if (getErrorCondition() < 0) { return; }
  initialize();


  DataArrayPath inputAMPath = DataArrayPath::GetAttributeMatrixPath(getInputDataArrayVector());

  QList<QString> arrayNames = DataArrayPath::GetDataArrayNames(getInputDataArrayVector());
  QListIterator<QString> iter(arrayNames);

  QString msgPrefix = getMessagePrefix();
  int32_t i = 1;
  // This is the routine that sets up the EM/MPM to segment the image
  while (iter.hasNext())
  {
    DataArrayPath arrayPath = inputAMPath;
    QString name = iter.next();

    arrayPath.setDataArrayName(name);
    setInputDataArrayPath(arrayPath);

    // Change the output AttributeMatrix
    arrayPath.setAttributeMatrixName(getOutputAttributeMatrixName());
    QString outName = getOutputArrayPrefix() + arrayPath.getDataArrayName();
    arrayPath.setDataArrayName(outName);
    // Remove the array if it already exists ; this would be very strange but check for it anyway
    getDataContainerArray()->getAttributeMatrix(arrayPath)->removeAttributeArray(outName);
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
    if (getErrorCondition() < 0) { break; }
    i++;

    if (getCancel()) { break; }
  }

  if (getErrorCondition() < 0)
  {
    QString ss = QObject::tr("Error occurred running the EM/MPM algorithm");
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
AbstractFilter::Pointer MultiEmmpmFilter::newFilterInstance(bool copyFilterParameters)
{
  MultiEmmpmFilter::Pointer filter = MultiEmmpmFilter::New();
  if (true == copyFilterParameters)
  {
    SIMPL_COPY_INSTANCEVAR(InputDataArrayVector)
    SIMPL_COPY_INSTANCEVAR(NumClasses)
    SIMPL_COPY_INSTANCEVAR(ExchangeEnergy)
    SIMPL_COPY_INSTANCEVAR(HistogramLoops)
    SIMPL_COPY_INSTANCEVAR(SegmentationLoops)
    SIMPL_COPY_INSTANCEVAR(UseSimulatedAnnealing)
    SIMPL_COPY_INSTANCEVAR(UseGradientPenalty)
    SIMPL_COPY_INSTANCEVAR(GradientPenalty)
    SIMPL_COPY_INSTANCEVAR(UseCurvaturePenalty)
    SIMPL_COPY_INSTANCEVAR(CurvaturePenalty)
    SIMPL_COPY_INSTANCEVAR(RMax)
    SIMPL_COPY_INSTANCEVAR(EMLoopDelay)
    SIMPL_COPY_INSTANCEVAR(OutputAttributeMatrixName)
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiEmmpmFilter::getCompiledLibraryName()
{
  return EMMPMConstants::EMMPMBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiEmmpmFilter::getBrandingString()
{
  return "EMMPM";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiEmmpmFilter::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiEmmpmFilter::getGroupName()
{ return SIMPL::FilterGroups::ReconstructionFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiEmmpmFilter::getSubGroupName()
{ return SIMPL::FilterSubGroups::SegmentationFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MultiEmmpmFilter::getHumanLabel()
{ return "Multi-Array EM/MPM"; }
