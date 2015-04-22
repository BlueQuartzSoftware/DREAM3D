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

#ifndef _FilterParameter_H_
#define _FilterParameter_H_

#include <QtCore/QMetaType>
#include <QtCore/QVector>
#include "EbsdLib/EbsdConstants.h"
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataContainers/DataContainerArrayProxy.h"
#include "DREAM3DLib/DataContainers/DataArrayPath.h"
#include "DREAM3DLib/Utilities/QMetaObjectUtilities.h"

typedef struct { int x; int y; int z; } IntVec3_t;
typedef struct { float x; float y; float z; } FloatVec3_t;
typedef struct { float a; float b; float c; float d; } FloatVec4_t;
typedef struct
{
  float v11;
  float v12;
  float v13;
  float v14;
  float v15;
  float v16;
  float v22;
  float v23;
  float v24;
  float v25;
  float v26;
  float v33;
  float v34;
  float v35;
  float v36;
  float v44;
  float v45;
  float v46;
  float v55;
  float v56;
  float v66;
} FloatVec21_t;
typedef struct
{
  float c20; float c02; float c11; float c10; float c01; float c00;
} Float2ndOrderPoly_t;
typedef struct
{
  float c30; float c03; float c21; float c12; float c20; float c02; float c11; float c10; float c01; float c00;
} Float3rdOrderPoly_t;
typedef struct
{
  float c40; float c04; float c31; float c13; float c22; float c30; float c03; float c21; float c12; float c20;
  float c02; float c11; float c10; float c01; float c00;
} Float4thOrderPoly_t;
typedef struct
{
  qint32 PaddingDigits;
  quint32 Ordering;
  qint32 StartIndex;
  qint32 EndIndex;
  QString InputPath;
  QString FilePrefix;
  QString FileSuffix;
  QString FileExtension;
}
FileListInfo_t;


//typedef struct { float angle; float h; float k; float l; } AxisAngleInput_t;

Q_DECLARE_METATYPE(IntVec3_t)
Q_DECLARE_METATYPE(FloatVec3_t)
Q_DECLARE_METATYPE(FloatVec4_t)
Q_DECLARE_METATYPE(FloatVec21_t)
Q_DECLARE_METATYPE(Float2ndOrderPoly_t)
Q_DECLARE_METATYPE(Float3rdOrderPoly_t)
Q_DECLARE_METATYPE(Float4thOrderPoly_t)
Q_DECLARE_METATYPE(AxisAngleInput_t)
Q_DECLARE_METATYPE(FileListInfo_t)


namespace FilterParameterWidgetType
{

  const QString StringWidget("StringWidget");
  const QString IntWidget("IntWidget");
  const QString DoubleWidget("DoubleWidget");
  const QString InputFileWidget("InputFileWidget");
  const QString InputPathWidget("InputPathWidget");
  const QString OutputFileWidget("OutputFileWidget");
  const QString OutputPathWidget("OutputPathWidget");
  const QString FileListInfoWidget("FileListInfoWidget");
  const QString BooleanWidget("BooleanWidget");
  const QString ChoiceWidget("ChoiceWidget"); // Generic ComboBox Drop down where the filter provides the list of strings
  const QString IntVec3Widget("IntVec3Widget");
  const QString FloatVec3Widget("FloatVec3Widget");
  const QString FloatVec4Widget("FloatVec4Widget");
  const QString SecondOrderPolynomialWidget("SecondOrderPolynomialWidget");
  const QString ThirdOrderPolynomialWidget("ThirdOrderPolynomialWidget");
  const QString FourthOrderPolynomialWidget("FourthOrderPolynomialWidget");
  const QString Symmetric6x6Widget("Symmetric6x6Widget");
  const QString AxisAngleWidget("AxisAngleWidget");
  const QString UnknownWidget("UnknownWidget");
  const QString DataContainerArrayProxyWidget("DataContainerArrayProxyWidget");
  const QString DataContainerReaderWidget("DataContainerReaderWidget");
  const QString DataArraySelectionWidget("DataArraySelectionWidget");
  const QString MultiDataArraySelectionWidget("MultiDataArraySelectionWidget");
  const QString ComparisonSelectionWidget("ComparisonSelectionWidget");
  const QString AttributeMatrixSelectionWidget("AttributeMatrixSelectionWidget");
  const QString DataBundleSelectionWidget("DataBundleSelectionWidget");
  const QString DataContainerSelectionWidget("DataContainerSelectionWidget");
  const QString ShapeTypeSelectionWidget("ShapeTypeSelectionWidget");
  const QString DataArrayCreationWidget("DataArrayCreationWidget");
  const QString SeparatorWidget("SeparatorWidget");
  const QString DynamicTableWidget("DynamicTableWidget");
  const QString LinkedBooleanWidget("LinkedBooleanWidget");
  const QString PhaseTypeSelectionWidget("PhaseTypeSelectionWidget");

