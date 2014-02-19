/*
 * AngReaderTest.cpp
 *
 *  Created on: Aug 24, 2012
 *      Author: mjackson
 */
#include <QtCore/QVector>

#include "EbsdLib/TSL/AngReader.h"


int main(int argc, char **argv)
{

  QVector<size_t> dim(1, 3);


  AngReader reader;
  reader.setFileName(argv[1]);
  int err = reader.readHeaderOnly();
  if (err < 0)
  {
    std::cout << "Error Trying to read the header for " << argv[1] << std::endl;
  }

  return 0;
}



