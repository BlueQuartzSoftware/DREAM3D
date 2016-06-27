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

#include "PrimaryRecrystallizedPreset.h"


#include "StatsGenerator/Widgets/Presets/Dialogs/PrimaryRecrystallizedPresetDialog.h"
#include "StatsGenerator/Widgets/StatsGenPlotWidget.h"
#include "StatsGenerator/Widgets/StatsGenODFWidget.h"
#include "StatsGenerator/Widgets/SGAxisODFWidget.h"
#include "StatsGenerator/Widgets/TableModels/SGLogNormalTableModel.h"
#include "StatsGenerator/Widgets/TableModels/SGBetaTableModel.h"
#include "StatsGenerator/Widgets/TableModels/SGPowerLawTableModel.h"
#include "StatsGenerator/Widgets/TableModels/SGODFTableModel.h"

#include "SIMPLib/Utilities/SIMPLibRandom.h"
#include "SIMPLib/Math/SIMPLibMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PrimaryRecrystallizedPreset::PrimaryRecrystallizedPreset()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PrimaryRecrystallizedPreset::~PrimaryRecrystallizedPreset()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryRecrystallizedPreset::displayUserInputDialog()
{
  PrimaryRecrystallizedPresetDialog d(NULL);
  int ret = d.exec();
  if (ret == QDialog::Accepted)
  {
    // The user clicked the OK button so transfer the values from the dialog into this class
    m_PercentRecrystallized = d.getPercentRecrystallized();
  }
  else
  {
    // Perform any cancellation actions if the user canceled the dialog box
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryRecrystallizedPreset::initializeOmega3TableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers)
{
  // Make sure the distribution is set correctly
  plot->setDistributionType(SIMPL::DistributionType::Beta, false);
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
  QVector<QColor> colors = GenerateColors(count, 160, 255);

  for (qint32 i = 0; i < count; ++i)
  {
    alpha = (0 * i) + 10.0 + rg.genrand_res53();
    beta = (0 * i) + 1.5 + (0.5 * rg.genrand_res53());
    alphas.push_back(alpha);
    betas.push_back(beta);
  }

  QVector<QVector<float> > data;
  data.push_back(alphas);
  data.push_back(betas);
  model->setTableData(binNumbers, data, colors);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryRecrystallizedPreset::initializeBOverATableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers)
{
  // Make sure the distribution is set correctly
  plot->setDistributionType(SIMPL::DistributionType::Beta, false);
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
  QVector<QColor> colors = GenerateColors(count, 160, 255);

  for (qint32 i = 0; i < count; ++i)
  {
    alpha = (0 * i) + 10.0 + rg.genrand_res53();
    beta = (0 * i) + 1.5 + (0.5 * rg.genrand_res53());
    alphas.push_back(alpha);
    betas.push_back(beta);
  }

  QVector<QVector<float> > data;
  data.push_back(alphas);
  data.push_back(betas);
  model->setTableData(binNumbers, data, colors);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryRecrystallizedPreset::initializeCOverATableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers)
{
  // Make sure the distribution is set correctly
  plot->setDistributionType(SIMPL::DistributionType::Beta, false);
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
  QVector<QColor> colors = GenerateColors(count, 160, 255);

  for (qint32 i = 0; i < count; ++i)
  {
    alpha = (0 * i) + 10.0 + rg.genrand_res53();
    beta = (0 * i) + 1.5 + (0.5 * rg.genrand_res53());
    alphas.push_back(alpha);
    betas.push_back(beta);
  }

  QVector<QVector<float> > data;
  data.push_back(alphas);
  data.push_back(betas);
  model->setTableData(binNumbers, data, colors);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryRecrystallizedPreset::initializeNeighborTableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers)
{
  // Make sure the distribution is set correctly
  plot->setDistributionType(SIMPL::DistributionType::LogNormal, false);
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
  QVector<QColor> colors = GenerateColors(count, 160, 255);

  int middlebin = count / 2;
  for (qint32 i = 0; i < count; ++i)
  {
    mu = log(14.0 + (2.0 * float(i - middlebin)));
    sigma = 0.3 + (float(middlebin - i) / float(middlebin * 10));
    mus.push_back(mu);
    sigmas.push_back(sigma);
  }

  QVector<QVector<float> > data;
  data.push_back(mus);
  data.push_back(sigmas);
  model->setTableData(binNumbers, data, colors);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryRecrystallizedPreset::initializeClusteringTableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers)
{
  Q_ASSERT(false);
}
