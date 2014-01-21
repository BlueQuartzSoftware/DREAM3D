/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#ifndef FeatureFaceCurvatureFilter_H_
#define FeatureFaceCurvatureFilter_H_

#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "SurfaceMeshing/SurfaceMeshingFilters/SurfaceMeshFilter.h"

/**
 * @class FeatureFaceCurvatureFilter FeatureFaceCurvatureFilter.h FeatureCurvature/Code/FeatureFaceCurvatureFilters/FeatureFaceCurvatureFilter.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class FeatureFaceCurvatureFilter : public SurfaceMeshFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(FeatureFaceCurvatureFilter)
    DREAM3D_STATIC_NEW_MACRO(FeatureFaceCurvatureFilter)
    DREAM3D_TYPE_MACRO_SUPER(FeatureFaceCurvatureFilter, SurfaceMeshFilter)

    virtual ~FeatureFaceCurvatureFilter();
    DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceDataContainerName)
    DREAM3D_INSTANCE_STRING_PROPERTY(FaceAttributeMatrixName)
    DREAM3D_INSTANCE_STRING_PROPERTY(EdgeAttributeMatrixName)
    DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceMeshUniqueEdgesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(PrincipalCurvature1ArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(PrincipalCurvature2ArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(PrincipalDirection1ArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(PrincipalDirection2ArrayName)


    /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */
    DREAM3D_INSTANCE_PROPERTY(int, NRing)
    Q_PROPERTY(int NRing READ getNRing WRITE setNRing NOTIFY parametersChanged)
    DREAM3D_INSTANCE_PROPERTY(bool, ComputePrincipalDirectionVectors)
    Q_PROPERTY(bool ComputePrincipalDirectionVectors READ getComputePrincipalDirectionVectors WRITE setComputePrincipalDirectionVectors NOTIFY parametersChanged)
    DREAM3D_INSTANCE_PROPERTY(bool, ComputeMeanCurvature)
    Q_PROPERTY(bool ComputeMeanCurvature READ getComputeMeanCurvature WRITE setComputeMeanCurvature NOTIFY parametersChanged)
    DREAM3D_INSTANCE_PROPERTY(bool, ComputeGaussianCurvature)
    Q_PROPERTY(bool ComputeGaussianCurvature READ getComputeGaussianCurvature WRITE setComputeGaussianCurvature NOTIFY parametersChanged)
    DREAM3D_INSTANCE_PROPERTY(bool, UseNormalsForCurveFitting)
    Q_PROPERTY(bool UseNormalsForCurveFitting READ getUseNormalsForCurveFitting WRITE setUseNormalsForCurveFitting NOTIFY parametersChanged)

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const QString getGroupName() { return DREAM3D::FilterGroups::SurfaceMeshingFilters; }
    virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::CurvatureFilters; }

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel() { return "Feature Face Curvature Filter"; }

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
    void parametersChanged();

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
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, SurfaceMeshFaceLabels)
    DEFINE_PTR_WEAKPTR_DATAARRAY(double, SurfaceMeshTriangleCentroids)
    DEFINE_PTR_WEAKPTR_DATAARRAY(double, SurfaceMeshFaceNormals)

    int32_t* m_SurfaceMeshUniqueEdges;
    int32_t* m_SurfaceMeshFaceEdges;
    int32_t  m_TotalFeatureFaces;
    int32_t  m_CompletedFeatureFaces;


    FeatureFaceCurvatureFilter(const FeatureFaceCurvatureFilter&); // Copy Constructor Not Implemented
    void operator=(const FeatureFaceCurvatureFilter&); // Operator '=' Not Implemented
};

#endif /* FeatureFaceCurvatureFilter_H_ */



