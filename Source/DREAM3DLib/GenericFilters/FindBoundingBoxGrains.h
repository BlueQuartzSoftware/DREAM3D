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

#ifndef FINDBOUNDINGBOXGRAINS_H_
#define FINDBOUNDINGBOXGRAINS_H_

#include <vector>
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/VolumeDataContainer.h"

/**
 * @class FindBoundingBoxGrains FindBoundingBoxGrains.h DREAM3DLib/GenericFilters/FindBoundingBoxGrains.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT FindBoundingBoxGrains : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(FindBoundingBoxGrains)
    DREAM3D_STATIC_NEW_MACRO(FindBoundingBoxGrains)
    DREAM3D_TYPE_MACRO_SUPER(FindBoundingBoxGrains, AbstractFilter)

    virtual ~FindBoundingBoxGrains();

	//------ Required Field Data
	DREAM3D_INSTANCE_STRING_PROPERTY(CentroidsArrayName)
	DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceFieldsArrayName)
	//------ Created Field Data
	DREAM3D_INSTANCE_STRING_PROPERTY(BiasedFieldsArrayName)

    DECLARE_WRAPPED_ARRAY(totalsurfacearea, m_TotalSurfaceArea, float)

    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::GenericFilters; }
	virtual const std::string getSubGroupName() { return DREAM3D::FilterSubGroups::SpatialFilters; }
    virtual const std::string getHumanLabel() { return "Find Biased Fields (Bounding Box)"; }


	virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);
    
    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

	/**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();
    virtual void preflight();

    void find_boundingboxgrains();
    void find_boundingboxgrains2D();


  protected:
    FindBoundingBoxGrains();

  private:
    float* m_Centroids;
    bool* m_SurfaceFields;
    bool* m_BiasedFields;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);


    FindBoundingBoxGrains(const FindBoundingBoxGrains&); // Copy Constructor Not Implemented
    void operator=(const FindBoundingBoxGrains&); // Operator '=' Not Implemented
};

#endif /* FINDBOUNDINGBOXGRAINS_H_ */
