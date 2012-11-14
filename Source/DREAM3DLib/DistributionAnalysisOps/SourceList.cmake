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

set(DREAM3DLib_DistributionAnalysisOps_HDRS
  ${DREAM3DLib_SOURCE_DIR}/DistributionAnalysisOps/BetaOps.h
  ${DREAM3DLib_SOURCE_DIR}/DistributionAnalysisOps/DistributionAnalysisOps.h
  ${DREAM3DLib_SOURCE_DIR}/DistributionAnalysisOps/LogNormalOps.h
  ${DREAM3DLib_SOURCE_DIR}/DistributionAnalysisOps/PowerLawOps.h
)

set(DREAM3DLib_DistributionAnalysisOps_SRCS
  ${DREAM3DLib_SOURCE_DIR}/DistributionAnalysisOps/BetaOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/DistributionAnalysisOps/DistributionAnalysisOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/DistributionAnalysisOps/LogNormalOps.cpp
  ${DREAM3DLib_SOURCE_DIR}/DistributionAnalysisOps/PowerLawOps.cpp
  )
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/DistributionAnalysisOps" "${DREAM3DLib_DistributionAnalysisOps_HDRS}" "${DREAM3DLib_DistributionAnalysisOps_SRCS}" "0")
if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_DistributionAnalysisOps_HDRS}
            DESTINATION include/DREAM3D/DistributionAnalysisOps
            COMPONENT Headers   )
endif()
