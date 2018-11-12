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
 * SERVICES; LOSS OF USE, Data, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
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

#include "ComputeGradient.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComputeGradient::ComputeGradient(DoubleArrayType::Pointer input, int width, int height)
: m_Input(input)
, m_Width(width)
, m_Height(height)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComputeGradient::~ComputeGradient() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComputeGradient::compute()
{
  m_GradX = DoubleArrayType::CreateArray(m_Input->getNumberOfTuples(), "X Gradient", true);
  m_GradX->initializeWithZeros();
  m_GradY = DoubleArrayType::CreateArray(m_Input->getNumberOfTuples(), "Y Gradient", true);
  m_GradY->initializeWithZeros();

  double* valPtr = m_Input->getPointer(0);

  size_t gIdx = 0;
  // Compute the X Gradient
  double* g = m_GradX->getPointer(0);
  std::vector<int> h(m_Height);
  int a = {1};
  std::generate(h.begin(), h.end(), [&a] { return a++; });
  // Take forward differences on top and bottom edges
  for(int col = 0; col < m_Width; col++)
  {
    int row = 0;
    gIdx = row * m_Width + col;
    g[gIdx] = (valPtr[(row + 1) * m_Width + col] - valPtr[row * m_Width + col]) / (h[row + 1] - h[row]);
    row = m_Height - 1;
    gIdx = row * m_Width + col;
    g[gIdx] = (valPtr[(row)*m_Width + col] - valPtr[(row - 1) * m_Width + col]) / (h[row] - h[row - 1]);
  }

  // Take centered differences on interior points
  for(int row = 1; row < m_Height - 1; row++)
  {
    for(int col = 0; col < m_Width; col++)
    {
      gIdx = row * m_Width + col;
      g[gIdx] = (valPtr[(row + 1) * m_Width + col] - valPtr[(row - 1) * m_Width + col]) / (h[row + 1] - h[row - 1]);
    }
  }

  // Compute the Y Gradient
  g = m_GradY->getPointer(0);
  h.resize(m_Width);
  a = {1};
  std::generate(h.begin(), h.end(), [&a] { return a++; });
  // Take forward differences on left and right edges
  for(int row = 0; row < m_Height; row++)
  {
    int col = 0;
    // g(:,1) = (f(:,2) - f(:,1))/(h(2)-h(1));
    gIdx = row * m_Width + col;
    g[gIdx] = (valPtr[row * m_Width + col + 1] - valPtr[row * m_Width + col]) / (h[col + 1] - h[col]);
    // g(:,n) = (f(:,n) - f(:,n-1))/(h(end)-h(end-1));
    col = m_Width - 1;
    gIdx = row * m_Width + col;
    g[gIdx] = (valPtr[row * m_Width + col] - valPtr[row * m_Width + col - 1]) / (h[col] - h[col - 1]);
  }

  // Take centered differences on interior points
  for(int row = 0; row < m_Height; row++)
  {
    for(int col = 1; col < m_Width - 1; col++)
    {
      gIdx = row * m_Width + col;
      g[gIdx] = (valPtr[row * m_Width + col + 1] - valPtr[row * m_Width + col - 1]) / (h[col + 1] - h[col - 1]);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DoubleArrayType::Pointer ComputeGradient::getGradY()
{
  return m_GradY;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DoubleArrayType::Pointer ComputeGradient::getGradX()
{
  return m_GradX;
}
