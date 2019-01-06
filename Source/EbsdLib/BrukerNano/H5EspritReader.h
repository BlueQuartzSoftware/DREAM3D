/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <hdf5.h>

#include <QtCore/QSet>
#include <QtCore/QString>
#include <QtCore/QVector>

#include "EbsdLib/BrukerNano/EspritConstants.h"
#include "EbsdLib/BrukerNano/EspritPhase.h"
#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdReader.h"
#include "EbsdLib/EbsdSetGetMacros.h"
#include "EbsdLib/TSL/AngHeaderEntry.h"

/**
 * @class H5EspritReader H5EspritReader.h EbsdLib/BrukerNano/H5EspritReader.h
 * @brief
 *
 * After reading the HDF5 file one can use the various methods to extract the phases
 * or other header information or all the columnar data.
 */
class EbsdLib_EXPORT H5EspritReader : public EbsdReader
{
public:
  EBSD_SHARED_POINTERS(H5EspritReader)
  EBSD_STATIC_NEW_MACRO(H5EspritReader)
  EBSD_TYPE_MACRO(H5EspritReader)
  ~H5EspritReader() override;

  /**
   * @brief The HDF5 path to find the EBSD data
   */
  EBSD_INSTANCE_STRING_PROPERTY(HDF5Path)

  EBSD_INSTANCE_PROPERTY(bool, ReadPatternData)

  EBSD_PTR_INSTANCE_PROPERTY(uint8_t*, PatternData)

  EBSD_INSTANCE_2DVECTOR_PROPERTY(int, PatternDims)

  EBSDHEADER_INSTANCE_PROPERTY(AngHeaderEntry<int>, int, NumColumns, Ebsd::H5Esprit::NCOLS)

  EBSDHEADER_INSTANCE_PROPERTY(AngHeaderEntry<int>, int, NumRows, Ebsd::H5Esprit::NROWS)

  EBSDHEADER_INSTANCE_PROPERTY(AngHeaderEntry<double>, double, XStep, Ebsd::H5Esprit::XSTEP)

  EBSDHEADER_INSTANCE_PROPERTY(AngHeaderEntry<double>, double, YStep, Ebsd::H5Esprit::YSTEP)

  EBSDHEADER_INSTANCE_PROPERTY(AngStringHeaderEntry, QString, Grid, Ebsd::H5Esprit::GridType)

  EBSDHEADER_INSTANCE_PROPERTY(AngHeaderEntry<int>, int, PatternWidth, Ebsd::H5Esprit::PatternWidth)

  EBSDHEADER_INSTANCE_PROPERTY(AngHeaderEntry<int>, int, PatternHeight, Ebsd::H5Esprit::PatternHeight)

