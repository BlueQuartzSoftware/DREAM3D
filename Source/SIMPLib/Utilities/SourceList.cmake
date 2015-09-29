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

set(SUBDIR_NAME Utilities)

# --------------------------------------------------------------------
# Any Class that inherits from QObject, either directly or through the hierarchy needs to have its header listed here
set(SIMPLib_${SUBDIR_NAME}_Moc_HDRS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/TestObserver.h
)

# --------------------------------------------------------------------
# Run Qts automoc program to generate some source files that get compiled
QT5_WRAP_CPP( SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS ${SIMPLib_${SUBDIR_NAME}_Moc_HDRS})
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)


set(SIMPLib_Utilities_HDRS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ColorTable.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ColorUtilities.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/TimeUtilities.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLibEndian.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLibRandom.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilePathGenerator.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/QMetaObjectUtilities.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/UnitTestSupport.hpp
)

set(SIMPLib_Utilities_SRCS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ColorTable.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/ColorUtilities.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/SIMPLibRandom.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/FilePathGenerator.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/QMetaObjectUtilities.cpp
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/TestObserver.cpp
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
