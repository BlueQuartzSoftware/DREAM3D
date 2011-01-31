/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "Texture.h"

const size_t Texture::Count = AIM_TEXTURE_COUNT;

const double Texture::Values[AIM_TEXTURE_COUNT][3] =
{
  { 0.610865,0.785398,0.0},
  { 1.029744,0.645772,1.099557},
  { 1.570796,0.610865,0.785398},
  { 1.029744,0.506145,1.099557},
 // { 0.820305,0.645772,1.099557},
  { 0.0,0.610865,0.785398},
  { 0.0,0.785398,0.0},
  { 0.0,0.0,0.0},
  { 0.0,0.349066,0.0},
  { 0.0,0.610865,0.0},
  { 0.349066,0.0,0.0},
  { 0.610865,0.0,0.0},
  { 1.22173,0.785398,0.0},
  { 0.959931,0.349066,0.0},
  { 0.959931,1.308997,0.436332}};

const char* Texture::Names[AIM_TEXTURE_COUNT] =
{
  "Brass",
  "S",
  "Copper",
  "S1",
  "S2",
 // "S3",
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
  // TODO Auto-generated destructor stub
}
