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



set(DREAM3DLib_Plugin_HDRS
  ${DREAM3DLib_SOURCE_DIR}/Plugin/DREAM3DPluginInterface.h

)
set(DREAM3DLib_Plugin_SRCS
  
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/Plugin" "${DREAM3DLib_Plugin_HDRS}" "${DREAM3DLib_Plugin_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_Plugin_HDRS}
            DESTINATION include/DREAM3D/Plugin
            COMPONENT Headers   )
endif()
