/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


#ifndef _TEXTUREPRESET_H_
#define _TEXTUREPRESET_H_

#include <QtCore/QVector>
#include <QtCore/QString>

#include "EbsdLib/EbsdConstants.h"



#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/Constants.h"

#include "OrientationLib/OrientationLib.h"


/**
 * @brief The TexturePreset class
 */
class OrientationLib_EXPORT TexturePreset
{
  public:
    SIMPL_SHARED_POINTERS(TexturePreset)
    typedef QVector<Pointer> Container;

    SIMPL_STATIC_NEW_MACRO(TexturePreset)
    SIMPL_TYPE_MACRO(TexturePreset)
    static Pointer New(unsigned int xtal,
                       const QString& name,
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

    SIMPL_INSTANCE_PROPERTY(unsigned int, CrystalStructure)
    SIMPL_INSTANCE_STRING_PROPERTY(Name)
    SIMPL_INSTANCE_PROPERTY(double, Euler1)
    SIMPL_INSTANCE_PROPERTY(double, Euler2)
    SIMPL_INSTANCE_PROPERTY(double, Euler3)

  protected:
    TexturePreset();

  private:
    TexturePreset(const TexturePreset&); // Copy Constructor Not Implemented
    void operator=(const TexturePreset&); // Operator '=' Not Implemented
};


/**
 * @brief The CubicTexturePresets class
 */
class OrientationLib_EXPORT CubicTexturePresets
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

/**
 * @brief The HexTexturePresets class
 */
class OrientationLib_EXPORT HexTexturePresets
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

