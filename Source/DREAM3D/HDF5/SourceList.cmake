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

SET (DREAM3DLib_HDF5_HDRS
  ${DREAM3DLib_SOURCE_DIR}/HDF5/AIM_H5VtkDataWriter.h
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5GrainWriter.hpp
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5StatsReader.h
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5StatsWriter.h
  ${DREAM3DLib_SOURCE_DIR}/HDF5/VTKH5Constants.h
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5VoxelWriter.h
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5VoxelReader.h
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5Macros.h
)

SET (DREAM3DLib_HDF5_SRCS
  ${DREAM3DLib_SOURCE_DIR}/HDF5/AIM_H5VtkDataWriter.cpp
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5StatsReader.cpp
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5StatsWriter.cpp
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5VoxelWriter.cpp
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5VoxelReader.cpp
  )
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/HDF5" "${DREAM3DLib_HDF5_HDRS}" "${DREAM3DLib_HDF5_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_HDF5_HDRS}
            DESTINATION include/DREAM3D/HDF5
            COMPONENT Headers   )
endif()
