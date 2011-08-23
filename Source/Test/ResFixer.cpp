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

#include "DREAM3D/DREAM3DVersion.h"

#include "MXA/Common/MXASetGetMacros.h"
#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXADir.h"
#include "MXA/Utilities/MXAFileInfo.h"

#define kBufferSize 1024

class AngResFixer : public AngReader
{
  public:
    AngResFixer();
    virtual~AngResFixer() {};

    virtual int readFile();
    std::string headerWord(char* buf, size_t length);

    MXA_INSTANCE_STRING_PROPERTY(OutputFileName)
    MXA_INSTANCE_PROPERTY(float, XStepFix);
    MXA_INSTANCE_PROPERTY(float, YStepFix);
  private:


      AngResFixer(const AngResFixer&); // Copy Constructor Not Implemented
      void operator=(const AngResFixer&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngResFixer::AngResFixer() :
    AngReader()
{}

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
  size_t wordEnd = i+1;
  while(1)
  {
    if (buf[i] == 45 || buf[i] == 95) { ++i; } // "-" or "_" character
    else if (buf[i] >= 65 && buf[i] <=90) { ++i; } // Upper case alpha character
    else if (buf[i] >= 97 && buf[i] <=122) {++i; } // Lower case alpha character
    else { break;}
  }
  wordEnd = i;

  std::string word( &(buf[wordStart]), wordEnd - wordStart);
  return word;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AngResFixer::readFile()
{

  int err = 1;
  char buf[kBufferSize];

  std::ifstream in(getFileName().c_str());

  if (!in.is_open())
  {
    std::cout << "Ang file could not be opened: " << getFileName() << std::endl;
    return -100;
  }

  FILE* out = fopen(m_OutputFileName.c_str(), "wb");
  bool xStepComplete = false;
  bool yStepComplete = false;


  ::memset(buf, 0, kBufferSize);
  in.getline(buf, kBufferSize);
  while (!in.eof() )
  {

    if (in.gcount() > 0 && xStepComplete && yStepComplete)
    {
      fprintf(out, "%s\n", buf);
    }
    else
    {
      std::string word = headerWord(buf, kBufferSize);
      if (word.length() == 0)
      {
        fprintf(out, "%s\n", buf);
      }
      else if (word.compare(Ebsd::Ang::XStep) == 0)
      {
        xStepComplete = true;
        fprintf(out, "# XSTEP: %06f\r\n", m_XStepFix);
      }
      else if (word.compare(Ebsd::Ang::YStep) == 0)
      {
        yStepComplete = true;
        fprintf(out, "# YSTEP: %06f\r\n", m_YStepFix);
      }
      else
      {
        fprintf(out, "%s\n", buf);
      }
    }
    ::memset(buf, 0, kBufferSize);
    in.getline(buf, kBufferSize);
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
    TCLAP::CmdLine cmd("ResFixer", ' ', DREAM3D::Version::Complete);

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
        fixer.readFile();
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


