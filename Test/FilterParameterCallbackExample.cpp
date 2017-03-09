

#include <functional>
#include <iostream>

#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
namespace Detail
{
const QString FilterName("FilterName");
}

class IFilterParameter
{
public:
  SIMPL_SHARED_POINTERS(IFilterParameter)
  IFilterParameter()
  {
  }

  virtual ~IFilterParameter()
  {
  }

  virtual void parseJsonObject(const QJsonObject& json) = 0;
  virtual void toJsonObject(QJsonObject& json) = 0;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class IntParameter : public IFilterParameter
{

public:
  SIMPL_SHARED_POINTERS(IntParameter)

  using SetterCallbackType = std::function<void(int)>;
  using GetterCallbackType = std::function<int(void)>;


  static Pointer New(const QString& property,
              SetterCallbackType setterCallback,

              GetterCallbackType getterCallback)
  {
    Pointer sharedPtr(new IntParameter(property, setterCallback, getterCallback));
    return sharedPtr;
  }


//  template<typename Setter,
//  typename std::enable_if <std::is_same<Setter, SetterCallbackType>::value == true>::type* = nullptr, typename Getter>
//  static Pointer New(const QString& property, Setter setterCallback, Getter getterCallback)
//  {
//    static_assert(std::is_copy_constructible<Setter>::value, "Compile Error");
//    Pointer sharedPtr(new IntParameter(property, setterCallback, getterCallback));
//    return sharedPtr;
//  }


  SetterCallbackType getSetterCallback() { return m_SetterCallback; }
  GetterCallbackType getGetterCallback() { return m_GetterCallback; }

  void parseJsonObject(const QJsonObject& json)
  {
    QJsonValue jsonValue = json[m_Key];
    if(!jsonValue.isUndefined())
    {
      m_SetterCallback(jsonValue.toInt(0.0));
    }
  }

  void toJsonObject(QJsonObject& json)
  {
    json[m_Key] = m_GetterCallback();
  }

protected:
  IntParameter(const QString& key, SetterCallbackType setterCallback, GetterCallbackType getterCallback)
  : m_Key(key)
  , m_SetterCallback(setterCallback)
  , m_GetterCallback(getterCallback)
  {
  }

private:
  QString m_Key;
  SetterCallbackType m_SetterCallback;
  GetterCallbackType m_GetterCallback;
};


#define ADD_NEW_FILTER_PARAMETER(Params, Class, Var, Type)\
    Class::SetterCallbackType _##Var##_ParamSetter = [this](Type i){return this->set##Var(i);};\
    Class::GetterCallbackType _##Var##_ParamGetter = [this]{return this->get##Var();};\
    Params.push_back(Class::New(#Var, _##Var##_ParamSetter, _##Var##_ParamGetter));\

#define ADD_DOUBLE_PARAMETER(Params, Var)\
  ADD_NEW_FILTER_PARAMETER(Params, DoubleParameter, Var, double)

#define ADD_INT32_PARAMETER(Params, Var)\
  ADD_NEW_FILTER_PARAMETER(Params, IntParameter, Var, int)

#define ADD_DATAARRAYPATH_PARAMETER(Params, Var)\
  ADD_NEW_FILTER_PARAMETER(Params, DataArrayPathParameter, Var, DataArrayPath)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DoubleParameter : public IFilterParameter
{
public:
  SIMPL_SHARED_POINTERS(DoubleParameter)

  using SetterCallbackType = std::function<void(double)>;
  using GetterCallbackType = std::function<double(void)>;

  static Pointer New(const QString& property, SetterCallbackType setterCallback, GetterCallbackType getterCallback)
  {
    Pointer sharedPtr(new DoubleParameter(property, setterCallback, getterCallback));
    return sharedPtr;
  }


//  template<typename Setter
// // ,typename std::enable_if<std::is_same<Setter, SetterCallbackType>::value == true>::type* = nullptr
//  ,typename Getter
// // ,typename std::enable_if<std::is_same<Getter, GetterCallbackType>::value == true>::type* = nullptr
// >
//  static Pointer New(const QString property, Setter setterCallback, Getter getterCallback)
//  {
//    static_assert(std::is_same<Setter, SetterCallbackType>::value == true, "class DoubleParameter not passed a compatible SetterCallbackType");
//    static_assert(std::is_same<Getter, GetterCallbackType>::value == true, "class DoubleParameter not passed a compatible GetterCallbackType");
//    Pointer sharedPtr(new DoubleParameter(property, setterCallback, getterCallback));
//    return sharedPtr;
//  }



  SetterCallbackType getSetterCallback() { return m_SetterCallback; }
  GetterCallbackType getGetterCallback() { return m_GetterCallback; }

  void parseJsonObject(const QJsonObject& json)
  {
    QJsonValue jsonValue = json[m_Key];
    if(!jsonValue.isUndefined() && jsonValue.isDouble())
    {
      m_SetterCallback(jsonValue.toDouble(0.0));
    }
  }

  void toJsonObject(QJsonObject& json)
  {
    json[m_Key] = m_GetterCallback();
  }

protected:
  DoubleParameter(const QString& key, SetterCallbackType setterCallback, GetterCallbackType getterCallback)
  : m_Key(key)
  , m_SetterCallback(setterCallback)
  , m_GetterCallback(getterCallback)
  {
  }

private:
  QString m_Key;
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

  static Pointer New(const QString& property, SetterCallbackType setterCallback, GetterCallbackType getterCallback)
  {
    Pointer sharedPtr(new DataArrayPathParameter(property, setterCallback, getterCallback));
    return sharedPtr;
  }

  SetterCallbackType getSetterCallback() { return m_SetterCallback; }
  GetterCallbackType getGetterCallback() { return m_GetterCallback; }

  void parseJsonObject(const QJsonObject& json)
  {
    QJsonValue jsonValue = json[m_Key];
    if(!jsonValue.isUndefined() && jsonValue.isObject())
    {
      DataArrayPath dap;
      QJsonObject obj = jsonValue.toObject();
      dap.readJson(obj);
      m_SetterCallback(dap);
    }
  }

  void toJsonObject(QJsonObject& json)
  {
    json[m_Key] = m_GetterCallback().toJsonObject();
  }

protected:
  DataArrayPathParameter(const QString& key, SetterCallbackType setterCallback, GetterCallbackType getterCallback)
  : m_Key(key)
  , m_SetterCallback(setterCallback)
  , m_GetterCallback(getterCallback)
  {
  }

private:
  QString m_Key;
  SetterCallbackType m_SetterCallback;
  GetterCallbackType m_GetterCallback;
};

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
//    IntParameter::SetterCallbackType intParamSetter = [this](int i){return this->setParameter1(i);};
//    IntParameter::GetterCallbackType intParamGetter = [this]{return this->getParameter1();};


//    IntParameter::Pointer intParam = IntParameter::New("Parameter1", intParamSetter, intParamGetter);
//    m_FilterParameters.push_back(intParam);

//    DoubleParameter::SetterCallbackType dParamSetter = [this](double i){return this->setParameter2(i);};
//    DoubleParameter::GetterCallbackType dParamGetter = [this]{return this->getParameter2();};
//    m_FilterParameters.push_back(DoubleParameter::New("Parameter2",dParamSetter, dParamGetter));

    ADD_INT32_PARAMETER(m_FilterParameters, Parameter1);
    ADD_DOUBLE_PARAMETER(m_FilterParameters, Parameter2);
    ADD_INT32_PARAMETER(m_FilterParameters, Index);
    ADD_DATAARRAYPATH_PARAMETER(m_FilterParameters, FeatureIdsPath)

//    m_FilterParameters.push_back(DoubleParameter::New
//          ("Parameter2",
//          [this](int i){return this->setParameter1(i);},
//          [this]{return this->getParameter1();}
//          )
//         );



    //  m_FilterParameters.push_back(IntParameter::New("Parameter1", SIMPL_BIND_SETTER(Filter, this, Parameter1), SIMPL_BIND_GETTER(Filter, this, Parameter1)));
    //  m_FilterParameters.push_back(DoubleParameter::New("Parameter2", SIMPL_BIND_SETTER(Filter, this, Parameter2), SIMPL_BIND_GETTER(Filter, this, Parameter2)));
    //  m_FilterParameters.push_back(DataArrayPathParameter::New("FeatureIdsPath", SIMPL_BIND_SETTER(Filter, this, FeatureIdsPath), SIMPL_BIND_GETTER(Filter, this, FeatureIdsPath)));
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
        parameter->parseJsonObject(obj);
      }
    }
  }

  void writeParameters(QJsonObject& root)
  {
    QJsonObject obj;
    obj["FilterName"] = getName();
    for(auto const& parameter : m_FilterParameters)
    {
      parameter->toJsonObject(obj);
    }
    root.insert(QString::number(getIndex()), obj);
  }

  void printValues(std::ostream& out)
  {
    out << "m_P1: " << m_Parameter1 << std::endl;
    out << "m_P2: " << m_Parameter2 << std::endl;
    out << "FeatureIds Path: " << m_FeatureIdsPath.serialize("|").toStdString() << std::endl;
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
void SimulateWidget(Filter* filter)
{

  //Lets assume that we have a Widget that has a text box that holds an integer
  // we would need to convert that to an actual integer value:
  bool ok = false;
  QString intTextField("999");
  int value = intTextField.toInt(&ok, 10);
  if(!ok) {
    qDebug() << "Error converting text to integer";
  }

  // If this simulated widgets knows the "type" of data, i.e., we are collecting
  // an integer from the user, then we know what we should be casting the FilterParameter.
  IntParameter::Pointer filtParam = std::dynamic_pointer_cast<IntParameter>(filter->getFilterParameters().at(0));
  if(nullptr != filtParam.get())
  {
    filtParam->getSetterCallback()(value);
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

    IntParameter::Pointer intParam = std::dynamic_pointer_cast<IntParameter>(params.at(0));
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

  SimulateWidget(&filter);
  filter.printValues(std::cout);


  return 0;
}
