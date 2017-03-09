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


#ifndef _primaryequiaxedpreset_h_
#define _primaryequiaxedpreset_h_


//-- C++ includes
#include <string>

//-- MXA Includes
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

//-- StatsGen Includes
#include "AbstractMicrostructurePresetFactory.h"
#include "AbstractMicrostructurePreset.h"



/**
 * @class PrimaryEquiaxedPreset PrimaryEquiaxedPreset.h StatsGenerator/Presets/PrimaryEquiaxedPreset.h
 * @brief This class will populate the various tables with the necessary values
 * to create an Equiaxed Microstructure.
 *
 * @date May 23, 2011
 * @version 1.0
 */
class PrimaryEquiaxedPreset : public AbstractMicrostructurePreset
{
  public:
    SIMPL_SHARED_POINTERS(PrimaryEquiaxedPreset)
    SIMPL_STATIC_NEW_MACRO(PrimaryEquiaxedPreset)
    SIMPL_STATIC_NEW_SUPERCLASS(AbstractMicrostructurePreset, PrimaryEquiaxedPreset)

    virtual ~PrimaryEquiaxedPreset();

    virtual QString getName();

    void initializeOmega3TableModel(QMap<QString, QVector<float>> &data, QVector<QColor> &colors);
    void initializeBOverATableModel(QMap<QString, QVector<float>> &data, QVector<QColor> &colors);
    void initializeCOverATableModel(QMap<QString, QVector<float>> &data, QVector<QColor> &colors);
    void initializeNeighborTableModel(QMap<QString, QVector<float>> &data, QVector<QColor> &colors);
    void initializeClusteringTableModel(QMap<QString, QVector<float>> &data, QVector<QColor> &colors);

    unsigned int getDistributionType(const QString & distType);


  protected:
    PrimaryEquiaxedPreset();

  private:
    PrimaryEquiaxedPreset(const PrimaryEquiaxedPreset&); // Copy Constructor Not Implemented
    void operator=(const PrimaryEquiaxedPreset&); // Operator '=' Not Implemented
};

DECLARE_FACTORY_CLASS(PrimaryEquiaxedPresetFactory, PrimaryEquiaxedPreset, "Primary Equiaxed" )

#endif /* PrimaryEquiaxedPresetFACTORY_H_ */

