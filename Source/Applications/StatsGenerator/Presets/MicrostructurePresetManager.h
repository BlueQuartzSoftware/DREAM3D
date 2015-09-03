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


#ifndef _MICROSTRUCTUREPRESETMANAGER_H_
#define _MICROSTRUCTUREPRESETMANAGER_H_

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "AbstractMicrostructurePresetFactory.h"
#include "AbstractMicrostructurePreset.h"


/**
 * @class AbstractMicrostructurePresetFactory AbstractMicrostructurePresetFactory.h StatsGenerator/Presets/AbstractMicrostructurePresetFactory.h
 * @brief This class controls the creation of MicrostructurePreset instances through
 * a Factory design method. Each Type of Preset should have an associated xxxxFactory class
 * and the actual Class that generates the data. Given a class name this class will
 * cycle through all the known factory classes looking for the proper class to instantiate
 * and hand back to the calling method.
 *
 * @date Apr 27, 2011
 * @version 1.0
 */
class MicrostructurePresetManager
{
  public:
    SIMPL_SHARED_POINTERS(MicrostructurePresetManager)
    SIMPL_TYPE_MACRO(MicrostructurePresetManager)

    virtual ~MicrostructurePresetManager();

    // -----------------------------------------------------------------------------
    //  Static Methods
    /**
    * @brief Use this method to get the instance of this class
    * @return Boost Shared pointer to the singleton instance
    */
    static Pointer instance();

    /**
    * @brief This method ensures the ImportDelegateManager is instantiated and
    * then registers built-in import delegates. To date these are:
    *  H5Tiff importer
    *  H5BMP Importer
    */
    static void registerKnownFactories();

    /**
    * @brief This method ensures the ImportDelegateManager is instantiated and
    * then registers the passed in ImportDelegate
    * @param importer A Valid AbstractImportDelegateFactory::Pointer subclass
    */
    static void registerFactory(AbstractMicrostructurePresetFactory::Pointer importer);

    /**
    * @brief Creates a new ImportDelegate based on a class name
    * @param className The name of the class to create
    * @return Returns an instance of MicrostructurePreset::Pointer. This method can return
    * a null ImportDelegate so check the return value with the boost::shared_ptr.get()
    * method to check the value of the wrapped pointer.
    */
    static AbstractMicrostructurePreset::Pointer createNewPreset (const QString& className);

    // -----------------------------------------------------------------------------
    //  Public Methods

    /**
    * @brief Creates a new DataImportDelegate based on a class name. Usually you should
    * use the static method createNewImportDelegate() instead of this although
    * there should not be a problem if you do use this method.
    * @param className The name of the class to create or the display name of the class
    * @return Returns an instance of MicrostructurePreset::Pointer. This method can return
    * a null ImportDelegate so check the return value with the boost::shared_ptr.get()
    * method to check the value of the wrapped pointer.
    */
    virtual AbstractMicrostructurePreset::Pointer createPreset (const QString& className);

    /**
    * @brief Registers a Specific DataImportFactory. Usually you would use the
    * static method registerImportDelegateFactory instead of this method although
    * it is perfectly legal to use this method.
    * @param factoryPtr The ImportDelegateFactory to register
    */
    virtual void addFactory(AbstractMicrostructurePresetFactory::Pointer factoryPtr);

    /**
    * @brief Attempts to return the ImportDelegateFactory for a given class name.
    * @param classname The name of the class to find the factory for OR the display name of the class
    */
    virtual AbstractMicrostructurePresetFactory::Pointer getFactory(const QString& classname);

    virtual AbstractMicrostructurePresetFactory::Collection getFactories() { return _factories; }

  protected:
    MicrostructurePresetManager();

  private:
    AbstractMicrostructurePresetFactory::Collection  _factories;

    MicrostructurePresetManager(const MicrostructurePresetManager&); // Copy Constructor Not Implemented
    void operator=(const MicrostructurePresetManager&); // Operator '=' Not Implemented
};

#endif /* MICROSTRUCTUREPRESETMANAGER_H_ */

