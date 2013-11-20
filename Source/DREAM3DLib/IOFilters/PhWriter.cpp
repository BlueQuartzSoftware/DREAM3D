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

#include "PhWriter.h"

#include <QtCore/QtDebug>
#include <fstream>
#include <QtCore/QString>
#include <iomanip>
#include <QtCore/QMap>

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhWriter::PhWriter() :
  FileWriter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhWriter::~PhWriter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhWriter::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("*.ph");
    option->setFileType("CMU Feature Growth");
    option->setValueType("string");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void PhWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setOutputFile( reader->readString( "OutputFile", getOutputFile() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PhWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputFile", getOutputFile() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhWriter::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  if(getOutputFile().isEmpty() == true)
  {

    QString ss = QObject::tr("%1 needs the Output File Set and it was not.").arg(ClassName());
    addErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-387);
  }

  QVector<int> dims(1, 1);
  m_FeatureIdsPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, -300, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_FeatureIds */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhWriter::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    QString ss = QObject::tr("The Volume Data Container with name '%1'' was not found in the Data Container Array.").arg(getDataContainerName());
    setErrorCondition(-1001);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PhWriter::writeHeader()
{
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PhWriter::writeFile()
{

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

//  int32_t* feature_indicies = 0;
//  {
//    IDataArray::Pointer iDataArray = m->getCellData(DREAM3D::CellData::FeatureIds);
//    if (iDataArray.get() == 0) {
//      return -10;
//    }
//    if (static_cast<size_t>(m->getTotalPoints()) != iDataArray->getNumberOfTuples()) {
//      return -20;
//    }
//    feature_indicies =
//    IDataArray::SafeReinterpretCast<IDataArray*, Int32ArrayType*, int32_t* >(m->getCellData(DREAM3D::CellData::FeatureIds).get());
//    if (0 == feature_indicies) {
//      return -30;
//    }
//  }

  int64_t totalPoints = m->getTotalPoints();
  dataCheck(false, totalPoints, 1, 1);
  if (getErrorCondition() < 0)
  {
    return -40;
  }

  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };
  int64_t totalpoints = dims[0] * dims[1] * dims[2];

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(getOutputFile());
  QDir parentPath(fi.path());
  if(!parentPath.mkpath("."))
  {

    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    notifyErrorMessage(ss, -1);
    setErrorCondition(-1);
    return -1;
  }

  std::ofstream outfile;
  outfile.open(getOutputFile().toLatin1().data(), std::ios_base::binary);
  if(!outfile)
  {
    qDebug() << "Failed to open: " << getOutputFile() ;
    return -1;
  }


  // Find the unique number of features
  QMap<int, bool> used;
  for (int i = 0; i < totalpoints; ++i)
  {
    used[m_FeatureIds[i]] = true;
  }

  int features = 0;
  typedef QMap<int, bool>::iterator iterator;
  for (iterator i = used.begin(); i != used.end(); i++)
  {
    if(i.value() == true)
    {
      features++;
    }
  }
  //qDebug()<<features<< " " << used.size() ;
  // Buffer the output with 4096 Bytes which is typically the size of a "Block" on a
  // modern Hard Drive. This should speed up the writes considerably
  char buffer[4096];
  outfile.rdbuf()->pubsetbuf(buffer, 4096);

  outfile << "     " << dims[0] << "     " << dims[1] << "     " << dims[2] << "\n";
  outfile << "\'DREAM3\'              52.00  1.000  1.0       " << features << "\n";
  outfile << " 0.000 0.000 0.000          0        \n"; // << features << endl;

  for (int k = 0; k < totalpoints; k++)
  {
    outfile << m_FeatureIds[k] << '\n';
  }
  outfile.close();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("Writing Ph File Complete");
  return 0;
}

