/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 
 
#include <stdlib.h>
#include <stdio.h>

#include "MXA/Utilities/MXADir.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/IOFilters/DataContainerWriter.h"
#include "DREAM3DLib/Common/H5FilterParametersReader.h"

#include "H5Support/H5Utilities.h"

#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"
#include "TestFilter.h"


#define StlFilePrefixTestValue "StlFilePrefixTestValue"
#define MaxIterationsTestValue 20
#define MisorientationToleranceTestValue 43.2
#define InputFileTestValue "InputFileTestValue.html"
#define InputPathTestValue "InputPathTestValue.html"
#define OutputFileTestValue "OutputFileTestValue.html"
#define OutputPathTestValue "OutputPathTestValue.html"
#define WriteAlignmentShiftsTestValue true
#define ConversionTypeTestValue 10
#define SelectedCellArrayNameTestValue "SelectedCellArrayNameTestValue"
#define SelectedFieldArrayNameTestValue "SelectedFieldArrayNameTestValue"
#define SelectedEnsembleArrayNameTestValue "SelectedEnsembleArrayNameTestValue"
#define SurfaceMeshPointArrayNameTestValue "SurfaceMeshPointArrayNameTestValue"
#define SurfaceMeshFaceArrayNameTestValue "SurfaceMeshFaceArrayNameTestValue"
#define SurfaceMeshEdgeArrayNameTestValue "SurfaceMeshEdgeArrayNameTestValue"
#define SolidMeshPointArrayNameTestValue "SolidMeshPointArrayNameTestValue"
#define SolidMeshFaceArrayNameTestValue "SolidMeshFaceArrayNameTestValue"
#define SolidMeshEdgeArrayNameTestValue "SolidMeshEdgeArrayNameTestValue"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  MXADir::remove(UnitTest::FilterParametersReaderTest::OutputFile);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParametersReaderTest()
{
	// Create our Pipeline object
	FilterPipeline::Pointer pipeline = FilterPipeline::New();


	TestFilter::Pointer filt = TestFilter::New();

	// Set something for each and every property so you have something to compare against.
	// You may want to make some constants for these values
	filt->setStlFilePrefix(StlFilePrefixTestValue);
	filt->setMaxIterations(MaxIterationsTestValue);
	filt->setMisorientationTolerance(MisorientationToleranceTestValue);
	filt->setInputFile(InputFileTestValue);
	filt->setInputPath(InputPathTestValue);
	filt->setOutputFile(OutputFileTestValue);
	filt->setOutputPath(OutputPathTestValue);
	filt->setWriteAlignmentShifts(WriteAlignmentShiftsTestValue);
	filt->setConversionType(ConversionTypeTestValue);
	filt->setSelectedCellArrayName(SelectedCellArrayNameTestValue);
	filt->setSelectedFieldArrayName(SelectedFieldArrayNameTestValue);
	filt->setSelectedEnsembleArrayName(SelectedEnsembleArrayNameTestValue);
	filt->setSurfaceMeshPointArrayName(SurfaceMeshPointArrayNameTestValue);
	filt->setSurfaceMeshFaceArrayName(SurfaceMeshFaceArrayNameTestValue);
	filt->setSurfaceMeshEdgeArrayName(SurfaceMeshEdgeArrayNameTestValue);
	filt->setSolidMeshPointArrayName(SolidMeshPointArrayNameTestValue);
	filt->setSolidMeshFaceArrayName(SolidMeshFaceArrayNameTestValue);
	filt->setSolidMeshEdgeArrayName(SolidMeshEdgeArrayNameTestValue);

	DataContainerWriter::Pointer writer = DataContainerWriter::New();
	writer->setOutputFile(UnitTest::FilterParametersReaderTest::OutputFile);

	pipeline->pushBack(filt);
	pipeline->pushBack(writer);

	pipeline->execute();
	int err = pipeline->getErrorCondition();
	DREAM3D_REQUIRED(err, >= , 0)


	// We are done writing a file, now we need to read the file using raw HDF5 codes

	hid_t fid = H5Utilities::openFile(UnitTest::FilterParametersReaderTest::OutputFile);
	DREAM3D_REQUIRED(fid, >, 0)

	H5FilterParametersReader::Pointer reader = H5FilterParametersReader::New();

	hid_t pipelineGroupId = H5Gopen(fid, DREAM3D::HDF5::PipelineGroupName.c_str(), H5P_DEFAULT);
	reader->setGroupId(pipelineGroupId);

	err = reader->openOptionsGroup( filt.get() ); // Open the HDF5 Group for this filter
	DREAM3D_REQUIRED(err, >=, 0)

	// This next line should read all the filter parameters into the filter.
	filt->readFilterParameters( reader.get() );

	// Now one by one, compare each of the filter parameters that you have stored in some constant somewhere to the values that are now in the filt variable.
	// Use DREAM3D_REQUIRED() to make sure each one is what you think it is.


	err = reader->closeOptionsGroup(); // Close the HDF5 group for this filter
	DREAM3D_REQUIRED(err, >=, 0)


	H5Gclose(pipelineGroupId); // Closes the "Pipeline" group
	H5Fclose(fid); // Closes the file
}

// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( FilterParametersReaderTest() )


  PRINT_TEST_SUMMARY();
  return err;
}