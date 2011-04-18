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

#include "SGPhase.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGPhase::SGPhase()
{
  // TODO Auto-generated constructor stub

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGPhase::~SGPhase()
{
  // TODO Auto-generated destructor stub
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SGPhase::getComboString()
{
  QString s = QString::number(m_Index);
  s.append(" - ");
  if (m_CrystalStructure == AIM::Reconstruction::Cubic)
  {
    s.append("Cubic");
  }
  else if (m_CrystalStructure == AIM::Reconstruction::Hexagonal)
  {
    s.append("Hexagonal");
  }
  return s;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGPhase::deleteWidgets()
{
  m_Omega3Plot->setParent(NULL); delete m_Omega3Plot;
  m_BOverAPlot->setParent(NULL); delete m_BOverAPlot;
  m_COverAPlot->setParent(NULL); delete m_COverAPlot;
  m_COverBPlot->setParent(NULL); delete m_COverBPlot;
  m_NeighborPlot->setParent(NULL); delete m_NeighborPlot;
  m_ODFWidget->setParent(NULL); delete m_ODFWidget;


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGPhase::updateWidgetPhase()
{
  m_Omega3Plot->setPhase(m_Index);
  m_BOverAPlot->setPhase(m_Index);
  m_COverAPlot->setPhase(m_Index);
  m_COverBPlot->setPhase(m_Index);
  m_NeighborPlot->setPhase(m_Index);
  m_ODFWidget->setPhase(m_Index);
}

