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

#ifndef _DataArrayCreationFilterParameter_H_
#define _DataArrayCreationFilterParameter_H_

#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"

/**
 * @brief The DataArrayCreationFilterParameter class
 */
class SIMPLib_EXPORT DataArrayCreationFilterParameter : public FilterParameter
{
  public:
    SIMPL_SHARED_POINTERS(DataArrayCreationFilterParameter)
    SIMPL_STATIC_NEW_MACRO(DataArrayCreationFilterParameter)
    SIMPL_TYPE_MACRO(DataArrayCreationFilterParameter)

    typedef struct
    {
      QVector<unsigned int> dcGeometryTypes;
      QVector<unsigned int> amTypes;
    } RequirementType;

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const DataArrayPath& defaultValue, Category category,
                       const RequirementType req, int groupIndex = -1);

    virtual ~DataArrayCreationFilterParameter();

    /**
     * @brief CreateRequirement
     * @param attributeMatrixType
     * @return
     */
    static RequirementType CreateRequirement(unsigned int attributeMatrixObjectType);

    /**
     * @brief CreateRequirement
     * @param attributeMatrixType
     * @param geometryType
     * @return
     */
    static RequirementType CreateRequirement(uint32_t attributeMatrixType,
                                             uint32_t geometryType);

    /**
    * @brief getWidgetType Returns the type of widget that displays and controls
    * this FilterParameter subclass
    * @return
    */
    QString getWidgetType();

    SIMPL_INSTANCE_PROPERTY(QVector<unsigned int>, DefaultGeometryTypes)
    SIMPL_INSTANCE_PROPERTY(QVector<unsigned int>, DefaultAttributeMatrixTypes)

  protected:
    DataArrayCreationFilterParameter();

  private:
    DataArrayCreationFilterParameter(const DataArrayCreationFilterParameter&); // Copy Constructor Not Implemented
    void operator=(const DataArrayCreationFilterParameter&); // Operator '=' Not Implemented
};



#endif /* _DataArrayCreationFilterParameter_H_ */
