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

#include "CtfFields.h"

#include "CtfConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CtfFields::CtfFields()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CtfFields::~CtfFields()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<QString> CtfFields::getFieldNames()
{
  std::vector<QString> fields;
  fields.push_back(Ebsd::Ctf::Phase);
  fields.push_back(Ebsd::Ctf::X);
  fields.push_back(Ebsd::Ctf::Y);
  fields.push_back(Ebsd::Ctf::Z);
  fields.push_back(Ebsd::Ctf::Bands);
  fields.push_back(Ebsd::Ctf::Error);
  fields.push_back(Ebsd::Ctf::Euler1);
  fields.push_back(Ebsd::Ctf::Euler2);
  fields.push_back(Ebsd::Ctf::Euler3);
  fields.push_back(Ebsd::Ctf::MAD);
  fields.push_back(Ebsd::Ctf::BC);
  fields.push_back(Ebsd::Ctf::BS);
  fields.push_back(Ebsd::Ctf::GrainIndex);
  fields.push_back(Ebsd::Ctf::GrainRandomColourR);
  fields.push_back(Ebsd::Ctf::GrainRandomColourG);
  fields.push_back(Ebsd::Ctf::GrainRandomColourB);

  return fields;
}

