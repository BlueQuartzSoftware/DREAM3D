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
#ifndef DATACONTAINERMACROS_H_
#define DATACONTAINERMACROS_H_

/**
 * @brief These are used in the filters to run checks on available arrays
 */

#define TEST_MACRO(NameSpace, DType, Name)\
  std::string t = NameSpace::DType::Name;


#define PF_CHECK_ARRAY_EXISTS( dc, NameSpace, DType, Name, ss, err, ptrType, ArrayType, size)\
  IDataArray::Pointer m_##Name##_Ptr = dc->get##DType(NameSpace::DType::Name);\
  if (NULL == m_##Name##_Ptr.get() ) {\
  ss << #NameSpace << "::" << #DType << "::" <<  #Name << " Array Not initialized at beginning of " << getNameOfClass() << " Filter" << std::endl;\
    setErrorCondition(err);\
  } else if (preflight == false) {\
    m_##Name = dc->get##DType##SizeCheck<ptrType, ArrayType, AbstractFilter>(NameSpace::DType::Name, size, this);\
  }

#define PF_CHECK_ARRAY_EXISTS_SUFFIX( dc, NameSpace, DType, Name, Post, ss, err, ptrType, ArrayType, size)\
  IDataArray::Pointer m_##Name##Post##_Ptr = dc->get##DType(NameSpace::DType::Name);\
  if (NULL == m_##Name##Post##_Ptr.get() ) {\
    ss << #NameSpace << "::" << #DType << "::" <<  #Name << " Array Not initialized at beginning of " << getNameOfClass() << " Filter" << std::endl;\
    setErrorCondition(err);\
  } else if (preflight == false) {\
    m_##Name##Post = dc->get##DType##SizeCheck<ptrType, ArrayType, AbstractFilter>(NameSpace::DType::Name, size, this);\
  }

#define PF_MAKE_SURE_ARRAY_EXISTS(dc, NameSpace, DType, Name, ss, ArrayType, size, NumComp)\
  IDataArray::Pointer m_##Name##_Ptr = dc->get##DType(NameSpace::DType::Name);\
  if (NULL ==  m_##Name##_Ptr.get() ) {\
    ArrayType::Pointer p = ArrayType::CreateArray((size * NumComp));\
    p->SetNumberOfComponents(NumComp);\
    p->SetName(NameSpace::DType::Name);\
    dc->add##DType(NameSpace::DType::Name, p);\
    m_##Name = p->GetPointer(0);\
  }

#define PF_MAKE_SURE_ARRAY_EXISTS_SUFFIX(dc, NameSpace, DType, Name, Post, ss, ArrayType, size, NumComp)\
  IDataArray::Pointer m_##Name##Post##_Ptr = dc->get##DType(NameSpace::DType::Name);\
  if (NULL ==  m_##Name##Post##_Ptr.get() ) {\
    ArrayType::Pointer p = ArrayType::CreateArray((size * NumComp));\
    p->SetNumberOfComponents(NumComp);\
    p->SetName(NameSpace::DType::Name);\
    dc->add##DType(NameSpace::DType::Name, p);\
    m_##Name##Post = p->GetPointer(0);\
  }
/**
 *
 */

#define METHOD_DEF_TEMPLATE_GETARRAYDATA(GetMethod)\
template<typename PtrType, typename DataArrayType, typename Observable>\
PtrType* GetMethod##SizeCheck(const std::string &arrayName, size_t size, Observable* obv)\
{\
PtrType* gi = NULL;\
IDataArray::Pointer iDataArray = GetMethod(arrayName);\
if (iDataArray.get() == 0) {\
  std::stringstream s;\
  s << getNameOfClass() << " - Array " << arrayName << " from the DataContainer class was not in the DataContainer";\
  obv->setErrorCondition(-10);\
  obv->setErrorMessage(s.str());\
  return gi;\
}\
if (size != iDataArray->GetNumberOfTuples()) {\
  std::stringstream s;\
  s << getNameOfClass() << " - Array " << arrayName << " from the DataContainer class did not have the correct number of elements.";\
  s << "Required: " << size << " Contains: " << iDataArray->GetNumberOfTuples();\
  obv->setErrorCondition(-11);\
  obv->setErrorMessage(s.str());\
  return gi;\
}\
gi = IDataArray::SafeReinterpretCast<IDataArray*, DataArrayType*, PtrType* >(iDataArray.get());\
if (NULL == gi) {\
  std::stringstream s;\
  s << getNameOfClass() << " -  Array " << arrayName << " from the DataContainer class could not be cast to correct type.";\
  obv->setErrorCondition(-13);\
  obv->setErrorMessage(s.str());\
  return gi;\
}\
return gi;\
}



