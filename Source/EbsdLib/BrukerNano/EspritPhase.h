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

#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QVector>

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdSetGetMacros.h"

class EbsdLib_EXPORT EspritPhase
{
public:
  EBSD_SHARED_POINTERS(EspritPhase)
  EBSD_STATIC_NEW_MACRO(EspritPhase)
  EBSD_TYPE_MACRO(EspritPhase)

  virtual ~EspritPhase();

  EBSD_INSTANCE_PROPERTY(int, PhaseIndex)

  EBSD_INSTANCE_STRING_PROPERTY(Formula)
  EBSD_INSTANCE_PROPERTY(int, IT)
  EBSD_INSTANCE_PROPERTY(QVector<float>, LatticeConstants)
  EBSD_INSTANCE_STRING_PROPERTY(Name)
  EBSD_INSTANCE_PROPERTY(int, Setting)
  EBSD_INSTANCE_STRING_PROPERTY(SpaceGroup)

  QString getMaterialName();

  void parseFormula(QList<QByteArray>& tokens);
  void parseName(QList<QByteArray>& tokens);
  void parseSpaceGroup(QList<QByteArray>& tokens);

  void printSelf(QTextStream& stream);

  /**
   * @brief Returns the type of crystal structure for this phase.
   */
  unsigned int determineLaueGroup();

protected:
  EspritPhase();

public:
  EspritPhase(const EspritPhase&) = delete;            // Copy Constructor Not Implemented
  EspritPhase(EspritPhase&&) = delete;                 // Move Constructor Not Implemented
  EspritPhase& operator=(const EspritPhase&) = delete; // Copy Assignment Not Implemented
  EspritPhase& operator=(EspritPhase&&) = delete;      // Move Assignment Not Implemented
};

struct Esprit_Private_Data
{
  QVector<size_t> dims;
  QVector<float> resolution;
  QVector<float> origin;
  QVector<EspritPhase::Pointer> phases;
  int32_t units;
};

Q_DECLARE_METATYPE(Esprit_Private_Data)
