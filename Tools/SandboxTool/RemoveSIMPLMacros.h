#pragma once
#include <memory>

#include <QtCore/QTextStream>

#include <iostream>

#include <QtCore/QProcess>
#include <QtCore/QString>

#include "Sandbox.h"

#define K_START_ID 20

class RemoveSIMPLMacros : public Sandbox
{

public:
  RemoveSIMPLMacros() = default;

  // -----------------------------------------------------------------------------
  QString check_SIMPL_FILTER_PARAMETER(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_FILTER_PARAMETER";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString varType = tokens[0].trimmed();
      QString varName = tokens[1].trimmed();

      //        QString code;
      //        QTextStream out(&code);

      // out << "  private:\n";
      privCodeOut << "    " << varType << " m_" << varName << " = {};\n";
      // out << "\n";
      // out << "  public: \n";
      pubCodeOut << "    /**\n    * @brief Setter property for " << varName << "\n    */\n";
      pubCodeOut << "    void set" << varName << "(const " << varType << "& value); \n";
      pubCodeOut << "    /**\n    * @brief Getter property for " << varName << "\n    * @return Value of " << varName << "\n    */\n";
      pubCodeOut << "    " << varType << " get" << varName << "() const;\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "void " << fi.baseName() << "::set" << varName << "(const " << varType << "& value)\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  m_" << varName << " = value;\n";
      definitionCodeOut << "}\n\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "" << varType << " " << fi.baseName() << "::get" << varName << "() const\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  return m_" << varName << ";\n";
      definitionCodeOut << "}\n\n";

      line = pubCode;
    }
    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_VIRTUAL_INSTANCE_PROPERTY(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_VIRTUAL_INSTANCE_PROPERTY";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString varType = tokens[0].trimmed();
      QString varName = tokens[1].trimmed();

      if(varType.contains("CallbackType"))
      {
        varType = fi.baseName() + "::" + varType;
      }
      //        QString code;
      //        QTextStream out(&code);

      // out << "  private:\n";
      privCodeOut << "    " << varType << " m_" << varName << " = {};\n";
      // out << "\n";
      // out << "  public: \n";
      pubCodeOut << "    /**\n    * @brief Setter property for " << varName << "\n    */\n";
      pubCodeOut << "    virtual void set" << varName << "(const " << varType << "& value); \n";
      pubCodeOut << "    /**\n    * @brief Getter property for " << varName << "\n    * @return Value of " << varName << "\n    */\n";
      pubCodeOut << "    virtual " << varType << " get" << varName << "() const;\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "void " << fi.baseName() << "::set" << varName << "(const " << varType << "& value)\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  m_" << varName << " = value;\n";
      definitionCodeOut << "}\n\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "" << varType << " " << fi.baseName() << "::get" << varName << "() const\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  return m_" << varName << ";\n";
      definitionCodeOut << "}\n\n";

      line = pubCode;
    }
    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_INSTANCE_PROPERTY(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_INSTANCE_PROPERTY";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString varType = tokens[0].trimmed();
      QString varName = tokens[1].trimmed();

      if(varType.contains("CallbackType"))
      {
        varType = fi.baseName() + "::" + varType;
      }
      //        QString code;
      //        QTextStream out(&code);

      // out << "  private:\n";
      privCodeOut << "    " << varType << " m_" << varName << " = {};\n";
      // out << "\n";
      // out << "  public: \n";
      pubCodeOut << "    /**\n    * @brief Setter property for " << varName << "\n    */\n";
      pubCodeOut << "    void set" << varName << "(const " << varType << "& value); \n";
      pubCodeOut << "    /**\n    * @brief Getter property for " << varName << "\n    * @return Value of " << varName << "\n    */\n";
      pubCodeOut << "    " << varType << " get" << varName << "() const;\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "void " << fi.baseName() << "::set" << varName << "(const " << varType << "& value)\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  m_" << varName << " = value;\n";
      definitionCodeOut << "}\n\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "" << varType << " " << fi.baseName() << "::get" << varName << "() const\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  return m_" << varName << ";\n";
      definitionCodeOut << "}\n\n";

      line = pubCode;
    }
    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_DECLARE_ARRAY(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_DECLARE_ARRAY";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString varType = tokens[0].trimmed();
      QString varObj = tokens[1].trimmed();
      QString varName = tokens[2].trimmed();

      if(varType.contains("CallbackType"))
      {
        varType = fi.baseName() + "::" + varType;
      }

      privCodeOut << "    std::shared_ptr<DataArray<" << varType << ">> m_" << varName << ";\n";
      privCodeOut << "    " << varType << "* " << varObj << " = nullptr;\n";
      // out << "\n";
      // out << "  public: \n";
      pubCodeOut << "    /**\n    * @brief Setter property for " << varName << "\n    */\n";
      pubCodeOut << "    void set" << varName << "(const std::shared_ptr<DataArray<" << varType << ">>& value); \n";
      pubCodeOut << "    /**\n    * @brief Getter property for " << varName << "\n    * @return Value of " << varName << "\n    */\n";
      pubCodeOut << "    std::shared_ptr<DataArray<" << varType << ">> get" << varName << "() const;\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "void " << fi.baseName() << "::set" << varName << "(const std::shared_ptr<DataArray<" << varType << ">>& value)\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  m_" << varName << " = value;\n";
      definitionCodeOut << "}\n\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "std::shared_ptr<DataArray<" << varType << ">> " << fi.baseName() << "::get" << varName << "() const\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  return m_" << varName << ";\n";
      definitionCodeOut << "}\n\n";

      line = pubCode;
    }
    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_GET_PROPERTY(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_GET_PROPERTY";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString varType = tokens[0].trimmed();
      QString varName = tokens[1].trimmed();

      if(varType.contains("CallbackType"))
      {
        varType = fi.baseName() + "::" + varType;
      }

      pubCodeOut << "    /**\n    * @brief Getter property for " << varName << "\n    * @return Value of " << varName << "\n    */\n";
      pubCodeOut << "    " << varType << " get" << varName << "() const;\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "" << varType << " " << fi.baseName() << "::get" << varName << "() const\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  return m_" << varName << ";\n";
      definitionCodeOut << "}\n\n";

      line = pubCode;
    }
    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_INSTANCE_VEC3_PROPERTY(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_INSTANCE_VEC3_PROPERTY";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString varType = tokens[0].trimmed();
      QString varName = tokens[1].trimmed();

      if(varType.contains("CallbackType"))
      {
        varType = fi.baseName() + "::" + varType;
      }
      //        QString code;
      //        QTextStream out(&code);

      // out << "  private:\n";
      privCodeOut << "    std::array<" << varType << ", 3> m_" << varName << ";\n";
      // out << "\n";
      // out << "  public: \n";
      pubCodeOut << "    /**\n    * @brief Setter property for " << varName << "\n    */\n";
      pubCodeOut << "    void set" << varName << "(const std::array<" << varType << ",3>& value); \n";
      pubCodeOut << "    /**\n    * @brief Setter property for " << varName << "\n    */\n";
      pubCodeOut << "    void set" << varName << "(" << varType << " v0, " << varType << " v1, " << varType << " v2); \n";

      pubCodeOut << "    /**\n    * @brief Getter property for " << varName << "\n    * @return Value of " << varName << "\n    */\n";
      pubCodeOut << "    std::array<" << varType << ",3> get" << varName << "() const;\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "void " << fi.baseName() << "::set" << varName << "(const std::array<" << varType << ",3>& value)\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  m_" << varName << " = value;\n";
      definitionCodeOut << "}\n\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "void " << fi.baseName() << "::set" << varName << "(" << varType << " v0, " << varType << " v1, " << varType << " v2)\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  m_" << varName << "[0] = v0;\n";
      definitionCodeOut << "  m_" << varName << "[1] = v1;\n";
      definitionCodeOut << "  m_" << varName << "[2] = v2;\n";
      definitionCodeOut << "}\n\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "std::array<" << varType << ",3> " << fi.baseName() << "::get" << varName << "() const\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  return m_" << varName << ";\n";
      definitionCodeOut << "}\n\n";

      line = pubCode;
    }
    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_INSTANCE_STRING_PROPERTY(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_INSTANCE_STRING_PROPERTY";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      QString needsOverride = "";
      if(line.contains("_OVERRIDE"))
      {
        needsOverride = " override ";
        line = line.replace("_OVERRIDE", "");
      }
      line = line.replace("(", "");
      line = line.replace(")", "");

      QString varType = "QString";
      QString varName = line.trimmed();

      privCodeOut << "    " << varType << " m_" << varName << " = {};\n";

      pubCodeOut << "    /**\n    * @brief Setter property for " << varName << "\n    */\n";
      pubCodeOut << "    void set" << varName << "(const " << varType << "& value) " << needsOverride << ";\n";
      pubCodeOut << "    /**\n    * @brief Getter property for " << varName << "\n    * @return Value of " << varName << "\n    */\n";
      pubCodeOut << "    " << varType << " get" << varName << "() const " << needsOverride << ";\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "void " << fi.baseName() << "::set" << varName << "(const " << varType << "& value)\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  m_" << varName << " = value;\n";
      definitionCodeOut << "}\n\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "" << varType << " " << fi.baseName() << "::get" << varName << "() const\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  return m_" << varName << ";\n";
      definitionCodeOut << "}\n\n";

      line = pubCode;
    }
    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_SHARED_POINTERS(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_SHARED_POINTERS";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString thisClass = tokens[0].trimmed();

      pubCodeOut << "    using Self = " << thisClass << ";\n";
      pubCodeOut << "    using Pointer = std::shared_ptr<Self>;\n";
      pubCodeOut << "    using ConstPointer = std::shared_ptr<const Self>;\n";
      pubCodeOut << "    using WeakPointer = std::weak_ptr<Self>;\n";
      pubCodeOut << "    using ConstWeakPointer = std::weak_ptr<const Self>;\n";
      pubCodeOut << "    static Pointer NullPointer();\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << fi.baseName() << "::Pointer " << fi.baseName() << "::NullPointer()\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  return Pointer(static_cast<Self*>(nullptr));\n";
      definitionCodeOut << "}\n\n";

      line = pubCode;
    }
    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_SHARED_POINTERS_HPP(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_SHARED_POINTERS";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString thisClass = tokens[0].trimmed();

      pubCodeOut << "    using Self = " << thisClass << ";\n";
      pubCodeOut << "    using Pointer = std::shared_ptr<Self>;\n";
      pubCodeOut << "    using ConstPointer = std::shared_ptr<const Self>;\n";
      pubCodeOut << "    using WeakPointer = std::weak_ptr<Self>;\n";
      pubCodeOut << "    using ConstWeakPointer = std::weak_ptr<const Self>;\n\n\n";

      pubCodeOut << "    static Pointer NullPointer();\n";

      pubCodeOut << "    Pointer NullPointer()\n";
      pubCodeOut << "    {\n";
      pubCodeOut << "      return Pointer(static_cast<Self*>(nullptr));\n";
      pubCodeOut << "    }\n\n";

      line = pubCode;
    }
    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_FILTER_NEW_MACRO(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_FILTER_NEW_MACRO";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");

