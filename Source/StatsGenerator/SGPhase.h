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

#error This file is no longer used


#ifndef SGPHASE_H_
#define SGPHASE_H_


#include <MXA/Common/MXASetGetMacros.h>

#include "AIM/Common/Constants.h"

#include "StatsGenPlotWidget.h"
#include "StatsGenODFWidget.h"

/**
 * @class SGPhase SGPhase.h AIM/StatsGenerator/SGPhase.h
 * @brief This class holds all the variables that a Phase object needs to know
 * about in order to properly function.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Apr 14, 2011
 * @version 1.0
 */
class SGPhase
{
  public:
    MXA_SHARED_POINTERS(SGPhase)
    MXA_TYPE_MACRO(SGPhase)

    /**
     * @brief Create a new Phase Object that has the index and crystal structure
     * @param index The index of this phase
     * @param xtal The Crystal Structure of this phase
     * @return Shared Pointer.
     */
    static Pointer New(int index, AIM::Reconstruction::CrystalStructure xtal)
    {
      Pointer sharedPtr (new SGPhase);
      sharedPtr->setIndex(index);
      sharedPtr->setCrystalStructure(xtal);
      return sharedPtr;
    }

    virtual ~SGPhase();

    MXA_INSTANCE_PROPERTY(int, Index);
    MXA_INSTANCE_PROPERTY(AIM::Reconstruction::CrystalStructure, CrystalStructure);
    MXA_INSTANCE_PROPERTY(double, Mu);
    MXA_INSTANCE_PROPERTY(double, Sigma);
    MXA_INSTANCE_PROPERTY(double, SigmaCutoff);
    MXA_INSTANCE_PROPERTY(double, BinStepSize);
    MXA_INSTANCE_PROPERTY(AIM::Reconstruction::DistributionType, DistributionType);


    MXA_INSTANCE_PROPERTY(StatsGenPlotWidget*, Omega3Plot)
    MXA_INSTANCE_PROPERTY(StatsGenPlotWidget*, BOverAPlot)
    MXA_INSTANCE_PROPERTY(StatsGenPlotWidget*, COverAPlot)
    MXA_INSTANCE_PROPERTY(StatsGenPlotWidget*, COverBPlot)
    MXA_INSTANCE_PROPERTY(StatsGenPlotWidget*, NeighborPlot)
    MXA_INSTANCE_PROPERTY(StatsGenODFWidget*, ODFWidget)

    QString getComboString();

    /**
     * @brief This will set the parent of each Widget to NULL and then "delete" the widget.
     */
    void deleteWidgets();

    /**
     * @brief Updates all the widgets to have the proper phase index
     * @param index
     */
    void updateWidgetPhase();

  protected:
    SGPhase();

  private:
    SGPhase(const SGPhase&); // Copy Constructor Not Implemented
    void operator=(const SGPhase&); // Operator '=' Not Implemented
};

#endif /* SGPHASE_H_ */
