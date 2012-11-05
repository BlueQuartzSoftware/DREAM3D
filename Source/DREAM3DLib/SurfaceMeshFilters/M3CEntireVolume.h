/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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

 //
 //  This code was written under United States Air Force Contract number
 //                           FA8650-07-D-5800
 //
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _MMCSurfaceMeshingFilter_H_
#define _MMCSurfaceMeshingFilter_H_

#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/SurfaceMeshStructs.h"
#include "DREAM3DLib/Common/AbstractSurfaceMeshAlgorithm.h"


/**
 * @class MMCSurfaceMeshingFilter MMCSurfaceMeshingFilter.h DREAM3DLic/SurfaceMeshingFilters/MMCSurfaceMeshingFilter.h
 * @brief This filter was contributed by Dr. Sukbin Lee of Carnegi-Mellon University and uses a "MultiMaterial Marching
 * Cubes" algorithm originally proposed by Wu & Sullivan.
 * Multiple material marching cubes algorithm, Ziji Wu1, John M. Sullivan Jr2, International Journal for Numerical Methods in Engineering
 * Special Issue: Trends in Unstructured Mesh Generation, Volume 58, Issue 2, pages 189–207, 14 September 2003
 * @author
 * @date
 * @version 1.0
 */
class DREAM3DLib_EXPORT M3CEntireVolume : public AbstractSurfaceMeshAlgorithm
{
  public:
    DREAM3D_SHARED_POINTERS(M3CEntireVolume)
    DREAM3D_STATIC_NEW_MACRO(M3CEntireVolume)
    DREAM3D_TYPE_MACRO_SUPER(M3CEntireVolume, AbstractFilter)

    virtual ~M3CEntireVolume();

    //------ Created Surface Mesh Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceMeshEdgesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceMeshInternalEdgesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceMeshNodeTypeArrayName)

    //------ Required Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)

    //--------- Input Parameters
    DREAM3D_INSTANCE_PROPERTY(bool, AddSurfaceLayer)

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::SurfaceMeshingFilters; }

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const std::string getHumanLabel() { return "Multi-Material Marching Cubes"; }

    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
    */
    virtual void setupFilterParameters();

    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
    virtual void writeFilterParameters(AbstractFilterParametersWriter* writer);

     /**
    * @brief Reimplemented from @see AbstractFilter class
    */
    virtual void execute();

    /**
    * @brief This function runs some sanity checks on the DataContainer and inputs
    * in an attempt to ensure the filter can process the inputs.
    */
    virtual void preflight();

  protected:
    M3CEntireVolume();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param fields The number of fields
    * @param ensembles The number of ensembles
    */
    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    virtual int createMesh();

    /**
     * @brief initialize_micro_from_grainIds
     * @param dims
     * @param res
     * @param fileDims
     * @param grainIds
     * @param points
     * @param voxelCoords
     * @return
     */
    virtual int initialize_micro_from_grainIds(size_t dims[3], float res[3], size_t fileDims[3],
                                       int32_t* grainIds,
                                       DataArray<int32_t>::Pointer points,
                                       VoxelCoord* point);


  private:

    M3CEntireVolume(const M3CEntireVolume&); // Copy Constructor Not Implemented
    void operator=(const M3CEntireVolume&); // Operator '=' Not Implemented
};

#endif /* _MMCSurfaceMeshingFilter_H_ */
