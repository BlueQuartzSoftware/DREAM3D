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

#ifndef AlignSectionsFeatureCentroid_H_
#define AlignSectionsFeatureCentroid_H_

#include <vector>
#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "Reconstruction/ReconstructionConstants.h"

#include "Reconstruction/ReconstructionFilters/AlignSections.h"


/**
 * @class AlignSectionsFeatureCentroid AlignSectionsFeatureCentroid.h DREAM3DLib/ReconstructionFilters/AlignSectionsFeatureCentroid.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
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

    virtual const QString getCompiledLibraryName() { return Reconstruction::ReconstructionBaseName; }
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName() { return DREAM3D::FilterGroups::ReconstructionFilters; }
    virtual const QString getSubGroupName() {return DREAM3D::FilterSubGroups::AlignmentFilters;}
    virtual const QString getHumanLabel() { return "Align Sections (Feature Centroid)"; }

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();
    virtual void preflight();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    AlignSectionsFeatureCentroid();

    virtual void find_shifts(QVector<int>& xshifts, QVector<int>& yshifts);

    virtual void setupChildUniqueFilterParameters(FilterParameterVector parameters);
    virtual void writeChildUniqueFilterParameters(AbstractFilterParametersWriter* writer, int index);
    virtual void readChildUniqueFilterParameters(AbstractFilterParametersReader* reader, int index);

  private:
    DEFINE_PTR_WEAKPTR_DATAARRAY(bool, GoodVoxels)

    void dataCheck();

    AlignSectionsFeatureCentroid(const AlignSectionsFeatureCentroid&); // Copy Constructor Not Implemented
    void operator=(const AlignSectionsFeatureCentroid&); // Operator '=' Not Implemented
};

#endif /* AlignSectionsFeatureCentroid_H_ */



