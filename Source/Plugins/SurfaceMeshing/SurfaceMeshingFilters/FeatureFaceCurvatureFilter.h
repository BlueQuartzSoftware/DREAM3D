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

#ifndef _FeatureFaceCurvatureFilter_H_
#define _FeatureFaceCurvatureFilter_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "Plugins/SurfaceMeshing/SurfaceMeshingFilters/SurfaceMeshFilter.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"

/**
 * @brief The FeatureFaceCurvatureFilter class. See [Filter documentation](@ref featurefacecurvaturefilter) for details.
 */
class FeatureFaceCurvatureFilter : public SurfaceMeshFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    SIMPL_SHARED_POINTERS(FeatureFaceCurvatureFilter)
    SIMPL_STATIC_NEW_MACRO(FeatureFaceCurvatureFilter)
    SIMPL_TYPE_MACRO_SUPER(FeatureFaceCurvatureFilter, SurfaceMeshFilter)

    virtual ~FeatureFaceCurvatureFilter();

    typedef std::vector<int64_t> FaceIds_t;
    typedef std::map<int32_t, FaceIds_t> SharedFeatureFaces_t;

    SIMPL_FILTER_PARAMETER(DataArrayPath, FaceAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath FaceAttributeMatrixPath READ getFaceAttributeMatrixPath WRITE setFaceAttributeMatrixPath)

    SIMPL_FILTER_PARAMETER(QString, SurfaceMeshPrincipalCurvature1sArrayName)
    Q_PROPERTY(QString SurfaceMeshPrincipalCurvature1sArrayName READ getSurfaceMeshPrincipalCurvature1sArrayName WRITE setSurfaceMeshPrincipalCurvature1sArrayName)

    SIMPL_FILTER_PARAMETER(QString, SurfaceMeshPrincipalCurvature2sArrayName)
    Q_PROPERTY(QString SurfaceMeshPrincipalCurvature2sArrayName READ getSurfaceMeshPrincipalCurvature2sArrayName WRITE setSurfaceMeshPrincipalCurvature2sArrayName)

    SIMPL_FILTER_PARAMETER(QString, SurfaceMeshPrincipalDirection1sArrayName)
    Q_PROPERTY(QString SurfaceMeshPrincipalDirection1sArrayName READ getSurfaceMeshPrincipalDirection1sArrayName WRITE setSurfaceMeshPrincipalDirection1sArrayName)

    SIMPL_FILTER_PARAMETER(QString, SurfaceMeshPrincipalDirection2sArrayName)
    Q_PROPERTY(QString SurfaceMeshPrincipalDirection2sArrayName READ getSurfaceMeshPrincipalDirection2sArrayName WRITE setSurfaceMeshPrincipalDirection2sArrayName)

    SIMPL_FILTER_PARAMETER(QString, SurfaceMeshGaussianCurvaturesArrayName)
    Q_PROPERTY(QString SurfaceMeshGaussianCurvaturesArrayName READ getSurfaceMeshGaussianCurvaturesArrayName WRITE setSurfaceMeshGaussianCurvaturesArrayName)

    SIMPL_FILTER_PARAMETER(QString, SurfaceMeshMeanCurvaturesArrayName)
    Q_PROPERTY(QString SurfaceMeshMeanCurvaturesArrayName READ getSurfaceMeshMeanCurvaturesArrayName WRITE setSurfaceMeshMeanCurvaturesArrayName)

    SIMPL_FILTER_PARAMETER(int, NRing)
    Q_PROPERTY(int NRing READ getNRing WRITE setNRing)

    SIMPL_FILTER_PARAMETER(bool, ComputePrincipalDirectionVectors)
    Q_PROPERTY(bool ComputePrincipalDirectionVectors READ getComputePrincipalDirectionVectors WRITE setComputePrincipalDirectionVectors)

    SIMPL_FILTER_PARAMETER(bool, ComputeMeanCurvature)
    Q_PROPERTY(bool ComputeMeanCurvature READ getComputeMeanCurvature WRITE setComputeMeanCurvature)

    SIMPL_FILTER_PARAMETER(bool, ComputeGaussianCurvature)
    Q_PROPERTY(bool ComputeGaussianCurvature READ getComputeGaussianCurvature WRITE setComputeGaussianCurvature)

    SIMPL_FILTER_PARAMETER(bool, UseNormalsForCurveFitting)
    Q_PROPERTY(bool UseNormalsForCurveFitting READ getUseNormalsForCurveFitting WRITE setUseNormalsForCurveFitting)

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFeatureFaceIdsArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshFeatureFaceIdsArrayPath READ getSurfaceMeshFeatureFaceIdsArrayPath WRITE setSurfaceMeshFeatureFaceIdsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceNormalsArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshFaceNormalsArrayPath READ getSurfaceMeshFaceNormalsArrayPath WRITE setSurfaceMeshFaceNormalsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshTriangleCentroidsArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshTriangleCentroidsArrayPath READ getSurfaceMeshTriangleCentroidsArrayPath WRITE setSurfaceMeshTriangleCentroidsArrayPath)

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

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    virtual void tbbTaskProgress();
#endif

  protected:
    FeatureFaceCurvatureFilter();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

  private:
    DEFINE_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFaceLabels)
    DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshTriangleCentroids)
    DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshFaceNormals)
    DEFINE_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFeatureFaceIds)

    DEFINE_DATAARRAY_VARIABLE(int32_t, SurfaceMeshUniqueEdges)
    DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshPrincipalCurvature1s)
    DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshPrincipalCurvature2s)
    DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshPrincipalDirection1s)
    DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshPrincipalDirection2s)
    DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshGaussianCurvatures)
    DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshMeanCurvatures)

    int32_t* m_SurfaceMeshFaceEdges;
    int32_t  m_TotalFeatureFaces;
    int32_t  m_CompletedFeatureFaces;


    FeatureFaceCurvatureFilter(const FeatureFaceCurvatureFilter&); // Copy Constructor Not Implemented
    void operator=(const FeatureFaceCurvatureFilter&); // Operator '=' Not Implemented
};

#endif /* FeatureFaceCurvatureFilter_H_ */
