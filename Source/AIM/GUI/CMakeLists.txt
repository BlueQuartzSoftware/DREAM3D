# set project's name
PROJECT( Representation )
cmake_minimum_required(VERSION 2.6)


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

add_definitions(-DAIM_USE_QT)

# Set the GUI type for different platforms
SET(GUI_TYPE )

# Set some Win32 Specific Settings
IF(WIN32)
 SET(GUI_TYPE WIN32)
ENDIF(WIN32)


# List the source files here
SET (Representation_SRCS
    ${Representation_SOURCE_DIR}/main.cpp
    ${Representation_SOURCE_DIR}/RepresentationUI.cpp
)

SET (Representation_HDRS
    ${Representation_SOURCE_DIR}/RepresentationUI.h
)
IDE_SOURCE_PROPERTIES( "Source/AIM/GUI" "${Representation_HDRS}" "${Representation_SRCS}")


set (AIM_COMMON_QT_SRCS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Qt/AIMAboutBox.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Qt/QRecentFilelist.cpp   )
set (AIM_COMMON_QT_HDRS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Qt/AIMAboutBox.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Qt/QRecentFilelist.h  )
IDE_SOURCE_PROPERTIES("Source/AIM/Common/Qt" "${AIM_COMMON_QT_HDRS}" "${AIM_COMMON_QT_SRCS}")

set (AIM_Threads_SRCS 
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Threads/AIMThread.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Threads/AIMThreadGroup.cpp
)                       
set (AIM_Threads_HDRS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Threads/AIMThread.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Threads/AIMThreadGroup.h   )
IDE_SOURCE_PROPERTIES("Source/AIM/Threads" "${AIM_Threads_HDRS}" "${AIM_Threads_SRCS}")


SET (COMMON_SRCS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/MicroGen3D.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Grain.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Voxel.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Bin.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Orient.cpp   
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/AIMRandomNG.cpp
    )

SET (COMMON_HDRS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/MicroGen3D.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Grain.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Voxel.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Bin.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Orient.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/AIMRandomNG.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/FeiConstants.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Constants.h
)
IDE_SOURCE_PROPERTIES( "Source/AIM/Common" "${COMMON_HDRS}" "${COMMON_SRCS}")


set (ANG_SRCS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngDirectoryPatterns.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngFileReader.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngFileHelper.cpp
 )
 
set (ANG_HDRS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngDirectoryPatterns.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngFileReader.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngFileHelper.h
)
IDE_SOURCE_PROPERTIES( "Source/AIM/ANG" "${ANG_HDRS}" "${ANG_SRCS}")

SET (Reconstruction_SRCS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Reconstruction/Reconstruction.cpp
)
SET (Reconstruction_HDRS
   ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Reconstruction/Reconstruction.h
    )
IDE_SOURCE_PROPERTIES( "Source/AIM/Reconstruction" "${Reconstruction_HDRS}" "${Reconstruction_SRCS}")

SET (GrainGenerator_SRCS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/GrainGenerator/GrainGenerator.cpp
)
SET (GrainGenerator_HDRS
   ${AIMRepresentation_SOURCE_DIR}/Source/AIM/GrainGenerator/GrainGenerator.h
    )
IDE_SOURCE_PROPERTIES( "Source/AIM/GrainGenerator" "${GrainGenerator_HDRS}" "${GrainGenerator_SRCS}")

set_source_files_properties(${AIMRepresentation_SOURCE_DIR}/Source/AIM/Reconstruction/Reconstruction.h
                            ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Reconstruction/Reconstruction.cpp
                            ${AIMRepresentation_SOURCE_DIR}/Source/AIM/GrainGenerator/GrainGenerator.h
                            ${AIMRepresentation_SOURCE_DIR}/Source/AIM/GrainGenerator/GrainGenerator.h
                    PROPERTIES  DEFINITIONS "-DAIM_USE_QT" )

