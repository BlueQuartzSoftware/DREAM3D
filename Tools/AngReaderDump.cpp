/*
 * AngReaderTest.cpp
 *
 *  Created on: Aug 24, 2012
 *      Author: mjackson
 */

#include "EbsdLib/TSL/AngReader.h"


int main(int argc, char **argv)
{
  AngReader reader;
  reader.setFileName(argv[1]);
  int err = reader.readHeaderOnly();
  if (err < 0)
  {
    std::cout << "Error Trying to read the header for " << argv[1] << std::endl;
  }

  return 0;
}



