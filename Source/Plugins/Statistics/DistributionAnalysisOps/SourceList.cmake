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

set(DistributionAnalysisOps_HDRS
  ${Statistics_SOURCE_DIR}/DistributionAnalysisOps/BetaOps.h
  ${Statistics_SOURCE_DIR}/DistributionAnalysisOps/DistributionAnalysisOps.h
  ${Statistics_SOURCE_DIR}/DistributionAnalysisOps/LogNormalOps.h
  ${Statistics_SOURCE_DIR}/DistributionAnalysisOps/PowerLawOps.h
)

set(DistributionAnalysisOps_SRCS
  ${Statistics_SOURCE_DIR}/DistributionAnalysisOps/BetaOps.cpp
  ${Statistics_SOURCE_DIR}/DistributionAnalysisOps/DistributionAnalysisOps.cpp
  ${Statistics_SOURCE_DIR}/DistributionAnalysisOps/LogNormalOps.cpp
  ${Statistics_SOURCE_DIR}/DistributionAnalysisOps/PowerLawOps.cpp
  )
cmp_IDE_SOURCE_PROPERTIES( "${PLUGIN_NAME}/DistributionAnalysisOps/" "${DistributionAnalysisOps_HDRS}" "${DistributionAnalysisOps_SRCS}" "0")

if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_DistributionAnalysisOps_HDRS}
            DESTINATION include/Statistics/DistributionAnalysisOps
            COMPONENT Headers   )
endif()
