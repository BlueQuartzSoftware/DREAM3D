/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#ifndef _generatenodetriangleconnectivity_h_
#define _generatenodetriangleconnectivity_h_

#include <string>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/Common/AbstractFilter.h"


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
class GenerateNodeTriangleConnectivity : public AbstractFilter
{
    Q_OBJECT
  public:
    SIMPL_SHARED_POINTERS(GenerateNodeTriangleConnectivity)
    SIMPL_STATIC_NEW_MACRO(GenerateNodeTriangleConnectivity)
    SIMPL_TYPE_MACRO_SUPER(GenerateNodeTriangleConnectivity, AbstractFilter)

    virtual ~GenerateNodeTriangleConnectivity();

    /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */
    SIMPL_INSTANCE_STRING_PROPERTY(SurfaceMeshUniqueEdgesArrayName)

//   NodeTrianglesMap_t& getNode2TriangleMap();

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const QString getGroupName() { return SIMPL::FilterGroups::SurfaceMeshingFilters; }

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel() { return "Generate Node's Triangle List"; }

    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
    */
    virtual void setupFilterParameters();

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

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
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();


  private:
//   int32_t* m_SurfaceMeshUniqueEdges;
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, SurfaceMeshTriangleEdges)


    /**
     * @brief generateConnectivity This is the method that actually implements the algorithm.
     */
    void generateConnectivity();


    GenerateNodeTriangleConnectivity(const GenerateNodeTriangleConnectivity&); // Copy Constructor Not Implemented
    void operator=(const GenerateNodeTriangleConnectivity&); // Operator '=' Not Implemented
};

#endif /* _GenerateNodeTriangleConnectivity_H_ */

