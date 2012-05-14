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

SET (SamplingPlugin_Sampling_HDRS
    ${SamplingPlugin_SOURCE_DIR}/Code/Sampling/ChangeResolution.h
    ${SamplingPlugin_SOURCE_DIR}/Code/Sampling/CropVolume.h
)

SET (SamplingPlugin_Sampling_SRCS
    ${SamplingPlugin_SOURCE_DIR}/Code/Sampling/ChangeResolution.cpp
    ${SamplingPlugin_SOURCE_DIR}/Code/Sampling/CropVolume.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "Sampling" "${SamplingPlugin_Sampling_HDRS}" "${SamplingPlugin_Sampling_SRCS}" "0")

