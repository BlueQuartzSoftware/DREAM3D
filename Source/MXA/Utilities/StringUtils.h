
//
//  Copyright (c) 2006, Michael A. Jackson for BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//

#ifndef STRINGUTILITIES_H_
#define STRINGUTILITIES_H_




//-- STL Includes
#include <string>
#include <list>
#include <map>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "MXA/MXA.h"

//TODO: Create Test Suite for these Methods

/**
 * @brief Just some convenience utilities for dealing with strings
 * @version $Revision: 1.2 $
 */
class StringUtils {

public:

/**
* @brief converts a string to a boolean. This method understands 'TRUE', "True", "true" as true
* and "FALSE", "False", "false" as false. Also, "0" is false. ANY OTHER value in the string is considered true.
* @param s
* @return The boolean value
*/
static bool stringToBool(const QString &s)
{
  if (s.compare("TRUE") == 0 ) return true;
  if (s.compare("True") == 0 ) return true;
  if (s.compare("true") == 0 ) return true;
  if (s.compare("FALSE") == 0 ) return false;
  if (s.compare("False") == 0 ) return false;
  if (s.compare("false") == 0 ) return false;

  bool ok = false;
  int b;
  ok = StringUtils::stringToNum<int>(b, s);
  if (ok == false) return false;
  if (b == 0) return false;
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
static QString boolStringToBool(const QString &s)
{
  if (s.compare("TRUE") == 0 ) return "1";
  if (s.compare("True") == 0 ) return "1";
  if (s.compare("true") == 0 ) return "1";
  if (s.compare("FALSE") == 0 ) return "0";
  if (s.compare("False") == 0 ) return "0";
  if (s.compare("false") == 0 ) return "0";
  if (s.compare("0") == 0 ) return "0";
  return "1";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
static bool stringToNum(T &t, const QString &s, std::ios_base& (*f)(std::ios_base&))
{
  std::istringstream iss(s);
  return !(iss >> f >> t).fail();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
static bool stringToNum(T &t, const QString &s)
{
  std::istringstream iss(s);
  return !(iss >> t).fail();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
static QString numToString(int num)
{
	QTextStream converter;
  converter << num;
  return converter.str();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
static QString numToString(T num)
{
  QTextStream converter;
  converter << num;
  return converter.str();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
static  QString numToString(size_t num)
{
  QTextStream converter;
  converter << num;
  return converter.str();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
static void addRightSlash(QString &path)
{
  //if (! (path.at(path.length()-1) == '/')) {
    path += "/";
  //}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
static bool hasRightSlash(const QString &path)
{
  return (path.at(path.length()-1) == '/');
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
/* Returns a copy of the list. Not memory efficient at all */
static QList<QString> splitString(QString sep, QString str)
{
  QList<QString> strings;
  QString::size_type sep_length = sep.length();
  QString::iterator iter = std::search(str.begin(), str.end(), sep.begin(), sep.end());
  strings.push_back(QString(str.begin(), iter));
  QString::iterator iter_start;
  while(iter != str.end()) {
    iter_start = iter+sep_length;
    iter = std::search(iter_start, str.end(), sep.begin(), sep.end());
    strings.push_back(QString(iter_start, iter));
  }
  return strings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
static QString joinList(QString sep, QList<QString> strs)
{
  QString result("");
  QList<QString>::const_iterator iter = strs.begin();
  result += *iter;
  if (iter != strs.end()) {
    iter++;
    for (; iter!=strs.end(); iter++) {
        #if DEBUG
        std::cout << "JOINING Current: " << result << " To: / + " << *iter << std::endl;
        #endif
      result += sep + *iter;
    }
  }
  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
static QString joinList(QString sep, QList<int> nums)
{
  QString result("");

  QList<int>::const_iterator iter = nums.begin();
  result += numToString(*iter);
  if (iter != nums.end()) {
    iter++;
    for (; iter!=nums.end(); iter++) {
      result += sep + numToString(*iter);
    }
  }
  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
static QString makePath(const QString parentPath, const QString name)
{
  QString result;
  if (parentPath == "/") {
    result = name;
  } else {
    result = parentPath;
    if (StringUtils::hasRightSlash(result) == false)
    {
      result = "/" + name;
    }
  }
  return result;
}

// -----------------------------------------------------------------------------
//  Determines if a string ends with another string
// -----------------------------------------------------------------------------
static bool endsWith(const QString &base, const QString ending) {
  QString::size_type len = base.length();
  QString::size_type eLen = ending.length();
  QString::size_type npos = len - eLen;
  QString::size_type pos = base.find(ending, npos);
  return   ( pos != QString::npos);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
static QString indent(int32_t depth)
{
  return QString(2 * depth, ' ');
}

};


#endif /*STRINGUTILITIES_H_*/
