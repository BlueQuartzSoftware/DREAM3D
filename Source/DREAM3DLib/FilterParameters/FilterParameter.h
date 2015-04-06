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
#include <QtCore/QJsonObject>
#include "EbsdLib/EbsdConstants.h"
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataContainers/DataContainerArrayProxy.h"
#include "DREAM3DLib/DataContainers/DataArrayPath.h"
#include "DREAM3DLib/Utilities/QMetaObjectUtilities.h"

typedef struct {
  int x; int y; int z;

  void writeJson(QJsonObject &json)
  {
    json["x"] = x;
    json["y"] = y;
    json["z"] = z;
  }

  bool readJson(QJsonObject &json)
  {
    if (json["x"].isDouble() && json["y"].isDouble() && json["z"].isDouble())
    {
      x = json["x"].toInt();
      y = json["y"].toInt();
      z = json["z"].toInt();
      return true;
    }
    return false;
  }
} IntVec3_t;

typedef struct {
  float x; float y; float z;

  void writeJson(QJsonObject &json)
  {
    json["x"] = x;
    json["y"] = y;
    json["z"] = z;
  }

  bool readJson(QJsonObject &json)
  {
    if (json["x"].isDouble() && json["y"].isDouble() && json["z"].isDouble())
    {
      x = static_cast<float>(json["x"].toDouble());
      y = static_cast<float>(json["y"].toDouble());
      z = static_cast<float>(json["z"].toDouble());
      return true;
    }
    return false;
  }
} FloatVec3_t;

typedef struct {
  float a; float b; float c; float d;

  void writeJson(QJsonObject &json)
  {
    json["a"] = a;
    json["b"] = b;
    json["c"] = c;
    json["d"] = d;
  }

  bool readJson(QJsonObject &json)
  {
    if (json["a"].isDouble() && json["b"].isDouble() && json["c"].isDouble() && json["d"].isDouble())
    {
      a = static_cast<float>(json["a"].toDouble());
      b = static_cast<float>(json["b"].toDouble());
      c = static_cast<float>(json["c"].toDouble());
      d = static_cast<float>(json["d"].toDouble());
      return true;
    }
    return false;
  }
} FloatVec4_t;

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

  void writeJson(QJsonObject &json)
  {
    json["v11"] = v11;
    json["v12"] = v12;
    json["v13"] = v13;
    json["v14"] = v14;
    json["v15"] = v15;
    json["v16"] = v16;
    json["v22"] = v22;
    json["v23"] = v23;
    json["v24"] = v24;
    json["v25"] = v25;
    json["v26"] = v26;
    json["v33"] = v33;
    json["v34"] = v34;
    json["v35"] = v35;
    json["v36"] = v36;
    json["v44"] = v44;
    json["v45"] = v45;
    json["v46"] = v46;
    json["v55"] = v55;
    json["v56"] = v56;
    json["v66"] = v66;
  }

  bool readJson(QJsonObject &json)
  {
    if (json["v11"].isDouble() && json["v12"].isDouble() && json["v13"].isDouble() && json["v14"].isDouble() 
      && json["v15"].isDouble() && json["v16"].isDouble() && json["v22"].isDouble() && json["v23"].isDouble() 
      && json["v24"].isDouble() && json["v25"].isDouble() && json["v26"].isDouble() && json["v33"].isDouble() 
      && json["v34"].isDouble() && json["v35"].isDouble() && json["v36"].isDouble() && json["v44"].isDouble()
      && json["v45"].isDouble() && json["v46"].isDouble() && json["v55"].isDouble() && json["v56"].isDouble()
      && json["v66"].isDouble())
    {
      v11 = static_cast<float>(json["v11"].toDouble());
      v12 = static_cast<float>(json["v12"].toDouble());
      v13 = static_cast<float>(json["v13"].toDouble());
      v14 = static_cast<float>(json["v14"].toDouble());
      v15 = static_cast<float>(json["v15"].toDouble());
      v16 = static_cast<float>(json["v16"].toDouble());
      v22 = static_cast<float>(json["v22"].toDouble());
      v23 = static_cast<float>(json["v23"].toDouble());
      v24 = static_cast<float>(json["v24"].toDouble());
      v25 = static_cast<float>(json["v25"].toDouble());
      v26 = static_cast<float>(json["v26"].toDouble());
      v33 = static_cast<float>(json["v33"].toDouble());
      v34 = static_cast<float>(json["v34"].toDouble());
      v35 = static_cast<float>(json["v35"].toDouble());
      v36 = static_cast<float>(json["v36"].toDouble());
      v44 = static_cast<float>(json["v44"].toDouble());
      v45 = static_cast<float>(json["v45"].toDouble());
      v46 = static_cast<float>(json["v46"].toDouble());
      v55 = static_cast<float>(json["v55"].toDouble());
      v56 = static_cast<float>(json["v56"].toDouble());
      v66 = static_cast<float>(json["v66"].toDouble());
      return true;
    }
    return false;
  }
} FloatVec21_t;

