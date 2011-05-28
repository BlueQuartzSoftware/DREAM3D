/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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

#include "TexturePreset.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TexturePreset::TexturePreset()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TexturePreset::~TexturePreset()
{
}

#define ADD_NEW_TEXTURE(name, xtal, e1, e2, e3)\
    textures.push_back(TexturePreset::New(xtal, name, e1, e2, e3));

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<TexturePreset::Pointer> CubicTexturePresets::getTextures()
{
  std::vector<TexturePreset::Pointer> textures;
  ADD_NEW_TEXTURE( "Brass", AIM::Reconstruction::Cubic, 35.0, 45.0, 0.0)
  ADD_NEW_TEXTURE( "S",  AIM::Reconstruction::Cubic, 59.0, 37.0, 63.0)
  ADD_NEW_TEXTURE( "Copper", AIM::Reconstruction::Cubic, 90.0, 35.0, 45.0)
  ADD_NEW_TEXTURE( "S1", AIM::Reconstruction::Cubic, 55.0, 30.0, 65.0)
  ADD_NEW_TEXTURE( "S2",  AIM::Reconstruction::Cubic, 45.0, 35.0, 65.0)
  ADD_NEW_TEXTURE( "Goss",  AIM::Reconstruction::Cubic, 0.0, 45.0, 0.0)
  ADD_NEW_TEXTURE( "Cube",  AIM::Reconstruction::Cubic, 0.0, 0.0, 0.0)
  ADD_NEW_TEXTURE( "RC(rd1)", AIM::Reconstruction::Cubic, 0.0, 20.0, 0.0)
  ADD_NEW_TEXTURE( "RC(rd2)", AIM::Reconstruction::Cubic, 0.0, 35.0, 0.0)
  ADD_NEW_TEXTURE( "RC(nd1)", AIM::Reconstruction::Cubic, 20.0, 0.0, 0.0)
  ADD_NEW_TEXTURE( "RC(nd2)", AIM::Reconstruction::Cubic, 35.0, 0.0, 0.0)
  ADD_NEW_TEXTURE( "P", AIM::Reconstruction::Cubic, 70.0, 45.0, 0.0)
  ADD_NEW_TEXTURE( "Q", AIM::Reconstruction::Cubic, 55.0, 20.0, 0.0)
  ADD_NEW_TEXTURE( "R", AIM::Reconstruction::Cubic, 55.0, 75.0, 25.0)
  return textures;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<TexturePreset::Pointer> HexTexturePresets::getTextures()
{
  std::vector<TexturePreset::Pointer> textures;
//  ADD_NEW_TEXTURE( "Brass", AIM::Reconstruction::Hexagonal, 35.0, 45.0, 0.0)

  return textures;
}
