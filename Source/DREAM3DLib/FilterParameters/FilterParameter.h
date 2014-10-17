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

//typedef struct { float angle; float h; float k; float l; } AxisAngleInput_t;

Q_DECLARE_METATYPE(IntVec3_t)
Q_DECLARE_METATYPE(FloatVec3_t)
Q_DECLARE_METATYPE(FloatVec4_t)
Q_DECLARE_METATYPE(AxisAngleInput_t)


namespace FilterParameterWidgetType
{

  const QString StringWidget("StringWidget");
  const QString IntWidget("IntWidget");
  const QString DoubleWidget("DoubleWidget");
  const QString InputFileWidget("InputFileWidget");
  const QString InputPathWidget("InputPathWidget");
  const QString OutputFileWidget("OutputFileWidget");
  const QString OutputPathWidget("OutputPathWidget");
  const QString BooleanWidget("BooleanWidget");
  const QString ChoiceWidget("ChoiceWidget"); // Generic ComboBox Drop down where the filter provides the list of strings
  const QString IntVec3Widget("IntVec3Widget");
  const QString FloatVec3Widget("FloatVec3Widget");
  const QString FloatVec4Widget("FloatVec4Widget");
  const QString AxisAngleWidget("AxisAngleWidget");
  const QString UnknownWidget("UnknownWidget");
  const QString DataContainerArrayProxyWidget("DataContainerArrayProxyWidget");
  const QString DataContainerReaderWidget("DataContainerReaderWidget");
  const QString DataArraySelectionWidget("DataArraySelectionWidget");
  const QString ComparisonSelectionWidget("ComparisonSelectionWidget");
  const QString AttributeMatrixSelectionWidget("AttributeMatrixSelectionWidget");
  const QString DataContainerSelectionWidget("DataContainerSelectionWidget");
  const QString ShapeTypeSelectionWidget("ShapeTypeSelectionWidget");
  const QString DataArrayCreationWidget("DataArrayCreationWidget");
  const QString SeparatorWidget("SeparatorWidget");
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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DREAM3DLib_EXPORT LinkedBooleanFilterParameter : public FilterParameter
{
  public:
    DREAM3D_SHARED_POINTERS(LinkedBooleanFilterParameter)
    DREAM3D_STATIC_NEW_MACRO(LinkedBooleanFilterParameter)
    DREAM3D_TYPE_MACRO_SUPER(LinkedBooleanFilterParameter, FilterParameter)

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QVariant& defaultValue,
                       QStringList conditionalProperties,
                       bool advanced = false,
                       int groupIndex = -1 );

    virtual ~LinkedBooleanFilterParameter();

    DREAM3D_INSTANCE_PROPERTY(QStringList, ConditionalProperties)

  protected:
    LinkedBooleanFilterParameter();

  private:
    LinkedBooleanFilterParameter(const LinkedBooleanFilterParameter&); // Copy Constructor Not Implemented
    void operator=(const LinkedBooleanFilterParameter&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DREAM3DLib_EXPORT ChoiceFilterParameter : public FilterParameter
{
  public:
    DREAM3D_SHARED_POINTERS(ChoiceFilterParameter)
    DREAM3D_STATIC_NEW_MACRO(ChoiceFilterParameter)
    DREAM3D_TYPE_MACRO_SUPER(ChoiceFilterParameter, FilterParameter)

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QVariant& defaultValue,
                       QVector<QString> choices,
                       bool editable,
                       bool advanced = false,
                       int groupIndex = -1 );

    virtual ~ChoiceFilterParameter();

    DREAM3D_INSTANCE_PROPERTY(QVector<QString>, Choices)
    DREAM3D_INSTANCE_PROPERTY(bool, Editable)

  protected:
    ChoiceFilterParameter();

  private:
    ChoiceFilterParameter(const ChoiceFilterParameter&); // Copy Constructor Not Implemented
    void operator=(const ChoiceFilterParameter&); // Operator '=' Not Implemented
};



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DREAM3DLib_EXPORT LinkedChoicesFilterParameter : public ChoiceFilterParameter
{
  public:
    DREAM3D_SHARED_POINTERS(LinkedChoicesFilterParameter)
    DREAM3D_STATIC_NEW_MACRO(LinkedChoicesFilterParameter)
    DREAM3D_TYPE_MACRO_SUPER(LinkedChoicesFilterParameter, FilterParameter)

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QVariant& defaultValue,
                       QVector<QString> choices,
                       QStringList linkedProperties,
                       bool advanced = false,
                       int groupIndex = -1 );

    virtual ~LinkedChoicesFilterParameter();

    DREAM3D_INSTANCE_PROPERTY(QStringList, LinkedProperties)

  protected:
    LinkedChoicesFilterParameter();

