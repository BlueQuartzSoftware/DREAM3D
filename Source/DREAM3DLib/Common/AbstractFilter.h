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
#ifndef _AbstractFilter_H_
#define _AbstractFilter_H_


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Observable.h"
#include "DREAM3DLib/Common/DataContainer.h"

#define METHOD_DEFINITION_TEMPLATE_GETARRAYDATA(GetMethod)\
template<typename PtrType, typename DataArrayType>\
PtrType* GetMethod##SizeCheck(const std::string &arrayName, size_t size)\
{\
PtrType* gi = NULL;\
IDataArray::Pointer iDataArray = m_DataContainer->GetMethod(arrayName);\
if (iDataArray.get() == 0) {\
  std::stringstream s;\
  s << getNameOfClass() << " - Array " << arrayName << " from the DataContainer class was not in the DataContainer";\
  setErrorCondition(-10);\
  setErrorMessage(s.str());\
  return gi;\
}\
if (size != iDataArray->GetNumberOfTuples()) {\
  std::stringstream s;\
  s << getNameOfClass() << " - Array " << arrayName << " from the DataContainer class did not have the correct number of elements.";\
  s << "Required: " << size << " Contains: " << iDataArray->GetNumberOfTuples();\
  setErrorCondition(-11);\
  setErrorMessage(s.str());\
  return gi;\
}\
gi = IDataArray::SafeReinterpretCast<IDataArray*, DataArrayType*, PtrType* >(iDataArray.get());\
if (NULL == gi) {\
  std::stringstream s;\
  s << getNameOfClass() << " -  Array " << arrayName << " from the DataContainer class could not be cast to correct type.";\
  setErrorCondition(-13);\
  setErrorMessage(s.str());\
  return gi;\
}\
return gi;\
}



/**
 * @class AbstractFilter AbstractFilter.h DREAM3DLib/Common/AbstractFilter.h
 * @brief This class is the basic class to subclass when creating a new Filter for
 * DREAM.3D. The subclass must implement at least the  execute method. If an
 * error occurs during the execution of the filter set the errorCondition to
 * a non zero value and optionally use the setErrorMessage() method to explain what the
 * error was. This class also inherits from Observable so that the filter can
 * provide updates to the user interface during execution.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Nov 28, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT AbstractFilter : public Observable
{
  public:
    DREAM3D_SHARED_POINTERS(AbstractFilter)
    DREAM3D_STATIC_NEW_MACRO(AbstractFilter)
    DREAM3D_TYPE_MACRO_SUPER(AbstractFilter, Observable)

    virtual ~AbstractFilter();

    DREAM3D_INSTANCE_PROPERTY(DataContainer*, DataContainer);

    DREAM3D_INSTANCE_STRING_PROPERTY(ErrorMessage);
    DREAM3D_INSTANCE_PROPERTY(int, ErrorCondition);

    /**
     * @brief Cancel the operation
     */
    DREAM3D_INSTANCE_PROPERTY(bool, Cancel);

    /**
     * @brief This method should be fully implemented in subclasses.
     */
    virtual void execute();

    /**
     *
     */
    METHOD_DEFINITION_TEMPLATE_GETARRAYDATA(getVoxelData);
    METHOD_DEFINITION_TEMPLATE_GETARRAYDATA(getFieldData);

  protected:
    AbstractFilter();

  private:
    AbstractFilter(const AbstractFilter&); // Copy Constructor Not Implemented
    void operator=(const AbstractFilter&); // Operator '=' Not Implemented
};




#endif /* _AbstractFilter_H_  */