      QString thisClass = line.trimmed();

      pubCodeOut << "    static std::shared_ptr<" << thisClass << "> New();\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "std::shared_ptr<" << thisClass << "> " << fi.baseName() << "::New()\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  struct make_shared_enabler : public " << thisClass << "  \n";
      definitionCodeOut << "  {\n";
      definitionCodeOut << "  };\n";
      definitionCodeOut << "  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();\n";
      definitionCodeOut << "  val->setupFilterParameters();\n";
      definitionCodeOut << "  return val;\n";
      definitionCodeOut << "}\n\n";

      line = pubCode;
    }
    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_STATIC_NEW_MACRO(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_STATIC_NEW_MACRO";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");

      QString thisClass = line.trimmed();

      pubCodeOut << "    static Pointer New();\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << fi.baseName() << "::Pointer " << fi.baseName() << "::New()\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  Pointer sharedPtr (new (" << thisClass << "));   \n";
      definitionCodeOut << "  return sharedPtr;\n";
      definitionCodeOut << "}\n\n";

      line = pubCode;
    }
    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_TYPE_MACRO_SUPER_OVERRIDE(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_TYPE_MACRO_SUPER_OVERRIDE";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString className = tokens[0].trimmed();
      QString superClassName = tokens[1].trimmed();

      pubCodeOut << "    /**\n    * @brief Returns the name of the class for " << className << "\n    */\n";
      pubCodeOut << "    QString getNameOfClass() const override;\n";
      pubCodeOut << "    /**\n    * @brief Returns the name of the class for " << className << "\n    */\n";
      pubCodeOut << "    static QString ClassName();\n";

      // pubCodeOut << "    /**\n    * @brief Getter property for " << varName << "\n    * @return Value of " << varName << "\n    */\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "QString " << fi.baseName() << "::getNameOfClass() const\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  return QString(\"" << className << "\");\n";
      definitionCodeOut << "}\n\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "QString " << fi.baseName() << "::ClassName()\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  return QString(\"" << className << "\");\n";
      definitionCodeOut << "}\n\n";

      line = pubCode;
    }

    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_TYPE_MACRO(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_TYPE_MACRO";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString className = tokens[0].trimmed();

      pubCodeOut << "    /**\n    * @brief Returns the name of the class for " << className << "\n    */\n";
      pubCodeOut << "    QString getNameOfClass() const override;\n";
      pubCodeOut << "    /**\n    * @brief Returns the name of the class for " << className << "\n    */\n";
      pubCodeOut << "    static QString ClassName();\n";

      // pubCodeOut << "    /**\n    * @brief Getter property for " << varName << "\n    * @return Value of " << varName << "\n    */\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "QString " << fi.baseName() << "::getNameOfClass() const\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  return QString(\"" << className << "\");\n";
      definitionCodeOut << "}\n\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "QString " << fi.baseName() << "::ClassName()\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  return QString(\"" << className << "\");\n";
      definitionCodeOut << "}\n\n";

      line = pubCode;
    }

    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_TYPE_MACRO_HPP(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_TYPE_MACRO";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString className = tokens[0].trimmed();

      pubCodeOut << "    /**\n    * @brief Returns the name of the class for " << className << "\n    */\n";
      pubCodeOut << "    QString getNameOfClass() const\n";
      pubCodeOut << "    {\n";
      pubCodeOut << "      return QString(\"" << className << "\");\n";
      pubCodeOut << "    }\n\n";

      pubCodeOut << "    /**\n    * @brief Returns the name of the class for " << className << "\n    */\n";
      pubCodeOut << "    QString ClassName()\n";
      pubCodeOut << "    {\n";
      pubCodeOut << "      return QString(\"" << className << "\");\n";
      pubCodeOut << "    }\n\n";

      line = pubCode;
    }

    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_POINTER_PROPERTY(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_POINTER_PROPERTY";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString varType = tokens[0].trimmed();
      QString varName = tokens[1].trimmed();

      privCodeOut << "    " << varType << "* m_" << varName << " = nullptr;\n";

      pubCodeOut << "    /**\n    * @brief Setter property for " << varName << "\n    */\n";
      pubCodeOut << "    void set" << varName << "(" << varType << "* value); \n";

      pubCodeOut << "    /**\n    * @brief Getter property for " << varName << "\n    * @return Value of " << varName << "\n    */\n";
      pubCodeOut << "    " << varType << "* get" << varName << "() const;\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "void " << fi.baseName() << "::set" << varName << "(" << varType << "* value)\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  m_" << varName << " = value;\n";
      definitionCodeOut << "}\n\n";

      definitionCodeOut << "// -----------------------------------------------------------------------------\n";
      definitionCodeOut << "" << varType << "* " << fi.baseName() << "::get" << varName << "() const\n";
      definitionCodeOut << "{\n";
      definitionCodeOut << "  return m_" << varName << ";\n";
      definitionCodeOut << "}";

      line = pubCode;
    }
    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_DEFINE_DATAARRAY_VARIABLE(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "DEFINE_DATAARRAY_VARIABLE";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString varType = tokens[0].trimmed();
      QString varName = tokens[1].trimmed();

      privCodeOut << "    std::weak_ptr<DataArray<" << varType << ">>  m_" << varName << "Ptr;\n";
      privCodeOut << "    " << varType << "* m_" << varName << " = nullptr;\n";

      line = pubCode;
    }
    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_DEFINE_IDATAARRAY_WEAKPTR(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "DEFINE_IDATAARRAY_WEAKPTR";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString varName = tokens[0].trimmed();

      privCodeOut << "    IDataArrayWkPtrType m_" << varName << "Ptr;\n";

      line = pubCode;
    }

    searchString = "DEFINE_DATAARRAY_WEAKPTR";
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString varType = tokens[0].trimmed();
      QString varName = tokens[1].trimmed();

      privCodeOut << "    std::weak_ptr<DataArray<" << varType << ">> m_" << varName << "Ptr;\n";

      line = pubCode;
    }

    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_DEFINE_IDATAARRAY_VARIABLE(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "DEFINE_IDATAARRAY_VARIABLE";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString varName = tokens[0].trimmed();

      privCodeOut << "    IDataArrayWkPtrType m_" << varName << "Ptr;\n";
      privCodeOut << "    void* m_" << varName << " = nullptr;\n";

      line = pubCode;
    }

    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_PIMPL_PROPERTY_DECL(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_PIMPL_PROPERTY_DECL";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString varType = tokens[0].trimmed();
      QString varName = tokens[1].trimmed();

      pubCodeOut << "    /**\n    * @brief Setter property for " << varName << "\n    */\n";
      pubCodeOut << "    void set" << varName << "(" << varType << " value); \n";

      pubCodeOut << "    /**\n    * @brief Getter property for " << varName << "\n    * @return Value of " << varName << "\n    */\n";
      pubCodeOut << "    " << varType << " get" << varName << "() const;\n";

      line = pubCode;
    }
    return line;
  }

  // -----------------------------------------------------------------------------
  QString check_SIMPL_PIMPL_PROPERTY_DEF(const QString& inLine, QTextStream& privCodeOut, QTextStream& definitionCodeOut, bool& hasSearchString, const QFileInfo& fi)
  {
    QString searchString = "SIMPL_PIMPL_PROPERTY_DEF";
    QString pubCode;
    QTextStream pubCodeOut(&pubCode);
    QString line = inLine;
    if(line.trimmed().startsWith("//"))
    {
      return inLine;
    }
    if(inLine.contains(searchString))
    {
      hasSearchString = true;

      line = line.replace(searchString, "");
      line = line.replace("(", "");
      line = line.replace(")", "");
      QStringList tokens = line.split(",");

      QString varClass = tokens[0].trimmed();
      QString varType = tokens[1].trimmed();
      QString varName = tokens[2].trimmed();

      pubCodeOut << "// -----------------------------------------------------------------------------\n";
      pubCodeOut << "void " << varClass << "::set" << varName << "(const " << varType << "& value)\n";
      pubCodeOut << "{\n";
      pubCodeOut << "  Q_D(" << varClass << ");\n";
      pubCodeOut << "  d->m_" << varName << " = value;\n";
      pubCodeOut << "}\n\n";

      pubCodeOut << "// -----------------------------------------------------------------------------\n";
      pubCodeOut << "" << varType << " " << varClass << "::get" << varName << "() const\n";
      pubCodeOut << "{\n";
      pubCodeOut << "  Q_D(" << varClass << ");\n";
      pubCodeOut << "  return d->m_" << varName << ";\n";
      pubCodeOut << "}\n";

      line = pubCode;
    }
    return line;
  }

  // -----------------------------------------------------------------------------
  void operator()(const QString& hFile)
  {
    QString msg;
    QTextStream out(&msg);
    out << hFile;

    QString contents;
    QFileInfo fi(hFile);

    if(fi.absoluteFilePath().contains("Wrapping"))
    {
      return;
    }
    if(fi.baseName() == ("SIMPLibSetGetMacros"))
    {
      return;
    }
    if(fi.baseName() == ("RemoveSIMPLMacros"))
    {
      return;
    }
    {
      // Read the Source File
      QFile source(hFile);
      source.open(QFile::ReadOnly);
      contents = source.readAll();
      source.close();
    }

    bool hasSearchString = false;

    QString searchString = "#include \"SIMPLib/Common/SIMPLibSetGetMacros.h\"";
    int includeIndex = 0;

    QVector<QString> outLines;
    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);

    int32_t lineIndex = 0;

    int32_t privLineIndex = 0;
    QString privCode;
    QTextStream privCodeOut(&privCode);

    QString definitionCode;
    QTextStream definitionCodeOut(&definitionCode);

    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.trimmed() == "private:")
      {
        privLineIndex = lineIndex;
        //  qDebug() << privLineIndex;
      }

