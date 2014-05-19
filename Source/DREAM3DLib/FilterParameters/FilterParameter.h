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

typedef struct { int x; int y; int z; } IntVec3_t;
typedef struct { float x; float y; float z; } FloatVec3_t;

//typedef struct { float angle; float h; float k; float l; } AxisAngleInput_t;

Q_DECLARE_METATYPE(IntVec3_t)
Q_DECLARE_METATYPE(FloatVec3_t)
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
  const QString AxisAngleWidget("AxisAngleWidget");
  const QString UnknownWidget("UnknownWidget");
  const QString DataContainerArrayProxyWidget("DataContainerArrayProxyWidget");
  const QString DataArraySelectionWidget("DataArraySelectionWidget");
  const QString ComparisonSelectionWidget("ComparisonSelectionWidget");
  const QString AttributeMatrixSelectionWidget("AttributeMatrixSelectionWidget");
  const QString DataContainerSelectionWidget("DataContainerSelectionWidget");
  const QString ShapeTypeSelectionWidget("ShapeTypeSelectionWidget");
  const QString DataArrayCreationWidget("DataArrayCreationWidget");
  const QString SeparatorWidget("SeparatorWidget");

  const QString PreflightUpdatedValueWidget("PreflightUpdatedValueWidget");
#if 0
  /* **** DO NOT PUT ANY OTHER WIDGETS BETWEEN THIS ***** */
  const QString SingleArraySelectionWidget("SingleArraySelectionWidget"); // ComboBox where the Cell Array names are used to populate
  const QString VolumeEdgeArrayNameSelectionWidget("VolumeEdgeArrayNameSelectionWidget"); //ComboBox where the Feature Array names are used to populate
  const QString VolumeFaceArrayNameSelectionWidget("VolumeFaceArrayNameSelectionWidget"); //ComboBox where the Ensemble Array names are used to populate
  const QString VolumeCellArrayNameSelectionWidget("VolumeCellArrayNameSelectionWidget"); // ComboBox where the Cell Array names are used to populate
  const QString VolumeFeatureArrayNameSelectionWidget("VolumeFeatureArrayNameSelectionWidget"); //ComboBox where the Feature Array names are used to populate
  const QString VolumeEnsembleArrayNameSelectionWidget("VolumeEnsembleArrayNameSelectionWidget"); //ComboBox where the Ensemble Array names are used to populate
  const QString SurfaceVertexArrayNameSelectionWidget("SurfaceVertexArrayNameSelectionWidget");
  const QString SurfaceFaceArrayNameSelectionWidget("SurfaceFaceArrayNameSelectionWidget");
  const QString SurfaceEdgeArrayNameSelectionWidget("SurfaceEdgeArrayNameSelectionWidget");
  const QString SurfaceFeatureArrayNameSelectionWidget("SurfaceFeatureArrayNameSelectionWidget");
  const QString SurfaceEnsembleArrayNameSelectionWidget("SurfaceEnsembleArrayNameSelectionWidget");
  const QString EdgeVertexArrayNameSelectionWidget("EdgeVertexArrayNameSelectionWidget");
  const QString EdgeEdgeArrayNameSelectionWidget("EdgeEdgeArrayNameSelectionWidget");
  const QString EdgeFeatureArrayNameSelectionWidget("EdgeFeatureArrayNameSelectionWidget");
  const QString EdgeEnsembleArrayNameSelectionWidget("EdgeEnsembleArrayNameSelectionWidget");
  const QString VertexVertexArrayNameSelectionWidget("VertexVertexArrayNameSelectionWidget");
  const QString VertexFeatureArrayNameSelectionWidget("VertexFeatureArrayNameSelectionWidget");
  const QString VertexEnsembleArrayNameSelectionWidget("VertexEnsembleArrayNameSelectionWidget");
  /* ****  AND THIS LINE ******** */
