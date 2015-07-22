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


#ifndef _UncertainRegularGridSampleSurfaceMesh_H_
#define _UncertainRegularGridSampleSurfaceMesh_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/FilterParameters/FloatVec3FilterParameter.h"

#include "Sampling/SamplingFilters/SampleSurfaceMesh.h"

/**
 * @brief The UncertainRegularGridSampleSurfaceMesh class. See [Filter documentation](@ref uncertainregulargridsamplesurfacemesh) for details.
 */
class UncertainRegularGridSampleSurfaceMesh : public SampleSurfaceMesh
{
    Q_OBJECT
  public:
    DREAM3D_SHARED_POINTERS(UncertainRegularGridSampleSurfaceMesh)
    DREAM3D_STATIC_NEW_MACRO(UncertainRegularGridSampleSurfaceMesh)
    DREAM3D_TYPE_MACRO_SUPER(UncertainRegularGridSampleSurfaceMesh, AbstractFilter)

    virtual ~UncertainRegularGridSampleSurfaceMesh();

    DREAM3D_FILTER_PARAMETER(QString, DataContainerName)
    Q_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)

    DREAM3D_FILTER_PARAMETER(QString, CellAttributeMatrixName)
    Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(int, XPoints)
    Q_PROPERTY(int XPoints READ getXPoints WRITE setXPoints)

    DREAM3D_FILTER_PARAMETER(int, YPoints)
    Q_PROPERTY(int YPoints READ getYPoints WRITE setYPoints)

    DREAM3D_FILTER_PARAMETER(int, ZPoints)
    Q_PROPERTY(int ZPoints READ getZPoints WRITE setZPoints)

    DREAM3D_FILTER_PARAMETER(FloatVec3_t, Resolution)
    Q_PROPERTY(FloatVec3_t Resolution READ getResolution WRITE setResolution)

    DREAM3D_FILTER_PARAMETER(FloatVec3_t, Origin)
    Q_PROPERTY(FloatVec3_t Origin READ getOrigin WRITE setOrigin)

    DREAM3D_FILTER_PARAMETER(FloatVec3_t, Uncertainty)
    Q_PROPERTY(FloatVec3_t Uncertainty READ getUncertainty WRITE setUncertainty)

    DREAM3D_FILTER_PARAMETER(QString, FeatureIdsArrayName)
    Q_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

    /**
     * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getCompiledLibraryName();

    /**
     * @brief newFilterInstance Reimplemented from @see AbstractFilter class
     */
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);

    /**
     * @brief getGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getGroupName();

    /**
     * @brief getSubGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getSubGroupName();

    /**
     * @brief getHumanLabel Reimplemented from @see AbstractFilter class
     */
    virtual const QString getHumanLabel();

    /**
     * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void setupFilterParameters();

    /**
     * @brief writeFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
     * @brief readFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
     * @brief execute Reimplemented from @see AbstractFilter class
     */
    virtual void execute();

    /**
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    virtual void preflight();

  protected:
    UncertainRegularGridSampleSurfaceMesh();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief generate_points Reimplemented from @see SampleSurfaceMesh class
     * @return VertexGeom object
     */
    virtual VertexGeom::Pointer generate_points();

    /**
     * @brief assign_points Reimplemented from @see SampleSurfaceMesh class
     * @param iArray Sampled Feature Ids from superclass
     */
    virtual void assign_points(Int32ArrayType::Pointer iArray);

  private:
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)

    UncertainRegularGridSampleSurfaceMesh(const UncertainRegularGridSampleSurfaceMesh&); // Copy Constructor Not Implemented
    void operator=(const UncertainRegularGridSampleSurfaceMesh&); // Operator '=' Not Implemented
};

#endif /* UncertainRegularGridSampleSurfaceMesh_H_ */
