/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "ReconstructionFunc.h"

// C Includes
#include <stdio.h>

// C++ Includes
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <set>

// TSL Ang Library Includes
#include "EbsdLib/TSL/AngReader.h"

//-- MXA Includes
#include "MXA/Common/MXAEndian.h"
#include "MXA/Utilities/MXADir.h"

//-- AIM Includes
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/EbsdColoring.hpp"
#include "DREAM3DLib/HDF5/AIM_H5VtkDataWriter.h"


#define NEW_SHARED_ARRAY(var, type, size)\
  boost::shared_array<type> var##Array(new type[size]);\
  type* var = var##Array.get();

//const static float m_OnePointThree = 1.33333333333;
//const static float threesixty_over_pi = 360.0 / m_pi;
//const static float m_pi_over_180 = M_PI / 180.0;
const static float sqrt_two = powf(2.0, 0.5);
const static float acos_neg_one = acos(-1.0);
const static float acos_pos_one = acos(1.0);
const static float sin_wmin_neg_1_over_2 = sinf(acos_neg_one / 2.0);
const static float sin_wmin_pos_1_over_2 = sinf(acos_pos_one / 2.0);

#define DIMS "DIMENSIONS"
#define LOOKUP "LOOKUP_TABLE"

#if 0
// -i C:\Users\GroebeMA\Desktop\NewFolder --outputDir C:\Users\GroebeMA\Desktop\NewFolder -f Slice_ --angMaxSlice 400 -s 1 -e 30 -z 0.25 -t -g 10 -c 0.1 -o 5.0 -x 2
#endif

using namespace std;

ReconstructionFunc::ReconstructionFunc()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReconstructionFunc::~ReconstructionFunc()
{
  // std::cout << "~ReconstructionFunc" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
