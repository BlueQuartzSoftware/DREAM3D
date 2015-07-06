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

set(DREAM3DLib_HDF5_HDRS
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5BoundaryStatsDataDelegate.h
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5DataArrayReader.h
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5DataArrayWriter.hpp
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5Macros.h
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5MatrixStatsDataDelegate.h
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5PrecipitateStatsDataDelegate.h
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5PrimaryStatsDataDelegate.h
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5StatsDataDelegate.h
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5TransformationStatsDataDelegate.h
  ${DREAM3DLib_SOURCE_DIR}/HDF5/VTKH5Constants.h

)

set(DREAM3DLib_HDF5_SRCS
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5BoundaryStatsDataDelegate.cpp
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5DataArrayReader.cpp
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5MatrixStatsDataDelegate.cpp
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5PrecipitateStatsDataDelegate.cpp
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5PrimaryStatsDataDelegate.cpp
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5StatsDataDelegate.cpp
  ${DREAM3DLib_SOURCE_DIR}/HDF5/H5TransformationStatsDataDelegate.cpp

)

cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/HDF5" "${DREAM3DLib_HDF5_HDRS}" "${DREAM3DLib_HDF5_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_HDF5_HDRS}
            DESTINATION include/DREAM3D/HDF5
            COMPONENT Headers   )
endif()
