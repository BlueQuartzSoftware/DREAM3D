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


#ifndef _FindNeighbors_H_
#define _FindNeighbors_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"

/**
 * @brief The FindNeighbors class. See [Filter documentation](@ref findneighbors) for details.
 */
class FindNeighbors : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(FindNeighbors)
    DREAM3D_STATIC_NEW_MACRO(FindNeighbors)
    DREAM3D_TYPE_MACRO_SUPER(FindNeighbors, AbstractFilter)

    virtual ~FindNeighbors();

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellFeatureAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath CellFeatureAttributeMatrixPath READ getCellFeatureAttributeMatrixPath WRITE setCellFeatureAttributeMatrixPath)

    DREAM3D_FILTER_PARAMETER(QString, SharedSurfaceAreaListArrayName)
    Q_PROPERTY(QString SharedSurfaceAreaListArrayName READ getSharedSurfaceAreaListArrayName WRITE setSharedSurfaceAreaListArrayName)

    DREAM3D_FILTER_PARAMETER(QString, NeighborListArrayName)
    Q_PROPERTY(QString NeighborListArrayName READ getNeighborListArrayName WRITE setNeighborListArrayName)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, BoundaryCellsArrayName)
    Q_PROPERTY(QString BoundaryCellsArrayName READ getBoundaryCellsArrayName WRITE setBoundaryCellsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, NumNeighborsArrayName)
    Q_PROPERTY(QString NumNeighborsArrayName READ getNumNeighborsArrayName WRITE setNumNeighborsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, SurfaceFeaturesArrayName)
    Q_PROPERTY(QString SurfaceFeaturesArrayName READ getSurfaceFeaturesArrayName WRITE setSurfaceFeaturesArrayName)

    DREAM3D_FILTER_PARAMETER(bool, StoreBoundaryCells)
    Q_PROPERTY(bool StoreBoundaryCells READ getStoreBoundaryCells WRITE setStoreBoundaryCells)

    DREAM3D_FILTER_PARAMETER(bool, StoreSurfaceFeatures)
    Q_PROPERTY(bool StoreSurfaceFeatures READ getStoreSurfaceFeatures WRITE setStoreSurfaceFeatures)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();
    virtual const QString getHumanLabel();

    /**
     * @brief setupFilterParameters
     */
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

    virtual void preflight();

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    FindNeighbors();

  private:
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, FeatureIds)

    DEFINE_CREATED_DATAARRAY_VARIABLE(int8_t, BoundaryCells)
    DEFINE_CREATED_DATAARRAY_VARIABLE(bool, SurfaceFeatures)
    DEFINE_CREATED_DATAARRAY_VARIABLE(int32_t, NumNeighbors)

    NeighborList<int32_t>::WeakPointer m_NeighborList;
    NeighborList<float>::WeakPointer m_SharedSurfaceAreaList;

    void dataCheck();

    FindNeighbors(const FindNeighbors&); // Copy Constructor Not Implemented
    void operator=(const FindNeighbors&); // Operator '=' Not Implemented
};

#endif /* FINDNEIGHBORS_H_ */
