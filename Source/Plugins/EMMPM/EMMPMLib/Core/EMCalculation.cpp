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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "EMCalculation.h"

#include "EMMPMLib/EMMPMLib.h"
#include "EMMPMLib/Common/MSVCDefines.h"
#include "EMMPMLib/Common/EMMPM_Math.h"
#include "EMMPMLib/Core/EMMPM_Constants.h"
#include "EMMPMLib/Core/EMMPM_Data.h"
#include "EMMPMLib/Core/InitializationFunctions.h"

#include "EMMPMLib/Core/EMMPMUtilities.h"
#include "EMMPMLib/Core/MorphFilt.h"
#include "EMMPMLib/Core/MPMCalculation.h"

#if defined (EMMPMLib_USE_PARALLEL_ALGORITHMS)
#include <tbb/task_scheduler_init.h>
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMCalculation::EMCalculation() :
m_StatsDelegate(NULL)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMCalculation::~EMCalculation()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMCalculation::execute()
{
#if defined (EMMPMLib_USE_PARALLEL_ALGORITHMS)
    tbb::task_scheduler_init init;
  //  int threads = init.default_num_threads();
 //   std::cout << "TBB Thread Count: " << threads << std::endl;
#endif
  EMMPM_Data* data = m_Data.get();
  int k;
  int emiter = data->emIterations;
  real_t* simAnnealKappas = NULL;
  bool stop = false;

  float totalLoops = (float)(data->emIterations * data->mpmIterations + data->mpmIterations);
  float currentLoopCount = 0.0;

  size_t ccostLoopDelay = data->ccostLoopDelay;

  char msgbuff[256];
  memset(msgbuff, 0, 256);
  data->currentEMLoop = 0;
  data->currentMPMLoop = 0;

  //Copy in the users Beta Value
  data->workingKappa = 1.0;

  // If we are using Sim Anneal then create a ramped beta
  if (data->simulatedAnnealing != 0 && data->emIterations > 1)
  {
    simAnnealKappas=(real_t*)(malloc(sizeof(real_t)*data->emIterations));
    for (int i = 0; i < data->emIterations; ++i)
    {
      simAnnealKappas[i] = data->workingKappa + pow(i/(data->emIterations-1.0), 8) * (10.0*data->workingKappa - data->workingKappa);
    }
    data->workingKappa = simAnnealKappas[0];
  }


  /* Perform a single MPM Loop to get things initialized. This is Jeff Simmons'
   * idea and is a good idea.  */
  k = 0; // Simulate first loop of EM by setting k=0;
  // Possibly update the beta value due to simulated Annealing
  if (data->simulatedAnnealing != 0 && data->emIterations > 1)
  {
    data->workingKappa = simAnnealKappas[k];
  }

  data->calculateBetaMatrix(data->in_beta);


  MorphFilter::Pointer morphFilt = MorphFilter::New();

  /* After curveLoopDelay iterations, begin calculating curvature costs */
  if (k >= ccostLoopDelay && data->useCurvaturePenalty)
  {
    notify("Performing Morphological Filter on input data", 0, UpdateProgressMessage);
    morphFilt->multiSE(data);
  }

  // Zero out the Mean, Variance and N values for both the current and previous
  EMMPMUtilities::ZeroMeanVariance(data->classes, data->dims, data->prev_mu, data->prev_variance, data->N);

  notify("Performing Initial MPM Loop", 0, UpdateProgressMessage);

  /* Perform initial MPM - (Estimation) */
  MPMCalculation::Pointer acvmpm = MPMCalculation::New();
  acvmpm->setData(getData());
  acvmpm->setObservers(getObservers());
  acvmpm->setStatsDelegate(getStatsDelegate());
  acvmpm->execute();


  /* -----------------------------------------------------------
  *                Perform EM Loops
  * ------------------------------------------------------------ */
  for (k = 0; k < emiter; k++)
  {

    /* Send back the Progress Stats and the segmented image. If we never get into this loop because
    * emiter == 0 then we will still send back the stats just after the end of the EM Loops */
    EMMPMUtilities::ConvertXtToOutputImage(getData());
    if (m_StatsDelegate != NULL)
    {
      m_StatsDelegate->reportProgress(getData());
    }

    snprintf(msgbuff, 256, "EM Loop %d", data->currentEMLoop);
    notify(msgbuff, 0, UpdateProgressMessage);

    /* Check to see if we are canceled */
    if (data->cancel) { data->progress = 100.0; break; }

    data->inside_em_loop = 1;
    data->currentEMLoop = k+1;
    data->currentMPMLoop = 0;
    currentLoopCount = (float)( (data->mpmIterations * data->currentEMLoop) + data->currentMPMLoop);

    data->progress = currentLoopCount/totalLoops * 100.0;

    /* Check if the "Error" is less than a user defined Tolerance and if it is then
     * bail out of the loop now.
     */
    stop = EMMPMUtilities::isStoppingConditionLessThanTolerance(getData());
    if (stop == true)
    {
        break;
    }

    /* Copy the current Mean and Variance Values to the "prev_*" variables */
    EMMPMUtilities::copyCurrentMeanVarianceValues(getData());

    /* Reset model parameters to zero */
    EMMPMUtilities::ZeroMeanVariance(data->classes, data->dims, data->mean, data->variance, data->N);

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
    /* Eliminate any classes that have zero probability */
    EMMPMUtilities::RemoveZeroProbClasses(getData());
#endif

    // Possibly update the beta value due to simulated Annealing
    if (data->simulatedAnnealing != 0 && data->emIterations > 1)
    {
      data->workingKappa = simAnnealKappas[k];
      data->calculateBetaMatrix(data->in_beta);
    }


    /* After curveLoopDelay iterations, begin calculating curvature costs */
    if (k >= ccostLoopDelay && data->useCurvaturePenalty)
    {
      morphFilt->multiSE(data);
    }

    /* Perform MPM - (Estimation) */
    acvmpm->execute();
  } /* EM Loop End */

  EMMPMUtilities::ConvertXtToOutputImage(getData());

  data->inside_em_loop = 0;
  free(simAnnealKappas);
}
