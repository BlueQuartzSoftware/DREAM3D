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

#ifndef DEFAULTSTATSPRESET_H_
#define DEFAULTSTATSPRESET_H_

//-- C++ includes
#include <string>

//-- MXA Includes
#include "MXA/Common/MXASetGetMacros.h"

//-- StatsGen Includes
#include "AbstractMicrostructurePresetFactory.h"
#include "AbstractMicrostructurePreset.h"



class DefaultStatsPreset : public AbstractMicrostructurePreset
{
  public:
    MXA_SHARED_POINTERS(DefaultStatsPreset);
    MXA_STATIC_NEW_SUPERCLASS(AbstractMicrostructurePreset, DefaultStatsPreset);
    virtual ~DefaultStatsPreset();


    void generateOmega3Data(StatsGenPlotWidget* plot, QVector<float> binNumbers);
    void generateBOverAPlotData(StatsGenPlotWidget* plot, QVector<float> binNumbers);
    void generateCOverAPlotData(StatsGenPlotWidget* plot, QVector<float> binNumbers);
    void generateCOverBPlotData(StatsGenPlotWidget* plot, QVector<float> binNumbers);
    void generateNeighborPlotData(StatsGenPlotWidget* plot, QVector<float> binNumbers);

  protected:
    DefaultStatsPreset();

  private:
    DefaultStatsPreset(const DefaultStatsPreset&); // Copy Constructor Not Implemented
    void operator=(const DefaultStatsPreset&); // Operator '=' Not Implemented
};

DECLARE_FACTORY_CLASS(DefaultStatsPresetFactory, DefaultStatsPreset, Default );


#endif /* DEFAULTSTATSPRESET_H_ */
