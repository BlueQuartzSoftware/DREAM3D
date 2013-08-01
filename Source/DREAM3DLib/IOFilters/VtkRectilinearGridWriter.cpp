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


#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/MXADir.h"

#include "EbsdLib/TSL/AngFields.h"
#include "EbsdLib/HKL/CtfFields.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkRectilinearGridWriter::VtkRectilinearGridWriter() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_ParentIdsArrayName(DREAM3D::CellData::ParentIds),
m_CellPhasesArrayName(DREAM3D::CellData::Phases),
m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
m_GlobAlphaArrayName(DREAM3D::CellData::GlobAlpha),
m_BCArrayName(Ebsd::Ctf::BC),
m_ConfidenceIndexArrayName(Ebsd::Ang::ConfidenceIndex),
m_ImageQualityArrayName(Ebsd::Ang::ImageQuality),
m_GrainReferenceMisorientationsArrayName(DREAM3D::CellData::GrainReferenceMisorientations),
m_GrainReferenceCAxisMisorientationsArrayName(DREAM3D::CellData::GrainReferenceCAxisMisorientations),
m_KernelAverageMisorientationsArrayName(DREAM3D::CellData::KernelAverageMisorientations),
m_GBEuclideanDistancesArrayName(DREAM3D::CellData::GBEuclideanDistances),
m_TJEuclideanDistancesArrayName(DREAM3D::CellData::TJEuclideanDistances),
m_QPEuclideanDistancesArrayName(DREAM3D::CellData::QPEuclideanDistances),
m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
m_EquivalentDiametersArrayName(DREAM3D::FieldData::EquivalentDiameters),
m_SchmidsArrayName(DREAM3D::FieldData::Schmids),
m_WriteGrainIds(false),
m_WriteParentIds(false),
m_WritePhaseIds(false),
m_WriteBandContrasts(false),
m_WriteConfidenceIndicies(false),
m_WriteImageQualities(false),
m_WriteGoodVoxels(false),
m_WriteGlobAlpha(false),
m_WriteGrainReferenceMisorientations(false),
m_WriteGrainReferenceCAxisMisorientations(false),
m_WriteKernelAverageMisorientations(false),
m_WriteIPFColors(false),
m_WriteGBEuclideanDistanceMap(false),
m_WriteTJEuclideanDistanceMap(false),
m_WriteQPEuclideanDistanceMap(false),
m_WriteSchmidFactors(false),
m_WriteGrainSizes(false),
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
  std::vector<FilterParameter::Pointer> parameters;
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
    option->setHumanLabel("Write Grain Ids");
    option->setPropertyName("WriteGrainIds");
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
  }  {
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
    option->setHumanLabel("Write Grain Reference Misorientation Values");
    option->setPropertyName("WriteGrainReferenceMisorientations");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Grain Reference CAxis Misorientation Values");
    option->setPropertyName("WriteGrainReferenceCAxisMisorientations");
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
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write IPF Colors");
    option->setPropertyName("WriteIPFColors");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
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
    option->setHumanLabel("Write Grain Sizes");
    option->setPropertyName("WriteGrainSizes");
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
void VtkRectilinearGridWriter::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkRectilinearGridWriter::writeFilterParameters(AbstractFilterParametersWriter* writer)

{
  writer->writeValue("OutputFile", getOutputFile() );
  writer->writeValue("WriteGrainIds", getWriteGrainIds() );
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
  writer->writeValue("WriteGrainReferenceMisorientations", getWriteGrainReferenceMisorientations() );
  writer->writeValue("WriteGrainReferenceCAxisMisorientations", getWriteGrainReferenceCAxisMisorientations() );
  writer->writeValue("WriteKernelAverageMisorientations", getWriteKernelAverageMisorientations() );
  writer->writeValue("WriteIPFColors", getWriteIPFColors() );
  writer->writeValue("WriteSchmidFactors", getWriteSchmidFactors() );
  writer->writeValue("WriteGrainSizes", getWriteGrainSizes() );
  writer->writeValue("WriteBinaryFile", getWriteBinaryFile() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkRectilinearGridWriter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  if(m_OutputFile.empty() == true)
  {
    ss << "The output file must be set before executing this filter.";
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    setErrorCondition(-1);
  }

  // Make sure what we are checking is an actual file name and not a directory
  if (MXAFileInfo::isDirectory(m_OutputFile) == false)
  {
    std::string parentPath = MXAFileInfo::parentPath(m_OutputFile);
    if (MXADir::exists(parentPath) == false)
    {
      ss.str("");
      ss <<  "The directory path for the output file does not exist.";
      addWarningMessage(getHumanLabel(), ss.str(), -1);
    }
  }
  else
  {
      ss.str("");
      ss <<  "The output file path is a path to an existing directory. Please change the path to point to a file";
      addErrorMessage(getHumanLabel(), ss.str(), -1);
      setErrorCondition(-1);
  }

  if(m_WriteGrainIds == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -301, int32_t, Int32ArrayType, voxels, 1)
  }
  if(m_WriteParentIds == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, ParentIds, ss, -301, int32_t, Int32ArrayType, voxels, 1)
  }
  if(m_WritePhaseIds == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, -302, int32_t, Int32ArrayType, voxels, 1)
  }
  if(m_WriteGoodVoxels == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, GoodVoxels, ss, -303, bool, BoolArrayType, voxels, 1)
  }
  if(m_WriteGlobAlpha == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, GlobAlpha, ss, -303, int32_t, Int32ArrayType, voxels, 1)
  }
  if(m_WriteKernelAverageMisorientations == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, KernelAverageMisorientations, ss, -303, float, FloatArrayType, voxels, 1)
  }
  if(m_WriteGrainReferenceMisorientations == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, GrainReferenceMisorientations, ss, -303, float, FloatArrayType, voxels, 1)
  }
  if(m_WriteGrainReferenceCAxisMisorientations == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, GrainReferenceCAxisMisorientations, ss, -303, float, FloatArrayType, voxels, 1)
  }
  if(m_WriteBandContrasts == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, BC, ss, -304, int32_t, Int32ArrayType, voxels, 1)
  }
  if(m_WriteConfidenceIndicies == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, ConfidenceIndex, ss, -304, float, FloatArrayType, voxels, 1)
  }
  if(m_WriteImageQualities == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, ImageQuality, ss, -304, float, FloatArrayType, voxels, 1)
  }
  if(m_WriteIPFColors == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, ss, -305, float, FloatArrayType, voxels, 3)
  }
  if(m_WriteGBEuclideanDistanceMap == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, GBEuclideanDistances, ss, -305, float, FloatArrayType, voxels, 1)
  }
  if(m_WriteTJEuclideanDistanceMap == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, TJEuclideanDistances, ss, -305, float, FloatArrayType, voxels, 1)
  }
  if(m_WriteQPEuclideanDistanceMap == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, QPEuclideanDistances, ss, -305, float, FloatArrayType, voxels, 1)
  }
  if(m_WriteSchmidFactors == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, FieldData, Schmids, ss, -305, float, FloatArrayType, fields, 1)
  }
  if(m_WriteEulerAngles == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, ss, -305, float, FloatArrayType, voxels, 3)
  }

  if(m_WriteGrainSizes == true)
  {
    GET_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, ss, -305, float, FloatArrayType, fields, 1)
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkRectilinearGridWriter::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkRectilinearGridWriter::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  std::string parentPath = MXAFileInfo::parentPath(m_OutputFile);
  if(!MXADir::mkdir(parentPath, true))
  {
      std::stringstream ss;
      ss << "Error creating parent path '" << parentPath << "'";
      notifyErrorMessage(ss.str(), -1);
      setErrorCondition(-1);
      return;
  }

  int64_t totalPoints = m->getTotalPoints();
  size_t totalFields = m->getNumFieldTuples();
  size_t totalEnsembleTuples = m->getNumEnsembleTuples();


  dataCheck(false, totalPoints, totalFields, totalEnsembleTuples);



  // Setup all the classes that will help us write the Scalars to the VTK File
  std::vector<VtkScalarWriter*> scalarsToWrite;

  if (m_WriteGrainIds == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelGrainIdScalarWriter<VoxelDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
  }

  if (m_WriteGBEuclideanDistanceMap == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelGBEDMScalarWriter<VoxelDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
  }

  if (m_WriteTJEuclideanDistanceMap == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelTJEDMScalarWriter<VoxelDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
  }

  if (m_WriteQPEuclideanDistanceMap == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelQPEDMScalarWriter<VoxelDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
  }

  if (m_WriteParentIds == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelParentIdScalarWriter<VoxelDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
  }

  if(m_WritePhaseIds == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelPhaseIdScalarWriter<VoxelDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
  }

  if(m_WriteBandContrasts == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelBCScalarWriter<VoxelDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
  }

  if(m_WriteImageQualities == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelIQScalarWriter<VoxelDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
  }

  if(m_WriteConfidenceIndicies == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelCIScalarWriter<VoxelDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
  }

  if (m_WriteGoodVoxels == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelGoodVoxelScalarWriter<VoxelDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
  }

  if (m_WriteGlobAlpha == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelGlobAlphaScalarWriter<VoxelDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
  }

  if (m_WriteKernelAverageMisorientations == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelKernelAverageMisorientationScalarWriter<VoxelDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
  }

  if (m_WriteGrainReferenceMisorientations == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelGrainReferenceMisorientationScalarWriter<VoxelDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
  }

  if (m_WriteGrainReferenceCAxisMisorientations == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelGrainReferenceCAxisMisorientationScalarWriter<VoxelDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
  }

  if(m_WriteIPFColors == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelIPFColorScalarWriter<VoxelDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
  }

  if (m_WriteEulerAngles == true)
  {

    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new VoxelEulerAngleScalarWriter<VoxelDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
  }

  if(m_WriteGrainSizes == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new FieldSizeScalarWriter<VoxelDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
    scalarsToWrite.push_back(w0);
  }

  if(m_WriteSchmidFactors == true)
  {
    VtkScalarWriter* w0 = static_cast<VtkScalarWriter*>(new SchmidFactorScalarWriter<VoxelDataContainer>(m));
    w0->m_WriteBinaryFiles = m_WriteBinaryFile;
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
    std::stringstream ss;
    ss << "Error writing output vtk file '" << m_OutputFile << "'\n ";
    addErrorMessage(getHumanLabel(), ss.str(), err);
    setErrorCondition(-1);
  }

  notifyStatusMessage("Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkRectilinearGridWriter::write(const std::string &file, VoxelDataContainer* r, std::vector<VtkScalarWriter*> &scalars)
{
  int err = 0;
  FILE* f = NULL;
  f = fopen(file.c_str(), "wb");
  if(NULL == f)
  {
    return -1;
  }
  // Write the correct header
  if(m_WriteBinaryFile == true)
  {
    WRITE_RECTILINEAR_GRID_HEADER("BINARY", r, r->getXPoints() + 1, r->getYPoints()+1, r->getZPoints()+1)
  }
  else
  {
    WRITE_RECTILINEAR_GRID_HEADER("ASCII", r, r->getXPoints() + 1, r->getYPoints()+1, r->getZPoints()+1)
  }

  // Write the XCoords
  VtkRectilinearGridWriter::WriteCoords(f, "X_COORDINATES", "float", r->getXPoints() + 1, 0.0f - r->getXRes() * 0.5f, (float)(r->getXPoints() + 1 * r->getXRes()), r->getXRes(), m_WriteBinaryFile);
  VtkRectilinearGridWriter::WriteCoords(f, "Y_COORDINATES", "float", r->getYPoints() + 1, 0.0f - r->getYRes() * 0.5f, (float)(r->getYPoints() + 1 * r->getYRes()), r->getYRes(), m_WriteBinaryFile);
  VtkRectilinearGridWriter::WriteCoords(f, "Z_COORDINATES", "float", r->getZPoints() + 1, 0.0f - r->getZRes() * 0.5f, (float)(r->getZPoints() + 1 * r->getZRes()), r->getZRes(), m_WriteBinaryFile);

  size_t total = r->getXPoints() * r->getYPoints() * r->getZPoints();
  fprintf(f, "CELL_DATA %d\n", (int)total);

  std::stringstream ss;
  int index = 0;
  // Now loop on all of our Scalars and write those arrays as CELL_DATA
  for (std::vector<VtkScalarWriter*>::iterator iter = scalars.begin(); iter != scalars.end(); ++iter)
  {
    ss.str("");
    ss << "Writing Scalar " << index++ << " of " << scalars.size();
    notifyStatusMessage(ss.str());
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


