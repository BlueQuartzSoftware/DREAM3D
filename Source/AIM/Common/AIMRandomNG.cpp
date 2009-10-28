///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////
#include "AIMRandomNG.h"
#include <math.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMRandomNG::AIMRandomNG()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMRandomNG::~AIMRandomNG()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double AIMRandomNG::Random()
{
  double c;
  c = (double)2111111111.0 * x[3] +
      1492.0 * (x[3] = x[2]) +
      1776.0 * (x[2] = x[1]) +
      5115.0 * (x[1] = x[0]) +
      x[4];
  x[4] = floorl(c);
  x[0] = c - x[4];
  x[4] = x[4] * (1./(65536.*65536.));
  return x[0];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMRandomNG::RandomInit (uint32 seed)
{
  int i;
  uint32 s = seed;
  // make random numbers and put them into the buffer
  for (i=0; i<5; i++) {
    s = s * 29943829 - 1;
    x[i] = s * (1./(65536.*65536.));}
  // randomize some more
  for (i=0; i<19; i++) Random();
}
