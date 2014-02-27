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

#ifndef UncertainRegularGridSampleSurfaceMesh_H_
#define UncertainRegularGridSampleSurfaceMesh_H_

#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/DataContainers/SurfaceDataContainer.h"
#include "Sampling/SamplingFilters/SampleSurfaceMesh.h"

/**
 * @class UncertainRegularGridSampleSurfaceMesh UncertainRegularGridSampleSurfaceMesh.h DREAM3DLib/SyntheticBuilderFilters/UncertainRegularGridSampleSurfaceMesh.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class UncertainRegularGridSampleSurfaceMesh : public SampleSurfaceMesh
{
  Q_OBJECT
  public:
    DREAM3D_SHARED_POINTERS(UncertainRegularGridSampleSurfaceMesh)
    DREAM3D_STATIC_NEW_MACRO(UncertainRegularGridSampleSurfaceMesh)
    DREAM3D_TYPE_MACRO_SUPER(UncertainRegularGridSampleSurfaceMesh, AbstractFilter)

    virtual ~UncertainRegularGridSampleSurfaceMesh();
    DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellAttributeMatrixName)


    DREAM3D_FILTER_PARAMETER(int, XPoints)
    Q_PROPERTY(int XPoints READ getXPoints WRITE setXPoints)
    DREAM3D_FILTER_PARAMETER(int, YPoints)
    Q_PROPERTY(int YPoints READ getYPoints WRITE setYPoints)
    DREAM3D_FILTER_PARAMETER(int, ZPoints)
    Q_PROPERTY(int ZPoints READ getZPoints WRITE setZPoints)
    DREAM3D_FILTER_PARAMETER(FloatVec3_t, Resolution)
    Q_PROPERTY(FloatVec3_t Resolution READ getResolution WRITE setResolution)
    DREAM3D_FILTER_PARAMETER(FloatVec3_t, Uncertainty)
    Q_PROPERTY(FloatVec3_t Uncertainty READ getUncertainty WRITE setUncertainty)

    virtual const QString getGroupName() { return DREAM3D::FilterGroups::SamplingFilters; }
    virtual const QString getSubGroupName()  { return DREAM3D::FilterSubGroups::ResolutionFilters; }
    virtual const QString getHumanLabel() { return "Sample Surface Mesh on Regular Grid"; }

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
    virtual void preflight();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    UncertainRegularGridSampleSurfaceMesh();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param features The number of features
    * @param ensembles The number of ensembles
    */
    void dataCheck();

    virtual VertexArray::Pointer generate_points();
    virtual void assign_points(Int32ArrayType::Pointer iArray);

  private:
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, FeatureIds)

    UncertainRegularGridSampleSurfaceMesh(const UncertainRegularGridSampleSurfaceMesh&); // Copy Constructor Not Implemented
    void operator=(const UncertainRegularGridSampleSurfaceMesh&); // Operator '=' Not Implemented
};

#endif /* UncertainRegularGridSampleSurfaceMesh_H_ */



