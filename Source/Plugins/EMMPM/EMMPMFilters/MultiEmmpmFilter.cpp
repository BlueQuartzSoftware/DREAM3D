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
#include <memory>

#include "MultiEmmpmFilter.h"

#include "EMMPM/EMMPMConstants.h"
#include "EMMPM/EMMPMLib/Common/EMMPM_Math.h"
#include "EMMPM/EMMPMLib/Common/EMTime.h"
#include "EMMPM/EMMPMLib/Common/StatsDelegate.h"
#include "EMMPM/EMMPMLib/Core/EMMPM.h"
#include "EMMPM/EMMPMLib/Core/EMMPMUtilities.h"
#include "EMMPM/EMMPMLib/Core/EMMPM_Data.h"
#include "EMMPM/EMMPMLib/Core/InitializationFunctions.h"
#include "EMMPM/EMMPMLib/EMMPMLib.h"

#include <QtCore/QTextStream>

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"

#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Messages/AbstractMessageHandler.h"
#include "SIMPLib/Messages/GenericProgressMessage.h"
#include "SIMPLib/Messages/GenericStatusMessage.h"
#include "SIMPLib/Messages/GenericErrorMessage.h"
#include "SIMPLib/Messages/GenericWarningMessage.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "EMMPM/EMMPMVersion.h"

/**
 * @brief This message handler is used by MultiEmmpmFilter instances to re-emit incoming generic messages from the
 * EMMPM observable object as its own filter messages.  It also prepends the current Array number to the prefix of
 * status messages received from MPMCalculation objects.
 */
class MultiEmmpmFilterMessageHandler : public AbstractMessageHandler
{
  public:
    explicit MultiEmmpmFilterMessageHandler(MultiEmmpmFilter* filter) : m_Filter(filter) {}

    /**
     * @brief Re-emits incoming GenericProgressMessages as FilterProgressMessages.
     */
    void processMessage(const GenericProgressMessage* msg) const override
    {
      emit m_Filter->notifyProgressMessage(msg->getProgressValue(), msg->getMessageText());
    }

    /**
     * @brief Re-emits incoming GenericStatusMessages as FilterStatusMessages.  Adds a prefix that
     * shows what array the filter is on.
     */
    void processMessage(const GenericStatusMessage* msg) const override
    {
      QString prefix = QObject::tr("(Array %2 of %3)").arg(m_Filter->m_CurrentArrayIndex + 1).arg(m_Filter->m_ArrayCount);
      emit m_Filter->notifyStatusMessageWithPrefix(prefix, msg->getMessageText());
    }

    /**
     * @brief Re-emits incoming GenericErrorMessages as FilterErrorMessages.
     */
    void processMessage(const GenericErrorMessage* msg) const override
    {
      emit m_Filter->setErrorCondition(msg->getCode(), msg->getMessageText());
    }

    /**
     * @brief Re-emits incoming GenericWarningMessages as FilterWarningMessages.
     */
    void processMessage(const GenericWarningMessage* msg) const override
    {
      emit m_Filter->setWarningCondition(msg->getCode(), msg->getMessageText());
    }

