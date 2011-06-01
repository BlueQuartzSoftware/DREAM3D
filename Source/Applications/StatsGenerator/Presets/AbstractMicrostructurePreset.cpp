
#include "AbstractMicrostructurePreset.h"

#include <QtCore/QVector>

#include "StatsGenerator/SGAxisODFWidget.h"
#include "StatsGenerator/StatsGenODFWidget.h"
#include "StatsGenerator/StatsGenMDFWidget.h"
#include "StatsGenerator/TableModels/SGODFTableModel.h"
#include "StatsGenerator/TableModels/SGMDFTableModel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractMicrostructurePreset::AbstractMicrostructurePreset()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractMicrostructurePreset::~AbstractMicrostructurePreset()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractMicrostructurePreset::initializeAxisODFTableModel(SGAxisODFWidget* widget)
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
  model->setTableData(e1, e2, e3, weights, sigmas);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractMicrostructurePreset::initializeODFTableModel(StatsGenODFWidget* widget)
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
  model->setTableData(e1, e2, e3, weights, sigmas);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractMicrostructurePreset::initializeMDFTableModel(StatsGenMDFWidget* widget)
{
  // This line basically makes sure we have the distribution type we are looking for
  SGMDFTableModel* model = (widget->tableModel());
  if (NULL == model)
  {
    return;
  }
  QVector<float> angles;
  QVector<float> axis;
  QVector<float> weights;
  model->setTableData(angles, axis, weights);
}


