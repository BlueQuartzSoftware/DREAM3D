



#include <QtCore/QVector>
#include <QtCore/QDir>

#include <QtGui/QApplication>
#include <QtGui/QImage>

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdConstants.h"


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Texture.h"
#include "DREAM3DLib/Common/StatsGen.hpp"

#define PFTEST_NO_QOBJECT 1

#include "StatsGenerator/PoleFigureMaker.h"

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
  PoleFigureMaker colorPoleFigure;
  return colorPoleFigure.generateColorPoleFigureImage(data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int makePoleFigures(QVector<float> e1s, QVector<float> e2s, QVector<float> e3s, QVector<float> weights, QVector<float> sigmas, QString prefix)
{
  int err = 0;
  QVector<float> odf;
  QVector<float> x001;
  QVector<float> y001;
  QVector<float> x011;
  QVector<float> y011;
  QVector<float> x111;
  QVector<float> y111;

  
  int size = 5000;
  unsigned int m_CrystalStructure = Ebsd::CrystalStructure::Cubic_High;
  qint32 kRad[2] = {4, 4};
  qint32 pfSize[2] = {226, 226};
  PoleFigureMaker colorPoleFigure;

  if ( Ebsd::CrystalStructure::Check::IsCubic(m_CrystalStructure))
  {
    std::cout << "Generating Cubic Pole Figure Images" << std::endl;
    static const size_t odfsize = 5832;
    // float totalweight = 0;
    odf.resize(odfsize);
    Texture::calculateCubicODFData(e1s, e2s, e3s, weights, sigmas, true, odf);
    err = StatsGen::GenCubicODFPlotData(odf, x001, y001, x011, y011, x111, y111, size);
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
  else if ( Ebsd::CrystalStructure::Check::IsHexagonal(m_CrystalStructure))
  {
    std::cout << "Generating Hex Pole Figure Images" << std::endl;
    static const size_t odfsize = 15552;
    // float totalweight = 0;
    odf.resize(odfsize);
    Texture::calculateHexODFData(e1s, e2s, e3s, weights, sigmas, true, odf);
    err = StatsGen::GenHexODFPlotData(odf, x001, y001, x011, y011, x111, y111, size);
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

  return EXIT_SUCCESS;
}
