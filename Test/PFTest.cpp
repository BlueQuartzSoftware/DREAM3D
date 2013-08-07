/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */



#include <QtCore/QVector>
#include <QtCore/QDir>

#include <QtGui/QApplication>
#include <QtGui/QImage>

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdConstants.h"


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Texture.hpp"
#include "DREAM3DLib/Common/StatsGen.hpp"

#define PFTEST_NO_QOBJECT 1

#include "QtSupport/PoleFigureImageUtilities.h"

#include "TestFileLocations.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void poleFigureGenerationComplete()
{
  //  std::cout << "ODF Pole Figure generation complete" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage generateODFPoleFigure(const PoleFigureData &data)
{
  PoleFigureImageUtilities colorPoleFigure;
  return colorPoleFigure.generateColorPoleFigureImage(data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int makePoleFigures(QVector<float> e1s, QVector<float> e2s, QVector<float> e3s,
                             QVector<float> weights, QVector<float> sigmas, QString prefix)
{
  int err = 0;
  QVector<float> odf(CubicOps::k_OdfSize);
  size_t npoints = 5000;
  QVector<float > x001(npoints * 3);
  QVector<float > y001(npoints * 3);
  QVector<float > x011(npoints * 6);
  QVector<float > y011(npoints * 6);
  QVector<float > x111(npoints * 4);
  QVector<float > y111(npoints * 4);


  unsigned int m_CrystalStructure = Ebsd::CrystalStructure::Cubic_High;
  qint32 kRad[2] = {4, 4};
  qint32 pfSize[2] = {226, 226};
  PoleFigureImageUtilities colorPoleFigure;
  size_t numEntries = e1s.size();

  if ( Ebsd::CrystalStructure::Cubic_High == m_CrystalStructure)
  {
    std::cout << "Generating Cubic Pole Figure Images" << std::endl;

    Texture::CalculateCubicODFData(e1s.data(), e2s.data(), e3s.data(),
                                   weights.data(), sigmas.data(), true,
                                   odf.data(), numEntries);

//    err = StatsGen::GenCubicODFPlotData(odf.data(), x001.data(), y001.data(),
//                                        x011.data(), y011.data(), x111.data(), y111.data(), npoints);
    if (err == 1)
    {
      return EXIT_FAILURE;
    }
    {
      QImage image = generateODFPoleFigure(PoleFigureData(x001, y001, QString("<001>"), kRad, pfSize));
      QString path = QString::fromStdString(UnitTest::PFTest::TestDir);
      path.append("/").append(prefix).append("001.bmp");
      bool saved = image.save(path);
      if (!saved)
      {
        std::cout << "Image Not Saved: " << path.toStdString() << std::endl;
      }
      else
      {
        std::cout << "Image Saved: " << path.toStdString() << std::endl;
      }
    }
    {
      QImage image = generateODFPoleFigure(PoleFigureData(x011, y011, QString("<011>"), kRad, pfSize));
      QString path = QString::fromStdString(UnitTest::PFTest::TestDir);
      path.append(prefix).append("011.bmp");
      bool saved = image.save(path);
      if (!saved)
      {
        std::cout << "Image Not Saved: " << path.toStdString() << std::endl;
      }
      else
      {
        std::cout << "Image Saved: " << path.toStdString() << std::endl;
      }
    }
    {
      QImage image = generateODFPoleFigure(PoleFigureData(x111, y111, QString("<111>"), kRad, pfSize));
      QString path = QString::fromStdString(UnitTest::PFTest::TestDir);
      path.append(prefix).append("111.bmp");
      bool saved = image.save(path);
      if (!saved)
      {
        std::cout << "Image Not Saved: " << path.toStdString() << std::endl;
      }
      else
      {
        std::cout << "Image Saved: " << path.toStdString() << std::endl;
      }
    }


  }
  else if ( Ebsd::CrystalStructure::Hexagonal_High == m_CrystalStructure)
  {
    std::cout << "Generating Hex Pole Figure Images" << std::endl;

    odf.resize(HexagonalOps::k_OdfSize);
    x001.resize(npoints * 1);
    y001.resize(npoints * 1);
    x011.resize(npoints * 3);
    y011.resize(npoints * 3);
    x111.resize(npoints * 3);
    y111.resize(npoints * 3);

    Texture::CalculateHexODFData(e1s.data(), e2s.data(), e3s.data(),
                                 weights.data(), sigmas.data(), true,
                                 odf.data(), numEntries);
//    err = StatsGen::GenHexODFPlotData(odf.data(), x001.data(), y001.data(),
//                                      x011.data(), y011.data(), x111.data(), y111.data(), npoints);
    if (err == 1)
    {
      return EXIT_FAILURE;
    }
    {
      QImage image = generateODFPoleFigure(PoleFigureData(x001, y001, QString("<0001>"), kRad, pfSize));
      QString path = QString::fromStdString(UnitTest::PFTest::TestDir);
      path.append(prefix).append("0001.bmp");
      bool saved = image.save(path);
      if (!saved)
      {
        std::cout << "Image Not Saved: " << path.toStdString() << std::endl;
      }
      else
      {
        std::cout << "Image Saved: " << path.toStdString() << std::endl;
      }
    }
    {

      QImage image = generateODFPoleFigure(PoleFigureData(x011, y011, QString("<11-20>"), kRad, pfSize));
      QString path = QString::fromStdString(UnitTest::PFTest::TestDir);
      path.append(prefix).append("11-20.bmp");
      bool saved = image.save(path);
      if (!saved)
      {
        std::cout << "Image Not Saved: " << path.toStdString() << std::endl;
      }
      else
      {
        std::cout << "Image Saved: " << path.toStdString() << std::endl;
      }
    }
    {
      QImage image = generateODFPoleFigure(PoleFigureData(x111, y111, QString("<10-10>"), kRad, pfSize));
      QString path = QString::fromStdString(UnitTest::PFTest::TestDir);
      path.append(prefix).append("10-10.bmp");
      bool saved = image.save(path);
      if (!saved)
      {
        std::cout << "Image Not Saved: " << path.toStdString() << std::endl;
      }
      else
      {
        std::cout << "Image Saved: " << path.toStdString() << std::endl;
      }
    }

  }

  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char *argv[])
{

  QApplication app(argc, argv);

  QDir d(QString::fromStdString(UnitTest::PFTest::TestDir));
  d.mkdir(QString::fromStdString(UnitTest::PFTest::TestDir));

  unsigned int err = 0;
  // std::cout << "StatsGenODFWidget[" << objectName().toStdString() << "]::on_m_CalculateODFBtn_clicked" << std::endl;

  QVector<float> e1s;
  QVector<float> e2s;
  QVector<float> e3s;
  QVector<float> weights;
  QVector<float> sigmas;

  err = makePoleFigures(e1s, e2s, e3s, weights, sigmas, "Random_");


  e1s.push_back(90.0);
  e2s.push_back(35.0);
  e3s.push_back(45.0);
  weights.push_back(1000);
  sigmas.push_back(3);

  // Convert from Degrees to Radians
  for(int i=0;i<e1s.size();i++)
  {
    e1s[i] = e1s[i]*M_PI/180.0;
    e2s[i] = e2s[i]*M_PI/180.0;
    e3s[i] = e3s[i]*M_PI/180.0;
  }

  err = makePoleFigures(e1s, e2s, e3s, weights, sigmas, "Phantom_");

  e1s[0] = 0.0;
  e2s[0] = 0.0;
  e3s[0] = 0.0;
  weights[0] = 45;
  sigmas[0] = 8;
  err = makePoleFigures(e1s, e2s, e3s, weights, sigmas, "000_");


  return EXIT_SUCCESS;
}
