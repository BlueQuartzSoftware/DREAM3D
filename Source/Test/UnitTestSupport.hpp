/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef UNITTESTSUPPORT_HPP_
#define UNITTESTSUPPORT_HPP_

#include "MXA/MXA.h"
#include <MXA/Common/MXASetGetMacros.h>
//#include <MXA/Core/MXADataModel.h>
//#include <MXA/Core/MXADataDimension.h>
//#include <MXA/Core/MXADataRecord.h>
//#include <MXA/Core/MXASupportFile.h>
//#include <MXA/DataWrappers/MXAAsciiStringData.h>


//-- C++ Includes
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>

#define NUM_COLS 70


namespace mxa
{
namespace unittest
{
  static std::string CurrentMethod("");
  static int numTestsPass = 0;
  static int numTestFailed = 0;
  static int numTests = 0;

  static char TestMessage[NUM_COLS + 1];
  static const char Passed[6] = { 'P', 'A', 'S', 'S', 'E', 'D'};
  static const char Failed[6] = { 'F', 'A', 'I', 'L', 'E', 'D'};
  static int SizeOfPassed = 6;
  static int SizeOfFailed = 6;
}
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class TestException : public std::exception
{
  public:
    /**
    * @brief
    * @param what
    * @param file
    * @param lineNumber
    */
     TestException(const std::string &what, const std::string &filename, int lineNumber) :
      m_Message(what), m_FileName(filename), m_LineNumber(lineNumber)
    {
    }

    /**
    * @brief Copy Constructor
    */
     TestException(const TestException &te)
     {
       m_Message = (&te)->getMessage();
       m_FileName = te.getFilename();
       m_LineNumber = te.getLineNumber();
     }

     virtual ~TestException() throw() {}

     /**
     * @brief Over ride from base class
     */
     std::string what() {
       std::stringstream ss;
       ss << "    Reason: " << m_Message << std::endl;
       ss << "    File:   " << m_FileName << std::endl;
       ss << "    Line:   " << m_LineNumber;
       return ss.str();
     }

     MXA_INSTANCE_STRING_PROPERTY(Message)
       std::string getMessage() const { return m_Message; }
     MXA_INSTANCE_STRING_PROPERTY(FileName)
       std::string getFilename() const { return m_FileName; }

     MXA_INSTANCE_PROPERTY(int, LineNumber)
       int getLineNumber() const { return m_LineNumber; }

  protected:
     TestException() {};

  private:


      void operator=(const TestException&);  // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestPassed(const std::string &test)
{
  ::memset(mxa::unittest::TestMessage, ' ', NUM_COLS); // Splat Spaces across the entire message
  mxa::unittest::TestMessage[NUM_COLS] = 0;  // Make sure it is null terminated

  std::string::size_type size = NUM_COLS - mxa::unittest::SizeOfPassed;
  ::strncpy( &(mxa::unittest::TestMessage[size]) , mxa::unittest::Passed, mxa::unittest::SizeOfPassed );
  if (test.length() < size )
  {
    ::strncpy(mxa::unittest::TestMessage, test.c_str(), test.length());
  }
  else
  {
    ::strncpy(mxa::unittest::TestMessage, test.substr(0, size).c_str(), size);
  }
  mxa::unittest::TestMessage[NUM_COLS] = 0;  // Make sure it is null terminated
  std::cout << mxa::unittest::TestMessage << std::endl;
  mxa::unittest::numTestsPass++;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestFailed(const std::string &test)
{
  ::memset(mxa::unittest::TestMessage, ' ', NUM_COLS); // Splat Spaces across the entire message
  mxa::unittest::TestMessage[NUM_COLS] = 0;  // Make sure it is null terminated

  std::string::size_type size = NUM_COLS - mxa::unittest::SizeOfFailed;
  ::strncpy( &(mxa::unittest::TestMessage[size]) , mxa::unittest::Failed, mxa::unittest::SizeOfFailed );
  if (test.length() < size )
  {
    ::strncpy(mxa::unittest::TestMessage, test.c_str(), test.length());
  }
  else
  {
    ::strncpy(mxa::unittest::TestMessage, test.substr(0, size).c_str(), size);
  }
  mxa::unittest::TestMessage[NUM_COLS] = 0;  // Make sure it is null terminated
  std::cout << mxa::unittest::TestMessage << std::endl;
  mxa::unittest::numTestFailed++;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#define MXA_TEST_THROW_EXCEPTION( P)\
      throw TestException( P, __FILE__, __LINE__);\


#define MXA_ASSERT( P )\
  assert( (P) );


#define MXA_REQUIRE( P ) \
  { \
  bool b = (P);\
  if ( (b) == (false) ) \
  {\
    std::string s ("Your test required the following\n            '");\
    s = s.append(#P).append("'\n            but this condition was not met.");\
    MXA_TEST_THROW_EXCEPTION( s )\
  }\
  }


#define MXA_REQUIRE_NE( L, R )\
  if ( (L) != (R) )\
  { std::string s ( #L);\
   s = s.append("==").append(#R);\
   MXA_TEST_THROW_EXCEPTION( s ) }


#define MXA_REQUIRE_EQUAL( L, R) \
    if ( (L) != (R) )\
    { std::string s ( #L);\
     s = s.append("==").append(#R);\
    MXA_TEST_THROW_EXCEPTION( s ) }


#define MXA_ENTER_TEST( test )\
  mxa::unittest::CurrentMethod = #test;\
  mxa::unittest::numTests++;


#define MXA_LEAVE_TEST( test )\
  TestPassed(#test);\
  mxa::unittest::CurrentMethod = "";


#define MXA_REGISTER_TEST( test )\
    try {\
      MXA_ENTER_TEST(test);\
      test;\
      MXA_LEAVE_TEST(test)\
    } catch (TestException& e)\
    {\
      TestFailed(mxa::unittest::CurrentMethod);\
      std::cout << e.what() << std::endl;\
      err = EXIT_FAILURE;\
    }

#define PRINT_TEST_SUMMARY()\
    std::cout << "Test Summary:" << std::endl;\
    std::cout << "  Tests Passed: " << mxa::unittest::numTestsPass << std::endl;\
    std::cout << "  Tests Failed: " << mxa::unittest::numTestFailed << std::endl;\
    std::cout << "  Total Tests:  " << mxa::unittest::numTests << std::endl;\
    if (mxa::unittest::numTestFailed > 0)\
    {\
      err = EXIT_FAILURE;\
    }\





#endif /* UNITTESTSUPPORT_HPP_ */
