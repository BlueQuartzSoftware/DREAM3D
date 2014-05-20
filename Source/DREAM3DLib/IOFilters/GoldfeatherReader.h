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

#ifndef _GoldfeatherReader_H_
#define _GoldfeatherReader_H_

#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/Constants.h"


/**
 * @class GoldfeatherReader GoldfeatherReader.h /IOFilters/GoldfeatherReader.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class DREAM3DLib_EXPORT GoldfeatherReader : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(GoldfeatherReader);
    DREAM3D_STATIC_NEW_MACRO(GoldfeatherReader);
    DREAM3D_TYPE_MACRO_SUPER(GoldfeatherReader, AbstractFilter);

    virtual ~GoldfeatherReader();
    DREAM3D_FILTER_PARAMETER(QString, SurfaceDataContainerName)
    Q_PROPERTY(QString SurfaceDataContainerName READ getSurfaceDataContainerName WRITE setSurfaceDataContainerName)
    DREAM3D_FILTER_PARAMETER(QString, VertexAttributeMatrixName)
    Q_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)
    DREAM3D_FILTER_PARAMETER(QString, FaceAttributeMatrixName)
    Q_PROPERTY(QString FaceAttributeMatrixName READ getFaceAttributeMatrixName WRITE setFaceAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(QString, SurfaceMeshPrincipalCurvature1sArrayName)
    Q_PROPERTY(QString SurfaceMeshPrincipalCurvature1sArrayName READ getSurfaceMeshPrincipalCurvature1sArrayName WRITE setSurfaceMeshPrincipalCurvature1sArrayName)
    DREAM3D_FILTER_PARAMETER(QString, SurfaceMeshPrincipalCurvature2sArrayName)
    Q_PROPERTY(QString SurfaceMeshPrincipalCurvature2sArrayName READ getSurfaceMeshPrincipalCurvature2sArrayName WRITE setSurfaceMeshPrincipalCurvature2sArrayName)
    DREAM3D_FILTER_PARAMETER(QString, SurfaceMeshPrincipalDirection1sArrayName)
    Q_PROPERTY(QString SurfaceMeshPrincipalDirection1sArrayName READ getSurfaceMeshPrincipalDirection1sArrayName WRITE setSurfaceMeshPrincipalDirection1sArrayName)
    DREAM3D_FILTER_PARAMETER(QString, SurfaceMeshPrincipalDirection2sArrayName)
    Q_PROPERTY(QString SurfaceMeshPrincipalDirection2sArrayName READ getSurfaceMeshPrincipalDirection2sArrayName WRITE setSurfaceMeshPrincipalDirection2sArrayName)
    DREAM3D_FILTER_PARAMETER(QString, SurfaceMeshNodeNormalsArrayName)
    Q_PROPERTY(QString SurfaceMeshNodeNormalsArrayName READ getSurfaceMeshNodeNormalsArrayName WRITE setSurfaceMeshNodeNormalsArrayName)
    DREAM3D_FILTER_PARAMETER(QString, SurfaceMeshFaceLabelsArrayName)
    Q_PROPERTY(QString SurfaceMeshFaceLabelsArrayName READ getSurfaceMeshFaceLabelsArrayName WRITE setSurfaceMeshFaceLabelsArrayName)
    DREAM3D_FILTER_PARAMETER(QString, SurfaceMeshFaceNormalsArrayName)
    Q_PROPERTY(QString SurfaceMeshFaceNormalsArrayName READ getSurfaceMeshFaceNormalsArrayName WRITE setSurfaceMeshFaceNormalsArrayName)

    /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */
    DREAM3D_INSTANCE_STRING_PROPERTY(InputFile)

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const QString getGroupName() { return DREAM3D::FilterGroups::IOFilters; }
    virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::InputFilters; }

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel() { return "Goldfeather Reader"; }

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
    GoldfeatherReader();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param features The number of features
    * @param ensembles The number of ensembles
    */
    void dataCheck();
    void updateVertexInstancePointers();
    void updateFaceInstancePointers();

  private:
    DEFINE_CREATED_DATAARRAY_VARIABLE(double, SurfaceMeshPrincipalCurvature1s)
    DEFINE_CREATED_DATAARRAY_VARIABLE(double, SurfaceMeshPrincipalCurvature2s)
    DEFINE_CREATED_DATAARRAY_VARIABLE(double, SurfaceMeshPrincipalDirection1s)
    DEFINE_CREATED_DATAARRAY_VARIABLE(double, SurfaceMeshPrincipalDirection2s)
    DEFINE_CREATED_DATAARRAY_VARIABLE(double, SurfaceMeshNodeNormals)
    DEFINE_CREATED_DATAARRAY_VARIABLE(double, SurfaceMeshFaceNormals)
    DEFINE_CREATED_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFaceLabels)

    GoldfeatherReader(const GoldfeatherReader&); // Copy Constructor Not Implemented
    void operator=(const GoldfeatherReader&); // Operator '=' Not Implemented
};

#endif /* _GoldfeatherReader_H_ */

