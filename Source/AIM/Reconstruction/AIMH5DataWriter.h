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

#ifndef H5POLYDATAWRITER_H_
#define H5POLYDATAWRITER_H_




#include "hdf5.h"

#include <vector>

#include "VTKH5Constants.h"

#include "MXA/MXATypes.h"
#include "MXA/Common/MXASetGetMacros.h"
#include "MXA/HDF5/H5Utilities.h"
#include "MXA/HDF5/H5Lite.h"

class ReconstructionFunc;
/**
 *
 */
class AIMH5DataWriter
{
  public:
    MXA_SHARED_POINTERS(AIMH5DataWriter);
    MXA_STATIC_NEW_MACRO(AIMH5DataWriter);

    virtual ~AIMH5DataWriter();

    MXA_INSTANCE_STRING_PROPERTY(FileName);

    int openFile(bool append = false);
    int closeFile();

    /**
     * @brief
     * @param hdfGroupPath
     * @param vtkDataObjectType The type of vtk data ojbect that is going to be written such as H5_VTK_UNSTRUCTURED_GRID
     * or H5_VTK_POLYDATA
     * @return
     */
    int createGroup(const std::string &hdfGroupPath, const char* vtkDataObjectType);

    int openGroup(const std::string &hdfGroupPath);

    int closeCurrentGroup();

/**
 *
 * @param points
 * @param cells
 * @param cell_types
 * @return
 */
    int writeUnstructuredGrid(const std::vector<float> &points, const std::vector<int32_t> &cells,
                              const std::vector<int32_t> &cell_types);



    /**
     * @brief
     * @param cells
     * @param label
     * @param polygonEntrySize The TOTAL number of elements that make up an entry for a polygon.
     * this should be the first value + 1. So for triangles we have a value of 4 because the
     * entry is setup as follows: "3 2 4 8" which says there are "3" indices followed by the
     * values of those indices (2 4 8). We assume that you are writing a HOMOGENEOUS set of
     * polygons, ie, ALL triangles or ALL cubes.
     * @return
     */
    int writePolyCells( const std::vector<int> &cells, const char *label, int polygonEntrySize);



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
      uint64_t dims[1] = { (uint64_t)num * (uint64_t)numComp};
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
    int writePoints(const std::vector<T> &points)
    {
      int numPts;

      if (points.size() < 3)
        {
        return 1;
        }

      numPts=points.size()/3;

      int err = this->vtkWriteDataArray<T>(m_CurrentGroupId, &(points.front()), H5_POINTS, numPts, 3);
      if (err != 1)
      {
        // std::cout << "Error Writing Points Array" << std::endl;
      }
      //Write the attributes to the dataset
      int32_t numComp = 3;
      err = H5Lite::writeScalarAttribute(m_CurrentGroupId, H5_POINTS, H5_NUMCOMPONENTS, numComp);
      if (err < 0)
      {
        // std::cout << "Error Writing NumComponents attribute to dataset." << std::endl;
      }
      return err;
    }

    /**
     * @brief
     * @param field_data
     * @param label
     * @param numComp
     * @return
     */
    template<typename T>
    int writeFieldData(const std::vector<T> &field_data, const char* label, int numComp)
    {

      int err = H5Utilities::createGroupsFromPath(H5_FIELD_DATA_GROUP_NAME, m_CurrentGroupId);
      if (err < 0)
      {
        std::cout << "Error creating HDF Group " << H5_FIELD_DATA_GROUP_NAME << std::endl;
        return err;
      }
      err = H5Lite::writeStringAttribute(m_CurrentGroupId, H5_FIELD_DATA_GROUP_NAME, H5_NAME, H5_FIELD_DATA_DEFAULT);

      hid_t prevGid = m_CurrentGroupId;
      m_CurrentGroupId = H5Gopen(m_CurrentGroupId, H5_FIELD_DATA_GROUP_NAME );
      if(err < 0)
      {
        std::cout << "Error writing string attribute to HDF Group " << H5_FIELD_DATA_GROUP_NAME << std::endl;
        return err;
      }

      T* data = const_cast<T*>(&(field_data.front()));
      int num = field_data.size() / numComp;
      err = vtkWriteDataArray(m_CurrentGroupId, data, label, num, numComp);
      if (err < 0)
      {
        std::cout << "Error writing dataset " << label << std::endl;
      }
      H5Gclose(m_CurrentGroupId);

      m_CurrentGroupId = prevGid;

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
    int writeCellData(const std::vector<T> &cell_data, const char *label, int numComp)
    {
      hid_t prevGid = m_CurrentGroupId;
      herr_t err = H5Utilities::createGroupsFromPath(H5_CELL_DATA_GROUP_NAME, m_CurrentGroupId);
      if (err < 0)
      {
        std::cout << "Error creating HDF Group " << H5_CELL_DATA_GROUP_NAME << std::endl;
        return err;
      }
      m_CurrentGroupId = H5Gopen(m_CurrentGroupId, H5_CELL_DATA_GROUP_NAME );
      if(err < 0)
      {
        std::cout << "Error writing string attribute to HDF Group " << H5_CELL_DATA_GROUP_NAME << std::endl;
        return err;
      }

      T* data = const_cast<T*>(&(cell_data.front()));
      int num = cell_data.size() / numComp;
      err = vtkWriteDataArray(m_CurrentGroupId, data, label, num, numComp);
      if (err < 0)
      {
        std::cout << "Error writing dataset " << label << std::endl;
      }
      H5Gclose(m_CurrentGroupId);

      m_CurrentGroupId = prevGid;
      return err;
    }

  protected:
    AIMH5DataWriter();

  private:
    hid_t m_FileId;
    hid_t m_CurrentGroupId;
    std::string m_CurrentGroupPath;



    AIMH5DataWriter(const AIMH5DataWriter&); // Copy Constructor Not Implemented
    void operator=(const AIMH5DataWriter&); // Operator '=' Not Implemented
};

#endif /* H5POLYDATAWRITER_H_ */
