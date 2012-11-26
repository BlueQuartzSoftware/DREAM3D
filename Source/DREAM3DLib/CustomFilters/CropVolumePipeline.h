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

#ifndef CROP_VOLUME_PIPELINE_H_
#define CROP_VOLUME_PIPELINE_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DistributionAnalysisOps/DistributionAnalysisOps.h"


/**
 * @class FindShapes FindShapes.h DREAM3DLib/GenericFilters/FindShapes.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT CropVolumePipeline : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(CropVolumePipeline)
    DREAM3D_STATIC_NEW_MACRO(CropVolumePipeline)
    DREAM3D_TYPE_MACRO_SUPER(CropVolumePipeline, AbstractFilter)

    virtual ~CropVolumePipeline();

    DREAM3D_INSTANCE_STRING_PROPERTY(InputFile)

    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::CustomFilters;}
    virtual const std::string getHumanLabel() { return "Crop Volume Filter"; }


    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void setupFilterParameters();
    virtual void writeFilterParameters(AbstractFilterParametersWriter* writer);

    virtual void execute();
    virtual void preflight();

    virtual void get_max_and_min_xyz_for_crop(std::vector<int> &xmax, std::vector<int> &ymax, std::vector<int> &zmax, std::vector<int> &xmin, std::vector<int> &ymin, std::vector<int> &zmin);

  protected:
    CropVolumePipeline();



  private:
    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    CropVolumePipeline(const CropVolumePipeline&); // Copy Constructor Not Implemented
    void operator=(const CropVolumePipeline&); // Operator '=' Not Implemented
};

#endif /* CROP_VOLUME_PIPELINE_H_ */
