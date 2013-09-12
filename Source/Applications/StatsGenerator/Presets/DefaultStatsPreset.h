/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#ifndef DEFAULTSTATSPRESET_H_
#define DEFAULTSTATSPRESET_H_

//-- C++ includes
#include <QtCore/QString>

//-- StatsGen Includes
#include "AbstractMicrostructurePresetFactory.h"
#include "AbstractMicrostructurePreset.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"


/**
 * @class DefaultStatsPreset DefaultStatsPreset.h StatsGenerator/Presets/DefaultStatsPreset.h
 * @brief This class will populate the various tables with the necessary values
 * to create a Generic Microstructure.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date May 23, 2011
 * @version 1.0
 */
class DefaultStatsPreset : public AbstractMicrostructurePreset
{
  public:
    DREAM3D_SHARED_POINTERS(DefaultStatsPreset)
    DREAM3D_STATIC_NEW_SUPERCLASS(AbstractMicrostructurePreset, DefaultStatsPreset)
    virtual ~DefaultStatsPreset();


    void initializeOmega3TableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers);
    void initializeBOverATableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers);
    void initializeCOverATableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers);
    void initializeNeighborTableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers);

  protected:
    DefaultStatsPreset();

  private:
    DefaultStatsPreset(const DefaultStatsPreset&); // Copy Constructor Not Implemented
    void operator=(const DefaultStatsPreset&); // Operator '=' Not Implemented
};

DECLARE_FACTORY_CLASS(DefaultStatsPresetFactory, DefaultStatsPreset, "Default" )


#endif /* DEFAULTSTATSPRESET_H_ */
