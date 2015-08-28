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

#ifndef _AttributeMatrixCreationFilterParameter_H_
#define _AttributeMatrixCreationFilterParameter_H_

#include "DREAM3DLib/FilterParameters/FilterParameter.h"

class DREAM3DLib_EXPORT AttributeMatrixCreationFilterParameter : public FilterParameter
{
  public:
    DREAM3D_SHARED_POINTERS(AttributeMatrixCreationFilterParameter)
    DREAM3D_STATIC_NEW_MACRO(AttributeMatrixCreationFilterParameter)
    DREAM3D_TYPE_MACRO(AttributeMatrixCreationFilterParameter)

    typedef struct
    {
      QVector<unsigned int> dcGeometryTypes;
    } RequirementType;

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const DataArrayPath& defaultValue, Category category,
                       const RequirementType req, int groupIndex = -1);

    virtual ~AttributeMatrixCreationFilterParameter();

    /**
     * @brief getWidgetType Returns the type of widget that displays and controls
     * this FilterParameter subclass
     * @return
     */
    QString getWidgetType();

    DREAM3D_INSTANCE_PROPERTY(QVector<unsigned int>, DefaultGeometryTypes)

  protected:
    AttributeMatrixCreationFilterParameter();

  private:
    AttributeMatrixCreationFilterParameter(const AttributeMatrixCreationFilterParameter&); // Copy Constructor Not Implemented
    void operator=(const AttributeMatrixCreationFilterParameter&); // Operator '=' Not Implemented
};

#endif /* _AttributeMatrixCreationFilterParameter_H_ */