  const QString DynamicChoiceWidget("DynamicChoiceWidget");

  const QString PreflightUpdatedValueWidget("PreflightUpdatedValueWidget");
  const QString VolumeDataContainerInfoWidget("VolumeDataContainerInfoWidget");
}

/**
 * @class FilterParameter FilterParameter.h DREAM3DLib/FilterParameters/FilterParameter.h
 * @brief This class holds the various properties that an input parameter to a
 * filter needs to describe itself.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 17, 2012
 * @version 1.0
 */
class DREAM3DLib_EXPORT FilterParameter
{
  public:
    DREAM3D_SHARED_POINTERS(FilterParameter)
    DREAM3D_STATIC_NEW_MACRO(FilterParameter)
    DREAM3D_TYPE_MACRO(FilterParameter)

    /**
     * @brief Creates a new Filter Parameter from the arguments. This is good for a general filter parameter that is needed.
     * @param humanLabel What is displayed to the user in the GUI
     * @param propertyName The name of the property that this FilterParameter controls. It should be an exact match for a Q_PROPERTY
     * that is in the header of the filter
     * @param widgetType The type of widget that will be used for display and gather the input value from the user
     * @param valueType The type of data structure that is used to store the data
     * @param advanced Is this parameter an advanced or basic. If it is advanced then the Filter Parameter will only
     * show up on the 'Advanced' Tab of the Input widget
     * @param units Optional argument that allows the programmer to set Units to help clarify an input value
     * @param fileExtension What file extension should this parameter use
     * @param fileType What type of file does this filter parameter represent (TIFF, STL, ...)
     * @param castableValueType What value can the input value be cast back to.
     * @return
     */
    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const QVariant& defaultValue,
                       bool advanced = false,
                       const QString& units = QString(""),
                       int groupIndex = -1);


    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const FloatVec3_t& defaultValue,
                       bool advanced = false,
                       const QString& units = QString(""),
                       int groupIndex = -1);


    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const FloatVec4_t& defaultValue,
                       bool advanced = false,
                       const QString& units = QString(""),
                       int groupIndex = -1);

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const FloatVec21_t& defaultValue,
                       bool advanced = false,
                       const QString& units = QString(""),
                       int groupIndex = -1);

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const Float2ndOrderPoly_t& defaultValue,
                       bool advanced = false,
                       const QString& units = QString(""),
                       int groupIndex = -1);

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const Float3rdOrderPoly_t& defaultValue,
                       bool advanced = false,
                       const QString& units = QString(""),
                       int groupIndex = -1);

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const Float4thOrderPoly_t& defaultValue,
                       bool advanced = false,
                       const QString& units = QString(""),
                       int groupIndex = -1);

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const IntVec3_t& defaultValue,
                       bool advanced = false,
                       const QString& units = QString(""),
                       int groupIndex = -1);

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const DataArrayPath& defaultValue,
                       bool advanced = false,
                       const QString& units = QString(""),
                       int groupIndex = -1);

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const AxisAngleInput_t& defaultValue,
                       bool advanced = false,
                       const QString& units = QString(""),
                       int groupIndex = -1);

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const UInt32Vector_t& defaultValue,
                       bool advanced = false,
                       int groupIndex = -1);

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const UInt32Vector_t& defaultValue,
                       bool advanced = false,
                       const QString& units = QString(""),
                       int groupIndex = -1);


    virtual ~FilterParameter();

    DREAM3D_INSTANCE_STRING_PROPERTY(HumanLabel)
    DREAM3D_INSTANCE_STRING_PROPERTY(PropertyName)
    DREAM3D_INSTANCE_STRING_PROPERTY(WidgetType)
    DREAM3D_VIRTUAL_INSTANCE_PROPERTY(QVariant, DefaultValue)
    DREAM3D_INSTANCE_PROPERTY(bool, Advanced)
    DREAM3D_INSTANCE_STRING_PROPERTY(Units)
    DREAM3D_INSTANCE_PROPERTY(bool, ReadOnly)
    DREAM3D_INSTANCE_PROPERTY(int, GroupIndex)

  protected:
    FilterParameter();

  private:
    FilterParameter(const FilterParameter&); // Copy Constructor Not Implemented
    void operator=(const FilterParameter&); // Operator '=' Not Implemented
};

typedef QVector<FilterParameter::Pointer> FilterParameterVector;

#endif /* _FilterParameter_H_ */

