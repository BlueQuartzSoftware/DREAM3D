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


#include "MatrixPhaseWidget.h"


#include <iostream>
#include <limits>

#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtCore/QVector>
#include <QtCore/QRunnable>
#include <QtCore/QThreadPool>
#include <QtConcurrent/QtConcurrentRun>
#include <QtWidgets/QMessageBox>


//-- Qwt Includes
#include <qwt.h>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_series_data.h>
#include <qwt_interval.h>
#include <qwt_point_3d.h>
#include <qwt_compat.h>
#include <qwt_painter.h>
#include <qwt_scale_map.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/StatsData/StatsData.h"
#include "SIMPLib/StatsData/MatrixStatsData.h"

#include "OrientationLib/Texture/StatsGen.hpp"


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
    qDebug() << "MatrixPhaseWidget::on_actionOpen_triggered Error: Could not read '" << group << "' data set '" << dataset << "'" << "\n";\
    qDebug() << "  File: " << __FILE__ << "\n";\
    qDebug() << "  Line: " << __LINE__ << "\n";\
    return err;\
  }

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_MatrixPhaseWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MatrixPhaseWidget::MatrixPhaseWidget(QWidget* parent) :
  SGWidget(parent),
  m_PhaseType(DREAM3D::PhaseType::PrimaryPhase),
  m_CrystalStructure(Ebsd::CrystalStructure::Cubic_High),
  m_PhaseIndex(0),
  m_PhaseFraction(1.0),
  m_TotalPhaseFraction(1.0),
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
QString MatrixPhaseWidget::getComboString()
{
  QString s = QString::number(m_PhaseIndex);
  s.append(" - ");
  if ( Ebsd::CrystalStructure::Cubic_High == m_CrystalStructure)
  {
    s.append("Cubic");
  }
  else if ( Ebsd::CrystalStructure::Hexagonal_High == m_CrystalStructure)
  {
    s.append("Hexagonal");
  }
  return s;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MatrixPhaseWidget::gatherStatsData(AttributeMatrix::Pointer attrMat)
{
  if (m_PhaseIndex < 1)
  {
    QMessageBox::critical(this, tr("StatsGenerator"),
                          tr("The Phase Index is Less than 1. This is not allowed."),
                          QMessageBox::Default);
    return -1;
  }
  int retErr = 0;
  float calcPhaseFraction = m_PhaseFraction / m_TotalPhaseFraction;

  //size_t ensembles = attrMat->getNumTuples();

  // Get pointers
  IDataArray::Pointer iDataArray = attrMat->getAttributeArray(DREAM3D::EnsembleData::CrystalStructures);
  unsigned int* crystalStructures = boost::dynamic_pointer_cast< UInt32ArrayType >(iDataArray)->getPointer(0);
  iDataArray = attrMat->getAttributeArray(DREAM3D::EnsembleData::PhaseTypes);
  unsigned int* phaseTypes = boost::dynamic_pointer_cast< UInt32ArrayType >(iDataArray)->getPointer(0);

  crystalStructures[m_PhaseIndex] = m_CrystalStructure;
  phaseTypes[m_PhaseIndex] = m_PhaseType;

  StatsDataArray* statsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(attrMat->getAttributeArray(DREAM3D::EnsembleData::Statistics).get());
  if (NULL != statsDataArray)
  {
    StatsData::Pointer statsData = statsDataArray->getStatsData(m_PhaseIndex);
    MatrixStatsData* matrixStatsData = MatrixStatsData::SafePointerDownCast(statsData.get());
    if(NULL != matrixStatsData)
    {
      matrixStatsData->setPhaseFraction(calcPhaseFraction);
    }
    else
    {
      retErr = -1000;
    }
  }
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatrixPhaseWidget::extractStatsData(AttributeMatrix::Pointer attrMat, int index)
{
  setPhaseIndex(index);

  IDataArray::Pointer iDataArray = attrMat->getAttributeArray(DREAM3D::EnsembleData::CrystalStructures);
  unsigned int* attributeArray = boost::dynamic_pointer_cast< UInt32ArrayType >(iDataArray)->getPointer(0);
  m_CrystalStructure = attributeArray[index];

  iDataArray = attrMat->getAttributeArray(DREAM3D::EnsembleData::PhaseTypes);
  attributeArray = boost::dynamic_pointer_cast< UInt32ArrayType >(iDataArray)->getPointer(0);
  m_PhaseType = attributeArray[index];

  iDataArray = attrMat->getAttributeArray(DREAM3D::EnsembleData::Statistics);
  StatsDataArray* statsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(iDataArray.get());
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

