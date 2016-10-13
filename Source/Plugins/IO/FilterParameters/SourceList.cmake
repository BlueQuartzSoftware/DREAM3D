#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////
set (${PLUGIN_NAME}_FilterParameters_SRCS
	${${PLUGIN_NAME}_SOURCE_DIR}/FilterParameters/ReadASCIIDataFilterParameter.cpp
)

set (${PLUGIN_NAME}_FilterParameters_HDRS
	${${PLUGIN_NAME}_SOURCE_DIR}/FilterParameters/ReadASCIIDataFilterParameter.h
)
cmp_IDE_SOURCE_PROPERTIES( "FilterParameters" "${${PLUGIN_NAME}_FilterParameters_HDRS}" "${${PLUGIN_NAME}_FilterParameters_SRCS}" "${PROJECT_INSTALL_HEADERS}")
