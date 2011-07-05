/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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

#include "GrainGeneratorVoxel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGeneratorVoxel::GrainGeneratorVoxel() :
grain_index(0),
phase(0),
euler1(-1.0),
euler2(-1.0),
euler3(-1.0),
neighbor(-1),
surfacevoxel(0),
numowners(0)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGeneratorVoxel::~GrainGeneratorVoxel()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorVoxel::deepCopy(GrainGeneratorVoxel* graingeneratorvoxel)
{
  if (graingeneratorvoxel == this) { return; } // The pointers are the same just return

   grain_index = graingeneratorvoxel->grain_index;

   euler1 = graingeneratorvoxel->euler1;
   euler2 = graingeneratorvoxel->euler2;
   euler3 = graingeneratorvoxel->euler3;
   neighbor = graingeneratorvoxel->neighbor;
   numowners = graingeneratorvoxel->numowners;
   surfacevoxel = graingeneratorvoxel->surfacevoxel;

   COPY_ARRAY_5(quat, graingeneratorvoxel);

}
