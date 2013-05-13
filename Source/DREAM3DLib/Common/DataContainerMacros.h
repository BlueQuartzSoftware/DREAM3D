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
#ifndef _DATACONTAINERMACROS_H_
#define _DATACONTAINERMACROS_H_


#define TEST_PREREQ_DATA( dc, NameSpace, DType, Name, errVariable, errCode, ptrType, ArrayType, size, NumComp)\
  {if (m_##Name##ArrayName.empty() == true){ \
    setErrorCondition(errCode##000);\
    std::stringstream _##Name##_ss;\
    _##Name##_ss << "The name of the array for the " << #NameSpace << #DType << #Name << " was empty. Please provide a name for this array" << std::endl;\
    addErrorMessage(getHumanLabel(), _##Name##_ss.str(), errCode##000);\
  }\
  m_##Name = dc->get##DType##SizeCheck<ptrType, ArrayType, AbstractFilter>(m_##Name##ArrayName, size, NumComp, NULL);\
  if (NULL == m_##Name ) {\
    errVariable = errCode;\
  }}



/**
 * @brief These are used in the filters to run checks on available arrays
 */
#define GET_PREREQ_DATA( dc, NameSpace, DType, Name, ss, err, ptrType, ArrayType, size, NumComp)\
  {if (m_##Name##ArrayName.empty() == true){ \
    setErrorCondition(err##000);\
    ss << "The name of the array for the " << #NameSpace << "::" << #DType << "::" << #Name << " was empty. Please provide a name for this array" << std::endl;\
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());\
  }\
  if (dc->does##DType##Exist(m_##Name##ArrayName) == false) {\
    setErrorCondition(err##001);\
    ss.str("");\
    ss << "An array with name '" << m_##Name##ArrayName << "'' in the " << #DType << " grouping does not exist and is required for this filter to execute." << std::endl;\
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());\
  }\
  else { \
  std::string _s(#Name); \
  /* addRequired##DType(_s);*/\
  m_##Name = dc->get##DType##SizeCheck<ptrType, ArrayType, AbstractFilter>(m_##Name##ArrayName, size, NumComp, this);\
  if (NULL == m_##Name ) {\
    ss << "\nThe current array with name '" << m_##Name##ArrayName << "' is not valid for the internal array named '" << NameSpace::DType::Name  << "' for this filter."\
    << "The preflight failed for one or more reasons. Check additional error messages for more details." << std::endl;\
    setErrorCondition(err##002);\
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition()); \
  }}}

#define CREATE_NON_PREREQ_DATA(dc, NameSpace, DType, Name, ss, ptrType, ArrayType, initValue, size, NumComp)\
  {\
  if (m_##Name##ArrayName.empty() == true)\
  {\
    setErrorCondition(-10000);\
    ss.str(""); ss << "The name of the array for the " << #NameSpace << #DType << #Name << " was empty. Please provide a name for this array/" << std::endl; \
    addErrorMessage(getHumanLabel(), ss.str(), -10000);\
  }\
  std::string _s(#Name);\
  m_##Name = dc->get##DType##SizeCheck<ptrType, ArrayType, AbstractFilter>(m_##Name##ArrayName, size, NumComp, NULL);\
  if (NULL ==  m_##Name ) \
  {\
    ArrayType::Pointer p = ArrayType::CreateArray((size * NumComp), m_##Name##ArrayName);\
    if (NULL == p.get()) \
    {\
      ss.str(""); ss << "Filter " << getNameOfClass() << " attempted to create array "; \
      if (m_##Name##ArrayName.empty() == true) \
      {\
        ss << " with an empty name and that is not allowed." << std::endl;;\
        setErrorCondition(-501);\
      } else {\
        ss << "'" << m_##Name##ArrayName << "' but was unsuccessful. This is most likely due to not enough contiguous memory." << std::endl;\
        setErrorCondition(-500);\
      }\
      addErrorMessage(getHumanLabel(), ss.str(), -50001);\
    } \
    else if (p->GetPointer(0) == NULL)\
    {\
      setErrorCondition(-11000);\
      ss.str("");\
      ss << "'" << m_##Name##ArrayName << "' was sized to Zero which may cause the program to crash in filters that use this array, including the current filter."\
      << " Please add a filter before this filter that will result in a properly sized array.";\
      addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());\
    }\
    else {\
      p->initializeWithValues(initValue);\
      p->SetNumberOfComponents(NumComp);\
      p->SetName(m_##Name##ArrayName);\
      dc->add##DType(m_##Name##ArrayName, p);\
      m_##Name = p->GetPointer(0);\
      CreatedArrayHelpIndexEntry::Pointer e = CreatedArrayHelpIndexEntry::New();\
      e->setFilterName(this->getNameOfClass());\
      e->setFilterHumanLabel(this->getHumanLabel());\
      e->setFilterGroup(this->getGroupName());\
      e->setFilterSubGroup(this->getSubGroupName());\
      e->setArrayDefaultName(m_##Name##ArrayName);\
      e->setArrayGroup(#DType);\
      e->setArrayNumComponents(NumComp);\
      e->setArrayType(#ArrayType);\
      addCreatedArrayHelpIndexEntry(e);\
    }\
  }\
}


/**
 *@brief
 */
#define ADD_HELP_INDEX_ENTRY(DType, Name, ArrayType, NumComp)\
CreatedArrayHelpIndexEntry::Pointer e = CreatedArrayHelpIndexEntry::New();\
e->setFilterName(this->getNameOfClass());\
e->setFilterHumanLabel(this->getHumanLabel());\
e->setFilterGroup(this->getGroupName());\
e->setFilterSubGroup(this->getSubGroupName());\
e->setArrayDefaultName(m_##Name##ArrayName);\
e->setArrayGroup(#DType);\
e->setArrayNumComponents(NumComp);\
e->setArrayType(#ArrayType);\
addCreatedArrayHelpIndexEntry(e);



#define METHOD_DEF_TEMPLATE_GETARRAYDATA(GetMethod)\
template<typename PtrType, typename DataArrayType, typename AbstractFilter>\
PtrType* GetMethod##SizeCheck(const std::string &arrayName, size_t size, int numComp, AbstractFilter* obv)\
{\
PtrType* gi = NULL;\
IDataArray::Pointer iDataArray = GetMethod(arrayName);\
if (iDataArray.get() == 0) {\
  return gi;\
}\
if (size*numComp != iDataArray->GetSize()) {\
  std::stringstream s;\
  s << " - Array '" << arrayName << "' from the DataContainer class did not have the required number of elements.";\
  s << " Required: " << (size*numComp) << " Contains: " << iDataArray->GetSize();\
  if (NULL != obv) {obv->setErrorCondition(-501);\
  obv->addErrorMessage(obv->getHumanLabel(), s.str(), -501);}\
  return gi;\
}\
if (numComp != iDataArray->GetNumberOfComponents()) {\
      if (NULL != obv) {std::stringstream ss;\
      ss << "\nFilter " << obv->getHumanLabel() << " requires an array where the number of components is " << numComp << " but the array"\
      << " that was supplied has " << iDataArray->GetNumberOfComponents() << "." << std::endl;\
      obv->addErrorMessage(obv->getHumanLabel(), ss.str(),503);}\
      return gi;\
}\
  gi = IDataArray::SafeReinterpretCast<IDataArray*, DataArrayType*, PtrType* >(iDataArray.get());\
  if (NULL == gi) {\
    std::stringstream s;\
    s << " -  Array '" << arrayName << "' stored in the DataContainer could not be cast to correct type. For example selecting an array with floating point values for an array that needs integer values";\
    if (NULL != obv) {obv->setErrorCondition(-502);\
    obv->addErrorMessage(obv->getHumanLabel(), s.str(), -502);}\
    return gi;\
  }\
return gi;\
}



#define METHOD_DEF_TEMPLATE_INITIALIZEARRAYDATA(Field)\
template<typename PtrType, typename DataArrayType, typename AbstractFilter>\
PtrType* create##Field##Data(const std::string &arrayName, size_t size, int numComp, AbstractFilter* obv)\
{\
  PtrType* valuePtr = NULL;\
  IDataArray::Pointer iDataArray = get##Field##Data(arrayName);\
  if (iDataArray.get() == NULL) { \
    iDataArray = DataArrayType::CreateArray(size * numComp, arrayName);\
    iDataArray->initializeWithZeros();\
    iDataArray->SetNumberOfComponents(numComp);\
    if (NULL == iDataArray.get()) { \
      std::stringstream s;\
      s << ": Array '" << arrayName << "' could not allocate " << size << " elements.";\
      if (NULL != obv) {obv->setErrorCondition(-25);\
      obv->addErrorMessage(getNameOfClass(), s.str(), -25);}\
      return valuePtr;\
    }\
    add##Field##Data(arrayName, iDataArray);\
  } \
  valuePtr =\
  IDataArray::SafeReinterpretCast<IDataArray*, DataArrayType*, PtrType* >(iDataArray.get());\
  if (NULL == valuePtr) {\
    std::stringstream s;\
    s << ": Array '" << arrayName << "' could not be cast to proper type;";\
    if (NULL != obv) {obv->setErrorCondition(-12);\
    obv->addErrorMessage(getNameOfClass(), s.str(), -12);}\
    return valuePtr;\
  }\
  return valuePtr;\
}


#define GET_NAMED_ARRAY_SIZE_CHK_RETVALUE(dataContainer, field, name, typeClass, m_msgType, size, valuePtr) \
m_msgType* valuePtr = NULL;\
{\
  IDataArray::Pointer iDataArray = dataContainer->get##field##Data(name);\
  if (iDataArray.get() == NULL) { \
    std::stringstream s;\
    s << ": Array " << name << " from the DataContainer class was not in the DataContainer";\
    setErrorCondition(-10);\
    addErrorMessage(getNameOfClass(), s.str(), -10);\
    return -10;\
  } \
  if (static_cast<size_t>(size) != iDataArray->GetSize()) {\
    std::stringstream s;\
    s << ": Array " << name << " from the DataContainer class did not have the correct number of elements.";\
    setErrorCondition(-20);\
    addErrorMessage(getNameOfClass(), s.str(), -20);\
    return -20;\
  }\
  valuePtr =\
  IDataArray::SafeReinterpretCast<IDataArray*, typeClass*, m_msgType* >(dataContainer->get##field##Data(name).get());\
  if (NULL == valuePtr) {\
    std::stringstream s;\
    s << ": Array " << name << " from the DataContainer class could not be cast to type " << #m_msgType;\
    setErrorCondition(-30);\
    addErrorMessage(getNameOfClass(), s.str(), -30);\
    return -30;\
  }\
}

#define GET_NAMED_ARRAY_SIZE_CHK_NOMSG(dataContainer, field, name, typeClass, m_msgType, size, valuePtr) \
m_msgType* valuePtr = NULL;\
{\
  IDataArray::Pointer iDataArray = dataContainer->get##field##Data(name);\
  if (iDataArray.get() == NULL) { \
    return;\
  } \
  if (static_cast<size_t>(size) != iDataArray->GetSize()) {\
    return;\
  }\
  valuePtr =\
  IDataArray::SafeReinterpretCast<IDataArray*, typeClass*, m_msgType* >(dataContainer->get##field##Data(name).get());\
  if (NULL == valuePtr) {\
    return;\
  }\
}

#define GET_NAMED_ARRAY_SIZE_CHK_NOMSG_RET(dataContainer, field, name, typeClass, m_msgType, size, valuePtr) \
m_msgType* valuePtr = NULL;\
{\
  IDataArray::Pointer iDataArray = dataContainer->get##field##Data(name);\
  if (iDataArray.get() == NULL) { \
    return -10;\
  } \
  if (static_cast<size_t>(size) != iDataArray->GetSize()) {\
    std::cout << name << " Size did not match." << size << " vs " << iDataArray->GetNumberOfTuples() << std::endl;\
    return -20;\
  }\
  valuePtr =\
  IDataArray::SafeReinterpretCast<IDataArray*, typeClass*, m_msgType* >(dataContainer->get##field##Data(name).get());\
  if (NULL == valuePtr) {\
    return -30;\
  }\
}



#define DOES_DATASET_EXIST_DECL(DType)\
virtual bool does##DType##Exist(const std::string &name);

#define DOES_DATASET_EXIST_DEFN(Class, DType)\
bool Class::does##DType##Exist(const std::string &name) {\
  std::map<std::string, IDataArray::Pointer>::iterator iter = m_##DType.find(name);\
  return ( iter != m_##DType.end());\
}

#endif /* _DATACONTAINERMACROS_H_ */


/*
   // Cell Data

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, AlreadyChecked, ss, -300, bool, BoolArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, GoodVoxels, ss, -300, bool, BoolArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, -300, float, FloatArrayType, voxels, 5)
  GET_PREREQ_DATA(m, DREAM3D, CellData, SurfaceVoxels, ss, -301, int8_t, Int8ArrayType, voxels)
  GET_PREREQ_DATA(m, DREAM3D, CellData, KernelAverageMisorientations, ss, -300, float, FloatArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainMisorientations, ss, -300, float, FloatArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, MisorientationGradients, ss, -300, float, FloatArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, NearestNeighbors, ss, -300, int32_t, Int32ArrayType, voxels, 3)
  GET_PREREQ_DATA(m, DREAM3D, CellData, NearestNeighborDistances, ss, -300, float, FloatArrayType, voxels, 3)
  GET_PREREQ_DATA(m, DREAM3D, CellData, Neighbors, ss, -300, int32_t, Int32ArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, Phases, C, ss, -300, int32_t, Int32ArrayType,  voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, ss, -300, float, FloatArrayType,  voxels, 3)


  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, int32_t, Int32ArrayType, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, float, FloatArrayType, fields, 5)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, int32_t, Int32ArrayType, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, SurfaceVoxels, ss, int8_t, Int8ArrayType, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Neighbors, ss, int32_t, Int32ArrayType, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, AlreadyChecked, ss, bool, BoolArrayType, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Phases, C, ss, int32_t, Int32ArrayType, voxels, 1)
  CREATE_NON_PREREQ_DATA( m, DREAM3D, CellData, CellEulerAngles, ss, float, FloatArrayType, voxels, 3)


  // Field Data

  GET_PREREQ_DATA(m, DREAM3D, FieldData, Phases, F, ss, -303,  int32_t, Int32ArrayType, fields, 1)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, -304, bool, BoolArrayType, fields, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, -300, float, FloatArrayType, voxels, 5)
  GET_PREREQ_DATA(m, DREAM3D, CellData, EulerAngles, ss, -304, float, FloatArrayType, voxels, 3)
  GET_PREREQ_DATA(m, DREAM3D, CellData, GoodVoxels, ss, -304, bool, BoolArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, NumNeighbors, ss, -306, int32_t, Int32ArrayType, fields, 1)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -301, float, FloatArrayType, fields, 5)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, NumCells, ss, -302, int32_t, Int32ArrayType, fields, 1)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, SurfaceFields, ss, -303, bool, BoolArrayType, fields, 1)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, ss, -305, float, FloatArrayType, fields)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, Omega3s, ss, -306, float, FloatArrayType, fields, 1)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, AxisEulerAngles, ss, -307, float, FloatArrayType, fields, 3)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, AxisLengths, ss, -308, float, FloatArrayType, fields)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, Volumes, ss, -309, float, FloatArrayType, fields, 1)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, Centroids, ss, -310, float, FloatArrayType, fields, 3)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, Schmids, ss, -305, float, FloatArrayType, fields, 1)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, SlipSystems, ss, -306, int32_t, Int32ArrayType, fields, 1)



  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Schmids, ss, float, FloatArrayType, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, GrainAvgMisorientations, ss, float, FloatArrayType, fields, 3)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, BiasedFields, ss, bool, BoolArrayType, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, float, FloatArrayType, fields, 5)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Neighborhoods, ss, int32_t, Int32ArrayType, fields, 3)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, NumCells, ss, int32_t, Int32ArrayType, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Centroids, ss, float, FloatArrayType, fields, 3)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Volumes, ss, float, FloatArrayType, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, AxisLengths, ss, float, FloatArrayType, fields, 3)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, AxisEulerAngles, ss, float, FloatArrayType, fields, 3)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Omega3s, ss, float,FloatArrayType, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, ss, float,FloatArrayType, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, FieldEulerAngles, ss, float, FloatArrayType, fields, 3)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Phases, F, ss, int32_t, Int32ArrayType, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, SlipSystems, ss, int32_t, Int32ArrayType, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, AspectRatios, ss, float,FloatArrayType, fields, 2)

 // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborList = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >
                                          (m->getFieldData(DREAM3D::FieldData::NeighborList).get());
  if(m_NeighborList == NULL)
  {
    ss << "NeighborLists Array Not Initialized At Beginning of MatchCrystallography Filter" << std::endl;
    setErrorCondition(-308);
  }

  // And we do the same for the SharedSurfaceArea list
  m_SharedSurfaceAreaList = NeighborList<float>::SafeObjectDownCast<IDataArray*, NeighborList<float>*>
                                 (m->getFieldData(DREAM3D::FieldData::SharedSurfaceAreaList).get());
  if(m_SharedSurfaceAreaList == NULL)
  {
    ss << "SurfaceAreaLists Array Not Initialized At Beginning of MatchCrystallography Filter" << std::endl;
    setErrorCondition(-309);
  }



  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, TotalSurfaceArea, ss, -303,  float, FloatArrayType, m->crystruct.size(), 1);




 */