typedef struct
{
  float c20; float c02; float c11; float c10; float c01; float c00;

  void writeJson(QJsonObject &json)
  {
    json["c20"] = c20;
    json["c02"] = c02;
    json["c11"] = c11;
    json["c10"] = c10;
    json["c01"] = c01;
    json["c00"] = c00;
  }

  bool readJson(QJsonObject &json)
  {
    if (json["c20"].isDouble() && json["c02"].isDouble() && json["c11"].isDouble() && json["c10"].isDouble()
      && json["c01"].isDouble() && json["c00"].isDouble())
    {
      c20 = static_cast<float>(json["c20"].toDouble());
      c02 = static_cast<float>(json["c02"].toDouble());
      c11 = static_cast<float>(json["c11"].toDouble());
      c10 = static_cast<float>(json["c10"].toDouble());
      c01 = static_cast<float>(json["c01"].toDouble());
      c00 = static_cast<float>(json["c00"].toDouble());
      return true;
    }
    return false;
  }
} Float2ndOrderPoly_t;

typedef struct
{
  float c30; float c03; float c21; float c12; float c20; float c02; float c11; float c10; float c01; float c00;

  void writeJson(QJsonObject &json)
  {
    json["c30"] = c30;
    json["c03"] = c03;
    json["c21"] = c21;
    json["c12"] = c12;
    json["c20"] = c20;
    json["c02"] = c02;
    json["c11"] = c11;
    json["c10"] = c10;
    json["c01"] = c01;
    json["c00"] = c00;
  }

  bool readJson(QJsonObject &json)
  {
    if (json["c30"].isDouble() && json["c03"].isDouble() && json["c21"].isDouble() && json["c12"].isDouble()
      && json["c20"].isDouble() && json["c02"].isDouble() && json["c11"].isDouble() && json["c10"].isDouble()
      && json["c01"].isDouble() && json["c00"].isDouble())
    {
      c30 = static_cast<float>(json["c30"].toDouble());
      c03 = static_cast<float>(json["c03"].toDouble());
      c21 = static_cast<float>(json["c21"].toDouble());
      c12 = static_cast<float>(json["c12"].toDouble());
      c20 = static_cast<float>(json["c20"].toDouble());
      c02 = static_cast<float>(json["c02"].toDouble());
      c11 = static_cast<float>(json["c11"].toDouble());
      c10 = static_cast<float>(json["c10"].toDouble());
      c01 = static_cast<float>(json["c01"].toDouble());
      c00 = static_cast<float>(json["c00"].toDouble());
      return true;
    }
    return false;
  }
} Float3rdOrderPoly_t;

