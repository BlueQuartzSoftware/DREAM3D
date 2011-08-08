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

#ifndef TEXTUREPRESET_H_
#define TEXTUREPRESET_H_

#include <vector>
#include <string>

#include "EbsdLib/EbsdConstants.h"

#include "MXA/Common/MXASetGetMacros.h"

#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/Constants.h"



class DREAM3DLib_EXPORT TexturePreset
{
  public:
    MXA_SHARED_POINTERS(TexturePreset);
    typedef std::vector<Pointer> Container;

    MXA_STATIC_NEW_MACRO(TexturePreset);
    MXA_TYPE_MACRO(TexturePreset);
    static Pointer New(Ebsd::CrystalStructure xtal,
                       const std::string &name,
                       double e1, double e2, double e3)
    {
      Pointer p (new TexturePreset);
      p->setCrystalStructure(xtal);
      p->setName(name);
      p->setEuler1(e1);
      p->setEuler2(e2);
      p->setEuler3(e3);
      return p;
    }

    virtual ~TexturePreset();

    MXA_INSTANCE_PROPERTY(Ebsd::CrystalStructure, CrystalStructure);
    MXA_INSTANCE_STRING_PROPERTY(Name);
    MXA_INSTANCE_PROPERTY(double, Euler1);
    MXA_INSTANCE_PROPERTY(double, Euler2);
    MXA_INSTANCE_PROPERTY(double, Euler3);

  protected:
    TexturePreset();

  private:
    TexturePreset(const TexturePreset&); // Copy Constructor Not Implemented
    void operator=(const TexturePreset&); // Operator '=' Not Implemented
};



class DREAM3DLib_EXPORT CubicTexturePresets
{
  public:
    virtual ~CubicTexturePresets();
    static TexturePreset::Container getTextures();

  protected:
    CubicTexturePresets();

  private:
    CubicTexturePresets(const CubicTexturePresets&); // Copy Constructor Not Implemented
    void operator=(const CubicTexturePresets&); // Operator '=' Not Implemented
};


class DREAM3DLib_EXPORT HexTexturePresets
{
  public:
    virtual ~HexTexturePresets();
    static TexturePreset::Container getTextures();

  protected:
    HexTexturePresets();

  private:
    HexTexturePresets(const HexTexturePresets&); // Copy Constructor Not Implemented
    void operator=(const HexTexturePresets&); // Operator '=' Not Implemented
};

#endif /* TEXTUREPRESET_H_ */
