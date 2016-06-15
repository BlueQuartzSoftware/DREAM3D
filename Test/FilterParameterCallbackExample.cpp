

#include <functional>
#include <iostream>

#include <QtCore/QDebug>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"




#define BIND_SETTER_PROPERTY(CLASS, PTR, PROP)\
std::bind(&CLASS::set##PROP, PTR, std::placeholders::_1),

#define BIND_GETTER_PROPERTY(CLASS, PTR, PROP)\
std::bind(&CLASS::get##PROP, PTR), QString(#PROP)

#define BIND_PROPERTY(CLASS, PTR, PROP)\
 QString(#PROP), std::bind(&CLASS::set##PROP, PTR, std::placeholders::_1), std::bind(&CLASS::get##PROP, PTR)

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
      IFilterParameter(){}
      virtual ~IFilterParameter(){}

      virtual void parseJsonObject(const QJsonObject &json) = 0;
      virtual void toJsonObject(QJsonObject &json) = 0;

};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class IntParameter : public IFilterParameter
{

  public:

    SIMPL_SHARED_POINTERS(IntParameter)

    typedef std::function<void(int)> SetterCallbackType;
    typedef std::function<int(void)> GetterCallbackType;

    static Pointer New(const QString &property, SetterCallbackType setterCallback, GetterCallbackType getterCallback)
    {
      Pointer sharedPtr (new IntParameter(property, setterCallback, getterCallback));
      return sharedPtr;
    }

    void parseJsonObject(const QJsonObject &json)
    {
      QJsonValue jsonValue = json[m_Key];
      if(!jsonValue.isUndefined() )
      {
        m_SetterCallback(jsonValue.toInt(0.0));
      }
    }

    void toJsonObject(QJsonObject &json)
    {
      json[m_Key] = m_GetterCallback();
    }

  protected:
    IntParameter(const QString &key, SetterCallbackType setterCallback, GetterCallbackType getterCallback) :
      m_Key(key),
      m_SetterCallback(setterCallback),
      m_GetterCallback(getterCallback)
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
class DoubleParameter : public IFilterParameter
{
  public:
    SIMPL_SHARED_POINTERS(DoubleParameter);

    typedef std::function<void(double)> SetterCallbackType;
    typedef std::function<double(void)> GetterCallbackType;

    static Pointer New(const QString &property, SetterCallbackType setterCallback, GetterCallbackType getterCallback)
    {
      Pointer sharedPtr (new DoubleParameter(property, setterCallback, getterCallback));
      return sharedPtr;
    }

    void parseJsonObject(const QJsonObject &json)
    {
      QJsonValue jsonValue = json[m_Key];
      if(!jsonValue.isUndefined() && jsonValue.isDouble())
      {
        m_SetterCallback(jsonValue.toDouble(0.0));
      }
    }

    void toJsonObject(QJsonObject &json)
    {
      json[m_Key] = m_GetterCallback();
    }

  protected:
    DoubleParameter(const QString &key, SetterCallbackType setterCallback, GetterCallbackType getterCallback) :
      m_Key(key),
      m_SetterCallback(setterCallback),
      m_GetterCallback(getterCallback)
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
    SIMPL_SHARED_POINTERS(DataArrayPathParameter);

    typedef std::function<void(DataArrayPath)> SetterCallbackType;
    typedef std::function<DataArrayPath(void)> GetterCallbackType;

    static Pointer New(const QString &property, SetterCallbackType setterCallback, GetterCallbackType getterCallback)
    {
      Pointer sharedPtr (new DataArrayPathParameter(property, setterCallback, getterCallback));
      return sharedPtr;
    }

    void parseJsonObject(const QJsonObject &json)
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

    void toJsonObject(QJsonObject &json)
    {
      json[m_Key] = m_GetterCallback().toJsonObject();
    }

  protected:
    DataArrayPathParameter(const QString &key, SetterCallbackType setterCallback, GetterCallbackType getterCallback) :
      m_Key(key),
      m_SetterCallback(setterCallback),
      m_GetterCallback(getterCallback)
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

    Filter() :
      m_Index(0),
      m_Parameter1(std::numeric_limits<int>::max()),
      m_Parameter2(std::numeric_limits<double>::max()),
      m_FeatureIdsPath("Foo", "Bar", "FeatureIds")
    {
      setupParameters();
    }

    void setupParameters()
    {
      m_FilterParameters.push_back( IntParameter::New(BIND_PROPERTY(Filter, this, Parameter1)) );
      m_FilterParameters.push_back( DoubleParameter::New(BIND_PROPERTY(Filter, this, Parameter2)) );
      m_FilterParameters.push_back( DataArrayPathParameter::New(BIND_PROPERTY(Filter, this, FeatureIdsPath)) );
    }

    SIMPL_FILTER_PARAMETER(int, Index)
    SIMPL_FILTER_PARAMETER(int, Parameter1)
    SIMPL_FILTER_PARAMETER(double, Parameter2)
    SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureIdsPath)


    void readFilterParameters(QJsonObject &root)
    {
        QJsonValue jsonValue = root[QString::number(getIndex())];
        if(!jsonValue.isUndefined() && jsonValue.isObject())
        {
          QJsonObject obj = jsonValue.toObject();
          for(auto const &parameter : m_FilterParameters)
          {
            parameter->parseJsonObject(obj);
          }
        }
    }

    void writeParameters(QJsonObject &root)
    {
      QJsonObject obj;
      obj["FilterName"] = getName();
      for(auto const &parameter : m_FilterParameters)
      {
        parameter->toJsonObject(obj);
      }
      root.insert(QString::number(getIndex()), obj);
    }

    void printValues(std::ostream &out)
    {
      out << "m_P1: " << m_Parameter1 << std::endl;
      out << "m_P2: " << m_Parameter2 << std::endl;
      out << "FeatureIds Path: " << m_FeatureIdsPath.serialize("|").toStdString() << std::endl;
    }

    QString getName()
    {
      return QString("CoolFilter");
    }


  private:
//     IntParameter::Pointer m_IntParameter;
//     DoubleParameter::Pointer m_DoubleParameter;
//     DataArrayPathParameter::Pointer m_DataArrayPathParameter;
     std::vector<IFilterParameter::Pointer> m_FilterParameters;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{



  QJsonObject jsonRoot;
  {
    Filter f;
    f.writeParameters(jsonRoot);
    qDebug() << jsonRoot;
    f.printValues(std::cout);

    std::cout << "=========== Manually Setting Values" << std::endl;
    f.setParameter1(10);
    f.setParameter2(66.6);
    DataArrayPath dap("DataContainer", "AttributeMatrix", "ArrayName");
    f.setFeatureIdsPath(dap);
    f.writeParameters(jsonRoot);
    qDebug() << jsonRoot;
    f.printValues(std::cout);
  }

  {
    Filter f;
    std::cout << "++++++++++++ Fresh Filter" << std::endl;
    std::cout << "Before Reading Values: " << std::endl;
    f.printValues(std::cout);
    f.readFilterParameters(jsonRoot);
    std::cout << "After Reading Values: " << std::endl;
    f.printValues(std::cout);

    f.setParameter1(666);
    f.writeParameters(jsonRoot);

    Filter f2;
    std::cout << "++++++++++++ Fresh Filter" << std::endl;
    std::cout << "Before Reading Values: " << std::endl;
    f2.printValues(std::cout);
    f2.readFilterParameters(jsonRoot);
    std::cout << "After Reading Values: " << std::endl;
    f2.printValues(std::cout);
  }

  {
    std::cout << "======================== JSON Write Test====================" << std::endl;

    QJsonObject root;
    Filter f;
    f.setIndex(0);
    f.setParameter1(10);
    f.setParameter2(66.6);
    DataArrayPath dap("DataContainer", "AttributeMatrix", "ArrayName");
    f.setFeatureIdsPath(dap);
    f.writeParameters(root);
    Filter f2;
    f2.setIndex(1);
    f2.writeParameters(root);

    QJsonDocument jdoc = QJsonDocument(root);
    std::cout << QString(jdoc.toJson()).toStdString() << std::endl;

    std::cout << "======================== JSON Read Test====================" << std::endl;
    f.setParameter1(0);
    f.setParameter2(0.0);
    f.setFeatureIdsPath(DataArrayPath());
    f.readFilterParameters(root);

    f.printValues(std::cout);

  }
  return 0;
}
