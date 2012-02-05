/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include <string>
#include <iostream>
#include <vector>

#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

#include <MXA/Common/LogTime.h>
#include <MXA/Utilities/MXALogger.h>
#include <MXA/Utilities/MXADir.h>

#include "DREAM3DLib/DREAM3DVersion.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/PhaseType.h"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/GenericFilters/DataContainerWriter.h"
#include "DREAM3DLib/GenericFilters/VtkRectilinearGridWriter.h"
#include "DREAM3DLib/ReconstructionFilters/LoadSlices.h"
#include "DREAM3DLib/ReconstructionFilters/AlignSections.h"
#include "DREAM3DLib/ReconstructionFilters/SegmentGrains.h"
#include "DREAM3DLib/ReconstructionFilters/CleanupGrains.h"
#include "DREAM3DLib/ReconstructionFilters/MergeTwins.h"
#include "DREAM3DLib/ReconstructionFilters/MergeColonies.h"

#define RECONSTRUCTION_MANUAL_DEBUG 0

#define CHECK_ARG(var, mandatory)\
  if (vm.count(#var) > 1) { mxa_log << logTime() << "Multiple Occurances for Parameter " << #var << std::endl; }\
  if (vm.count(#var) == 0 && mandatory == true) { \
  mxa_log << "Parameter --" << #var << " ==> Required. Program will Terminate." << std::endl; }\
  if (vm.count(#var) == 0 && mandatory == false) { \
  mxa_log << "--" << #var << " Using Default: '"  << var << "'" << std::endl; }\
  if (vm.count(#var) == 1 && mandatory == true) { \
  mxa_log << "--" << #var << " Value: '"  << var << "'" << std::endl; }

#define CHECK_BOOL_ARG(var)\
  mxa_log << "--" << #var << " is ";\
  if (var == true) { mxa_log << "TRUE"; } else { mxa_log << "FALSE"; }\
  mxa_log << "" << std::endl;

#define MAKE_OUTPUT_FILE_PATH(outpath, filename, outdir, prefix)\
    std::string outpath = outdir + MXADir::Separator + prefix + filename;

/**
 * @brief Parses unknown number of numeric values from a delimited string and places
 * the values into the output variable.
 * @param values The string to be parsed
 * @param format The stdio format specifier to use (%f for floats, %d for integers
 * @param output The output location to store the parsed values
 * @return Error condition
 */
template<typename T>
int parseUnknownArray(const std::string &values, const char* format, typename DataArray<T>::Pointer output)
{
  std::string::size_type pos = values.find(",", 0);
  size_t index = 0;
  T t;
  int n = sscanf(values.substr(0, pos).c_str(), format, &t);
  if(n != 1)
  {
    return -1;
  }
  size_t oldSize = output->GetSize();
  output->Resize(oldSize + 1);
  output->SetValue(oldSize, t);

  ++index;
  while (pos != std::string::npos && pos != values.size() - 1)
  {
    n = sscanf(values.substr(pos + 1).c_str(), format, &(t));
    oldSize = output->GetSize();
    output->Resize(oldSize + 1);
    output->SetValue(oldSize, t);
    pos = values.find(",", pos + 1);
    ++index;
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<QualityMetricFilter::Pointer> getQualityMetricFilters()
{
  std::vector<QualityMetricFilter::Pointer> filters;
  {
    QualityMetricFilter::Pointer filter = QualityMetricFilter::New();
    filter->setFieldName(Ebsd::Ang::ImageQuality);
    filter->setFieldValue(120);
    filter->setFieldOperator(">");
    filters.push_back(filter);
  }
  {
    QualityMetricFilter::Pointer filter = QualityMetricFilter::New();
    filter->setFieldName(Ebsd::Ang::ConfidenceIndex);
    filter->setFieldValue(0.1f);
    filter->setFieldOperator(">");
    filters.push_back(filter);
  }

  return filters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  std::cout << logTime() << "Starting Reconstruction ... " << std::endl;
  MXALOGGER_METHOD_VARIABLE_INSTANCE
  int err = EXIT_FAILURE;
  if(true)
  {
    std::cout << "Reconstruction Tool is NOT complete and is missing the ability to ingest filters. Please use the GUI version instead" << std::endl;
    return err;
  }

  try
  {
    // Handle program options passed on command line.
    TCLAP::CmdLine cmd("DREAM.3D Reconstruction", ' ', DREAM3DLib::Version::Complete);

    TCLAP::ValueArg<std::string> h5InputFile("i", "input", "HDF5 Input File", false, "", "HDF5 Input File");
    cmd.add(h5InputFile);

    TCLAP::ValueArg<int> zStartIndex("", "zStartIndex", "Starting Slice", false, 0, "Starting Slice");
    cmd.add(zStartIndex);
    TCLAP::ValueArg<int> zEndIndex("", "zEndIndex", "Ending Slice", false, 0, "Ending Slice");
    cmd.add(zEndIndex);

    TCLAP::ValueArg<int> AlignMeth("", "alignment", "Alignment Method [0] OuterBoundary [1] Misorientation [2] Mutual Information", false, 0, "Default=0");
    cmd.add(AlignMeth);

    TCLAP::ValueArg<int> MinAllowedGrainSize("", "minGrainSize", "What is the minimum allowed grain size", false, 50, "Default=50");
    cmd.add(MinAllowedGrainSize);
    TCLAP::ValueArg<double> MisOrientationTolerance("", "misorientationTolerance", "What is the misorientation tolerance (degrees)", false, 4.0, "Default=4.0");
    cmd.add(MisOrientationTolerance);

    TCLAP::SwitchArg mergeColonies("", "mergeColonies", "Do you want to merge colonies", false);
    cmd.add(mergeColonies);
    TCLAP::SwitchArg mergeTwins("", "mergeTwins", "Do you want to merge twins", false);
    cmd.add(mergeTwins);
    TCLAP::SwitchArg rectangularize("", "rectangularize", "Rectangularize Sample", false);
    cmd.add(rectangularize);

    TCLAP::ValueArg<int> CrystalStructure("", "crystruct", "Do you have a HCP (0) or FCC (1) material", false, 1, "Default=1");
    cmd.add(CrystalStructure);

    TCLAP::ValueArg<unsigned int> StackingOrder("", "stackingorder", "Stacking order of the slices: (0)Low to high, (1)High to Low", false, 0, "Default=0");
    cmd.add(StackingOrder);

    TCLAP::ValueArg<std::string> phaseTypeStr("", "phasetypes", "Comma separated list of phase type for each phase: (0)Primary (1)Precipitate (2)Transformation:", false, "0", "Phase Type");
    cmd.add(phaseTypeStr);

    TCLAP::ValueArg<double> DownSampleFactor("", "downSampleRes", "Down Sampling Resolution", false, 1.0, "Default=1.0");
    cmd.add(DownSampleFactor);

    TCLAP::ValueArg<std::string> outputDir("", "outputDir", "Where to write all the output files. If it does not exist it will be created.", true, ".", "/Path/To/Output");
    cmd.add(outputDir);
    TCLAP::ValueArg<std::string> outputPrefix("", "outputFilePrefix", "This is an optional file prefix for each of the output files", false, "Reconstruction_", "Default=Reconstruction_");
    cmd.add(outputPrefix);

    TCLAP::ValueArg<std::string> logfile("l", "logfile", "Name of the Log file to store any output into", false, "", "/Path/To/LogFile.log");
    cmd.add(logfile);

    // Parse the argv array.
    cmd.parse(argc, argv);
    if(argc == 1)
    {
      std::cout << "DREAM.3D Reconstruction program was not provided any arguments. Use the --help argument to show the help listing." << std::endl;
      return EXIT_FAILURE;
    }

#if 0
    Reconstruction::Pointer m_Reconstruction = Reconstruction::New();

    m_Reconstruction->setH5EbsdFile(h5InputFile.getValue());

    m_Reconstruction->setZStartIndex(zStartIndex.getValue());
    m_Reconstruction->setZEndIndex(zEndIndex.getValue() + 1);

    m_Reconstruction->setMergeColonies(mergeColonies.getValue() );
    m_Reconstruction->setMergeTwins(mergeTwins.getValue() );

    unsigned int alignmeth = static_cast<unsigned int>(AlignMeth.getValue() );

    m_Reconstruction->setAlignmentMethod(alignmeth);

    m_Reconstruction->setMinAllowedGrainSize(MinAllowedGrainSize.getValue());
    m_Reconstruction->setMisorientationTolerance(MisOrientationTolerance.getValue());

    m_Reconstruction->setOutputDirectory(OutputDir.getValue());
    m_Reconstruction->setOutputFilePrefix(OutputFilePrefix.getValue());
    m_Reconstruction->setWriteVtkFile(true);
    m_Reconstruction->setWritePhaseId(true);
    m_Reconstruction->setWriteIPFColor(true);
    m_Reconstruction->setWriteBinaryVTKFiles(true);

    m_Reconstruction->setWriteHDF5GrainFile(false);

    m_Reconstruction->run();
    err = m_Reconstruction->getErrorCondition();

#endif

    // Create a FilterPipeline Object to hold all the filters. Later on we will execute all the filters
    FilterPipeline::Pointer m_FilterPipeline = FilterPipeline::New();

    DataArray<unsigned int>::Pointer phaseTypes = DataArray<unsigned int>::CreateArray(1);
    phaseTypes->SetName(DREAM3D::EnsembleData::PhaseTypes);
    phaseTypes->SetValue(0, DREAM3D::PhaseType::UnknownPhaseType);
    if(parseUnknownArray<unsigned int>(phaseTypeStr.getValue(), "%d", phaseTypes) < 0)
    {
      std::cout << "Error parsing the Phase Types. The value should be entered as --phasetypes 1,0,1 for 3 phases." << std::endl;
      return EXIT_FAILURE;
    }

    typedef std::vector<AbstractFilter::Pointer> FilterContainerType;
    // Create a Vector to hold all the filters. Later on we will execute all the filters
    FilterContainerType pipeline;

    LoadSlices::Pointer load_slices = LoadSlices::New();
    load_slices->setH5EbsdFile(h5InputFile.getValue());
    load_slices->setZStartIndex(zStartIndex.getValue());
    load_slices->setZEndIndex(zEndIndex.getValue() + 1);
    load_slices->setPhaseTypes(phaseTypes);
    load_slices->setQualityMetricFilters(getQualityMetricFilters());
    load_slices->setRefFrameZDir(static_cast<Ebsd::RefFrameZDir>(StackingOrder.getValue()));
    load_slices->setMisorientationTolerance(MisOrientationTolerance.getValue());
    m_FilterPipeline->pushBack(load_slices);

    AlignSections::Pointer align_sections = AlignSections::New();
    align_sections->setMisorientationTolerance(MisOrientationTolerance.getValue());
    unsigned int alignmeth = static_cast<unsigned int>(AlignMeth.getValue());
    align_sections->setAlignmentMethod(alignmeth);
    m_FilterPipeline->pushBack(align_sections);

    SegmentGrains::Pointer segment_grains = SegmentGrains::New();
    segment_grains->setMisorientationTolerance(MisOrientationTolerance.getValue());
    m_FilterPipeline->pushBack(segment_grains);

    CleanupGrains::Pointer cleanup_grains = CleanupGrains::New();
    cleanup_grains->setMinAllowedGrainSize(MinAllowedGrainSize.getValue());
    cleanup_grains->setMisorientationTolerance(MisOrientationTolerance.getValue());
    m_FilterPipeline->pushBack(cleanup_grains);

    if(mergeTwins.getValue() == true)
    {
      MergeTwins::Pointer merge_twins = MergeTwins::New();
      m_FilterPipeline->pushBack(merge_twins);
    }

    if(mergeColonies.getValue() == true)
    {
      MergeColonies::Pointer merge_colonies = MergeColonies::New();
      m_FilterPipeline->pushBack(merge_colonies);
    }

    // Create a new HDF5 Volume file by overwriting any HDF5 file that may be in the way
    MAKE_OUTPUT_FILE_PATH(hdf5VolumeFile, DREAM3D::Reconstruction::H5VoxelFile, outputDir.getValue(), outputPrefix.getValue())
    DataContainerWriter::Pointer writer = DataContainerWriter::New();
    writer->setOutputFile(hdf5VolumeFile);
    m_FilterPipeline->pushBack(writer);

    //  if(m_VisualizationVizFile.getValue() == true)
    {
      MAKE_OUTPUT_FILE_PATH(reconVisFile, DREAM3D::Reconstruction::VisualizationVizFile, outputDir.getValue(), outputPrefix.getValue());
      VtkRectilinearGridWriter::Pointer vtkWriter = VtkRectilinearGridWriter::New();
      vtkWriter->setOutputFile(reconVisFile);
      vtkWriter->setWriteGrainIds(true);
      vtkWriter->setWritePhaseIds(true);
      vtkWriter->setWriteGoodVoxels(true);
      vtkWriter->setWriteIPFColors(true);
      vtkWriter->setWriteBinaryFile(true);
      m_FilterPipeline->pushBack(vtkWriter);
    }

    // Now preflight this pipeline to make sure we can actually run it
       int err = m_FilterPipeline->preflightPipeline();
       // If any error occured during the preflight exit now
       if(err < 0)
       {

         // Show a Dialog with the error from the Preflight
         return EXIT_FAILURE;
       }
       DataContainer::Pointer m = DataContainer::New();
       m_FilterPipeline->setDataContainer(m);

       m_FilterPipeline->run();
       err = m_FilterPipeline->getErrorCondition();
       if (err < 0)
       {
         std::cout << "Grain Generator threw an error during execution." << std::endl;
         return EXIT_FAILURE;
       }

  }
  catch (TCLAP::ArgException &e) // catch any exceptions
  {
    std::cerr << logTime() << " error: " << e.error() << " for arg " << e.argId() << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "Reconstruction Complete" << std::endl;
  return err;
}

