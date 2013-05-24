/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef FINDSHAPES_H_
#define FINDSHAPES_H_

#include <vector>
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/VoxelDataContainer.h"


/**
 * @class FindShapes FindShapes.h DREAM3DLib/GenericFilters/FindShapes.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT FindShapes : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(FindShapes)
    DREAM3D_STATIC_NEW_MACRO(FindShapes)
    DREAM3D_TYPE_MACRO_SUPER(FindShapes, AbstractFilter)

    virtual ~FindShapes();

    //------ Required Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)
    //------ Required Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(CentroidsArrayName)
    //------ Created Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(VolumesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(AspectRatiosArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(AxisEulerAnglesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(AxisLengthsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(Omega3sArrayName)

    DREAM3D_DECLARE_ARRAY(double, grainmoments, GrainMoments) // N x 6 Array
    DREAM3D_DECLARE_ARRAY(double, graineigenvals, GrainEigenVals) // N x 3 Array

    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::StatisticsFilters; }
    virtual const std::string getSubGroupName() { return DREAM3D::FilterSubGroups::MorphologicalFilters; }
    virtual const std::string getHumanLabel() { return "Find Field Shapes"; }


    /**
     * @brief Reimplemented from @see AbstractFilter class
     */

    virtual void setupFilterParameters();
    virtual void writeFilterParameters(AbstractFilterParametersWriter* writer);

    virtual void execute();
    virtual void preflight();


  protected:
    FindShapes();

    void find_moments();
    void find_moments2D();
    void find_axes();
    void find_axes2D();
    void find_axiseulers();
    void find_axiseulers2D();
    float find_xcoord(size_t index);
    float find_ycoord(size_t index);
    float find_zcoord(size_t index);

  private:
    int32_t* m_GrainIds;
    float* m_AxisEulerAngles;
    float* m_Centroids;
    float* m_AxisLengths;
    float* m_Omega3s;
    float* m_Volumes;
    float* m_AspectRatios;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    FindShapes(const FindShapes&); // Copy Constructor Not Implemented
    void operator=(const FindShapes&); // Operator '=' Not Implemented
};

#endif /* FINDSHAPES_H_ */
