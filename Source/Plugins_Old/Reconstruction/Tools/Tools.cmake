  
# Create a Command line tool for the Reconstruction
# --------------------------------------------------------------------
# Setup the install rules for the various platforms
set(install_dir "tools")
if (WIN32)
    set (install_dir ".")
endif()
COMPILE_TOOL(
    TARGET Reconstruction
    SOURCES ${PROJECT_SOURCE_DIR}/Tools/ReconstructionMain.cpp 
            ${Reconstruction_HDRS} ${Reconstruction_SRCS}
            ${EbsdSupport_HDRS} ${EbsdSupport_SRCS} ${Algorithms_HDRS} ${Algorithms_SRCS}
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    COMPONENT     Applications
    INSTALL_DEST  "${install_dir}" 
    LINK_LIBRARIES EbsdLib DREAM3DLib
)
  

# Create a Command line tool for the H5EbsdToVtk
COMPILE_TOOL(
    TARGET H5EbsdToVtk
    SOURCES ${PROJECT_SOURCE_DIR}/Tools/H5EbsdToVtk.cpp 
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    COMPONENT     Applications
    INSTALL_DEST  "${install_dir}" 
    LINK_LIBRARIES DREAM3DLib
)
