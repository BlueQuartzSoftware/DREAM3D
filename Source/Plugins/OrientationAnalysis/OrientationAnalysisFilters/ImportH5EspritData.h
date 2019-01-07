/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
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
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#pragma once

#include <QtCore/QSharedPointer>

#include "EbsdLib/BrukerNano/EspritPhase.h"

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/SIMPLib.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/ImportH5OimData.h"

class ImportH5EspritDataPrivate;

/**
 * @brief The ImportH5EspritData class. See [Filter documentation](@ref ImportH5EspritData) for details.
 */
class OrientationAnalysis_EXPORT ImportH5EspritData : public ImportH5OimData
{
  Q_OBJECT

  Q_DECLARE_PRIVATE(ImportH5EspritData)

public:
  SIMPL_SHARED_POINTERS(ImportH5EspritData)
  SIMPL_FILTER_NEW_MACRO(ImportH5EspritData)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ImportH5EspritData, ImportH5OimData)

  ~ImportH5EspritData() override;

  SIMPL_PIMPL_PROPERTY_DECL(Esprit_Private_Data, FileCacheData)
  Q_PROPERTY(Esprit_Private_Data FileCacheData READ getFileCacheData WRITE setFileCacheData)

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  const QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  const QUuid getUuid() override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  const QString getHumanLabel() const override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

  /**
   * @brief preflight Reimplemented from @see AbstractFilter class
   */
  void preflight() override;

protected:
  ImportH5EspritData();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief copyRawEbsdData Reads the H5OIM file and puts the data into the data container
   * @param reader H5OIMReader instance pointer
   * @param tDims Tuple dimensions
   * @param cDims Component dimensions
   * @param index Current slice index
   */
  void copyRawEbsdData(EbsdReader* ebsdReader, QVector<size_t>& tDims, QVector<size_t>& cDims, int index) override;

  /**
   * @brief loadMaterialInfo Reads the values for the phase type, crystal structure
   * and precipitate fractions from the EBSD file
   * @param reader H5OIMReader instance pointer
   * @return Integer error value
   */
  int32_t loadMaterialInfo(EbsdReader* ebsdReader) override;

  /**
   * @brief readDataFile Reads the H5OIM file
   * @param reader H5OIMReader instance pointer
   * @param m DataContainer instance pointer
   * @param tDims Tuple dimensions
   */
  void readDataFile(EbsdReader* reader, DataContainer* m, QVector<size_t>& tDims, const QString& scanName, ANG_READ_FLAG flag) override;

  /**
   * @brief readManfacturer
   * @return
   */
  Ebsd::OEM readManufacturer() const override;

  /**
   * @brief dataCheckEdax
   */
  void dataCheckOEM() override;

private:
  DEFINE_DATAARRAY_VARIABLE(float, CellEulerAngles)
  DEFINE_DATAARRAY_VARIABLE(uint8_t, CellPatternData)
  DEFINE_DATAARRAY_VARIABLE(uint32_t, CrystalStructures)
  DEFINE_DATAARRAY_VARIABLE(float, LatticeConstants)

  QScopedPointer<ImportH5EspritDataPrivate> const d_ptr;

public:
  ImportH5EspritData(const ImportH5EspritData&) = delete;            // Copy Constructor Not Implemented
  ImportH5EspritData(ImportH5EspritData&&) = delete;                 // Move Constructor Not Implemented
  ImportH5EspritData& operator=(const ImportH5EspritData&) = delete; // Copy Assignment Not Implemented
  ImportH5EspritData& operator=(ImportH5EspritData&&) = delete;      // Move Assignment Not Implemented
};
