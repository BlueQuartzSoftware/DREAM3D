#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2015, BlueQuartz Software
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////

set(DREAM3DTest_SOURCE_DIR ${DREAM3DProj_SOURCE_DIR}/Test)
set(DREAM3DTest_BINARY_DIR ${DREAM3DProj_BINARY_DIR}/Test)

#------------------------------------------------------------------------------
# Include this file from the CMP Project
include(${CMP_SOURCE_DIR}/cmpCMakeMacros.cmake)
include(${SIMPLProj_SOURCE_DIR}/Source/SIMPLib/SIMPLibMacros.cmake)

set(TEST_TEMP_DIR ${DREAM3DTest_BINARY_DIR}/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})


configure_file(${DREAM3DTest_SOURCE_DIR}/TestFileLocations.h.in
               ${DREAM3DTest_BINARY_DIR}/DREAM3DTestFileLocations.h @ONLY IMMEDIATE)


configure_file(${SIMPLProj_SOURCE_DIR}/Resources/UnitTestSupport.hpp
               ${DREAM3DTest_BINARY_DIR}/UnitTestSupport.hpp COPYONLY IMMEDIATE)


## QT5_WRAP_CPP( PreflightVerify_MOC  "${DREAM3DTest_SOURCE_DIR}/PreflightVerify.h")
#set_source_files_properties(${DREAM3DTest_SOURCE_DIR}/PreflightVerify.h PROPERTIES HEADER_FILE_ONLY TRUE)
set(PreFlightTest_SOURCES ${DREAM3DTest_SOURCE_DIR}/PreFlightTest.cpp
                          ${DREAM3DTest_SOURCE_DIR}/PreflightVerify.h
                          ${DREAM3DTest_SOURCE_DIR}/PreflightVerify.cpp
                          )
AddSIMPLUnitTest(TESTNAME PreFlightTest
                  SOURCES ${PreFlightTest_SOURCES}
                  FOLDER 
                    "DREAM3D UnitTests"
                  LINK_LIBRARIES 
                    Qt5::Core H5Support SIMPLib  
                  INCLUDE_DIRS
                    ${DREAM3DTest_BINARY_DIR}
                    ${SIMPLProj_SOURCE_DIR}/Source
                    ${SIMPLProj_BINARY_DIR}
                   )

#----------------------------------------------------------------------------
# Here we are trying to get something together that will run all the PrebuiltPipelines
# pipelines as a sanity check
# Reconstruction Pipelines First
set(PREBUILT_PIPELINES_DIR "PrebuiltPipelines/Workshop")
set(TEST_PIPELINE_LIST_FILE ${DREAM3DTest_BINARY_DIR}/PipelineRunnerTest.txt)
set(TEST_DATA_DIR ${DREAM3D_DATA_DIR})

