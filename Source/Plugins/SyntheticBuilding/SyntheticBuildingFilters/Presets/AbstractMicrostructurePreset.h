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

#include <memory>

#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QMap>

#include "SIMPLib/Utilities/ColorTable.h"

#include "SyntheticBuilding/SyntheticBuildingDLLExport.h"

/**
 * @class AbstractMicrostructurePreset AbstractMicrostructurePreset.h StatsGenerator/Presets/AbstractMicrostructurePreset.h
 * @brief This class is the superclass for Microstructure Preset classes. It contains both
 * pure virtual and virtual methods. For those programmers wanting to create new
 * presets to make available to the end user they need to fully implement the methods
 * that they want to setup specific presets for. Each method has a default implementation
 * that does nothing.
 *
 * @date May 23, 2011
 * @version 1.0
 */
class SyntheticBuilding_EXPORT AbstractMicrostructurePreset
{
public:
  using Self = AbstractMicrostructurePreset;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<Self>;
  static Pointer NullPointer();

  /**
   * @brief Returns the name of the class for AbstractMicrostructurePreset
   */
  virtual QString getNameOfClass() const;
  /**
   * @brief Returns the name of the class for AbstractMicrostructurePreset
   */
  static QString ClassName();

  virtual ~AbstractMicrostructurePreset();

  static const QString kOmega3Distribution;
  static const QString kBOverADistribution;
  static const QString kCOverADistribution;
  static const QString kClusterDistribution;
  static const QString kNeighborDistribution;
  static const QString kBinNumbers;
  static const QString kAlpha;
  static const QString kBeta;
  static const QString kColor;
  static const QString kMu;
  static const QString kSigma;
  static const QString kEuler1;
  static const QString kEuler2;
  static const QString kEuler3;
  static const QString kWeight;
  static const QString kAngles;
  static const QString kAxis;

  virtual QString getName();

  /**
   * @brief Initializes the Omega3 preset table values
   * @param tableModel The TableModel that will receive the preset values
   * @param binNumbers The array of Bin values which are needed for the table model
   */
  virtual void initializeOmega3TableModel(QMap<QString, QVector<float>>& data) = 0;

  /**
   * @brief Initializes the B Over A preset table values
   * @param tableModel The TableModel that will receive the preset values
   * @param binNumbers The array of Bin values which are needed for the table model
   */
  virtual void initializeBOverATableModel(QMap<QString, QVector<float>>& data) = 0;

  /**
   * @brief Initializes the C Over A preset table values
   * @param tableModel The TableModel that will receive the preset values
   * @param binNumbers The array of Bin values which are needed for the table model
   */
  virtual void initializeCOverATableModel(QMap<QString, QVector<float>>& data) = 0;

  /**
   * @brief Initializes the Neighbor preset table values
   * @param tableModel The TableModel that will receive the preset values
   * @param binNumbers The array of Bin values which are needed for the table model
   */
  virtual void initializeClusteringTableModel(QMap<QString, QVector<float>>& data) = 0;

  /**
   * @brief Initializes the Clustering preset table values
   * @param tableModel The TableModel that will receive the preset values
   * @param binNumbers The array of Bin values which are needed for the table model
   */
  virtual void initializeNeighborTableModel(QMap<QString, QVector<float>>& data) = 0;

  /**
   * @brief This method initializes the AxisODF Table with preset Euler angles,
   * Weights and Spreads.
   * @param widget Non Null StatsGenAxisODFWidget
   */
  virtual void initializeAxisODFTableModel(QMap<QString, QVector<float>>& data);

  /**
   * @brief This method initializes the ODF Table with preset Euler angles,
   * Weights and Spreads.
   * @param widget
   */
  virtual void initializeODFTableModel(QMap<QString, QVector<float>>& data);

  /**
   * @brief This method initializes the MDF Table with preset Euler angles,
   * Weights and Spreads.
   * @param widget
   */
  virtual void initializeMDFTableModel(QMap<QString, QVector<float>>& data);

  /**
   * @brief getDistributionType
   * @return
   */
  virtual unsigned int getDistributionType(const QString& distType) = 0;

protected:
  AbstractMicrostructurePreset();

public:
  AbstractMicrostructurePreset(const AbstractMicrostructurePreset&) = delete; // Copy Constructor Not Implemented
  AbstractMicrostructurePreset(AbstractMicrostructurePreset&&) = delete;      // Move Constructor Not Implemented
  AbstractMicrostructurePreset& operator=(const AbstractMicrostructurePreset&) = delete; // Copy Assignment Not Implemented
  AbstractMicrostructurePreset& operator=(AbstractMicrostructurePreset&&) = delete;      // Move Assignment Not Implemented

private:
};

