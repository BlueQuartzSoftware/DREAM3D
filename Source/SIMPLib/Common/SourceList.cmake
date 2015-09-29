#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2011, Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2011, Michael Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////

set(SUBDIR_NAME Common)

# --------------------------------------------------------------------
# Any Class that inherits from QObject, either directly or through the heirarchy needs to have its header listed here
set(SIMPLib_${SUBDIR_NAME}_Moc_HDRS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractFilter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ComparisonInputs.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DocRequestManager.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterPipeline.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Observer.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Observable.h
)

# --------------------------------------------------------------------
# Run Qts automoc program to generate some source files that get compiled
QT5_WRAP_CPP( SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS ${SIMPLib_${SUBDIR_NAME}_Moc_HDRS})
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

set(SIMPLib_${SUBDIR_NAME}_HDRS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AppVersion.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Constants.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/CreatedArrayHelpIndexEntry.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterFactory.hpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterManager.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/IDataArrayFilter.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/IFilterFactory.hpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/IObserver.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PhaseType.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PipelineMessage.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ScopedFileMonitor.hpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ShapeType.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLibDLLExport.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLibSetGetMacros.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/TemplateHelpers.hpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ThresholdFilterHelper.h
)


set(SIMPLib_${SUBDIR_NAME}_SRCS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AbstractFilter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/AppVersion.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/DocRequestManager.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ComparisonInputs.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/CreatedArrayHelpIndexEntry.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterManager.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterPipeline.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/IDataArrayFilter.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/IObserver.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Observable.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Observer.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/PhaseType.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ShapeType.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ThresholdFilterHelper.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "${SUBDIR_NAME}" "${SIMPLib_${SUBDIR_NAME}_HDRS};${SIMPLib_${SUBDIR_NAME}_Moc_HDRS}" "${SIMPLib_${SUBDIR_NAME}_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/SIMPLib/${SUBDIR_NAME}" "" "${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS}" "0")

set(SIMPLib_${SUBDIR_NAME}_HDRS
  ${SIMPLib_${SUBDIR_NAME}_HDRS}
  ${SIMPLib_${SUBDIR_NAME}_Moc_HDRS}  # Add the headers that get Moc'ed here so they show up in solutions/IDEs/Project files
)

set(SIMPLib_${SUBDIR_NAME}_SRCS
  ${SIMPLib_${SUBDIR_NAME}_SRCS}
  ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS}
)

# -- Add the binary directory for this subdirectory to the include path which is where the moc files are generated
include_directories( ${SIMPLib_BINARY_DIR}/${SUBDIR_NAME})


if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${SIMPLib_${SUBDIR_NAME}_HDRS}
                   ${SIMPLib_${SUBDIR_NAME}_Moc_HDRS}
            DESTINATION include/SIMPLib/${SUBDIR_NAME}
            COMPONENT Headers   )
endif()

if(MSVC)
  set_source_files_properties(${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterManager.cpp PROPERTIES COMPILE_FLAGS /bigobj)
  set_source_files_properties(${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilterManager.h PROPERTIES COMPILE_FLAGS /bigobj)
endif()