set(PREBUILT_PIPELINE_NAMES
"PrebuiltPipelines/Workshop/EBSD Reconstruction/(01) SmallIN100 Archive"
"PrebuiltPipelines/Workshop/EBSD Reconstruction/(02) SmallIN100 Initial Visualization"
"PrebuiltPipelines/Workshop/EBSD Reconstruction/(03) SmallIN100 Alignment"
"PrebuiltPipelines/Workshop/EBSD Reconstruction/(04) SmallIN100 Presegmentation Processing"
"PrebuiltPipelines/Workshop/EBSD Reconstruction/(05) SmallIN100 Segmentation"
"PrebuiltPipelines/Workshop/EBSD Reconstruction/(06) SmallIN100 Postsegmentation Processing"
"PrebuiltPipelines/Workshop/EBSD Reconstruction/(07) SmallIN100 Final Processing"
"PrebuiltPipelines/Workshop/EBSD Reconstruction/(08) SmallIN100 Full Reconstruction"
"PrebuiltPipelines/Workshop/EBSD Statistics/(01) SmallIN100 Morphological Statistics"
"PrebuiltPipelines/Workshop/EBSD Statistics/(02) SmallIN100 Export Statistics"
"PrebuiltPipelines/Workshop/EBSD Statistics/(03) SmallIN100 Transfer Field Data"
"PrebuiltPipelines/Workshop/EBSD Statistics/(04) SmallIN100 Field Threshold"
"PrebuiltPipelines/Workshop/EBSD Statistics/(05) SmallIN100 Crystallographic Statistics"
"PrebuiltPipelines/Workshop/EBSD SurfaceMeshing/(01) SmallIN100 Quick Mesh"
"PrebuiltPipelines/Workshop/EBSD SurfaceMeshing/(02) SmallIN100 Smooth Mesh"
"PrebuiltPipelines/Workshop/EBSD SurfaceMeshing/(03) SmallIN100 Mesh Statistics"
"PrebuiltPipelines/Workshop/EBSD SurfaceMeshing/(04) SmallIN100 GBCD"
"PrebuiltPipelines/Workshop/Synthetic/(01) Single Cubic Phase Equiaxed"
"PrebuiltPipelines/Workshop/Synthetic/(02) Single Hexagonal Phase Equiaxed"
"PrebuiltPipelines/Workshop/Synthetic/(03) Single Cubic Phase Rolled"
"PrebuiltPipelines/Workshop/Synthetic/(04) Two Phase Cubic Hexagonal Particles Equiaxed"
"PrebuiltPipelines/Workshop/Synthetic/(05) Composite"
"PrebuiltPipelines/Workshop/Synthetic/(06) SmallIN100 Synthetic"

"PrebuiltPipelines/Examples/ASTMD638_specimen" 
"PrebuiltPipelines/Examples/CreateLambertSphereSurface" 
"PrebuiltPipelines/Examples/Edax_IPF_Colors"
"PrebuiltPipelines/Examples/INL_Export"     
"PrebuiltPipelines/Examples/Import_ASCI"       
"PrebuiltPipelines/Examples/StatsGenOdfData"       
"PrebuiltPipelines/Examples/TxCopper_Exposed"
"PrebuiltPipelines/Examples/CI_Histogram"      
"PrebuiltPipelines/Examples/Cylinder_Synthetic"   
"PrebuiltPipelines/Examples/ExtractVertexGeometry"     
"PrebuiltPipelines/Examples/Image_Histogram"       
"PrebuiltPipelines/Examples/Misc_Filters"    
"PrebuiltPipelines/Examples/Threshold2_MoveData"    
"PrebuiltPipelines/Examples/TxCopper_Unexposed"
)

if(SIMPL_USE_ITK)
  set(PREBUILT_PIPELINE_NAMES
    ${PREBUILT_PIPELINE_NAMES}
    "PrebuiltPipelines/Workshop/Image Reconstruction/(01) Image Initial Visualization"
    "PrebuiltPipelines/Workshop/Image Reconstruction/(02) Image Segmentation"
    )
endif()

FILE(WRITE ${TEST_PIPELINE_LIST_FILE} )


set(TEST_SCRIPT_FILE_EXT "sh")
set(EXE_EXT "")
if(WIN32)
  set(TEST_SCRIPT_FILE_EXT "bat")
  set(EXE_EXT ".exe")
endif()
set(PREBUILT_SUPPORT_DIR "${DREAM3D_SUPPORT_DIR}")
set(test_index  "0")
foreach(test ${PREBUILT_PIPELINE_NAMES} )
  math(EXPR test_index "${test_index} + 1")
  set(Example_PIPELINE_FILE "${DREAM3D_SUPPORT_DIR}/${test}.json")
  FILE(APPEND ${TEST_PIPELINE_LIST_FILE} "[${test_index}]    ${Example_PIPELINE_FILE}\n")
  
  #----------------
  set(SIMPL_CTEST_TEST_DRIVER "${DREAM3DTest_BINARY_DIR}/PrebuiltPipelineTests/${test}.${TEST_SCRIPT_FILE_EXT}")
  configure_file(${DREAM3DTest_SOURCE_DIR}/ctest_pipeline_driver.${TEST_SCRIPT_FILE_EXT} "${SIMPL_CTEST_TEST_DRIVER}" @ONLY)

  string(REPLACE "/" "_" test "${test}")
  
  add_test(NAME D3D_Prebuilt_${test_index} COMMAND "${SIMPL_CTEST_TEST_DRIVER}" )
endforeach()

# Some more Testing Only pipelines that can be put here
set(PREBUILT_PIPELINES_DIR "TestPipelines")
set(TEST_PIPELINE_LIST_FILE ${DREAM3DTest_BINARY_DIR}/PipelineRunnerTest.txt)
set(TEST_DATA_DIR ${DREAM3D_DATA_DIR})
set(PREBUILT_PIPELINE_NAMES
)

foreach(f ${PREBUILT_PIPELINE_NAMES} )
  set(DREAM3D_PIPELINE_FILE "${DREAM3D_SUPPORT_DIR}/${PREBUILT_PIPELINES_DIR}/${f}")
  FILE(APPEND ${TEST_PIPELINE_LIST_FILE} "${DREAM3D_PIPELINE_FILE}\n")
