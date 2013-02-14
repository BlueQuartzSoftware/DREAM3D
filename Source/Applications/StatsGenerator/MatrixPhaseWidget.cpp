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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "MatrixPhaseWidget.h"


#include <iostream>
#include <limits>

#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtCore/QVector>
#include <QtCore/QRunnable>
#include <QtCore/QThreadPool>
#include <QtCore/QtConcurrentRun>
#include <QtGui/QMessageBox>


//-- Qwt Includes
#include <qwt.h>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#if QWT_VERSION >= 0x060000
#include <qwt_series_data.h>
#else
#include <qwt_interval_data.h>
#endif
#include <qwt_painter.h>
#include <qwt_scale_map.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/StatsGen.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/StatsDataArray.h"
#include "DREAM3DLib/Common/StatsData.h"
#include "DREAM3DLib/Common/MatrixStatsData.h"


#define CHECK_ERROR_ON_WRITE(var, msg)\
    if (err < 0) {\
      QMessageBox::critical(this, tr("StatsGenerator"),\
      tr("There was an error writing the " msg " to the HDF5 file"),\
      QMessageBox::Ok,\
      QMessageBox::Ok);\
      return err;\
      }


#define CHECK_STATS_READ_ERROR(err, group, dataset)\
if (err < 0) {\
  std::cout << "MatrixPhaseWidget::on_actionOpen_triggered Error: Could not read '" << group << "' data set '" << dataset << "'" << std::endl;\
  std::cout << "  File: " << __FILE__ << std::endl;\
  std::cout << "  Line: " << __LINE__ << std::endl;\
  return err;\
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MatrixPhaseWidget::MatrixPhaseWidget(QWidget *parent) :
SGWidget(parent),
m_PhaseType(DREAM3D::PhaseType::PrimaryPhase),
m_PhaseFraction(1.0),
m_TotalPhaseFraction(1.0),
m_DataHasBeenGenerated(true),
m_PhaseIndex(0),
m_CrystalStructure(Ebsd::CrystalStructure::Cubic),
m_grid(NULL)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MatrixPhaseWidget::~MatrixPhaseWidget()
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatrixPhaseWidget::setupGui()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatrixPhaseWidget::setPhaseIndex(int index)
{
  m_PhaseIndex = index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MatrixPhaseWidget::getPhaseIndex()
{
  return m_PhaseIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatrixPhaseWidget::setCrystalStructure(unsigned int xtal)
{
  m_CrystalStructure = xtal;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int MatrixPhaseWidget::getCrystalStructure()
{
  return m_CrystalStructure;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MatrixPhaseWidget::getComboString()
{
  QString s = QString::number(m_PhaseIndex);
  s.append(" - ");
  if (m_CrystalStructure == Ebsd::CrystalStructure::Cubic)
  {
    s.append("Cubic");
  }
  else if (m_CrystalStructure == Ebsd::CrystalStructure::Hexagonal)
  {
    s.append("Hexagonal");
  }
  return s;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MatrixPhaseWidget::gatherStatsData(VoxelDataContainer::Pointer m)
{
  if (m_PhaseIndex < 1)
  {
    QMessageBox::critical(this, tr("StatsGenerator"),
                                  tr("The Phase Index is Less than 1. This is not allowed."),
                                  QMessageBox::Default);
    return -1;
  }
  int retErr = 0;
 // int err = 0;
  float calcPhaseFraction = m_PhaseFraction / m_TotalPhaseFraction;

  typedef DataArray<unsigned int> XTalStructArrayType;
  typedef DataArray<unsigned int> PhaseTypeArrayType;
  typedef DataArray<unsigned int> ShapeTypeArrayType;
  size_t ensembles = m->getNumEnsembleTuples();

  // Get pointers
  unsigned int* crystalStructures = m->getEnsembleDataSizeCheck<unsigned int, XTalStructArrayType, AbstractFilter>(DREAM3D::EnsembleData::CrystalStructures, ensembles, 1, NULL);
  unsigned int* phaseTypes = m->getEnsembleDataSizeCheck<unsigned int, PhaseTypeArrayType, AbstractFilter>(DREAM3D::EnsembleData::PhaseTypes, ensembles, 1, NULL);

  //unsigned int* shapeTypes = m->getEnsembleDataSizeCheck<unsigned int, ShapeTypeArrayType, AbstractFilter>(DREAM3D::EnsembleData::ShapeTypes, ensembles*1, NULL);

  crystalStructures[m_PhaseIndex] = m_CrystalStructure;
  phaseTypes[m_PhaseIndex] = m_PhaseType;

  StatsDataArray* statsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(m->getEnsembleData(DREAM3D::EnsembleData::Statistics).get());
  StatsData::Pointer statsData = statsDataArray->getStatsData(m_PhaseIndex);
  MatrixStatsData* matrixStatsData = MatrixStatsData::SafePointerDownCast(statsData.get());

  matrixStatsData->setPhaseFraction(calcPhaseFraction);

  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatrixPhaseWidget::extractStatsData(VoxelDataContainer::Pointer m, int index)
{

  setPhaseIndex(index);

  IDataArray* iDataPtr = NULL;

  iDataPtr = m->getEnsembleData(DREAM3D::EnsembleData::CrystalStructures).get();
  UInt32ArrayType* data = UInt32ArrayType::SafeObjectDownCast<IDataArray*, UInt32ArrayType*>(iDataPtr);
  m_CrystalStructure = data->GetValue(index);

  iDataPtr = m->getEnsembleData(DREAM3D::EnsembleData::PhaseTypes).get();
  m_PhaseType = data->GetValue(index);

  iDataPtr = m->getEnsembleData(DREAM3D::EnsembleData::Statistics).get();
  StatsDataArray* statsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(iDataPtr);
  if (statsDataArray == NULL)
  {
    return;
  }
  StatsData::Pointer statsData = statsDataArray->getStatsData(index);
  MatrixStatsData* matrixStatsData = MatrixStatsData::SafePointerDownCast(statsData.get());

  m_PhaseFraction = matrixStatsData->getPhaseFraction();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatrixPhaseWidget::on_m_GenerateDefaultData_clicked()
{
  m_DataHasBeenGenerated = true;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatrixPhaseWidget::dataWasEdited()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MatrixPhaseWidget::getTabTitle()
{
  return QString("Matrix");
}

