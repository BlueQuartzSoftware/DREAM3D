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
  ${DREAM3DLib_SOURCE_DIR}/Common/AbstractFilter.h
  ${DREAM3DLib_SOURCE_DIR}/Common/AppVersion.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Constants.h
  ${DREAM3DLib_SOURCE_DIR}/Common/CreatedArrayHelpIndexEntry.h
  ${DREAM3DLib_SOURCE_DIR}/Common/CrystalStructure.h
  ${DREAM3DLib_SOURCE_DIR}/Common/DREAM3DDLLExport.h
  ${DREAM3DLib_SOURCE_DIR}/Common/DREAM3DSetGetMacros.h
  ${DREAM3DLib_SOURCE_DIR}/Common/FilterFactory.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/FilterManager.h
  ${DREAM3DLib_SOURCE_DIR}/Common/FilterPipeline.h
  ${DREAM3DLib_SOURCE_DIR}/Common/IDataArrayFilter.h
  ${DREAM3DLib_SOURCE_DIR}/Common/IFilterFactory.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/ModifiedLambertProjection.h
  ${DREAM3DLib_SOURCE_DIR}/Common/ModifiedLambertProjectionArray.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Observable.h
  ${DREAM3DLib_SOURCE_DIR}/Common/Observer.h
  ${DREAM3DLib_SOURCE_DIR}/Common/PhaseType.h
  ${DREAM3DLib_SOURCE_DIR}/Common/PipelineMessage.h
  ${DREAM3DLib_SOURCE_DIR}/Common/ScopedFileMonitor.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/ShapeType.h
  ${DREAM3DLib_SOURCE_DIR}/Common/StatsGen.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Texture.hpp
  ${DREAM3DLib_SOURCE_DIR}/Common/TexturePreset.h
  ${DREAM3DLib_SOURCE_DIR}/Common/ThresholdFilterHelper.h
)

set(DREAM3DLib_Common_SRCS
  ${DREAM3DLib_SOURCE_DIR}/Common/AbstractFilter.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/AppVersion.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/CreatedArrayHelpIndexEntry.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/FilterManager.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/FilterPipeline.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/IDataArrayFilter.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/ModifiedLambertProjection.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/ModifiedLambertProjectionArray.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Observable.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/Observer.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/PhaseType.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/ShapeType.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/TexturePreset.cpp
  ${DREAM3DLib_SOURCE_DIR}/Common/ThresholdFilterHelper.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/Common" "${DREAM3DLib_Common_HDRS}" "${DREAM3DLib_Common_SRCS}" "0")
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${DREAM3DLib_Common_HDRS}
            DESTINATION include/DREAM3D/Common
            COMPONENT Headers   )
endif()

if(MSVC)
  set_source_files_properties(${DREAM3DLib_SOURCE_DIR}/Common/FilterManager.cpp PROPERTIES COMPILE_FLAGS /bigobj)
  set_source_files_properties(${DREAM3DLib_SOURCE_DIR}/Common/FilterManager.h PROPERTIES COMPILE_FLAGS /bigobj)
endif()
