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

#include "AbstractMicrostructurePreset.h"

#include <QtCore/QVector>

const QString AbstractMicrostructurePreset::kOmega3Distribution = "Omega3";
const QString AbstractMicrostructurePreset::kBOverADistribution = "BOverA";
const QString AbstractMicrostructurePreset::kCOverADistribution = "COverA";
const QString AbstractMicrostructurePreset::kClusterDistribution = "Clustering";
const QString AbstractMicrostructurePreset::kNeighborDistribution = "Neighbor";
const QString AbstractMicrostructurePreset::kBinNumbers = "BinNumbers";
const QString AbstractMicrostructurePreset::kAlpha = "Alpha";
const QString AbstractMicrostructurePreset::kBeta = "Beta";
const QString AbstractMicrostructurePreset::kColor = "Color";
const QString AbstractMicrostructurePreset::kMu = "Mu";
const QString AbstractMicrostructurePreset::kSigma = "Sigma";
const QString AbstractMicrostructurePreset::kEuler1 = "Euler1";
const QString AbstractMicrostructurePreset::kEuler2 = "Euler2";
const QString AbstractMicrostructurePreset::kEuler3 = "Euler3";
const QString AbstractMicrostructurePreset::kWeight = "Weight";
const QString AbstractMicrostructurePreset::kAngles = "Angles";
const QString AbstractMicrostructurePreset::kAxis = "Axis";

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
QString AbstractMicrostructurePreset::getName()
{
  return QString::fromLatin1("Unknown Preset");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractMicrostructurePreset::initializeAxisODFTableModel(QMap<QString, QVector<float>>& data)
{
  QVector<float> e1;
  QVector<float> e2;
  QVector<float> e3;
  QVector<float> weights;
  QVector<float> sigmas;
  data[kEuler1] = e1;
  data[kEuler2] = e2;
  data[kEuler3] = e3;
  data[kWeight] = weights;
  data[kSigma] = sigmas;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractMicrostructurePreset::initializeODFTableModel(QMap<QString, QVector<float>>& data)
{
  QVector<float> e1;
  QVector<float> e2;
  QVector<float> e3;
  QVector<float> weights;
  QVector<float> sigmas;
  data[kEuler1] = e1;
  data[kEuler2] = e2;
  data[kEuler3] = e3;
  data[kWeight] = weights;
  data[kSigma] = sigmas;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AbstractMicrostructurePreset::initializeMDFTableModel(QMap<QString, QVector<float>>& data)
{
  QVector<float> angles;
  QVector<float> axis;
  QVector<float> weights;
  data[kAngles] = angles;
  data[kAngles] = axis;
  data[kWeight] = weights;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QColor> AbstractMicrostructurePreset::GenerateColors(int count, int saturation, int value)
{
  QVector<QColor> colors(count);

  float increment = 360.0 / count;
  for(int i = 0; i < count; i++)
  {
    QColor c = QColor::fromHsv(i * increment, saturation, value, 255);
    colors[i] = c;
  }
  return colors;
}
