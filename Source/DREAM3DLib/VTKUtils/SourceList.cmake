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

# Any Class that inherits from QObject, either directly or through the heirarchy needs to have its header listed here
set(DREAM3DLib_VTKUtils_Moc_HDRS
  ${DREAM3DLib_SOURCE_DIR}/VTKUtils/VTKFileReader.h
)
# --------------------------------------------------------------------
# Run Qts automoc program to generate some source files that get compiled
#QT5_WRAP_CPP( DREAM3DLib_VTKUtils_Generated_MOC_SRCS ${DREAM3DLib_VTKUtils_Moc_HDRS})



set(DREAM3DLib_VTKUtils_HDRS
  ${DREAM3DLib_SOURCE_DIR}/VTKUtils/VTKWriterMacros.h
  ${DREAM3DLib_SOURCE_DIR}/VTKUtils/VTKUtil.hpp
)

set(DREAM3DLib_VTKUtils_SRCS
  ${DREAM3DLib_SOURCE_DIR}/VTKUtils/VTKFileReader.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/VTKUtils" "${DREAM3DLib_VTKUtils_HDRS}" "${DREAM3DLib_VTKUtils_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/DREAM3DLib/VTKUtils" "" "${DREAM3DLib_VTKUtils_Generated_MOC_SRCS}" "0")

set(DREAM3DLib_VTKUtils_SRCS
	${DREAM3DLib_VTKUtils_SRCS}
	${DREAM3DLib_VTKUtils_Generated_MOC_SRCS}
)

if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_VTKUtils_HDRS}
            DESTINATION include/DREAM3D/VTKUtils
            COMPONENT Headers   )
endif()
