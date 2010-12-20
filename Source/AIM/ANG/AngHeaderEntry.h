
#include <iostream>

#include "MXA/Common/MXASetGetMacros.h"
#include "AIM/Common/AIMCommonConfiguration.h"

/**
 * @brief Creates a static "New" method that creates an instance of thisClass
 */
#define HEADERENTRY_NEW_SUPERCLASS(thisClass, SuperClass)\
  typedef SuperClass::Pointer SuperClass##Type;\
  static SuperClass##Type New##SuperClass(const std::string &key) \
{ \
  SuperClass##Type sharedPtr (new thisClass(key)); \
  return sharedPtr; \
}\

/**
 *
 */
class AIMCOMMON_EXPORT HeaderEntry
{
  public:
    MXA_SHARED_POINTERS(HeaderEntry);
    virtual ~HeaderEntry() {}
    virtual std::string getKey() = 0;
    virtual void parseValue(char* value, size_t start, size_t length) = 0;
    virtual void print(std::ostream &out) = 0;

  protected:
    HeaderEntry() {}


  private:
    HeaderEntry(const HeaderEntry&); // Copy Constructor Not Implemented
    void operator=(const HeaderEntry&); // Operator '=' Not Implemented

};


template<typename T>
class AIMCOMMON_EXPORT AngHeaderEntry : public HeaderEntry
{

  public:
    MXA_SHARED_POINTERS(AngHeaderEntry<T >);
    HEADERENTRY_NEW_SUPERCLASS(AngHeaderEntry<T>, HeaderEntry);

    virtual ~AngHeaderEntry() {}

    std::string getKey() { return m_key; }
    void parseValue(char* value, size_t start, size_t length)
    {
      if (value[start] == ':') { ++start; } // move past the ":" character
      std::string data( &(value[start]), strlen(value) - start);
      std::stringstream ss(data);
      ss >> m_value;
    }
    void print(std::ostream &out) {
      out << m_key << "  " << m_value << std::endl;
    }

    T getValue() { return m_value; }
    void setValue(T value) { m_value = value;}

  protected:
    AngHeaderEntry(const std::string &key) :
      m_value(0),
      m_key(key)
    {
    }

    AngHeaderEntry() {}

  private:
    T m_value;
    std::string m_key;

    AngHeaderEntry(const AngHeaderEntry&); // Copy Constructor Not Implemented
    void operator=(const AngHeaderEntry&); // Operator '=' Not Implemented
};


class AngStringHeaderEntry : public HeaderEntry
{
  public:
    MXA_SHARED_POINTERS(AngStringHeaderEntry);
    HEADERENTRY_NEW_SUPERCLASS(AngStringHeaderEntry, HeaderEntry);

    virtual ~AngStringHeaderEntry() {}

    std::string getKey() { return m_key; }
    void parseValue(char* value, size_t start, size_t length)
    {
      if (value[start] == ':') { ++start; } // move past the ":" character
      while(value[start] == ' ')
      {
        ++start;
      }
      std::string data( &(value[start]), strlen(value) - start);
      m_value = data;
    }
    void print(std::ostream &out) {
      out << m_key << "  " << m_value << std::endl;
    }

    std::string getValue() { return m_value; }
    void setValue(const std::string &value)
    {
      m_value = value;
    }

  protected:
    AngStringHeaderEntry(const std::string &key) :
      m_key(key)
    {
    }

    AngStringHeaderEntry() {}

  private:
    std::string m_value;
    std::string m_key;

    AngStringHeaderEntry(const AngStringHeaderEntry&); // Copy Constructor Not Implemented
    void operator=(const AngStringHeaderEntry&); // Operator '=' Not Implemented
};








