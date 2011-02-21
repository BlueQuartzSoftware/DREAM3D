/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _AIMH5DATAWRITER_H_
#define _AIMH5DATAWRITER_H_




#include "hdf5.h"

#include <vector>

#include "VTKH5Constants.h"

#include "MXA/MXATypes.h"
#include "MXA/Common/MXASetGetMacros.h"
#include "MXA/HDF5/H5Utilities.h"
#include "MXA/HDF5/H5Lite.h"

#include "AIM/Common/AIMCommonConfiguration.h"


class ReconstructionFunc;

/**
 * @class AIMH5DataWriter AIMH5DataWriter.h AIM/Reconstruction/AIMH5DataWriter.h
 * @brief This class is designed to write VTK style objects into an HDF5 type file.
 * Currently supported types are
 * <li> UnstructuredGrid
 *
 * One needs to create at a minimum the points, cells and cell_types array. See the
 * method signatures to create the proper types for each array. Currently STL container
 * vector is used to hold the data to be written. Some sample code
 *
 * @code
 * AIMH5DataWriter::Pointer h5writer = AIMH5DataWriter::New();
 * h5writer->setFileName(hdfFile);
 * int err = h5writer->openFile(false);
 *  std::vector<float> points;
    std::vector<int32_t> cells;
    std::vector<int32_t> cell_types(number_of_cells, VTK_CELLTYPE_VOXEL);
    std::string hdfPath ("/SomePathToTheData");
    ... Code to populate the arrays ....
    int err = h5writer->writeUnstructuredGrid(hdfPath, points, cells, cell_types);
 * @endcode
 *
 * Cell Data, Point Data and Field Data can also be added to the vtk object.
 * @code
 * err = h5writer->writeFieldData<int>( hdfPath, grainName, "Grain_ID", 1);
 * std::vector<float> kernelAvgDisorientation(vlist->size());
 * ...Code to copy data into the vector ....
 * err = h5writer->writeCellData<float>(hdfPath, kernelAvgDisorientation, "KernelAvgDisorientation", 1);
 *
 * @endcode
 *
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Nov 19, 2010
 * @version 1.0
 */
class AIMCOMMON_EXPORT AIM_H5VtkDataWriter
{
  public:
    MXA_SHARED_POINTERS(AIM_H5VtkDataWriter);
    MXA_STATIC_NEW_MACRO(AIM_H5VtkDataWriter);

    virtual ~AIM_H5VtkDataWriter();

    MXA_INSTANCE_STRING_PROPERTY(FileName);

    /**
     * @brief Opens or Creates an HDF5 file to write data into
     * @param append Should a new file be created or append data to a currenlty existing file
     * @return
     */
    int openFile(bool append = false);

    /**
     * @brief Closes the currently open file
     * @return
     */
    int closeFile();



  /**
   * @brief
   * @param hdfPath
   * @param points
   * @param cells
   * @param cell_types
   * @return
   */
    int writeUnstructuredGrid(const std::string &hdfPath,
                              const std::vector<float> &points,
                              const std::vector<int32_t> &cells,
                              const std::vector<int32_t> &cell_types);

    /**
     *
     */
    int writeObjectIndex(std::vector<std::string> &hdfPaths);


    /**
     * @brief
     * @param fp
     * @param data
     * @param dsetName
     * @param num
     * @param numComp
     * @return
     */
    template <typename T>
    int vtkWriteDataArray(hid_t fp, const T *data, const char *dsetName,
                           int num, int numComp)
    {
      // std::cout << "      vtkH5DataWriter::vtkWriteDataArray<T>()" << std::endl;
      int32_t rank = 1;
      hsize_t dims[1] = { (hsize_t)num * (hsize_t)numComp};
      std::string name (dsetName);
      herr_t err = H5Lite::writePointerDataset(fp, name, rank, dims, data);
      if (err < 0)
      {
        std::cout << "Error writing array with name: " << std::string (dsetName) << std::endl;
      }
      err = H5Lite::writeScalarAttribute(fp, name, std::string(H5_NUMCOMPONENTS), numComp);
      return err;
    }

