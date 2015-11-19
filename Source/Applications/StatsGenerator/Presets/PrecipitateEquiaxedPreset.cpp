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


#include "PrecipitateEquiaxedPreset.h"



#include "StatsGenerator/Presets/Dialogs/PrecipitateRolledPresetDialog.h"
#include "StatsGenerator/StatsGenPlotWidget.h"
#include "StatsGenerator/StatsGenODFWidget.h"
#include "StatsGenerator/SGAxisODFWidget.h"
#include "StatsGenerator/StatsGenMDFWidget.h"
#include "StatsGenerator/TableModels/SGLogNormalTableModel.h"
#include "StatsGenerator/TableModels/SGBetaTableModel.h"
#include "StatsGenerator/TableModels/SGPowerLawTableModel.h"
#include "StatsGenerator/TableModels/SGODFTableModel.h"

#include "SIMPLib/Utilities/SIMPLibRandom.h"
#include "SIMPLib/Math/SIMPLibMath.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PrecipitateEquiaxedPreset::PrecipitateEquiaxedPreset()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PrecipitateEquiaxedPreset::~PrecipitateEquiaxedPreset()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitateEquiaxedPreset::initializeOmega3TableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers)
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

  float alpha, beta;
  SIMPL_RANDOMNG_NEW()

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
    if (colorOffset == colorNames.size())
    {
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
void PrecipitateEquiaxedPreset::initializeBOverATableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers)
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

  float alpha, beta;
  SIMPL_RANDOMNG_NEW()

  QVector<float> alphas;
  QVector<float> betas;
  QVector<QString> colors;
  QStringList colorNames = QColor::colorNames();
  qint32 colorOffset = 21;
  for (qint32 i = 0; i < count; ++i)
  {
    alpha = (0 * i) + 15.0 + rg.genrand_res53();
    beta = (0 * i) + 1.25 + (0.5 * rg.genrand_res53());
    alphas.push_back(alpha);
    betas.push_back(beta);
    colors.push_back(colorNames[colorOffset++]);
    if (colorOffset == colorNames.size())
    {
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
void PrecipitateEquiaxedPreset::initializeCOverATableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers)
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

  float alpha, beta;
  SIMPL_RANDOMNG_NEW()

  QVector<float> alphas;
  QVector<float> betas;
  QVector<QString> colors;
  QStringList colorNames = QColor::colorNames();
  qint32 colorOffset = 21;
  for (qint32 i = 0; i < count; ++i)
  {
    alpha = (0 * i) + 15.0 + rg.genrand_res53();
    beta = (0 * i) + 1.25 + (0.5 * rg.genrand_res53());
    alphas.push_back(alpha);
    betas.push_back(beta);
    colors.push_back(colorNames[colorOffset++]);
    if (colorOffset == colorNames.size())
    {
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
void PrecipitateEquiaxedPreset::initializeClusteringTableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers)
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
  SIMPL_RANDOMNG_NEW()

  QVector<float> mus;
  QVector<float> sigmas;
  QVector<QString> colors;
  QStringList colorNames = QColor::colorNames();
  qint32 colorOffset = 21;
  int middlebin = count / 2;
  for (qint32 i = 0; i < count; ++i)
  {
    mu = log(14.0 + (2.0 * float(i - middlebin)));
    sigma = 0.3 + (float(middlebin - i) / float(middlebin * 10));
    mus.push_back(mu);
    sigmas.push_back(sigma);
    colors.push_back(colorNames[colorOffset++]);
    if (colorOffset == colorNames.size())
    {
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
void PrecipitateEquiaxedPreset::initializeNeighborTableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers)
{
  Q_ASSERT(false);
}
