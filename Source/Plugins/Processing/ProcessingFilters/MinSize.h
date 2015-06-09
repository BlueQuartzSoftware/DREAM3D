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


#ifndef _MinSize_H_
#define _MinSize_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

/**
 * @brief The MinSize class. See [Filter documentation](@ref minsize) for details.
 */
class MinSize : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(MinSize)
    DREAM3D_STATIC_NEW_MACRO(MinSize)
    DREAM3D_TYPE_MACRO_SUPER(MinSize, AbstractFilter)

    virtual ~MinSize();

    DREAM3D_FILTER_PARAMETER(int, MinAllowedFeatureSize)
    Q_PROPERTY(int MinAllowedFeatureSize READ getMinAllowedFeatureSize WRITE setMinAllowedFeatureSize)

    DREAM3D_FILTER_PARAMETER(bool, ApplyToSinglePhase)
    Q_PROPERTY(bool ApplyToSinglePhase READ getApplyToSinglePhase WRITE setApplyToSinglePhase)

    DREAM3D_FILTER_PARAMETER(int, PhaseNumber)
    Q_PROPERTY(int PhaseNumber READ getPhaseNumber WRITE setPhaseNumber)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
    Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, NumCellsArrayPath)
    Q_PROPERTY(DataArrayPath NumCellsArrayPath READ getNumCellsArrayPath WRITE setNumCellsArrayPath)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();
    virtual const QString getHumanLabel();

    virtual void setupFilterParameters();
    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    virtual void execute();
    virtual void preflight();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    MinSize();

    /**
     * @brief assign_badpoints Coarsens those Features remaining in the structure after removing any Features
     * that do not have the required size.  The coarsening is intended to fill gaps left by the
     * removed Features and proceeds via an isotropic growth process.
     */
    void assign_badpoints();

    /**
     * @brief remove_smallfeatures Assigns a boolean value to Features dependent upon whether they meet
     * the supplied criterion for the minimum size.
     * @return QVector<bool> A vector of boolean values whose length is the number of Features.
     */
    QVector<bool> remove_smallfeatures();

  private:
    int32_t* m_Neighbors;

    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, FeatureIds)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, NumCells)

    void dataCheck();

    MinSize(const MinSize&); // Copy Constructor Not Implemented
    void operator=(const MinSize&); // Operator '=' Not Implemented
};

#endif /* MinSize_H_ */
