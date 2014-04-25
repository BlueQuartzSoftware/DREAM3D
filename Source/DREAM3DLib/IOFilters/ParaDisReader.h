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

#ifndef _ParaDisREADER_H_
#define _ParaDisREADER_H_

#include <QtCore/QString>
#include <vector>
#include <QtCore/QFile>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/IOFilters/FileReader.h"
#include "DREAM3DLib/Common/Constants.h"

/**
 * @class ParaDisReader ParaDisReader.h DREAM3DLib/IO/ParaDisReader.h
 * @brief
 * @author mjackson
 * @date Sep 28, 2011
 * @version $Revision$
 */
class DREAM3DLib_EXPORT ParaDisReader : public FileReader
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(ParaDisReader)
    DREAM3D_STATIC_NEW_MACRO(ParaDisReader)
    DREAM3D_TYPE_MACRO_SUPER(ParaDisReader, FileReader)

    virtual ~ParaDisReader();
    DREAM3D_FILTER_PARAMETER(QString, EdgeDataContainerName)
    Q_PROPERTY(QString EdgeDataContainerName READ getEdgeDataContainerName WRITE setEdgeDataContainerName)
    DREAM3D_FILTER_PARAMETER(QString, VertexAttributeMatrixName)
    Q_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)
    DREAM3D_FILTER_PARAMETER(QString, EdgeAttributeMatrixName)
    Q_PROPERTY(QString EdgeAttributeMatrixName READ getEdgeAttributeMatrixName WRITE setEdgeAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(QString, InputFile)
    Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
    DREAM3D_FILTER_PARAMETER(float, BurgersVector)
    Q_PROPERTY(float BurgersVector READ getBurgersVector WRITE setBurgersVector)

    DREAM3D_FILTER_PARAMETER(QString, NumberOfArmsArrayName)
    Q_PROPERTY(QString NumberOfArmsArrayName READ getNumberOfArmsArrayName WRITE setNumberOfArmsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, NodeConstraintsArrayName)
    Q_PROPERTY(QString NodeConstraintsArrayName READ getNodeConstraintsArrayName WRITE setNodeConstraintsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, BurgersVectorsArrayName)
    Q_PROPERTY(QString BurgersVectorsArrayName READ getBurgersVectorsArrayName WRITE setBurgersVectorsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, SlipPlaneNormalsArrayName)
    Q_PROPERTY(QString SlipPlaneNormalsArrayName READ getSlipPlaneNormalsArrayName WRITE setSlipPlaneNormalsArrayName)

    virtual const QString getCompiledLibraryName() { return IO::IOBaseName; }
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName() { return DREAM3D::FilterGroups::IOFilters; }
    virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::InputFilters; }
    virtual const QString getHumanLabel() { return "Read ParaDis File"; }

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

    virtual void preflight();
    virtual void execute();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    ParaDisReader();

    virtual int readHeader();
    virtual int readFile();

    void dataCheck();
    void updateVertexInstancePointers();
    void updateEdgeInstancePointers();

  private:
    DEFINE_CREATED_DATAARRAY_VARIABLE(int32_t, NumberOfArms)
    DEFINE_CREATED_DATAARRAY_VARIABLE(int32_t, NodeConstraints)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, BurgersVectors)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, SlipPlaneNormals)
    QFile  m_InStream;

    int numVerts;
    int numEdges;

    int fileVersion;

    ParaDisReader(const ParaDisReader&); // Copy Constructor Not Implemented
    void operator=(const ParaDisReader&); // Operator '=' Not Implemented
};

#endif /* ParaDisREADER_H_ */





