/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#include "DefaultStatsPreset.h"

#include "StatsGenerator/Presets/Dialogs/RolledPresetDialog.h"
#include "StatsGenerator/StatsGenPlotWidget.h"
#include "StatsGenerator/StatsGenODFWidget.h"
#include "StatsGenerator/SGAxisODFWidget.h"
#include "StatsGenerator/StatsGenMDFWidget.h"
#include "StatsGenerator/TableModels/SGLogNormalTableModel.h"
#include "StatsGenerator/TableModels/SGBetaTableModel.h"
#include "StatsGenerator/TableModels/SGPowerLawTableModel.h"
#include "StatsGenerator/TableModels/SGODFTableModel.h"

#include "DREAM3DLib/Common/DREAM3DRandom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DefaultStatsPreset::DefaultStatsPreset()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DefaultStatsPreset::~DefaultStatsPreset()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DefaultStatsPreset::initializeOmega3TableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers)
{
  // Make sure the distribution is set correctly
  plot->setDistributionType(DREAM3D::DistributionType::Beta, false);
  // This line basically makes sure we have the distribution type we are looking for
  SGBetaTableModel* model = qobject_cast<SGBetaTableModel*>(plot->tableModel());
  if (NULL == model)
  {
    return;
  }
  qint32 count = binNumbers.count();

  // Remove all the current rows in the table model
  //  model->removeRows(0, model->rowCount());

  float alpha = 5.0;
  float beta = 1.0;
  float betaStep = 10.0 / count;

  QVector<float> alphas;
  QVector<float> betas;
  QVector<QString> colors;
  QStringList colorNames = QColor::colorNames();
  qint32 colorOffset = 21;
  for (qint32 i = 0; i < count; ++i)
  {
    alphas.push_back(alpha);
    betas.push_back(beta);
    colors.push_back(colorNames[colorOffset++]);
    if (colorOffset >= colorNames.size())
    {
      colorOffset = 0;
    }
    alpha += 0.1f;
    beta += betaStep;
  }

  QVector<QVector<float> > data;
  data.push_back(alphas);
  data.push_back(betas);
  model->setTableData(binNumbers, data, colors);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DefaultStatsPreset::initializeBOverATableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers)
{
  // Make sure the distribution is set correctly
  plot->setDistributionType(DREAM3D::DistributionType::Beta, false);
  // This line basically makes sure we have the distribution type we are looking for
  SGBetaTableModel* model = qobject_cast<SGBetaTableModel*>(plot->tableModel());
  if (NULL == model)
  {
    return;
  }
  qint32 count = binNumbers.count();

  // Remove all the current rows in the table model
  model->removeRows(0, model->rowCount());

  float alpha = 5.0;
  float beta = 1.0;
  float betaStep = 10.0 / count;

  QVector<float> alphas;
  QVector<float> betas;
  QVector<QString> colors;
  QStringList colorNames = QColor::colorNames();
  qint32 colorOffset = 21;
  for (qint32 i = 0; i < count; ++i)
  {
    alphas.push_back(alpha);
    betas.push_back(beta);
    colors.push_back(colorNames[colorOffset++]);
    alpha += 0.1f;
    beta += betaStep;
    if (colorOffset == colorNames.size()) {
      colorOffset = 21;
    }
  }

  QVector<QVector<float> > data;
  data.push_back(alphas);
  data.push_back(betas);
  model->setTableData(binNumbers, data, colors);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DefaultStatsPreset::initializeCOverATableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers)
{
  // Make sure the distribution is set correctly
  plot->setDistributionType(DREAM3D::DistributionType::Beta, false);
  // This line basically makes sure we have the distribution type we are looking for
  SGBetaTableModel* model = qobject_cast<SGBetaTableModel*>(plot->tableModel());
  if (NULL == model)
  {
    return;
  }
  qint32 count = binNumbers.count();

  // Remove all the current rows in the table model
  model->removeRows(0, model->rowCount());

  float alpha = 5.0;
  float beta = 1.0;
  float betaStep = 10.0 / count;

  QVector<float> alphas;
  QVector<float> betas;
  QVector<QString> colors;
  QStringList colorNames = QColor::colorNames();
  qint32 colorOffset = 21;
  for (qint32 i = 0; i < count; ++i)
  {
    alphas.push_back(alpha);
    betas.push_back(beta);
    colors.push_back(colorNames[colorOffset++]);
    alpha += 0.1f;
    beta += betaStep;
    if (colorOffset == colorNames.size()) {
      colorOffset = 21;
    }
  }

  QVector<QVector<float> > data;
  data.push_back(alphas);
  data.push_back(betas);
  model->setTableData(binNumbers, data, colors);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DefaultStatsPreset::initializeNeighborTableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers)
{
  // Make sure the distribution is set correctly
  plot->setDistributionType(DREAM3D::DistributionType::LogNormal, false);
  // This line basically makes sure we have the distribution type we are looking for
  SGLogNormalTableModel* model = qobject_cast<SGLogNormalTableModel*> (plot->tableModel());
  if (NULL == model)
  {
    return;
  }

  qint32 count = binNumbers.count();

  // Remove all the current rows in the table model
  model->removeRows(0, model->rowCount());

  float mu, sigma;
  DREAM3D_RANDOMNG_NEW()

      QVector<float> mus;
  QVector<float> sigmas;
  QVector<QString> colors;
  QStringList colorNames = QColor::colorNames();
  qint32 colorOffset = 21;
  int middlebin = count / 2;
  for (qint32 i = 0; i < count; ++i)
  {
    mu = log(14.0 + (2.0*float(i - middlebin)));
    sigma = 0.3 + (float(middlebin - i)/float(middlebin*10));
    mus.push_back(mu);
    sigmas.push_back(sigma);
    colors.push_back(colorNames[colorOffset++]);
    if (colorOffset == colorNames.size()) {
      colorOffset = 21;
    }
  }

  QVector<QVector<float> > data;
  data.push_back(mus);
  data.push_back(sigmas);
  model->setTableData(binNumbers, data, colors);

}
