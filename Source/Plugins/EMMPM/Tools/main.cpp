/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#include <iostream>
#include <ostream>
#include <fstream>
#include <string>
#include <sstream>


#include "EMMPMLib/EMMPMLib.h"
#include "EMMPMLib/Common/MSVCDefines.h"
#include "EMMPMLib/Common/EMTime.h"
#include "EMMPMLib/Common/EMMPM_Math.h"
#include "EMMPMLib/Common/Observer.h"
#include "EMMPMLib/Core/EMMPM_Data.h"
#include "EMMPMLib/Core/EMMPM.h"
#include "EMMPMLib/Common/EMMPMInputParser.h"
#include "EMMPMLib/Common/StatsDelegate.h"
#include "EMMPMLib/Core/InitializationFunctions.h"
#include "EMMPMLib/Core/EMMPMUtilities.h"

#if defined (EMMPM_USE_PARALLEL_ALGORITHMS)
#include <tbb/task_scheduler_init.h>
#endif

class CLIStatsDelegate : public StatsDelegate
{
  public:
    CLIStatsDelegate() {}
    virtual ~CLIStatsDelegate() {};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
    virtual void reportProgress(EMMPM_Data::Pointer data)
    {
      // Check to make sure we are at the end of an em loop
      if ( data->inside_mpm_loop == 0 && NULL != data->outputImage)
      {
        char buff[256];
        memset(buff, 0, 256);
// #if (_WIN32)
//         snprintf(buff, 256, "C:\\Data\\emmpm_out_%d.tif", data->currentEMLoop);
// #else
//         snprintf(buff, 256, "/tmp/emmpm_out_%d.tif", data->currentEMLoop);
// #endif
        std::cout << "Writing Image: " << buff << std::endl;
        TiffUtilities tifUtil;
        int err = tifUtil.writeGrayScaleImage(buff, data->rows, data->columns, "Intermediate Image", data->outputImage);
        if (err < 0)
        {
          std::cout << "Error writing intermediate tiff image." << std::endl;
        }

        std::cout << "Class\tMu\tSigma" << std::endl;
        for (int l = 0; l < data->classes; l++)
        {
          //    snprintf(msgbuff, 256, "%d\t%.3f\t%.3f", l, data->m[l], data->v[l]);
          //    EMMPM_ShowProgress(msgbuff, data->progress);
          std::cout << l << "\t" << data->mean[l] << "\t" << data->variance[l] << "\t" << std::endl;
        }
#if 0
        real_t hist[EMMPM_MAX_CLASSES][256];
        // Generate a gaussian curve for each class based off the mu and sigma for that class
        for (int c = 0; c < data->classes; ++c)
        {
          real_t mu = data->m[c];
          real_t sig = data->v[c];
          real_t twoSigSqrd = sig * sig * 2.0f;
          real_t constant = 1.0f / (sig * sqrtf(2.0f * M_PI));
          for (size_t x = 0; x < 256; ++x)
          {
            hist[c][x] = constant * exp(-1.0f * ((x - mu) * (x - mu)) / (twoSigSqrd));
          }
        }

        memset(buff, 0, 256);
        snprintf(buff, 256, "/tmp/emmpm_hist_%d.csv", data->currentEMLoop);
        std::ofstream file(buff, std::ios::out | std::ios::binary);
        if (file.is_open())
        {
          for (size_t x = 0; x < 256; ++x)
          {
            file << x;
            for (int c = 0; c < data->classes; ++c)
            {
              file << ", " << hist[c][x];
            }
            file << std::endl;
          }
        }
#endif

      }

    }

