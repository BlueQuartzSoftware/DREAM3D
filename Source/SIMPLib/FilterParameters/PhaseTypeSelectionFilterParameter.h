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

#ifndef _PhaseTypeSelectionFilterParameter_H_
#define _PhaseTypeSelectionFilterParameter_H_

#include "SIMPLib/FilterParameters/FilterParameter.h"

class SIMPLib_EXPORT PhaseTypeSelectionFilterParameter : public FilterParameter
{
public:
  SIMPL_SHARED_POINTERS(PhaseTypeSelectionFilterParameter)
    SIMPL_STATIC_NEW_MACRO(PhaseTypeSelectionFilterParameter)
    SIMPL_TYPE_MACRO_SUPER(PhaseTypeSelectionFilterParameter, FilterParameter)

    static Pointer New(const QString& humanLabel,
                      const QString& PhaseTypesArrayName,
                      const QString& phaseTypeCountProperty,
                      const QString& phaseTypeDataProperty,
                      const QString& attributeMatrixProperty,
                      const DataArrayPath attributeMatrixDefault,
                      const QStringList phaseListChoices,
                      Category category,
                      int groupIndex = -1);

  virtual ~PhaseTypeSelectionFilterParameter();

  SIMPL_INSTANCE_PROPERTY(QString, PhaseTypeCountProperty)
  SIMPL_INSTANCE_PROPERTY(QString, PhaseTypeDataProperty)
  SIMPL_INSTANCE_PROPERTY(QString, AttributeMatrixPathProperty)
  SIMPL_INSTANCE_PROPERTY(DataArrayPath, AttributeMatrixPathDefault)
  SIMPL_INSTANCE_PROPERTY(QStringList, PhaseListChoices)

  /**
   * @brief getWidgetType Returns the type of widget that displays and controls
   * this FilterParameter subclass
   * @return
   */
  QString getWidgetType();


protected:
  PhaseTypeSelectionFilterParameter();

private:
  PhaseTypeSelectionFilterParameter(const PhaseTypeSelectionFilterParameter&); // Copy Constructor Not Implemented
  void operator=(const PhaseTypeSelectionFilterParameter&); // Operator '=' Not Implemented
};

#endif /* _PhaseTypeSelectionFilterParameter_H_ */
