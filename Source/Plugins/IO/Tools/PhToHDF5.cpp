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

#include <iostream>
#include <fstream>

#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtCore/QVector>

#include <QtCore/QMap>

#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/HDF5/VTKH5Constants.h"


#define APPEND_DATA_TRUE 1
#define APPEND_DATA_FALSE 0

hid_t m_FileId = 0;

/** @brief Holds a single Euler Angle Set */
class EulerSet
{
  public:
    float e0;
    float e1;
    float e2;
    virtual ~EulerSet() {};
    EulerSet() : e0(0.0f), e1(0.0f), e2(0.0f) {}
};




/**
 *
 * @param FileName
 * @param data
 * @param nx X Dimension
 * @param ny Y Dimension
 * @param nz Z Dimension
 */
int  ReadPHFile(QString FileName, QVector<int>& data, int& nx, int& ny, int& nz)
{
  DataContainerArray::Pointer dca = DataContainerArray::New();
  DataContainer::Pointer m = DataContainer::New(); /* FIXME: What Geometry do we need? */
  dca->pushBack(m);

  FilterManager::Pointer fm = FilterManager::Instance();
  AbstractFilter::Pointer reader = fm->getFactoryForFilter("PhReader")->create();
  reader->setDataContainerArray(dca);
  bool propWasSet = reader->setProperty("InputFile", FileName);
  if(propWasSet == false)
  {

  }
  reader->execute();
  if (reader->getErrorCondition() < 0)
  {
    qDebug() << "Error Reading the Ph File '" << FileName << "' Error Code:" << reader->getErrorCondition();
    return -1;
  }

  Int32ArrayType* featureIds = Int32ArrayType::SafePointerDownCast(m->getAttributeMatrix(SIMPL::Defaults::CellAttributeMatrixName)->getAttributeArray(SIMPL::CellData::FeatureIds).get());
  size_t count = featureIds->getNumberOfTuples();

  data.resize(count);
  for(size_t i = 0; i < count; ++i)
  {
    data[i] = featureIds->getValue(i);
  }


  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int openHDF5File(const QString m_FileName, bool appendData)
{
  // Try to open a file to append data into
  if (APPEND_DATA_TRUE == appendData)
  {
    m_FileId = QH5Utilities::openFile(m_FileName, false);
  }
  // No file was found or we are writing new data only to a clean file
  if (APPEND_DATA_FALSE == appendData || m_FileId < 0)
  {
    m_FileId = QH5Utilities::createFile (m_FileName);
  }

  //Something went wrong either opening or creating the file. Error messages have
  // Alread been written at this point so just return.
  if (m_FileId < 0)
  {
    qDebug() << "The hdf5 file could not be opened or created.\n The Given filename was:\n\t[" << m_FileName << "]";
  }
  return m_FileId;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int closeHDF5File()
{
  // Close the file when we are finished with it
  return QH5Utilities::closeFile(m_FileId);
}

/**
 *
 */
template<typename T>
int writeScalarData(const QString& hdfPath,
                    const QVector<T>& scalar_data,
                    const QString& name,
                    int numComp, int32_t rank, hsize_t* dims)
{
  hid_t gid = H5Gopen(m_FileId, hdfPath.toLatin1().data(), H5P_DEFAULT );
  if (gid < 0)
  {
    qDebug() << "Error opening Group " << hdfPath;
    return gid;
  }
  herr_t err = QH5Utilities::createGroupsFromPath(H5_SCALAR_DATA_GROUP_NAME, gid);
  if (err < 0)
  {
    qDebug() << "Error creating HDF Group " << H5_SCALAR_DATA_GROUP_NAME;
    return err;
  }
  hid_t cellGroupId = H5Gopen(gid, H5_SCALAR_DATA_GROUP_NAME, H5P_DEFAULT );
  if(err < 0)
  {
    qDebug() << "Error writing string attribute to HDF Group " << H5_SCALAR_DATA_GROUP_NAME;
    return err;
  }

  T* data = const_cast<T*>(&(scalar_data.front()));


  err = QH5Lite::replacePointerDataset(cellGroupId, name, rank, dims, data);
  if (err < 0)
  {
    qDebug() << "Error writing array with name: " << name;
  }
  err = QH5Lite::writeScalarAttribute(cellGroupId, name, SIMPL::HDF5::NumComponents, numComp);
  if (err < 0)
  {
    qDebug() << "Error writing dataset " << name;
  }
  err = H5Gclose(cellGroupId);

  err = H5Gclose(gid);
  return err;
}

/**
 *
 * @param h5File
 * @param data
 * @param nx
 * @param ny
 * @param nz
 * @return
 */
int writePhDataToHDF5File(const QString& h5File, QVector<int>& data, int& nx, int& ny, int& nz)
{
  int err = 0;
  err = openHDF5File(h5File, true);

  int totalPoints = nx * ny * nz;
  int32_t rank = 1;
  hsize_t dims[1] =
  { totalPoints };

  int numComp = 1;
  err = writeScalarData(SIMPL::Defaults::DataContainerName, data, SIMPL::CellData::FeatureIds, numComp, rank, dims);
  if (err < 0)
  {
    qDebug() << "Error Writing Scalars '" << SIMPL::CellData::FeatureIds << "' to " << SIMPL::Defaults::DataContainerName;
    return err;
  }
  // Close the file when we are done with it.
  err = closeHDF5File();
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int writeEulerDataToHDF5File(const QString& h5File, QVector<float>& data, int numComp, int32_t rank, hsize_t* dims)
{
  int err = 0;
  err = openHDF5File(h5File, true);

  err = writeScalarData(SIMPL::Defaults::DataContainerName, data, SIMPL::CellData::EulerAngles, numComp, rank, dims);
  if (err < 0)
  {
    qDebug() << "Error Writing Scalars '" << SIMPL::CellData::EulerAngles << "' to " << SIMPL::Defaults::DataContainerName;
    return err;
  }
  // Close the file when we are done with it.
  err = closeHDF5File();

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ReadEulerFile(const QString& filename, QMap<int, EulerSet>& gidToEulerMap)
{
  int err = -1;
  FILE* f = fopen(filename.toLatin1().data(), "rb");
  if (NULL == f)
  {
    qDebug() << "Could not open Euler Angle File '" << filename << "'";
    return err;
  }
  err = 1;
  int read = 4;
  int gid;
  while (read == 4)
  {
    EulerSet e;
    read = fscanf(f, "%d %f %f %f", &gid, &(e.e0), &(e.e1), &(e.e2) );
    gidToEulerMap[gid] = e;
  }

  //Close the file when we are done
  fclose(f);

  return err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  qDebug() << "Starting Ph to HDF5 Merging...";

// Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("PhToHDF5");

  std::cout << "PhToHDF5 Starting. Version " << SIMPLib::Version::PackageComplete().toStdString() << std::endl;


  // Register all the filters including trying to load those from Plugins
  FilterManager::Pointer fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm.get());

  // Send progress messages from PipelineBuilder to this object for display
  QMetaObjectUtilities::RegisterMetaTypes();

  try
  {
    // Handle program options passed on command line.
    TCLAP::CmdLine cmd("PhToHDF5", ' ', SIMPLib::Version::Complete().toStdString());

    TCLAP::ValueArg<std::string> phFileArg( "p", "phfile", "Ph Input File", true, "", "Ph Input File");
    cmd.add(phFileArg);

    TCLAP::ValueArg<std::string> angleFileArg( "e", "eulerfile", "Euler Angle File", false, "", "Euler Angle File");
    cmd.add(angleFileArg);

    TCLAP::ValueArg<std::string> h5InputFileArg( "t", "h5file", "Target HDF5 File", true, "", "Target HDF5 File");
    cmd.add(h5InputFileArg);

    // Parse the argv array.
    cmd.parse(argc, argv);
    if (argc == 1)
    {
      qDebug() << "PhToHDF5 program was not provided any arguments. Use the --help argument to show the help listing.";
      return EXIT_FAILURE;
    }


    QString phFile = QString::fromStdString(phFileArg.getValue());
    QString h5File = QString::fromStdString(h5InputFileArg.getValue());

    QVector<int> voxels;
    int nx = 0;
    int ny = 0;
    int nz = 0;

    qDebug() << "Merging the FeatureID data from " << phFile;
    qDebug() << "  into";
    qDebug() << "file: " << h5File;


    qDebug() << "Reading the Ph data file....";
    int err = ReadPHFile(phFile, voxels, nx, ny, nz);
    if (err < 0)
    {
      return EXIT_FAILURE;
    }
    qDebug() << "Ph File has dimensions: " << nx << " x " << ny << " x " << nz;


    qDebug() << "Now Overwriting the FeatureID data set in the HDF5 file....";
    err = writePhDataToHDF5File(h5File, voxels, nz, ny, nz);
    if (err < 0)
    {
      qDebug() << "There was an error writing the feature id data. Check other errors for possible clues.";
      return EXIT_FAILURE;
    }
    qDebug() << "+ Done Writing the Feature ID Data.";


    QMap<int, EulerSet> gidToEulerMap;
    if (angleFileArg.getValue().empty() == false)
    {
      qDebug() << "Reading the Euler Angle Data....";
      err = ReadEulerFile(QString::fromStdString(angleFileArg.getValue()), gidToEulerMap);
      if (err < 0)
      {
        qDebug() << "Error Reading the Euler Angle File";
        return EXIT_FAILURE;
      }

      // Over Write the Euler Angles if the Euler File was supplied

      qDebug() << "Now Over Writing the Euler Angles data in the HDF5 file.....";
      int totalPoints = nx * ny * nz;
      int numComp = 3;
      // Loop over each Voxel getting its Feature ID and then setting the Euler Angle
      QVector<float> dataf(totalPoints * 3);
      for (int i = 0; i < totalPoints; ++i)
      {
        EulerSet& angle = gidToEulerMap[voxels[i]];
        dataf[i * 3] = angle.e0;
        dataf[i * 3 + 1] = angle.e1;
        dataf[i * 3 + 2] = angle.e2;
      }
      // This is going to be a 2 Dimension Table Data set.
      int32_t rank = 2;
      hsize_t dims[2] = {totalPoints, numComp};
      err = writeEulerDataToHDF5File(h5File, dataf, numComp, rank, dims);
      if (err < 0)
      {
        qDebug() << "There was an error writing the Euler Angle data. Check other errors for possible clues.";
        return EXIT_FAILURE;
      }
      qDebug() << "+ Done Writing the Euler Angle Data.";
    }

  }
  catch (TCLAP::ArgException& e) // catch any exceptions
  {
    std::cerr << " error: " << e.error() << " for arg " << e.argId();
    return EXIT_FAILURE;
  }
  qDebug() << "Successfully completed the merge.";

  return EXIT_SUCCESS;
}
