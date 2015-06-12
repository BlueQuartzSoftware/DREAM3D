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

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "Plugins/SurfaceMeshing/SurfaceMeshingFilters/SurfaceMeshFilter.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"

/**
 * @brief The FeatureFaceCurvatureFilter class. See [Filter documentation](@ref featurefacecurvaturefilter) for details.
 */
class FeatureFaceCurvatureFilter : public SurfaceMeshFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(FeatureFaceCurvatureFilter)
    DREAM3D_STATIC_NEW_MACRO(FeatureFaceCurvatureFilter)
    DREAM3D_TYPE_MACRO_SUPER(FeatureFaceCurvatureFilter, SurfaceMeshFilter)

    virtual ~FeatureFaceCurvatureFilter();

    typedef std::vector<int64_t> FaceIds_t;
    typedef std::map<int32_t, FaceIds_t> SharedFeatureFaces_t;

    DREAM3D_FILTER_PARAMETER(QString, EdgeAttributeMatrixName)
    Q_PROPERTY(QString EdgeAttributeMatrixName READ getEdgeAttributeMatrixName WRITE setEdgeAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(QString, SurfaceMeshUniqueEdgesArrayName)
    Q_PROPERTY(QString SurfaceMeshUniqueEdgesArrayName READ getSurfaceMeshUniqueEdgesArrayName WRITE setSurfaceMeshUniqueEdgesArrayName)

    DREAM3D_FILTER_PARAMETER(QString, SurfaceMeshPrincipalCurvature1sArrayName)
    Q_PROPERTY(QString SurfaceMeshPrincipalCurvature1sArrayName READ getSurfaceMeshPrincipalCurvature1sArrayName WRITE setSurfaceMeshPrincipalCurvature1sArrayName)

    DREAM3D_FILTER_PARAMETER(QString, SurfaceMeshPrincipalCurvature2sArrayName)
    Q_PROPERTY(QString SurfaceMeshPrincipalCurvature2sArrayName READ getSurfaceMeshPrincipalCurvature2sArrayName WRITE setSurfaceMeshPrincipalCurvature2sArrayName)

    DREAM3D_FILTER_PARAMETER(QString, SurfaceMeshPrincipalDirection1sArrayName)
    Q_PROPERTY(QString SurfaceMeshPrincipalDirection1sArrayName READ getSurfaceMeshPrincipalDirection1sArrayName WRITE setSurfaceMeshPrincipalDirection1sArrayName)

    DREAM3D_FILTER_PARAMETER(QString, SurfaceMeshPrincipalDirection2sArrayName)
    Q_PROPERTY(QString SurfaceMeshPrincipalDirection2sArrayName READ getSurfaceMeshPrincipalDirection2sArrayName WRITE setSurfaceMeshPrincipalDirection2sArrayName)

    DREAM3D_FILTER_PARAMETER(QString, SurfaceMeshGaussianCurvaturesArrayName)
    Q_PROPERTY(QString SurfaceMeshGaussianCurvaturesArrayName READ getSurfaceMeshGaussianCurvaturesArrayName WRITE setSurfaceMeshGaussianCurvaturesArrayName)

    DREAM3D_FILTER_PARAMETER(QString, SurfaceMeshMeanCurvaturesArrayName)
    Q_PROPERTY(QString SurfaceMeshMeanCurvaturesArrayName READ getSurfaceMeshMeanCurvaturesArrayName WRITE setSurfaceMeshMeanCurvaturesArrayName)

    DREAM3D_FILTER_PARAMETER(int, NRing)
    Q_PROPERTY(int NRing READ getNRing WRITE setNRing)

    DREAM3D_FILTER_PARAMETER(bool, ComputePrincipalDirectionVectors)
    Q_PROPERTY(bool ComputePrincipalDirectionVectors READ getComputePrincipalDirectionVectors WRITE setComputePrincipalDirectionVectors)

    DREAM3D_FILTER_PARAMETER(bool, ComputeMeanCurvature)
    Q_PROPERTY(bool ComputeMeanCurvature READ getComputeMeanCurvature WRITE setComputeMeanCurvature)

    DREAM3D_FILTER_PARAMETER(bool, ComputeGaussianCurvature)
    Q_PROPERTY(bool ComputeGaussianCurvature READ getComputeGaussianCurvature WRITE setComputeGaussianCurvature)

    DREAM3D_FILTER_PARAMETER(bool, UseNormalsForCurveFitting)
    Q_PROPERTY(bool UseNormalsForCurveFitting READ getUseNormalsForCurveFitting WRITE setUseNormalsForCurveFitting)

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFeatureFaceIdsArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshFeatureFaceIdsArrayPath READ getSurfaceMeshFeatureFaceIdsArrayPath WRITE setSurfaceMeshFeatureFaceIdsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceNormalsArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshFaceNormalsArrayPath READ getSurfaceMeshFaceNormalsArrayPath WRITE setSurfaceMeshFaceNormalsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, SurfaceMeshTriangleCentroidsArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshTriangleCentroidsArrayPath READ getSurfaceMeshTriangleCentroidsArrayPath WRITE setSurfaceMeshTriangleCentroidsArrayPath)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel();

    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
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

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();

    /**
    * @brief This function runs some sanity checks on the DataContainer and inputs
    * in an attempt to ensure the filter can process the inputs.
    */
    virtual void preflight();

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    virtual void tbbTaskProgress();
#endif

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    FeatureFaceCurvatureFilter();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param features The number of features
    * @param ensembles The number of ensembles
    */
    void dataCheck();

  private:
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFaceLabels)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(double, SurfaceMeshTriangleCentroids)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(double, SurfaceMeshFaceNormals)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFeatureFaceIds)

    DEFINE_CREATED_DATAARRAY_VARIABLE(int32_t, SurfaceMeshUniqueEdges)
    DEFINE_CREATED_DATAARRAY_VARIABLE(double, SurfaceMeshPrincipalCurvature1s)
    DEFINE_CREATED_DATAARRAY_VARIABLE(double, SurfaceMeshPrincipalCurvature2s)
    DEFINE_CREATED_DATAARRAY_VARIABLE(double, SurfaceMeshPrincipalDirection1s)
    DEFINE_CREATED_DATAARRAY_VARIABLE(double, SurfaceMeshPrincipalDirection2s)
    DEFINE_CREATED_DATAARRAY_VARIABLE(double, SurfaceMeshGaussianCurvatures)
    DEFINE_CREATED_DATAARRAY_VARIABLE(double, SurfaceMeshMeanCurvatures)

    int32_t* m_SurfaceMeshFaceEdges;
    int32_t  m_TotalFeatureFaces;
    int32_t  m_CompletedFeatureFaces;

    FeatureFaceCurvatureFilter(const FeatureFaceCurvatureFilter&); // Copy Constructor Not Implemented
    void operator=(const FeatureFaceCurvatureFilter&); // Operator '=' Not Implemented
};

#endif /* FeatureFaceCurvatureFilter_H_ */