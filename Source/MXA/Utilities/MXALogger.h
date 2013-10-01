
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//

#ifndef _MXALogger_H_
#define _MXALogger_H_

#ifdef LOGGER_NAMESPACE
#error REMOVE THE DEFINITION YOU DONT NEED IT ANYMORE
#endif

#include "MXA/MXA.h"
#include <MXA/Common/LogTime.h>
#include <MXA/Common/MXASetGetMacros.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>


/**
 * @def MXALOGGER_METHOD_VARIABLE_INSTANCE
 * Use this to get an instance of the Global MXALogger variable
 */
#define MXALOGGER_METHOD_VARIABLE_INSTANCE MXALogger::Pointer MXA_Global_Logger = MXALogger::instance();


/**
 * @def MXALOGGER_CLASS_VARIABLE_DECLARATION
 * Use this macro to declare an instance variable for your class or method.
 */
#define MXALOGGER_CLASS_VARIABLE_DECLARATION MXALogger::Pointer MXA_Global_Logger;

/**
 * @def MXALOGGER_CLASS_VARIABLE_INSTANCE
 * Uses the default naming scheme for the Global instance of the MXALogger class.
 */
#define MXALOGGER_CLASS_VARIABLE_INSTANCE       MXA_Global_Logger = MXALogger::instance();

/**
 * @def mxa_log
 * Use this macro in order to create a syntax similar to std::cout << style.
 */
#define mxa_log                    MXA_Global_Logger->mxaLogger


#define CHECK_PRECONDITION( stuff )\
  if(m_IsFileBased == false){\
    std::cout << stuff; return std::cout;}\
    else {\
      m_OutStream << (stuff);  return m_OutStream; }


/**
* @class MXALogger_Implementation MXALogger.h MXA/Utilities/MXALogger.h
* @brief The actual implementation of the logging class. This class should NEVER
* be instantiated by a program. Use 'MXALogger' instead.
* @author Michael A. Jackson for BlueQuartz Software
* @date May 22, 2009
* @version $Revision$
*/
class MXA_EXPORT MXALogger_Implementation
{

  public:
	  MXALogger_Implementation();
    virtual ~MXALogger_Implementation();

    MXA_INSTANCE_PROPERTY(bool, IsFileBased)
    MXA_INSTANCE_STRING_PROPERTY(FileName)

    bool open(const std::string &fn, std::ios::openmode mode = std::ios::trunc);
    bool close();

    std::ostream& dateTime() { CHECK_PRECONDITION(logTime()) }
    std::ostream& warn() { CHECK_PRECONDITION("[Warning] ")}
    std::ostream& error() { CHECK_PRECONDITION("[Error] ")}
    std::ostream& operator<<(uint8_t v) { CHECK_PRECONDITION(v) }
    std::ostream& operator<<(int8_t v) { CHECK_PRECONDITION(v) }
    std::ostream& operator<<(uint16_t v) { CHECK_PRECONDITION(v) }
    std::ostream& operator<<(uint32_t v) { CHECK_PRECONDITION(v) }
    std::ostream& operator<<(int32_t v) { CHECK_PRECONDITION(v) }
    std::ostream& operator<<(uint64_t v) { CHECK_PRECONDITION(v) }
    std::ostream& operator<<(int64_t v) { CHECK_PRECONDITION(v) }
    std::ostream& operator<<(double v)  { CHECK_PRECONDITION(v) }
    std::ostream& operator<<(const std::string &v) { CHECK_PRECONDITION(v) }
    std::ostream& operator<<(const char* v) { CHECK_PRECONDITION(v) }

  private:
    std::ofstream m_OutStream;
    MXALogger_Implementation(const MXALogger_Implementation&);   //Copy Constructor Not Implemented
    void operator=(const MXALogger_Implementation&);  // Operator '=' Not Implemented

};


/**
* @class MXALogger MXALogger.h Source/Utilities/MXALogger.h
* @brief MXALogger is a wrapper class around the actual implementation class. MXALogger should be
* used to log debug information into a log file or to std::cout.
*
* @details To use this class you should use the macros provided in this class header.
*   To get an instance of the global logger use the folling in your code:
*    <code> MXALOGGER_METHOD_VARIABLE_INSTANCE</code>
*    Then to use to the logger it can be syntactically the same as 'std::cout <<'
*    style by using the 'mxa_log' macro definition.
*    <code>mxa_log << "My debugging statement " << errorCode << std::endl;</code>
*
*    If you want your class to have an class wide instance variable to the global logger
*    then you can use the following macros:
*     In your class declaration use 'MXALOGGER_CLASS_VARIABLE_DECLARATION' macro.
*     In the code within the class method definitions use the 'mxa_log' macro.
*     Do NOT forget to 'mxa_log.open(filename)' using the path/name to a log file otherwise
*     the logging output will simply be sent to 'std::cout'.
*
* @author Michael A. Jackson for BlueQuartz Software
* @date May 26, 2009
* @version 1.0
*/
class MXA_EXPORT MXALogger
{
  public:
	MXA_SHARED_POINTERS(MXALogger)
	virtual ~MXALogger();

    static MXALogger::Pointer instance();

    MXALogger_Implementation mxaLogger;

  private:
    MXALogger();

};

#endif