SET( Representation_MOC_HDRS
    ${Representation_SOURCE_DIR}/RepresentationUI.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Qt/AIMAboutBox.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Qt/QRecentFilelist.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Reconstruction/Reconstruction.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/GrainGenerator/GrainGenerator.h
    ${AIM_Threads_HDRS}
)

# The UI files As generated by Qt Designer
SET( Representation_UIS
  ${Representation_SOURCE_DIR}/UI_Files/RepresentationUI.ui
  ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Qt/UI_Files/AIMAboutBox.ui
)

# and finally any resource files
SET( Representation_RCS ${AIMRepresentation_RESOURCES_DIR}/icons/images/Icons.qrc)

# -- Run MOC and UIC on the necessary files   
QT4_ADD_RESOURCES( Generated_RC_SRCS ${Representation_RCS} )

# this will run uic on .ui files:
QT4_WRAP_UI( Generated_UI_HDRS ${Representation_UIS} )

# and finally this will run moc:
QT4_WRAP_CPP( Generated_MOC_SRCS ${Representation_MOC_HDRS} )

#-- Put the generated files into their own group for IDEs
IDE_SOURCE_PROPERTIES( "Generated" "${Generated_UI_HDRS}" "${Generated_MOC_SRCS};${Generated_RC_SRCS}")

# -- Finally set the Project Sources to include all the sources
SET (Representation_PROJECT_SRCS 
    ${Representation_SRCS}
    ${Representation_HDRS}
    ${AIM_COMMON_QT_HDRS}
    ${AIM_COMMON_QT_SRCS}
    ${AIM_Threads_SRCS}
    ${AIM_Threads_HDRS}
    ${COMMON_SRCS}
    ${COMMON_HDRS}
    ${ANG_SRCS}
    ${ANG_HDRS}
    ${Reconstruction_SRCS}
    ${Reconstruction_HDRS}
    ${GrainGenerator_SRCS}
    ${GrainGenerator_HDRS}
    ${Generated_RC_SRCS} 
    ${Generated_UI_HDRS} 
    ${Generated_MOC_SRCS}
)


include_directories( ${Representation_BINARY_DIR} )
#-- Set up OS X Application Bundle properties
#-- This file have tools for OS X Bunlde creation and configuration
INCLUDE (${AIMRepresentation_SOURCE_DIR}/Resources/OSX_Tools/OSX_BundleTools.cmake)
SET (EXE_DEBUG_EXTENSION "_debug")
IF(APPLE)
 SET(GUI_TYPE MACOSX_BUNDLE)
 ConfigureMacOSXBundlePlist(${PROJECT_NAME} ${EXE_DEBUG_EXTENSION} 
                            ${AIMRepresentation_RESOURCES_DIR}/icons/icns/${PROJECT_NAME}.icns
                            ${AIMRepresentation_VERSION} )
ENDIF (APPLE)

IF(Q_WS_WIN)
    include_directories(${AIMRepresentation_RESOURCES_DIR}/icons/ico)
    configure_file(${AIMRepresentation_RESOURCES_DIR}/icons/ico/IconResource.rc.in ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}.rc)
    SET (${PROJECT_NAME}_PROJECT_SRCS ${${PROJECT_NAME}_PROJECT_SRCS} ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}.rc) 
    IDE_SOURCE_PROPERTIES ("ConfiguredFiles" "${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}.rc" "")
ENDIF(Q_WS_WIN)

#IDE_SOURCE_PROPERTIES ("ConfiguredFiles" "${AIMRepresentation_BINARY_DIR}/MXA/MXAConfiguration.h" "")
IDE_SOURCE_PROPERTIES ("ConfiguredFiles" "${AIMRepresentation_BINARY_DIR}/AIM/Common/AIMVersion.h" "")  

# ---------- Build the Viewer Application  ----------------------
ADD_EXECUTABLE( ${PROJECT_NAME} ${GUI_TYPE} ${Representation_PROJECT_SRCS} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME}
                        MXA 
                        ${QT_LIBRARIES}  )

InstallationSupport(${PROJECT_NAME} ${EXE_DEBUG_EXTENSION} ${AIMRepresentation_BINARY_DIR})






