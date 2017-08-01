

#include <functional>
#include <iostream>

#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"



#define ADD_NEW_FILTER_PARAMETER(Params, Class, Desc, Prop, Category, Type)                                                                                                                                             \
  Class::SetterCallbackType _##Prop##_ParamSetter = [this](Type i) { return this->set##Prop(i); };                                                                                                       \
  Class::GetterCallbackType _##Prop##_ParamGetter = [this] { return this->get##Prop(); };                                                                                                                \
  Params.push_back(Class::New(Desc, #Prop, get##Prop(), Category, _##Prop##_ParamSetter, _##Prop##_ParamGetter));

#define ADD_DOUBLE_PARAMETER(Params, Desc, Prop, Category) \
  ADD_NEW_FILTER_PARAMETER(Params, DoubleParameter, Desc, Prop, Category, double)

//#define ADD_INT32_PARAMETER(Params, Desc, Prop) \
//  ADD_NEW_FILTER_PARAMETER(Params, Int32Parameter, Desc, Prop, int)

//#define ADD_DATAARRAYPATH_PARAMETER(Params, Desc, Prop) \
//  ADD_NEW_FILTER_PARAMETER(Params, DataArrayPathParameter, Desc, Prop, DataArrayPath)


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
namespace Detail
{
const QString FilterName("FilterName");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class IFilterParameter
{
public:
  SIMPL_SHARED_POINTERS(IFilterParameter)

  using EnumType = unsigned int;

  enum class Category : EnumType
  {
    Parameter = 0,
    RequiredArray = 1,
    CreatedArray = 2,
    Uncategorized = 3
  };

  IFilterParameter(const QString& humanLabel, const QString& propertyName, Category category, int groupIndex)
    : m_HumanLabel(humanLabel)
    , m_PropertyName(propertyName)
    , m_Category(category)
    , m_GroupIndex(groupIndex)
  {
  }

  virtual ~IFilterParameter()
  {
  }

  SIMPL_INSTANCE_STRING_PROPERTY(HumanLabel)
  SIMPL_INSTANCE_STRING_PROPERTY(PropertyName)
  SIMPL_INSTANCE_PROPERTY(Category, Category)
  SIMPL_INSTANCE_PROPERTY(int, GroupIndex)


  virtual void readJson(const QJsonObject& json) = 0;
  virtual void writeJson(QJsonObject& json) = 0;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class Int32Parameter : public IFilterParameter
{

public:
  SIMPL_SHARED_POINTERS(Int32Parameter)

  using SetterCallbackType = std::function<void(int)>;
  using GetterCallbackType = std::function<int(void)>;


  static Pointer New(const QString& humanLabel,
                     const QString& propertyName,
                     const int& defaultValue,
                     Category category,
                     SetterCallbackType setterCallback,
                     GetterCallbackType getterCallback,
                     int groupIndex = -1)
  {
    Pointer sharedPtr(new Int32Parameter(humanLabel, propertyName, defaultValue, category, setterCallback, getterCallback, groupIndex));
    return sharedPtr;
  }

  SIMPL_INSTANCE_PROPERTY(int, DefaultValue)

  SetterCallbackType getSetterCallback()
  {
    return m_SetterCallback;
  }
  GetterCallbackType getGetterCallback()
  {
    return m_GetterCallback;
  }

  void readJson(const QJsonObject& json)
  {
    QJsonValue jsonValue = json[getPropertyName()];
    if(!jsonValue.isUndefined())
    {
      m_SetterCallback(jsonValue.toInt(0.0));
    }
  }

  void writeJson(QJsonObject& json)
  {
    json[getPropertyName()] = m_GetterCallback();
  }

protected:
  Int32Parameter(const QString& humanLabel, const QString& propertyName, int defaultValue, Category category, SetterCallbackType setterCallback, GetterCallbackType getterCallback, int groupIndex)
  : IFilterParameter(humanLabel, propertyName, category, groupIndex)
  , m_DefaultValue(defaultValue)
  , m_SetterCallback(setterCallback)
  , m_GetterCallback(getterCallback)
  {
  }

private:

  SetterCallbackType m_SetterCallback;
  GetterCallbackType m_GetterCallback;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DoubleParameter : public IFilterParameter
{
public:
  SIMPL_SHARED_POINTERS(DoubleParameter)

  using SetterCallbackType = std::function<void(double)>;
  using GetterCallbackType = std::function<double(void)>;

  static Pointer New(const QString& humanLabel,
                     const QString& propertyName,
                     const double& defaultValue,
                     Category category,
                     SetterCallbackType setterCallback,
                     GetterCallbackType getterCallback,
                     int groupIndex = -1)
  {
    Pointer sharedPtr(new DoubleParameter(humanLabel, propertyName, defaultValue, category, setterCallback, getterCallback, groupIndex));
    return sharedPtr;
  }

  SIMPL_INSTANCE_PROPERTY(double, DefaultValue)


  SetterCallbackType getSetterCallback()
  {
    return m_SetterCallback;
  }
  GetterCallbackType getGetterCallback()
  {
    return m_GetterCallback;
  }

  void readJson(const QJsonObject& json)
  {
    QJsonValue jsonValue = json[getPropertyName()];
    if(!jsonValue.isUndefined() && jsonValue.isDouble())
    {
      m_SetterCallback(jsonValue.toDouble(0.0));
    }
  }

  void writeJson(QJsonObject& json)
  {
    json[getPropertyName()] = m_GetterCallback();
  }

protected:
  DoubleParameter(const QString& humanLabel, const QString& propertyName, double defaultValue, Category category, SetterCallbackType setterCallback, GetterCallbackType getterCallback, int groupIndex)
  : IFilterParameter(humanLabel, propertyName, category, groupIndex)
  , m_DefaultValue(defaultValue)
  , m_SetterCallback(setterCallback)
  , m_GetterCallback(getterCallback)
  {
  }

private:

  SetterCallbackType m_SetterCallback;
  GetterCallbackType m_GetterCallback;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DataArrayPathParameter : public IFilterParameter
{
public:
  SIMPL_SHARED_POINTERS(DataArrayPathParameter)

  using SetterCallbackType = std::function<void(DataArrayPath)>;
  using GetterCallbackType = std::function<DataArrayPath(void)>;

  static Pointer New(const QString& humanLabel,
                     const QString& propertyName,
                     const DataArrayPath& defaultValue,
                     Category category,
                     SetterCallbackType setterCallback,
                     GetterCallbackType getterCallback,
                     int groupIndex = -1)
  {
    Pointer sharedPtr(new DataArrayPathParameter(humanLabel, propertyName, defaultValue, category, setterCallback, getterCallback, groupIndex));
    return sharedPtr;
  }

  SIMPL_INSTANCE_PROPERTY(DataArrayPath, DefaultValue)


  SetterCallbackType getSetterCallback()
  {
    return m_SetterCallback;
  }
  GetterCallbackType getGetterCallback()
  {
    return m_GetterCallback;
  }

  void readJson(const QJsonObject& json)
  {
    QJsonValue jsonValue = json[getPropertyName()];
    if(!jsonValue.isUndefined() && jsonValue.isObject() && m_SetterCallback)
    {
      DataArrayPath dap;
      QJsonObject obj = jsonValue.toObject();
      dap.readJson(obj);
      m_SetterCallback(dap);
    }
  }

  void writeJson(QJsonObject& json)
  {
    if(m_GetterCallback)
    {
      json[getPropertyName()] = m_GetterCallback().toJsonObject();
    }
  }

protected:
  DataArrayPathParameter(const QString& humanLabel, const QString& propertyName, const DataArrayPath &defaultValue, Category category, SetterCallbackType setterCallback, GetterCallbackType getterCallback, int groupIndex)
  : IFilterParameter(humanLabel, propertyName, category, groupIndex)
  , m_DefaultValue(defaultValue)
  , m_SetterCallback(setterCallback)
  , m_GetterCallback(getterCallback)
  {
  }

private:

  SetterCallbackType m_SetterCallback;
  GetterCallbackType m_GetterCallback;
};

// -----------------------------------------------------------------------------
// This section of Macros allows each FilterParameter subclass to create a macro
// or set of macros that can lessen the amout of code that needs to be written
// in order to create an instantiation of the subclass. The technique used here
// is the 'paired, sliding list' of macro parameters that also makes use of
// __VA__ARGS__
// -----------------------------------------------------------------------------

#define SIMPL_BIND_SETTER(Class, Type, Prop)\
  Class::SetterCallbackType _##Prop##_ParamSetter = [this](Type i) { return this->set##Prop(i); };                                                                                                       \

#define SIMPL_BIND_GETTER(Class, Type, Prop)\
  Class::GetterCallbackType _##Prop##_ParamGetter = [this] { return this->get##Prop(); };                                                                                                                \

// Define overrides that can be used by the expansion of our main macro.
// Each subclass can define a macro that takes up to nine (9) arguments
// to the constructor. These macros support a minimum of 4 arguments.

#define SIMPL_NEW_FP_9(Class, Type, Desc, Prop, Category, Index, A, B, C, D, E)\
  Class::New(Desc, #Prop, get##Prop(), Category, \
  [this](Type i) { return this->set##Prop(i); }, \
  [this] { return this->get##Prop(); },\
  Index, A, B, C, D, E)

#define SIMPL_NEW_FP_8(Class, Type, Desc, Prop, Category, Index, A, B, C, D)\
  Class::New(Desc, #Prop, get##Prop(), Category, \
  [this](Type i) { return this->set##Prop(i); }, \
  [this] { return this->get##Prop(); },\
  Index, A, B, C, D)

#define SIMPL_NEW_FP_7(Class, Type, Desc, Prop, Category, Index, A, B, C)\
  Class::New(Desc, #Prop, get##Prop(), Category, \
  [this](Type i) { return this->set##Prop(i); }, \
  [this] { return this->get##Prop(); },\
  Index, A, B, C)

#define SIMPL_NEW_FP_6(Class, Type, Desc, Prop, Category, Index, A, B)\
  Class::New(Desc, #Prop, get##Prop(), Category, \
  [this](Type i) { return this->set##Prop(i); }, \
  [this] { return this->get##Prop(); },\
  Index, A, B)

#define SIMPL_NEW_FP_5(Class, Type, Desc, Prop, Category, Index, A)\
  Class::New(Desc, #Prop, get##Prop(), Category, \
  [this](Type i) { return this->set##Prop(i); }, \
  [this] { return this->get##Prop(); },\
  Index, A)

#define SIMPL_NEW_FP_4(Class, Type, Desc, Prop, Category, Index)\
  Class::New(Desc, #Prop, get##Prop(), Category, \
  [this](Type i) { return this->set##Prop(i); }, \
  [this] { return this->get##Prop(); },\
  Index)

#define SIMPL_NEW_FP_3(Class, Type, Desc, Prop, Category)\
  Class::New(Desc, #Prop, get##Prop(), Category, \
  [this](Type i) { return this->set##Prop(i); }, \
  [this] { return this->get##Prop(); }\
  )


/**
 * @brief This macro is needed for Visual Studio due to differences of VAR_ARGS when
 * passed to another macro that results in a new macro that needs expansion.
 */
#define SIMPL_EXPAND( x ) x

// -----------------------------------------------------------------------------
// Define a macro that uses the "paired, sliding arg list"
// technique to select the appropriate override.
#define _FP_GET_OVERRIDE(A, B, C, D, E, F, G, H, I, NAME, ...) NAME


#define SIMPL_NEW_INT32_FP(...) \
  SIMPL_EXPAND(_FP_GET_OVERRIDE(__VA_ARGS__, \
  SIMPL_NEW_FP_9, SIMPL_NEW_FP_8, SIMPL_NEW_FP_7, SIMPL_NEW_FP_6, SIMPL_NEW_FP_5, SIMPL_NEW_FP_4, SIMPL_NEW_FP_3)\
  (Int32Parameter, int, __VA_ARGS__))


#define SIMPL_NEW_DOUBLE_FP(...) \
  SIMPL_EXPAND(_FP_GET_OVERRIDE(__VA_ARGS__, \
  SIMPL_NEW_FP_9, SIMPL_NEW_FP_8, SIMPL_NEW_FP_7, SIMPL_NEW_FP_6, SIMPL_NEW_FP_5, SIMPL_NEW_FP_4, SIMPL_NEW_FP_3)\
  (DoubleParameter, double, __VA_ARGS__))

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class Filter
{
public:
  Filter()
  : m_Index(0)
  , m_Parameter1(std::numeric_limits<int>::max())
  , m_Parameter2(std::numeric_limits<double>::max())
  , m_FeatureIdsPath("Foo", "Bar", "FeatureIds")
  {
    setupParameters();
  }

  void setupParameters()
  {
    std::vector<IFilterParameter::Pointer> params;
    int groupIndex = -1;

    params.push_back( SIMPL_NEW_INT32_FP("Parameter 1", Parameter1, IFilterParameter::Category::Parameter) );
    params.push_back( SIMPL_NEW_DOUBLE_FP("Parameter 2", Parameter2, IFilterParameter::Category::Parameter) );
    params.push_back( SIMPL_NEW_INT32_FP("Index", Index, IFilterParameter::Category::Parameter, groupIndex) );

    m_FilterParameters = params;
  }

  SIMPL_FILTER_PARAMETER(int, Index)
  SIMPL_FILTER_PARAMETER(int, Parameter1)
  SIMPL_FILTER_PARAMETER(double, Parameter2)
  SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureIdsPath)

  void readFilterParameters(QJsonObject& root)
  {
    QJsonValue jsonValue = root[QString::number(getIndex())];
    if(!jsonValue.isUndefined() && jsonValue.isObject())
    {
      QJsonObject obj = jsonValue.toObject();
      for(auto const& parameter : m_FilterParameters)
      {
        parameter->readJson(obj);
      }
    }
  }

  void writeParameters(QJsonObject& root)
  {
    QJsonObject obj;
    obj["FilterName"] = getName();
    for(auto const& parameter : m_FilterParameters)
    {
      parameter->writeJson(obj);
    }
    root.insert(QString::number(getIndex()), obj);
  }

  void printValues(std::ostream& out)
  {
    out << "m_P1: " << m_Parameter1 << std::endl;
    out << "m_P2: " << m_Parameter2 << std::endl;
    out << "m_Index: " << m_Index << std::endl;
    out << "FeatureIds Path: " << m_FeatureIdsPath.serialize("/").toStdString() << std::endl;
  }

  QString getName()
  {
    return QString("CoolFilter");
  }

  std::vector<IFilterParameter::Pointer> getFilterParameters()
  {
    return m_FilterParameters;
  }

private:
  std::vector<IFilterParameter::Pointer> m_FilterParameters;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IntFilterParameterWidget(IFilterParameter* parameter, Filter* filter)
{
  if(parameter)
  {
    qDebug() << "Human Label: " << parameter->getHumanLabel();
    qDebug() << "Property Name: " << parameter->getPropertyName();
  }

  // Lets assume that we have a Widget that has a text box that holds an integer
  // we would need to convert that to an actual integer value:
  bool ok = false;
  QString intTextField("999");
  int value = intTextField.toInt(&ok, 10);
  if(!ok)
  {
    qDebug() << "Error converting text to integer";
  }

  // If this simulated widgets knows the "type" of data, i.e., we are collecting
  // an integer from the user, then we know what we should be casting the FilterParameter.
  Int32Parameter* filtParam = dynamic_cast<Int32Parameter*>(parameter);
  if(nullptr != filtParam)
  {
    Int32Parameter::SetterCallbackType setter = filtParam->getSetterCallback();
    if(setter)
    {
      setter(value);
    }
  }
  else
  {
    qDebug() << "Filter Parameter is NOT Int32Parameter";
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  Filter filter;
  filter.printValues(std::cout);
  std::vector<IFilterParameter::Pointer> params = filter.getFilterParameters();

  Int32Parameter::Pointer intParam = std::dynamic_pointer_cast<Int32Parameter>(params.at(0));
  if(nullptr != intParam.get())
  {
    intParam->getSetterCallback()(234234234);
  }

  DoubleParameter::Pointer dblParam = std::dynamic_pointer_cast<DoubleParameter>(params.at(1));
  if(nullptr != dblParam.get())
  {
    dblParam->getSetterCallback()(6.66666);
  }

  dblParam = std::dynamic_pointer_cast<DoubleParameter>(params.at(2));
  if(nullptr != dblParam.get())
  {
    dblParam->getSetterCallback()(6.66666);
  }

  filter.printValues(std::cout);

  IntFilterParameterWidget(intParam.get(), &filter);
  filter.printValues(std::cout);

  IntFilterParameterWidget(dblParam.get(), &filter);
  filter.printValues(std::cout);


  QJsonObject root;
  filter.writeParameters(root);

  qDebug() << root;

  return 0;
}
