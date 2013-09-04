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


//-- C++ Includes
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>

#define NUM_COLS 70


namespace DREAM3D
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
      updateWhat();
    }

    /**
    * @brief Copy Constructor
    */
    TestException(const TestException &te)
    {
      m_Message = (&te)->getMessage();
      m_FileName = te.getFileName();
      m_LineNumber = te.getLineNumber();
      updateWhat();
    }

    virtual ~TestException() throw() {
    }

    /**
     * @brief Over ride from base class
     */
    virtual const char* what() const throw()
    {
      return m_What;
    }

    void setMessage(const std::string &m)
    {
      m_Message = m;
      updateWhat();
    }
    std::string getMessage()
    {
      return m_Message;
    }
    std::string getMessage() const { return m_Message; }

    void setFileName(const std::string &fn)
    {
      m_FileName = fn;
      updateWhat();
    }
    std::string getFileName()
    {
      return m_FileName;
    }
    std::string getFileName() const { return m_FileName; }

    void setLineNumber(int ln)
    {
      m_LineNumber = ln;
      updateWhat();
    }
    int getLineNumber()
    {
      return m_LineNumber;
    }
    int getLineNumber() const { return m_LineNumber; }

  protected:
    TestException() {
      updateWhat();
    }

    void updateWhat()
    {
      std::stringstream ss;
      ss << "    Reason: " << m_Message << std::endl;
      ss << "    File:   " << m_FileName << std::endl;
      ss << "    Line:   " << m_LineNumber;

      ::memset(m_What, 0, 2048);
      ::memcpy(m_What, ss.str().c_str(), ss.str().size() );
      m_What[2047] = 0; // Make sure we NULL terminate no matter what.
    }

  private:
    std::string m_Message;
    std::string m_FileName;
    int m_LineNumber;

    char m_What[2048];
    void operator=(const TestException&);  // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestPassed(const std::string &test)
{
  ::memset(DREAM3D::unittest::TestMessage, ' ', NUM_COLS); // Splat Spaces across the entire message
  DREAM3D::unittest::TestMessage[NUM_COLS] = 0;  // Make sure it is null terminated

  std::string::size_type size = NUM_COLS - DREAM3D::unittest::SizeOfPassed;
  ::strncpy( &(DREAM3D::unittest::TestMessage[size]) , DREAM3D::unittest::Passed, DREAM3D::unittest::SizeOfPassed );
  if (test.length() < size )
  {
    ::strncpy(DREAM3D::unittest::TestMessage, test.c_str(), test.length());
  }
  else
  {
    ::strncpy(DREAM3D::unittest::TestMessage, test.substr(0, size).c_str(), size);
  }
  DREAM3D::unittest::TestMessage[NUM_COLS] = 0;  // Make sure it is null terminated
  std::cout << DREAM3D::unittest::TestMessage << std::endl;
  DREAM3D::unittest::numTestsPass++;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestFailed(const std::string &test)
{
  ::memset(DREAM3D::unittest::TestMessage, ' ', NUM_COLS); // Splat Spaces across the entire message
  DREAM3D::unittest::TestMessage[NUM_COLS] = 0;  // Make sure it is null terminated

  std::string::size_type size = NUM_COLS - DREAM3D::unittest::SizeOfFailed;
  ::strncpy( &(DREAM3D::unittest::TestMessage[size]) , DREAM3D::unittest::Failed, DREAM3D::unittest::SizeOfFailed );
  if (test.length() < size )
  {
    ::strncpy(DREAM3D::unittest::TestMessage, test.c_str(), test.length());
  }
  else
  {
    ::strncpy(DREAM3D::unittest::TestMessage, test.substr(0, size).c_str(), size);
  }
  DREAM3D::unittest::TestMessage[NUM_COLS] = 0;  // Make sure it is null terminated
  std::cout << DREAM3D::unittest::TestMessage << std::endl;
  DREAM3D::unittest::numTestFailed++;
}


// -----------------------------------------------------------------------------
// Developer Used Macros
// -----------------------------------------------------------------------------
#define DREAM3D_REQUIRE( P ) \
{ \
  bool b = (P);\
  if ( (b) == (false) ) \
{\
  std::string s ("Your test required the following\n            '");\
  s = s.append(#P).append("'\n             but this condition was not met.");\
  DREAM3D_TEST_THROW_EXCEPTION( s )\
  }\
  }

#define DREAM3D_REQUIRED(L, Q, R)\
{ \
  std::stringstream ss;\
  bool b = (L Q R);\
  if ( (b) == (false) ) \
{\
  ss <<"Your test required the following\n            '";\
  ss << #L << " " << #Q << " " << #R << "' but this condition was not met.\n";\
  ss << "            " << #L << " = " << L << "\n";\
  ss << "            " << #R << " = " << R << "\n";\
  DREAM3D_TEST_THROW_EXCEPTION( ss.str() )\
  }\
  }

#define DREAM3D_REQUIRE_NE( L, R )\
  if ( (L) == (R) ) {  \
  std::stringstream ss;\
  ss << "Your test required the following\n            '";\
  ss << #L << " != " << #R << "'\n             but this condition was not met.\n";\
  ss << "             " << L << "==" << R;\
  DREAM3D_TEST_THROW_EXCEPTION( ss.str() ) }



#define DREAM3D_REQUIRE_EQUAL( L, R) \
  if ( (L) != (R) ) {  \
  std::stringstream ss;\
  ss << "Your test required the following\n            '";\
  ss << #L << " == " << #R << "'\n             but this condition was not met.\n";\
  ss << "             " << L << "==" << R;\
  DREAM3D_TEST_THROW_EXCEPTION( ss.str() ) }




// -----------------------------------------------------------------------------
// Private Macros. The Normal developer should NOT be using these.
// -----------------------------------------------------------------------------
#define DREAM3D_TEST_THROW_EXCEPTION( P)\
  throw TestException( P, __FILE__, __LINE__);\


#define DREAM3D_ASSERT( P )\
  assert( (P) );




#define DREAM3D_ENTER_TEST( test )\
  DREAM3D::unittest::CurrentMethod = #test;\
  DREAM3D::unittest::numTests++;


#define DREAM3D_LEAVE_TEST( test )\
  TestPassed(#test);\
  DREAM3D::unittest::CurrentMethod = "";


#define DREAM3D_REGISTER_TEST( test )\
  try {\
  DREAM3D_ENTER_TEST(test);\
  test;\
  DREAM3D_LEAVE_TEST(test)\
  } catch (TestException& e)\
{\
  TestFailed(DREAM3D::unittest::CurrentMethod);\
  std::cout << e.what() << std::endl;\
  err = EXIT_FAILURE;\
  }

#define PRINT_TEST_SUMMARY()\
  std::cout << "Test Summary:" << std::endl;\
  std::cout << "  Tests Passed: " << DREAM3D::unittest::numTestsPass << std::endl;\
  std::cout << "  Tests Failed: " << DREAM3D::unittest::numTestFailed << std::endl;\
  std::cout << "  Total Tests:  " << DREAM3D::unittest::numTests << std::endl;\
  if (DREAM3D::unittest::numTestFailed > 0)\
{\
  err = EXIT_FAILURE;\
  }\





#endif /* UNITTESTSUPPORT_HPP_ */