typedef struct
{
  float c40; float c04; float c31; float c13; float c22; float c30; float c03; float c21; float c12; float c20;
  float c02; float c11; float c10; float c01; float c00;

  void writeJson(QJsonObject &json)
  {
    json["c40"] = c40;
    json["c04"] = c04;
    json["c31"] = c31;
    json["c13"] = c13;
    json["c22"] = c22;
    json["c30"] = c30;
    json["c03"] = c03;
    json["c21"] = c21;
    json["c12"] = c12;
    json["c20"] = c20;
    json["c02"] = c02;
    json["c11"] = c11;
    json["c10"] = c10;
    json["c01"] = c01;
    json["c00"] = c00;
  }

  bool readJson(QJsonObject &json)
  {
    if (json["c40"].isDouble() && json["c04"].isDouble() && json["c31"].isDouble() && json["c13"].isDouble()
      && json["c22"].isDouble() && json["c30"].isDouble() && json["c03"].isDouble() && json["c21"].isDouble()
      && json["c12"].isDouble() && json["c20"].isDouble() && json["c02"].isDouble() && json["c11"].isDouble()
      && json["c10"].isDouble() && json["c01"].isDouble() && json["c00"].isDouble())
    {
      c40 = static_cast<float>(json["c40"].toDouble());
      c04 = static_cast<float>(json["c04"].toDouble());
      c31 = static_cast<float>(json["c31"].toDouble());
      c13 = static_cast<float>(json["c13"].toDouble());
      c22 = static_cast<float>(json["c22"].toDouble());
      c30 = static_cast<float>(json["c30"].toDouble());
      c03 = static_cast<float>(json["c03"].toDouble());
      c21 = static_cast<float>(json["c21"].toDouble());
      c12 = static_cast<float>(json["c12"].toDouble());
      c20 = static_cast<float>(json["c20"].toDouble());
      c02 = static_cast<float>(json["c02"].toDouble());
      c11 = static_cast<float>(json["c11"].toDouble());
      c10 = static_cast<float>(json["c10"].toDouble());
      c01 = static_cast<float>(json["c01"].toDouble());
      c00 = static_cast<float>(json["c00"].toDouble());
      return true;
    }
    return false;
  }
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

  void writeJson(QJsonObject &json)
  {
    json["PaddingDigits"] = static_cast<double>(PaddingDigits);
    json["Ordering"] = static_cast<double>(Ordering);
    json["StartIndex"] = static_cast<double>(StartIndex);
    json["EndIndex"] = static_cast<double>(EndIndex);
    json["InputPath"] = InputPath;
    json["FilePrefix"] = FilePrefix;
    json["FileSuffix"] = FileSuffix;
    json["FileExtension"] = FileExtension;
  }

  bool readJson(QJsonObject &json)
  {
    if (json["PaddingDigits"].isDouble() && json["Ordering"].isDouble() && json["StartIndex"].isDouble() && json["EndIndex"].isDouble()
      && json["InputPath"].isString() && json["FilePrefix"].isString() && json["FileSuffix"].isString() && json["FileExtension"].isString())
    {
      PaddingDigits = static_cast<qint32>(json["PaddingDigits"].toDouble());
      Ordering = static_cast<quint32>(json["Ordering"].toDouble());
      StartIndex = static_cast<qint32>(json["StartIndex"].toDouble());
      EndIndex = static_cast<qint32>(json["EndIndex"].toDouble());
      InputPath = json["InputPath"].toString();
      FilePrefix = json["FilePrefix"].toString();
      FileSuffix = json["FileSuffix"].toString();
      FileExtension = json["FileExtension"].toString();
      return true;
    }
    return false;
  }
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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DREAM3DLib_EXPORT FileListInfoFilterParameter : public FilterParameter
{
  public:
    DREAM3D_SHARED_POINTERS(FileListInfoFilterParameter)
    DREAM3D_STATIC_NEW_MACRO(FileListInfoFilterParameter)
    DREAM3D_TYPE_MACRO_SUPER(FileListInfoFilterParameter, FilterParameter)

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const FileListInfo_t& defaultValue,
                       bool advanced = false);

    virtual ~FileListInfoFilterParameter();

  protected:
    FileListInfoFilterParameter();

  private:
    FileListInfoFilterParameter(const FileListInfoFilterParameter&); // Copy Constructor Not Implemented
    void operator=(const FileListInfoFilterParameter&); // Operator '=' Not Implemented
};

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

    static Pointer New(const QString& humanLabel,
                       const QString& PhaseTypesArrayName,
                       const QString& phaseTypeCountProperty,
                       const QString& phaseTYpeDataProperty,
                       const QString& attributeMatrixProperty,
                       const DataArrayPath attributeMatrixDefault,
                       bool advanced = false,
                       int groupIndex = -1 );

    virtual ~PhaseTypesFilterParameter();

    DREAM3D_INSTANCE_PROPERTY(QString, PhaseTypeCountProperty)
    DREAM3D_INSTANCE_PROPERTY(QString, PhaseTypeDataProperty)
    DREAM3D_INSTANCE_PROPERTY(QString, AttributeMatrixPathProperty)
    DREAM3D_INSTANCE_PROPERTY(DataArrayPath, AttributeMatrixPathDefault)

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
class DREAM3DLib_EXPORT DataContainerReaderFilterParameter : public FileSystemFilterParameter
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DREAM3DLib_EXPORT MultiDataArraySelectionFilterParameter : public FilterParameter
{
public:
	DREAM3D_SHARED_POINTERS(MultiDataArraySelectionFilterParameter)
		DREAM3D_STATIC_NEW_MACRO(MultiDataArraySelectionFilterParameter)
		DREAM3D_TYPE_MACRO_SUPER(MultiDataArraySelectionFilterParameter, FilterParameter)

		static Pointer New(const QString& humanLabel, const QString& propertyName,
		const QString& widgetType, const QVector<DataArrayPath>& defaultPaths,
		bool advanced = false,
		int groupIndex = -1);

	virtual ~MultiDataArraySelectionFilterParameter();

		DREAM3D_INSTANCE_PROPERTY(QVector<DataArrayPath>, DefaultPaths)

protected:
	MultiDataArraySelectionFilterParameter();

private:
	MultiDataArraySelectionFilterParameter(const MultiDataArraySelectionFilterParameter&); // Copy Constructor Not Implemented
	void operator=(const MultiDataArraySelectionFilterParameter&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DREAM3DLib_EXPORT DynamicTableFilterParameter : public FilterParameter
{
public:
	DREAM3D_SHARED_POINTERS(DynamicTableFilterParameter)
		DREAM3D_STATIC_NEW_MACRO(DynamicTableFilterParameter)
		DREAM3D_TYPE_MACRO_SUPER(DynamicTableFilterParameter, FilterParameter)

		static Pointer New(const QString& humanLabel, const QString& propertyName,
		const QString& widgetType, QStringList rHeaders, 
		QStringList cHeaders, std::vector<std::vector<double> > defaultTable,
		bool isRowsDynamic = true, bool isColsDynamic = true,
		int minRowCount = 0, int minColCount = 0 );

	virtual ~DynamicTableFilterParameter();

	DREAM3D_INSTANCE_PROPERTY(int, DefaultRowCount)
		DREAM3D_INSTANCE_PROPERTY(int, DefaultColCount)
		DREAM3D_INSTANCE_PROPERTY(QStringList, ColumnHeaders)
		DREAM3D_INSTANCE_PROPERTY(QStringList, RowHeaders)
		DREAM3D_INSTANCE_PROPERTY(std::vector<std::vector<double> >, DefaultTable)
		DREAM3D_INSTANCE_PROPERTY(bool, AreRowsDynamic)
		DREAM3D_INSTANCE_PROPERTY(bool, AreColsDynamic)
		DREAM3D_INSTANCE_PROPERTY(int, MinRowCount)
		DREAM3D_INSTANCE_PROPERTY(int, MinColCount)
		DREAM3D_INSTANCE_PROPERTY(int, ErrorCondition)
		DREAM3D_INSTANCE_PROPERTY(QString, ErrorMessage)

protected:
	DynamicTableFilterParameter();

private:
	DynamicTableFilterParameter(const DynamicTableFilterParameter&); // Copy Constructor Not Implemented
	void operator=(const DynamicTableFilterParameter&); // Operator '=' Not Implemented
};



#endif /* _FilterParameter_H_ */

