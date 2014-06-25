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

#ifndef _NodesTrianglesToStl_H_
#define _NodesTrianglesToStl_H_

#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/Common/AbstractFilter.h"

/**
 * @class CMUNodesTrianglesToStl CMUNodesTrianglesToStl.h MFESurfaceSmoothing/CMUNodesTrianglesToStl.h
 * @brief Converts the Nodes and Triangles files produced by the CMU SurfaceMeshing
 * and Surface Smoothing algorithms into STL Files
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jul 17, 2012
 * @version 1.0
 */
class  NodesTrianglesToStl : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(NodesTrianglesToStl);
    DREAM3D_STATIC_NEW_MACRO(NodesTrianglesToStl);
    DREAM3D_TYPE_MACRO_SUPER(NodesTrianglesToStl, AbstractFilter);

    virtual ~NodesTrianglesToStl();

    DREAM3D_FILTER_PARAMETER(QString, NodesFile);
    Q_PROPERTY(QString NodesFile READ getNodesFile WRITE setNodesFile)
    DREAM3D_FILTER_PARAMETER(QString, TrianglesFile);
    Q_PROPERTY(QString TrianglesFile READ getTrianglesFile WRITE setTrianglesFile)
    DREAM3D_FILTER_PARAMETER(QString, OutputStlDirectory);
    Q_PROPERTY(QString OutputStlDirectory READ getOutputStlDirectory WRITE setOutputStlDirectory)
    DREAM3D_FILTER_PARAMETER(QString, OutputStlPrefix);
    Q_PROPERTY(QString OutputStlPrefix READ getOutputStlPrefix WRITE setOutputStlPrefix)

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
    NodesTrianglesToStl();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param features The number of features
    * @param ensembles The number of ensembles
    */
    void dataCheck(bool preflight);


    int writeBinaryCellData(const QString& TrianglesFile, FILE* vtkFile, int nTriangles, bool conformalMesh);
    int writeASCIICellData(const QString& TrianglesFile, FILE* vtkFile, int nTriangles, bool conformalMesh);

    int writeBinaryPointData(const QString& NodesFile, FILE* vtkFile, int nNodes, bool conformalMesh);
    int writeASCIIPointData(const QString& NodesFile, FILE* vtkFile, int nNodes, bool conformalMesh);


  private:

    int writeHeader(FILE* f, const QString& header, int triCount);
    int writeNumTrianglesToFile(const QString& filename, int triCount);

    NodesTrianglesToStl(const NodesTrianglesToStl&); // Copy Constructor Not Implemented
    void operator=(const NodesTrianglesToStl&); // Operator '=' Not Implemented
};

#endif /* CMUNodesTrianglesToStl_H_ */