  private:
    LinkedChoicesFilterParameter(const LinkedChoicesFilterParameter&); // Copy Constructor Not Implemented
    void operator=(const LinkedChoicesFilterParameter&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
class DREAM3DLib_EXPORT ConstrainedFilterParameter : public FilterParameter
{
  public:
    DREAM3D_SHARED_POINTERS(ConstrainedFilterParameter<T>)
    DREAM3D_STATIC_NEW_MACRO(ConstrainedFilterParameter<T>)
    DREAM3D_TYPE_MACRO_SUPER(ConstrainedFilterParameter<T>, FilterParameter)

    virtual ~ConstrainedFilterParameter() {}

    DREAM3D_INSTANCE_PROPERTY(T, Minimum)
    DREAM3D_INSTANCE_PROPERTY(T, Maximum)

  protected:
    ConstrainedFilterParameter() {}

  private:
    ConstrainedFilterParameter(const ConstrainedFilterParameter&); // Copy Constructor Not Implemented
    void operator=(const ConstrainedFilterParameter&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DREAM3DLib_EXPORT FileSystemFilterParameter : public FilterParameter
{
  public:
    DREAM3D_SHARED_POINTERS(FileSystemFilterParameter)
    DREAM3D_STATIC_NEW_MACRO(FileSystemFilterParameter)
    DREAM3D_TYPE_MACRO_SUPER(FileSystemFilterParameter, FilterParameter)

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const QVariant& defaultValue,
                       bool advanced = false,
                       const QString& units = QString(""),
                       const QString& fileExtension = QString(""),
                       const QString& fileType = QString(""),
                       int groupIndex = -1);

    virtual ~FileSystemFilterParameter();

    DREAM3D_INSTANCE_STRING_PROPERTY(FileExtension)
    DREAM3D_INSTANCE_STRING_PROPERTY(FileType)

  protected:
    FileSystemFilterParameter();

  private:
    FileSystemFilterParameter(const FileSystemFilterParameter&); // Copy Constructor Not Implemented
    void operator=(const FileSystemFilterParameter&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DREAM3DLib_EXPORT VolumeInfoFilterParameter : public FilterParameter
{
  public:
    DREAM3D_SHARED_POINTERS(VolumeInfoFilterParameter)
    DREAM3D_STATIC_NEW_MACRO(VolumeInfoFilterParameter)
    DREAM3D_TYPE_MACRO_SUPER(VolumeInfoFilterParameter, FilterParameter)

    static Pointer New(const QString& humanLabel, const QString& dimsProperty,
                       const QString& widgetType, const IntVec3_t& defaultValue,
                       bool advanced = false,
                       const QString& units = QString(""),
                       const QString& resProperty = QString(""),
                       int groupIndex = -1);

    virtual ~VolumeInfoFilterParameter();

    DREAM3D_INSTANCE_STRING_PROPERTY(DimensionsProperty)
    DREAM3D_INSTANCE_STRING_PROPERTY(ResolutionProperty)

  protected:
    VolumeInfoFilterParameter();

  private:
    VolumeInfoFilterParameter(const VolumeInfoFilterParameter&); // Copy Constructor Not Implemented
    void operator=(const VolumeInfoFilterParameter&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DREAM3DLib_EXPORT DynamicChoiceFilterParameter : public FilterParameter
{
  public:
    DREAM3D_SHARED_POINTERS(DynamicChoiceFilterParameter)
    DREAM3D_STATIC_NEW_MACRO(DynamicChoiceFilterParameter)
    DREAM3D_TYPE_MACRO_SUPER(DynamicChoiceFilterParameter, FilterParameter)

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QVariant& defaultValue,
                       const QString& listProperty,
                       bool advanced = false,
                       int groupIndex = -1 );

    virtual ~DynamicChoiceFilterParameter();


    DREAM3D_INSTANCE_STRING_PROPERTY(ListProperty)

  protected:
    DynamicChoiceFilterParameter();

  private:
    DynamicChoiceFilterParameter(const DynamicChoiceFilterParameter&); // Copy Constructor Not Implemented
    void operator=(const DynamicChoiceFilterParameter&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DREAM3DLib_EXPORT ComparisonFilterParameter : public FilterParameter
{
  public:
    DREAM3D_SHARED_POINTERS(ComparisonFilterParameter)
    DREAM3D_STATIC_NEW_MACRO(ComparisonFilterParameter)
    DREAM3D_TYPE_MACRO_SUPER(ComparisonFilterParameter, FilterParameter)

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const QVariant& defaultValue,
                       QVector<QString> choices,
                       bool showOperators,
                       bool advanced = false,
                       int groupIndex = -1 );


    virtual ~ComparisonFilterParameter();

    DREAM3D_INSTANCE_PROPERTY(QVector<QString>, Choices)
    DREAM3D_INSTANCE_PROPERTY(bool, ShowOperators)

  protected:
    ComparisonFilterParameter();

  private:
    ComparisonFilterParameter(const ComparisonFilterParameter&); // Copy Constructor Not Implemented
    void operator=(const ComparisonFilterParameter&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DREAM3DLib_EXPORT ShapeTypesFilterParameter : public FilterParameter
{
  public:
    DREAM3D_SHARED_POINTERS(ShapeTypesFilterParameter)
    DREAM3D_STATIC_NEW_MACRO(ShapeTypesFilterParameter)
    DREAM3D_TYPE_MACRO_SUPER(ShapeTypesFilterParameter, FilterParameter)

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const QVariant& defaultValue,
                       const QString& phaseTypeCountProperty,
                       const QString& phaseTypeArrayPathProperty,
                       bool advanced = false,
                       int groupIndex = -1 );

    virtual ~ShapeTypesFilterParameter();

    DREAM3D_INSTANCE_PROPERTY(QString, PhaseTypeCountProperty)
    DREAM3D_INSTANCE_PROPERTY(QString, PhaseTypeArrayPathProperty)

  protected:
    ShapeTypesFilterParameter();

  private:
    ShapeTypesFilterParameter(const ShapeTypesFilterParameter&); // Copy Constructor Not Implemented
    void operator=(const ShapeTypesFilterParameter&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DREAM3DLib_EXPORT PhaseTypesFilterParameter : public FilterParameter
{
  public:
    DREAM3D_SHARED_POINTERS(PhaseTypesFilterParameter)
    DREAM3D_STATIC_NEW_MACRO(PhaseTypesFilterParameter)
    DREAM3D_TYPE_MACRO_SUPER(PhaseTypesFilterParameter, FilterParameter)

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const QVariant& defaultValue,
                       const QString& phaseTypeCountProperty,
                       const QString& phaseTypeArrayPathProperty,
                       bool advanced = false,
                       int groupIndex = -1 );

    virtual ~PhaseTypesFilterParameter();

    DREAM3D_INSTANCE_PROPERTY(QString, PhaseTypeCountProperty)
    DREAM3D_INSTANCE_PROPERTY(QString, PhaseTypeArrayPathProperty)

  protected:
    PhaseTypesFilterParameter();

  private:
    PhaseTypesFilterParameter(const PhaseTypesFilterParameter&); // Copy Constructor Not Implemented
    void operator=(const PhaseTypesFilterParameter&); // Operator '=' Not Implemented
};





// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DREAM3DLib_EXPORT PreflightUpdatedValue : public FilterParameter
{
  public:
    DREAM3D_SHARED_POINTERS(PreflightUpdatedValue)
    DREAM3D_STATIC_NEW_MACRO(PreflightUpdatedValue)
    DREAM3D_TYPE_MACRO_SUPER(PreflightUpdatedValue, FilterParameter)

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const QVariant& defaultValue,
                       bool advanced = false,
                       int groupIndex = -1 );

    virtual ~PreflightUpdatedValue();

  protected:
    PreflightUpdatedValue();

  private:
    PreflightUpdatedValue(const PreflightUpdatedValue&); // Copy Constructor Not Implemented
    void operator=(const PreflightUpdatedValue&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DREAM3DLib_EXPORT DataContainerArrayProxyFilterParameter : public FilterParameter
{
  public:
    DREAM3D_SHARED_POINTERS(DataContainerArrayProxyFilterParameter)
    DREAM3D_STATIC_NEW_MACRO(DataContainerArrayProxyFilterParameter)
    DREAM3D_TYPE_MACRO_SUPER(DataContainerArrayProxyFilterParameter, FilterParameter)

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const QVariant& defaultValue,
                       DataContainerArrayProxy proxy, Qt::CheckState defState,
                       bool advanced = false,
                       int groupIndex = -1 );

    virtual ~DataContainerArrayProxyFilterParameter();

    DREAM3D_INSTANCE_PROPERTY(DataContainerArrayProxy, DataContainerArrayProxy)
    DREAM3D_INSTANCE_PROPERTY(Qt::CheckState,  DefaultFlagValue)

  protected:
    DataContainerArrayProxyFilterParameter();

  private:
    DataContainerArrayProxyFilterParameter(const DataContainerArrayProxyFilterParameter&); // Copy Constructor Not Implemented
    void operator=(const DataContainerArrayProxyFilterParameter&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DREAM3DLib_EXPORT DataContainerReaderFilterParameter : public FilterParameter
{
  public:
    DREAM3D_SHARED_POINTERS(DataContainerReaderFilterParameter)
    DREAM3D_STATIC_NEW_MACRO(DataContainerReaderFilterParameter)
    DREAM3D_TYPE_MACRO_SUPER(DataContainerReaderFilterParameter, FilterParameter)

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const QVariant& defaultValue,
                       bool advanced = false,
                       int groupIndex = -1 );

    virtual ~DataContainerReaderFilterParameter();

    DREAM3D_INSTANCE_PROPERTY(Qt::CheckState,  DefaultFlagValue)
    DREAM3D_INSTANCE_PROPERTY(QString, InputFileProperty)

  protected:
    DataContainerReaderFilterParameter();

  private:
    DataContainerReaderFilterParameter(const DataContainerReaderFilterParameter&); // Copy Constructor Not Implemented
    void operator=(const DataContainerReaderFilterParameter&); // Operator '=' Not Implemented
};



#endif /* _FilterParameter_H_ */

