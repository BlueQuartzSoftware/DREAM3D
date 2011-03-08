#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2011, Michael A. Jackson. BlueQuartz Software
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////

project(CMU)



set(CMU_SRCS
    ${CMU_SOURCE_DIR}/main.cpp
    ${CMU_SOURCE_DIR}/StatsGenerator.cpp
    ${PROJECT_CODE_DIR}/AIM/StatsGenerator/StatsGen.cpp
    ${PROJECT_CODE_DIR}/AIM/GrainGenerator/GrainGenerator.cpp
)

set(CMU_HDRS
    ${PROJECT_CODE_DIR}/AIM/StatsGenerator/StatsGen.h
    ${PROJECT_CODE_DIR}/AIM/GrainGenerator/GrainGenerator.h
    ${CMU_SOURCE_DIR}/StatsGenerator.h
)


add_executable(CMU ${CMU_SRCS})
target_link_libraries(CMU ${HDF5_LIBRARIES} MXADataModel AIMCommon)
