/* ============================================================================
 * Copyright (c) 2014 DREAM3D Consortium
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
 * Neither the names of any of the DREAM3D Consortium contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *  This code was partially written under United States Air Force Contract number
 *                              FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _AlignSectionsPhaseCorrelation_H_
#define _AlignSectionsPhaseCorrelation_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

#include "DREAM3DLib/Common/TemplateHelpers.hpp"

#include "Plugins/Reconstruction/ReconstructionFilters/AlignSections.h"

/**
 * @class AlignSectionsPhaseCorrelation AlignSectionsPhaseCorrelation.h ImageProcessing/ImageProcessingFilters/AlignSectionsPhaseCorrelation.h
 * @brief aligns sections using phase correlation
 * @author will lenthe
 * @date 9/1/2014
 * @version 1.0
 */
class AlignSectionsPhaseCorrelation : public AlignSections
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(AlignSectionsPhaseCorrelation)
    DREAM3D_STATIC_NEW_MACRO(AlignSectionsPhaseCorrelation)
    DREAM3D_TYPE_MACRO_SUPER(AlignSectionsPhaseCorrelation, AlignSections)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedCellArrayPath)
    Q_PROPERTY(DataArrayPath SelectedCellArrayPath READ getSelectedCellArrayPath WRITE setSelectedCellArrayPath)

    virtual ~AlignSectionsPhaseCorrelation();

    DREAM3D_FILTER_PARAMETER(QString, InputFile)
    Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();
    virtual const QString getHumanLabel();
    virtual const QString getBrandingString() { return "DREAM3D Reconstruction Plugin"; }

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();
    virtual void preflight();


  protected:
    AlignSectionsPhaseCorrelation();

    virtual void find_shifts(QVector<int>& xshifts, QVector<int>& yshifts);

    virtual void setupFilterParameters();
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

  private:
    DEFINE_REQUIRED_IDATAARRAY_VARIABLE(SelectedCellArray)
    void dataCheck();

    AlignSectionsPhaseCorrelation(const AlignSectionsPhaseCorrelation&); // Copy Constructor Not Implemented
    void operator=(const AlignSectionsPhaseCorrelation&); // Operator '=' Not Implemented
};

#endif /* _AlignSectionsPhaseCorrelation_H_ */
