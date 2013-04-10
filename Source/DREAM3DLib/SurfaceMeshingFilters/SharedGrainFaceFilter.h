/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#ifndef _GrainFaceFilter_H_
#define _GrainFaceFilter_H_

#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"
#include "DREAM3DLib/SurfaceMeshingFilters/SurfaceMeshFilter.h"

/**
 * @class SharedGrainFaceFilter SharedGrainFaceFilter.h DREAM3DLib/SurfaceMeshingFilters/SharedGrainFaceFilter.h
 * @brief This filter groups triangles together in a way where each group would define a grain face where the group
 * of triangles share a common pair of grain ids or region ids.
 * @author Michael A. Jackson (BlueQuartz Software)
 * @date Dec 28, 2012
 * @version 1.0
 */
class DREAM3DLib_EXPORT SharedGrainFaceFilter : public SurfaceMeshFilter
{
  public:
    DREAM3D_SHARED_POINTERS(SharedGrainFaceFilter)
    DREAM3D_STATIC_NEW_MACRO(SharedGrainFaceFilter)
    DREAM3D_TYPE_MACRO_SUPER(SharedGrainFaceFilter, SurfaceMeshFilter)

    virtual ~SharedGrainFaceFilter();

    /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */
    typedef std::vector<int> FaceIds_t;
    typedef std::map<int32_t, FaceIds_t> SharedGrainFaces_t;


    DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceMeshGrainFaceIdArrayName)

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::SurfaceMeshingFilters; }
    virtual const std::string getSubGroupName() { return DREAM3D::FilterSubGroups::ConnectivityArrangementFilters; }

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const std::string getHumanLabel() { return "Shared Grain Face Filter"; }

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


    SharedGrainFaces_t& getSharedGrainFaces();

  protected:
    SharedGrainFaceFilter();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param fields The number of fields
    * @param ensembles The number of ensembles
    */
    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

  private:
    SharedGrainFaces_t m_SharedGrainFaces;

    SharedGrainFaceFilter(const SharedGrainFaceFilter&); // Copy Constructor Not Implemented
    void operator=(const SharedGrainFaceFilter&); // Operator '=' Not Implemented
};

#endif /* _GrainFaceFilter_H_ */
