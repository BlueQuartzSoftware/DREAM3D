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

#ifndef _DATACONTAINER_READER_H_
#define _DATACONTAINER_READER_H_

#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/VoxelDataContainer.h"
#include "DREAM3DLib/Common/FilterPipeline.h"

/**
 * @class DataContainerReader DataContainerReader.h DREAM3DLib/IOFilters/DataContainerReader.h
 * @brief
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jul 17, 2012
 * @version 1.0
 */
class DREAM3DLib_EXPORT DataContainerReader : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(DataContainerReader)
    DREAM3D_STATIC_NEW_MACRO(DataContainerReader)
    DREAM3D_TYPE_MACRO_SUPER(DataContainerReader, AbstractFilter)

    virtual ~DataContainerReader();

    DREAM3D_INSTANCE_STRING_PROPERTY(InputFile)
    DREAM3D_INSTANCE_PROPERTY(bool, ReadVoxelData)
    DREAM3D_INSTANCE_PROPERTY(bool, ReadSurfaceMeshData)
    DREAM3D_INSTANCE_PROPERTY(bool, ReadSolidMeshData)
    DREAM3D_INSTANCE_PROPERTY(bool, ReadAllArrays)

    DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedVoxelCellArrays)
    DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedVoxelFieldArrays)
    DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedVoxelEnsembleArrays)

    DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedSurfaceMeshVertexArrays)
    DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedSurfaceMeshFaceArrays)
    DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedSurfaceMeshEdgeArrays)
    DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedSurfaceMeshFieldArrays)
    DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedSurfaceMeshEnsembleArrays)

    DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedSolidMeshVertexArrays)
    DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedSolidMeshFaceArrays)
    DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedSolidMeshEdgeArrays)



    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::IOFilters; }
    virtual const std::string getSubGroupName() { return DREAM3D::FilterSubGroups::InputFilters; }
    virtual const std::string getHumanLabel() { return "Read DREAM3D Data File"; }

    FilterPipeline::Pointer getPipelinePointer() { return m_PipelineFromFile; }

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

    virtual void setVoxelSelectedArrayNames(std::set<std::string> selectedCellArrays,
                                            std::set<std::string> selectedFieldArrays,
                                            std::set<std::string> selectedEnsembleArrays);
    virtual void setSurfaceMeshSelectedArrayNames(std::set<std::string> selectedVertexArrays,
                                                  std::set<std::string> selectedFaceArrays,
                                                  std::set<std::string> selectedEdgeArrays,
                                                  std::set<std::string> selectedFieldArrays,
                                                  std::set<std::string> selectedEnsembleArrays);
    virtual void setSolidMeshSelectedArrayNames(std::set<std::string> selectedVertexArrays,
                                                std::set<std::string> selectedFaceArrays,
                                                std::set<std::string> selectedEdgeArrays);
    /**
     * @brief readExistingPipelineFromFile This will read the existing pipeline that is stored in the file and store it
     * in the class instance for later writing to another dream3d data file
     * @param fileId
     * @return
     */
    int readExistingPipelineFromFile(hid_t fileId);

    /**
     * @brief writeExistingPipelineToFile
     * @param writer
     * @param index
     * @return
     */
    int writeExistingPipelineToFile(AbstractFilterParametersWriter* writer, int index);

  protected:
    DataContainerReader();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param fields The number of fields
    * @param ensembles The number of ensembles
    */
    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

  private:
    FilterPipeline::Pointer m_PipelineFromFile;

    DataContainerReader(const DataContainerReader&); // Copy Constructor Not Implemented
    void operator=(const DataContainerReader&); // Operator '=' Not Implemented


};

#endif /* _DATACONTAINER_READER_H_ */
