#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////
set (EMMPMLib_Common_SRCS
    ${EMMPMLib_SOURCE_DIR}/Common/StatsDelegate.cpp
)

set (EMMPMLib_Common_HDRS
    ${EMMPMLib_SOURCE_DIR}/Common/EMMPM_Math.h
    ${EMMPMLib_SOURCE_DIR}/Common/EMMPMLibDLLExport.h
    ${EMMPMLib_SOURCE_DIR}/Common/StatsDelegate.h
    ${EMMPMLib_SOURCE_DIR}/Common/MSVCDefines.h
)


cmp_IDE_SOURCE_PROPERTIES( "EMMPMLib/Common" "${EMMPMLib_Common_HDRS}" "${EMMPMLib_Common_SRCS}" "${PROJECT_INSTALL_HEADERS}")
