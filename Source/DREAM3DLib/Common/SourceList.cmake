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

set(DREAM3DLib_Common_HDRS
  ${DREAM3DLib_SOURCE_DIR}/Common/AppVersion.h
  ${DREAM3DLib_SOURCE_DIR}/Common/AbstractFilterParametersReader.h
  ${DREAM3DLib_SOURCE_DIR}/Common/AbstractFilterParametersWriter.h
  ${DREAM3DLib_SOURCE_DIR}/Common/AbstractFilter.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Constants.h
  ${DREAM3DLib_SOURCE_DIR}/Common/CrystalStructure.h
  ${DREAM3DLib_SOURCE_DIR}/Common/DataArray.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/GbcdDataArray.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/VoxelDataContainer.h
  ${DREAM3DLib_SOURCE_DIR}/Common/SurfaceMeshDataContainer.h
  ${DREAM3DLib_SOURCE_DIR}/Common/SolidMeshDataContainer.h
  ${DREAM3DLib_SOURCE_DIR}/Common/DataContainerMacros.h
  ${DREAM3DLib_SOURCE_DIR}/Common/DREAM3DDLLExport.h
  ${DREAM3DLib_SOURCE_DIR}/Common/DREAM3DMath.h
  ${DREAM3DLib_SOURCE_DIR}/Common/DREAM3DRandom.h
  ${DREAM3DLib_SOURCE_DIR}/Common/DREAM3DSetGetMacros.h
  ${DREAM3DLib_SOURCE_DIR}/Common/FilterParameter.h
  ${DREAM3DLib_SOURCE_DIR}/Common/FilterPipeline.h
  ${DREAM3DLib_SOURCE_DIR}/Common/FileReader.h
  ${DREAM3DLib_SOURCE_DIR}/Common/FileWriter.h
  ${DREAM3DLib_SOURCE_DIR}/Common/IDataArray.h
  ${DREAM3DLib_SOURCE_DIR}/Common/NeighborList.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Observable.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Observer.h
  ${DREAM3DLib_SOURCE_DIR}/Common/PhaseType.h
  ${DREAM3DLib_SOURCE_DIR}/Common/PipelineMessage.h
  ${DREAM3DLib_SOURCE_DIR}/Common/ShapeType.h
  ${DREAM3DLib_SOURCE_DIR}/Common/SolidMeshStructs.h
  ${DREAM3DLib_SOURCE_DIR}/Common/StatsDataArray.h
  ${DREAM3DLib_SOURCE_DIR}/Common/StatsData.h
  ${DREAM3DLib_SOURCE_DIR}/Common/SurfaceMeshStructs.h
  ${DREAM3DLib_SOURCE_DIR}/Common/StructArray.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/ManagedArrayOfArrays.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/PrimaryStatsData.h
  ${DREAM3DLib_SOURCE_DIR}/Common/PrecipitateStatsData.h
  ${DREAM3DLib_SOURCE_DIR}/Common/TransformationStatsData.h
  ${DREAM3DLib_SOURCE_DIR}/Common/MatrixStatsData.h
  ${DREAM3DLib_SOURCE_DIR}/Common/BoundaryStatsData.h
  ${DREAM3DLib_SOURCE_DIR}/Common/StatsGen.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/StringDataArray.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Texture.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/TexturePreset.h
  ${DREAM3DLib_SOURCE_DIR}/Common/TextFilterParametersWriter.h
  ${DREAM3DLib_SOURCE_DIR}/Common/ScopedFileMonitor.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/IDataArrayFilter.h
  ${DREAM3DLib_SOURCE_DIR}/Common/ThresholdFilterHelper.h
  ${DREAM3DLib_SOURCE_DIR}/Common/CreatedArrayHelpIndexEntry.h
  ${DREAM3DLib_SOURCE_DIR}/Common/ModifiedLambertProjection.h
  ${DREAM3DLib_SOURCE_DIR}/Common/ModifiedLambertProjectionArray.h
  ${DREAM3DLib_SOURCE_DIR}/Common/FilterManager.h
  ${DREAM3DLib_SOURCE_DIR}/Common/IFilterFactory.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/FilterFactory.hpp
)

set(DREAM3DLib_Common_SRCS
  ${DREAM3DLib_SOURCE_DIR}/Common/AppVersion.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/AbstractFilterParametersReader.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/AbstractFilterParametersWriter.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/AbstractFilter.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/VoxelDataContainer.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/SurfaceMeshDataContainer.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/SolidMeshDataContainer.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/DREAM3DRandom.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/DREAM3DMath.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/FilterPipeline.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/FileReader.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/FileWriter.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/IDataArray.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Observer.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Observable.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/PhaseType.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/ShapeType.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/StatsDataArray.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/StatsData.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/PrimaryStatsData.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/PrecipitateStatsData.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/TransformationStatsData.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/MatrixStatsData.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/BoundaryStatsData.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/TexturePreset.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/TextFilterParametersWriter.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/IDataArrayFilter.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/ThresholdFilterHelper.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/CreatedArrayHelpIndexEntry.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/ModifiedLambertProjection.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/ModifiedLambertProjectionArray.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/FilterManager.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/Common" "${DREAM3DLib_Common_HDRS}" "${DREAM3DLib_Common_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_Common_HDRS}
            DESTINATION include/DREAM3D/Common
            COMPONENT Headers   )
endif()
