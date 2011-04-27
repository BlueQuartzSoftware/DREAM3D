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

#include "DefaultStatsPreset.h"
#include "StatsGenerator/StatsGenPlotWidget.h"
#include "StatsGenerator/TableModels/SGBetaTableModel.h"
#include "StatsGenerator/TableModels/SGPowerLawTableModel.h"


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
void DefaultStatsPreset::generateOmega3Data(StatsGenPlotWidget* plot, QVector<double> binNumbers)
{
  // Make sure the distribution is set correctly
  plot->setDistributionType(AIM::Reconstruction::Beta);
  // This line basically makes sure we have the distribution type we are looking for
  SGBetaTableModel* model = qobject_cast<SGBetaTableModel*>(plot->tableModel());
  if (NULL == model)
  {
    return;
  }
  qint32 count = binNumbers.count();

  // Remove all the current rows in the table model
  model->removeRows(0, model->rowCount());

  double alpha = 5.0;
  double beta = 1.0;
  double betaStep = 10.0 / count;

  QStringList colorNames = QColor::colorNames();
  qint32 colorOffset = 21;
  qint32 cRow = 0;
  // Now Populate the table data with the data that was passed in
  for (qint32 i = 0; i < count; ++i)
  {
    if (!model->insertRow(model->rowCount())) return;
    cRow = model->rowCount() - 1;
    QModelIndex binNumberIndex = model->index(cRow, SGAbstractTableModel::BinNumber);
    model->setData(binNumberIndex, QVariant(binNumbers[i]), Qt::EditRole);

    QModelIndex alphaIndex = model->index(cRow, SGBetaTableModel::Alpha);
    model->setData(alphaIndex, QVariant(alpha), Qt::EditRole);

    QModelIndex betaIndex = model->index(cRow, SGBetaTableModel::Beta);
    model->setData(betaIndex, QVariant(beta), Qt::EditRole);

    alpha += 0.1;
    beta += betaStep;

    QModelIndex colorIndex = model->index(cRow, SGBetaTableModel::LineColor);
    model->setData(colorIndex, QVariant(colorNames[colorOffset++]), Qt::EditRole);
    if (colorOffset == colorNames.count())
    {
      colorOffset = colorNames.count() - 1;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DefaultStatsPreset::generateBOverAPlotData(StatsGenPlotWidget* plot, QVector<double> binNumbers)
{
  // Make sure the distribution is set correctly
  plot->setDistributionType(AIM::Reconstruction::Beta);
  // This line basically makes sure we have the distribution type we are looking for
  SGBetaTableModel* model = qobject_cast<SGBetaTableModel*>(plot->tableModel());
  if (NULL == model)
  {
    return;
  }
  qint32 count = binNumbers.count();

  // Remove all the current rows in the table model
  model->removeRows(0, model->rowCount());

  double alpha = 5.0;
  double beta = 1.0;
  double betaStep = 10.0 / count;

  QStringList colorNames = QColor::colorNames();
  qint32 colorOffset = 21;
  qint32 cRow = 0;
  // Now Populate the table data with the data that was passed in
  for (qint32 i = 0; i < count; ++i)
  {
    if (!model->insertRow(model->rowCount())) return;
    cRow = model->rowCount() - 1;
    QModelIndex binNumberIndex = model->index(cRow, SGAbstractTableModel::BinNumber);
    model->setData(binNumberIndex, QVariant(binNumbers[i]), Qt::EditRole);

    QModelIndex alphaIndex = model->index(cRow, SGBetaTableModel::Alpha);
    model->setData(alphaIndex, QVariant(alpha), Qt::EditRole);

    QModelIndex betaIndex = model->index(cRow, SGBetaTableModel::Beta);
    model->setData(betaIndex, QVariant(beta), Qt::EditRole);

    alpha += 0.1;
    beta += betaStep;

    QModelIndex colorIndex = model->index(cRow, SGBetaTableModel::LineColor);
    model->setData(colorIndex, QVariant(colorNames[colorOffset++]), Qt::EditRole);
    if (colorOffset == colorNames.count())
    {
      colorOffset = colorNames.count() - 1;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DefaultStatsPreset::generateCOverAPlotData(StatsGenPlotWidget* plot, QVector<double> binNumbers)
{
  // Make sure the distribution is set correctly
  plot->setDistributionType(AIM::Reconstruction::Beta);
  // This line basically makes sure we have the distribution type we are looking for
  SGBetaTableModel* model = qobject_cast<SGBetaTableModel*>(plot->tableModel());
  if (NULL == model)
  {
    return;
  }
  qint32 count = binNumbers.count();

  // Remove all the current rows in the table model
  model->removeRows(0, model->rowCount());

  double alpha = 5.0;
  double beta = 1.0;
  double betaStep = 10.0 / count;

  QStringList colorNames = QColor::colorNames();
  qint32 colorOffset = 21;
  qint32 cRow = 0;
  // Now Populate the table data with the data that was passed in
  for (qint32 i = 0; i < count; ++i)
  {
    if (!model->insertRow(model->rowCount())) return;
    cRow = model->rowCount() - 1;
    QModelIndex binNumberIndex = model->index(cRow, SGAbstractTableModel::BinNumber);
    model->setData(binNumberIndex, QVariant(binNumbers[i]), Qt::EditRole);

    QModelIndex alphaIndex = model->index(cRow, SGBetaTableModel::Alpha);
    model->setData(alphaIndex, QVariant(alpha), Qt::EditRole);

    QModelIndex betaIndex = model->index(cRow, SGBetaTableModel::Beta);
    model->setData(betaIndex, QVariant(beta), Qt::EditRole);

    alpha += 0.1;
    beta += betaStep;

    QModelIndex colorIndex = model->index(cRow, SGBetaTableModel::LineColor);
    model->setData(colorIndex, QVariant(colorNames[colorOffset++]), Qt::EditRole);
    if (colorOffset == colorNames.count())
    {
      colorOffset = colorNames.count() - 1;
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DefaultStatsPreset::generateCOverBPlotData(StatsGenPlotWidget* plot, QVector<double> binNumbers)
{
  // Make sure the distribution is set correctly
  plot->setDistributionType(AIM::Reconstruction::Beta);
  // This line basically makes sure we have the distribution type we are looking for
  SGBetaTableModel* model = qobject_cast<SGBetaTableModel*>(plot->tableModel());
  if (NULL == model)
  {
    return;
  }
  qint32 count = binNumbers.count();

  // Remove all the current rows in the table model
  model->removeRows(0, model->rowCount());

  double alpha = 5.0;
  double beta = 1.0;
  double betaStep = 10.0 / count;

  QStringList colorNames = QColor::colorNames();
  qint32 colorOffset = 21;
  qint32 cRow = 0;
  // Now Populate the table data with the data that was passed in
  for (qint32 i = 0; i < count; ++i)
  {
    if (!model->insertRow(model->rowCount())) return;
    cRow = model->rowCount() - 1;
    QModelIndex binNumberIndex = model->index(cRow, SGAbstractTableModel::BinNumber);
    model->setData(binNumberIndex, QVariant(binNumbers[i]), Qt::EditRole);

    QModelIndex alphaIndex = model->index(cRow, SGBetaTableModel::Alpha);
    model->setData(alphaIndex, QVariant(alpha), Qt::EditRole);

    QModelIndex betaIndex = model->index(cRow, SGBetaTableModel::Beta);
    model->setData(betaIndex, QVariant(beta), Qt::EditRole);

    alpha += 0.1;
    beta += betaStep;

    QModelIndex colorIndex = model->index(cRow, SGBetaTableModel::LineColor);
    model->setData(colorIndex, QVariant(colorNames[colorOffset++]), Qt::EditRole);
    if (colorOffset == colorNames.count())
    {
      colorOffset = colorNames.count() - 1;
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DefaultStatsPreset::generateNeighborPlotData(StatsGenPlotWidget* plot, QVector<double> binNumbers)
{
  // Make sure the distribution is set correctly
  plot->setDistributionType(AIM::Reconstruction::Power);
  // This line basically makes sure we have the distribution type we are looking for
  SGPowerLawTableModel* model = qobject_cast<SGPowerLawTableModel*>(plot->tableModel());
  if (NULL == model)
  {
    return;
  }

  qint32 count = binNumbers.count();

  // Remove all the current rows in the table model
  model->removeRows(0, model->rowCount());

  double alpha = 14.0;
  double k = 1.0;
  double beta = 1.0;

  double alphaStep = 2.0 / (double)(count);
  double kStep = 0.25 / (double)(count);
  double betaStep = 20.0 / (double)(count);

  QStringList colorNames = QColor::colorNames();
  qint32 colorOffset = 21;
  qint32 cRow = 0;
  // Now Populate the table data with the data that was passed in
  for (qint32 i = 0; i < count; ++i)
  {
    if (!model->insertRow(model->rowCount())) return;
    cRow = model->rowCount() - 1;

    QModelIndex binNumberIndex = model->index(cRow, SGAbstractTableModel::BinNumber);
    model->setData(binNumberIndex, QVariant(binNumbers[i]), Qt::EditRole);

    QModelIndex alphaIndex = model->index(cRow, SGPowerLawTableModel::Alpha);
    model->setData(alphaIndex, QVariant(alpha), Qt::EditRole);

    QModelIndex kIndex = model->index(cRow, SGPowerLawTableModel::K);
    model->setData(kIndex, QVariant(k), Qt::EditRole);

    QModelIndex betaIndex = model->index(cRow, SGPowerLawTableModel::Beta);
    model->setData(betaIndex, QVariant(beta), Qt::EditRole);

    alpha += alphaStep;
    k += kStep;
    beta += betaStep;

    QModelIndex colorIndex = model->index(cRow, SGPowerLawTableModel::LineColor);
    model->setData(colorIndex, QVariant(colorNames[colorOffset++]), Qt::EditRole);
    if (colorOffset == colorNames.count())
    {
      colorOffset = colorNames.count() - 1;
    }
  }

}



