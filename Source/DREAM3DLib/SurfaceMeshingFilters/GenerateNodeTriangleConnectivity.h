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

#ifndef _GenerateNodeTriangleConnectivity_H_
#define _GenerateNodeTriangleConnectivity_H_

#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"


/**
 * @class GenerateNodeTriangleConnectivity GenerateNodeTriangleConnectivity.h /SurfaceMeshFilters/GenerateNodeTriangleConnectivity.h
 * @brief This filter creates a pair of arrays that help to describe the connectivity in a triangle
 * based surface mesh. The two arrays are:
 * @li UniqueEdges Array - This is a 2 component array where each component is a vertex id and the pair of
 * vertex ids make up the Edge Id (64 bit unique value). The index into the array that the pair occurs is
 * used internally in other referencing arrays but pairs are guaranteed to be unique.
 * @li TriangleEdges Array - This array is a 3 component array where the values of each component is the index
 * into the UniqueEdges array. Since each triangle can only have 3 Edges.
 * @author Michael A. Jackson (BlueQuartz Software)
 * @date  Dec 2012
 * @version 1.0
 */
class DREAM3DLib_EXPORT GenerateNodeTriangleConnectivity : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(GenerateNodeTriangleConnectivity)
    DREAM3D_STATIC_NEW_MACRO(GenerateNodeTriangleConnectivity)
    DREAM3D_TYPE_MACRO_SUPER(GenerateNodeTriangleConnectivity, AbstractFilter)

    virtual ~GenerateNodeTriangleConnectivity();

    /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */
    DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceMeshUniqueEdgesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceMeshTriangleEdgesArrayName)

 //   NodeTrianglesMap_t& getNode2TriangleMap();

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
    virtual const std::string getHumanLabel() { return "Generate Node's Triangle List"; }

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
    GenerateNodeTriangleConnectivity();

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
 //   int32_t* m_SurfaceMeshUniqueEdges;
    int32_t* m_SurfaceMeshTriangleEdges;


    /**
     * @brief generateConnectivity This is the method that actually implements the algorithm.
     */
    void generateConnectivity();


    GenerateNodeTriangleConnectivity(const GenerateNodeTriangleConnectivity&); // Copy Constructor Not Implemented
    void operator=(const GenerateNodeTriangleConnectivity&); // Operator '=' Not Implemented
};

#endif /* _GenerateNodeTriangleConnectivity_H_ */