endforeach()


#------------------------------------------------------------------------------
# If Python is enabled, then enable the Python unit tests for this plugin
if(SIMPL_ENABLE_PYTHON)
    get_property(SIMPL_ANACONDA_OUTPUT_DIR GLOBAL PROPERTY SIMPL_ANACONDA_OUTPUT_DIR)
    get_property(SIMPL_PYTHON_TEST_SCRIPT GLOBAL PROPERTY SIMPL_PYTHON_TEST_SCRIPT)
    get_property(PYTHON_SITE_PACKAGES_NAME GLOBAL PROPERTY PYTHON_SITE_PACKAGES_NAME)
    set(PLUGIN_NAME "DREAM3D")

    #------------------------------
    # Initialize the PYTHON_TEST_INPUT_DIR variable to point to the "Testing Directory"
    file(TO_NATIVE_PATH "${DREAM3DTest_SOURCE_DIR}/Python" PYTHON_TEST_INPUT_DIR)
    #------------------------------
    # These names should match the names "EXACTLY" (including capitalization).
    # NO Spaces in the names (which means no spaces in the variable names)
    set(PLUGIN_PYTHON_TESTS
      Import_ASCII
      ColorData_Change_Test
      Confidence_Index_Histogram
      Create_Lambert_Sphere_Surface
      Edax_IPF_Colors
      Export_Small_IN100_ODF_Data
      Image_Histogram
      INL_Export
      Link_Feature_Attribute_Matrix_To_Element_Array
      Misc_Filters
      Threshold2_MoveData
      TxCopper_Exposed
      TxCopper_Unexposed

      EBSD/Reconstruction/01_SmallIN100_Archive
      EBSD/Reconstruction/02_SmallIN100_Initial_Visualization
      EBSD/Reconstruction/03_SmallIN100_Alignment
      EBSD/Reconstruction/04_SmallIN100_Presegmentation_Processing
      EBSD/Reconstruction/05_SmallIN100_Segmentation
      EBSD/Reconstruction/06_SmallIN100_Postsegmentation_Processing
      EBSD/Reconstruction/07_SmallIN100_Final_Processing
      EBSD/Reconstruction/08_SmallIN100_Full_Reconstruction
      EBSD/Statistics/01_SmallIN100_Morphological_Statistics
      EBSD/Statistics/02_SmallIN100_Export_Statistics
      EBSD/Statistics/03_SmallIN100_Transfer_Field_Data
      EBSD/Statistics/04_SmallIN100_Field_Threshold
      EBSD/Statistics/05_SmallIN100_Crystallographic_Statistics
      EBSD/SurfaceMesh/01_SmallIN100_Quick_Mesh
      EBSD/SurfaceMesh/02_SmallIN100_Smooth_Mesh
      EBSD/SurfaceMesh/03_SmallIN100_Mesh_Statistics
      EBSD/SurfaceMesh/04_SmallIN100_GBCD

      Image_Reconstruction/01_Image_Initial_Visualization
      Image_Reconstruction/02_Image_Segmentation

      Synthetic/01_Single_Cubic_Phase_Equiaxed
      Synthetic/02_Single_Hexagonal_Phase_Equiaxed
      Synthetic/03_Single_Cubic_Phase_Rolled
      Synthetic/04_Two_Phase_Cubic_Hexagonal_Particles_Equiaxed
      Synthetic/06_SmallN100_Synthetic

    )

    set(TEST_SCRIPT_FILE_EXT "sh")
    if(WIN32)
      set(TEST_SCRIPT_FILE_EXT "bat")
    endif()
    foreach(test ${PLUGIN_PYTHON_TESTS})
        set(SIMPL_PYTHON_TEST_DRIVER "${SIMPL_ANACONDA_OUTPUT_DIR}/UnitTest/${PLUGIN_NAME}/${test}.${TEST_SCRIPT_FILE_EXT}")
        configure_file(${SIMPL_PYTHON_TEST_SCRIPT} "${SIMPL_PYTHON_TEST_DRIVER}" @ONLY)

        string(REPLACE "/" "_" test "${test}")
        add_test(NAME PY_${PLUGIN_NAME}_${test} COMMAND "${SIMPL_PYTHON_TEST_DRIVER}" )
    endforeach(test ${PLUGIN_PYTHON_TESTS})

endif()




