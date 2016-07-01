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

#include "MicrostructurePresetManager.h"
#include <iostream>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MicrostructurePresetManager::MicrostructurePresetManager()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MicrostructurePresetManager::~MicrostructurePresetManager()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MicrostructurePresetManager::Pointer MicrostructurePresetManager::instance()
{
  static MicrostructurePresetManager::Pointer singleton;

  if (singleton.get() == NULL)
  {
    singleton.reset (new MicrostructurePresetManager() );
  }
  return singleton;
}

// -----------------------------------------------------------------------------
//  Static Method
// -----------------------------------------------------------------------------
void MicrostructurePresetManager::registerKnownFactories()
{
#if 0
  //Register to be able to import Tiff images
  AbstractMicrostructurePreset::Pointer h5TiffMicrostructurePresetFactory ( new H5TiffMicrostructurePresetFactory() );
  MicrostructurePresetManager::registerFactory(h5TiffMicrostructurePresetFactory);
#endif


}

// -----------------------------------------------------------------------------
//  Static Method
// -----------------------------------------------------------------------------
void MicrostructurePresetManager::registerFactory(AbstractMicrostructurePresetFactory::Pointer importer)
{
  if (NULL != importer.get() )
  {
    // Instantiate the Instance Manager for import delegates
    MicrostructurePresetManager::Pointer idManager = MicrostructurePresetManager::instance();
    idManager->addFactory( importer );
  }
}

// -----------------------------------------------------------------------------
//  Static Method
// -----------------------------------------------------------------------------
AbstractMicrostructurePreset::Pointer MicrostructurePresetManager::createNewPreset(const QString& classname)
{
  MicrostructurePresetManager::Pointer idManager = MicrostructurePresetManager::instance();
  return idManager->createPreset( classname );
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractMicrostructurePreset::Pointer MicrostructurePresetManager::createPreset(const QString& classname)
{
  //std::cout << "MicrostructurePresetManager::newMicrostructurePreset -> Trying to create MicrostructurePreset for '" << classname << "'" << std::endl;
  AbstractMicrostructurePreset::Pointer MicrostructurePreset;
  for (AbstractMicrostructurePresetFactory::Collection::iterator iter = _factories.begin(); iter != _factories.end(); ++iter )
  {
    if ( (*(iter))->canCreateClass(classname) == true)
    {
      return (*(iter)).get()->newMicrostructurePreset();
    }
  }

  return MicrostructurePreset;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructurePresetManager::addFactory(AbstractMicrostructurePresetFactory::Pointer factoryPtr)
{
  this->_factories.push_back(factoryPtr);
}

// -----------------------------------------------------------------------------
// public Method
// -----------------------------------------------------------------------------
AbstractMicrostructurePresetFactory::Pointer MicrostructurePresetManager::getFactory(const QString& classname)
{
  for (AbstractMicrostructurePresetFactory::Collection::iterator iter = _factories.begin(); iter != _factories.end(); ++iter )
  {
    if ( (*(iter))->canCreateClass(classname) == true)
    {
      return *(iter);
    }
  }
  AbstractMicrostructurePresetFactory::Pointer nullPointer;
  return nullPointer;
}

