/*
The Original EM/MPM algorithm was developed by Mary L. Comer and is distributed
under the BSD License.
Copyright (c) <2010>, <Mary L. Comer>
All rights reserved.

[1] Comer, Mary L., and Delp, Edward J.,  ÒThe EM/MPM Algorithm for Segmentation
of Textured Images: Analysis and Further Experimental Results,Ó IEEE Transactions
on Image Processing, Vol. 9, No. 10, October 2000, pp. 1731-1744.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list
of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

Neither the name of <Mary L. Comer> nor the names of its contributors may be
used to endorse or promote products derived from this software without specific
prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "EMCalculation.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <QtCore/QTextStream>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Messages/AbstractMessageHandler.h"
#include "SIMPLib/Messages/GenericProgressMessage.h"
#include "SIMPLib/Messages/GenericStatusMessage.h"
#include "SIMPLib/Messages/GenericErrorMessage.h"
#include "SIMPLib/Messages/GenericWarningMessage.h"

#include "EMMPMLib/Common/EMMPM_Math.h"
#include "EMMPMLib/Common/MSVCDefines.h"
#include "EMMPMLib/Core/EMMPM_Constants.h"
#include "EMMPMLib/Core/EMMPM_Data.h"
#include "EMMPMLib/Core/InitializationFunctions.h"
#include "EMMPMLib/EMMPMLib.h"

#include "EMMPMLib/Core/EMMPMUtilities.h"
#include "EMMPMLib/Core/MPMCalculation.h"
#include "EMMPMLib/Core/MorphFilt.h"

#if EMMPM_USE_PARALLEL_ALGORITHMS
#include <tbb/task_scheduler_init.h>
#endif

/**
 * @brief This message handler is used by EMCalculation instances to re-emit incoming generic messages from the
 * MPMCalculation observable object as its own generic messages.  It also prepends the EM Loop number to the
 * message text of status messages received from MPMCalculation objects.
 */
class EMCalculationMessageHandler : public AbstractMessageHandler
{
  public:
    explicit EMCalculationMessageHandler(EMCalculation* calcObj) : m_CalculationObject(calcObj) {}

    /**
     * @brief Re-emits incoming GenericProgressMessages as FilterProgressMessages.
     */
    void processMessage(const GenericProgressMessage* msg) const override
    {
      emit m_CalculationObject->notifyProgressMessage(msg->getProgressValue(), msg->getMessageText());
    }

    /**
     * @brief Re-emits incoming GenericStatusMessages as FilterStatusMessages.  Appends a message to
     * the end of the message text that shows what EM Loop number the filter is on.
     */
    void processMessage(const GenericStatusMessage* msg) const override
    {
      EMMPM_Data* data = m_CalculationObject->m_Data.get();
      QString prefix = QObject::tr("EM Loop %1").arg(data->currentEMLoop);
      emit m_CalculationObject->notifyStatusMessageWithPrefix(prefix, msg->getMessageText());
    }

    /**
     * @brief Handle incoming GenericErrorMessages
     */
    void processMessage(const GenericErrorMessage* msg) const override
    {
      emit m_CalculationObject->setErrorCondition(msg->getCode(), msg->getMessageText());
    }

    /**
     * @brief Handle incoming GenericWarningMessages
     */
    void processMessage(const GenericWarningMessage* msg) const override
    {
      emit m_CalculationObject->setWarningCondition(msg->getCode(), msg->getMessageText());
    }

