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

set(SUBDIR_NAME VTKUtils)


# Any Class that inherits from QObject, either directly or through the heirarchy needs to have its header listed here
set(SIMPLib_VTKUtils_Moc_HDRS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/VTKFileReader.h
)
# --------------------------------------------------------------------
# Run Qts automoc program to generate some source files that get compiled
QT5_WRAP_CPP( SIMPLib_VTKUtils_Generated_MOC_SRCS ${SIMPLib_VTKUtils_Moc_HDRS})
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)



set(SIMPLib_VTKUtils_HDRS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/VTKWriterMacros.h
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/VTKUtil.hpp
)

set(SIMPLib_VTKUtils_SRCS
  ${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/VTKFileReader.cpp
)


cmp_IDE_SOURCE_PROPERTIES( "SIMPLib/${SUBDIR_NAME}" "${SIMPLib_${SUBDIR_NAME}_HDRS};${SIMPLib_${SUBDIR_NAME}_Moc_HDRS}" "${SIMPLib_${SUBDIR_NAME}_SRCS}" "0")
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
