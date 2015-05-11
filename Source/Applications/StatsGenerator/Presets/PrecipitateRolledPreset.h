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
#ifndef _PrecipitateRolledPreset_H_
#define _PrecipitateRolledPreset_H_

//-- C++ includes
#include <string>

//-- MXA Includes
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

//-- StatsGen Includes
#include "AbstractMicrostructurePresetFactory.h"
#include "AbstractMicrostructurePreset.h"


/**
 * @class PrecipitateRolledPreset PrecipitateRolledPreset.h StatsGenerator/Presets/PrecipitateRolledPreset.h
 * @brief This class will populate the various tables with the necessary values
 * to create a Rolled Microstructure.
 *
 * @date May 23, 2011
 * @version 1.0
 */
class PrecipitateRolledPreset : public AbstractMicrostructurePreset
{
  public:
    DREAM3D_SHARED_POINTERS(PrecipitateRolledPreset)
    DREAM3D_STATIC_NEW_MACRO(PrecipitateRolledPreset)
    DREAM3D_STATIC_NEW_SUPERCLASS(AbstractMicrostructurePreset, PrecipitateRolledPreset)
    virtual ~PrecipitateRolledPreset();

    void displayUserInputDialog();
    void initializeOmega3TableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers);
    void initializeBOverATableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers);
    void initializeCOverATableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers);
    void initializeClusteringTableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers);
    void initializeNeighborTableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers);

    void initializeAxisODFTableModel(SGAxisODFWidget* widget);


    DREAM3D_INSTANCE_PROPERTY(float, AspectRatio1)
    DREAM3D_INSTANCE_PROPERTY(float, AspectRatio2)

  protected:
    PrecipitateRolledPreset();

  private:
    PrecipitateRolledPreset(const PrecipitateRolledPreset&); // Copy Constructor Not Implemented
    void operator=(const PrecipitateRolledPreset&); // Operator '=' Not Implemented
};

DECLARE_FACTORY_CLASS(PrecipitateRolledPresetFactory, PrecipitateRolledPreset, "Precipitate Rolled")


#endif /* PrecipitateRolledPreset_H_ */