  private:
    EMCalculation* m_CalculationObject = nullptr;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMCalculation::EMCalculation()
: m_ErrorCode(0)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMCalculation::~EMCalculation() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMCalculation::execute()
{
#if EMMPM_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
//  int threads = init.default_num_threads();
//   std::cout << "TBB Thread Count: " << threads << std::endl;
#endif
  EMMPM_Data* data = m_Data.get();
  int k;
  int emiter = data->emIterations;
  std::vector<real_t> simAnnealKappas;
  bool stop = false;

  float totalLoops = (float)(data->emIterations * data->mpmIterations + data->mpmIterations);
  float currentLoopCount = 0.0;

  size_t ccostLoopDelay = data->ccostLoopDelay;

  char msgbuff[256];
  memset(msgbuff, 0, 256);
  data->currentEMLoop = 0;
  data->currentMPMLoop = 0;

  // Copy in the users Beta Value
  data->workingKappa = 1.0;

  // If we are using Sim Anneal then create a ramped beta
  if(data->simulatedAnnealing != 0 && data->emIterations > 1)
  {
    simAnnealKappas.resize(data->emIterations);
    for(int i = 0; i < data->emIterations; ++i)
    {
      simAnnealKappas[i] = data->workingKappa + pow(i / (data->emIterations - 1.0), 8) * (10.0 * data->workingKappa - data->workingKappa);
    }
    data->workingKappa = simAnnealKappas[0];
  }

  /* Perform a single MPM Loop to get things initialized. This is Jeff Simmons'
   * idea and is a good idea.  */
  k = 0; // Simulate first loop of EM by setting k=0;
  // Possibly update the beta value due to simulated Annealing
  if(data->simulatedAnnealing != 0 && data->emIterations > 1)
  {
    data->workingKappa = simAnnealKappas[k];
  }

  data->calculateBetaMatrix(data->in_beta);

  MorphFilter::Pointer morphFilt = MorphFilter::New();

  /* After curveLoopDelay iterations, begin calculating curvature costs */
  if(k >= ccostLoopDelay && (data->useCurvaturePenalty != 0))
  {
    notifyStatusMessage("Performing Morphological Filter on input data");
    morphFilt->multiSE(data);
  }

  // Zero out the Mean, Variance and N values for both the current and previous
  EMMPMUtilities::ZeroMeanVariance(data->classes, data->dims, data->prev_mu, data->prev_variance, data->N);
  notifyStatusMessage("Performing Initial MPM Loop");

  /* Perform initial MPM - (Estimation) */
  MPMCalculation::Pointer acvmpm = MPMCalculation::New();
  acvmpm->setData(getData());
  acvmpm->setStatsDelegate(getStatsDelegate());

  // Connect up the Error/Warning/Progress object so the filter can report those things
  connect(acvmpm.get(), &MPMCalculation::messageGenerated, [=] (AbstractMessage::Pointer msg) {
    EMCalculationMessageHandler msgHandler(this);
    msg->visit(&msgHandler);
  });

  acvmpm->execute();

  QString ss;
  QTextStream msgOut(&ss);
  /* -----------------------------------------------------------
  *                Perform EM Loops
  * ------------------------------------------------------------ */
  for(k = 0; k < emiter; k++)
  {

    ss.clear();
    msgOut << "EM Loop " << data->currentEMLoop << " - Converting Xt Data to Output Image..";
    notifyStatusMessage(ss);

    /* Send back the Progress Stats and the segmented image. If we never get into this loop because
    * emiter == 0 then we will still send back the stats just after the end of the EM Loops */
    EMMPMUtilities::ConvertXtToOutputImage(getData());
    if(m_StatsDelegate != nullptr)
    {
      m_StatsDelegate->reportProgress(getData());
    }

    /* Check to see if we are canceled */
    if(data->cancel != 0)
    {
      data->progress = 100.0;
      break;
    }

    data->inside_em_loop = 1;
    data->currentEMLoop = k + 1;
    data->currentMPMLoop = 0;
    currentLoopCount = (float)((data->mpmIterations * data->currentEMLoop) + data->currentMPMLoop);

    data->progress = currentLoopCount / totalLoops * 100.0;

    /* Check if the "Error" is less than a user defined Tolerance and if it is then
     * bail out of the loop now.
     */
    stop = EMMPMUtilities::isStoppingConditionLessThanTolerance(getData());
    if(stop)
    {
      break;
    }

    ss.clear();
    msgOut << "EM Loop " << data->currentEMLoop << " - Copying Current Mean & Variance Values ...";
    notifyStatusMessage(ss);

    /* Copy the current Mean and Variance Values to the "prev_*" variables */
    EMMPMUtilities::copyCurrentMeanVarianceValues(getData());

    ss.clear();
    msgOut << "EM Loop " << data->currentEMLoop << " - Zeroing Mean & Variance Values...";
    notifyStatusMessage(ss);

    /* Reset model parameters to zero */
    EMMPMUtilities::ZeroMeanVariance(data->classes, data->dims, data->mean, data->variance, data->N);

    ss.clear();
    msgOut << "EM Loop " << data->currentEMLoop << " - Updating Mean & Variance Values...";
    notifyStatusMessage(ss);
    /* Update Means and Variances */
    EMMPMUtilities::UpdateMeansAndVariances(getData());

#if 0
    /* Monitor estimates of mean and variance */
    if (emiter < 10 || (k + 1) % (emiter / 10) == 0)
    {
      EMMPM_MonitorMeansAndVariances(data, callbacks);
    }
#endif

#if 1
    ss.clear();
    msgOut << "EM Loop " << data->currentEMLoop << " - Removing Zero Probability Classes ...";
    notifyStatusMessage(ss);
    /* Eliminate any classes that have zero probability */
    EMMPMUtilities::RemoveZeroProbClasses(getData());
#endif

    // Possibly update the beta value due to simulated Annealing
    if(data->simulatedAnnealing != 0 && data->emIterations > 1)
    {
      data->workingKappa = simAnnealKappas[k];
      data->calculateBetaMatrix(data->in_beta);
    }

    /* After curveLoopDelay iterations, begin calculating curvature costs */
    if(k >= ccostLoopDelay && (data->useCurvaturePenalty != 0))
    {
      ss.clear();
      msgOut << "EM Loop " << data->currentEMLoop << " - Performing Morphological filtering ...";
      notifyStatusMessage(ss);
      morphFilt->multiSE(data);
    }

    /* Perform MPM - (Estimation) */
    ss.clear();
    acvmpm->execute();
  } /* EM Loop End */

  msgOut << "EM Loop " << data->currentEMLoop << " - Converting Xt Data to Output final Array..";
  notifyStatusMessage(ss);
  EMMPMUtilities::ConvertXtToOutputImage(getData());

  data->inside_em_loop = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMCalculation::getHumanLabel() const
{
  return "EMCalculation";
}

// -----------------------------------------------------------------------------
EMCalculation::Pointer EMCalculation::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
EMCalculation::Pointer EMCalculation::New()
{
  Pointer sharedPtr(new(EMCalculation));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString EMCalculation::getNameOfClass() const
{
  return QString("EMCalculation");
}

// -----------------------------------------------------------------------------
QString EMCalculation::ClassName()
{
  return QString("EMCalculation");
}

// -----------------------------------------------------------------------------
void EMCalculation::setData(const EMMPM_Data::Pointer& value)
{
  m_Data = value;
}

// -----------------------------------------------------------------------------
EMMPM_Data::Pointer EMCalculation::getData() const
{
  return m_Data;
}

// -----------------------------------------------------------------------------
void EMCalculation::setErrorCode(int value)
{
  m_ErrorCode = value;
}

// -----------------------------------------------------------------------------
int EMCalculation::getErrorCode() const
{
  return m_ErrorCode;
}

// -----------------------------------------------------------------------------
void EMCalculation::setStatsDelegate(StatsDelegate* value)
{
  m_StatsDelegate = value;
}

// -----------------------------------------------------------------------------
StatsDelegate* EMCalculation::getStatsDelegate() const
{
  return m_StatsDelegate;
}
