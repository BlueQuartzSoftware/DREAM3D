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
PROJECT( Representation )
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

# List the source files here
SET (Representation_SRCS
    ${Representation_SOURCE_DIR}/main.cpp
    ${Representation_SOURCE_DIR}/RepresentationUI.cpp
)

SET (Representation_HDRS
    ${Representation_SOURCE_DIR}/RepresentationUI.h
)
cmp_IDE_SOURCE_PROPERTIES( "Source/AIM/GUI" "${Representation_HDRS}" "${Representation_SRCS}" ${PROJECT_INSTALL_HEADERS})


set (AIM_COMMON_QT_SRCS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Qt/AIMAboutBox.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Qt/QRecentFileList.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Qt/QR3DFileCompleter.cpp
)
set (AIM_COMMON_QT_HDRS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Qt/AIMAboutBox.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Qt/QRecentFileList.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Qt/QR3DFileCompleter.h     )
cmp_IDE_SOURCE_PROPERTIES("Source/AIM/Common/Qt" "${AIM_COMMON_QT_HDRS}" "${AIM_COMMON_QT_SRCS}" ${PROJECT_INSTALL_HEADERS})

set (AIM_Threads_SRCS 
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Threads/AIMThread.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Threads/AIMThreadGroup.cpp
)                       
set (AIM_Threads_HDRS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Threads/AIMThread.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Threads/AIMThreadGroup.h   )
cmp_IDE_SOURCE_PROPERTIES("Source/AIM/Threads" "${AIM_Threads_HDRS}" "${AIM_Threads_SRCS}" ${PROJECT_INSTALL_HEADERS})

set (ANG_IMPORTER_SRCS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/H5AngImporter.cpp
    )
set (ANG_IMPORTER_HDRS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/H5AngImporter.h
    )
cmp_IDE_SOURCE_PROPERTIES( "Source/AIM/AngImporter" "${ANG_IMPORTER_HDRS}" "${ANG_IMPORTER_SRCS}" ${PROJECT_INSTALL_HEADERS})


SET (Reconstruction_SRCS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Reconstruction/Reconstruction.cpp
)
SET (Reconstruction_HDRS
   ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Reconstruction/Reconstruction.h
    )
cmp_IDE_SOURCE_PROPERTIES( "Source/AIM/Reconstruction" "${Reconstruction_HDRS}" "${Reconstruction_SRCS}" ${PROJECT_INSTALL_HEADERS})

SET (GrainGenerator_SRCS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/GrainGenerator/GrainGenerator.cpp
)
SET (GrainGenerator_HDRS
   ${AIMRepresentation_SOURCE_DIR}/Source/AIM/GrainGenerator/GrainGenerator.h
)
cmp_IDE_SOURCE_PROPERTIES( "Source/AIM/GrainGenerator" "${GrainGenerator_HDRS}" "${GrainGenerator_SRCS}" ${PROJECT_INSTALL_HEADERS})

SET (SurfaceMesh_SRCS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/SurfaceMesh/SurfaceMesh.cpp
)
SET (SurfaceMesh_HDRS
   ${AIMRepresentation_SOURCE_DIR}/Source/AIM/SurfaceMesh/SurfaceMesh.h
)
cmp_IDE_SOURCE_PROPERTIES( "Source/AIM/SurfaceMesh" "${SurfaceMesh_HDRS}" "${SurfaceMesh_SRCS}" ${PROJECT_INSTALL_HEADERS})

SET (VolumeMesh_SRCS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/VolumeMesh/VolumeMesh.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/VolumeMesh/MeshGenerator.cpp
)
SET (VolumeMesh_HDRS
   ${AIMRepresentation_SOURCE_DIR}/Source/AIM/VolumeMesh/VolumeMesh.h
   ${AIMRepresentation_SOURCE_DIR}/Source/AIM/VolumeMesh/MeshGenerator.h
)
cmp_IDE_SOURCE_PROPERTIES( "Source/AIM/VolumeMesh" "${VolumeMesh_HDRS}" "${VolumeMesh_SRCS}" ${PROJECT_INSTALL_HEADERS})

                        
SET( Representation_MOC_HDRS
    ${Representation_SOURCE_DIR}/RepresentationUI.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Qt/AIMAboutBox.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Qt/QRecentFileList.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Qt/QR3DFileCompleter.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Reconstruction/Reconstruction.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/GrainGenerator/GrainGenerator.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/SurfaceMesh/SurfaceMesh.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/VolumeMesh/VolumeMesh.h
    ${AIM_Threads_HDRS}
    ${ANG_IMPORTER_HDRS}
)