  private:
    CLIStatsDelegate(const CLIStatsDelegate&); // Copy Constructor Not Implemented
    void operator=(const CLIStatsDelegate&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{

//  unsigned long long int millis = EMMPM_getMilliSeconds();

#if defined (EMMPM_USE_PARALLEL_ALGORITHMS)
  tbb::task_scheduler_init init;
  std::cout << "Default Number of Threads: " << init.default_num_threads() << std::endl;
#endif

  int err = 0;
  EMMPM_Data::Pointer data = EMMPM_Data::New();
//  EMMPM_CallbackFunctions* callbacks = EMMPM_AllocateCallbackFunctionStructure();
#if 1
  /* Parse the command line arguments */
  EMMPMInputParser parser;
  err = parser.parseCLIArguments(argc, argv, data.get());

  if (err < 0)
  {
    printf("Error trying to parse the arguments.\n");
    return 0;
  }
#else
  char* infile = (char*)(malloc(1024));
  memset(infile, 0, 1024);
  snprintf(infile, 1024, "/Users/mjackson/Desktop/EMMPM.tif");
  char* outfile = (char*)(malloc(1024));
  memset(outfile, 0, 1024);
  snprintf(outfile, 1024, "/tmp/out.tif");
  data->input_file_name = infile;
  data->output_file_name = outfile;
  data->emIterations = 1;
  data->mpmIterations = 1;
  data->in_beta = 1.0;
  data->classes = 2;
  data->dims = 1;
  data->initType = EMMPM_Basic;
  data->simulatedAnnealing = 0;
  data->grayTable[0] = 0;
  data->grayTable[1] = 255;
  data->verbose = 1;
  data->w_gamma[0] = 1.0;
  data->w_gamma[1] = 1.0;
  data->useCurvaturePenalty = 0;
  data->ccostLoopDelay = 0;
  data->beta_e = 1.0;
  data->beta_c = 0.0;
  data->r_max = 1.0;
#endif


  /* Set the Callback functions to provide feedback */
  Observer obs;
  CLIStatsDelegate::Pointer statsDelegate = CLIStatsDelegate::New();


  // Get our input image from the Image IO functions
  TiffUtilities tifUtil;
  err = tifUtil.readInputImage(data);
  if (err < 0)
  {
    printf("Error Reading the input image.\n");
    return 0;
  }

  InitializationFunction::Pointer initFunction = BasicInitialization::New();

  // Set the initialization function based on the command line arguments
  switch(data->initType)
  {
    case EMMPM_ManualInit:
      initFunction = InitializationFunction::New();
      break;
    case EMMPM_UserInitArea:
      initFunction = UserDefinedAreasInitialization::New();
      break;
    default:
      break;
  }

  obs.updateProgressAndMessage("EM/MPM Starting.... ", 0);

  // Allocate all the memory here
  data->allocateDataStructureMemory();

  if (err)
  {
    printf("Error allocating memory for the EMMPM Data Structure.\n   %s(%d)\n", __FILE__, __LINE__);
    return 1;
  }

  EMMPM::Pointer emmpm = EMMPM::New();
  emmpm->addObserver(&obs);
  emmpm->setData(data);
  emmpm->setStatsDelegate(statsDelegate.get());
  emmpm->setInitializationFunction(initFunction);
  emmpm->execute();



  err = tifUtil.writeOutputImage(data);
  if (err < 0)
  {
    return 0;
  }
#if 0
#if defined (EMMPM_USE_PARALLEL_ALGORITHMS)
  std::cout << "Parrallel Time to Complete:";
#else
  std::cout << "Serial Time To Complete: ";
#endif
  std::cout  << (EMMPM_getMilliSeconds() - millis) << std::endl;
#endif

  EMMPMUtilities::MonitorMeansAndVariances(data);

  // Write the stats file
  std::stringstream ss;
  ss << data->output_file_name << "_Stats.txt";

  std::cout << "StatsFile: " << ss.str() << std::endl;


  FILE* f = fopen(ss.str().c_str(), "wb");
  fprintf(f, "InputFile:%s\n", data->input_file_name);
  fprintf(f, "SegmentedFile:%s\n" , data->output_file_name);
  fprintf(f, "NumClasses:%d\n", data->classes);
  fprintf(f, "Class,Mu,Sigma\n");
  // Remember the Sigma is the Square Root of the variance
  for(int i = 0; i < data->classes; ++i)
  {
    fprintf(f, "%d,%f,%f\n", i,  data->mean[i] , sqrtf(data->variance[i]) );
  }

  fclose(f);

  std::cout << "EM/MPM Ending" << std::endl;

  return 1;
}

