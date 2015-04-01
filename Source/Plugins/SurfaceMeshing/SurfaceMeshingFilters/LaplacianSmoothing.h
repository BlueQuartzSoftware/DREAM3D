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

#ifndef _LaplacianSmoothing_H_
#define _LaplacianSmoothing_H_

#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "Plugins/SurfaceMeshing/SurfaceMeshingFilters/SurfaceMeshFilter.h"
#include "SurfaceMeshing/SurfaceMeshingConstants.h"

#define OUTPUT_DEBUG_VTK_FILES 1


/**
 * @class LaplacianSmoothing LaplacianSmoothing.h SurfaceMesh/Code/SurfaceMeshFilters/LaplacianSmoothing.h
 * @brief This filter performs a Laplacian smoothing operation on a surface mesh. Some of the options allow
 * the user to set specific lambda values for the various types of nodes that the surface meshers generate.
 * @author Michael Jackson (mike.jackson@bluequartz.net)
 * @date   Oct 2012
 * @version 1.0
 */
class LaplacianSmoothing : public SurfaceMeshFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(LaplacianSmoothing)
    DREAM3D_STATIC_NEW_MACRO(LaplacianSmoothing)
    DREAM3D_TYPE_MACRO_SUPER(LaplacianSmoothing, SurfaceMeshFilter)

    virtual ~LaplacianSmoothing();
    DREAM3D_FILTER_PARAMETER(QString, SurfaceDataContainerName)
    Q_PROPERTY(QString SurfaceDataContainerName READ getSurfaceDataContainerName WRITE setSurfaceDataContainerName)

    // We need these arrays for this filter to work correctly
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SurfaceMeshNodeTypeArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshNodeTypeArrayPath READ getSurfaceMeshNodeTypeArrayPath WRITE setSurfaceMeshNodeTypeArrayPath)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)

    /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */
    DREAM3D_FILTER_PARAMETER(int, IterationSteps)
    Q_PROPERTY(int IterationSteps READ getIterationSteps WRITE setIterationSteps)
    DREAM3D_FILTER_PARAMETER(float, Lambda)
    Q_PROPERTY(float Lambda READ getLambda WRITE setLambda)
    DREAM3D_FILTER_PARAMETER(float, SurfacePointLambda)
    Q_PROPERTY(float SurfacePointLambda READ getSurfacePointLambda WRITE setSurfacePointLambda)
    DREAM3D_FILTER_PARAMETER(float, TripleLineLambda)
    Q_PROPERTY(float TripleLineLambda READ getTripleLineLambda WRITE setTripleLineLambda)
    DREAM3D_FILTER_PARAMETER(float, QuadPointLambda)
    Q_PROPERTY(float QuadPointLambda READ getQuadPointLambda WRITE setQuadPointLambda)
    DREAM3D_FILTER_PARAMETER(float, SurfaceTripleLineLambda)
    Q_PROPERTY(float SurfaceTripleLineLambda READ getSurfaceTripleLineLambda WRITE setSurfaceTripleLineLambda)
    DREAM3D_FILTER_PARAMETER(float, SurfaceQuadPointLambda)
    Q_PROPERTY(float SurfaceQuadPointLambda READ getSurfaceQuadPointLambda WRITE setSurfaceQuadPointLambda)


    /* This class is designed to be subclassed so that thoes subclasses can add
     * more functionality such as constrained surface nodes or Triple Lines. We use
     * this array to assign each vertex a specific Lambda value. Subclasses can set
     * this array then simply call the 'smooth' protected method to actually run the
     * smoothing iterations
     */
    DREAM3D_VIRTUAL_INSTANCE_PROPERTY(DataArray<float>::Pointer, LambdaArray)


    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
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

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    LaplacianSmoothing();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param features The number of features
    * @param ensembles The number of ensembles
    */
    void dataCheck();

    /**
     * @brief This method generates the Lambda array that will be use during the smoothing
     * @return
     */
    virtual int generateLambdaArray();

    /**
     * @brief This version of the smoothing algorithm uses Edge->Vertex connectivity information for its algorithm
     * @return
     */
    virtual int edgeBasedSmoothing();

  private:
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int8_t, SurfaceMeshNodeType)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFaceLabels)

    LaplacianSmoothing(const LaplacianSmoothing&); // Copy Constructor Not Implemented
    void operator=(const LaplacianSmoothing&); // Operator '=' Not Implemented
};

#endif /* _LaplacianSmoothing_H_ */







