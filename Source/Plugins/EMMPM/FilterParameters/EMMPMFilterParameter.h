/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#ifndef _emmpmfilterparameter_h_
#define _emmpmfilterparameter_h_

#include <QtCore/QJsonObject>

#include "EMMPM/EMMPMFilters/EMMPMFilter.h"

#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"

/**
 * @brief The EMMPMFilterParameter class is used by filters to instantiate an DataContainerReaderWidget.  By instantiating an instance of
 * this class in a filter's setupFilterParameters() method, a DataContainerReaderWidget will appear in the filter's "filter input" section in the DREAM3D GUI.
 */
class EMMPMFilterParameter : public FilterParameter
{
  public:
    SIMPL_SHARED_POINTERS(EMMPMFilterParameter)
    SIMPL_STATIC_NEW_MACRO(EMMPMFilterParameter)
    SIMPL_TYPE_MACRO_SUPER(EMMPMFilterParameter, FilterParameter)

    /**
     * @brief New This function instantiates an instance of the DataContainerCreationFilterParameter.
     * @param humanLabel The name that the users of DREAM.3D see for this filter parameter
     * @param propertyName The internal property name for this filter parameter.
     * @param defaultValue The value that this filter parameter will be initialized to by default.
     * @param category The category for the filter parameter in the DREAM.3D user interface.  There
     * are three categories: Parameter, Required Arrays, and Created Arrays.
     * @param filter The corresponding filter that sets all its values into the DataContainerReaderWidget.
     * @param groupIndex Integer that specifies the group that this filter parameter will be placed in.
     * @return
     */
    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& defaultValue, Category category, EMMPMFilter* filter,
                       int groupIndex = -1);

    virtual ~EMMPMFilterParameter();

    SIMPL_INSTANCE_PROPERTY(EMMPMFilter*, Filter)

    /**
     * @brief getWidgetType Returns the type of widget that displays and controls
     * this FilterParameter subclass
     * @return
     */
    QString getWidgetType();

    /**
     * @brief readJson Reads this filter parameter's corresponding property out of a QJsonObject.
     * @param json The QJsonObject that the filter parameter reads from.
     */
    void readJson(const QJsonObject &json);

    /**
     * @brief writeJson Writes this filter parameter's corresponding property to a QJsonObject.
     * @param json The QJsonObject that the filter parameter writes to.
     */
    void writeJson(QJsonObject &json);

  protected:
    /**
     * @brief EMMPMFilterParameter The default constructor.  It is protected because this
     * filter parameter should only be instantiated using its New(...) function or short-form macro.
     */
    EMMPMFilterParameter();

  private:

    EMMPMFilterParameter(const EMMPMFilterParameter&); // Copy Constructor Not Implemented
    void operator=(const EMMPMFilterParameter&); // Operator '=' Not Implemented
};

#endif /* _emmpmfilterparameter_h_ */
