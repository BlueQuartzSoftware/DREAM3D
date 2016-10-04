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

#include "EMMPMInputParser.h"

#include <string.h>

#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

#include "EMMPMLib/EMMPMLibVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMInputParser::EMMPMInputParser()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMInputParser::~EMMPMInputParser()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EMMPMInputParser::parseGrayTable(const std::string& values, EMMPM_Data* inputs)
{
  std::string::size_type pos = values.find(",", 0);

  size_t index = 0;
  // unsigned int* cPtr = &(inputs->grayTable[index]);

  int n = sscanf(values.substr(0, pos).c_str(), "%d", &(inputs->colorTable[index]));
  if(n != 1)
  {
    inputs->colorTable[index] = 0;
    return -1;
  }

  ++index;
  while(pos != std::string::npos && pos != values.size() - 1)
  {
    n = sscanf(values.substr(pos + 1).c_str(), "%d", &(inputs->colorTable[index]));
    pos = values.find(",", pos + 1);
    ++index;
  }

  if(index != static_cast<size_t>(inputs->classes))
  {
    std::cout << "Mismatch between the number of classes declared [" << inputs->classes << "] and the number of values declared for the gray level table. [" << index << "]. Both should match."
              << std::endl;
    return -1;
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EMMPMInputParser::parseInitCoords(const std::string& coords, EMMPM_Data* inputs)
{
  std::string::size_type pos = coords.find(":", 0);

  size_t index = 0;
  unsigned int* cPtr = inputs->initCoords[index];

  int n = sscanf(coords.substr(0, pos).c_str(), "%d,%d,%d,%d", &(cPtr[0]), &(cPtr[1]), &(cPtr[2]), &(cPtr[3]));
  if(n != 4)
  {
    cPtr[0] = cPtr[1] = cPtr[2] = cPtr[3] = 0;
    return -1;
  }

  ++index;
  while(pos != std::string::npos && pos != coords.size() - 1)
  {
    cPtr = inputs->initCoords[index];
    n = sscanf(coords.substr(pos + 1).c_str(), "%d,%d,%d,%d", &(cPtr[0]), &(cPtr[1]), &(cPtr[2]), &(cPtr[3]));
    pos = coords.find(":", pos + 1);
    ++index;
  }

  if(index != static_cast<size_t>(inputs->classes))
  {
    std::cout << "Mismatch between the number of classes declared [" << inputs->classes << "] and the number of sets of coordinated to use for initialization [" << index << "]" << std::endl;
    return -1;
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EMMPMInputParser::parseMuSigmaValues(const std::string& values, EMMPM_Data* inputs)
{
  std::string::size_type pos = values.find(":", 0);

  inputs->dims = 1; // FORCING A GRAY SCALE IMAGE TO BE USED
  inputs->mean = (real_t*)malloc(inputs->classes * inputs->dims * sizeof(real_t));
  inputs->variance = (real_t*)malloc(inputs->classes * inputs->dims * sizeof(real_t));

  size_t index = 0;
  real_t* mPtr = inputs->mean;
  real_t* vPtr = inputs->variance;

  int n = sscanf(values.substr(0, pos).c_str(), "%f,%f", mPtr, vPtr);
  if(n != 2)
  {
    *mPtr = 0.0;
    *vPtr = 0.0;
    return -1;
  }
  index++;

  while(pos != std::string::npos && pos != values.size() - 1)
  {
    mPtr++;
    vPtr++;
    n = sscanf(values.substr(pos + 1).c_str(), "%f,%f", &(mPtr[0]), &(vPtr[0]));
    pos = values.find(":", pos + 1);
    ++index;
  }

  // Take the square of the sigma to get the variance (v = sigma^2);
  for(size_t i = 0; i < index; ++i)
  {
    inputs->variance[i] = (inputs->variance[i]) * (inputs->variance[i]);
  }

  if(index != static_cast<size_t>(inputs->classes))
  {
    std::cout << "Mismatch between the number of classes declared [" << inputs->classes << "] and the number of sets of Mean/Variance to use for initialization [" << index << "]" << std::endl;
    return -1;
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char* EMMPMInputParser::copyFilenameToNewCharBuffer(const std::string& fname)
{
  std::string::size_type size = fname.size() + 1;
  char* buf = nullptr;
  if(size > 1)
  {
    buf = (char*)malloc(size);
    ::memset(buf, 0, size);
    strncpy(buf, fname.c_str(), size - 1);
  }
  return buf;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EMMPMInputParser::parseCLIArguments(int argc, char* argv[], EMMPM_Data* inputs)
{
  if(nullptr == inputs)
  {
    printf("The EMMPM_Inputs pointer was null. Returning early.\n");
    return -1;
  }

  TCLAP::CmdLine cmd("", ' ', EMMPMLib::Version::Complete().toStdString());

  TCLAP::ValueArg<std::string> in_inputFile("i", "inputfile", "Image File to be used as input", true, "", "");
  cmd.add(in_inputFile);
  TCLAP::ValueArg<std::string> in_outputFile("o", "outputfile", "The Image Output File", true, "", "");
  cmd.add(in_outputFile);

  TCLAP::ValueArg<float> in_beta("b", "beta", "Beta Value", false, 1.0, "1.0");
  cmd.add(in_beta);
  TCLAP::ValueArg<float> in_gamma("g", "gamma", "Gamma Value", false, 0.0f, "0.0");
  cmd.add(in_gamma);
  TCLAP::ValueArg<int> in_emIter("e", "emIter", "Number of EM Iterations", false, 5, "5");
  cmd.add(in_emIter);
  TCLAP::ValueArg<int> in_mpmIter("m", "mpmIter", "Number of MPM Iterations", false, 5, "5");
  cmd.add(in_mpmIter);
  TCLAP::ValueArg<int> in_numClasses("n", "numClasses", "The Number of classes or phases in the material", false, 2, "2");
  cmd.add(in_numClasses);
  TCLAP::SwitchArg in_verbose("v", "verbose", "Verbose Output", false);
  cmd.add(in_verbose);

  TCLAP::SwitchArg simAnneal("s", "simanneal", "Use Simulated Annealing", false);
  cmd.add(simAnneal);

  TCLAP::ValueArg<int> initType("z", "inittype", "The initialization algorithm that should be performed", false, 0, "1");
  cmd.add(initType);
  TCLAP::ValueArg<std::string> initcoords("", "coords", "The upper left (x,y) and lower right (x,y) pixel coordinate sets of each class to be used in the initialization algorithm where each set is "
                                                        "separated by a colon ':'. An example is 487,192,507,212:0,332,60,392 for 2 class system.",
                                          false, "", "");
  cmd.add(initcoords);

  TCLAP::ValueArg<std::string> graytable("", "graytable", "Comma separated list of grayscale values for each class. This can be used to combine classes together at file writing time.", false, "", "");
  cmd.add(graytable);

  TCLAP::ValueArg<float> beta_e("", "beta_e", "Gradient Penalty Weight", false, 0.0, "0.0");
  cmd.add(beta_e);

  TCLAP::ValueArg<float> beta_c("", "beta_c", "Curvature Penalty Weight", false, 0.0, "0.0");
  cmd.add(beta_c);
  TCLAP::ValueArg<float> rmax("", "rmax", "Maximum Radius for Curvature Morphalogical Filter", false, 15.0, "15.0");
  cmd.add(rmax);
  TCLAP::ValueArg<int> em_loop_delay("", "emLoopDelay", "Number of EM Loops to delay before applying the Morphological Filter", false, 1, "1");
  cmd.add(em_loop_delay);

  TCLAP::ValueArg<std::string> mv("", "mv", "Pairs of Mean,Sigma initial values for each class where each set is separated by a ':'. Example for 2 classes is: 121.3,22.8:205.2,45.0", false, "", "");
  cmd.add(mv);

  if(argc < 2)
  {
    std::cout << "EM/MPM Command Line Version " << cmd.getVersion() << std::endl;
    std::vector<std::string> args;
    args.push_back(argv[0]);
    args.push_back("-h");
    cmd.parse(args);
    return -1;
  }

  try
  {
    int error = 0;
    cmd.parse(argc, argv);
    inputs->in_beta = in_beta.getValue();
    for(int i = 0; i < EMMPM_MAX_CLASSES; i++)
    {
      inputs->w_gamma[i] = in_gamma.getValue();
    }
    inputs->mpmIterations = in_mpmIter.getValue();
    inputs->emIterations = in_emIter.getValue();
    inputs->classes = in_numClasses.getValue();
    inputs->verbose = in_verbose.getValue();
    inputs->simulatedAnnealing = simAnneal.getValue();

    inputs->input_file_name = copyFilenameToNewCharBuffer(in_inputFile.getValue());
    if(inputs->input_file_name == nullptr)
    {
      std::cout << "There was an error parsing the input file name. Did you use the '-i' argument to set an input file?" << std::endl;
      return -1;
    }

    inputs->output_file_name = copyFilenameToNewCharBuffer(in_outputFile.getValue());
    if(inputs->output_file_name == nullptr)
    {
      std::cout << "There was an error parsing the output file name. Did you use the '-o' argument to set an input file?" << std::endl;
      return -1;
    }

    if(inputs->initType == EMMPM_UserInitArea)
    {
      error = parseInitCoords(initcoords.getValue(), inputs);
      if(error < 0)
      {
        std::cout << "There was an error parsing the command line arguments for the initialization coordinates." << std::endl;
        return -1;
      }
    }
    if(inputs->initType == EMMPM_ManualInit)
    {
      error = parseMuSigmaValues(mv.getValue(), inputs);
      if(error < 0)
      {
        std::cout << "There was an error parsing the command line arguments for the mean and variance values" << std::endl;
        return -1;
      }
    }

    if(graytable.getValue().empty() == false)
    {
      error = parseGrayTable(graytable.getValue(), inputs);
      if(error < 0)
      {
        std::cout << "There was an error parsing the Gray Level Table." << std::endl;
        return -1;
      }
    }
    else // Create a default gray level table
    {
      int n = inputs->classes - 1;
      for(int value = 0; value < inputs->classes; ++value)
      {
        unsigned int argb = 0xFF000000; // Black
        unsigned char* ptr = reinterpret_cast<unsigned char*>(&argb);
        ptr[1] = value * 255 / n;
        ptr[2] = value * 255 / n;
        ptr[3] = value * 255 / n;
        inputs->colorTable[value] = argb;
      }
    }

    /* Parse the Gradient Penalty Weighting (Beta E) from the command line */
    if(beta_e.getValue() > 0.0)
    {
      inputs->useGradientPenalty = 1;
      inputs->beta_e = beta_e.getValue();
    }

    /* Parse the Curvature Penalty Arguments  */
    if(beta_c.getValue() > 0.0)
    {
      inputs->useCurvaturePenalty = 1;
      inputs->beta_c = beta_c.getValue();
      inputs->r_max = rmax.getValue();
      inputs->ccostLoopDelay = em_loop_delay.getValue();
    }
  } catch(TCLAP::ArgException& e)
  {
    std::cerr << " error: " << e.error() << " for arg " << e.argId() << std::endl;
    std::cout << "** Unknown Arguments. Displaying help listing instead. **" << std::endl;
    return -1;
  }
  return 0;
}
