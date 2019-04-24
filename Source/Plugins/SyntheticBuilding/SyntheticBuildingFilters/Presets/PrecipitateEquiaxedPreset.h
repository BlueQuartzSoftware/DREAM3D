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

#pragma once

//-- C++ includes
#include <string>

//-- SIMPLib Includes
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

//-- StatsGen Includes
#include "AbstractMicrostructurePreset.h"
#include "AbstractMicrostructurePresetFactory.h"
#include "SyntheticBuilding/SyntheticBuildingDLLExport.h"

/**
 * @class PrecipitateEquiaxedPreset PrecipitateEquiaxedPreset.h StatsGenerator/Presets/PrecipitateEquiaxedPreset.h
 * @brief This class will populate the various tables with the necessary values
 * to create an Equiaxed Microstructure.
 *
 * @date May 23, 2011
 * @version 1.0
 */
class SyntheticBuilding_EXPORT PrecipitateEquiaxedPreset : public AbstractMicrostructurePreset
{
public:
  SIMPL_SHARED_POINTERS(PrecipitateEquiaxedPreset)
  SIMPL_STATIC_NEW_MACRO(PrecipitateEquiaxedPreset)
  SIMPL_STATIC_NEW_SUPERCLASS(AbstractMicrostructurePreset, PrecipitateEquiaxedPreset)

  virtual ~PrecipitateEquiaxedPreset();

  virtual QString getName();

  void initializeOmega3TableModel(QMap<QString, QVector<float>>& data) override;
  void initializeBOverATableModel(QMap<QString, QVector<float>>& data) override;
  void initializeCOverATableModel(QMap<QString, QVector<float>>& data) override;
  void initializeNeighborTableModel(QMap<QString, QVector<float>>& data) override;
  void initializeClusteringTableModel(QMap<QString, QVector<float>>& data) override;

  unsigned int getDistributionType(const QString& distType) override;

protected:
  PrecipitateEquiaxedPreset();

public:
  PrecipitateEquiaxedPreset(const PrecipitateEquiaxedPreset&) = delete; // Copy Constructor Not Implemented
  PrecipitateEquiaxedPreset(PrecipitateEquiaxedPreset&&) = delete;      // Move Constructor Not Implemented
  PrecipitateEquiaxedPreset& operator=(const PrecipitateEquiaxedPreset&) = delete; // Copy Assignment Not Implemented
  PrecipitateEquiaxedPreset& operator=(PrecipitateEquiaxedPreset&&) = delete;      // Move Assignment Not Implemented
};

DECLARE_FACTORY_CLASS(PrecipitateEquiaxedPresetFactory, PrecipitateEquiaxedPreset, "Precipitate Equiaxed")

