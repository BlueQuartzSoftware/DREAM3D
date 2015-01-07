#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////
set (EMMPMLib_Core_SRCS
    ${EMMPMLib_SOURCE_DIR}/Core/EMCalculation.cpp
    ${EMMPMLib_SOURCE_DIR}/Core/EMMPM_Data.cpp
    ${EMMPMLib_SOURCE_DIR}/Core/EMMPM.cpp
    ${EMMPMLib_SOURCE_DIR}/Core/EMMPMUtilities.cpp
    ${EMMPMLib_SOURCE_DIR}/Core/InitializationFunctions.cpp
    ${EMMPMLib_SOURCE_DIR}/Core/MorphFilt.cpp
    ${EMMPMLib_SOURCE_DIR}/Core/MPMCalculation.cpp
)

set (EMMPMLib_Core_HDRS
    ${EMMPMLib_SOURCE_DIR}/Core/EMCalculation.h
    ${EMMPMLib_SOURCE_DIR}/Core/EMMPM_Constants.h
    ${EMMPMLib_SOURCE_DIR}/Core/EMMPM_Data.h
    ${EMMPMLib_SOURCE_DIR}/Core/EMMPM.h
    ${EMMPMLib_SOURCE_DIR}/Core/EMMPMUtilities.h
    ${EMMPMLib_SOURCE_DIR}/Core/InitializationFunctions.h
    ${EMMPMLib_SOURCE_DIR}/Core/MorphFilt.h
    ${EMMPMLib_SOURCE_DIR}/Core/MPMCalculation.h
)
cmp_IDE_SOURCE_PROPERTIES( "EMMPMLib/Core" "${EMMPM_Core_HDRS}" "${EMMPM_Core_SRCS}" "${PROJECT_INSTALL_HEADERS}")
