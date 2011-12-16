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

#ifndef PLACEPRECIPITATES_H_
#define PLACEPRECIPITATES_H_

#include <string>
#include <vector>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DataContainer.h"
#include "DREAM3DLib/ShapeOps/ShapeOps.h"

/**
 * @class PlacePrecipitates PlacePrecipitates.h DREAM3DLib/SyntheticBuilderFilters/PlacePrecipitates.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT PlacePrecipitates : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(PlacePrecipitates);
    DREAM3D_STATIC_NEW_MACRO(PlacePrecipitates);
    DREAM3D_TYPE_MACRO_SUPER(PlacePrecipitates, AbstractFilter);

    virtual ~PlacePrecipitates();

    DREAM3D_INSTANCE_STRING_PROPERTY(H5StatsFile)
    DREAM3D_INSTANCE_PROPERTY(bool, periodic_boundaries);

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */

    virtual void execute();

  protected:
    PlacePrecipitates();

    void insert_precipitate(size_t grainNum);
    void place_precipitates();
    void fillin_precipitates();
    float find_xcoord(long long int index);
    float find_ycoord(long long int index);
    float find_zcoord(long long int index);

    float totalprecipvol;
    std::vector<int> precipitatephases;
    std::vector<float> precipitatephasefractions;

  private:
	std::vector<int> currentprecipvoxellist;

	int numprimarygrains;
    unsigned long long int Seed;
    float sizex;
    float sizey;
    float sizez;
    float totalvol;
    std::map<DREAM3D::SyntheticBuilder::ShapeType, DREAM3D::ShapeOps*> m_ShapeOps;
    DREAM3D::ShapeOps::Pointer m_UnknownShapeOps;
    DREAM3D::ShapeOps::Pointer m_CubicOctohedronOps;
    DREAM3D::ShapeOps::Pointer m_CylinderOps;
    DREAM3D::ShapeOps::Pointer m_EllipsoidOps;
    DREAM3D::ShapeOps::Pointer m_SuprtEllipsoidOps;
    int32_t* grain_indicies;
    int32_t* phases;
    int8_t* surfacevoxels;

    PlacePrecipitates(const PlacePrecipitates&); // Copy Constructor Not Implemented
    void operator=(const PlacePrecipitates&); // Operator '=' Not Implemented
};

#endif /* PLACEPRECIPITATES_H_ */
