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


#include "VtkRectilinearGridWriter.h"


#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "EbsdLib/TSL/AngFields.h"
#include "EbsdLib/HKL/CtfFields.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkRectilinearGridWriter::VtkRectilinearGridWriter() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_ParentIdsArrayName(DREAM3D::CellData::ParentIds),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_GlobAlphaArrayName(DREAM3D::CellData::GlobAlpha),
  m_BCArrayName(Ebsd::Ctf::BC),
  m_ConfidenceIndexArrayName(Ebsd::Ang::ConfidenceIndex),
  m_ImageQualityArrayName(Ebsd::Ang::ImageQuality),
  m_FeatureReferenceMisorientationsArrayName(DREAM3D::CellData::FeatureReferenceMisorientations),
  m_FeatureReferenceCAxisMisorientationsArrayName(DREAM3D::CellData::FeatureReferenceCAxisMisorientations),
  m_KernelAverageMisorientationsArrayName(DREAM3D::CellData::KernelAverageMisorientations),
  m_GBEuclideanDistancesArrayName(DREAM3D::CellData::GBEuclideanDistances),
  m_TJEuclideanDistancesArrayName(DREAM3D::CellData::TJEuclideanDistances),
  m_QPEuclideanDistancesArrayName(DREAM3D::CellData::QPEuclideanDistances),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_EquivalentDiametersArrayName(DREAM3D::FeatureData::EquivalentDiameters),
  m_SchmidsArrayName(DREAM3D::FeatureData::Schmids),
  m_WriteFeatureIds(false),
  m_WriteParentIds(false),
  m_WritePhaseIds(false),
  m_WriteBandContrasts(false),
  m_WriteConfidenceIndicies(false),
  m_WriteImageQualities(false),
  m_WriteGoodVoxels(false),
  m_WriteGlobAlpha(false),
  m_WriteFeatureReferenceMisorientations(false),
  m_WriteFeatureReferenceCAxisMisorientations(false),
  m_WriteKernelAverageMisorientations(false),
  //m_WriteIPFColors(false),
  m_WriteGBEuclideanDistanceMap(false),
  m_WriteTJEuclideanDistanceMap(false),
  m_WriteQPEuclideanDistanceMap(false),
  m_WriteSchmidFactors(false),
  m_WriteFeatureSizes(false),
  m_WriteEulerAngles(false),
  m_WriteBinaryFile(false)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkRectilinearGridWriter::~VtkRectilinearGridWriter()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkRectilinearGridWriter::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("*.vtk");
    option->setFileType("VTK Rectilinear Grid");
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Feature Ids");
    option->setPropertyName("WriteFeatureIds");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Parent Ids");
    option->setPropertyName("WriteParentIds");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Phase Ids");
    option->setPropertyName("WritePhaseIds");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Band Contrasts");
    option->setPropertyName("WriteBandContrasts");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write ImageQualities");
    option->setPropertyName("WriteImageQualities");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write ConfidenceIndicies");
    option->setPropertyName("WriteConfidenceIndicies");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Kernel Average Misorientation Value");
    option->setPropertyName("WriteKernelAverageMisorientations");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Feature Reference Misorientation Values");
    option->setPropertyName("WriteFeatureReferenceMisorientations");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Feature Reference CAxis Misorientation Values");
    option->setPropertyName("WriteFeatureReferenceCAxisMisorientations");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Good Voxels");
    option->setPropertyName("WriteGoodVoxels");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Glob Alpha");
    option->setPropertyName("WriteGlobAlpha");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  //  {
  //    FilterParameter::Pointer option = FilterParameter::New();
  //    option->setHumanLabel("Write IPF Colors");
  //    option->setPropertyName("WriteIPFColors");
  //    option->setWidgetType(FilterParameter::BooleanWidget);
  //    option->setValueType("bool");
  //    parameters.push_back(option);
  //  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Schmid Factors");
    option->setPropertyName("WriteSchmidFactors");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write GB Euclidean Distance Map");
    option->setPropertyName("WriteGBEuclideanDistanceMap");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write TJ Euclidean Distance Map");
    option->setPropertyName("WriteTJEuclideanDistanceMap");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write QP Euclidean Distance Map");
    option->setPropertyName("WriteQPEuclideanDistanceMap");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Feature Sizes");
    option->setPropertyName("WriteFeatureSizes");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Euler Angles");
    option->setPropertyName("WriteEulerAngles");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Binary File");
    option->setPropertyName("WriteBinaryFile");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void VtkRectilinearGridWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setOutputFile( reader->readString( "OutputFile", getOutputFile() ) );
  setWriteFeatureIds( reader->readValue("WriteFeatureIds", false) );
  setWriteParentIds( reader->readValue("WriteParentIds", false) );
  setWritePhaseIds( reader->readValue("WritePhaseIds", false) );
  setWriteBandContrasts( reader->readValue("WriteBandContrasts", false) );
  setWriteImageQualities( reader->readValue("WriteImageQualities", false) );
  setWriteConfidenceIndicies( reader->readValue("WriteConfidenceIndicies", false) );
  setWriteKernelAverageMisorientations( reader->readValue("WriteKernelAverageMisorientations", false) );
  setWriteFeatureReferenceMisorientations( reader->readValue("WriteFeatureReferenceMisorientations", false) );
  setWriteFeatureReferenceCAxisMisorientations( reader->readValue("WriteFeatureReferenceCAxisMisorientations", false) );
  setWriteGoodVoxels( reader->readValue("WriteGoodVoxels", false) );
  setWriteGlobAlpha( reader->readValue("WriteGlobAlpha", false) );
  //  setWriteIPFColors( reader->readValue("WriteIPFColors", false) );
  setWriteSchmidFactors( reader->readValue("WriteSchmidFactors", false) );
  setWriteGBEuclideanDistanceMap( reader->readValue("WriteGBEuclideanDistanceMap", false) );
  setWriteTJEuclideanDistanceMap( reader->readValue("WriteTJEuclideanDistanceMap", false) );
  setWriteQPEuclideanDistanceMap( reader->readValue("WriteQPEuclideanDistanceMap", false) );
  setWriteFeatureSizes( reader->readValue("WriteFeatureSizes", false) );
  setWriteEulerAngles( reader->readValue("WriteEulerAngles", false) );
  setWriteBinaryFile( reader->readValue("WriteBinaryFile", false) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkRectilinearGridWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputFile", getOutputFile() );
  writer->writeValue("WriteFeatureIds", getWriteFeatureIds() );
  writer->writeValue("WriteParentIds", getWriteParentIds() );
  writer->writeValue("WritePhaseIds", getWritePhaseIds() );
  writer->writeValue("WriteBandContrasts", getWriteBandContrasts() );
  writer->writeValue("WriteImageQualities", getWriteImageQualities() );
  writer->writeValue("WriteConfidenceIndicies", getWriteConfidenceIndicies() );
  writer->writeValue("WriteGoodVoxels", getWriteGoodVoxels() );
  writer->writeValue("WriteGlobAlpha", getWriteGlobAlpha() );
  writer->writeValue("WriteGBEuclideanDistanceMap", getWriteGBEuclideanDistanceMap() );
  writer->writeValue("WriteTJEuclideanDistanceMap", getWriteTJEuclideanDistanceMap() );
  writer->writeValue("WriteQPEuclideanDistanceMap", getWriteQPEuclideanDistanceMap() );
  writer->writeValue("WriteFeatureReferenceMisorientations", getWriteFeatureReferenceMisorientations() );
  writer->writeValue("WriteFeatureReferenceCAxisMisorientations", getWriteFeatureReferenceCAxisMisorientations() );
  writer->writeValue("WriteKernelAverageMisorientations", getWriteKernelAverageMisorientations() );
  //  writer->writeValue("WriteIPFColors", getWriteIPFColors() );
  writer->writeValue("WriteSchmidFactors", getWriteSchmidFactors() );
  writer->writeValue("WriteFeatureSizes", getWriteFeatureSizes() );
  writer->writeValue("WriteBinaryFile", getWriteBinaryFile() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkRectilinearGridWriter::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  if(m_OutputFile.isEmpty() == true)
  {
    QString ss = QObject::tr("The output file must be set before executing this filter.");
    addErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
  }

  // Make sure what we are checking is an actual file name and not a directory
  QFileInfo fi(m_OutputFile);
  if (fi.isDir() == false)
  {
    QDir parentPath = fi.path();
    if (parentPath.exists() == false)
    {
      QString ss = QObject::tr("The directory path for the output file does not exist.");
      addWarningMessage(getHumanLabel(), ss, -1);
    }
  }
  else
  {
    QString ss = QObject::tr("The output file path is a path to an existing directory. Please change the path to point to a file");
    addErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
  }

  QVector<int> dims(1, 1);
  if(m_WriteFeatureIds == true)
  {
    m_FeatureIdsPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, -301, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_WriteParentIds == true)
  {
    m_ParentIdsPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_ParentIdsArrayName, -301, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ParentIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_ParentIds = m_ParentIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_WritePhaseIds == true)
  {
    m_CellPhasesPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_CellPhasesArrayName, -302, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_WriteGoodVoxels == true)
  {
    m_GoodVoxelsPtr = m->getPrereqArray<bool, AbstractFilter>(this, m_CellAttributeMatrixName,  m_GoodVoxelsArrayName, -303, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_WriteGlobAlpha == true)
  {
    m_GlobAlphaPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_GlobAlphaArrayName, -303, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GlobAlphaPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_GlobAlpha = m_GlobAlphaPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_WriteKernelAverageMisorientations == true)
  {
    m_KernelAverageMisorientationsPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellAttributeMatrixName,  m_KernelAverageMisorientationsArrayName, -303, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_KernelAverageMisorientationsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_KernelAverageMisorientations = m_KernelAverageMisorientationsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_WriteFeatureReferenceMisorientations == true)
  {
    m_FeatureReferenceMisorientationsPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellAttributeMatrixName,  m_FeatureReferenceMisorientationsArrayName, -303, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureReferenceMisorientationsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_FeatureReferenceMisorientations = m_FeatureReferenceMisorientationsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_WriteFeatureReferenceCAxisMisorientations == true)
  {
    m_FeatureReferenceCAxisMisorientationsPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellAttributeMatrixName,  m_FeatureReferenceCAxisMisorientationsArrayName, -303, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureReferenceCAxisMisorientationsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_FeatureReferenceCAxisMisorientations = m_FeatureReferenceCAxisMisorientationsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_WriteBandContrasts == true)
  {
    m_BCPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_BCArrayName, -304, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_BCPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_BC = m_BCPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_WriteConfidenceIndicies == true)
  {
    m_ConfidenceIndexPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellAttributeMatrixName,  m_ConfidenceIndexArrayName, -304, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ConfidenceIndexPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_ConfidenceIndex = m_ConfidenceIndexPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_WriteImageQualities == true)
  {
    m_ImageQualityPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellAttributeMatrixName,  m_ImageQualityArrayName, -304, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ImageQualityPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_ImageQuality = m_ImageQualityPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_WriteGBEuclideanDistanceMap == true)
  {
    m_GBEuclideanDistancesPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellAttributeMatrixName,  m_GBEuclideanDistancesArrayName, -305, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GBEuclideanDistancesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_GBEuclideanDistances = m_GBEuclideanDistancesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_WriteTJEuclideanDistanceMap == true)
  {
    m_TJEuclideanDistancesPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellAttributeMatrixName,  m_TJEuclideanDistancesArrayName, -305, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_TJEuclideanDistancesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_TJEuclideanDistances = m_TJEuclideanDistancesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_WriteQPEuclideanDistanceMap == true)
  {
    m_QPEuclideanDistancesPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellAttributeMatrixName,  m_QPEuclideanDistancesArrayName, -305, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QPEuclideanDistancesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_QPEuclideanDistances = m_QPEuclideanDistancesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_WriteSchmidFactors == true)
  {
    m_SchmidsPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_SchmidsArrayName, -305, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SchmidsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_Schmids = m_SchmidsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_WriteFeatureSizes == true)
  {
    m_EquivalentDiametersPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_EquivalentDiametersArrayName, -305, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_EquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  if(m_WriteEulerAngles == true)
  {
    dims[0] = 3;
    m_CellEulerAnglesPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellAttributeMatrixName,  m_CellEulerAnglesArrayName, -305, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkRectilinearGridWriter::preflight()
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
void VtkRectilinearGridWriter::execute()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(m_OutputFile);
  QString parentPath = fi.path();
  QDir dir;
  if(!dir.mkpath(parentPath))
  {

    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
    notifyErrorMessage(ss, -1);
    setErrorCondition(-1);
    return;
  }

  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();
  size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  size_t totalEnsembleTuples = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  dataCheck(false, totalPoints, totalFeatures, totalEnsembleTuples);

  // Setup all the classes that will help us write the Scalars to the VTK File
  std::vector<VtkScalarWriter*> scalarsToWrite;

  if (m_WriteFeatureIds == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelFeatureIdScalarWriter<VolumeDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    w0->setAttributeMatrixName(getCellAttributeMatrixName());
    scalarsToWrite.push_back(w0);
  }

  if (m_WriteGBEuclideanDistanceMap == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelGBEDMScalarWriter<VolumeDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    w0->setAttributeMatrixName(getCellAttributeMatrixName());
    scalarsToWrite.push_back(w0);
  }

  if (m_WriteTJEuclideanDistanceMap == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelTJEDMScalarWriter<VolumeDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    w0->setAttributeMatrixName(getCellAttributeMatrixName());
    scalarsToWrite.push_back(w0);
  }

  if (m_WriteQPEuclideanDistanceMap == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelQPEDMScalarWriter<VolumeDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    w0->setAttributeMatrixName(getCellAttributeMatrixName());
    scalarsToWrite.push_back(w0);
  }

  if (m_WriteParentIds == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelParentIdScalarWriter<VolumeDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    w0->setAttributeMatrixName(getCellAttributeMatrixName());
    scalarsToWrite.push_back(w0);
  }

  if(m_WritePhaseIds == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelPhaseIdScalarWriter<VolumeDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    w0->setAttributeMatrixName(getCellAttributeMatrixName());
    scalarsToWrite.push_back(w0);
  }

  if(m_WriteBandContrasts == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelBCScalarWriter<VolumeDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
    w0->setAttributeMatrixName(getCellAttributeMatrixName());
  }

  if(m_WriteImageQualities == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelIQScalarWriter<VolumeDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    w0->setAttributeMatrixName(getCellAttributeMatrixName());
    scalarsToWrite.push_back(w0);
  }

  if(m_WriteConfidenceIndicies == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelCIScalarWriter<VolumeDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    w0->setAttributeMatrixName(getCellAttributeMatrixName());
    scalarsToWrite.push_back(w0);
  }

  if (m_WriteGoodVoxels == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelGoodVoxelScalarWriter<VolumeDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    w0->setAttributeMatrixName(getCellAttributeMatrixName());
    scalarsToWrite.push_back(w0);
  }

  if (m_WriteGlobAlpha == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelGlobAlphaScalarWriter<VolumeDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    w0->setAttributeMatrixName(getCellAttributeMatrixName());
    scalarsToWrite.push_back(w0);
  }

  if (m_WriteKernelAverageMisorientations == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelKernelAverageMisorientationScalarWriter<VolumeDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    w0->setAttributeMatrixName(getCellAttributeMatrixName());
    scalarsToWrite.push_back(w0);
  }

  if (m_WriteFeatureReferenceMisorientations == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelFeatureReferenceMisorientationScalarWriter<VolumeDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    w0->setAttributeMatrixName(getCellAttributeMatrixName());
    scalarsToWrite.push_back(w0);
  }

  if (m_WriteFeatureReferenceCAxisMisorientations == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelFeatureReferenceCAxisMisorientationScalarWriter<VolumeDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    w0->setAttributeMatrixName(getCellAttributeMatrixName());
    scalarsToWrite.push_back(w0);
  }

  //  if(m_WriteIPFColors == true)
  //  {
  //    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelIPFColorScalarWriter<VolumeDataContainer>(m));
  //    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
  //    w0->setAttributeMatrixName(getCellAttributeMatrixName());
  //    scalarsToWrite.push_back(w0);
  //  }

  if (m_WriteEulerAngles == true)
  {

    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelEulerAngleScalarWriter<VolumeDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    w0->setAttributeMatrixName(getCellAttributeMatrixName());
    scalarsToWrite.push_back(w0);
  }

  if(m_WriteFeatureSizes == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new FeatureSizeScalarWriter<VolumeDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    w0->setAttributeMatrixName(getCellFeatureAttributeMatrixName());
    scalarsToWrite.push_back(w0);
  }

  if(m_WriteSchmidFactors == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new SchmidFactorScalarWriter<VolumeDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    w0->setAttributeMatrixName(getCellFeatureAttributeMatrixName());
    scalarsToWrite.push_back(w0);
  }

  int err = write(m_OutputFile, m, scalarsToWrite);
  // Now Delete all the Scalar Helpers that we just created and used.
  for (std::vector<VtkScalarWriter*>::iterator iter = scalarsToWrite.begin(); iter != scalarsToWrite.end(); ++iter)
  {
    delete (*iter);
  }

  if (err < 0)
  {

    QString ss = QObject::tr("Error writing output vtk file '%1'\n ").arg(m_OutputFile);
    addErrorMessage(getHumanLabel(), ss, err);
    setErrorCondition(-1);
  }

  notifyStatusMessage("Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkRectilinearGridWriter::write(const QString& file, VolumeDataContainer* r, std::vector<VtkScalarWriter*>& scalars)
{
  int err = 0;
  FILE* f = NULL;
  f = fopen(file.toLatin1().data(), "wb");
  if(NULL == f)
  {
    return -1;
  }
  // Write the correct header
  if(m_WriteBinaryFile == true)
  {
    WRITE_RECTILINEAR_GRID_HEADER("BINARY", r, r->getXPoints() + 1, r->getYPoints() + 1, r->getZPoints() + 1)
  }
  else
  {
    WRITE_RECTILINEAR_GRID_HEADER("ASCII", r, r->getXPoints() + 1, r->getYPoints() + 1, r->getZPoints() + 1)
  }

  // Write the XCoords
  VtkRectilinearGridWriter::WriteCoords(f, "X_COORDINATES", "float", r->getXPoints() + 1, 0.0f - r->getXRes() * 0.5f, (float)(r->getXPoints() + 1 * r->getXRes()), r->getXRes(), m_WriteBinaryFile);
  VtkRectilinearGridWriter::WriteCoords(f, "Y_COORDINATES", "float", r->getYPoints() + 1, 0.0f - r->getYRes() * 0.5f, (float)(r->getYPoints() + 1 * r->getYRes()), r->getYRes(), m_WriteBinaryFile);
  VtkRectilinearGridWriter::WriteCoords(f, "Z_COORDINATES", "float", r->getZPoints() + 1, 0.0f - r->getZRes() * 0.5f, (float)(r->getZPoints() + 1 * r->getZRes()), r->getZRes(), m_WriteBinaryFile);

  size_t total = r->getXPoints() * r->getYPoints() * r->getZPoints();
  fprintf(f, "CELL_DATA %d\n", (int)total);


  int index = 0;
  // Now loop on all of our Scalars and write those arrays as CELL_DATA
  for (std::vector<VtkScalarWriter*>::iterator iter = scalars.begin(); iter != scalars.end(); ++iter)
  {

    QString ss = QObject::tr("Writing Scalar %1 of %2").arg(index++).arg(scalars.size());
    notifyStatusMessage(ss);
    err = (*iter)->writeScalars(f);
    if(err < 0)
    {
      setErrorCondition((*iter)->getErrorCondition());
      setPipelineMessages((*iter)->getPipelineMessages());
      break;
    }
  }
  // Close the file
  fclose(f);
  return err;
}


