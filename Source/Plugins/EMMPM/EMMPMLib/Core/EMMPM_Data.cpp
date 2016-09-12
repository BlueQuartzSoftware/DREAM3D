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

#include "EMMPM_Data.h"


#define EMMPM_FREE_POINTER(ptr)\
  if (nullptr != (ptr)) { free(ptr); (ptr) = nullptr;}




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPM_Data::EMMPM_Data()
{
  initVariables();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPM_Data::~EMMPM_Data()
{

  EMMPM_FREE_POINTER(this->input_file_name)
  EMMPM_FREE_POINTER(this->output_file_name)

  if(this->inputImage != nullptr)
  {
    free(this->inputImage);
    this->inputImage = nullptr;
  }
  if(this->outputImage != nullptr)
  {
    free(this->outputImage);
    this->outputImage = nullptr;
  }

  EMMPM_FREE_POINTER(this->y)
  EMMPM_FREE_POINTER(this->xt)
  EMMPM_FREE_POINTER(this->mean)
  EMMPM_FREE_POINTER(this->variance)
  EMMPM_FREE_POINTER(this->prev_mu)
  EMMPM_FREE_POINTER(this->prev_variance)
  EMMPM_FREE_POINTER(this->probs)
  EMMPM_FREE_POINTER(this->ccost)
  EMMPM_FREE_POINTER(this->ns)
  EMMPM_FREE_POINTER(this->ew)
  EMMPM_FREE_POINTER(this->sw)
  EMMPM_FREE_POINTER(this->nw)
  EMMPM_FREE_POINTER(this->histograms);
  EMMPM_FREE_POINTER(this->couplingBeta);
//  EMMPM_FREE_POINTER(this->rngVars);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EMMPM_Data::allocateDataStructureMemory()
{
  if(nullptr == this->y)
  {
    this->y = (unsigned char*)malloc(this->columns * this->rows * this->dims * sizeof(unsigned char));
  }
  if(nullptr == this->y) { return -1; }

  if(nullptr == this->xt)
  {
    this->xt = (unsigned char*)malloc(this->columns * this->rows * sizeof(unsigned char));
  }
  if(nullptr == this->xt) { return -1; }

  if(nullptr == this->mean)
  {
    this->mean = (real_t*)malloc(this->classes * this->dims * sizeof(real_t));
  }
  if(nullptr == this->mean) { return -1; }

  if(nullptr == this->prev_mu)
  {
    this->prev_mu = (real_t*)malloc(this->classes * this->dims * sizeof(real_t));
  }
  if(nullptr == this->prev_mu) { return -1; }


  if(nullptr == this->variance)
  {
    this->variance = (real_t*)malloc(this->classes * this->dims * sizeof(real_t));
  }
  if(nullptr == this->variance) { return -1; }

  if(nullptr == this->prev_variance)
  {
    this->prev_variance = (real_t*)malloc(this->classes * this->dims * sizeof(real_t));
  }
  if(nullptr == this->prev_variance) { return -1; }

  if(nullptr == this->probs)
  {
    this->probs = (real_t*)malloc(this->classes * this->columns * this->rows * sizeof(real_t));
  }
  if(nullptr == this->probs) { return -1; }

  if(nullptr == this->histograms)
  {
    this->histograms = (real_t*)malloc(this->classes * this->dims * 256 * sizeof(real_t));
  }
  if(nullptr == this->histograms) { return -1; }

  if (nullptr == this->couplingBeta)
  {
    unsigned int cSize = this->classes + 1;
    size_t couplingElements = cSize * cSize;
    this->couplingBeta = static_cast<real_t*>(malloc(sizeof(real_t) * couplingElements));
  }
  if (nullptr == this->couplingBeta) { return -1; }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_Data::allocateOutputImage()
{
  if(nullptr != this->outputImage)
  {
    free(this->outputImage);
    this->outputImage = nullptr;
  }

  this->outputImage = reinterpret_cast<unsigned char*>(malloc(this->columns * this->rows * this->dims));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_Data::initVariables()
{

  int c;

  this->emIterations = 0;
  this->mpmIterations = 0;
  this->in_beta = 0.0;
  this->classes = 0;
  this->rows = 0;
  this->columns = 0;
  this->dims = 1;
  this->initType = EMMPM_Basic;
  this->couplingBeta = nullptr;
  for (c = 0; c < EMMPM_MAX_CLASSES; c++)
  {
    this->initCoords[c][0] = 0;
    this->initCoords[c][1] = 1;
    this->initCoords[c][2] = 2;
    this->initCoords[c][3] = 3;
    this->colorTable[c] = 0;
    this->N[c] = 0.0;
    this->w_gamma[c] = 1.0;
    this->min_variance[c] = 1.0;
  }
  this->verbose = 0;
  this->cancel = 0;

  this->mean = nullptr;
  this->variance = nullptr;
  this->prev_mu = nullptr;
  this->prev_variance = nullptr;
  this->probs = nullptr;
  this->ccost = nullptr;

  this->input_file_name = nullptr;
  this->inputImage = nullptr;
  this->inputImageChannels = 0;
  this->output_file_name = nullptr;
  this->outputImage = nullptr;

  this->y = nullptr;
  this->xt = nullptr;
  this->workingKappa = 0.0;

  this->currentEMLoop = 0;
  this->currentMPMLoop = 0;
  this->progress = 0.0;
  this->userData = nullptr;
  this->inside_em_loop = 0;
  this->inside_mpm_loop = 0;

  this->useCurvaturePenalty = 0;
  this->useGradientPenalty = 0;
  this->ccostLoopDelay = 1;
  this->beta_e = 0.0;
  this->beta_c = 0.0;
  this->r_max = 0.0;
  this->ns = nullptr;
  this->ew = nullptr;
  this->sw = nullptr;
  this->nw = nullptr;

  this->useStoppingThreshold = 0;
  this->stoppingThreshold = 0.0;
  this->currentMSE = 0.0;
  this->histograms = nullptr;

  //this->rngVars = nullptr;

  this->tiffResSet = 0;
  this->xResolution = 0;
  this->yResolution = 0;
  this->resolutionUnits = 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_Data::calculateBetaMatrix(double default_beta)
{
  if (nullptr == couplingBeta)
  {
    return;
  }

  // Recalculate the Class Coupling Matrix
  int ij = 0;
  for (int i = 0; i < (classes + 1); ++i)
  {
    for (int j = 0; j < (classes + 1); ++j)
    {
      ij = ((classes + 1) * i) + j;
      if(j == classes) { couplingBeta[ij] = 0.0; }
      else if(i == j) { couplingBeta[ij] = 0.0; }
      else if(i == classes) { couplingBeta[ij] = 0.0; }
      else { couplingBeta[ij] = default_beta; }
    }
  }
  // Update the Coupling Matrix with user defined entries
  for (std::vector<CoupleType>::iterator iter = coupleEntries.begin(); iter != coupleEntries.end(); ++iter)
  {
    ij = ((classes + 1) * ((*iter).label_1)) + (*iter).label_2;
    couplingBeta[ij] = (*iter).beta;
    ij = ((classes + 1) * ((*iter).label_2)) + (*iter).label_1;
    couplingBeta[ij] = (*iter).beta;
  }
#if 0
  std::cout << "***\t";
  for (int i = 0; i < (classes + 1); ++i)
  {
    std::cout   << i << "\t";
  }
  std::cout << std::endl;

  for (int i = 0; i < (classes + 1); ++i)
  {
    std::cout << i << "\t";
    for (int j = 0; j < (classes + 1); ++j)
    {
      ij = ((classes + 1) * i) + j;
      std::cout << couplingBeta[ij] << "\t";
    }
    std::cout << std::endl;
  }
#endif
}
