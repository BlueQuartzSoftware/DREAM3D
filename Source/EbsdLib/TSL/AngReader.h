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

#include <QtCore/QByteArray>
#include <QtCore/QFile>
#include <QtCore/QString>

#include <map>

#include "AngConstants.h"
#include "AngHeaderEntry.h"
#include "AngPhase.h"
#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdReader.h"
#include "EbsdLib/EbsdSetGetMacros.h"

/**
 * @class AngReader AngReader.h EbsdLib/TSL/AngReader.h
 * @brief This class is a self contained TSL OIM .ang file reader and will read a
 * single .ang file and store all the data in column centric pointers.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Mar 1, 2010
 * @version 1.0
 */
class EbsdLib_EXPORT AngReader : public EbsdReader
{
public:
  AngReader();
  ~AngReader() override;

  /** @brief Header Values from the TSL ang file */

  EBSDHEADER_INSTANCE_PROPERTY(AngHeaderEntry<float>, float, TEMpixPerum, Ebsd::Ang::TEMPIXPerUM)

  EBSDHEADER_INSTANCE_PROPERTY(AngHeaderEntry<float>, float, XStar, Ebsd::Ang::XStar)

  EBSDHEADER_INSTANCE_PROPERTY(AngHeaderEntry<float>, float, YStar, Ebsd::Ang::YStar)

  EBSDHEADER_INSTANCE_PROPERTY(AngHeaderEntry<float>, float, ZStar, Ebsd::Ang::ZStar)

  EBSDHEADER_INSTANCE_PROPERTY(AngHeaderEntry<float>, float, WorkingDistance, Ebsd::Ang::WorkingDistance)

  EBSDHEADER_INSTANCE_PROPERTY(AngStringHeaderEntry, QString, Grid, Ebsd::Ang::Grid)

  EBSDHEADER_INSTANCE_PROPERTY(AngHeaderEntry<float>, float, XStep, Ebsd::Ang::XStep)

  EBSDHEADER_INSTANCE_PROPERTY(AngHeaderEntry<float>, float, YStep, Ebsd::Ang::YStep)

  EBSDHEADER_INSTANCE_PROPERTY(AngHeaderEntry<int>, int, NumOddCols, Ebsd::Ang::NColsOdd)

  EBSDHEADER_INSTANCE_PROPERTY(AngHeaderEntry<int>, int, NumEvenCols, Ebsd::Ang::NColsEven)

  EBSDHEADER_INSTANCE_PROPERTY(AngHeaderEntry<int>, int, NumRows, Ebsd::Ang::NRows)

  EBSDHEADER_INSTANCE_PROPERTY(AngStringHeaderEntry, QString, OIMOperator, Ebsd::Ang::OPERATOR)

  EBSDHEADER_INSTANCE_PROPERTY(AngStringHeaderEntry, QString, SampleID, Ebsd::Ang::SAMPLEID)

  EBSDHEADER_INSTANCE_PROPERTY(AngStringHeaderEntry, QString, SCANID, Ebsd::Ang::SCANID)

  EBSD_INSTANCE_PROPERTY(QVector<AngPhase::Pointer>, PhaseVector)

  EBSD_INSTANCE_PROPERTY(bool, ReadHexGrid)

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
  EBSD_POINTER_PROPERTY(Phi1, Phi1, float)
  EBSD_POINTER_PROPERTY(Phi, Phi, float)
  EBSD_POINTER_PROPERTY(Phi2, Phi2, float)
  EBSD_POINTER_PROPERTY(XPosition, X, float)
  EBSD_POINTER_PROPERTY(YPosition, Y, float)
  EBSD_POINTER_PROPERTY(ImageQuality, Iq, float)
  EBSD_POINTER_PROPERTY(ConfidenceIndex, Ci, float)
  EBSD_POINTER_PROPERTY(PhaseData, PhaseData, int)
  EBSD_POINTER_PROPERTY(SEMSignal, SEMSignal, float)
  EBSD_POINTER_PROPERTY(Fit, Fit, float)

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
   * @brief Reads the complete TSL .ang file.
   * @return 1 on success
   */
  int readFile() override;

  /**
   * @brief Reads ONLY the header portion of the TSL .ang file
   * @return 1 on success
   */
  int readHeaderOnly() override;

  int getXDimension() override;
  void setXDimension(int xdim) override;
  int getYDimension() override;
  void setYDimension(int ydim) override;

private:
  AngPhase::Pointer m_CurrentPhase;
  int m_ErrorColumn = 0;

  void readData(QFile& in, QByteArray& buf);

  /** @brief Parses the value from a single line of the header section of the TSL .ang file
   * @param line The line to parse
   */
  void parseHeaderLine(QByteArray& buf);

  /** @brief Parses the data from a line of data from the TSL .ang file
   * @param line The line of data to parse
   */
  void parseDataLine(QByteArray& line, size_t i);

public:
  AngReader(const AngReader&) = delete;            // Copy Constructor Not Implemented
  AngReader(AngReader&&) = delete;                 // Move Constructor Not Implemented
  AngReader& operator=(const AngReader&) = delete; // Copy Assignment Not Implemented
  AngReader& operator=(AngReader&&) = delete;      // Move Assignment Not Implemented
};
