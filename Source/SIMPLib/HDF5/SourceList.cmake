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

set(SIMPLib_HDF5_HDRS
  ${SIMPLib_SOURCE_DIR}/HDF5/H5BoundaryStatsDataDelegate.h
  ${SIMPLib_SOURCE_DIR}/HDF5/H5DataArrayReader.h
  ${SIMPLib_SOURCE_DIR}/HDF5/H5DataArrayWriter.hpp
  ${SIMPLib_SOURCE_DIR}/HDF5/H5Macros.h
  ${SIMPLib_SOURCE_DIR}/HDF5/H5MatrixStatsDataDelegate.h
  ${SIMPLib_SOURCE_DIR}/HDF5/H5PrecipitateStatsDataDelegate.h
  ${SIMPLib_SOURCE_DIR}/HDF5/H5PrimaryStatsDataDelegate.h
  ${SIMPLib_SOURCE_DIR}/HDF5/H5StatsDataDelegate.h
  ${SIMPLib_SOURCE_DIR}/HDF5/H5TransformationStatsDataDelegate.h
  ${SIMPLib_SOURCE_DIR}/HDF5/VTKH5Constants.h

)

set(SIMPLib_HDF5_SRCS
  ${SIMPLib_SOURCE_DIR}/HDF5/H5BoundaryStatsDataDelegate.cpp
  ${SIMPLib_SOURCE_DIR}/HDF5/H5DataArrayReader.cpp
  ${SIMPLib_SOURCE_DIR}/HDF5/H5MatrixStatsDataDelegate.cpp
  ${SIMPLib_SOURCE_DIR}/HDF5/H5PrecipitateStatsDataDelegate.cpp
  ${SIMPLib_SOURCE_DIR}/HDF5/H5PrimaryStatsDataDelegate.cpp
  ${SIMPLib_SOURCE_DIR}/HDF5/H5StatsDataDelegate.cpp
  ${SIMPLib_SOURCE_DIR}/HDF5/H5TransformationStatsDataDelegate.cpp

)

cmp_IDE_SOURCE_PROPERTIES( "SIMPLib/HDF5" "${SIMPLib_HDF5_HDRS}" "${SIMPLib_HDF5_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${SIMPLib_HDF5_HDRS}
            DESTINATION include/DREAM3D/HDF5
            COMPONENT Headers   )
endif()
