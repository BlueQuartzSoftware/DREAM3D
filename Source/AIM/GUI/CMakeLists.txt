#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////
# set project's name
PROJECT( DREAM3DApp )
cmake_minimum_required(VERSION 2.8)


# ---------- Setup output Directories -------------------------
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/Bin
  CACHE PATH
  "Single Directory for all Libraries"
  )

# --------- Setup the Executable output Directory -------------
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/Bin
  CACHE PATH
  "Single Directory for all Executables."
  )

# --------- Setup the Executable output Directory -------------
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/Bin
  CACHE PATH
  "Single Directory for all static libraries."
  )

# Set the GUI type for different platforms
SET(GUI_TYPE )

# Set some Win32 Specific Settings
IF(WIN32)
 SET(GUI_TYPE WIN32)
ENDIF(WIN32)

add_definitions(-DAIM_USE_QT 
                -DSVG_3D_LIBRARY 
                -DSURFACE_MESH_LIBRARY
                -DSMOOTH_MESH_LIBRARY
                -DVOLUME_MESH_LIBRARY 
                -DUPDATE_EDGE_TRI_LIBRARY)

include_directories(${PROJECT_CODE_DIR})
include_directories(${HDF5_INCLUDE_DIR})
include_directories(${DREAM3DApp_SOURCE_DIR})
include_directories(${DREAM3DApp_BINARY_DIR})
include_directories(${DREAM3D_BINARY_DIR}/QtSupport)

# --------------------------------------------------------------------
# Generate the License File Headers
include(${DREAM3D_SOURCE_DIR}/License/License.cmake)

# List the source files here
SET (DREAM3DApp_SRCS
    ${DREAM3DApp_SOURCE_DIR}/main.cpp
    ${DREAM3DApp_SOURCE_DIR}/RepresentationUI.cpp
    ${DREAM3DApp_SOURCE_DIR}/AIMPluginFrame.cpp
    ${DREAM3DApp_SOURCE_DIR}/ReconstructionWidget.cpp
    ${DREAM3DApp_SOURCE_DIR}/OIMImportWidget.cpp
    ${DREAM3DApp_SOURCE_DIR}/GrainGeneratorWidget.cpp
    ${DREAM3DApp_SOURCE_DIR}/SurfaceMeshingWidget.cpp
    ${DREAM3DApp_SOURCE_DIR}/VolumeMeshingWidget.cpp
)

SET (DREAM3DApp_HDRS
    ${DREAM3DApp_SOURCE_DIR}/RepresentationUI.h
    ${DREAM3DApp_SOURCE_DIR}/AIMPluginFrame.h
    ${DREAM3DApp_SOURCE_DIR}/ReconstructionWidget.h
    ${DREAM3DApp_SOURCE_DIR}/OIMImportWidget.h
    ${DREAM3DApp_SOURCE_DIR}/GrainGeneratorWidget.h
    ${DREAM3DApp_SOURCE_DIR}/SurfaceMeshingWidget.h
    ${DREAM3DApp_SOURCE_DIR}/VolumeMeshingWidget.h
    )
cmp_IDE_SOURCE_PROPERTIES( "AIM/GUI" "${DREAM3DApp_HDRS}" "${DREAM3DApp_SRCS}" ${PROJECT_INSTALL_HEADERS})

SET (Reconstruction_SRCS
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Reconstruction/Reconstruction.cpp
)
SET (Reconstruction_HDRS
   ${DREAM3D_SOURCE_DIR}/Source/AIM/Reconstruction/Reconstruction.h
    )
cmp_IDE_SOURCE_PROPERTIES( "AIM/Reconstruction" "${Reconstruction_HDRS}" "${Reconstruction_SRCS}" ${PROJECT_INSTALL_HEADERS})

SET (GrainGenerator_SRCS
    ${DREAM3D_SOURCE_DIR}/Source/AIM/GrainGenerator/GrainGenerator.cpp
)
SET (GrainGenerator_HDRS
   ${DREAM3D_SOURCE_DIR}/Source/AIM/GrainGenerator/GrainGenerator.h
)
cmp_IDE_SOURCE_PROPERTIES( "AIM/GrainGenerator" "${GrainGenerator_HDRS}" "${GrainGenerator_SRCS}" ${PROJECT_INSTALL_HEADERS})

SET (SurfaceMesh_SRCS
    ${DREAM3D_SOURCE_DIR}/Source/AIM/SurfaceMesh/SurfaceMesh.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/SurfaceMesh/SMVtkFileIO.cpp
)
SET (SurfaceMesh_HDRS
   ${DREAM3D_SOURCE_DIR}/Source/AIM/SurfaceMesh/SurfaceMesh.h
   ${DREAM3D_SOURCE_DIR}/Source/AIM/SurfaceMesh/SMVtkFileIO.h
)
cmp_IDE_SOURCE_PROPERTIES( "AIM/SurfaceMesh" "${SurfaceMesh_HDRS}" "${SurfaceMesh_SRCS}" ${PROJECT_INSTALL_HEADERS})

SET (VolumeMesh_SRCS
    ${DREAM3D_SOURCE_DIR}/Source/AIM/VolumeMesh/VolumeMesh.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/VolumeMesh/MeshGenerator.cpp
)
SET (VolumeMesh_HDRS
   ${DREAM3D_SOURCE_DIR}/Source/AIM/VolumeMesh/VolumeMesh.h
   ${DREAM3D_SOURCE_DIR}/Source/AIM/VolumeMesh/MeshGenerator.h
)
cmp_IDE_SOURCE_PROPERTIES( "AIM/VolumeMesh" "${VolumeMesh_HDRS}" "${VolumeMesh_SRCS}" ${PROJECT_INSTALL_HEADERS})

