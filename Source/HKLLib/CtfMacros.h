/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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

#ifndef _HKLMacros.h_
#define _HKLMacros.h_

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
/**
 * @brief Creates a "setter" method to set the property.
 */
#define hklSetMacro_old(type, prpty) \
  void set##prpty(type value) { this->m_##prpty = value; }

#define hklGetMacro_old(type, prpty)\
    type get##prpty() {return m_##prpty; }


#define hklInstanceProperty_old(type, prpty)\
  private:\
      type   m_##prpty;\
  public:\
    hklSetMacro_old(type, prpty)\
    hklGetMacro_old(type, prpty)


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#define hklSetStringMacro_old(prpty, varname) \
  void set##prpty(const std::string &value) { this->m_##prpty = value; }


/**
 * @brief Creates a "getter" method to retrieve the value of the property.
 */
#define hklGetStringMacro_old( prpty, varname) \
  std::string get##prpty() { return varname; }


/**
 * @brief Creates setters and getters in the form of 'setXXX()' and 'getXXX()' methods
 */
#define hklStringProperty_old(prpty)\
  private:\
      std::string   m_##prpty;\
  public:\
    hklGetStringMacro_old(prpty, m_##prpty)\
    hklSetStringMacro_old(prpty, m_##prpty)


#define STATIC_STRING_CONSTANT(str, quoted)\
  static std::string str##() { return std::string(quoted); }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
/**
 * @brief Creates a "setter" method to set the property.
 */
#define hklSetMacro( HeaderType, type, prpty, key) \
  void set##prpty(type value) { \
    HeaderType* p = dynamic_cast<HeaderType*>(m_Headermap[key].get()); \
    if (NULL != p) { p->setValue(value); } else {\
      std::cout << "Value for Key: " << key << " was null." << std::endl;} }

/**
 * @brief Creates a "getter" method to retrieve the value of the property.
 */
#define hklGetMacro(HeaderType, type, prpty, key) \
  type get##prpty() { \
    HeaderType* p = dynamic_cast<HeaderType*>(m_Headermap[key].get());\
    if (NULL != p) { return p->getValue(); } else {\
      std::cout << "Value for Key: " << key << " was null." << std::endl; return 0;} }


#define hklInstanceProperty(HeaderType, type, prpty, key)\
  public:\
    hklSetMacro(HeaderType, type, prpty, key)\
    hklGetMacro(HeaderType, type, prpty, key)

#define SET_POINTER(name, var, type)\
void set##name##Pointer(type* f)\
  {\
    if (m_##var != NULL && m_##var != f)\
    {\
      deallocateArrayData(m_##var);\
      m_##var = NULL;\
    }\
    m_##var = f;\
  }

#endif /* _HKLMacros.h_  */







