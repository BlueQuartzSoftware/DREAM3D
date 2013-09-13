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

#include "RolledPreset.h"

#include "StatsGenerator/Presets/Dialogs/RolledPresetDialog.h"
#include "StatsGenerator/StatsGenPlotWidget.h"
#include "StatsGenerator/StatsGenODFWidget.h"
#include "StatsGenerator/SGAxisODFWidget.h"
#include "StatsGenerator/TableModels/SGLogNormalTableModel.h"
#include "StatsGenerator/TableModels/SGBetaTableModel.h"
#include "StatsGenerator/TableModels/SGPowerLawTableModel.h"
#include "StatsGenerator/TableModels/SGODFTableModel.h"

#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RolledPreset::RolledPreset() :
  m_AspectRatio1(1.0), m_AspectRatio2(1.0)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RolledPreset::~RolledPreset()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RolledPreset::displayUserInputDialog()
{
  RolledPresetDialog d(NULL);
  int ret = d.exec();
  if (ret == QDialog::Accepted)
  {
    // The user clicked the OK button so transfer the values from the dialog into this class
    m_AspectRatio1 = d.getA()/d.getB();
    m_AspectRatio2 = d.getA()/d.getC();
  }
  else
  {
    // Perform any cancellation actions if the user canceled the dialog box
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RolledPreset::initializeOmega3TableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers)
{
  // Make sure the distribution is set correctly
  plot->setDistributionType(DREAM3D::DistributionType::Beta, false);
  // This line basically makes sure we have the distribution type we are looking for
  SGBetaTableModel* model = qobject_cast<SGBetaTableModel*> (plot->tableModel());
  if (NULL == model)
  {
    return;
  }
  qint32 count = binNumbers.count();

  // Remove all the current rows in the table model
  //  model->removeRows(0, model->rowCount());

  float alpha, beta;
  DREAM3D_RANDOMNG_NEW()

      QVector<float> alphas;
  QVector<float> betas;
  QVector<QString> colors;
  QStringList colorNames = QColor::colorNames();
  qint32 colorOffset = 21;
  for (qint32 i = 0; i < count; ++i)
  {
    alpha = (0 * i) + 10.0 + rg.genrand_res53();
    beta = (0 * i) + 1.5 + (0.5 * rg.genrand_res53());
    alphas.push_back(alpha);
    betas.push_back(beta);
    colors.push_back(colorNames[colorOffset++]);
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
void RolledPreset::initializeBOverATableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers)
{
  // Make sure the distribution is set correctly
  plot->setDistributionType(DREAM3D::DistributionType::Beta, false);
  // This line basically makes sure we have the distribution type we are looking for
  SGBetaTableModel* model = qobject_cast<SGBetaTableModel*> (plot->tableModel());
  if (NULL == model)
  {
    return;
  }
  qint32 count = binNumbers.count();

  // Remove all the current rows in the table model
  model->removeRows(0, model->rowCount());

  float alpha, beta;
  DREAM3D_RANDOMNG_NEW()

      QVector<float> alphas;
  QVector<float> betas;
  QVector<QString> colors;
  QStringList colorNames = QColor::colorNames();
  qint32 colorOffset = 21;
  for (qint32 i = 0; i < count; ++i)
  {
    alpha = (0 * i) + (1.1+(28.9*(1.0/m_AspectRatio1))) + (rg.genrand_res53());
    beta = (0 * i) + (30-(28.9*(1.0/m_AspectRatio1))) + (rg.genrand_res53());
    alphas.push_back(alpha);
    betas.push_back(beta);
    colors.push_back(colorNames[colorOffset++]);
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
void RolledPreset::initializeCOverATableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers)
{
  // Make sure the distribution is set correctly
  plot->setDistributionType(DREAM3D::DistributionType::Beta, false);
  // This line basically makes sure we have the distribution type we are looking for
  SGBetaTableModel* model = qobject_cast<SGBetaTableModel*> (plot->tableModel());
  if (NULL == model)
  {
    return;
  }
  qint32 count = binNumbers.count();

  // Remove all the current rows in the table model
  model->removeRows(0, model->rowCount());

  float alpha, beta;
  DREAM3D_RANDOMNG_NEW()

      QVector<float> alphas;
  QVector<float> betas;
  QVector<QString> colors;
  QStringList colorNames = QColor::colorNames();
  qint32 colorOffset = 21;
  for (qint32 i = 0; i < count; ++i)
  {
    alpha = (0 * i) + (1.1+(28.9*(1.0/m_AspectRatio2))) + (rg.genrand_res53());
    beta = (0 * i) + (30-(28.9*(1.0/m_AspectRatio2))) + (rg.genrand_res53());
    alphas.push_back(alpha);
    betas.push_back(beta);
    colors.push_back(colorNames[colorOffset++]);
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
void RolledPreset::initializeNeighborTableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers)
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
    mu = log(8.0 + (1.0*float(i - middlebin)));
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RolledPreset::initializeAxisODFTableModel(SGAxisODFWidget* widget)
{
  // This line basically makes sure we have the distribution type we are looking for
  SGODFTableModel* model = (widget->tableModel());
  if (NULL == model)
  {
    return;
  }
  QVector<float> e1;
  QVector<float> e2;
  QVector<float> e3;
  QVector<float> weights;
  QVector<float> sigmas;

  e1.push_back(0.0f);
  e2.push_back(0.0f);
  e3.push_back(0.0f);
  int spread = int(1.0+(5*powf((1/m_AspectRatio1),1.0)));
  float weight = (46656.0*(1-powf((1/m_AspectRatio1),2.0)));
  if(weight == 0.0) spread = 1;
  int scaler = (8*(spread-1)*(spread-1)*(spread-1));
  if(scaler == 0) scaler = 1;
  weights.push_back(weight/scaler);
  sigmas.push_back(spread);


  model->setTableData(e1, e2, e3, weights, sigmas);
}

