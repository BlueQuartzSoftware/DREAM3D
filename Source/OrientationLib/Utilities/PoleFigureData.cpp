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

#include "PoleFigureData.h"

#include "OrientationLib/Utilities/moc_PoleFigureData.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PoleFigureData::PoleFigureData(QVector<float>& xData, QVector<float>& yData, const QString& s, qint32* kernelRad, qint32* size) :
  xData(xData),
  yData(yData),
  label(s)
{
  imageSize[0] = size[0];
  imageSize[1] = size[1];
  kernelRadius[0] = kernelRad[0];
  kernelRadius[1] = kernelRad[1];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PoleFigureData::PoleFigureData(const PoleFigureData& rhs)
{
  this->xData = rhs.xData;
  this->yData = rhs.yData;

  this->label = rhs.label;
  this->imageSize[0] = rhs.imageSize[0];
  this->imageSize[1] = rhs.imageSize[1];
  this->kernelRadius[0] = rhs.kernelRadius[0];
  this->kernelRadius[1] = rhs.kernelRadius[1];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PoleFigureData::~PoleFigureData()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PoleFigureData::operator=(const PoleFigureData& rhs)
{
  this->xData = rhs.xData;
  this->yData = rhs.yData;
  this->label = rhs.label;
  this->imageSize[0] = rhs.imageSize[0];
  this->imageSize[1] = rhs.imageSize[1];
  this->kernelRadius[0] = rhs.kernelRadius[0];
  this->kernelRadius[1] = rhs.kernelRadius[1];
}
