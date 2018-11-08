/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#pragma once

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "Plugins/SurfaceMeshing/SurfaceMeshingFilters/SurfaceMeshFilter.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"

#include "SurfaceMeshing/SurfaceMeshingDLLExport.h"

/**
 * @brief The FeatureFaceCurvatureFilter class. See [Filter documentation](@ref featurefacecurvaturefilter) for details.
 */
class SurfaceMeshing_EXPORT FeatureFaceCurvatureFilter : public SurfaceMeshFilter
{
    Q_OBJECT
    // PYB11_CREATE_BINDINGS(FeatureFaceCurvatureFilter SUPERCLASS SurfaceMeshFilter)
    // PYB11_PROPERTY(DataArrayPath FaceAttributeMatrixPath READ getFaceAttributeMatrixPath WRITE setFaceAttributeMatrixPath)
    // PYB11_PROPERTY(QString SurfaceMeshPrincipalCurvature1sArrayName READ getSurfaceMeshPrincipalCurvature1sArrayName WRITE setSurfaceMeshPrincipalCurvature1sArrayName)
    // PYB11_PROPERTY(QString SurfaceMeshPrincipalCurvature2sArrayName READ getSurfaceMeshPrincipalCurvature2sArrayName WRITE setSurfaceMeshPrincipalCurvature2sArrayName)
    // PYB11_PROPERTY(QString SurfaceMeshPrincipalDirection1sArrayName READ getSurfaceMeshPrincipalDirection1sArrayName WRITE setSurfaceMeshPrincipalDirection1sArrayName)
    // PYB11_PROPERTY(QString SurfaceMeshPrincipalDirection2sArrayName READ getSurfaceMeshPrincipalDirection2sArrayName WRITE setSurfaceMeshPrincipalDirection2sArrayName)
    // PYB11_PROPERTY(QString SurfaceMeshGaussianCurvaturesArrayName READ getSurfaceMeshGaussianCurvaturesArrayName WRITE setSurfaceMeshGaussianCurvaturesArrayName)
    // PYB11_PROPERTY(QString SurfaceMeshMeanCurvaturesArrayName READ getSurfaceMeshMeanCurvaturesArrayName WRITE setSurfaceMeshMeanCurvaturesArrayName)
    // PYB11_PROPERTY(int NRing READ getNRing WRITE setNRing)
    // PYB11_PROPERTY(bool ComputePrincipalDirectionVectors READ getComputePrincipalDirectionVectors WRITE setComputePrincipalDirectionVectors)
    // PYB11_PROPERTY(bool ComputeMeanCurvature READ getComputeMeanCurvature WRITE setComputeMeanCurvature)
    // PYB11_PROPERTY(bool ComputeGaussianCurvature READ getComputeGaussianCurvature WRITE setComputeGaussianCurvature)
    // PYB11_PROPERTY(bool UseNormalsForCurveFitting READ getUseNormalsForCurveFitting WRITE setUseNormalsForCurveFitting)
    // PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)
    // PYB11_PROPERTY(DataArrayPath SurfaceMeshFeatureFaceIdsArrayPath READ getSurfaceMeshFeatureFaceIdsArrayPath WRITE setSurfaceMeshFeatureFaceIdsArrayPath)
    // PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceNormalsArrayPath READ getSurfaceMeshFaceNormalsArrayPath WRITE setSurfaceMeshFaceNormalsArrayPath)
    // PYB11_PROPERTY(DataArrayPath SurfaceMeshTriangleCentroidsArrayPath READ getSurfaceMeshTriangleCentroidsArrayPath WRITE setSurfaceMeshTriangleCentroidsArrayPath)
  public:
    SIMPL_SHARED_POINTERS(FeatureFaceCurvatureFilter)
    SIMPL_FILTER_NEW_MACRO(FeatureFaceCurvatureFilter)
     SIMPL_TYPE_MACRO_SUPER_OVERRIDE(FeatureFaceCurvatureFilter, SurfaceMeshFilter)

     ~FeatureFaceCurvatureFilter() override;

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
     const QString getCompiledLibraryName() const override;

     /**
      * @brief getBrandingString Returns the branding string for the filter, which is a tag
      * used to denote the filter's association with specific plugins
      * @return Branding string
      */
     const QString getBrandingString() const override;

     /**
      * @brief getFilterVersion Returns a version string for this filter. Default
      * value is an empty string.
      * @return
      */
     const QString getFilterVersion() const override;

     /**
      * @brief newFilterInstance Reimplemented from @see AbstractFilter class
      */
     AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

     /**
      * @brief getGroupName Reimplemented from @see AbstractFilter class
      */
     const QString getGroupName() const override;

     /**
      * @brief getSubGroupName Reimplemented from @see AbstractFilter class
      */
     const QString getSubGroupName() const override;

     /**
      * @brief getUuid Return the unique identifier for this filter.
      * @return A QUuid object.
      */
     const QUuid getUuid() override;

     /**
      * @brief getHumanLabel Reimplemented from @see AbstractFilter class
      */
     const QString getHumanLabel() const override;

     /**
      * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
      */
     void setupFilterParameters() override;

     /**
      * @brief readFilterParameters Reimplemented from @see AbstractFilter class
      */
     void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

     /**
      * @brief execute Reimplemented from @see AbstractFilter class
      */
     void execute() override;

     /**
      * @brief preflight Reimplemented from @see AbstractFilter class
      */
     void preflight() override;

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
    virtual void tbbTaskProgress();
#endif

  protected:
    FeatureFaceCurvatureFilter();
    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();


  private:
    DEFINE_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFaceLabels)
    DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshTriangleCentroids)
    DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshFaceNormals)
    DEFINE_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFeatureFaceIds)
    DataArray<int32_t>::WeakPointer m_SurfaceMeshUniqueEdgesPtr;
    DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshPrincipalCurvature1s)
    DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshPrincipalCurvature2s)
    DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshPrincipalDirection1s)
    DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshPrincipalDirection2s)
    DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshGaussianCurvatures)
    DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshMeanCurvatures)

    int32_t* m_SurfaceMeshFaceEdges;
    int32_t  m_TotalFeatureFaces;
    int32_t  m_CompletedFeatureFaces;

  public:
    FeatureFaceCurvatureFilter(const FeatureFaceCurvatureFilter&) = delete; // Copy Constructor Not Implemented
    FeatureFaceCurvatureFilter(FeatureFaceCurvatureFilter&&) = delete;      // Move Constructor Not Implemented
    FeatureFaceCurvatureFilter& operator=(const FeatureFaceCurvatureFilter&) = delete; // Copy Assignment Not Implemented
    FeatureFaceCurvatureFilter& operator=(FeatureFaceCurvatureFilter&&) = delete;      // Move Assignment Not Implemented
};