# The UI files As generated by Qt Designer
SET( Representation_UIS
  ${Representation_SOURCE_DIR}/UI_Files/RepresentationUI.ui
  ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Qt/UI_Files/AIMAboutBox.ui
)

# and finally any resource files
SET( Representation_RCS ${PROJECT_RESOURCES_DIR}/icons/images/Icons.qrc)

# -- Run MOC and UIC on the necessary files   
QT4_ADD_RESOURCES( Generated_RC_SRCS ${Representation_RCS} )

# this will run uic on .ui files:
QT4_WRAP_UI( Generated_UI_HDRS ${Representation_UIS} )

# and finally this will run moc:
QT4_WRAP_CPP( Generated_MOC_SRCS ${Representation_MOC_HDRS} )

#-- Put the generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated" "${Generated_UI_HDRS}" "${Generated_MOC_SRCS};${Generated_RC_SRCS}" ${PROJECT_INSTALL_HEADERS})

# -- Finally set the Project Sources to include all the sources
SET (Representation_PROJECT_SRCS 
    ${Representation_SRCS}
    ${Representation_HDRS}
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
    ${ANG_IMPORTER_HDRS}
    ${ANG_IMPORTER_SRCS}
)



include_directories(${PROJECT_CODE_DIR})
include_directories(${HDF5_INCLUDE_DIR})
include_directories( ${Representation_BINARY_DIR} )

#-- Set up OS X Application Bundle properties
#-- This file has tools for OS X Bunlde creation and configuration
INCLUDE (${CMP_OSX_TOOLS_SOURCE_DIR}/OSX_BundleTools.cmake)
SET (EXE_DEBUG_EXTENSION "_debug")
IF(APPLE)
 SET(GUI_TYPE MACOSX_BUNDLE)
 ConfigureMacOSXBundlePlist(${PROJECT_NAME} ${EXE_DEBUG_EXTENSION} 
                            ${PROJECT_RESOURCES_DIR}/icons/icns/${PROJECT_NAME}.icns
                            ${AIMRepresentation_VERSION} )
ENDIF (APPLE)

IF(Q_WS_WIN)
    include_directories(${PROJECT_RESOURCES_DIR}/icons/ico)
    configure_file(${PROJECT_RESOURCES_DIR}/icons/ico/IconResource.rc.in 
                    ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}.rc)
    SET (${PROJECT_NAME}_PROJECT_SRCS ${${PROJECT_NAME}_PROJECT_SRCS} 
            ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}.rc) 
    cmp_IDE_SOURCE_PROPERTIES ("ConfiguredFiles" "${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}.rc" ""  ${PROJECT_INSTALL_HEADERS})
ENDIF(Q_WS_WIN)

# ---------- Build the Viewer Application  ----------------------
ADD_EXECUTABLE( ${PROJECT_NAME} ${GUI_TYPE} ${Representation_PROJECT_SRCS} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME}
                        MXADataModel
                        AIMCommon
                        ${QT_LIBRARIES}  )
message(STATUS "HDF5_BINARY_DIR: ${HDF5_BINARY_DIR}")
set(AppNeedsPlugins "0")
list(APPEND libSearchDirs 
    ${QT_BINARY_DIR}
    ${QT_LIBRARY_DIR}
    ${HDF5_BINARY_DIR}
    ${HDF5_LIBRARY_DIR}
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release )
cmp_InstallationSupport(${PROJECT_NAME} 
                        ${EXE_DEBUG_EXTENSION} 
                        ${Representation_BINARY_DIR} 
                        "${AppNeedsPlugins}" 
                        "${PROJECT_INSTALL_EXECUTABLES}"
                        "Applications"
                        "."
                        "${libSearchDirs}"
                        )






