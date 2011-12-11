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

#ifndef SURFACEMESH_H_
#define SURFACEMESH_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractPipeline.h"
#include "DREAM3DLib/Common/Observer.h"

#include "DREAM3DLib/SurfaceMeshingFilters/SurfaceMeshFilter.h"




/**
* @class SurfaceMesh SurfaceMesh.h AIM/SurfaceMesh/SurfaceMesh.h
* @brief This class serves as the main entry point to execute the Surface Mesh
* codes.
* @author Michael A. Jackson for BlueQuartz Software,
* @author Dr. Michael Groeber, USAFRL
* @date Nov 3, 2009
* @version 1.0
*/
class SurfaceMesh : public AbstractPipeline, public Observer
{
  public:
    DREAM3D_SHARED_POINTERS(SurfaceMesh);
    DREAM3D_TYPE_MACRO(SurfaceMesh);
    DREAM3D_STATIC_NEW_MACRO(SurfaceMesh);

    virtual ~SurfaceMesh();

    DREAM3D_INSTANCE_STRING_PROPERTY(InputFile)

    DREAM3D_INSTANCE_STRING_PROPERTY(VisualizationFile)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteConformalMesh)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteBinaryVTKFiles)

    DREAM3D_INSTANCE_PROPERTY(bool, WriteSTLFile)
    DREAM3D_INSTANCE_STRING_PROPERTY(OutputDirectory)
    DREAM3D_INSTANCE_STRING_PROPERTY(OutputFilePrefix)


    DREAM3D_INSTANCE_PROPERTY(bool, SmoothMesh)
    DREAM3D_INSTANCE_PROPERTY(int, SmoothIterations)
    DREAM3D_INSTANCE_PROPERTY(int, SmoothFileOutputIncrement)
    DREAM3D_INSTANCE_PROPERTY(bool, SmoothLockQuadPoints)

    DREAM3D_INSTANCE_PROPERTY(bool, DeleteTempFiles)

    /**
     * @brief Main method to run the operation
     */
    void execute();


  protected:

    SurfaceMesh();

  private:
	  SurfaceMeshFilter::Pointer m;


    SurfaceMesh(const SurfaceMesh&);    // Copy Constructor Not Implemented
    void operator=(const SurfaceMesh&);  // Operator '=' Not Implemented
};


#endif /* SURFACEMESH_H_ */
