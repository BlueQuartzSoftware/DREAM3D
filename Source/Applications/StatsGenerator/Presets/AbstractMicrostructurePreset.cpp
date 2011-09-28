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


