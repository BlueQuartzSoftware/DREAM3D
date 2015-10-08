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

#ifndef _DataArraySelectionFilterParameter_H_
#define _DataArraySelectionFilterParameter_H_

#include "SIMPLib/FilterParameters/FilterParameter.h"

class SIMPLib_EXPORT DataArraySelectionFilterParameter : public FilterParameter
{
  public:
    SIMPL_SHARED_POINTERS(DataArraySelectionFilterParameter)
    SIMPL_STATIC_NEW_MACRO(DataArraySelectionFilterParameter)
    SIMPL_TYPE_MACRO(DataArraySelectionFilterParameter)

    typedef struct
    {
      QVector<unsigned int> dcGeometryTypes;
      QVector<unsigned int> amTypes;
      QVector<QString> daTypes;
      QVector< QVector<size_t> > componentDimensions;
    } RequirementType;

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const DataArrayPath& defaultValue, Category category,
                       const RequirementType req, int groupIndex = -1);

    virtual ~DataArraySelectionFilterParameter();

    /**
     * @brief CreateRequirement
     * @param primitiveType
     * @param allowedCompDim
     * @param attributeMatrixCategory
     * @return
     */
    static RequirementType CreateCategoryRequirement(const QString& primitiveType,
                                                     size_t allowedCompDim,
                                                     unsigned int attributeMatrixCategory);

    /**
     * @brief CreateRequirement
     * @param primitiveType
     * @param allowedCompDim
     * @param attributeMatrixType
     * @param geometryType
     * @return
     */
    static RequirementType CreateRequirement(const QString& primitiveType,
                                             size_t allowedCompDim,
                                             unsigned int attributeMatrixType,
                                             unsigned int geometryType);


    /**
    * @brief getWidgetType Returns the type of widget that displays and controls
    * this FilterParameter subclass
    * @return
    */
    QString getWidgetType();

    SIMPL_INSTANCE_PROPERTY(QVector<unsigned int>, DefaultGeometryTypes)
    SIMPL_INSTANCE_PROPERTY(QVector<unsigned int>, DefaultAttributeMatrixTypes)
    SIMPL_INSTANCE_PROPERTY(QVector<QString>, DefaultAttributeArrayTypes)
    SIMPL_INSTANCE_PROPERTY(QVector< QVector<size_t> >, DefaultComponentDimensions)

  protected:
    DataArraySelectionFilterParameter();

  private:
    DataArraySelectionFilterParameter(const DataArraySelectionFilterParameter&); // Copy Constructor Not Implemented
    void operator=(const DataArraySelectionFilterParameter&); // Operator '=' Not Implemented
};


#endif /* _DataArraySelectionFilterParameter_H_ */