#if 1
      line = check_SIMPL_SHARED_POINTERS(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_SIMPL_FILTER_NEW_MACRO(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_SIMPL_STATIC_NEW_MACRO(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_SIMPL_STATIC_NEW_MACRO(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_SIMPL_TYPE_MACRO_SUPER_OVERRIDE(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_SIMPL_FILTER_PARAMETER(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_SIMPL_INSTANCE_PROPERTY(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_SIMPL_INSTANCE_STRING_PROPERTY(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_SIMPL_INSTANCE_VEC3_PROPERTY(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_SIMPL_POINTER_PROPERTY(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_SIMPL_GET_PROPERTY(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_SIMPL_VIRTUAL_INSTANCE_PROPERTY(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_DEFINE_IDATAARRAY_WEAKPTR(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_DEFINE_DATAARRAY_VARIABLE(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_DEFINE_IDATAARRAY_VARIABLE(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_SIMPL_PIMPL_PROPERTY_DECL(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_SIMPL_PIMPL_PROPERTY_DEF(line, privCodeOut, definitionCodeOut, hasSearchString, fi);
      line = check_SIMPL_DECLARE_ARRAY(line, privCodeOut, definitionCodeOut, hasSearchString, fi);

#endif

      if(line.startsWith(searchString))
      {
        includeIndex = lineIndex;
      }

      lineIndex++;
      outLines.push_back(line);
    }

    if(hasSearchString)
    {
      if(privLineIndex == 0)
      {
        // Work backwards from the end of the file to find the end of the class definition
        int32_t numLines = outLines.size() - 1;
        for(int32_t i = numLines; i >= 0; i--)
        {
          QString line = outLines[i].trimmed();
          if(line == "};")
          {
            privLineIndex = i - 1;
            outLines[privLineIndex] = outLines[privLineIndex] + "\n\n  private:";
            break;
          }
        }
      }
      // We still didn't find the end of the class def.
      if(privLineIndex == 0)
      {
        qDebug() << "Line containing the private: section was not found. End of Class definition was not found. BAILING OUT NOW.";
        qDebug() << "    " << fi.absoluteFilePath();
        return;
      }
      QString line = outLines[privLineIndex];
      line = line + "\n" + privCode;
      outLines[privLineIndex] = line;

      if(includeIndex > 0)
      {
        outLines.remove(includeIndex);
      }
    }

    writeOutput(hasSearchString, outLines, hFile);

    if(hasSearchString && hFile.endsWith(".h"))
    {
      /*  *************************************** */
      // Update the .cpp file
      QString cppFile = hFile;
      cppFile.replace(".h", ".cpp");
      QFileInfo cppFi(cppFile);
      if(cppFi.exists())
      {
        {
          // Read the Source File
          QFile source(cppFile);
          source.open(QFile::ReadOnly);
          contents = source.readAll();
          source.close();
        }
        contents = contents + "\n" + definitionCode;
        outLines.clear();
        outLines.push_back(contents);
        outLines.push_back(QString("JUNK"));

        writeOutput(hasSearchString, outLines, cppFile);
      }
    }
  }
};
