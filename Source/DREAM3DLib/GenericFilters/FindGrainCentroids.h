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
#ifndef FindGrainCentroids_H_
#define FindGrainCentroids_H_

#include <vector>
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/VoxelDataContainer.h"

/*
 *
 */
class DREAM3DLib_EXPORT FindGrainCentroids : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(FindGrainCentroids)
    DREAM3D_STATIC_NEW_MACRO(FindGrainCentroids)
    DREAM3D_TYPE_MACRO_SUPER(FindGrainCentroids, AbstractFilter)

    virtual ~FindGrainCentroids();

    //------ Required Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)
    //------ Created Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(CentroidsArrayName)

    DREAM3D_INSTANCE_STRING_PROPERTY(ActiveArrayName)



    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::GenericFilters; }
    virtual const std::string getSubGroupName() { return DREAM3D::FilterSubGroups::MiscFilters; }
    virtual const std::string getHumanLabel() { return "Find Field Centroids"; }

    virtual void writeFilterParameters(AbstractFilterParametersWriter* writer);

    virtual void execute();
    virtual void preflight();

  protected:
    FindGrainCentroids();

    void find_centroids();
    void find_centroids2D();

  private:
    int32_t* m_GrainIds;
    float* m_Centroids;

    DECLARE_WRAPPED_ARRAY(graincenters, m_GrainCenters, float) // N x 5 Array


    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    FindGrainCentroids(const FindGrainCentroids&); // Copy Constructor Not Implemented
    void operator=(const FindGrainCentroids&); // Operator '=' Not Implemented

};

#endif /* FindGrainCentroids_H_ */
