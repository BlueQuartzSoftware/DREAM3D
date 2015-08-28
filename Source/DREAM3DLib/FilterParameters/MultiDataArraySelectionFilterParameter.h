/* ============================================================================
* Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
* Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef _MultiDataArraySelectionFilterParameter_H_
#define _MultiDataArraySelectionFilterParameter_H_

#include "DREAM3DLib/FilterParameters/FilterParameter.h"

class DREAM3DLib_EXPORT MultiDataArraySelectionFilterParameter : public FilterParameter
{
  public:
    DREAM3D_SHARED_POINTERS(MultiDataArraySelectionFilterParameter)
    DREAM3D_STATIC_NEW_MACRO(MultiDataArraySelectionFilterParameter)
    DREAM3D_TYPE_MACRO_SUPER(MultiDataArraySelectionFilterParameter, FilterParameter)

    typedef struct
    {
      QVector<unsigned int> dcGeometryTypes;
      QVector<unsigned int> amTypes;
      QVector<QString> daTypes;
      QVector< QVector<size_t> > componentDimensions;
    } RequirementType;

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QVector<DataArrayPath>& defaultValue, Category category,
                       const RequirementType req, int groupIndex = -1);

    virtual ~MultiDataArraySelectionFilterParameter();

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

    DREAM3D_INSTANCE_PROPERTY(QVector<DataArrayPath>, DefaultPaths)

    /**
     * @brief getWidgetType Returns the type of widget that displays and controls
     * this FilterParameter subclass
     * @return
     */
    QString getWidgetType();

    DREAM3D_INSTANCE_PROPERTY(QVector<unsigned int>, DefaultGeometryTypes)
    DREAM3D_INSTANCE_PROPERTY(QVector<unsigned int>, DefaultAttributeMatrixTypes)
    DREAM3D_INSTANCE_PROPERTY(QVector<QString>, DefaultAttributeArrayTypes)
    DREAM3D_INSTANCE_PROPERTY(QVector< QVector<size_t> >, DefaultComponentDimensions)

  protected:
    MultiDataArraySelectionFilterParameter();

  private:
    MultiDataArraySelectionFilterParameter(const MultiDataArraySelectionFilterParameter&); // Copy Constructor Not Implemented
    void operator=(const MultiDataArraySelectionFilterParameter&); // Operator '=' Not Implemented
};

#endif /* _MultiDataArraySelectionFilterParameter_H_ */