#endif
  /* This widget presents a blank table and the user clicks an "Add" button to add arrays and Opertors
  const QString CellArrayComparisonSelectionWidget("CellArrayComparisonSelectionWidget");
  const QString FeatureArrayComparisonSelectionWidget("FeatureArrayComparisonSelectionWidget");
  const QString EnsembleArrayComparisonSelectionWidget("EnsembleArrayComparisonSelectionWidget");
  const QString VertexArrayComparisonSelectionWidget("VertexArrayComparisonSelectionWidget");
  const QString FaceArrayComparisonSelectionWidget("FaceArrayComparisonSelectionWidget");
  const QString EdgeArrayComparisonSelectionWidget("EdgeArrayComparisonSelectionWidget");
  const QString CustomWidget("CustomWidget");
  */



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
                       const QString& widgetType, const QString& valueType,
                       bool advanced = false,
                       const QString& units = QString(""),
                       const QString& fileExtension = QString(""),
                       const QString& fileType = QString(""),
                       const QString& castableValueType = QString("") );

    /**
     * @brief Creates a new Filter Parameter that has conditional logic associated with it through an additional boolean
     * Q_PROPERTY in the filter's header file. On the GUI this manifests itself as a checkbox that enables or disables the
     * widget and will also set both the boolean property and the actual filter parameter property
     * @param humanLabel What is displayed to the user in the GUI
     * @param propertyName The name of the property that this FilterParameter controls. It should be an exact match for a Q_PROPERTY
     * that is in the header of the filter
     * @param widgetType The type of widget that will be used for display and gather the input value from the user
     * @param valueType The type of data structure that is used to store the data
     * @param advanced Is this parameter an advanced or basic. If it is advanced then the Filter Parameter will only
     * show up on the 'Advanced' Tab of the Input widget
     * @param isConditional Does this FilterParameter have conditional logic
     * @param conditionalProperty What Q_PROPERTY does this condition link back to
     * @param conditionalLabel The text to display next to the check box to help the user understand what is going on
     * @return
     */
    static Pointer NewConditional(const QString& humanLabel, const QString& propertyName,
                                  const QString& widgetType, const QString& valueType,
                                  bool advanced,
                                  bool isConditional,
                                  const QString& conditionalProperty,
                                  const QString& conditionalLabel);

    virtual ~FilterParameter();

    DREAM3D_INSTANCE_STRING_PROPERTY(HumanLabel)
    DREAM3D_INSTANCE_STRING_PROPERTY(PropertyName)
    DREAM3D_INSTANCE_STRING_PROPERTY(WidgetType)
    DREAM3D_INSTANCE_STRING_PROPERTY(ValueType)
    DREAM3D_INSTANCE_PROPERTY(bool, Advanced)
    DREAM3D_INSTANCE_STRING_PROPERTY(Units)
    DREAM3D_INSTANCE_STRING_PROPERTY(FileExtension)
    DREAM3D_INSTANCE_STRING_PROPERTY(FileType)
    DREAM3D_INSTANCE_STRING_PROPERTY(CastableValueType)
    DREAM3D_INSTANCE_PROPERTY(bool, ReadOnly)

    DREAM3D_BOOL_PROPERTY(Conditional)
    DREAM3D_INSTANCE_STRING_PROPERTY(ConditionalProperty)
    DREAM3D_INSTANCE_STRING_PROPERTY(ConditionalLabel)

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
class DREAM3DLib_EXPORT ChoiceFilterParameter : public FilterParameter
{
  public:
    DREAM3D_SHARED_POINTERS(ChoiceFilterParameter)
    DREAM3D_STATIC_NEW_MACRO(ChoiceFilterParameter)
    DREAM3D_TYPE_MACRO_SUPER(ChoiceFilterParameter, FilterParameter)

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const QString& valueType,
                       QVector<QString> choices,
                       bool editable,
                       bool advanced = false );

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
class DREAM3DLib_EXPORT ComparisonFilterParameter : public FilterParameter
{
  public:
    DREAM3D_SHARED_POINTERS(ComparisonFilterParameter)
    DREAM3D_STATIC_NEW_MACRO(ComparisonFilterParameter)
    DREAM3D_TYPE_MACRO_SUPER(ComparisonFilterParameter, FilterParameter)

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const QString& valueType,
                       QVector<QString> choices,
                       bool showOperators,
                       bool advanced = false );


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
                       const QString& widgetType, const QString& valueType,
                       const QString& phaseTypeCountProperty,
                       const QString& phaseTypeArrayPathProperty,
                       bool advanced = false );

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
class DREAM3DLib_EXPORT PreflightUpdatedValue : public FilterParameter
{
  public:
    DREAM3D_SHARED_POINTERS(PreflightUpdatedValue)
    DREAM3D_STATIC_NEW_MACRO(PreflightUpdatedValue)
    DREAM3D_TYPE_MACRO_SUPER(PreflightUpdatedValue, FilterParameter)

    static Pointer New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const QString& valueType,
                       bool advanced = false );

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
                       const QString& widgetType, const QString& valueType,
                       DataContainerArrayProxy proxy, Qt::CheckState defState,
                       bool advanced = false );

    virtual ~DataContainerArrayProxyFilterParameter();

    DREAM3D_INSTANCE_PROPERTY(DataContainerArrayProxy, DataContainerArrayProxy)
    DREAM3D_INSTANCE_PROPERTY(Qt::CheckState,  DefaultFlagValue)

  protected:
    DataContainerArrayProxyFilterParameter();

  private:
    DataContainerArrayProxyFilterParameter(const DataContainerArrayProxyFilterParameter&); // Copy Constructor Not Implemented
    void operator=(const DataContainerArrayProxyFilterParameter&); // Operator '=' Not Implemented
};

#endif /* _FilterParameter_H_ */

