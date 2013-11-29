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

#include "INLWriter.h"

#include <QtCore/QtDebug>
#include <fstream>


#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "EbsdLib/TSL/AngConstants.h"

#include "DREAM3DLib/DREAM3DVersion.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/CubicLowOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/TrigonalOps.h"
#include "DREAM3DLib/OrientationOps/TetragonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/OrientationOps/MonoclinicOps.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
INLWriter::INLWriter() :
  FileWriter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_MaterialNamesArrayName(DREAM3D::EnsembleData::MaterialName),
  m_FeatureIds(NULL),
  m_CellPhases(NULL),
  m_CellEulerAngles(NULL)

{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
INLWriter::~INLWriter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void INLWriter::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("*.txt");
    option->setFileType("INL Format");
    option->setValueType("string");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void INLWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int INLWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputFile", getOutputFile());
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void INLWriter::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
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
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CellPhasesPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_CellPhasesArrayName, -302, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = m->getPrereqArray<unsigned int, AbstractFilter>(this, m_CellEnsembleAttributeMatrixName,  m_CrystalStructuresArrayName, -304, ensembles, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  m_CellEulerAnglesPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellAttributeMatrixName,  m_CellEulerAnglesArrayName, -305, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  IDataArray::Pointer materialNamePtr = m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->getAttributeArray(DREAM3D::EnsembleData::MaterialName);
  StringDataArray* materialNames = StringDataArray::SafePointerDownCast(materialNamePtr.get());
  if (NULL == materialNames)
  {

    QString ss = QObject::tr("%1 requires the %2 Ensemble array to be present in the Data Container.").arg(ClassName()).arg(DREAM3D::EnsembleData::MaterialName);
    addErrorMessage(getHumanLabel(), ss, -1111);
    setErrorCondition(-1111);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void INLWriter::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    addErrorMessage(getHumanLabel(), "The VolumeDataContainer Object with the specific name " + getDataContainerName() + " was not available.", getErrorCondition());
    return;
  }

  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int INLWriter::writeHeader()
{
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int INLWriter::writeFile()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  int64_t totalPoints = m->getTotalPoints();
  size_t numfeatures = 0; //m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  size_t numensembles = 0; //m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  dataCheck(false, totalPoints, numfeatures, numensembles);
  if(getErrorCondition() < 0)
  {
    return -40;
  }

  int err = 0;
  size_t dims[3] =
  { 0, 0, 0 };
  m->getDimensions(dims);

  float res[3];
  m->getResolution(res);

  float origin[3];
  m->getOrigin(origin);

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(getOutputFile());
  QDir dir(fi.path());
  if(!dir.mkpath("."))
  {

    QString ss = QObject::tr("Error creating parent path '%1'").arg(fi.path());
    notifyErrorMessage(ss, -1);
    setErrorCondition(-1);
    return -1;
  }

  FILE* f = fopen(getOutputFile().toLatin1().data(), "wb");
  if(NULL == f)
  {

    QString ss = QObject::tr("Error Opening File for writing '%1'").arg(getOutputFile());
    notifyErrorMessage(ss, -1);
    setErrorCondition(-1);
    return -1;
  }

  // Write the header, Each line starts with a "#" symbol
  fprintf(f, "# File written from %s\r\n", DREAM3DLib::Version::PackageComplete().toLatin1().data());
  fprintf(f, "# DateTime: %s\r\n", QDateTime::currentDateTime().toString().toLatin1().data());
  fprintf(f, "# X_STEP: %f\r\n", res[0]);
  fprintf(f, "# Y_STEP: %f\r\n", res[1]);
  fprintf(f, "# Z_STEP: %f\r\n", res[2]);
  fprintf(f, "#\r\n");
  fprintf(f, "# X_MIN: %f\r\n", origin[0]);
  fprintf(f, "# Y_MIN: %f\r\n", origin[1]);
  fprintf(f, "# Z_MIN: %f\r\n", origin[2]);
  fprintf(f, "#\r\n");
  fprintf(f, "# X_MAX: %f\r\n", origin[0] + (dims[0]*res[0]));
  fprintf(f, "# Y_MAX: %f\r\n", origin[1] + (dims[1]*res[1]));
  fprintf(f, "# Z_MAX: %f\r\n", origin[2] + (dims[2]*res[2]));
  fprintf(f, "#\r\n");
  fprintf(f, "# X_DIM: %llu\r\n", static_cast<unsigned long long int>(dims[0]));
  fprintf(f, "# Y_DIM: %llu\r\n", static_cast<unsigned long long int>(dims[1]));
  fprintf(f, "# Z_DIM: %llu\r\n", static_cast<unsigned long long int>(dims[2]));
  fprintf(f, "#\r\n");

// IDataArray::Pointer pDataPtr = m->getCellEnsembleData(DREAM3D::EnsembleData::PhaseTypes);
  IDataArray::Pointer materialNamePtr = m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->getAttributeArray(DREAM3D::EnsembleData::MaterialName);
  StringDataArray* materialNames = StringDataArray::SafePointerDownCast(materialNamePtr.get());
  if (NULL == materialNames)
  {
    fclose(f);

    QString ss = QObject::tr("The MaterialNames Ensemble Array was not in the Data Container");
    notifyErrorMessage(ss, -1111);
    setErrorCondition(-1111);
    return -1;
  }
  uint32_t symmetry = 0;
  int32_t count = static_cast<int32_t>(materialNamePtr->getNumberOfTuples());
  for(int32_t i = 1; i < count; ++i)
  {
    QString matName = materialNames->GetValue(i);
    fprintf(f, "# Phase_%d: %s\r\n", i, matName.toLatin1().data());
    symmetry = m_CrystalStructures[i];
    fprintf(f, "# Phase_%u: %s\r\n", i, materialNames->GetValue(i).toLatin1().data());
    if(symmetry == Ebsd::CrystalStructure::Cubic_High)
    {
      symmetry = Ebsd::Ang::PhaseSymmetry::Cubic;
    }
    else if(symmetry == Ebsd::CrystalStructure::Hexagonal_High)
    {
      symmetry = Ebsd::Ang::PhaseSymmetry::DiHexagonal;
    }
    else
    {
      symmetry = Ebsd::Ang::PhaseSymmetry::UnknownSymmetry;
    }
    fprintf(f, "# Symmetry_%d: %u\r\n", i, symmetry);
    fprintf(f, "#\r\n");
  }

  QSet<int32_t> uniqueFeatureIds;
  for(int64_t i = 0; i < totalPoints; ++i)
  {
    uniqueFeatureIds.insert(m_FeatureIds[i]);
  }
  count = static_cast<int32_t>(uniqueFeatureIds.size());
  fprintf(f, "# Num_Features: %d \r\n", count);
  fprintf(f, "#\r\n");

  //  fprintf(f, "# Column 1-3: phi1, PHI, phi2 (orientation of point in radians)\r\n");
  //  fprintf(f, "# Column 4-6: x, y, z (coordinates of point in microns)\r\n");
  //  fprintf(f, "# Column 7: Feature ID\r\n");
  //  fprintf(f, "# Column 8: Phase ID\r\n");


  fprintf(f, "# phi1 PHI phi2 x y z FeatureId PhaseId Symmetry\r\n");

  float phi1, phi, phi2;
  float xPos, yPos, zPos;
  int32_t featureId;
  int32_t phaseId;

  // unsigned char rgba[4] = {0,0,0,255};
  //float refDir[3] = {0.0f, 0.0f, 1.0f};


  size_t index = 0;
  for(size_t z = 0; z < dims[2]; ++z)
  {
    for (size_t y = 0; y < dims[1]; ++y)
    {
      for (size_t x = 0; x < dims[0]; ++x)
      {
        index = (z * dims[0] * dims[1]) + (dims[0] * y) + x;
        phi1 = m_CellEulerAngles[index * 3];
        phi = m_CellEulerAngles[index * 3 + 1];
        phi2 = m_CellEulerAngles[index * 3 + 2];
        xPos = origin[0] + (x * res[0]);
        yPos = origin[1] + (y * res[1]);
        zPos = origin[2] + (z * res[2]);
        featureId = m_FeatureIds[index];
        phaseId = m_CellPhases[index];
        // rgba[0] = 0; rgba[1] = 0; rgba[2] = 0; // Reset the color to black
        symmetry = m_CrystalStructures[phaseId];
        if(phaseId > 0)
        {
          if(symmetry == Ebsd::CrystalStructure::Cubic_High)
          {
            symmetry = Ebsd::Ang::PhaseSymmetry::Cubic;
          }
          else if(symmetry == Ebsd::CrystalStructure::Hexagonal_High)
          {
            symmetry = Ebsd::Ang::PhaseSymmetry::DiHexagonal;
          }
          else
          {
            symmetry = Ebsd::Ang::PhaseSymmetry::UnknownSymmetry;
          }
        }
        else
        {
          symmetry = Ebsd::Ang::PhaseSymmetry::UnknownSymmetry;
        }


        fprintf(f, "%f %f %f %f %f %f %d %d %d\r\n", phi1, phi, phi2, xPos, yPos, zPos, featureId, phaseId, symmetry);
      }
    }
  }



  fclose(f);


  notifyStatusMessage("Complete");
  return err;
}
