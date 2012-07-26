#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////

FIND_PACKAGE(Doxygen)
if (NOT DOXYGEN_FOUND)
    message(WARNING "Doxygen is needed to build the documentation. Please install it correctly")
else()
  
# Add a target that builds just the High Level User Manuals/Tutorial type documentation
add_subdirectory(${PROJECT_SOURCE_DIR}/Documentation/Developer ${PROJECT_BINARY_DIR}/Documentation/Developer)
  
# Add a target that builds just the High Level User Manuals/Tutorial type documentation
add_subdirectory(${PROJECT_SOURCE_DIR}/Documentation/UserManual ${PROJECT_BINARY_DIR}/Documentation/UserManual)
  
# Generating the API Docs will also include the other "Developer Documentation"
#option(DREAM3D_BUILD_API_DOCS "Use Doxygen to generate the HTML API documentation" ON)
#if (DREAM3D_BUILD_API_DOCS)
  set (DOXY_OUTPUT_DIR ${PROJECT_BINARY_DIR}/Documentation/API)
  configure_file(${PROJECT_RESOURCES_DIR}/DREAM3D.doxyfile.in 
                 ${PROJECT_BINARY_DIR}/Doxyfile  @ONLY IMMEDIATE)
  add_custom_target(APIDocs COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile)
#endif()
  
endif()