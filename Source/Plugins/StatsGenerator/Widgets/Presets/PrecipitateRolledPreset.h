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

#ifndef _precipitaterolledpreset_h_
#define _precipitaterolledpreset_h_

//-- C++ includes
#include <string>

//-- MXA Includes
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

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
    SIMPL_SHARED_POINTERS(PrecipitateRolledPreset)
    SIMPL_STATIC_NEW_MACRO(PrecipitateRolledPreset)
    SIMPL_STATIC_NEW_SUPERCLASS(AbstractMicrostructurePreset, PrecipitateRolledPreset)
    virtual ~PrecipitateRolledPreset();

    void displayUserInputDialog();
    void initializeOmega3TableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers);
    void initializeBOverATableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers);
    void initializeCOverATableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers);
    void initializeClusteringTableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers);
    void initializeNeighborTableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers);

    void initializeAxisODFTableModel(SGAxisODFWidget* widget);


    SIMPL_INSTANCE_PROPERTY(float, AspectRatio1)
    SIMPL_INSTANCE_PROPERTY(float, AspectRatio2)

  protected:
    PrecipitateRolledPreset();

  private:
    PrecipitateRolledPreset(const PrecipitateRolledPreset&); // Copy Constructor Not Implemented
    void operator=(const PrecipitateRolledPreset&); // Operator '=' Not Implemented
};

DECLARE_FACTORY_CLASS(PrecipitateRolledPresetFactory, PrecipitateRolledPreset, "Precipitate Rolled")


#endif /* PrecipitateRolledPreset_H_ */