  private:
    MultiEmmpmFilter* m_Filter = nullptr;
};

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,

  DataArrayID30 = 30,
  DataArrayID31 = 31,
  DataArrayID32 = 32,
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiEmmpmFilter::MultiEmmpmFilter()
: m_OutputAttributeMatrixName("MultiArrayEMMPMOutput")
, m_OutputArrayPrefix("Segmented_")
, m_UsePreviousMuSigma(true)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiEmmpmFilter::~MultiEmmpmFilter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiEmmpmFilter::setupFilterParameters()
{
  EMMPMFilter::setupFilterParameters();
  FilterParameterVectorType parameters = getFilterParameters();

  parameters.push_back(SIMPL_NEW_BOOL_FP("Use Mu/Sigma from Previous Image as Initialization for Current Image", UsePreviousMuSigma, FilterParameter::Parameter, MultiEmmpmFilter));
  parameters.push_back(SIMPL_NEW_STRING_FP("Output Array Prefix", OutputArrayPrefix, FilterParameter::Parameter, MultiEmmpmFilter));

  for(qint32 i = 0; i < parameters.size(); i++)
  {
    FilterParameter::Pointer& p = parameters[i];
    if(p->getPropertyName().compare("InputDataArrayPath") == 0)
    {
      {
        MultiDataArraySelectionFilterParameter::RequirementType req =
            MultiDataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt8, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
        parameters[i] = SIMPL_NEW_MDA_SELECTION_FP("Input Attribute Arrays", InputDataArrayVector, FilterParameter::RequiredArray, MultiEmmpmFilter, req);
      }
    }
  }

  // Look for the OutputDataArrayPath and replace with our OutputAttributeMatrixName instead
  for(qint32 i = 0; i < parameters.size(); i++)
  {
    FilterParameter::Pointer& p = parameters[i];
    if(p->getPropertyName().compare("OutputDataArrayPath") == 0)
    {
      parameters[i] = SIMPL_NEW_STRING_FP("Output Cell Attribute Matrix", OutputAttributeMatrixName, FilterParameter::CreatedArray, MultiEmmpmFilter);
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
  setGradientBetaE(reader->readValue("GradientPenalty", getGradientBetaE()));
  setUseCurvaturePenalty(reader->readValue("UseCurvaturePenalty", getUseCurvaturePenalty()));
  setCurvatureBetaC(reader->readValue("CurvaturePenalty", getCurvatureBetaC()));
  setCurvatureRMax(reader->readValue("RMax", getCurvatureRMax()));
  setCurvatureEMLoopDelay(reader->readValue("EMLoopDelay", getCurvatureEMLoopDelay()));
  setOutputAttributeMatrixName(reader->readString("OutputAttributeMatrixName", getOutputAttributeMatrixName()));
  setUsePreviousMuSigma(reader->readValue("UsePreviousMuSigma", getUsePreviousMuSigma()));
  setOutputArrayPrefix(reader->readString("OutputArrayPrefix", getOutputArrayPrefix()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiEmmpmFilter::initialize()
{
  EMMPMFilter::initialize();

  m_ArrayCount = 0;
  m_CurrentArrayIndex = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiEmmpmFilter::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  if(!DataArrayPath::ValidateVector(getInputDataArrayVector()))
  {
    QString ss = QObject::tr("All Attribute Arrays must belong to the same Data Container and Attribute Matrix");
    setErrorCondition(-89004, ss);
  }

  std::vector<size_t> cDims(1, 1); // We need a single component, gray scale image

#if 0
  m_InputImagePtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(this, getInputDataArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (nullptr != m_InputImagePtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_InputImage = m_InputImagePtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to m_Data from the DataArray<T> object */
  if (getErrorCode() < 0) { return; }

  ImageGeom::Pointer image = getDataContainerArray()->getDataContainer(getInputDataArrayPath().getDataContainerName())->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if (getErrorCode() < 0 || nullptr == image.get()) { return; }

  m_OutputImagePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, getOutputDataArrayPath(), 0, cDims, "", DataArrayID31);
  if (nullptr != m_OutputImagePtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_OutputImage = m_OutputImagePtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to m_Data from the DataArray<T> object */

#endif

  if(getOutputArrayPrefix().isEmpty())
  {
    QString message = QObject::tr("Using a prefix (even a single alphanumeric value) is required so that the output Xdmf files can be written correctly");
    setErrorCondition(-89002, message);
  }

  if(getInputDataArrayVector().isEmpty())
  {
    QString message = QObject::tr("At least one Attribute Array must be selected");
    setErrorCondition(-89003, message);
    return;
  }

  DataArrayPath inputAMPath = DataArrayPath::GetAttributeMatrixPath(getInputDataArrayVector());

  AttributeMatrix::Pointer inAM = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, inputAMPath, -301);
  if(getErrorCode() < 0 || nullptr == inAM.get())
  {
    return;
  }

  // Now create our output attributeMatrix which will contain all of our segmented images
  std::vector<size_t> tDims = inAM->getTupleDimensions();
  AttributeMatrix::Pointer outAM = getDataContainerArray()
                                       ->getDataContainer(inputAMPath.getDataContainerName())
                                       ->createNonPrereqAttributeMatrix(this, getOutputAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell, AttributeMatrixID21);
  if(getErrorCode() < 0 || nullptr == outAM.get())
  {
    return;
  }

  // Get the list of checked array names from the input m_Data arrays list
  QList<QString> arrayNames = DataArrayPath::GetDataArrayNames(getInputDataArrayVector());

  for(int32_t i = 0; i < arrayNames.size(); i++)
  {
    QString daName = arrayNames.at(i);
    QString newName = getOutputArrayPrefix() + arrayNames.at(i);
    inputAMPath.setDataArrayName(daName);

    getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(this, inputAMPath, cDims);
    if(getErrorCode() < 0)
    {
      return;
    }

    outAM->createAndAddAttributeArray<UInt8ArrayType, AbstractFilter, uint8_t>(this, newName, 0, cDims, DataArrayID32);
  }

  // The EM/MPM Library has a hard coded MAX Classes of 16
  if(getNumClasses() > 15)
  {
    QString ss = QObject::tr("The maximum number of classes is 15");
    setErrorCondition(-89000, ss);
  }
  // It does not make any sense if we want anything less than 2 classes
  if(getNumClasses() < 2)
  {
    QString ss = QObject::tr("The minimum number of classes is 2");
    setErrorCondition(-89001, ss);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiEmmpmFilter::preflight()
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
void MultiEmmpmFilter::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }
  initialize();

  DataArrayPath inputAMPath = DataArrayPath::GetAttributeMatrixPath(getInputDataArrayVector());

  QList<QString> arrayNames = DataArrayPath::GetDataArrayNames(getInputDataArrayVector());
  QListIterator<QString> iter(arrayNames);

  m_ArrayCount = arrayNames.size();

  // This is the routine that sets up the EM/MPM to segment the image
  while(iter.hasNext())
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

    if(m_CurrentArrayIndex == 2 && getUsePreviousMuSigma())
    {
      setEmmpmInitType(EMMPM_ManualInit);
    }
    else
    {
      setEmmpmInitType(EMMPM_Basic);
    }

    EMMPMFilter::execute();
    if(getErrorCode() < 0)
    {
      break;
    }
    m_CurrentArrayIndex++;

    if(getCancel())
    {
      break;
    }
  }

  if(getErrorCode() < 0)
  {
    QString ss = QObject::tr("Error occurred running the EM/MPM algorithm");
    setErrorCondition(-60009, ss);
    return;
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiEmmpmFilter::handleEmmpmMessage(const AbstractMessage::Pointer& msg)
{
  MultiEmmpmFilterMessageHandler msgHandler(this);
  msg->visit(&msgHandler);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MultiEmmpmFilter::newFilterInstance(bool copyFilterParameters) const
{
  MultiEmmpmFilter::Pointer filter = MultiEmmpmFilter::New();
  if(copyFilterParameters)
  {
    filter->setInputDataArrayVector(getInputDataArrayVector());
    filter->setNumClasses(getNumClasses());
    filter->setExchangeEnergy(getExchangeEnergy());
    filter->setHistogramLoops(getHistogramLoops());
    filter->setSegmentationLoops(getSegmentationLoops());
    filter->setUseSimulatedAnnealing(getUseSimulatedAnnealing());
    filter->setUseGradientPenalty(getUseGradientPenalty());
    filter->setGradientBetaE(getGradientBetaE());
    filter->setUseCurvaturePenalty(getUseCurvaturePenalty());
    filter->setCurvatureBetaC(getCurvatureBetaC());
    filter->setCurvatureRMax(getCurvatureRMax());
    filter->setCurvatureEMLoopDelay(getCurvatureEMLoopDelay());
    filter->setOutputAttributeMatrixName(getOutputAttributeMatrixName());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiEmmpmFilter::getCompiledLibraryName() const
{
  return EMMPMConstants::EMMPMBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiEmmpmFilter::getBrandingString() const
{
  return "EMMPM";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiEmmpmFilter::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << EM_MPM::Version::Major() << "." << EM_MPM::Version::Minor() << "." << EM_MPM::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiEmmpmFilter::getGroupName() const
{
  return SIMPL::FilterGroups::ReconstructionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid MultiEmmpmFilter::getUuid() const
{
  return QUuid("{b2847755-dd39-5989-b624-98b1fdc9db5b}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiEmmpmFilter::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::SegmentationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MultiEmmpmFilter::getHumanLabel() const
{
  return "Segment Features (EM/MPM Multi-Array)";
}

// -----------------------------------------------------------------------------
MultiEmmpmFilter::Pointer MultiEmmpmFilter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<MultiEmmpmFilter> MultiEmmpmFilter::New()
{
  struct make_shared_enabler : public MultiEmmpmFilter
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString MultiEmmpmFilter::getNameOfClass() const
{
  return QString("MultiEmmpmFilter");
}

// -----------------------------------------------------------------------------
QString MultiEmmpmFilter::ClassName()
{
  return QString("MultiEmmpmFilter");
}

// -----------------------------------------------------------------------------
void MultiEmmpmFilter::setInputDataArrayVector(const QVector<DataArrayPath>& value)
{
  m_InputDataArrayVector = value;
}

// -----------------------------------------------------------------------------
QVector<DataArrayPath> MultiEmmpmFilter::getInputDataArrayVector() const
{
  return m_InputDataArrayVector;
}

// -----------------------------------------------------------------------------
void MultiEmmpmFilter::setOutputAttributeMatrixName(const QString& value)
{
  m_OutputAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString MultiEmmpmFilter::getOutputAttributeMatrixName() const
{
  return m_OutputAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void MultiEmmpmFilter::setOutputArrayPrefix(const QString& value)
{
  m_OutputArrayPrefix = value;
}

// -----------------------------------------------------------------------------
QString MultiEmmpmFilter::getOutputArrayPrefix() const
{
  return m_OutputArrayPrefix;
}

// -----------------------------------------------------------------------------
void MultiEmmpmFilter::setUsePreviousMuSigma(bool value)
{
  m_UsePreviousMuSigma = value;
}

// -----------------------------------------------------------------------------
bool MultiEmmpmFilter::getUsePreviousMuSigma() const
{
  return m_UsePreviousMuSigma;
}
