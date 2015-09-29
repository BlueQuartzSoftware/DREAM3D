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

set(OrientationLib_Texture_HDRS
  ${OrientationLib_SOURCE_DIR}/Texture/TexturePreset.h
  ${OrientationLib_SOURCE_DIR}/Texture/Texture.hpp
  ${OrientationLib_SOURCE_DIR}/Texture/StatsGen.hpp
)

set(OrientationLib_Texture_SRCS
  ${OrientationLib_SOURCE_DIR}/Texture/TexturePreset.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "Common" "${OrientationLib_Texture_HDRS}" "${OrientationLib_Texture_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${OrientationLib_Texture_HDRS}
            DESTINATION include/OrientationLib/Texture
            COMPONENT Headers   )
endif()

set(OrientationLib_Texture_HDRS ${OrientationLib_Texture_HDRS} ${OrientationLib_Texture_IO_HDRS})
set(OrientationLib_Texture_SRCS ${OrientationLib_Texture_SRCS} ${OrientationLib_Texture_IO_SRCS})
