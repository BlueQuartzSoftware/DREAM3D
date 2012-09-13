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
#include "Crop_Volume_Pipeline.h" 
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "MXA/MXA.h"
#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXADir.h"

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/HKL/CtfConstants.h"


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Observer.h"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/Common/QualityMetricFilter.h"
#include "DREAM3DLib/IOFilters/DataContainerWriter.h"
#include "DREAM3DLib/IOFilters/DataContainerReader.h"
#include "DREAM3DLib/IOFilters/FieldDataCSVWriter.h"
#include "DREAM3DLib/IOFilters/VtkRectilinearGridWriter.h"
#include "DREAM3DLib/IOFilters/ReadH5Ebsd.h"
#include "DREAM3DLib/ReconstructionFilters/AlignSectionsMisorientation.h"
#include "DREAM3DLib/ReconstructionFilters/EBSDSegmentGrains.h"
#include "DREAM3DLib/ProcessingFilters/MinSize.h"
#include "DREAM3DLib/ProcessingFilters/OpenCloseBadData.h"
#include "DREAM3DLib/ProcessingFilters/MinNeighbors.h"
#include "DREAM3DLib/ProcessingFilters/ConvertEulerAngles.h"
#include "DREAM3DLib/GenericFilters/FindNeighbors.h"
#include "DREAM3DLib/SamplingFilters/CropVolume.h"
#include "DREAM3DLib/SamplingFilters/RegularizeZSpacing.h"
#include "DREAM3DLib/StatisticsFilters/FindSizes.h"
#include "DREAM3DLib/StatisticsFilters/FindShapes.h"
#include "DREAM3DLib/StatisticsFilters/FindAvgOrientations.h"
#include "DREAM3DLib/StatisticsFilters/FindNeighborhoods.h"
#include "DREAM3DLib/StatisticsFilters/FindODF.h"
#include "DREAM3DLib/StatisticsFilters/FindMDF.h"


using namespace std;


#define DREAM3D_BENCHMARKS 1

#if DREAM3D_BENCHMARKS
#define START_CLOCK()\
  unsigned long long int millis;\
  millis = MXA::getMilliSeconds();
#else
#define START_CLOCK() unsigned long long int millis = 0;\
  millis = 0;
#endif

typedef int64_t DimType;

