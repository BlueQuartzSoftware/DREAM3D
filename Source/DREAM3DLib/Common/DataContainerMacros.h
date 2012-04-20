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

/**
 * @brief These are used in the filters to run checks on available arrays
 */

#define TEST_MACRO(NameSpace, DType, Name)\
  std::string t = NameSpace::DType::Name;


#define GET_PREREQ_DATA( dc, NameSpace, DType, Name, ss, err, ptrType, ArrayType, size, NumComp)\
  {if (m_##Name##ArrayName.empty() == true){setErrorCondition(100#err);\
  ss << "The name of the array for the " << #NameSpace << #DType << #Name << " was empty. Please provide a name for this array/" << std::endl; }\
  std::string _s(#Name); addRequired##DType(_s);\
  m_##Name = dc->get##DType##SizeCheck<ptrType, ArrayType, AbstractFilter>(m_##Name##ArrayName, size*NumComp, this);\
  if (NULL == m_##Name ) {\
    ss << "Filter " << getNameOfClass() << " requires " << #DType << " array '" << \
    m_##Name##ArrayName << "' to already be created prior to execution." << std::endl;\
    /* ss << "Data Container Issued the following error message\n" << getErrorMessage() << std::endl; */ \
    setErrorCondition(err);\
  }}


#define GET_PREREQ_DATA_SUFFIX( dc, NameSpace, DType, Name, Post, ss, err, ptrType, ArrayType, size, NumComp)\
  {std::string _s(#Name); addRequired##DType(_s);\
  m_##Name##Post = dc->get##DType##SizeCheck<ptrType, ArrayType, AbstractFilter>(m_##Name##ArrayName, size*NumComp, this);\
  if (NULL == m_##Name##Post ) {\
    ss << "Filter " << getNameOfClass() << " requires " << #DType << " array '" << \
    m_##Name##ArrayName << "' to already be created prior to execution." << std::endl;\
    /*ss << "Data Container Issued the following error message\n" << getErrorMessage() << std::endl; */ \
    setErrorCondition(err);\
  }}

#define CREATE_NON_PREREQ_DATA(dc, NameSpace, DType, Name, ss, ptrType, ArrayType, value, size, NumComp)\
  {if (m_##Name##ArrayName.empty() == true){setErrorCondition(10000);\
  ss << "The name of the array for the " << #NameSpace << #DType << #Name << " was empty. Please provide a name for this array/" << std::endl; }\
  std::string _s(#Name); addCreated##DType(_s);\
  int preFlightError = getErrorCondition();\
  std::string errorMsg = getErrorMessage();\
  m_##Name = dc->get##DType##SizeCheck<ptrType, ArrayType, AbstractFilter>(m_##Name##ArrayName, size*NumComp, this);\
  if (NULL ==  m_##Name ) {\
    setErrorCondition(preFlightError); setErrorMessage(errorMsg);\
    ArrayType::Pointer p = ArrayType::CreateArray((size * NumComp), m_##Name##ArrayName);\
    if (NULL == p.get()) {\
      ss << "Filter " << getNameOfClass() << " attempted to create array '" << \
      m_##Name##ArrayName << "' but was unsuccessful. This is most likely due to not enough contiguous memory." << std::endl;\
    /*ss << "Data Container Issued the following error message\n" << getErrorMessage() << std::endl; */ \
      setErrorCondition(-500);\
    } else {\
    p->initializeWithValues(value);\
    p->SetNumberOfComponents(NumComp);\
    p->SetName(m_##Name##ArrayName);\
    dc->add##DType(m_##Name##ArrayName, p);\
    m_##Name = p->GetPointer(0);\
  }\
  } }


#define CREATE_NON_PREREQ_DATA_SUFFIX(dc, NameSpace, DType, Name, Post, ss, ptrType, ArrayType, value, size, NumComp)\
  {if (m_##Name##ArrayName.empty() == true){setErrorCondition(10001);\
  ss << "The name of the array for the " << #NameSpace << #DType << #Name << " was empty. Please provide a name for this array/" << std::endl; }\
  std::string _s(#Name); addCreated##DType(_s);\
  int preFlightError = getErrorCondition();\
  std::string errorMsg = getErrorMessage();\
  m_##Name##Post = dc->get##DType##SizeCheck<ptrType, ArrayType, AbstractFilter>(m_##Name##ArrayName, size*NumComp, this);\
  if (NULL ==  m_##Name##Post ) {\
    setErrorCondition(preFlightError); setErrorMessage(errorMsg);\
    ArrayType::Pointer p = ArrayType::CreateArray((size * NumComp), m_##Name##ArrayName);\
    if (NULL == p.get()) {\
      ss << "Filter " << getNameOfClass() << " attempted to create array '" << \
      m_##Name##ArrayName << "' with size of " << (size*NumComp) << " but was unsuccessful. This is most likely due to not enough contiguous memory." << std::endl;\
      ss << "Data Container Issued the following error message\n" << getErrorMessage() << std::endl;\
      setErrorCondition(-500);\
    } else {\
      p->initializeWithValues(value);\
      p->SetNumberOfComponents(NumComp);\
      p->SetName(m_##Name##ArrayName);\
      dc->add##DType(m_##Name##ArrayName, p);\
      m_##Name##Post = p->GetPointer(0);\
    }\
  } }


/**
 *
 */

#define METHOD_DEF_TEMPLATE_GETARRAYDATA(GetMethod)\
template<typename PtrType, typename DataArrayType, typename AbstractFilter>\
PtrType* GetMethod##SizeCheck(const std::string &arrayName, size_t size, AbstractFilter* obv)\
{\
PtrType* gi = NULL;\
IDataArray::Pointer iDataArray = GetMethod(arrayName);\
if (iDataArray.get() == 0) {\
  std::stringstream s;\
  s << getNameOfClass() << "::" << #GetMethod << "(std::string name) where name = '" << arrayName \
  << "' returned a NULL DataArray indicating the array with 'name=" << arrayName << "' was not in the DataContainer";\
  if (NULL != obv) {obv->setErrorCondition(-500);\
  obv->setErrorMessage(s.str());}\
  return gi;\
}\
if (size != iDataArray->GetSize()) {\
  std::stringstream s;\
  s << getNameOfClass() << " - Array '" << arrayName << "' from the DataContainer class did not have the required number of elements.";\
  s << " Required: " << size << " Contains: " << iDataArray->GetSize();\
  if (NULL != obv) {obv->setErrorCondition(-501);\
  obv->setErrorMessage(s.str());}\
  return gi;\
}\
gi = IDataArray::SafeReinterpretCast<IDataArray*, DataArrayType*, PtrType* >(iDataArray.get());\
if (NULL == gi) {\
  std::stringstream s;\
  s << getNameOfClass() << " -  Array " << arrayName << " from the DataContainer class could not be cast to correct type.";\
  if (NULL != obv) {obv->setErrorCondition(-502);\
  obv->setErrorMessage(s.str());}\
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
      s << getNameOfClass() << ": Array '" << arrayName << "' could not allocate " << size << " elements.";\
      if (NULL != obv) {obv->setErrorCondition(-25);\
      obv->setErrorMessage(s.str());}\
      return valuePtr;\
    }\
    add##Field##Data(arrayName, iDataArray);\
  } \
  valuePtr =\
  IDataArray::SafeReinterpretCast<IDataArray*, DataArrayType*, PtrType* >(iDataArray.get());\
  if (NULL == valuePtr) {\
    std::stringstream s;\
    s << getNameOfClass() << ": Array '" << arrayName << "' could not be cast to proper type;";\
    if (NULL != obv) {obv->setErrorCondition(-12);\
    obv->setErrorMessage(s.str());}\
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
  if (static_cast<size_t>(size) != iDataArray->GetSize()) {\
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
  if (static_cast<size_t>(size) != iDataArray->GetSize()) {\
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
  if (static_cast<size_t>(size) != iDataArray->GetSize()) {\
    std::cout << name << " Size did not match." << size << " vs " << iDataArray->GetNumberOfTuples() << std::endl;\
    return -20;\
  }\
  valuePtr =\
  IDataArray::SafeReinterpretCast<IDataArray*, typeClass*, type* >(dataContainer->get##field##Data(name).get());\
  if (NULL == valuePtr) {\
    return -30;\
  }\
}



#endif /* _DATACONTAINERMACROS_H_ */


/*
   // Cell Data

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1);
  GET_PREREQ_DATA(m, DREAM3D, CellData, AlreadyChecked, ss, -300, bool, BoolArrayType, voxels, 1);
  GET_PREREQ_DATA(m, DREAM3D, CellData, GoodVoxels, ss, -300, bool, BoolArrayType, voxels, 1);
  GET_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, -300, float, FloatArrayType, voxels, 5);
  GET_PREREQ_DATA(m, DREAM3D, CellData, SurfaceVoxels, ss, -301, int8_t, Int8ArrayType, voxels);
  GET_PREREQ_DATA(m, DREAM3D, CellData, KernelAverageMisorientations, ss, -300, float, FloatArrayType, voxels, 1);
  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainMisorientations, ss, -300, float, FloatArrayType, voxels, 1);
  GET_PREREQ_DATA(m, DREAM3D, CellData, MisorientationGradients, ss, -300, float, FloatArrayType, voxels, 1);
  GET_PREREQ_DATA(m, DREAM3D, CellData, NearestNeighbors, ss, -300, int32_t, Int32ArrayType, voxels, 3);
  GET_PREREQ_DATA(m, DREAM3D, CellData, NearestNeighborDistances, ss, -300, float, FloatArrayType, voxels, 3);
  GET_PREREQ_DATA(m, DREAM3D, CellData, Neighbors, ss, -300, int32_t, Int32ArrayType, voxels, 1);
  GET_PREREQ_DATA_SUFFIX(m, DREAM3D, CellData, Phases, C, ss, -300, int32_t, Int32ArrayType,  voxels, 1);
  GET_PREREQ_DATA_SUFFIX(m, DREAM3D, CellData, EulerAngles, C, ss, -300, float, FloatArrayType,  voxels, 3);


  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, int32_t, Int32ArrayType, voxels, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, float, FloatArrayType, fields, 5);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, int32_t, Int32ArrayType, voxels, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, SurfaceVoxels, ss, int8_t, Int8ArrayType, voxels, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Neighbors, ss, int32_t, Int32ArrayType, voxels, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, AlreadyChecked, ss, bool, BoolArrayType, voxels, 1);
  CREATE_NON_PREREQ_DATA_SUFFIX(m, DREAM3D, CellData, Phases, C, ss, int32_t, Int32ArrayType, voxels, 1);
  CREATE_NON_PREREQ_DATA_SUFFIX( m, DREAM3D, CellData, EulerAngles, C, ss, float, FloatArrayType, voxels, 3);


  // Field Data

  GET_PREREQ_DATA_SUFFIX(m, DREAM3D, FieldData, Phases, F, ss, -303,  int32_t, Int32ArrayType, fields, 1);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, -304, bool, BoolArrayType, fields, 1);
  GET_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, -300, float, FloatArrayType, voxels, 5);
  GET_PREREQ_DATA(m, DREAM3D, CellData, EulerAngles, ss, -304, float, FloatArrayType, voxels, 3);
  GET_PREREQ_DATA(m, DREAM3D, CellData, GoodVoxels, ss, -304, bool, BoolArrayType, voxels, 1);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, NumNeighbors, ss, -306, int32_t, Int32ArrayType, fields, 1);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -301, float, FloatArrayType, fields, 5);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, NumCells, ss, -302, int32_t, Int32ArrayType, fields, 1);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, SurfaceFields, ss, -303, bool, BoolArrayType, fields, 1);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, ss, -305, float, FloatArrayType, fields);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, Omega3s, ss, -306, float, FloatArrayType, fields, 1);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, AxisEulerAngles, ss, -307, float, FloatArrayType, fields, 3);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, AxisLengths, ss, -308, float, FloatArrayType, fields);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, Volumes, ss, -309, float, FloatArrayType, fields, 1);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, Centroids, ss, -310, float, FloatArrayType, fields, 3);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, Schmids, ss, -305, float, FloatArrayType, fields, 1);
  GET_PREREQ_DATA(m, DREAM3D, FieldData, SlipSystems, ss, -306, int32_t, Int32ArrayType, fields, 1);



  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Schmids, ss, float, FloatArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, GrainAvgMisorientations, ss, float, FloatArrayType, fields, 3);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, BiasedFields, ss, bool, BoolArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, float, FloatArrayType, fields, 5);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Neighborhoods, ss, int32_t, Int32ArrayType, fields, 3);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, NumCells, ss, int32_t, Int32ArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Centroids, ss, float, FloatArrayType, fields, 3);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Volumes, ss, float, FloatArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, AxisLengths, ss, float, FloatArrayType, fields, 3);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, AxisEulerAngles, ss, float, FloatArrayType, fields, 3);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Omega3s, ss, float,FloatArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, ss, float,FloatArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA_SUFFIX(m, DREAM3D, FieldData, EulerAngles, F, ss, float, FloatArrayType, fields, 3);
  CREATE_NON_PREREQ_DATA_SUFFIX(m, DREAM3D, FieldData, Phases, F, ss, int32_t, Int32ArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, SlipSystems, ss, int32_t, Int32ArrayType, fields, 1);
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, AspectRatios, ss, float,FloatArrayType, fields, 2);

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




