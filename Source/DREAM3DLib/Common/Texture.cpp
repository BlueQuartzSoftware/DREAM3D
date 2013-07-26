/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "Texture.h"

#if 0
const size_t Texture::Count = AIM_TEXTURE_COUNT;

const double Texture::Values[AIM_TEXTURE_COUNT][3] =
{
  { 35*M_PI/180.0,45*M_PI/180.0,0*M_PI/180.0},
  { 59*M_PI/180.0,37*M_PI/180.0,63*M_PI/180.0},
  { 90*M_PI/180.0,35*M_PI/180.0,45*M_PI/180.0},
  { 55*M_PI/180.0,30*M_PI/180.0,65*M_PI/180.0},
  { 45*M_PI/180.0,35*M_PI/180.0,65*M_PI/180.0},
  { 0*M_PI/180.0,45*M_PI/180.0,0*M_PI/180.0},
  { 0*M_PI/180.0,0*M_PI/180.0,0*M_PI/180.0},
  { 0*M_PI/180.0,20*M_PI/180.0,0*M_PI/180.0},
  { 0*M_PI/180.0,35*M_PI/180.0,0*M_PI/180.0},
  { 20*M_PI/180.0,0*M_PI/180.0,0*M_PI/180.0},
  { 35*M_PI/180.0,0*M_PI/180.0,0*M_PI/180.0},
  { 70*M_PI/180.0,45*M_PI/180.0,0*M_PI/180.0},
  { 55*M_PI/180.0,20*M_PI/180.0,0*M_PI/180.0},
  { 55*M_PI/180.0,75*M_PI/180.0,25*M_PI/180.0}};

const char* Texture::Names[AIM_TEXTURE_COUNT] =
{
  "Brass",
  "S",
  "Copper",
  "S1",
  "S2",
  "Goss",
  "Cube",
  "RC(rd1)",
  "RC(rd2)",
  "RC(nd1)",
  "RC(nd2)",
  "P",
  "Q",
  "R"};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const double Texture::Weights[AIM_TEXTURE_COUNT] =
{
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0
};

const double Texture::Sigmas[AIM_TEXTURE_COUNT] =
{
    1.0,
    1.0,
    1.0,
    1.0,
    1.0,
    1.0,
    1.0,
    1.0,
    1.0,
    1.0,
    1.0,
    1.0,
    1.0,
    1.0
};
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Texture::Texture()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Texture::~Texture()
{
}
