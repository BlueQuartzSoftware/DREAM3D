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

#include "Patch.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Patch::Patch()
{
  v_id[0] = 0; v_id[1] = 0; v_id[2] = 0; // stores three new node id for vertices of the triangles...
  new_v_id[0] = 0; new_v_id[1] = 0; new_v_id[2] = 0;
  ngrainname[0] = 0; ngrainname[1] = 0;
  edgePlace[0] = -1; edgePlace[1] = -1; edgePlace[2] = -1;
  normal[0] = 0.0; normal[1] = 0.0; normal[2] = 0.0;
  area = 0.0;
  triID = -1;
  e_id[0] = -1; e_id[1] = -1; e_id[2] = -1;
  nSpin[0] = 0; nSpin[1] = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Patch::~Patch()
{
}
