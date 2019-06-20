#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////
set (${PLUGIN_NAME}_FilterParameters_SRCS
    ${${PLUGIN_NAME}_SOURCE_DIR}/FilterParameters/ConvertHexGridToSquareGridFilterParameter.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/FilterParameters/EbsdToH5EbsdFilterParameter.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/FilterParameters/EnsembleInfoFilterParameter.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/FilterParameters/OEMEbsdScanSelectionFilterParameter.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/FilterParameters/ReadH5EbsdFilterParameter.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/FilterParameters/OrientationUtilityFilterParameter.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/FilterParameters/EbsdMontageImportFilterParameter.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/FilterParameters/EbsdMontageListInfo.cpp
)

set (${PLUGIN_NAME}_FilterParameters_HDRS
    ${${PLUGIN_NAME}_SOURCE_DIR}/FilterParameters/ConvertHexGridToSquareGridFilterParameter.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/FilterParameters/EbsdToH5EbsdFilterParameter.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/FilterParameters/EnsembleInfoFilterParameter.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/FilterParameters/OEMEbsdScanSelectionFilterParameter.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/FilterParameters/ReadH5EbsdFilterParameter.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/FilterParameters/OrientationUtilityFilterParameter.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/FilterParameters/EbsdMontageImportFilterParameter.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/FilterParameters/EbsdMontageListInfo.h
)
cmp_IDE_SOURCE_PROPERTIES( "FilterParameters" "${${PLUGIN_NAME}_FilterParameters_HDRS}" "${${PLUGIN_NAME}_FilterParameters_SRCS}" "${PROJECT_INSTALL_HEADERS}")