  /**
   * @brief These methods allow the developer to set/get the raw pointer for a given array, release ownership of the memory
   * and forcibly release the memory for a given array.
   *
   * The methods will follow the form of:
   * @brief This will return the raw pointer to the data. The Reader class WILL 'free' the memory when it goes out of
   * scope. If you need the memory to persist longer then call the release[NAME]Ownership() method to tell the reader
   * NOT to free the memory.
   * @param releaseOwnerhsip If this is true then the internal pointer is set to nullptr and the Cleanup flag is set to false.
   * [type]* get[NAME]Pointer(bool releaseOwnership = false);
   *
   * @brief This will get the ownership of the raw pointer. If 'true' then this class will 'free' the pointer before
   * each read or when the object goes out of scope.
   * bool get[NAME]Ownership();
   *
   * @brief This method will set the internal pointer to nullptr without calling 'free'. It is now up to the developer
   * to 'free' the memory that was used.
   * void release[NAME]Ownership();
   *
   * @brief This will free the internal pointer as long as it already isn't nullptr.
   * void free[NAME]Pointer();
   *
   */
  EBSD_POINTER_PROPERTY(DD, DD, Ebsd::H5Esprit::DD_t)
  EBSD_POINTER_PROPERTY(MAD, MAD, Ebsd::H5Esprit::MAD_t)
  EBSD_POINTER_PROPERTY(MADPhase, MADPhase, Ebsd::H5Esprit::MADPhase_t)
  EBSD_POINTER_PROPERTY(NIndexedBands, NIndexedBands, Ebsd::H5Esprit::NIndexedBands_t)
  EBSD_POINTER_PROPERTY(PCX, PCX, Ebsd::H5Esprit::PCX_t)
  EBSD_POINTER_PROPERTY(PCY, PCY, Ebsd::H5Esprit::PCY_t)
  EBSD_POINTER_PROPERTY(PHI, PHI, Ebsd::H5Esprit::PHI_t)
  EBSD_POINTER_PROPERTY(Phase, Phase, Ebsd::H5Esprit::Phase_t)
  EBSD_POINTER_PROPERTY(RadonBandCount, RadonBandCount, Ebsd::H5Esprit::RadonBandCount_t)
  EBSD_POINTER_PROPERTY(RadonQuality, RadonQuality, Ebsd::H5Esprit::RadonQuality_t)
  EBSD_POINTER_PROPERTY(XBEAM, XBEAM, Ebsd::H5Esprit::XBEAM_t)
  EBSD_POINTER_PROPERTY(YBEAM, YBEAM, Ebsd::H5Esprit::YBEAM_t)
  EBSD_POINTER_PROPERTY(XSAMPLE, XSAMPLE, Ebsd::H5Esprit::XSAMPLE_t)
  EBSD_POINTER_PROPERTY(YSAMPLE, YSAMPLE, Ebsd::H5Esprit::YSAMPLE_t)
  EBSD_POINTER_PROPERTY(phi1, phi1, Ebsd::H5Esprit::phi1_t)
  EBSD_POINTER_PROPERTY(phi2, phi2, Ebsd::H5Esprit::phi2_t)

  /**
   * @brief releaseOwnership
   * @param name
   */
  void releaseOwnership(const QString& name);

  /**
   * @brief Returns the pointer to the data for a given feature
   * @param featureName The name of the feature to return the pointer to.
   */
  void* getPointerByName(const QString& featureName) override;

  /**
   * @brief Returns an enumeration value that depicts the numerical
   * primitive type that the data is stored as (Int, Float, etc).
   * @param featureName The name of the feature.
   */
  Ebsd::NumType getPointerType(const QString& featureName) override;

  /**
   * @brief Reads the file
   * @return error condition
   */
  int readFile() override;

  /**
   * @brief readScanNames
   * @return
   */
  int readScanNames(QStringList& names);

  /**
   * @brief Reads the header section of the file
   * @param Valid HDF5 Group ID
   * @return error condition
   */
  int readHeader(hid_t parId);

  /**
   * @brief Reads ONLY the header portion of the Esprit HDF5 file
   * @return 1 on success
   */
  int readHeaderOnly() override;

  /**
   * @brief Returns a vector of AngPhase objects corresponding to the phases
   * present in the file
   */
  QVector<EspritPhase::Pointer> getPhaseVector() const;

  /**
   * @brief Sets the names of the arrays to read out of the file
   * @param names
   */
  void setArraysToRead(const QSet<QString>& names);

  /**
   * @brief Over rides the setArraysToReads to tell the reader to load ALL the data from the HDF5 file. If the
   * ArrayNames to read is empty and this is true then all arrays will be read.
   * @param b
   */
  void readAllArrays(bool b);

  int getXDimension() override;
  void setXDimension(int xdim) override;
  int getYDimension() override;
  void setYDimension(int ydim) override;

protected:
  H5EspritReader();

  /**
   * @brief Reads the data section of the file
   * @param Valid HDF5 Group ID
   * @return error condition
   */
  int readData(hid_t parId);

  /**
   * @brief sanityCheckForOpening
   * @return
   */
  int sanityCheckForOpening();

private:
  QSet<QString> m_ArrayNames;
  bool m_ReadAllArrays;

  QVector<EspritPhase::Pointer> m_Phases;

public:
  H5EspritReader(const H5EspritReader&) = delete;            // Copy Constructor Not Implemented
  H5EspritReader(H5EspritReader&&) = delete;                 // Move Constructor Not Implemented
  H5EspritReader& operator=(const H5EspritReader&) = delete; // Copy Assignment Not Implemented
  H5EspritReader& operator=(H5EspritReader&&) = delete;      // Move Assignment Not Implemented
};
