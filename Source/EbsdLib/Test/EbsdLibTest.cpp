 


#include "AngImportTest.cpp"
#include "CtfReaderTest.cpp"
#include "EdaxOIMReaderTest.cpp"



// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  AngImportTest()();
  CtfReaderTest()();
  EdaxOIMReaderTest()();

  int err = 0;
  PRINT_TEST_SUMMARY();

  return EXIT_SUCCESS;
}
