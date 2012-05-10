  
# Create a Command line tool for the Reconstruction
# --------------------------------------------------------------------
# Setup the install rules for the various platforms
set(install_dir "tools")
if (WIN32)
    set (install_dir ".")
endif()


# Create a Command line tool for the VtkToHDF5
COMPILE_TOOL(
    TARGET VtkGrainIdToH5Voxel
    SOURCES ${PROJECT_SOURCE_DIR}/Tools/VtkGrainIdToH5Voxel.cpp ${PROJECT_SOURCE_DIR}/VtkGrainIdReader.cpp
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    COMPONENT     Applications
    INSTALL_DEST  "${install_dir}" 
    LINK_LIBRARIES DREAM3DLib
)
