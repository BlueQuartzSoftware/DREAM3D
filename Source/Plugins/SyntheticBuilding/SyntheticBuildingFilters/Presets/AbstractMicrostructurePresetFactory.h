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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>

#include <string>
#include <vector>

#include "AbstractMicrostructurePreset.h"

/**
 * @def DECLARE_FACTORY_CLASS(name, type, display)
 * @brief This macro is used to quickly create the Factory class for a custom
 * preset subclass of AbstractMicrostructurePreset. This must be implemented
 * because of the Factory Design pattern used for each of the presets.
 * @param name The Name of the Factory subclass
 * @param type The name of the AbstractMicrostructurePreset subclass that you are implementing
 * @param display A Human understandable string for your custom preset class.
 *
 * An example would be a programmer that wants to create a custom preset class for
 * cast Magnesium microstructures. If they call their AbstractMicrostructurePreset subclass
 * CastAlMicrostructurePreset then they may want to call this macro in the following way:
 * @code
 * DECLARE_FACTORY_CLASS(CastAlMicrostructurePresetFactory, CastAlMicrostructurePreset, "Cast Al" );
 * @endcode
 */
#define DECLARE_FACTORY_CLASS(name, m_msgType, display)                                                                                                                                                \
  class name : public AbstractMicrostructurePresetFactory                                                                                                                                              \
  {                                                                                                                                                                                                    \
  public:                                                                                                                                                                                              \
    using Self = name;                                                                                                                                                                                 \
    using Pointer = std::shared_ptr<Self>;                                                                                                                                                             \
    using ConstPointer = std::shared_ptr<const Self>;                                                                                                                                                  \
    using WeakPointer = std::weak_ptr<Self>;                                                                                                                                                           \
    using ConstWeakPointer = std::weak_ptr<const Self>;                                                                                                                                                \
    static Pointer NullPointer()                                                                                                                                                                       \
    {                                                                                                                                                                                                  \
      return Pointer(static_cast<Self*>(nullptr));                                                                                                                                                     \
    }                                                                                                                                                                                                  \
    QString getNameOfClass() const override                                                                                                                                                            \
    {                                                                                                                                                                                                  \
      return QString(#name);                                                                                                                                                                           \
    }                                                                                                                                                                                                  \
    static QString ClassName()                                                                                                                                                                         \
    {                                                                                                                                                                                                  \
      return QString(#name);                                                                                                                                                                           \
    }                                                                                                                                                                                                  \
    static Pointer New()                                                                                                                                                                               \
    {                                                                                                                                                                                                  \
      Pointer sharedPtr(new(Self));                                                                                                                                                                    \
      return sharedPtr;                                                                                                                                                                                \
    }                                                                                                                                                                                                  \
    virtual ~name() override = default;                                                                                                                                                                \
    AbstractMicrostructurePreset::Pointer newMicrostructurePreset() override                                                                                                                           \
    {                                                                                                                                                                                                  \
      return m_msgType::New();                                                                                                                                                                         \
    }                                                                                                                                                                                                  \
    bool canCreateClass(const QString& name) override                                                                                                                                                  \
    {                                                                                                                                                                                                  \
      QString cn = this->className();                                                                                                                                                                  \
      QString dn = this->displayName();                                                                                                                                                                \
      return (cn.compare(name) == 0 || dn.compare(name) == 0);                                                                                                                                         \
    }                                                                                                                                                                                                  \
    const QString className() override                                                                                                                                                                 \
    {                                                                                                                                                                                                  \
      return QString(#m_msgType);                                                                                                                                                                      \
    }                                                                                                                                                                                                  \
    const QString displayName() override                                                                                                                                                               \
    {                                                                                                                                                                                                  \
      return QString(display);                                                                                                                                                                         \
    }                                                                                                                                                                                                  \
                                                                                                                                                                                                       \
  protected:                                                                                                                                                                                           \
    name() = default;                                                                                                                                                                                  \
                                                                                                                                                                                                       \
  public:                                                                                                                                                                                              \
    name(const name&) = delete;                                                                                                                                                                        \
    name(name&&) = delete;                                                                                                                                                                             \
    name& operator=(const name&) = delete;                                                                                                                                                             \
    name& operator=(name&&) = delete;                                                                                                                                                                  \
  };

/**
 * @class AbstractMicrostructurePresetFactory AbstractMicrostructurePresetFactory.h StatsGenerator/Presets/AbstractMicrostructurePresetFactory.h
 * @brief This class is the base class for all Preset Factory implementations. In fact
 * the easiest way to implement this class is to use the macro DECLARE_FACTORY_CLASS(...)
 * in the header of your AbstractMicrostructurePreset implementation class. This
 * class defines the minimum set of methods that each implementation requires in
 * order for it to work with the Microstructure Preset System.
 *
 * @date Apr 27, 2011
 * @version 1.0
 */
class AbstractMicrostructurePresetFactory
{
public:
  using Self = AbstractMicrostructurePresetFactory;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer()
  {
    return Pointer(static_cast<Self*>(nullptr));
  }

  virtual QString getNameOfClass() const
  {
    return QString("AbstractMicrostructurePresetFactory");
  }
  static QString ClassName()
  {
    return QString("AbstractMicrostructurePresetFactory");
  }

  using Collection = QVector<Pointer>;

  virtual ~AbstractMicrostructurePresetFactory() = default;

  /**
   * @brief Creates a new AbstractMicrostructurePreset based on a class name
   * @param className The name of the class to create
   * @return Returns an instance of AbstractMicrostructurePreset::Pointer. This method can return
   * a null AbstractMicrostructurePreset so check the return value with the std::shared_ptr.get()
   * method to check the value of the wrapped pointer.
   */
  virtual AbstractMicrostructurePreset::Pointer newMicrostructurePreset() = 0;

  /**
   * @brief Returns true if this class can instantiate a new object of type 'name'
   * @param name The name of the class to check which can also be the 'displayName'
   * @return
   */
  virtual bool canCreateClass(const QString& name) = 0;

  /**
   * @brief Returns the ClassName of the delegate that this factory will create.
   */
  virtual const QString className() = 0;

  /**
   * @brief returns a string value that is appropriate to display to a user through
   * some sort of human interface into the program.
   * @return
   */
  virtual const QString displayName() = 0;

protected:
  AbstractMicrostructurePresetFactory() = default;

public:
  AbstractMicrostructurePresetFactory(const AbstractMicrostructurePresetFactory&) = delete;            // Copy Constructor Not Implemented
  AbstractMicrostructurePresetFactory(AbstractMicrostructurePresetFactory&&) = delete;                 // Move Constructor Not Implemented
  AbstractMicrostructurePresetFactory& operator=(const AbstractMicrostructurePresetFactory&) = delete; // Copy Assignment Not Implemented
  AbstractMicrostructurePresetFactory& operator=(AbstractMicrostructurePresetFactory&&) = delete;      // Move Assignment Not Implemented
};