  /**
   * @brief
   * @param points
   * @return
   */
    template<typename T>
    int writePoints(hid_t groupId, const std::vector<T> &points)
    {
      typename std::vector<T>::size_type numPts;

      if (points.size() < 3)
        {
        return 1;
        }

      numPts=points.size()/3;

      int err = this->vtkWriteDataArray<T>(groupId, &(points.front()), H5_POINTS, (int)numPts, 3);
      if (err != 1)
      {
        // std::cout << "Error Writing Points Array" << std::endl;
      }
      //Write the attributes to the dataset
      int32_t numComp = 3;
      err = H5Lite::writeScalarAttribute(groupId, H5_POINTS, H5_NUMCOMPONENTS, numComp);
      if (err < 0)
      {
        // std::cout << "Error Writing NumComponents attribute to dataset." << std::endl;
      }
      return err;
    }

    /**
     * @brief
     * @param hdfPath
     * @param field_data
     * @param label
     * @param numComp
     * @return
     */
    template<typename T>
    int writeFieldData(const std::string &hdfPath,
                       const std::vector<T> &field_data,
                       const char* label,
                       int numComp)
    {
      if (field_data.size() == 0 )
      {
        std::cout << "Field Data '" << label << "' has a Zero length array. No data is written" << std::endl;
        return -1;
      }
      if (field_data.empty() == true)
      {
        std::cout << "Field Data '" << label << "' has a Zero length array. No data is written" << std::endl;
        return -1;
      }
      hid_t gid = H5Gopen(m_FileId, hdfPath.c_str() );
      int err = H5Utilities::createGroupsFromPath(H5_FIELD_DATA_GROUP_NAME, gid);
      if (err < 0)
      {
        std::cout << "Error creating HDF Group " << H5_FIELD_DATA_GROUP_NAME << std::endl;
        return err;
      }
      err = H5Lite::writeStringAttribute(gid, H5_FIELD_DATA_GROUP_NAME, H5_NAME, H5_FIELD_DATA_DEFAULT);


      hid_t fieldGroupId = H5Gopen(gid, H5_FIELD_DATA_GROUP_NAME );
      if(err < 0)
      {
        std::cout << "Error writing string attribute to HDF Group " << H5_FIELD_DATA_GROUP_NAME << std::endl;
        return err;
      }

      T* data = const_cast<T*>(&(field_data.front()));
      int num = static_cast<int>(field_data.size() / numComp);
      if(field_data.size() > 0)
      {
        err = vtkWriteDataArray(fieldGroupId, data, label, num, numComp);
        if (err < 0)
        {
          std::cout << "Error writing dataset " << label << std::endl;
        }
      }
      err = H5Gclose(fieldGroupId);

      err = H5Gclose(gid);

      return err;
    }

    /**
     * @brief
     * @param cell_data
     * @param label
     * @param groupname
     * @return
     */
    template<typename T>
    int writeCellData(const std::string &hdfPath,
                      const std::vector<T> &cell_data,
                      const char *label,
                      int numComp)
    {
      hid_t gid = H5Gopen(m_FileId, hdfPath.c_str() );
      herr_t err = H5Utilities::createGroupsFromPath(H5_CELL_DATA_GROUP_NAME, gid);
      if (err < 0)
      {
        std::cout << "Error creating HDF Group " << H5_CELL_DATA_GROUP_NAME << std::endl;
        return err;
      }
      hid_t cellGroupId = H5Gopen(gid, H5_CELL_DATA_GROUP_NAME );
      if(err < 0)
      {
        std::cout << "Error writing string attribute to HDF Group " << H5_CELL_DATA_GROUP_NAME << std::endl;
        return err;
      }

      T* data = const_cast<T*>(&(cell_data.front()));
      int num = static_cast<int>(cell_data.size() / numComp);
      err = vtkWriteDataArray(cellGroupId, data, label, num, numComp);
      if (err < 0)
      {
        std::cout << "Error writing dataset " << label << std::endl;
      }
      err = H5Gclose(cellGroupId);

      err = H5Gclose(gid);
      return err;
    }

  protected:
    AIM_H5VtkDataWriter();

    /**
     * @brief
     * @param hdfGroupPath
     * @param vtkDataObjectType The type of vtk data ojbect that is going to be written such as H5_VTK_UNSTRUCTURED_GRID
     * or H5_VTK_POLYDATA
     * @return
     */
    int createVtkObjectGroup(const std::string &hdfGroupPath, const char* vtkDataObjectType);


  private:
    hid_t m_FileId;

    AIM_H5VtkDataWriter(const AIM_H5VtkDataWriter&); // Copy Constructor Not Implemented
    void operator=(const AIM_H5VtkDataWriter&); // Operator '=' Not Implemented
};

#endif /* _AIMH5DATAWRITER_H_ */
