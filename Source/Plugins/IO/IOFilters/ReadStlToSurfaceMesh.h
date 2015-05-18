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


#ifndef _ReadStlToSurfaceMesh_H_
#define _ReadStlToSurfaceMesh_H_

#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"


/**
 * @class ReadStlToSurfaceMesh ReadStlToSurfaceMesh.h IOFilters/Code/IOFiltersFilters/ReadStlToSurfaceMesh.h
 * @brief This filter creates an STL file for each Feature ID, or Region ID that is encountered in
 * the volume.
 * @author
 * @date
 * @version 1.0
 */
class  ReadStlToSurfaceMesh : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(ReadStlToSurfaceMesh)
    DREAM3D_STATIC_NEW_MACRO(ReadStlToSurfaceMesh)
    DREAM3D_TYPE_MACRO_SUPER(ReadStlToSurfaceMesh, AbstractFilter)

    virtual ~ReadStlToSurfaceMesh();

    /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */
    DREAM3D_FILTER_PARAMETER(QString, SurfaceMeshDataContainerName)
    Q_PROPERTY(QString SurfaceMeshDataContainerName READ getSurfaceMeshDataContainerName WRITE setSurfaceMeshDataContainerName)
    DREAM3D_FILTER_PARAMETER(QString, FaceAttributeMatrixName)
    Q_PROPERTY(QString FaceAttributeMatrixName READ getFaceAttributeMatrixName WRITE setFaceAttributeMatrixName)
    DREAM3D_FILTER_PARAMETER(QString, StlFilePath)
    Q_PROPERTY(QString StlFilePath READ getStlFilePath WRITE setStlFilePath)
    DREAM3D_FILTER_PARAMETER(QString, SurfaceMeshTriangleNormalsArrayName)
    Q_PROPERTY(QString SurfaceMeshTriangleNormalsArrayName READ getSurfaceMeshTriangleNormalsArrayName WRITE setSurfaceMeshTriangleNormalsArrayName)

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
    ReadStlToSurfaceMesh();

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
    DEFINE_CREATED_DATAARRAY_VARIABLE(double, SurfaceMeshTriangleNormals)

    double m_minXcoord;
    double m_maxXcoord;
    double m_minYcoord;
    double m_maxYcoord;
    double m_minZcoord;
    double m_maxZcoord;

    void updateFaceInstancePointers();

    void readFile();
    void eliminate_duplicate_nodes();

    ReadStlToSurfaceMesh(const ReadStlToSurfaceMesh&); // Copy Constructor Not Implemented
    void operator=(const ReadStlToSurfaceMesh&); // Operator '=' Not Implemented
};

#endif /* _ReadStlToSurfaceMesh_H_ */



