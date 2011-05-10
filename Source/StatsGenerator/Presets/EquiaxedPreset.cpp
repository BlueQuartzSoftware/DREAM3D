/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "EquiaxedPreset.h"
#include "StatsGenerator/StatsGenPlotWidget.h"
#include "StatsGenerator/TableModels/SGBetaTableModel.h"
#include "StatsGenerator/TableModels/SGPowerLawTableModel.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EquiaxedPreset::EquiaxedPreset()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EquiaxedPreset::~EquiaxedPreset()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EquiaxedPreset::generateOmega3Data(StatsGenPlotWidget* plot, QVector<double> binNumbers)
{
  // Make sure the distribution is set correctly
  plot->setDistributionType(AIM::Reconstruction::Beta, false);
  // This line basically makes sure we have the distribution type we are looking for
  SGBetaTableModel* model = qobject_cast<SGBetaTableModel*>(plot->tableModel());
  if (NULL == model)
  {
    return;
  }
  qint32 count = binNumbers.count();

  // Remove all the current rows in the table model
//  model->removeRows(0, model->rowCount());

  double alpha, beta;
      AIMRandomNG rg;
      /* Get a seed value based off the system clock. The issue is that this will
       * be a 64 bit unsigned integer where the high 32 bits will basically not
       * change where as the lower 32 bits will. The following lines of code will
       * pull off the low 32 bits from the number. This operation depends on most
       * significant byte ordering which is different between Big Endian and
       * Little Endian machines. For Big endian machines the Most Significant Byte
       * (MSB) is the first 32 bits. For Little Endian machines the MSB is the
       * second 32 bits.
       */
      unsigned long long int seed = MXA::getMilliSeconds();
      unsigned int* seedPtr = reinterpret_cast<unsigned int*> (&seed);
#if CMP_WORDS_BIGENDIAN
      rg.RandomInit(seedPtr[1]);
#else
      rg.RandomInit(seedPtr[0]);
#endif

  QVector<double> alphas;
  QVector<double> betas;
  QVector<QString> colors;
  QStringList colorNames = QColor::colorNames();
  qint32 colorOffset = 21;
  for (qint32 i = 0; i < count; ++i)
   {
		alpha = (0*i) + 10.0 + rg.Random(); 
		beta = (0*i) + 1.5 + (0.5*rg.Random()); 
		alphas.push_back(alpha);
		betas.push_back(beta);
		colors.push_back(colorNames[colorOffset++]);
   }

  QVector<QVector<double> > data;
  data.push_back(alphas);
  data.push_back(betas);
  model->setTableData(binNumbers, data, colors);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EquiaxedPreset::generateBOverAPlotData(StatsGenPlotWidget* plot, QVector<double> binNumbers)
{
  // Make sure the distribution is set correctly
  plot->setDistributionType(AIM::Reconstruction::Beta, false);
  // This line basically makes sure we have the distribution type we are looking for
  SGBetaTableModel* model = qobject_cast<SGBetaTableModel*>(plot->tableModel());
  if (NULL == model)
  {
    return;
  }
  qint32 count = binNumbers.count();

  // Remove all the current rows in the table model
  model->removeRows(0, model->rowCount());

  double alpha, beta;
      AIMRandomNG rg;
      /* Get a seed value based off the system clock. The issue is that this will
       * be a 64 bit unsigned integer where the high 32 bits will basically not
       * change where as the lower 32 bits will. The following lines of code will
       * pull off the low 32 bits from the number. This operation depends on most
       * significant byte ordering which is different between Big Endian and
       * Little Endian machines. For Big endian machines the Most Significant Byte
       * (MSB) is the first 32 bits. For Little Endian machines the MSB is the
       * second 32 bits.
       */
      unsigned long long int seed = MXA::getMilliSeconds();
      unsigned int* seedPtr = reinterpret_cast<unsigned int*> (&seed);
#if CMP_WORDS_BIGENDIAN
      rg.RandomInit(seedPtr[1]);
#else
      rg.RandomInit(seedPtr[0]);
#endif

  QVector<double> alphas;
  QVector<double> betas;
  QVector<QString> colors;
  QStringList colorNames = QColor::colorNames();
  qint32 colorOffset = 21;
  for (qint32 i = 0; i < count; ++i)
   {
		alpha = (0*i) + 10.0 + rg.Random(); 
		beta = (0*i) + 1.5 + (0.5*rg.Random()); 
		alphas.push_back(alpha);
		betas.push_back(beta);
		colors.push_back(colorNames[colorOffset++]);
   }

  QVector<QVector<double> > data;
  data.push_back(alphas);
  data.push_back(betas);
  model->setTableData(binNumbers, data, colors);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EquiaxedPreset::generateCOverAPlotData(StatsGenPlotWidget* plot, QVector<double> binNumbers)
{
  // Make sure the distribution is set correctly
  plot->setDistributionType(AIM::Reconstruction::Beta, false);
  // This line basically makes sure we have the distribution type we are looking for
  SGBetaTableModel* model = qobject_cast<SGBetaTableModel*>(plot->tableModel());
  if (NULL == model)
  {
    return;
  }
  qint32 count = binNumbers.count();

  // Remove all the current rows in the table model
  model->removeRows(0, model->rowCount());

  double alpha, beta;
      AIMRandomNG rg;
      /* Get a seed value based off the system clock. The issue is that this will
       * be a 64 bit unsigned integer where the high 32 bits will basically not
       * change where as the lower 32 bits will. The following lines of code will
       * pull off the low 32 bits from the number. This operation depends on most
       * significant byte ordering which is different between Big Endian and
       * Little Endian machines. For Big endian machines the Most Significant Byte
       * (MSB) is the first 32 bits. For Little Endian machines the MSB is the
       * second 32 bits.
       */
      unsigned long long int seed = MXA::getMilliSeconds();
      unsigned int* seedPtr = reinterpret_cast<unsigned int*> (&seed);
#if CMP_WORDS_BIGENDIAN
      rg.RandomInit(seedPtr[1]);
#else
      rg.RandomInit(seedPtr[0]);
#endif

  QVector<double> alphas;
  QVector<double> betas;
  QVector<QString> colors;
  QStringList colorNames = QColor::colorNames();
  qint32 colorOffset = 21;
  for (qint32 i = 0; i < count; ++i)
   {
		alpha = (0*i) + 10.0 + rg.Random(); 
		beta = (0*i) + 1.5 + (0.5*rg.Random()); 
		alphas.push_back(alpha);
		betas.push_back(beta);
		colors.push_back(colorNames[colorOffset++]);
   }

  QVector<QVector<double> > data;
  data.push_back(alphas);
  data.push_back(betas);
  model->setTableData(binNumbers, data, colors);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EquiaxedPreset::generateCOverBPlotData(StatsGenPlotWidget* plot, QVector<double> binNumbers)
{
  // Make sure the distribution is set correctly
  plot->setDistributionType(AIM::Reconstruction::Beta, false);
  // This line basically makes sure we have the distribution type we are looking for
  SGBetaTableModel* model = qobject_cast<SGBetaTableModel*>(plot->tableModel());
  if (NULL == model)
  {
    return;
  }
  qint32 count = binNumbers.count();

  // Remove all the current rows in the table model
  model->removeRows(0, model->rowCount());

  double alpha, beta;
      AIMRandomNG rg;
      /* Get a seed value based off the system clock. The issue is that this will
       * be a 64 bit unsigned integer where the high 32 bits will basically not
       * change where as the lower 32 bits will. The following lines of code will
       * pull off the low 32 bits from the number. This operation depends on most
       * significant byte ordering which is different between Big Endian and
       * Little Endian machines. For Big endian machines the Most Significant Byte
       * (MSB) is the first 32 bits. For Little Endian machines the MSB is the
       * second 32 bits.
       */
      unsigned long long int seed = MXA::getMilliSeconds();
      unsigned int* seedPtr = reinterpret_cast<unsigned int*> (&seed);
#if CMP_WORDS_BIGENDIAN
      rg.RandomInit(seedPtr[1]);
#else
      rg.RandomInit(seedPtr[0]);
#endif

  QVector<double> alphas;
  QVector<double> betas;
  QVector<QString> colors;
  QStringList colorNames = QColor::colorNames();
  qint32 colorOffset = 21;
  for (qint32 i = 0; i < count; ++i)
   {
		alpha = (0*i) + 10.0 + rg.Random(); 
		beta = (0*i) + 1.5 + (0.5*rg.Random()); 
		alphas.push_back(alpha);
		betas.push_back(beta);
		colors.push_back(colorNames[colorOffset++]);
   }

  QVector<QVector<double> > data;
  data.push_back(alphas);
  data.push_back(betas);
  model->setTableData(binNumbers, data, colors);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EquiaxedPreset::generateNeighborPlotData(StatsGenPlotWidget* plot, QVector<double> binNumbers)
{
  // Make sure the distribution is set correctly
  plot->setDistributionType(AIM::Reconstruction::Power, false);
  // This line basically makes sure we have the distribution type we are looking for
  SGPowerLawTableModel* model = qobject_cast<SGPowerLawTableModel*>(plot->tableModel());
  if (NULL == model)
  {
    return;
  }

  qint32 count = binNumbers.count();

  // Remove all the current rows in the table model
  model->removeRows(0, model->rowCount());

  double alpha, k, beta;
      AIMRandomNG rg;
      /* Get a seed value based off the system clock. The issue is that this will
       * be a 64 bit unsigned integer where the high 32 bits will basically not
       * change where as the lower 32 bits will. The following lines of code will
       * pull off the low 32 bits from the number. This operation depends on most
       * significant byte ordering which is different between Big Endian and
       * Little Endian machines. For Big endian machines the Most Significant Byte
       * (MSB) is the first 32 bits. For Little Endian machines the MSB is the
       * second 32 bits.
       */
      unsigned long long int seed = MXA::getMilliSeconds();
      unsigned int* seedPtr = reinterpret_cast<unsigned int*> (&seed);
#if CMP_WORDS_BIGENDIAN
      rg.RandomInit(seedPtr[1]);
#else
      rg.RandomInit(seedPtr[0]);
#endif

  QVector<double> alphas;
  QVector<double> ks;
  QVector<double> betas;
  QVector<QString> colors;
  QStringList colorNames = QColor::colorNames();
  qint32 colorOffset = 21;
  int middlebin = count/2;
  for (qint32 i = 0; i < count; ++i)
   {
		alpha = (4*(binNumbers[i]/binNumbers[middlebin])) + rg.Random(); 
		k = 2 + (0.2*(binNumbers[i]/binNumbers[middlebin])) + (0.05*rg.Random()); 
		beta = (0*i) + 1;
		alphas.push_back(alpha);
		ks.push_back(k);
		betas.push_back(beta);
		colors.push_back(colorNames[colorOffset++]);
   }

  QVector<QVector<double> > data;
  data.push_back(alphas);
  data.push_back(ks);
  data.push_back(betas);
  model->setTableData(binNumbers, data, colors);

}


