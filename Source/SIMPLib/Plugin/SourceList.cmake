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



set(SIMPLib_Plugin_HDRS
  ${SIMPLib_SOURCE_DIR}/Plugin/ISIMPLibPlugin.h
  ${SIMPLib_SOURCE_DIR}/Plugin/SIMPLibPlugin.h
  ${SIMPLib_SOURCE_DIR}/Plugin/SIMPLibPluginLoader.h
  ${SIMPLib_SOURCE_DIR}/Plugin/PluginManager.h
  ${SIMPLib_SOURCE_DIR}/Plugin/PluginProxy.h

)
set(SIMPLib_Plugin_SRCS
  ${SIMPLib_SOURCE_DIR}/Plugin/SIMPLibPluginLoader.cpp
  ${SIMPLib_SOURCE_DIR}/Plugin/PluginManager.cpp
  ${SIMPLib_SOURCE_DIR}/Plugin/PluginProxy.cpp
  ${SIMPLib_SOURCE_DIR}/Plugin/SIMPLibPlugin.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "SIMPLibLib/Plugin" "${SIMPLib_Plugin_HDRS}" "${SIMPLib_Plugin_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${SIMPLib_Plugin_HDRS}
            DESTINATION include/SIMPLib/Plugin
            COMPONENT Headers   )
endif()