Crop_Volume_Pipeline::Crop_Volume_Pipeline() :
AbstractFilter()
{
  setupFilterParameters();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------


Crop_Volume_Pipeline::~Crop_Volume_Pipeline()
{
}

void Crop_Volume_Pipeline::setupFilterParameters()
{
    // Now append our options
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input File");
    option->setPropertyName("InputFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void Crop_Volume_Pipeline::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("InputFile", getInputFile() );
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Crop_Volume_Pipeline::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{

  setErrorCondition(0);
  std::stringstream ss;


  if(true == m_InputFile.empty())
  {
    ss << "The Input file name must be set before executing this filter.";
    setErrorCondition(-1);
    addErrorMessage(getHumanLabel(), ss.str(), -1);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Crop_Volume_Pipeline::preflight()
{  
  dataCheck(true, 1, 1, 1);
}







std::string getH5EbsdFile()
{
  std::string s = "D:/IN100_run1/DREAM3D_files/slice_11_173_transformed.h5ebsd";
  return s;
}

std::string getZ_spacingfile()
{
  std::string s = "D:/IN100_run1/DREAM3D_files/Rectilinear_Grid_Data.txt";
  return s;
}

std::string getFieldDataFile()
{
  std::string s =  "D:/IN100_run1/DREAM3D_files/test.csv";
  return s;
}


void Crop_Volume_Pipeline::get_max_and_min_xyz_for_crop(std::vector<int> &xmax, std::vector<int> &ymax, std::vector<int> &zmax, std::vector<int> &xmin, std::vector<int> &ymin, std::vector<int> &zmin)
{
    DataContainer* m = getDataContainer();
  //int64_t totalPoints = m->totalPoints();
  int numinlist;
  ifstream inFile;
  inFile.open(m_InputFile.c_str());

  #if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
  #else
  typedef int64_t DimType;
  #endif

  int a, b, c, d, e, f; 
  //DimType iter = 1;
  inFile >> numinlist;

  for (DimType iter = 1; iter < numinlist+1; iter++)
  {
    //inFile >> xmax[iter] >> ymax[iter] >> zmax[iter] >> xmin[iter] >> ymin[iter] >> zmin[iter];
    //iter = iter+1; 

    inFile >> a >> b >> c >> d >> e >> f ; 
    xmax[iter] = a; 
    ymax[iter] = b; 
    zmax[iter] = c; 
    xmin[iter] = d; 
    ymin[iter] = e; 
    zmin[iter] = f; 

  } 

  inFile.close();
}

unsigned int getNumLinesinFile(std::string filename) 
{ 

  //DataContainer* m = getDataContainer();

  int num_lines; 
  ifstream inFile; 
  inFile.open(filename.c_str());
  inFile >> num_lines;
  inFile.close(); 
  
  return num_lines;  

} 



int getZStartIndex() { return 11; }
int getZEndIndex() { return 21; }
DataArray<unsigned int>::Pointer getPhaseTypes()
{
  DataArray<unsigned int>::Pointer phaseTypes
                = DataArray<unsigned int>::CreateArray(4, DREAM3D::EnsembleData::PhaseTypes);
  phaseTypes->SetValue(0, DREAM3D::PhaseType::UnknownPhaseType);
  phaseTypes->SetValue(1, DREAM3D::PhaseType::PrimaryPhase);
  phaseTypes->SetValue(2, DREAM3D::PhaseType::PrimaryPhase);
  phaseTypes->SetValue(3, DREAM3D::PhaseType::PrimaryPhase);
  return phaseTypes;
}

std::vector<QualityMetricFilter::Pointer> getQualityMetricFilters()
{
  std::vector<QualityMetricFilter::Pointer> filters;
  {
    QualityMetricFilter::Pointer filter = QualityMetricFilter::New();
    filter->setFieldName(Ebsd::Ctf::Error);
    filter->setFieldValue(0);
    filter->setFieldOperator("=");
    filter->setFieldPhaseNumber(1);
    filters.push_back(filter);
  }
  {
    QualityMetricFilter::Pointer filter = QualityMetricFilter::New();
    filter->setFieldName(Ebsd::Ctf::Error);
    filter->setFieldValue(0);
    filter->setFieldOperator("=");
    filter->setFieldPhaseNumber(2);
    filters.push_back(filter);
  }
  {
    QualityMetricFilter::Pointer filter = QualityMetricFilter::New();
    filter->setFieldName(Ebsd::Ctf::Error);
    filter->setFieldValue(0);
    filter->setFieldOperator("=");
    filter->setFieldPhaseNumber(3);
    filters.push_back(filter);
  }


  return filters;
}



void Crop_Volume_Pipeline::execute()
{
  DataContainer* m = getDataContainer();
  float m_MisorientationTolerance = 5.0f;
  float m_AlignMisorientationTolerance = 5.0f;
  float m_Zres = 4.0f;
  int m_MinAllowedGrainSize = 10;
  int m_MinNumNeighbors = 1;
  int m_PhaseNumberMinSize = 1; 
  int m_NumIterations_Erode = 3; 
  int NUM_OF_CROPS = getNumLinesinFile(m_InputFile); 

  std::vector<int> m_Xmin(NUM_OF_CROPS+1, 0);
  std::vector<int> m_Ymin(NUM_OF_CROPS+1, 0);
  std::vector<int> m_Zmin(NUM_OF_CROPS+1, 0);
  std::vector<int> m_Xmax(NUM_OF_CROPS+1, 0);
  std::vector<int> m_Ymax(NUM_OF_CROPS+1, 0);
  std::vector<int> m_Zmax(NUM_OF_CROPS+1, 0);

  get_max_and_min_xyz_for_crop(m_Xmax, m_Ymax, m_Zmax, m_Xmin, m_Ymin, m_Zmin);

  /*  int m_Zmin = 1 ;
  int m_Xmax = 495; 
  int m_Ymax = 355; 
  int m_Zmax = 163;*/ 

for (DimType i = 1; i < 2; i++)
{
      // Create our Pipeline object
     typedef std::vector<AbstractFilter::Pointer> FilterContainerType ;
     FilterContainerType pipeline;
     // typedef DataArray<unsigned int> XTalStructArrayType;
      //XTalStructArrayType::Pointer xtal = XTalStructArrayType::CreateArray(2, DREAM3D::EnsembleData::CrystalStructures);
      //xtal->SetValue(0, Ebsd::CrystalStructure::UnknownCrystalStructure);
      //xtal->SetValue(1, Ebsd::CrystalStructure::Cubic);



     // updateProgressAndMessage(("Loading Slices"), 10);
      ReadH5Ebsd::Pointer read_h5ebsd = ReadH5Ebsd::New();
  
      read_h5ebsd->setH5EbsdFile(getH5EbsdFile());
      //read_h5ebsd->setRefFrameZDir(Ebsd::LowtoHigh);
      read_h5ebsd->setZStartIndex(getZStartIndex());
      read_h5ebsd->setZEndIndex(getZEndIndex());
      read_h5ebsd->setPTypes(getPhaseTypes());
      read_h5ebsd->setQualityMetricFilters(getQualityMetricFilters());
      read_h5ebsd->setDataContainer(m);
      read_h5ebsd->execute();
      pipeline.push_back(read_h5ebsd);
      
 


      ConvertEulerAngles::Pointer convert_euler = ConvertEulerAngles::New();
      convert_euler->setConversionType(DREAM3D::EulerAngleConversionType::DegreesToRadians);
      convert_euler->setDataContainer(m);
     // convert_euler->setPreviousFilter(convert_euler->getPreviousFilter());
      convert_euler->execute(); 
      pipeline.push_back(convert_euler); 
      

//      AlignSectionsMisorientation::Pointer align_sections = AlignSectionsMisorientation::New();
//      align_sections->setMisorientationTolerance(m_AlignMisorientationTolerance); 
//      align_sections->setDataContainer(m);
//// align_sections->setPreviousFilter(align_sections->getPreviousFilter());
//      align_sections->execute();     
//      pipeline.push_back(align_sections);
//
//      CropVolume::Pointer crop_volume = CropVolume::New(); 
//      crop_volume->setXMin(m_Xmin[i]);
//      crop_volume->setYMin(m_Ymin[i]);
//      crop_volume->setZMin(m_Zmin[i]);
//      crop_volume->setXMax(m_Xmax[i]);
//      crop_volume->setYMax(m_Ymax[i]);
//      crop_volume->setZMax(m_Zmax[i]); 
//      crop_volume->setRenumberGrains(false); 
//      crop_volume->setDataContainer(m);
//    //  crop_volume->setPreviousFilter(crop_volume->getPreviousFilter());
//      crop_volume->execute();
//      pipeline.push_back(crop_volume);
//
//
//      bool m_WriteVtkFile(true);
//      bool m_WriteGrainID(false);
//      bool m_WriteBinaryVTKFiles(true);
//      bool m_WritePhaseId(false);
//      bool m_WriteIPFColor(true);
//      bool m_WriteGoodVoxels(true);
//      bool m_WriteGrainSizes(false);
//      bool m_WriteBandContrasts(true); 
//
//      VtkRectilinearGridWriter::Pointer vtkWriter = VtkRectilinearGridWriter::New();
//      if(m_WriteVtkFile)
//      {
//        std::string vtk_file = "D:/IN100_run1/DREAM3D_files/test.vtk";
//        vtkWriter->setOutputFile(vtk_file);
//        vtkWriter->setWriteGrainIds(m_WriteGrainID);
//        vtkWriter->setWritePhaseIds(m_WritePhaseId);
//        vtkWriter->setWriteBandContrasts(m_WriteBandContrasts);
//        vtkWriter->setWriteGoodVoxels(m_WriteGoodVoxels);
//        vtkWriter->setWriteIPFColors(m_WriteIPFColor);
//        vtkWriter->setWriteBinaryFile(m_WriteBinaryVTKFiles);
//        vtkWriter->setWriteBinaryFile(m_WriteGrainSizes);
//        vtkWriter->setDataContainer(m); 
//     //   vtkWriter->setPreviousFilter(vtkWriter->getPreviousFilter());
//        vtkWriter->execute(); 
//        pipeline.push_back(vtkWriter);
//      }
//
//      RegularizeZSpacing::Pointer regularize_z = RegularizeZSpacing::New(); 
//      regularize_z->setInputFile(getZ_spacingfile()); 
//      regularize_z->setZRes(m_Zres); 
//      regularize_z->setDataContainer(m);
//    //  regularize_z->setPreviousFilter(regularize_z->getPreviousFilter());
//      regularize_z->execute();
//      pipeline.push_back(regularize_z);
//
//      EBSDSegmentGrains::Pointer ebsdsegment_grains = EBSDSegmentGrains::New();
//      ebsdsegment_grains->setMisorientationTolerance(m_MisorientationTolerance);
//      ebsdsegment_grains->setDataContainer(m);
//   //   ebsdsegment_grains->setPreviousFilter(ebsdsegment_grains->getPreviousFilter());
//      ebsdsegment_grains->execute();
//      pipeline.push_back(ebsdsegment_grains);
//
//      OpenCloseBadData::Pointer erode_dilate = OpenCloseBadData::New(); 
//      erode_dilate->setDirection(0); // 0 is erode? 
//      erode_dilate->setNumIterations(m_NumIterations_Erode); 
//      erode_dilate->setDataContainer(m);
//   //   erode_dilate->setPreviousFilter(erode_dilate->getPreviousFilter());
//      erode_dilate->execute();
//      pipeline.push_back(erode_dilate);
//
//
//      MinSize::Pointer min_size = MinSize::New();
//      min_size->setMinAllowedGrainSize(m_MinAllowedGrainSize);
//      min_size->setPhaseNumber(m_PhaseNumberMinSize);
//      min_size->setDataContainer(m);
//    //  min_size->setPreviousFilter(min_size->getPreviousFilter());
//      min_size->execute();
//      pipeline.push_back(min_size);
//
//      MinNeighbors::Pointer min_neighbors = MinNeighbors::New();
//      min_neighbors->setMinNumNeighbors(m_MinNumNeighbors);
//      min_neighbors->setDataContainer(m);
//    //  min_neighbors->setPreviousFilter(min_neighbors->getPreviousFilter());
//      min_neighbors->execute();
//      pipeline.push_back(min_neighbors);
//
//      FindSizes::Pointer find_sizes = FindSizes::New(); 
//      find_sizes->setDistributionType(DREAM3D::DistributionType::Beta);
//      find_sizes->setDataContainer(m);
//    //  find_sizes->setPreviousFilter(find_sizes->getPreviousFilter());
//      find_sizes->execute();
//      pipeline.push_back(find_sizes);
//
//
//      FindShapes::Pointer find_shapes = FindShapes::New(); 
//      find_shapes->setDistributionType(DREAM3D::DistributionType::Beta);
//      find_shapes->setDataContainer(m);
//     // find_shapes->setPreviousFilter(find_shapes->getPreviousFilter());
//      find_shapes->execute();
//      pipeline.push_back(find_shapes);
//
//
//
//      FieldDataCSVWriter::Pointer field_data_write_csv = FieldDataCSVWriter::New(); 
//      field_data_write_csv->setFieldDataFile(getFieldDataFile()); 
//      field_data_write_csv->setDataContainer(m); 
//     // field_data_write_csv->setPreviousFilter(field_data_write_csv->getPreviousFilter());
//      field_data_write_csv->execute(); 
//      pipeline.push_back(field_data_write_csv);
//
//
//
//      m_WriteVtkFile = true ; 
//      m_WriteBinaryVTKFiles= true ; 
//      m_WriteGrainID= true;
//      m_WritePhaseId= true ; 
//      m_WriteIPFColor= true ; 
//      m_WriteGoodVoxels= true ; 
//      m_WriteGrainSizes = true ; 
//      m_WriteBandContrasts = true ; 
//
//      vtkWriter = VtkRectilinearGridWriter::New();
//      if(m_WriteVtkFile)
//      {
//        std::string vtk_file = "D:/IN100_run1/DREAM3D_files/test_final.vtk";
//        vtkWriter->setOutputFile(vtk_file);
//        vtkWriter->setWriteGrainIds(m_WriteGrainID);
//        vtkWriter->setWritePhaseIds(m_WritePhaseId);
//        vtkWriter->setWriteBandContrasts(m_WriteBandContrasts);
//        vtkWriter->setWriteGoodVoxels(m_WriteGoodVoxels);
//        vtkWriter->setWriteIPFColors(m_WriteIPFColor);
//        vtkWriter->setWriteBinaryFile(m_WriteBinaryVTKFiles);
//        vtkWriter->setWriteBinaryFile(m_WriteGrainSizes);
//        vtkWriter->setDataContainer(m); 
//        //vtkWriter->setPreviousFilter(vtkWriter->getPreviousFilter());
//        vtkWriter->execute(); 
//        pipeline.push_back(vtkWriter);
//      }

      DataContainerWriter::Pointer writer = DataContainerWriter::New();
      std::string dream_3d_file = "D:/IN100_run1/DREAM3D_files/test.dream3d";
      writer->setOutputFile(dream_3d_file);
      writer->setDataContainer(m); 

  FilterContainerType::iterator prev;
  FilterContainerType::iterator next;

  for (FilterContainerType::iterator iter = pipeline.begin(); iter != pipeline.end(); ++iter)
  {
    // currFilt = *iter;
    if(iter != pipeline.begin())
    {
      prev = iter;
      prev--;
      // prevFilt = *prev;
      (*iter)->setPreviousFilter(*prev);
    }

    if(iter != pipeline.end())
    {
      next = iter;
      next++;
      //  nextFilt = *next;
      if(next != pipeline.end()) { (*iter)->setNextFilter(*next); }
    }
  }
  int index = 0;
  for (FilterContainerType::iterator filter = pipeline.begin(); filter != pipeline.end(); ++filter)
  {
    (*filter)->setPipelineIndex(index++);
  }





      writer->execute();    
      pipeline.push_back(writer);
    


      std::cout << "********* RUNNING PIPELINE **********************" << std::endl;
     // pipeline->run();
      pipeline.clear();
}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------