SET (OIMImport_SRCS
    ${DREAM3D_SOURCE_DIR}/Source/AIM/OIMImport/OIMImport.cpp
)
SET (OIMImport_HDRS
   ${DREAM3D_SOURCE_DIR}/Source/AIM/OIMImport/OIMImport.h
)
cmp_IDE_SOURCE_PROPERTIES( "AIM/OIMImport" "${OIMImport_HDRS}" "${OIMImport_SRCS}" ${PROJECT_INSTALL_HEADERS})

                    
SET( DREAM3DApp_MOC_HDRS
    ${DREAM3DApp_SOURCE_DIR}/RepresentationUI.h
    ${DREAM3DApp_SOURCE_DIR}/AIMPluginFrame.h
    ${DREAM3DApp_SOURCE_DIR}/ReconstructionWidget.h
    ${DREAM3DApp_SOURCE_DIR}/OIMImportWidget.h
    ${DREAM3DApp_SOURCE_DIR}/GrainGeneratorWidget.h
    ${DREAM3DApp_SOURCE_DIR}/SurfaceMeshingWidget.h
    ${DREAM3DApp_SOURCE_DIR}/VolumeMeshingWidget.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Reconstruction/Reconstruction.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/GrainGenerator/GrainGenerator.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/SurfaceMesh/SurfaceMesh.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/VolumeMesh/VolumeMesh.h
    ${AIM_Threads_HDRS}
    ${OIMImport_HDRS}
)

# The UI files As generated by Qt Designer
SET( DREAM3DApp_UIS
    ${DREAM3DApp_SOURCE_DIR}/UI_Files/Test.ui
    ${DREAM3DApp_SOURCE_DIR}/UI_Files/RepresentationUI.ui
    ${DREAM3DApp_SOURCE_DIR}/UI_Files/ReconstructionWidget.ui
    ${DREAM3DApp_SOURCE_DIR}/UI_Files/OIMImportWidget.ui
    ${DREAM3DApp_SOURCE_DIR}/UI_Files/GrainGeneratorWidget.ui
    ${DREAM3DApp_SOURCE_DIR}/UI_Files/SurfaceMeshingWidget.ui
    ${DREAM3DApp_SOURCE_DIR}/UI_Files/VolumeMeshingWidget.ui
)
cmp_IDE_GENERATED_PROPERTIES("DREAM3DApp/UI_Files" "${DREAM3DApp_UIS}" "")

# and finally any resource files
SET( DREAM3DApp_RCS ${PROJECT_RESOURCES_DIR}/icons/images/Icons.qrc)

# -- Run MOC and UIC on the necessary files   
QT4_ADD_RESOURCES( Generated_RC_SRCS ${DREAM3DApp_RCS} ${QRC_LICENSE_FILES})

# this will run uic on .ui files:
QT4_WRAP_UI( Generated_UI_HDRS ${DREAM3DApp_UIS} )

# and finally this will run moc:
QT4_WRAP_CPP( Generated_MOC_SRCS ${DREAM3DApp_MOC_HDRS} )

#-- Put the generated files into their own group for IDEs
cmp_IDE_GENERATED_PROPERTIES("Generated/ui" "${Generated_UI_HDRS}" "")
cmp_IDE_GENERATED_PROPERTIES("Generated/moc" "${Generated_MOC_SRCS}" "")
cmp_IDE_GENERATED_PROPERTIES("Generated/qrc" "${QRC_LICENSE_FILES}" "${Generated_RC_SRCS}")

# -- Finally set the Project Sources to include all the sources
SET (DREAM3DApp_PROJECT_SRCS 
    ${DREAM3DApp_SRCS}
    ${DREAM3DApp_HDRS}
    ${AIM_COMMON_QT_HDRS}
    ${AIM_COMMON_QT_SRCS}
    ${AIM_Threads_SRCS}
    ${AIM_Threads_HDRS}
    ${Reconstruction_SRCS}
    ${Reconstruction_HDRS}
    ${GrainGenerator_SRCS}
    ${GrainGenerator_HDRS}
    ${SurfaceMesh_SRCS}
    ${SurfaceMesh_HDRS}
    ${VolumeMesh_SRCS}
    ${VolumeMesh_HDRS}
    ${Generated_RC_SRCS} 
    ${Generated_UI_HDRS} 
    ${Generated_MOC_SRCS}
    ${OIMImport_HDRS}
    ${OIMImport_SRCS}
)


IF(APPLE)
    set(APPLICATION_ICON ${PROJECT_RESOURCES_DIR}/icons/icns/DREAM3D.icns)
ENDIF (APPLE)

IF(Q_WS_WIN)
    set(APPLICATION_ICON ${PROJECT_RESOURCES_DIR}/icons/ico/DREAM3D.ico)
ENDIF(Q_WS_WIN)

                       
# file(READ ${PROJECT_BINARY_DIR}/Qt_Plugins.txt QT_PLUGINS)
BuildQtAppBundle(
    TARGET DREAM3D
    SOURCES ${${PROJECT_NAME}_PROJECT_SRCS}
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    ICON_FILE  ${APPLICATION_ICON}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    LINK_LIBRARIES ${QT_LIBRARIES} QtSupport MXADataModel AIMCommon
    LIB_SEARCH_DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
#    QT_PLUGINS    ${QT_PLUGINS}
#    OTHER_PLUGINS ${OTHER_PLUGINS}
    COMPONENT     Applications
    INSTALL_DEST  "."
)   
