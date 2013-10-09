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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>

#include "EbsdLib/TSL/AngReader.h"

#include "DREAM3DLib/DREAM3DVersion.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

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
    QString headerWord(QByteArray buf);
    int fixHeaderValues(QString in,
                        QVector<QString>& headerLines);

    /** @brief Sets the file name of the ebsd file to be read */
    DREAM3D_INSTANCE_STRING_PROPERTY(FileName)
    DREAM3D_INSTANCE_STRING_PROPERTY(OutputFileName)
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
int AngResFixer::fixHeaderValues(QString str,
                                 QVector<QString>& headerLines)
{
  QTextStream in(&str);
  int err = 0;
  QString buf = in.readLine();
  while (!in.atEnd() )
  {

    QStringList tokens = buf.split(' ');
    QString word(tokens[1]);

    if (word.size() > 0 && word.startsWith(Ebsd::Ang::XStep) == 0)
    {
      QString fix = QString("# XSTEP: %1\r\n").arg(m_XStepFix);
      headerLines.push_back(fix);
    }
    else if (word.size() > 0 && word.startsWith(Ebsd::Ang::YStep) == 0)
    {
      QString fix = QString("# YSTEP: %1\r\n").arg(m_YStepFix);
      headerLines.push_back(fix);
    }

    buf = in.readLine();
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

  QVector<QString> headerLines;
  QString in(reader.getOriginalHeader());
  err = fixHeaderValues(in, headerLines);


  FILE* out = fopen(m_OutputFileName.toAscii().data(), "wb");

  // Write out the header
  for (QVector<QString>::iterator hline = headerLines.begin(); hline != headerLines.end(); ++hline )
  {
    fprintf(out, "%s", (*hline).toAscii().data());
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  std::cout << "Starting Resolution Fixer" << std::endl;

  try
  {
    // Handle program options passed on command line.
    TCLAP::CmdLine cmd("ResFixer", ' ', DREAM3DLib::Version::Complete().toStdString());

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

    QDir dir(QString::fromStdString(outputDir.getValue()));
    if (dir.exists() == false)
    {
      dir.mkpath(".");
    }

    QDir inputD( QString::fromStdString(inputDir.getValue()));
    QStringList entryList = inputD.entryList();
    foreach(QString file, entryList)
    {
      QFileInfo fi(file);
      if (fi.suffix().compare("ang") == 0 )
      {
        std::cout << "Fixing file " << file.toStdString() << std::endl;
        AngResFixer fixer;
        fixer.setFileName( QString::fromStdString(inputDir.getValue()) + "/" + fi.fileName() );
        fixer.setXStepFix(xres.getValue());
        fixer.setYStepFix(yres.getValue());
        QString outFile = QString::fromStdString(outputDir.getValue()) + "/" + fi.fileName();
        fixer.setOutputFileName(outFile);
        fixer.fixFile();
        std::cout << "   + Complete" << std::endl;
      }
    }

  }
  catch (TCLAP::ArgException& e) // catch any exceptions
  {
    std::cerr  << " error: " << e.error() << " for arg " << e.argId() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}


