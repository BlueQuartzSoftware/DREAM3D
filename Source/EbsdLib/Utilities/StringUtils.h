/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#define StringUtils EbsdStringUtils

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
static bool stringToBool(const std::string &s)
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
static std::string boolStringToBool(const std::string &s)
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
static bool stringToNum(T &t, const std::string &s, std::ios_base& (*f)(std::ios_base&))
{
  std::istringstream iss(s);
  return !(iss >> f >> t).fail();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
static bool stringToNum(T &t, const std::string &s)
{
  std::istringstream iss(s);
  return !(iss >> t).fail();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
static std::string numToString(int num)
{
	std::stringstream converter;
  converter << num;
  return converter.str();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
static std::string numToString(T num)
{
  std::stringstream converter;
  converter << num;
  return converter.str();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
static  std::string numToString(size_t num)
{
  std::stringstream converter;
  converter << num;
  return converter.str();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
static void addRightSlash(std::string &path)
{
  //if (! (path.at(path.length()-1) == '/')) {
    path += "/";
  //}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
static bool hasRightSlash(const std::string &path)
{
  return (path.at(path.length()-1) == '/');
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
/* Returns a copy of the list. Not memory efficient at all */
static std::list<std::string> splitString(std::string sep, std::string str)
{
  std::list<std::string> strings;
  std::string::size_type sep_length = sep.length();
  std::string::iterator iter = std::search(str.begin(), str.end(), sep.begin(), sep.end());
  strings.push_back(std::string(str.begin(), iter));
  std::string::iterator iter_start;
  while(iter != str.end()) {
    iter_start = iter+sep_length;
    iter = std::search(iter_start, str.end(), sep.begin(), sep.end());
    strings.push_back(std::string(iter_start, iter));
  }
  return strings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
static std::string joinList(std::string sep, std::list<std::string> strs)
{
  std::string result("");
  std::list<std::string>::const_iterator iter = strs.begin();
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
static std::string joinList(std::string sep, std::list<int> nums)
{
  std::string result("");

  std::list<int>::const_iterator iter = nums.begin();
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
static std::string makePath(const std::string parentPath, const std::string name)
{
  std::string result;
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
static bool endsWith(const std::string &base, const std::string ending) {
  std::string::size_type len = base.length();
  std::string::size_type eLen = ending.length();
  std::string::size_type npos = len - eLen;
  std::string::size_type pos = base.find(ending, npos);
  return   ( pos != std::string::npos);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
static std::string indent(int32_t depth)
{
  return std::string(2 * depth, ' ');
}

}; // end class def



#endif /*STRINGUTILITIES_H_*/