#define METHOD_DEF_TEMPLATE_INITIALIZEARRAYDATA(Field)\
template<typename PtrType, typename DataArrayType, typename Observable>\
PtrType* create##Field##Data(const std::string &arrayName, size_t size, int numComp, Observable* obv)\
{\
  PtrType* valuePtr = NULL;\
  IDataArray::Pointer iDataArray = get##Field##Data(arrayName);\
  if (iDataArray.get() == NULL) { \
    iDataArray = DataArrayType::CreateArray(size * NumComp);\
    iDataArray->SetNumberOfComponents(numComp);\
    iDataArray->SetName(arrayName);\
    if (NULL == iDataArray.get()) { \
      std::stringstream s;\
      s << getNameOfClass() << ": Array '" << arrayName << "' could not allocate " << size << " elements.";\
      obv->setErrorCondition(-25);\
      obv->setErrorMessage(s.str());\
      return valuePtr;\
    }\
    add##Field##Data(arrayName, iDataArray);\
  } \
  valuePtr =\
  IDataArray::SafeReinterpretCast<IDataArray*, DataArrayType*, PtrType* >(iDataArray.get());\
  if (NULL == valuePtr) {\
    std::stringstream s;\
    s << getNameOfClass() << ": Array '" << arrayName << "' could not be cast to proper type;";\
    obv->setErrorCondition(-12);\
    obv->setErrorMessage(s.str());\
    return valuePtr;\
  }\
  return valuePtr;\
}


#define GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(dataContainer, field, name, typeClass, type, size, valuePtr) \
type* valuePtr = NULL;\
{\
  IDataArray::Pointer iDataArray = dataContainer->get##field##Data(name);\
  if (iDataArray.get() == NULL) { \
    std::stringstream s;\
    s << getNameOfClass() << ": Array " << name << " from the DataContainer class was not in the DataContainer";\
    setErrorCondition(-10);\
    setErrorMessage(s.str());\
    return -10;\
  } \
  if (static_cast<size_t>(size) != iDataArray->GetNumberOfTuples()) {\
    std::stringstream s;\
    s << getNameOfClass() << ": Array " << name << " from the DataContainer class did not have the correct number of elements.";\
    setErrorCondition(-20);\
    setErrorMessage(s.str());\
    return -20;\
  }\
  valuePtr =\
  IDataArray::SafeReinterpretCast<IDataArray*, typeClass*, type* >(dataContainer->get##field##Data(name).get());\
  if (NULL == valuePtr) {\
    std::stringstream s;\
    s << getNameOfClass() << ": Array " << name << " from the DataContainer class could not be cast to type " << #type;\
    setErrorCondition(-30);\
    setErrorMessage(s.str());\
    return -30;\
  }\
}

#define GET_NAMED_ARRAY_SIZE_CHK_NOMSG(dataContainer, field, name, typeClass, type, size, valuePtr) \
type* valuePtr = NULL;\
{\
  IDataArray::Pointer iDataArray = dataContainer->get##field##Data(name);\
  if (iDataArray.get() == NULL) { \
    return;\
  } \
  if (static_cast<size_t>(size) != iDataArray->GetNumberOfTuples()) {\
    return;\
  }\
  valuePtr =\
  IDataArray::SafeReinterpretCast<IDataArray*, typeClass*, type* >(dataContainer->get##field##Data(name).get());\
  if (NULL == valuePtr) {\
    return;\
  }\
}

#define GET_NAMED_ARRAY_SIZE_CHK_NOMSG_RET(dataContainer, field, name, typeClass, type, size, valuePtr) \
type* valuePtr = NULL;\
{\
  IDataArray::Pointer iDataArray = dataContainer->get##field##Data(name);\
  if (iDataArray.get() == NULL) { \
    return -10;\
  } \
  if (static_cast<size_t>(size) != iDataArray->GetNumberOfTuples()) {\
    return -20;\
  }\
  valuePtr =\
  IDataArray::SafeReinterpretCast<IDataArray*, typeClass*, type* >(dataContainer->get##field##Data(name).get());\
  if (NULL == valuePtr) {\
    return -30;\
  }\
}



#endif /* DATACONTAINERMACROS_H_ */
