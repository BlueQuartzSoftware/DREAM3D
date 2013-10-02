/*
 * CtfReaderTest.cpp
 *
 *  Created on: Aug 24, 2012
 *      Author: mjackson
 */

#include <stdio.h>


#include "EbsdLib/HKL/CtfReader.h"


#include "EbsdLib/TSL/AngReader.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"


int main(int argc, char *argv[])
{

  AngReader reader;
  reader.setFileName("/Users/Shared/Data/Ang_Data/Small_IN100/Small_IN100_1.ang");
  int err = reader.readFile();
  if(err < 0) { return EXIT_FAILURE; }
  int count = reader.getNumberOfElements();

  float* phi1 = reader.getPhi1Pointer();
  float* phi = reader.getPhiPointer();
  float* phi2 = reader.getPhi2Pointer();
  float* x = reader.getXPositionPointer();
  float* y = reader.getYPositionPointer();
  int* phase = reader.getPhaseDataPointer();
  float* ci = reader.getConfidenceIndexPointer();
  float* iq = reader.getImageQualityPointer();

  printf("Phase\tX\tY\tBands\tError\tEuler1\tEuler2\tEuler3\tMAD\tBC\tBS\n");

  int error = 0;

  for(int i = 0; i < count; ++i)
  {
    if (ci[i] > 0.1 && iq[i] > 120) { error = 0; }
    else { error = 1; }
  printf("%d\t%.4f\t%.4f\t10\t%d\t%.3f\t%.3f\t%.3f\t%.3f\t200\t0\n", phase[i]+1,x[i], y[i], error,
                                          phi1[i]*180.0/M_PI, phi[i]*180.0/M_PI, phi2[i]*180.0/M_PI, iq[i]  );


  }

  return 0;
}











#if 0
int main(int argc, char **argv)
{
  CtfReader reader;
  reader.setFileName(argv[1]);
  int err = reader.readHeaderOnly();
  if (err < 0)
  {
    std::cout << "Error Trying to read the header for " << argv[1] << std::endl;
  }

  return 0;
}

#endif


