#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////

FIND_PACKAGE(Doxygen)
if(NOT DOXYGEN_FOUND)
    message(WARNING "Doxygen is needed to build the documentation. Please install it correctly")
else()

# Add a target that builds just the High Level User Manuals/Tutorial type documentation
add_subdirectory(${PROJECT_SOURCE_DIR}/Documentation/Developer ${PROJECT_BINARY_DIR}/Documentation/Developer)


endif()
