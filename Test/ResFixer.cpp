/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

#include "EbsdLib/TSL/AngReader.h"

#include "DREAM3DLib/DREAM3DVersion.h"

#include "MXA/Common/MXASetGetMacros.h"
#include "MXA/Common/LogTime.h"
#include <QtCore/QDir>
#include <QtCore/QFile>
#include "MXA/Utilities/MXAFileInfo.h"

#define kBufferSize 1024

#if _MSC_VER
#define snprintf _snprintf
#endif

class AngResFixer
{
  public:
    AngResFixer();
    virtual~AngResFixer() {}

    virtual int fixFile();
    std::string headerWord(char* buf, size_t length);
    int fixHeaderValues(std::iostream &in,
                       std::vector<std::string> &headerLines);

    /** @brief Sets the file name of the ebsd file to be read */
    MXA_INSTANCE_STRING_PROPERTY(FileName)
    MXA_INSTANCE_STRING_PROPERTY(OutputFileName)
    DREAM3D_INSTANCE_PROPERTY(float, XStepFix)
    DREAM3D_INSTANCE_PROPERTY(float, YStepFix)
  private:


      AngResFixer(const AngResFixer&); // Copy Constructor Not Implemented
      void operator=(const AngResFixer&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngResFixer::AngResFixer()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string AngResFixer::headerWord(char* buf, size_t length)
{
  if (buf[0] != '#')
  {
    return std::string("");
  }
  // Start at the first character and walk until you find another non-space character
  size_t i = 1;
  while(buf[i] == ' ')
  {
    ++i;
  }
  size_t wordStart = i;
  while(1)
  {
    if (buf[i] == 45 || buf[i] == 95) { ++i; } // "-" or "_" character
    else if (buf[i] >= 65 && buf[i] <=90) { ++i; } // Upper case alpha character
    else if (buf[i] >= 97 && buf[i] <=122) {++i; } // Lower case alpha character
    else { break;}
  }
  size_t wordEnd = i;

  std::string word( &(buf[wordStart]), wordEnd - wordStart);
  return word;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AngResFixer::fixHeaderValues(std::iostream &in,
                                std::vector<std::string> &headerLines)
{
  int err = 0;
  char buf[kBufferSize];
  ::memset(buf, 0, kBufferSize);
  in.getline(buf, kBufferSize);
  while (!in.eof() )
  {
    int i = 0;
    while (buf[i] != 0) { ++i; }
    buf[i] = 10; //Add back in the \n character

    std::string word = headerWord(buf, kBufferSize);
    if (word.size() > 0 && word.compare(Ebsd::Ang::XStep) == 0)
    {
      ::memset(buf, 0, kBufferSize);
      snprintf(buf, kBufferSize, "# XSTEP: %06f\r\n", m_XStepFix);
    }
    else if (word.size() > 0 && word.compare(Ebsd::Ang::YStep) == 0)
    {
      ::memset(buf, 0, kBufferSize);
      snprintf(buf, kBufferSize, "# YSTEP: %06f\r\n", m_YStepFix);
    }
    headerLines.push_back(std::string(buf));
    ::memset(buf, 0, kBufferSize);
    in.getline(buf, kBufferSize);
  }
  return err;
}




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AngResFixer::fixFile()
{

  int err = 0;

  AngReader reader;
  reader.setFileName(m_FileName);
  err |= reader.readFile();
  if (err != 0)
  {
    return err;
  }

  std::vector<std::string> headerLines;
  std::stringstream in(reader.getOriginalHeader());
  err = fixHeaderValues(in, headerLines);


  FILE* out = fopen(m_OutputFileName.c_str(), "wb");

  // Write out the header
  for (std::vector<std::string>::iterator hline = headerLines.begin(); hline != headerLines.end(); ++hline )
  {
    fprintf(out, "%s", (*hline).c_str());
  }

  float* p1 = reader.getPhi1Pointer();
  float* p = reader.getPhiPointer();
  float* p2 = reader.getPhi2Pointer();
  float* x = reader.getXPositionPointer();
  float* y = reader.getYPositionPointer();
  float* iqual = reader.getImageQualityPointer();
  float* conf = reader.getConfidenceIndexPointer();
  int* ph = reader.getPhaseDataPointer();
  float* semSignal = reader.getSEMSignalPointer();
  float* fit = reader.getFitPointer();

  size_t count = reader.getNumberOfElements();
 // float xstep_old = reader.getXStep();
 // float ystep_old = reader.getYStep();
  float xFactor = m_XStepFix / reader.getXStep();
  float yFactor = m_YStepFix / reader.getYStep();

  for(size_t i = 0; i < count; ++i)
  {
    fprintf(out, "  %.5f  %.5f  %.5f  %.5f  %.5f  %.1f  %0.3f %d  %d  %.3f\n", p1[i], p[i], p2[i],
            x[i]*xFactor, y[i]*yFactor, iqual[i], conf[i], ph[i], (int)(semSignal[i]), fit[i]);
  }

  fclose(out);
  return err;

}


int main(int argc, char **argv)
{
  std::cout << "Starting Resolution Fixer" << std::endl;

  try
  {
    // Handle program options passed on command line.
    TCLAP::CmdLine cmd("ResFixer", ' ', DREAM3DLib::Version::Complete());

    TCLAP::ValueArg<float> xres( "x", "xres", "New X Resolution", true, 0.0f, "New X Resolution");
    cmd.add(xres);

    TCLAP::ValueArg<float> yres( "y", "yres", "New Y Resolution", true, 0.0f, "New Y Resolution");
    cmd.add(yres);

    TCLAP::ValueArg<std::string> inputDir( "i", "inputdir", "Input Directory", true, "", "Input Directory");
    cmd.add(inputDir);

    TCLAP::ValueArg<std::string> outputDir( "o", "outputdir", "Output Directory", true, "", "Output Directory");
    cmd.add(outputDir);

    // Parse the argv array.
    cmd.parse(argc, argv);
    if (argc == 1)
    {
      std::cout << "ResFixer program was not provided any arguments. Use the --help argument to show the help listing." << std::endl;
      return EXIT_FAILURE;
    }

    std::cout << "InputDir:" << std::endl;
    std::cout << "   " << inputDir.getValue() << std::endl;
    std::cout << "OutputDir:" << std::endl;
    std::cout << "   " << outputDir.getValue() << std::endl;
    std::cout << "New X Step: " << xres.getValue() << std::endl;
    std::cout << "New Y Step: " << yres.getValue() << std::endl;


    if (MXADir::exists(outputDir.getValue()) == false)
    {
      MXADir::mkdir(outputDir.getValue(), true);
    }

    std::vector<std::string> entryList = MXADir::entryList(inputDir.getValue());
    for (std::vector<std::string>::iterator file = entryList.begin(); file != entryList.end(); ++file )
    {
      if (MXAFileInfo::extension(*file).compare("ang") == 0 )
      {
        std::cout << "Fixing file " << *file << std::endl;
        AngResFixer fixer;
        fixer.setFileName(inputDir.getValue() + MXADir::Separator + *file);
        fixer.setXStepFix(xres.getValue());
        fixer.setYStepFix(yres.getValue());
        std::string outFile = outputDir.getValue() + MXADir::Separator + MXAFileInfo::filename(*file);
        fixer.setOutputFileName(outFile);
        fixer.fixFile();
        std::cout << "   + Complete" << std::endl;
      }
    }

  }
  catch (TCLAP::ArgException &e) // catch any exceptions
  {
    std::cerr << logTime() << " error: " << e.error() << " for arg " << e.argId() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}


