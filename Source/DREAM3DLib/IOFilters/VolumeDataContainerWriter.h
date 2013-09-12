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

#ifndef _VolumeDataContainerWriter_H_
#define _VolumeDataContainerWriter_H_

#include <QtCore/QString>

#include <hdf5.h>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/HDF5/VTKH5Constants.h"
#include "DREAM3DLib/IOFilters/SurfaceDataContainerWriter.h"


/**
 * @class VolumeDataContainerWriter VolumeDataContainerWriter.h /IOFiltersFilters/VolumeDataContainerWriter.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class DREAM3DLib_EXPORT VolumeDataContainerWriter : public SurfaceDataContainerWriter
{
  public:
    DREAM3D_SHARED_POINTERS(VolumeDataContainerWriter)
    DREAM3D_STATIC_NEW_MACRO(VolumeDataContainerWriter)
    DREAM3D_TYPE_MACRO_SUPER(VolumeDataContainerWriter, AbstractFilter)

    virtual ~VolumeDataContainerWriter();

    /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */
    DREAM3D_INSTANCE_PROPERTY(hid_t, HdfFileId)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteXdmfFile)

    typedef QList<QString> NameListType;

    void setXdmfOStream(std::ostream* xdmf);

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const QString getGroupName() { return DREAM3D::FilterGroups::IOFilters; }
  virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::OutputFilters; }

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel() { return "Voxel DataContainer Writer"; }

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

  protected:
    VolumeDataContainerWriter();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param fields The number of fields
    * @param ensembles The number of ensembles
    */
    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);


    int writeMetaInfo(const QString &hdfPath, int64_t volDims[3],
                              float spacing[3], float origin[3]);

    int createVtkObjectGroup(const QString &hdfGroupPath, const char* vtkDataObjectType);
    int writeVertexData(hid_t dcGid);
    int writeEdgeData(hid_t dcGid);
    int writeFaceData(hid_t dcGid);
    int writeCellData(hid_t dcGid);
    int writeFieldData(hid_t dcGid);
    int writeEnsembleData(hid_t dcGid);

    void writeCellXdmfGridHeader(float* origin, float* spacing, int64_t* volDims);
    void writeFieldXdmfGridHeader(size_t numElements, const QString &label);
    //void writeFieldNeighborXdmfGridHeader(size_t numElements);
    void writeXdmfGridFooter(const QString &label);


    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    template<typename T, typename K>
    int writeEnsembleDataArray(hid_t ensembleGid, const QVector<T> &v, const QString &label)
     {
      herr_t err = 0;
      int numComp = 1;
      QVector<int> eData(v.size());
      for (size_t i = 0; i < v.size(); ++i)
      {
        eData[i] = v[i];
      }

      if(eData.size() > 0)
      {
        K* eDataPtr = const_cast<K*>(&(eData.front()));
        int num = static_cast<int>(eData.size() / numComp);
        int32_t rank = 1;
        hsize_t dims[1] =
        { (hsize_t)num * (hsize_t)numComp };

        err |= H5Lite::writePointerDataset(ensembleGid, label, rank, dims, eDataPtr);
        err |= H5Lite::writeScalarAttribute(ensembleGid, label, QString(H5_NUMCOMPONENTS), numComp);
        err |= H5Lite::writeStringAttribute(ensembleGid, label, DREAM3D::HDF5::ObjectType, "vector");

        if(err < 0)
        {
          setErrorCondition(err);
          
          ss << "Error writing Ensemble data set '" << label << "'";
        }
      }

      return err;
    }

  private:
    std::ostream* m_XdmfPtr;

    VolumeDataContainerWriter(const VolumeDataContainerWriter&); // Copy Constructor Not Implemented
    void operator=(const VolumeDataContainerWriter&); // Operator '=' Not Implemented
};

#endif /* _VolumeDataContainerWriter_H_ */
