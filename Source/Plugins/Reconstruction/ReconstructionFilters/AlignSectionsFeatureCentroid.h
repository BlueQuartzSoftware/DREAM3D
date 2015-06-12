/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


#ifndef _AlignSectionsFeatureCentroid_H_
#define _AlignSectionsFeatureCentroid_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

#include "Reconstruction/ReconstructionFilters/AlignSections.h"

/**
 * @brief The AlignSectionsFeatureCentroid class. See [Filter documentation](@ref alignsectionsfeaturecentroid) for details.
 */
class AlignSectionsFeatureCentroid : public AlignSections
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(AlignSectionsFeatureCentroid)
    DREAM3D_STATIC_NEW_MACRO(AlignSectionsFeatureCentroid)
    DREAM3D_TYPE_MACRO_SUPER(AlignSectionsFeatureCentroid, AlignSections)

    virtual ~AlignSectionsFeatureCentroid();

    DREAM3D_FILTER_PARAMETER(int, ReferenceSlice)
    Q_PROPERTY(int ReferenceSlice READ getReferenceSlice WRITE setReferenceSlice)

    DREAM3D_FILTER_PARAMETER(bool, UseReferenceSlice)
    Q_PROPERTY(bool UseReferenceSlice READ getUseReferenceSlice WRITE setUseReferenceSlice)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, GoodVoxelsArrayPath)
    Q_PROPERTY(DataArrayPath GoodVoxelsArrayPath READ getGoodVoxelsArrayPath WRITE setGoodVoxelsArrayPath)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();
    virtual const QString getHumanLabel();


    virtual void setupFilterParameters();
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();
    virtual void preflight();

  protected:
    AlignSectionsFeatureCentroid();

    /**
     * @brief find_shifts Reimplemented from @see AlignSections class
     */
    virtual void find_shifts(std::vector<int64_t>& xshifts, std::vector<int64_t>& yshifts);

  private:
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(bool, GoodVoxels)

    void dataCheck();

    AlignSectionsFeatureCentroid(const AlignSectionsFeatureCentroid&); // Copy Constructor Not Implemented
    void operator=(const AlignSectionsFeatureCentroid&); // Operator '=' Not Implemented
};

#endif /* AlignSectionsFeatureCentroid_H_ */